#include "Camera.h"

CameraGL::CameraGL() : CameraGL(-1.0f, 1.0f)
{
}

CameraGL::CameraGL(float near_plane, float far_plane) : 
   Width( 0 ), Height( 0 ), NearPlane( near_plane ), FarPlane( far_plane ),
   ViewMatrix( glm::mat4(1.0f) ), ProjectionMatrix( glm::mat4(1.0f) )
{
}

void CameraGL::updateWindowSize(int width, int height)
{
   Width = width;
   Height = height;
   ProjectionMatrix = glm::ortho( 
      0.0f, 
      static_cast<float>(width), 
      0.0f, 
      static_cast<float>(height), 
      NearPlane, 
      FarPlane 
   );
}