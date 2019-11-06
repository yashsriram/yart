* Compile the raytracer using
  * `make`
* All examples are given under examples directory
    * To run on an example scene, make the current directory containing raytracer executable and run `./raytracer <path-to-scene>.txt`
    * The image file will be created in the same directory as the scene description file
    * For example: `./raytracer examples/triangles/single_flat.txt` creates `examples/triangles/single_flat.ppm`
* In scene descriptions texture file paths are assumed to start from raytracer executable directory
* There are examples scenes of
    * reflections and refractions where a parameter slightly changed to illustrate the changes
    * total internal reflection where refractive indices are changed to toggle the effect
    * compositing shadows where object size is incrementally changed
    * depth of field effects where viewdist is changed to change the focus of near and far objects. also includes a case where this effect is disabled
    * a combined scene with all effects
