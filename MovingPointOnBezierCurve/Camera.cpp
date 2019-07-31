#include "Camera.h"

CameraGL::CameraGL() : CameraGL(-1.0f, 1.0f)
{   
}

CameraGL::CameraGL(float near_plane, float far_plane) :
   NearPlane( near_plane ), FarPlane( far_plane ), ViewMatrix( mat4(1.0f) ), ProjectionMatrix( mat4(1.0f) )
{
}

void CameraGL::updateWindowSize(const int& width, const int& height)
{
   const auto w = static_cast<float>(width);
   const auto h = static_cast<float>(height);
   ProjectionMatrix = ortho( 0.0f, w, 0.0f, h, NearPlane, FarPlane );
}