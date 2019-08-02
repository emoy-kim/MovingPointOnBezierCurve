/*
 * Author: Emoy Kim
 * E-mail: emoy.kim_AT_gmail.com
 * 
 * This code is a free software; it can be freely used, changed and redistributed.
 * If you use any version of the code, please reference the code.
 * 
 */

#pragma once

#include "_Common.h"

#include "Camera.h"
#include "Object.h"

class RendererGL
{
   enum MOVE_TYPE{ NONE=0, UNIFORM, VARIABLE };

   static RendererGL* Renderer;
   GLFWwindow* Window;

   bool PositionMode, VelocityMode;
   int MoveType;
   int TotalPositionCurvePointNum;
   int TotalVelocityCurvePointNum;
   int TotalAnimationFrameNum;
   vector<vec3> PositionControlPoints, VelocityControlPoints;
   vector<vec3> PositionCurve, VelocityCurve;
   vector<vec3> UniformVelocityCurve, VariableVelocityCurve;

   CameraGL MainCamera;
   ShaderGL ObjectShader;
   ObjectGL AxisObject;
   ObjectGL PositionObject, VelocityObject;
   ObjectGL PositionCurveObject, VelocityCurveObject;
   ObjectGL MovingObject;
 
   void registerCallbacks() const;
   void initialize();

   void printOpenGLInformation() const;

   void getPointOnPositionBezierCurve(vec3& point, const float& t);
   float getDeltaLength(const float& t);
   float getCurveLengthFromZeroTo(const float& t);
   float getInverseCurveLength(const float& length);
   void createPositionCurve();
   void getPointOnVelocityBezierCurve(vec3& point, const float& t);
   void createVelocityCurve();
   void clearCurve();

   void error(int error, const char* description) const;
   void cleanup(GLFWwindow* window);
   void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
   void cursor(GLFWwindow* window, double xpos, double ypos);
   void mouse(GLFWwindow* window, int button, int action, int mods);
   void reshape(GLFWwindow* window, int width, int height);
   static void errorWrapper(int error, const char* description);
   static void cleanupWrapper(GLFWwindow* window);
   static void keyboardWrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
   static void cursorWrapper(GLFWwindow* window, double xpos, double ypos);
   static void mouseWrapper(GLFWwindow* window, int button, int action, int mods);
   static void reshapeWrapper(GLFWwindow* window, int width, int height);

   void setAxisObject();
   void setCurveObjects();
   void drawAxisObject();
   void drawControlPoints(ObjectGL& control_points);
   void drawCurve(ObjectGL& curve);
   void drawMovingPoint();
   void drawMainCurve();
   void drawPositionCurve();
   void drawVelocityCurve();
   void update();
   void render();


public:
   RendererGL(const RendererGL&) = delete;
   RendererGL(const RendererGL&&) = delete;
   RendererGL& operator=(const RendererGL&) = delete;
   RendererGL& operator=(const RendererGL&&) = delete;


   RendererGL();
   ~RendererGL();

   void play();
};