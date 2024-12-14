#### Better UX, encapsulate existing widgets in dropdown lists. 
  e.g. a list for post-processing effects, another list for camera settings, etc.

#### Switch between Blinn-Phong and Gooch.
   currently both are implemented, but enabling Blinn-Phong requires (small) editing of fragment shader

#### Create/find a cool skybox. 
   once again, skybox class is implemented but has NO usage inside existing program. i wasn't able to find well-suiting skybox for this map, so this is to be done

#### Model Loading
  our light source now is just an invisible 3dvector in the world space. it would be cool to have some sort of global-illumination object, e.g a sun or a moon model
