#include "Renderer.h"

RendererGL* RendererGL::Renderer = nullptr;
RendererGL::RendererGL() : 
   Window( nullptr ), PositionMode( false ), VelocityMode( false ), MoveType( NONE ),
   TotalPositionCurvePointNum( 201 ), TotalVelocityCurvePointNum( 101 ), TotalAnimationFrameNum( 301 )
{
   Renderer = this;

   initialize();
   printOpenGLInformation();
}

RendererGL::~RendererGL()
{
   glfwTerminate();
}

void RendererGL::printOpenGLInformation() const
{
   cout << "****************************************************************" << endl;
   cout << " - GLFW version supported: " << glfwGetVersionString() << endl;
   cout << " - GLEW version supported: " << glewGetString( GLEW_VERSION ) << endl;
   cout << " - OpenGL renderer: " << glGetString( GL_RENDERER ) << endl;
   cout << " - OpenGL version supported: " << glGetString( GL_VERSION ) << endl;
   cout << " - OpenGL shader version supported: " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << endl  ;
   cout << "****************************************************************" << endl << endl;
}

void RendererGL::initialize()
{
   if (!glfwInit()) {
      cout << "Cannot Initialize OpenGL..." << endl;
      return;
   }
   glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
   glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
   glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );
   glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

   const int width = 1920;
   const int height = 1080;
   Window = glfwCreateWindow( width, height, "Bezier Curve", nullptr, nullptr );
   glfwMakeContextCurrent( Window );
   glewInit();
   
   registerCallbacks();
   
   glEnable( GL_DEPTH_TEST );
   glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

   MainCamera.updateWindowSize( width, height );
   ObjectShader.setShader(
      "Shaders/VertexShaderForObject.glsl",
      "Shaders/FragmentShaderForObject.glsl"
   );
}

void RendererGL::error(int error, const char* description) const
{
   puts( description );
}

void RendererGL::errorWrapper(int error, const char* description)
{
   Renderer->error( error, description );
}

void RendererGL::cleanup(GLFWwindow* window)
{
   glDeleteProgram( ObjectShader.ShaderProgram );
   glDeleteVertexArrays( 1, &AxisObject.ObjVAO );
   glDeleteVertexArrays( 1, &PositionObject.ObjVAO );
   glDeleteVertexArrays( 1, &VelocityObject.ObjVAO );
   glDeleteVertexArrays( 1, &PositionCurveObject.ObjVAO );
   glDeleteVertexArrays( 1, &VelocityCurveObject.ObjVAO );
   glDeleteVertexArrays( 1, &MovingObject.ObjVAO );
   glDeleteBuffers( 1, &AxisObject.ObjVBO );
   glDeleteBuffers( 1, &PositionObject.ObjVBO );
   glDeleteBuffers( 1, &VelocityObject.ObjVBO );
   glDeleteBuffers( 1, &PositionCurveObject.ObjVBO );
   glDeleteBuffers( 1, &VelocityCurveObject.ObjVBO );
   glDeleteBuffers( 1, &MovingObject.ObjVBO );

   glfwSetWindowShouldClose( window, GLFW_TRUE );
}

void RendererGL::cleanupWrapper(GLFWwindow* window)
{
   Renderer->cleanup( window );
}

void RendererGL::clearCurve()
{
   double x_pos, y_pos;
   glfwGetCursorPos( Window, &x_pos, &y_pos );
   const auto x = static_cast<float>(x_pos);
   const auto y = static_cast<float>(y_pos);
   
   if (1280.0f <= x && y <= 540.0f) {
      PositionMode = false;
      PositionControlPoints.clear();
      PositionCurve.clear();
      UniformVelocityCurve.clear();
      cout << "Clear the position curve." << endl;
   }
   else if (1280.0f <= x && 540.0f < y) {
      VelocityMode = false;
      VelocityControlPoints.clear();
      VelocityCurve.clear();
      VariableVelocityCurve.clear();
      cout << "Clear the velocity curve." << endl;
   }
}

