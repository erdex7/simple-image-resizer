Simple Image Resizer
==================================================================

About
------------------------------------------------------------------
Simple Image Resizer - allows to resize multiple images to a new size with a single command.
Includes console and graphical versions and a shared library.

Example of resizing all images from `/photo/in` directory to a height of 500 pixels 
and saving the resized images to `/photo/out` directory:

`./img_resizer_cli -d /photo/in -o /photo/out -h 500`

Build
------------------------------------------------------------------
To build you will need install Qt 5.5.1 or newer framework, qmake, mingw32-make or equal. 
At the root project directory (whith image_resizer.pro file) execute next commands:

Linux:
 - qmake
 - make -j
 
Windows:
 - qmake
 - mingw32-make -j
 
 By default Simple Image Resizer will be build to the root project directory `/bin/release`
 
Run
------------------------------------------------------------------
From root project directory run next commands:

cd bin
cd release

Console:
	`./img_resizer_cli`

GUI:
	`./img_resizer_gui`

