#include "Renderer.h"

RendererGL* RendererGL::Renderer = nullptr;
RendererGL::RendererGL() : 
   Window( nullptr ), ClickedPoint( -1, -1 )
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
   glDeleteVertexArrays( 1, &TeapotObject.ObjVAO );
   glDeleteBuffers( 1, &AxisObject.ObjVBO );
   glDeleteBuffers( 1, &TeapotObject.ObjVBO );

   glfwSetWindowShouldClose( window, GLFW_TRUE );
}

void RendererGL::cleanupWrapper(GLFWwindow* window)
{
   Renderer->cleanup( window );
}

void RendererGL::keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (action != GLFW_PRESS) return;

   switch (key) {
      case GLFW_KEY_L:
         Lights.toggleLightSwitch();
         cout << "Light Turned " << (Lights.isLightOn() ? "On!" : "Off!") << endl;
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
   const auto x = static_cast<int>(round( xpos ));
   const auto y = static_cast<int>(round( ypos ));
   const int dx = x - ClickedPoint.x;
   const int dy = y - ClickedPoint.y;

   if (x >= 1280) {
      glfwSetCursor( window, glfwCreateStandardCursor( GLFW_CROSSHAIR_CURSOR ) );
   }
   else glfwSetCursor( window, nullptr );

   ClickedPoint.x = x;
   ClickedPoint.y = y;
}

void RendererGL::cursorWrapper(GLFWwindow* window, double xpos, double ypos)
{
   Renderer->cursor( window, xpos, ypos );
}

void RendererGL::mouse(GLFWwindow* window, int button, int action, int mods)
{
   if (button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) {
      const bool moving_state = action == GLFW_PRESS;
      if (moving_state) {
         double x, y;
         glfwGetCursorPos( window, &x, &y );
         ClickedPoint.x = static_cast<int>(round( x ));
         ClickedPoint.y = static_cast<int>(round( y ));
      }
   }
}

void RendererGL::mouseWrapper(GLFWwindow* window, int button, int action, int mods)
{
   Renderer->mouse( window, button, action, mods );
}

void RendererGL::mousewheel(GLFWwindow* window, double xoffset, double yoffset)
{
   if (yoffset >= 0.0) {
      
   }
   else {
      
   }
}

void RendererGL::mousewheelWrapper(GLFWwindow* window, double xoffset, double yoffset)
{
   Renderer->mousewheel( window, xoffset, yoffset );
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
   glfwSetScrollCallback( Window, mousewheelWrapper );
   glfwSetFramebufferSizeCallback( Window, reshapeWrapper );
}

void RendererGL::setLights()
{  
   vec4 light_position(10.0f, 150.0f, 10.0f, 1.0f);
   vec4 ambient_color(0.9f, 0.9f, 0.9f, 1.0f);
   vec4 diffuse_color(0.9f, 0.9f, 0.9f, 1.0f);
   vec4 specular_color(0.9f, 0.9f, 0.9f, 1.0f);
   Lights.addLight( light_position, ambient_color, diffuse_color, specular_color );

   light_position = vec4(7.0f, 100.0f, 7.0f, 1.0f);
   ambient_color = vec4(0.5f, 0.5f, 0.5f, 1.0f);
   diffuse_color = vec4(0.0f, 0.47f, 0.75f, 1.0f);
   specular_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
   vec3 spotlight_direction(0.0f, -1.0f, 0.0f);
   float spotlight_exponent = 128;
   float spotlight_cutoff_angle_in_degree = 7.0f;
   Lights.addLight( 
      light_position, 
      ambient_color, 
      diffuse_color, 
      specular_color,
      spotlight_direction,
      spotlight_exponent,
      spotlight_cutoff_angle_in_degree
   );  
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

void RendererGL::setTeapotObject()
{
   if (TeapotObject.ObjVAO != 0) {
      glDeleteVertexArrays( 1, &TeapotObject.ObjVAO );
      glDeleteBuffers( 1, &TeapotObject.ObjVBO );
   }

   //TeapotObject.setObject( GL_TRIANGLES, teapot_vertices, teapot_normals );
}

void RendererGL::drawAxisObject()
{
   const bool origin_light_status = Lights.isLightOn();
   if (origin_light_status) Lights.toggleLightSwitch();
   
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
   Lights.transferUniformsToShader( ObjectShader );

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

   if (origin_light_status) Lights.toggleLightSwitch();
}

void RendererGL::drawTeapotObject(const mat4& to_world)
{
   glUseProgram( ObjectShader.ShaderProgram );

   const mat4 model_view_projection = MainCamera.ProjectionMatrix * MainCamera.ViewMatrix * to_world;
   glUniformMatrix4fv( ObjectShader.Location.World, 1, GL_FALSE, &to_world[0][0] );
   glUniformMatrix4fv( ObjectShader.Location.View, 1, GL_FALSE, &MainCamera.ViewMatrix[0][0] );
   glUniformMatrix4fv( ObjectShader.Location.Projection, 1, GL_FALSE, &MainCamera.ProjectionMatrix[0][0] );
   glUniformMatrix4fv( ObjectShader.Location.ModelViewProjection, 1, GL_FALSE, &model_view_projection[0][0] );

   TeapotObject.transferUniformsToShader( ObjectShader );
   Lights.transferUniformsToShader( ObjectShader );
   
   glBindVertexArray( TeapotObject.ObjVAO );
   glDrawArrays( TeapotObject.DrawMode, 0, TeapotObject.VerticesCount );
}

void RendererGL::drawMainCurve()
{
   glViewport( 0, 0, 1280, 1080 );
   glClearColor( 0.72f, 0.72f, 0.77f, 1.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   drawAxisObject();
}

void RendererGL::drawPositionCurve()
{
   glEnable( GL_SCISSOR_TEST );
   glViewport( 1280, 540, 640, 540 );
   glScissor( 1280, 540, 640, 540 );
   glClearColor( 0.63f, 0.53f, 0.49f, 1.0f );
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   drawAxisObject();

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

   setLights();
   setAxisObject();
   setTeapotObject();
   ObjectShader.setUniformLocations( Lights.TotalLightNum );

   while (!glfwWindowShouldClose( Window )) {
      update();
      render();
      
      glfwPollEvents();
      glfwSwapBuffers( Window );
   }
   glfwDestroyWindow( Window );
}