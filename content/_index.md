+++
+++

## description
- Yet another ray tracer.
- A simple [ray tracer](https://en.wikipedia.org/wiki/Ray_tracing_(graphics)) based on [Blinn-Phong reflection model](https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_reflection_model).

## demonstration

### comparisions
- In each of the following comparisions, one parameter is changed while keeping all others constant.
- Decreasing `ka` darkens object as it decreases ambient color component.

| 0.9 | 0.7 | 0.5 | 0.3 | 0.1 |
| --- | --- | --- | --- | --- |
| <img src="github/ka/ka=0.9.jpg" width="100%" /> | <img src="github/ka/ka=0.7.jpg" width="100%" /> | <img src="github/ka/ka=0.5.jpg" width="100%" /> | <img src="github/ka/ka=0.3.jpg" width="100%" /> | <img src="github/ka/ka=0.1.jpg" width="100%" /> |

- Decreasing `kd` darkens object as it decreases diffused color component.

| 0.9 | 0.7 | 0.5 | 0.3 | 0.1 |
| --- | --- | --- | --- | --- |
| <img src="github/kd/kd=0.9.jpg" width="100%" /> | <img src="github/kd/kd=0.7.jpg" width="100%" /> | <img src="github/kd/kd=0.5.jpg" width="100%" /> | <img src="github/kd/kd=0.3.jpg" width="100%" /> | <img src="github/kd/kd=0.1.jpg" width="100%" /> |

- Decreasing ks decreases shininess as it decreases reflective color component.

| 0.9 | 0.7 | 0.5 | 0.3 | 0.1 |
| --- | --- | --- | --- | --- |
| <img src="github/ks/ks=0.9.jpg" width="100%" /> | <img src="github/ks/ks=0.7.jpg" width="100%" /> | <img src="github/ks/ks=0.5.jpg" width="100%" /> | <img src="github/ks/ks=0.3.jpg" width="100%" /> | <img src="github/ks/ks=0.1.jpg" width="100%" /> |

- Increasing n decreases the size of the reflected spot. As higher powers of a number < 1, viz. coefficient of reflective color, go to zero.

| 2 | 4 | 8 | 16 | 32 |
| --- | --- | --- | --- | --- |
| <img src="github/n/n=2.jpg" width="100%" /> | <img src="github/n/n=4.jpg" width="100%" /> | <img src="github/n/n=8.jpg" width="100%" /> | <img src="github/n/n=16.jpg" width="100%" /> | <img src="github/n/n=32.jpg" width="100%" /> |

- Changing Od changes base ambient and diffused color of object but keeps reflective color same.

| yellow | red | cyan | green | blue |
| --- | --- | --- | --- | --- |
| <img src="github/diffusion_color/110.jpg" width="100%" /> | <img src="github/diffusion_color/100.jpg" width="100%" /> | <img src="github/diffusion_color/011.jpg" width="100%" /> | <img src="github/diffusion_color/010.jpg" width="100%" /> | <img src="github/diffusion_color/001.jpg" width="100%" /> |

- Changing Os changes reflective color of object but keeps ambient and diffused color same.

| yellow | red | cyan | green | blue |
| --- | --- | --- | --- | --- |
| <img src="github/specular_color/110.jpg" width="100%" /> | <img src="github/specular_color/100.jpg" width="100%" /> | <img src="github/specular_color/011.jpg" width="100%" /> | <img src="github/specular_color/010.jpg" width="100%" /> | <img src="github/specular_color/001.jpg" width="100%" /> |

- Point source reflections are more focused than directional source.

| point source (at eye) | directional source (towards view dir) |
| --- | --- |
| <img src="github/light_type/type=1.jpg" width="100%" /> | <img src="github/light_type/type=0.jpg" width="100%" /> |

- Increasing number of lights increases intensity of reflections.

| 1 | 3 | 5 |
| --- | --- | --- |
| <img src="github/light_count/nl=1.jpg" width="100%" /> | <img src="github/light_count/nl=3.jpg" width="100%" /> | <img src="github/light_count/nl=5.jpg" width="100%" /> |

- Changing position of positional light moves the reflection spots towards the light as expected.

| left | down | center | up | right |
| --- | --- | --- | --- | --- |
| <img src="github/light_position/xyz=-305.jpg" width="100%" /> | <img src="github/light_position/xyz=0-35.jpg" width="100%" /> | <img src="github/light_position/xyz=005.jpg" width="100%" /> | <img src="github/light_position/xyz=035.jpg" width="100%" /> | <img src="github/light_position/xyz=305.jpg" width="100%" /> |

- Increasing light rgb values increases intensity of diffused, ambient and reflected colors.

| (0.25, 0.25, 0.25) | (.5, .5, .5) | (1, 1, 1) |
| --- | --- | --- |
| <img src="github/light_intensity/rgb=0.250.250.25.jpg" width="100%" /> | <img src="github/light_intensity/rgb=0.50.50.5.jpg" width="100%" /> | <img src="github/light_intensity/rgb=111.jpg" width="100%" /> |

- Soft shadows

| disabled | enabled |
| --- | --- |
| <img src="github/softshadows/shadows.jpg" width="100%" /> | <img src="github/softshadows/softshadows.jpg" width="100%" /> |

<img src="github/softshadows/sharp_soft_shadows.gif" width="100%" />

- Vertex normals can be used to add smoothness without changing the mesh. In both of the below images the triangles have exactly same positions. Tuning vertex normals generates illusion of smoothness.

| flat | smooth |
| --- | --- |
| <img src="github/vertex_normals/texture-flat.jpg" width="100%" /> | <img src="github/vertex_normals/texture-smooth.jpg" width="100%" /> |

- Texture stitching.

<img src="github/textures/sphere.jpg" width="100%" />

- Viewing distance can be adjusted to focus on objects at that particular distance while blurring out objects at other distances.

| disabled | near focus | far focus |
| --- | --- | --- |
| <img src="github/viewdistance/viewdist_disabled.jpg" width="100%" /> | <img src="github/viewdistance/viewdist_near.jpg" width="100%" /> | <img src="github/viewdistance/viewdist_far.jpg" width="100%" /> |

<img src="github/viewdistance/viewdist.gif" width="100%" />

- Parallel projection has no perspective distortion, therefore spheres look like perfect circles.

| parallel | perspective |
| --- | --- |
| <img src="github/projection/parallel.jpg" width="100%" /> | <img src="github/projection/perspective.jpg" width="100%" /> |

- A simple reflection scene.

<img src="github/reflection/scene.jpg" width="100%" />

- A refraction scene. Moving the back sphere, moves its image on the font sphere in reverse direction.

| | | |
| --- | --- | --- |
| <img src="github/refraction/r1.jpg" width="100%" /> | <img src="github/refraction/r2.jpg" width="100%" /> | <img src="github/refraction/r3.jpg" width="100%" /> |

- A transparent hollow sphere in a colorful room.

<img src="github/rainbow.jpg" width="100%" />

- A transparent sphere of higher refractive index inside another transparent sphere. Increasing inner sphere size increases the shadow it casts (on the bottom left).

<img src="github/shadow_size/shadow_size.gif" width="100%" />

- Total internal reflection (TIR). A transparent sphere of refractive index `i` inside another transparent sphere of refractive index `o`.

| i = o (no TIR) | i > o (TIR) |
| --- | --- |
| <img src="github/tir/notir.jpg" width="100%" /> | <img src="github/tir/tir.jpg" width="100%" /> |

- A house.

<img src="github/t_house.jpg" width="100%" />