void RendererGL::keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (action != GLFW_PRESS) return;

   switch (key) {
      case GLFW_KEY_P:
         PositionMode = true;
         VelocityMode = false;
         PositionCurve.resize( TotalPositionCurvePointNum );
         UniformVelocityCurve.resize( TotalAnimationFrameNum );
         cout << "Select 4 points for the position curve." << endl;
         break;
      case GLFW_KEY_V:
         PositionMode = false;
         VelocityMode = true;
         VelocityCurve.resize( TotalVelocityCurvePointNum );
         VariableVelocityCurve.resize( TotalAnimationFrameNum );
         cout << "Select 2 points for the velocity curve." << endl;
         break;
      case GLFW_KEY_C:
         clearCurve();
         break;
      case GLFW_KEY_1:
         MoveType = UNIFORM;
         break;
      case GLFW_KEY_2:
         MoveType = VARIABLE;
         break;
      case GLFW_KEY_Q:
      case GLFW_KEY_ESCAPE:
         cleanupWrapper( window );
         break;
      default:
         return;
   }
}

void RendererGL::keyboardWrapper(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   Renderer->keyboard( window, key, scancode, action, mods );
}

void RendererGL::cursor(GLFWwindow* window, double xpos, double ypos)
{
   if (1280.0f <= static_cast<float>(xpos)) {
      glfwSetCursor( window, glfwCreateStandardCursor( GLFW_CROSSHAIR_CURSOR ) );
   }
   else glfwSetCursor( window, nullptr );
}

void RendererGL::cursorWrapper(GLFWwindow* window, double xpos, double ypos)
{
   Renderer->cursor( window, xpos, ypos );
}

void RendererGL::getPointOnPositionBezierCurve(vec3& point, const float& t)
{
   const float t2 = t * t;
   const float t3 = t2 * t;
   const float one_minus_t = 1.0f - t;

   const float b0 = one_minus_t * one_minus_t * one_minus_t / 6.0f;
   const float b1 = (3.0f * t3 - 6.0f * t2 + 4.0f) / 6.0f;
   const float b2 = (-3.0f * t3 + 3.0f * t2 + 3.0f * t + 1.0f) / 6.0f;
   const float b3 = t3 / 6.0f;

   point = 
      b0 * PositionControlPoints[0] + b1 * PositionControlPoints[1] + 
      b2 * PositionControlPoints[2] + b3 * PositionControlPoints[3];
}

float RendererGL::getDeltaLength(const float& t)
{
   const float t2 = t * t;
   const float one_minus_t = 1.0f - t;

   const float b0 = -1.0f * one_minus_t * one_minus_t / 2.0f;
   const float b1 = (3.0f * t2 - 4.0f * t) / 2.0f;
   const float b2 = (-3.0f * t2 + 2.0f * t + 1.0f) / 2.0f;
   const float b3 = t2 / 2.0f;
   
   const vec3 derivative =
      b0 * PositionControlPoints[0] + b1 * PositionControlPoints[1] + 
      b2 * PositionControlPoints[2] + b3 * PositionControlPoints[3];
   return length( derivative );
}

float RendererGL::getCurveLengthFromZeroTo(const float& t)
{
   const int n = 6;
   const float h = t / static_cast<float>(n);
   float sum1 = 0.0f, sum2 = 0.0f;
   for (int i = 1; i < n; i += 2) {
      sum1 += getDeltaLength( static_cast<float>(i) * h );
   }
   for (int i = 2; i < n; i += 2) {
      sum2 += getDeltaLength( static_cast<float>(i) * h );
   }
   return h / 3.0f * (getDeltaLength( 0.0f ) + getDeltaLength( t ) + 4.0f * sum1 + 2.0f * sum2);
}

