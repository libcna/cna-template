<!-- GENERATED FILE — do not edit by hand. -->
<!-- Source: cmake/renderers.json — regenerate with tools/gen_renderer_files.py -->

# CNA renderers

CNA currently exposes **10 renderers**. You pick one at configure time with a single cache variable:

```bash
cmake -S . -B build -DCNA_GRAPHICS_RENDERER=OPENGLES3
```

If you set nothing, CNA chooses `WEBGL2` on Web; `SDLGPU` on Windows, macOS, iOS; `OPENGLES3` on Linux, Android, BSD, other targets.

## How to read this

**Platforms** are where the renderer is supported. cna-template refuses, with an explanation, to configure a renderer for a platform not listed here. A few renderers additionally have *experimental* platforms — CNA permits them but does not support them, so the template warns instead of refusing.

**Display** says whether the renderer opens a window. The three that do not (`HEADLESS`, `SOFTWARE`, `STUB`) need no X server, no GPU and no video driver, which makes them the right choice for CI and servers. Note that `SOFTWARE` genuinely rasterizes but presents nowhere.

**Scope** is `2D` or `2D+3D`. On a 2D-only renderer the 3D calls (`VertexBuffer`, `DrawUserPrimitives`, depth state) throw. Query `GraphicsDevice::SupportsCapability()` at runtime rather than testing the renderer's name.

**Tested by cna-template** describes this template's CI, not CNA's support: *every PR* is built and smoke-run on each push, *broad matrix* is the scheduled job, *on demand* is manual, and *not on CI runners* means a GitHub runner cannot provide the platform or SDK.

## SDL

| Renderer | Platforms | Scope | Display | Dependency | Presets | Tested | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `SDLGPU` | Linux, Windows, macOS (+ iOS experimental) | 2D+3D | window | none | `sdlgpu`, `windows-msvc-sdlgpu` | broad matrix | CNA's default on Windows, macOS and iOS. SDL3 selects Vulkan, Direct3D 12 or Metal internally. |

## OpenGL family (shared EasyGL implementation)

| Renderer | Platforms | Scope | Display | Dependency | Presets | Tested | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `OPENGLES3` | Linux, Windows, macOS, Android (+ BSD, other targets experimental) | 2D+3D | window | sibling ../easy-gl (which needs ../meta-gl) | `opengles3` | every PR | CNA's default on Linux, Android, BSD and other non-web targets. Internally EasyGL. |
| `OPENGL33` | Linux, Windows, macOS | 2D+3D | window | sibling ../easy-gl (which needs ../meta-gl) | `opengl33` | broad matrix | Internally EasyGL, desktop GL 3.3 core profile. |
| `WEBGL2` | Web | 2D+3D | window | sibling ../easy-gl (which needs ../meta-gl) | `web-webgl2` | broad matrix | CNA's default renderer under Emscripten. Internally EasyGL. |

## Modern GPU APIs

| Renderer | Platforms | Scope | Display | Dependency | Presets | Tested | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `WEBGPU` | Linux, Windows, macOS, Web | 2D+3D | window | wgpu-native binary release, auto-downloaded | `webgpu`, `web-webgpu` | broad matrix | Native wgpu-native on desktop. Under Emscripten it builds through the Dawn (emdawnwebgpu) port, as CNA's own web CI does, alongside WEBGL2 in one -DCNA_GRAPHICS_RENDERERS="WEBGL2;WEBGPU" build; supported, and the browser then needs WebGPU. |

## CPU / diagnostic

| Renderer | Platforms | Scope | Display | Dependency | Presets | Tested | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `HEADLESS` | Linux, Windows, macOS, Android (+ Web experimental) | 2D+3D | **none** | none | `headless` | every PR | No GPU and no window at all. The right choice for CI and servers. |
| `SOFTWARE` | Linux, Windows, macOS, Android (+ Web experimental) | 2D+3D | **none** | none | `software` | every PR | Real CPU rasterizer. Creates no window, so Present() puts nothing on screen -- it is a compute/test renderer, not a display one. |
| `STUB` | Linux, Windows, macOS, Android (+ Web experimental) | 2D+3D | **none** | none | `stub` | every PR | Deliberate no-op renderer. Draws nothing, touches no GPU or window. |

## Portable middleware

| Renderer | Platforms | Scope | Display | Dependency | Presets | Tested | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `FNA3D` | Linux, Windows, macOS | 2D+3D | window | FNA3D + MojoShader via FetchContent; needs Python3 | `fna3d` | broad matrix | The XNA-shaped C library FNA renders through. Picks SDL_GPU/D3D11/GL internally at runtime; SDL_GPU here is FNA3D's technology name, not CNA's SDLGPU renderer identity. |

## DirectX

| Renderer | Platforms | Scope | Display | Dependency | Presets | Tested | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `DIRECTX9` | Windows | 2D+3D | window | d3d9.h | `windows-mingw-directx9` | not on CI runners | Direct3D 9. |

## Presets

Renderers in common use have a ready-made preset; the rest are selected with `-DCNA_GRAPHICS_RENDERER=<NAME>` against any preset or a plain build directory. Every one of the 10 renderers above is selectable either way.

```bash
cmake --list-presets                 # what is available
cmake --preset opengles3             # configure
cmake --build --preset opengles3     # build
ctest --preset opengles3             # smoke test
```

<sub>Cross-checked against CNA's own canonical list of 10 renderers at generation time.</sub>
