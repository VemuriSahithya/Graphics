This folder contains the code in C++ to rotate and zoom the 3D object using OpenGL. 
Initially the 3D object is loaded, read and then finally displayed using OpenGL.
I've implemented the rotation along x,y,z-axis and zooming features, these features can be observed in the following way.
	1. If we scroll up and down using mouse wheel or by using touchpad gesture then the object can be zoomed out and in respectively.
	2. The object can be rotated along X-axis by pressing and holding the left mouse button and by moving the cursor accordingly inside the window where the object is displayed.
	3. The object can be rotated along Y-axis by pressing and holding the right mouse button and by moving the cursor accordingly inside the window where the object is displayed.
	4. The object can be rotated along Z-axis just by moving the cursor inside the window.

To reproduce the 3D model, please run the following commands.
$g++ main.cpp -lGL -lglut -lglfw -lGLEW
$./a.out
Once the 3D model of the violin case (3D object which I've used to load and check the features) is displayed then you can the check the features by following the above mentioned steps.
If you want to quit then press Esc to exit the window.