float RendererGL::getInverseCurveLength(const float& length) 
{
   const float epsilon = 0.00001f;
   float a = 0.0f, b = 1.0f;
   float mid;
   int M = 20;

   do {
      mid = (a + b) / 2;

      if ((getCurveLengthFromZeroTo( a ) - length) * (getCurveLengthFromZeroTo( mid ) - length) < 0.0f) b = mid;
      else a = mid;
      
   } while ((abs(getCurveLengthFromZeroTo( mid ) - length) > epsilon) && (M-- > 0));
   return (a + b) / 2;
}

void RendererGL::createPositionCurve()
{
   float t = 0.0f;
   const float dt = 1.0f / static_cast<float>(TotalPositionCurvePointNum - 1);
   for (auto& position : PositionCurve) {
      getPointOnPositionBezierCurve( position, t );
      t += dt;
   }
   PositionCurveObject.updateDataBuffer( PositionCurve );

   float l = 0.0f;
   const float dl = getCurveLengthFromZeroTo( 1.0f ) / static_cast<float>(TotalAnimationFrameNum - 1);
   for (auto& uniform : UniformVelocityCurve) {
      t = getInverseCurveLength( l );
      getPointOnPositionBezierCurve( uniform, t );
      l += dl;
   }
}

void RendererGL::getPointOnVelocityBezierCurve(vec3& point, const float& t)
{
   const float one_minus_t = 1.0f - t;
   const vec3 b0 = one_minus_t * VelocityControlPoints[0] + t * VelocityControlPoints[1];
   const vec3 b1 = one_minus_t * VelocityControlPoints[1] + t * VelocityControlPoints[2];
   const vec3 b2 = one_minus_t * VelocityControlPoints[2] + t * VelocityControlPoints[3];
   const vec3 b3 = one_minus_t * b0 + t * b1;
   const vec3 b4 = one_minus_t * b1 + t * b2;
   point = one_minus_t * b3 + t * b4;
}

void RendererGL::createVelocityCurve()
{
   float t = 0.0f; 
   const float dt = 1.0f / static_cast<float>(TotalVelocityCurvePointNum - 1); 
   for (auto& velocity : VelocityCurve) {
      getPointOnVelocityBezierCurve( velocity, t );
      t += dt; 
   }
   VelocityCurveObject.updateDataBuffer( VelocityCurve );

   //float l = 0.0f;
   //const float dl = getCurveLengthFromZeroTo( 1.0f ) / static_cast<float>(TotalAnimationFrameNum - 1);
   //for (auto& variable : VariableVelocityCurve) {
   //   t = getInverseCurveLength( l );
   //   getPointOnPositionBezierCurve( uniform, t );
   //   l += dl;
   //}

   //root = D_inverse(Sy(Sx_inverse(i*S*10.0f/D(1.0f)))*D(1.0f)*0.1f);
   //      mpath_anim_velocity[i][0] = x(root);
   //      mpath_anim_velocity[i][1] = y(root);
}

void RendererGL::mouse(GLFWwindow* window, int button, int action, int mods)
{
   if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
      double x_pos, y_pos;
      glfwGetCursorPos( window, &x_pos, &y_pos );
      const auto x = static_cast<float>(x_pos);
      const auto y = static_cast<float>(y_pos);
      
      if (PositionMode && PositionControlPoints.size() <= 3 && 1280.0f <= x && y <= 540.0f) {
         PositionControlPoints.emplace_back( (x - 1280.0f) * 3.0f, (540.0f - y) * 2.0f, 0.0f );
         if (PositionControlPoints.size() == 4) {
            PositionMode = false;
            createPositionCurve();
         }
      }
      else if (VelocityMode && VelocityControlPoints.size() <= 3 && 1280.0f <= x && 540.0f < y) {
         if (VelocityControlPoints.empty()) {
            VelocityControlPoints.emplace_back( 150.0f, 100.0f, 0.0f );
            VelocityControlPoints.emplace_back( (x - 1280.0f) * 3.0f, (1080.0f - y) * 2.0f, 0.0f );
         }
         else {
            VelocityControlPoints.emplace_back( (x - 1280.0f) * 3.0f, (1080.0f - y) * 2.0f, 0.0f );
            VelocityControlPoints.emplace_back( 1750.0f, 900.0f, 0.0f );

            VelocityMode = false;
            createVelocityCurve();
         }
      }
   }
}

