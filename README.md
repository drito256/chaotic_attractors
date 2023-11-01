# Chaotic attractors #

Interactive simulation of chosen chaotic attractors written in OpenGL/C++ using following libraries: 
* GLFW
* GLM
* ImGui

Attractors shown in simulation:
* lorenz 
* halovrsen
* chen
* four-wing
* sprott
* thomas
* dadras
* nose - hoover
* aizawa
* rossler

------------------------------------------------------------------------------------------------

list of tasks to do:

- [x] separation of Point class
- [x] creation of Camera class
- [ ] creation of Arrow class ? 
- [x] separation of shader.h -> shader.h & shader.cpp

when selecting different equations:
- [ ] function to reinit point/arrow positions
- [ ] function for changing camera location
- [ ] scaling coordinate system net

  * will probably need to do abstraction of VAOs/VBOs/Renderer because of the previous points made

