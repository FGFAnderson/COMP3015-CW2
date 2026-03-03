# COMP3015-CW1

Plymouth University COMP3015 Games Graphics Pipelines (25/SP/SB/M)

## YouTube Demo

[https://youtu.be/wuI9aAfJ708]

## How It Works

Simply an OpenGL scene of a forest rendered with Blinn-Phong lighting. The scene renders into an HDR framebuffer, then a fullscreen tone mapping pass resolves it to LDR with gamma correction.

Lighting has three components directional sun, a camera-attached spotlight, and distance fog. 

## Main Code Layout

- `scenebasic_uniform.cpp` — scene setup and render loop
- `core/mesh_instance` — wraps a mesh with its textures and sets shader uniforms per draw
- `core/flycamera` — WASD + mouse first-person camera
- `shader/lighting` — Blinn-Phong, normal mapping, fog, alpha cutout
- `shader/tonemapping` — Reinhard tone map + gamma correction
- `helper/` — OBJ loading, texture loading, GLSL program wrapper, GLFW runner

## Controls

| Input | Action |
|---|---|
| W / S / A / D | Move |
| Space / Left Ctrl | Up / Down |
| Mouse | Look |
| Esc | Quit |

## AI Usage

Transcripts are in `ai_transcript/`.
