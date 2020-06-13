target_link_libraries(MovingPointOnBezierCurve glad glfw3dll)

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
   target_link_libraries(MovingPointOnBezierCurve FreeImaged)
else()
   target_link_libraries(MovingPointOnBezierCurve FreeImage)
endif()