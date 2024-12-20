# Terrain Renderer
Simple C++/OpenGL renderer of beautiful terrains 

### Features
- Procedural terrain generation using Improved Perlin Noise function
- Post-processing effects: blur, sharp, edge outlining
- Dynamic Levels of Detail
- Run-time configuration with ImGui widgets

#### Requirements
- Linux
- GPU supporting OpenGL 4.1
- libglm, GLFW3

#### To build, just run
``` Shell
make
```
#### Then to run:
``` Shell
./main <width> <length>
```
default map size is 1024x1024

#### Controls
```space``` - fly upward <br/>
```b``` - show cursor, interact with ImGui panel <br/>
```n``` - hide cursor, enable camera movement <br/>

![kamni1](https://i.imgur.com/DvgsKPk.jpeg)
![kamni2](https://i.imgur.com/P65hMA5.png)
