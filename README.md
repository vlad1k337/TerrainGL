# Terrain Renderer
Simple C++/OpenGL renderer of beautiful terrains \
![](https://i.giphy.com/media/v1.Y2lkPTc5MGI3NjExdGpuNDV0MXB5dGFsdG9iejVoN3hqdm9la3B4MWthMGJ3OThuYXkyOCZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/fOxiqg4ICvaIlt1Qtv/giphy.gif)

### Features
- Terrain rendering based on heightmap
- Dynamic Levels of Detail
- Cool ImGui configuration panel

#### Requirements
- Linux System
- GPU supporting OpenGL4.0+
- libglm

#### To build, just run
``` Shell
make
```
#### Then to run:
``` Shell
./main assets/heightmap.png
```
or specify any other heightmap picture
##### Currently only supports .png heightmaps!

#### Controls
```space``` - fly upward <br/>
```b``` - show cursor, interact with ImGui panel <br/>
```n``` - hide cursor, enable camera movement <br/>