void RendererGL::mouseWrapper(GLFWwindow* window, int button, int action, int mods)
{
   Renderer->mouse( window, button, action, mods );
}

void RendererGL::reshape(GLFWwindow* window, int width, int height)
{
   MainCamera.updateWindowSize( width, height );
   glViewport( 0, 0, width, height );
}

void RendererGL::reshapeWrapper(GLFWwindow* window, int width, int height)
{
   Renderer->reshape( window, width, height );
}

void RendererGL::registerCallbacks() const
{
   glfwSetErrorCallback( errorWrapper );
   glfwSetWindowCloseCallback( Window, cleanupWrapper );
   glfwSetKeyCallback( Window, keyboardWrapper );
   glfwSetCursorPosCallback( Window, cursorWrapper );
   glfwSetMouseButtonCallback( Window, mouseWrapper );
   glfwSetFramebufferSizeCallback( Window, reshapeWrapper );
}

void RendererGL::setAxisObject()
{
   if (AxisObject.ObjVAO != 0) {
      glDeleteVertexArrays( 1, &AxisObject.ObjVAO );
      glDeleteBuffers( 1, &AxisObject.ObjVBO );
   }

   const vector<vec3> axis_vertices = {
      { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } 
   };
   AxisObject.setObject( GL_LINES, axis_vertices );
   AxisObject.setDiffuseReflectionColor( { 0.93f, 0.92f, 0.91f, 1.0f } ); 
}

void RendererGL::setCurveObjects()
{
   if (PositionObject.ObjVAO != 0) {
      glDeleteVertexArrays( 1, &PositionObject.ObjVAO );
      glDeleteBuffers( 1, &PositionObject.ObjVBO );
   }
   if (VelocityObject.ObjVAO != 0) {
      glDeleteVertexArrays( 1, &VelocityObject.ObjVAO );
      glDeleteBuffers( 1, &VelocityObject.ObjVBO );
   }
   if (PositionCurveObject.ObjVAO != 0) {
      glDeleteVertexArrays( 1, &PositionCurveObject.ObjVAO );
      glDeleteBuffers( 1, &PositionCurveObject.ObjVBO );
   }
   if (VelocityCurveObject.ObjVAO != 0) {
      glDeleteVertexArrays( 1, &VelocityCurveObject.ObjVAO );
      glDeleteBuffers( 1, &VelocityCurveObject.ObjVBO );
   }
   if (MovingObject.ObjVAO != 0) {
      glDeleteVertexArrays( 1, &MovingObject.ObjVAO );
      glDeleteBuffers( 1, &MovingObject.ObjVBO );
   }

   PositionObject.setObject( GL_LINE_STRIP, {} );
   PositionObject.setDiffuseReflectionColor( { 0.9f, 0.8f, 0.1f, 1.0f } );

   VelocityObject.setObject( GL_LINE_STRIP, {} );
   VelocityObject.setDiffuseReflectionColor( { 0.9f, 0.8f, 0.1f, 1.0f } );

   PositionCurveObject.setObject( GL_LINE_STRIP, {} );
   PositionCurveObject.setDiffuseReflectionColor( { 0.9f, 0.1f, 0.1f, 1.0f } );

   VelocityCurveObject.setObject( GL_LINE_STRIP, {} );
   VelocityCurveObject.setDiffuseReflectionColor( { 0.9f, 0.1f, 0.1f, 1.0f } );

   MovingObject.setObject( GL_POINTS, {} );
   MovingObject.setDiffuseReflectionColor( { 1.0f, 0.0f, 0.0f, 1.0f } );
}

