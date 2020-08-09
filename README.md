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

Extras
    HW0
    - [x] interesting images
    HW1a
    - [x] gently varying background (implemented but removed for better images, was a single line impl anyway)
    HW1b
    - [x] soft shadow
    HW1c
    - [x] parallel projection (not done properly)

Bug fixes
    - [x] Invalid number of args fix
    - [x] Output file name fix
    - [x] Accept empty lines
    - [x] Ignore unknown keywords
    - [x] Sometimes your rays go through triangles without detecting the intersection.
    - [x] Your texture parser doesnt always work, there might be newlines or whitespace at the end of a line.
    - [x] Your shadows aren't correct sometimes.
    - [x] texture paths
    - [x] 1c testcases
    - [x] 1d testcases
    - [ ] I see you did the parallel projection, but it pulls the camera extremely far back

Features
    - [x] texture mapping on triangles
    - [x] texture mapping + smooth shading on triangles
    - [x] parallel projection
    - [x] texture mapping on spheres
    - [x] shadow subtractive model
    - [x] hollow sphere
    - [x] depth of field effect
    - [x] example ppms
        - [x] reflection
        - [x] refraction
        - [x] total internal reflection
        - [x] shadows
        - [x] depth of field
        - [x] recheck
    - [x] readme
        - [x] explain examples
    - [x] set macros properly before submission

