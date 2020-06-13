#pragma once

#include "_Common.h"

class CameraGL
{
public:
   CameraGL();
   CameraGL(float near_plane, float far_plane);

   [[nodiscard]] const glm::mat4& getViewMatrix() const { return ViewMatrix; }
   [[nodiscard]] const glm::mat4& getProjectionMatrix() const { return ProjectionMatrix; }
   void updateWindowSize(int width, int height);

private:
   int Width;
   int Height;
   float NearPlane;
   float FarPlane;
   glm::mat4 ViewMatrix;
   glm::mat4 ProjectionMatrix;
};