void RendererGL::drawAxisObject()
{   
   glUseProgram( ObjectShader.ShaderProgram );
   glLineWidth( 5.0f );

   const mat4 scale_matrix = scale( mat4(1.0f), vec3(1600.0f, 800.0f, 1.0f) );
   const mat4 translation = translate( mat4(1.0f), vec3(150.0f, 100.0f, 0.0f) );
   mat4 to_world = translation * scale_matrix;
   mat4 model_view_projection = MainCamera.ProjectionMatrix * MainCamera.ViewMatrix * to_world;
   glUniformMatrix4fv( ObjectShader.Location.World, 1, GL_FALSE, &to_world[0][0] );
   glUniformMatrix4fv( ObjectShader.Location.View, 1, GL_FALSE, &MainCamera.ViewMatrix[0][0] );
   glUniformMatrix4fv( ObjectShader.Location.Projection, 1, GL_FALSE, &MainCamera.ProjectionMatrix[0][0] );
   glUniformMatrix4fv( ObjectShader.Location.ModelViewProjection, 1, GL_FALSE, &model_view_projection[0][0] );
   AxisObject.transferUniformsToShader( ObjectShader );
   
   glBindVertexArray( AxisObject.ObjVAO );
   glDrawArrays( AxisObject.DrawMode, 0, AxisObject.VerticesCount );

   const mat4 rotation = glm::rotate( mat4(1.0f), radians( 90.0f ), vec3(0.0f, 0.0f, 1.0f) );
   to_world = to_world * rotation;
   model_view_projection = MainCamera.ProjectionMatrix * MainCamera.ViewMatrix * to_world;
   glUniformMatrix4fv( ObjectShader.Location.World, 1, GL_FALSE, &to_world[0][0] );
   glUniformMatrix4fv( ObjectShader.Location.ModelViewProjection, 1, GL_FALSE, &model_view_projection[0][0] );
   AxisObject.transferUniformsToShader( ObjectShader );

   glDrawArrays( AxisObject.DrawMode, 0, AxisObject.VerticesCount );
   glLineWidth( 1.0f );
}

void RendererGL::drawControlPoints(ObjectGL& control_points)
{ 
   glUseProgram( ObjectShader.ShaderProgram );
   glLineWidth( 3.0f );
   glPointSize( 10.0f );

   mat4 to_world = mat4(1.0f);
   mat4 model_view_projection = MainCamera.ProjectionMatrix * MainCamera.ViewMatrix * to_world;
   glUniformMatrix4fv( ObjectShader.Location.World, 1, GL_FALSE, &to_world[0][0] );
   glUniformMatrix4fv( ObjectShader.Location.View, 1, GL_FALSE, &MainCamera.ViewMatrix[0][0] );
   glUniformMatrix4fv( ObjectShader.Location.Projection, 1, GL_FALSE, &MainCamera.ProjectionMatrix[0][0] );
   glUniformMatrix4fv( ObjectShader.Location.ModelViewProjection, 1, GL_FALSE, &model_view_projection[0][0] );
   control_points.transferUniformsToShader( ObjectShader );

   glBindVertexArray( control_points.ObjVAO );
   glDrawArrays( control_points.DrawMode, 0, control_points.VerticesCount );
   glDrawArrays( GL_POINTS, 0, control_points.VerticesCount );
   glLineWidth( 1.0f );
   glPointSize( 1.0f );
}

void RendererGL::drawCurve(ObjectGL& curve)
{
   glUseProgram( ObjectShader.ShaderProgram );
   glLineWidth( 3.0f );

   mat4 to_world = mat4(1.0f);
   mat4 model_view_projection = MainCamera.ProjectionMatrix * MainCamera.ViewMatrix * to_world;
   glUniformMatrix4fv( ObjectShader.Location.World, 1, GL_FALSE, &to_world[0][0] );
   glUniformMatrix4fv( ObjectShader.Location.View, 1, GL_FALSE, &MainCamera.ViewMatrix[0][0] );
   glUniformMatrix4fv( ObjectShader.Location.Projection, 1, GL_FALSE, &MainCamera.ProjectionMatrix[0][0] );
   glUniformMatrix4fv( ObjectShader.Location.ModelViewProjection, 1, GL_FALSE, &model_view_projection[0][0] );
   curve.transferUniformsToShader( ObjectShader );

   glBindVertexArray( curve.ObjVAO );
   glDrawArrays( curve.DrawMode, 0, curve.VerticesCount );
   glLineWidth( 1.0f );
}

