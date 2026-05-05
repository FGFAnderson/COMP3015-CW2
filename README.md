# COMP3015-CW2

Plymouth University COMP3015 Games Graphics Pipelines (25/SP/SB/M)

**Initally developed on:** 

- **OS:** Pop!_OS (Linux) 
- **Build:** CMake + Ninja 
- **Compiler:** Clang

**Ported To**:
[TODO]

## Videos & Repo

- [CW1 Demo](https://youtu.be/wuI9aAfJ708)
- [CW2 Video](https://youtu.be/T5bjJyn9lh4)
- GitHub: [FGFAnderson/COMP3015-CW2](https://github.com/FGFAnderson/COMP3015-CW2)

## Building and Running

```sh
./run.sh
```

Or manually: `cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug && ninja -C build && ./build/comp3015`

## How It Works

A first-person shooter on OpenGL. The player shoots enemy drones and survives progressive waves.

## Controls

| Input | Action |
|---|---|
| W / S / A / D | Move |
| Space / Left Ctrl | Jump / Down |
| Mouse | Look |
| Left Click | Shoot |
| Esc | Quit |

## Code Structure

| File | Responsibility |
|---|---|
| `scenebasic_uniform` | Main scene - update, render, input, wave spawning |
| `core/physics_world` | Bullet Physics, character controller, map BVH, raycasting |
| `core/projectile_system` | Projectiles, enemy AI, dynamic point lights |
| `core/particle_system` | GPU burst particles, instanced rendering |
| `core/post_process` | Offscreen FBO + 2-pass Gaussian blur |
| `core/mesh_instance` | Manages AssimpMeshes |
| `core/flycamera` | Mouse look camera |
| `core/skybox` | Cubemap skybox |
| `core/hud` | ImGui overlay - health, kill counter, game over |
| `helper/assimpmesh` | Assimp loader |
| `shader/lighting` | Blinn-Phong + dynamic point lights |
| `shader/blur_horizontal/vertical` | Gaussian blur |

## Shaders

### Gaussian Blur (Week 5)

**Gaussian Blur** - blur triggered on player hit. Started from the Week 5 lab framebuffer template which renders the scene to an offscreen FBO.

**What makes it unique:** activates on player hit and decays over time.

### Dynamic Multi-Light (Week 2)

**Dynamic Multi-Light** - Dynamic lights placed by projectile positions. Player shots glow yellow-white, enemy shots red. Started from the Blinn-Phong single directional light shader from CW1.

**What was changed:** light positions and colours are uploaded as uniform arrays each frame. The positions from live projectile positions transformed to eye space.

**What makes it unique:** the lighting is dynamic to the gameplay. Projectiles move and die throughout the scene, lighting it.

### Particle System (Week 7)

**Particle System** - Particles spawn on enemy death with randomised velocity and gravity. Alpha fades from `life / maxLife`. Started from a combination of two Week 7 lecture techniques, the Particle Fountain's kinematic equation `P(t) = P0 + v0t + 0.5at^2`, and `glDrawElementsInstanced` with a 3D mesh.

**What was changed:** By using the Particle Fountain's kinematic equation, position can always be computed from initial conditions and elapsed time. Removing the need for transform feedback and buffer ping-pong entirely.

**What makes it unique:** burst on enemy death rather than a continuous emitter.

## Software Engineering Trade-offs

**Normal Mapping** - the normals of the FBX model exported from Unity were malformed. This caused me to remove normal mapping from the application, possibly losing detail. The voxel nature of the scene likely mitigated the impact of this.

**Dynamic lights** - are packed into a uniform array each frame. Excess lights are silently dropped if too many projectiles are alive at once.

## Evaluation

The game functions and has a basic gameplay loop. Bullet Physics handles jumping, gravity, and arena collisions. ImGui gives live health and kill counter feedback without interfering with the render pipeline. The visuals of the game are far better from CW1 and work well with the shading techniques.

The shader techniques are not particularly well shown off, only contributing to minor parts of the gameplay. Particles are only spawned on death of enemies; projectiles are small and fast so the coloured glow of the dynamic lighting can be missed; and blur is only used when the character is hit.

I would also have liked to add more game mechanics to make the game more interesting such as improving the AI of the enemies and more variation between them.

## AI Usage

Transcripts are in `ai_transcript/`.
