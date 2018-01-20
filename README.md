The assignment was tested on Windows PC and was built with Visual Studio.

The design decisions that I made are the following:

- Each class in shapes.hpp has a member of type b2body* that gets initiliazed by 
class constructor using b2world pointer. Each instantiated object gets added to the
global Box2D simulation. When an object gets instantiated it gets added to the corresponding 
shape vector (except for red circle and white rectangle)

- Box2D objects have member function GetAngle() which is used for creating transformation
matrix for each object to sync the simulation with on screen rendering.  

- When the program mode is switched to dragging, the world point of a mouse 
is passed to attachMouse function that loops through all the objects and instantiates
a mouse joint if a cursor intersects an object. When a mouse is moved the target is updated 
accordingly. When a mouse button is no longer pressed the joint gets destroyed and set to NULL

- The on-screen drawings are created as chain shapes using the method from Moodle forum. 
