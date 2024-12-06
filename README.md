# Terrain Renderer
Simple C++/OpenGL renderer of beautiful terrains \
![](https://i.giphy.com/media/v1.Y2lkPTc5MGI3NjExdGpuNDV0MXB5dGFsdG9iejVoN3hqdm9la3B4MWthMGJ3OThuYXkyOCZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/fOxiqg4ICvaIlt1Qtv/giphy.gif)

### Features
- Terrain rendering based on heightmap
- Dynamic Level of Detail
- Blinn-Phong shading, Gooch shading (implemented, but needs more graining)
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
**b** - show cursor, interact with ImGui panel \
**n** - hide cursor, enable camera movement

![kek](https://github.com/user-attachments/assets/7bd32e78-9984-42ec-a1db-40725d6a09ad)
