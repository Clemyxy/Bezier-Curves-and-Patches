# Bezier Curves and Splines

This project was made for the Geometric Modeling course dispensed by Eric Galin during my second year of Master at Université Lyon 1.

## Getting Started

Project made using gkit (french documentation and how to install found [here](https://perso.univ-lyon1.fr/jean-claude.iehl/Public/educ/M1IMAGE/html/index.html)).


### How to Compile

##### Linux

Once downloaded, install premake4 or 5, go to this project's root folder and use the console command ```premake(4/5) --file=./premake4.lua gmake``` to build the makefiles for all the projects, and then run the "make bezier" command to build the project's sources.

##### Windows (for visual studio 2019)

Go to the extern/visual/bin folder and copy the bin folder to the project's root folder.

Then open the powershell, go to the project's root folder and execute the ```./extern/premake5.exe --file=./premake4.lua vs2019``` command.


### How to use

This program isn't user friendly yet, you can execute simply and look at the current Bezier patch and rotate around it using you mouse (Left click to turn, right click to move along x and y axis, and mouse wheel to zoom in or out), but to see anything else, you'll have to comment and uncomment some parts of the code, as such, a rework will be necessary to make the code more user friendly.

### What I did

* Implemented Bezier curves.
* Implemented Surface of Revolution generation following a Bezier curve on the Y axis:

![SOR](https://user-images.githubusercontent.com/50468590/148090663-9d37dda9-a43e-46ad-9427-684b227630c1.png)
* Implemented Bezier patches (Using both Casteljau's and the analytical methodology):
 
![BPatch](https://user-images.githubusercontent.com/50468590/148090580-129c3c85-43f9-4eca-af5a-930ed91f58a0.png)
* Implemented global and local tapering:
  
![Taper](https://user-images.githubusercontent.com/50468590/148090705-53363273-ce72-442c-b6b2-143041717a2c.png)
![LocalTaper](https://user-images.githubusercontent.com/50468590/148091515-606ac0a5-9c7f-4292-94cf-280397543c31.png)

* Implemented global and local twisting:

![Twist](https://user-images.githubusercontent.com/50468590/148090850-74007bfb-5827-4be8-8449-4213374ef8a9.png)
![LocalTwist](https://user-images.githubusercontent.com/50468590/148091578-7ff5395d-8eee-4bc7-864e-01e1be7df7fc.png)


### TODO:

* Upgrade from Bezier to B-splines or even NURBS.
* Make this program more user friendly (Load curve/patch data from a setting file and then use imgui to create a graphical interface to apply transforms).
* Clean up and optimize code.
