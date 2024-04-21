# Chaotic attractors #

Interactive simulation of chosen chaotic attractors written in OpenGL/C++ using following libraries: 
* __GLFW__
* __GLM__
* __ImGui__

Attractors shown in simulation:
* _lorenz_
* _halovrsen_
* _chen_
* _four-wing_
* _sprott_
* _thomas_
* _dadras_
* _nose - hoover_
* _aizawa_
* _rossler_

------------------------------------------------------------------------------------------------
<img src="images/lorenz.png" alt="Lorenz" width="1000">

------------------------------------------------------------------------------------------------

<img src="images/four-wing.png" alt="Four-wing" width="1000">

------------------------------------------------------------------------------------------------

<img src="images/sprott.png" alt="Sprott" width="1000">

------------------------------------------------------------------------------------------------

## Building on Linux: ##

Before running the application you will need following packages:

```
build-essential mesa mesa-dev mesa-common-dev mesa-utils glfw-x11 glm
```
To compile it simply use:
```make```
in 
```/src``` folder

And finally to execute the application use:
```./demo```

