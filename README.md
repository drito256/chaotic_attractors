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

