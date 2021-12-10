# GraphicsRenderer

To run the program just use the command "make" (it defaults to make speedy).
If it doesn't work, run "make clean" first.

To cycle through modes press m (cycles through ray tracing, rasterizing, different types of shading).

To close the program press "q".
To move the camera use the arrow keys and to move the camera orientation use "w,a,s,d".
To go forwards or backwards use "p" and "l".
With "y,g,h,j,n,m" you can change the position of the light/lights.
Press "o" to statr orbitiong.

If you want to add PhotonMaps or multiple light sources change the value of the bool variables in src/GraphicsRenderer lines 158/159.

The keys "u" and "f" will spawn triangles on the screen only on mode = 6.
