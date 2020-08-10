# yart

## description
- Yet another ray tracer.
- A simple ray tracer based on [Blinn-Phong reflection model](https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_reflection_model).

## code
- Code is written in `C++`.
- `include/` & `src/` contains all the src code.
- `examples/` contains some example scene files.
- `textures/` contains texture files.
- `assignments/` contains problem statements from which this raytracer was created and some reports.

## documentation
- The documentation for the code is itself.

## usage

### how to run? [linux]
- Compile the raytracer using `make` to create an executable `raytracer`.
- The executable reads a scene file (and possibly some texture files) and generates a `ppm` image.
- Create the image of a scene using `./raytracer <path-to-scene-file>`. It will be in the same directory as the scene file.
    - For example, `./raytracer examples/scene.txt` creates `examples/scene.ppm`.

### format of scene file
- The format is similar to [.obj](https://en.wikipedia.org/wiki/Wavefront_.obj_file) file format.
- Each line a the scene file defines something. Each line starts with a keyword and varying number of space separated parameters can follow.
- All colors are in normalized scale (0 - 1).
- The recognized keywords are as follows.
    - `# ...`: Comment. Will be ignored.
    - `eye x y z`: Camera position.
    - `viewdir x y z`: Camera viewing direction. Must be a unit vector.
    - `updir x y z`: Up direction. Must be a unit vector.
    - `vfov angle`: Vertical field of view in degrees.
    - `imsize width height`: Output image dimensions in pixels.
    - `bkgcolor r g b`: Background color.
    - `light x y z w r g b`: A light source.
        - `x y z` is position.
        - `w` can be 0 (directional source) or 1 (point source).
        - `r g b` is color.
    - `mtlcolor Odr Odg Odb Osr Osg Osb ka kd ks n a h`: Material color.
        - `Odr Odg Odb` is diffusion color.
        - `Osr Osg Osb` is specular color.
        - `ka kd ks` are ambient, diffusion and specular co-efficients respectively in the Blinn-Phong model.
        - `n` is the power in Blinn-Phong model.
        - `a` is opacity level (0 - 1).
        - `h` is refractive index.
    - `texture <path-to-texture-file>`: Path is assumed to start from raytracer executable directory. Has to be a valid `ppm` file.
    - `sphere x y z radius`: A spherical object.
    - `v x y z`: Vertex position.
    - `vt u v`: A texture coordinate. `u` & `v` must be in [0, 1].
    - `vn x y z`: A vertex normal definition. Must be a unit vector.
    - `f ...`: A face of a triangle.
        - All following arguments are indices of previously defined entities. The entities are counted from starting from 1 (and not 0).
        - `f v1 v2 v3`: `v1 v2 v3`: Vertex indices.
        - `f v1/vt1 v2/vt2 v3/vt2`: Vertex indices annotated with texture coordinate indices.
        - `f v1//vn1 v2//vn2 v3//vn3`: Vertex indices annotated with vertex normal indices.
        - `f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3`: Vertex indices annotated with texture coordinate indices & vertex normal indices.
    - `parallel`: Presence indicates that parallel projection is to be used. Default is `perspective`.
    - `viewdist distance`: Viewing distance for depth of field effect.
- Once a material color or texture is defined, it will be used for all the following objects in the scene until another is defined.
- For integer parameters, don't add a succeding decimal part.
    - `1` is ok.
    - `1.` is not okay. `1.0` is not okay.
    - This is caused merely because of the `cin` construct behaviour and has nothing to do with the ray tracer.

## demonstration

### comparisions
- In each of the following comparisions, one parameter is changed while keeping all others constant.
- Decreasing `ka` darkens object as it decreases ambient color component.

| 0.9 | 0.7 | 0.5 | 0.3 | 0.1 |
| --- | --- | --- | --- | --- |
| ![](./github/ka/ka=0.9.jpg) | ![](./github/ka/ka=0.7.jpg) | ![](./github/ka/ka=0.5.jpg) | ![](./github/ka/ka=0.3.jpg) | ![](./github/ka/ka=0.1.jpg) |

- Decreasing `kd` darkens object as it decreases diffused color component.

| 0.9 | 0.7 | 0.5 | 0.3 | 0.1 |
| --- | --- | --- | --- | --- |
| ![](./github/kd/kd=0.9.jpg) | ![](./github/kd/kd=0.7.jpg) | ![](./github/kd/kd=0.5.jpg) | ![](./github/kd/kd=0.3.jpg) | ![](./github/kd/kd=0.1.jpg) |

- Decreasing ks decreases shininess as it decreases reflective color component.

| 0.9 | 0.7 | 0.5 | 0.3 | 0.1 |
| --- | --- | --- | --- | --- |
| ![](./github/ks/ks=0.9.jpg) | ![](./github/ks/ks=0.7.jpg) | ![](./github/ks/ks=0.5.jpg) | ![](./github/ks/ks=0.3.jpg) | ![](./github/ks/ks=0.1.jpg) |

- Increasing n decreases the size of the reflected spot. As higher powers of a number < 1, viz. coefficient of reflective color, go to zero.

| 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- |
| ![](./github/n/n=2.jpg) | ![](./github/n/n=4.jpg) | ![](./github/n/n=8.jpg) | ![](./github/n/n=16.jpg) | ![](./github/n/n=32.jpg) |

- Changing Od changes base ambient and diffused color of object but keeps reflective color same.

| yellow | red | cyan | green | blue |
| --- | --- | --- | --- | --- |
| ![](./github/diffusion_color/110.jpg) | ![](./github/diffusion_color/100.jpg) | ![](./github/diffusion_color/011.jpg) | ![](./github/diffusion_color/010.jpg) | ![](./github/diffusion_color/001.jpg) |

- Changing Os changes reflective color of object but keeps ambient and diffused color same.

| yellow | red | cyan | green | blue |
| --- | --- | --- | --- | --- |
| ![](./github/specular_color/110.jpg) | ![](./github/specular_color/100.jpg) | ![](./github/specular_color/011.jpg) | ![](./github/specular_color/010.jpg) | ![](./github/specular_color/001.jpg) |

- Point source reflections are more focused than directional source.

| point source (at eye) | directional source (towards view dir) |
| --- | --- |
| ![](./github/light_type/type=1.jpg) | ![](./github/light_type/type=0.jpg) |

- Increasing number of lights increases intensity of reflections.

| 1 | 3 | 5 |
| --- | --- | --- |
| ![](./github/light_count/nl=1.jpg) | ![](./github/light_count/nl=3.jpg) | ![](./github/light_count/nl=5.jpg) |

- Changing position of positional light moves the reflection spots towards the light as expected.

| left | down | center | up | right |
| --- | --- | --- | --- | --- |
| ![](./github/light_position/xyz=-305.jpg) | ![](./github/light_position/xyz=0-35.jpg) | ![](./github/light_position/xyz=005.jpg) | ![](./github/light_position/xyz=035.jpg) | ![](./github/light_position/xyz=305.jpg) |

- Increasing light rgb values increases intensity of diffused, ambient and reflected colors.

| (0.25, 0.25, 0.25) | (.5, .5, .5) | (1, 1, 1) |
| --- | --- | --- |
| ![](./github/light_intensity/rgb=0.250.250.25.jpg) | ![](./github/light_intensity/rgb=0.50.50.5.jpg) | ![](./github/light_intensity/rgb=111.jpg) |

## roadmap
- [ ] decrease size of repo.
- [ ] make sure all examples work.
- [ ] add good examples to demo section.
- [ ] reorder sections of readme.
- [ ] add features to roadmap.
- There are examples scenes of
    - reflections and refractions where a parameter slightly changed to illustrate the changes
    - total internal reflection where refractive indices are changed to toggle the effect
    - compositing shadows where object size is incrementally changed
    - depth of field effects where viewdist is changed to change the focus of near and far objects. also includes a case where this effect is disabled
    - a combined scene with all effects
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

