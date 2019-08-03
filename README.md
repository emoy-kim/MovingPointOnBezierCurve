# MovingPointOnBezierCurve

A point on a Bezier curve which is created from some control points is moving at the different speed.

*Top-right* graph means the position of this point, and *bottom-right* graph means the velocity of this as the time passes.

Two moving types are supported, which are an uniform or variable speed.

The first rendering as below picture shows the point moving at an uniform speed.

The second one shows the point moving at an variable speed based on the velocity graph.




![movingpoint](https://user-images.githubusercontent.com/17864157/62411453-d97ad480-b62d-11e9-9271-1e67bcec5697.gif)






## Test Environment
  * Windows 10
  * Visual Studio 2017
  * Surface Book 2
  
## Library Dependencies
  * OpenCV (not used)
  * OpenGL

## Keyboard Commands
  * **p key**: activate the position curve drawing mode (*top-right graph*)
  * **v key**: activate the velocity curve drawing mode (*bottom-right graph*)
  * **c key**: clear the graph of the sub-window the mouse cursor is included in.
  * **1 key**: redering the moving point at an uniform speed
  * **2 key**: redering the moving point at an variable speed
  * **q key**: exit
