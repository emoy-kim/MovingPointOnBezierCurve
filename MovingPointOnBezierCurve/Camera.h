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

class CameraGL
{
   float NearPlane, FarPlane;

public:
   mat4 ViewMatrix, ProjectionMatrix;

   CameraGL();
   CameraGL(float near_plane, float far_plane);

   void updateWindowSize(const int& width, const int& height);
};