void RendererGL::drawMovingPoint()
{
   static int time = 0;
   if (time >= TotalAnimationFrameNum) return;

   switch(MoveType) {
      case UNIFORM:
         MovingObject.updateDataBuffer( { UniformVelocityCurve[time] } );
         break;
      case VARIABLE:
         break;
      case NONE:
      default:
         return;
   }

   glUseProgram( ObjectShader.ShaderProgram );
   glPointSize( 10.0f );

   mat4 to_world = mat4(1.0f);
   mat4 model_view_projection = MainCamera.ProjectionMatrix * MainCamera.ViewMatrix * to_world;
   glUniformMatrix4fv( ObjectShader.Location.World, 1, GL_FALSE, &to_world[0][0] );
   glUniformMatrix4fv( ObjectShader.Location.View, 1, GL_FALSE, &MainCamera.ViewMatrix[0][0] );
   glUniformMatrix4fv( ObjectShader.Location.Projection, 1, GL_FALSE, &MainCamera.ProjectionMatrix[0][0] );
   glUniformMatrix4fv( ObjectShader.Location.ModelViewProjection, 1, GL_FALSE, &model_view_projection[0][0] );
   MovingObject.transferUniformsToShader( ObjectShader );

   glBindVertexArray( MovingObject.ObjVAO );
   glDrawArrays( MovingObject.DrawMode, 0, MovingObject.VerticesCount );
   glPointSize( 1.0f );

   time++;
}

void RendererGL::drawMainCurve()
{
   glViewport( 0, 0, 1280, 1080 );
   glClearColor( 0.72f, 0.72f, 0.77f, 1.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   drawAxisObject();

   if (!PositionMode && !PositionCurve.empty()) {
      drawCurve( PositionCurveObject );
      drawMovingPoint();
   }
}

void RendererGL::drawPositionCurve()
{
   glEnable( GL_SCISSOR_TEST );
   glViewport( 1280, 540, 640, 540 );
   glScissor( 1280, 540, 640, 540 );
   glClearColor( 0.63f, 0.53f, 0.49f, 1.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   drawAxisObject();

   if (PositionControlPoints.size() <= 4) {
      PositionObject.updateDataBuffer( PositionControlPoints );
   }
   drawControlPoints( PositionObject );

   if (!PositionMode && !PositionCurve.empty()) {
      drawCurve( PositionCurveObject );
   }

   glDisable( GL_SCISSOR_TEST );
}

void RendererGL::drawVelocityCurve()
{
   glEnable( GL_SCISSOR_TEST );
   glViewport( 1280, 0, 640, 540 );
   glScissor( 1280, 0, 640, 540 );
   glClearColor( 0.55f, 0.43f, 0.38f, 1.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   
   drawAxisObject();

   if (VelocityControlPoints.size() <= 4) {
      VelocityObject.updateDataBuffer( VelocityControlPoints );
   }
   drawControlPoints( VelocityObject );

   if (!VelocityMode && !VelocityCurve.empty()) {
      drawCurve( VelocityCurveObject );
   }

   glDisable( GL_SCISSOR_TEST );
}

void RendererGL::render()
{
   drawMainCurve();
   drawPositionCurve();
   drawVelocityCurve();

   glBindVertexArray( 0 );
   glUseProgram( 0 );
}

void RendererGL::update()
{
   
}

void RendererGL::play()
{
   if (glfwWindowShouldClose( Window )) initialize();

   setAxisObject();
   setCurveObjects();
   ObjectShader.setUniformLocations( 0 );

   while (!glfwWindowShouldClose( Window )) {
      update();
      render();
      
      glfwPollEvents();
      glfwSwapBuffers( Window );
   }
   glfwDestroyWindow( Window );
}