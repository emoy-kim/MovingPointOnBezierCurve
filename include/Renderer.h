#pragma once

#include "_Common.h"
#include "Object.h"

class RendererGL
{
public:
   RendererGL(const RendererGL&) = delete;
   RendererGL(const RendererGL&&) = delete;
   RendererGL& operator=(const RendererGL&) = delete;
   RendererGL& operator=(const RendererGL&&) = delete;


   RendererGL();
   ~RendererGL();

   void play();

private:
   enum class MOVE_TYPE { NONE=0, UNIFORM, VARIABLE };

   inline static RendererGL* Renderer = nullptr;
   GLFWwindow* Window;
   bool PositionMode;
   bool VelocityMode;
   MOVE_TYPE MoveType;
   int FrameWidth;
   int FrameHeight;
   int FrameIndex;
   int PositionCurveSamplePointNum;
   int TotalPositionCurvePointNum;
   int TotalVelocityCurvePointNum;
   std::vector<glm::vec3> PositionControlPoints;
   std::vector<glm::vec3> VelocityControlPoints;
   std::vector<glm::vec3> PositionCurve;
   std::vector<glm::vec3> VelocityCurve;
   std::vector<glm::vec3> UniformVelocityCurve;
   std::vector<glm::vec3> VariableVelocityCurve;
   std::unique_ptr<CameraGL> MainCamera;
   std::unique_ptr<ShaderGL> ObjectShader;
   std::unique_ptr<ObjectGL> AxisObject;
   std::unique_ptr<ObjectGL> PositionObject;
   std::unique_ptr<ObjectGL> VelocityObject;
   std::unique_ptr<ObjectGL> PositionCurveObject;
   std::unique_ptr<ObjectGL> VelocityCurveObject;
   std::unique_ptr<ObjectGL> MovingObject;
 
   void registerCallbacks() const;
   void initialize();

   static void printOpenGLInformation();

   void getPointOnPositionBezierCurve(glm::vec3& point, float t);
   float getDeltaLength(float t);
   float getCurveLengthFromZeroTo(float t);
   float getInverseCurveLength(float length);
   void createPositionCurve();
   glm::vec3 getPointOnVelocityBezierCurve(float t);
   void createVelocityCurve();
   void clearCurve();

   void error(int error, const char* description) const;
   void cleanup(GLFWwindow* window);
   void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
   void cursor(GLFWwindow* window, double xpos, double ypos);
   void mouse(GLFWwindow* window, int button, int action, int mods);
   void reshape(GLFWwindow* window, int width, int height) const;
   static void errorWrapper(int error, const char* description);
   static void cleanupWrapper(GLFWwindow* window);
   static void keyboardWrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
   static void cursorWrapper(GLFWwindow* window, double xpos, double ypos);
   static void mouseWrapper(GLFWwindow* window, int button, int action, int mods);
   static void reshapeWrapper(GLFWwindow* window, int width, int height);

   void setAxisObject() const;
   void setCurveObjects() const;
   void drawAxisObject() const;
   void drawControlPoints(ObjectGL* control_points) const;
   void drawCurve(ObjectGL* curve) const;
   void drawMovingPoint();
   void drawMainCurve();
   void drawPositionCurve() const;
   void drawVelocityCurve() const;
   void render();
};