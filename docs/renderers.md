<!-- GENERATED FILE — do not edit by hand. -->
<!-- Source: cmake/renderers.json — regenerate with tools/gen_renderer_files.py -->

# CNA renderers

CNA currently exposes **22 renderers**. You pick one at configure time with a single cache variable:

```bash
cmake -S . -B build -DCNA_GRAPHICS_RENDERER=OPENGLES3
```

If you set nothing, CNA chooses for you: `WEBGL2` on the web, `OPENGLES3` on Linux, `SDL_RENDERER` everywhere else.

## How to read this

**Platforms** are where the renderer is supported. cna-template refuses, with an explanation, to configure a renderer for a platform not listed here. A few renderers additionally have *experimental* platforms — CNA permits them but does not support them, so the template warns instead of refusing.

**Display** says whether the renderer opens a window. The three that do not (`HEADLESS`, `SOFTWARE`, `STUB`) need no X server, no GPU and no video driver, which makes them the right choice for CI and servers. Note that `SOFTWARE` genuinely rasterizes but presents nowhere.

**Scope** is `2D` or `2D+3D`. On a 2D-only renderer the 3D calls (`VertexBuffer`, `DrawUserPrimitives`, depth state) throw. Query `GraphicsDevice::SupportsCapability()` at runtime rather than testing the renderer's name.

**Tested by cna-template** describes this template's CI, not CNA's support: *every PR* is built and smoke-run on each push, *broad matrix* is the scheduled job, *on demand* is manual, and *not on CI runners* means a GitHub runner cannot provide the platform or SDK.

## SDL

| Renderer | Platforms | Scope | Display | Dependency | Tested | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `SDL_RENDERER` | Linux, Windows, macOS, Android (+ Web experimental) | 2D | window | none | every PR | Most portable renderer, and CNA's default off Linux. 2D only: no VertexBuffer/IndexBuffer/depth buffer. CNA documents it as untested on the web -- use WEBGL2 or CANVAS there. |
| `SDL_GPU` | Linux, Windows, macOS | 2D+3D | window | system libshaderc | broad matrix | SDL3's own GPU abstraction. Needs libshaderc installed (libshaderc-dev). |

## OpenGL family (shared EasyGL implementation)

| Renderer | Platforms | Scope | Display | Dependency | Tested | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `OPENGLES3` | Linux, Windows, macOS, Android | 2D+3D | window | sibling ../easy-gl (which needs ../meta-gl) | every PR | CNA's default renderer on Linux. Internally EasyGL. |
| `OPENGLES2` | Linux, Windows, macOS, Android | 2D+3D | window | sibling ../easy-gl (which needs ../meta-gl) | broad matrix | Internally EasyGL, GLES 2.0 profile. Cannot target the web — use WEBGL1. |
| `OPENGL33` | Linux, Windows, macOS | 2D+3D | window | sibling ../easy-gl (which needs ../meta-gl) | broad matrix | Internally EasyGL, desktop GL 3.3 core profile. |
| `WEBGL2` | Web | 2D+3D | window | sibling ../easy-gl (which needs ../meta-gl) | broad matrix | CNA's default renderer under Emscripten. Internally EasyGL. |
| `WEBGL1` | Web | 2D+3D | window | sibling ../easy-gl (which needs ../meta-gl) | broad matrix | WebGL 1 profile. Forcing MAX_WEBGL_VERSION=2 globally would break this. |

## OpenGL family (native, no EasyGL)

| Renderer | Platforms | Scope | Display | Dependency | Tested | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `OPENGL4` | Linux, Windows, macOS | 2D+3D | window | system OpenGL | broad matrix | Real desktop OpenGL 4.x core profile, independent of EasyGL. |

## Modern GPU APIs

| Renderer | Platforms | Scope | Display | Dependency | Tested | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `VULKAN` | Linux, Windows (+ Android experimental) | 2D+3D | window | system Vulkan SDK (find_package(Vulkan REQUIRED)) | broad matrix | Real 3D via Vulkan. Builds on a CI runner but needs a real or software ICD to run. |
| `WEBGPU` | Linux, Windows, macOS | 2D+3D | window | wgpu-native binary release, auto-downloaded | broad matrix | Despite the name this is a NATIVE renderer; CNA rejects it under Emscripten. |
| `METAL` | macOS | 2D+3D | window | Apple Metal framework | not on CI runners | macOS only. iOS/tvOS explicitly unvalidated upstream. |

## CPU / diagnostic

| Renderer | Platforms | Scope | Display | Dependency | Tested | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `HEADLESS` | Linux, Windows, macOS, Android (+ Web experimental) | 2D+3D | **none** | none | every PR | No GPU and no window at all. The right choice for CI and servers. |
| `SOFTWARE` | Linux, Windows, macOS, Android (+ Web experimental) | 2D+3D | **none** | none | every PR | Real CPU rasterizer. Creates no window and does not link SDL3, so Present() puts nothing on screen -- it is a compute/test renderer, not a display one. |
| `STUB` | Linux, Windows, macOS, Android (+ Web experimental) | 2D+3D | **none** | none | every PR | Deliberate no-op renderer. Draws nothing, touches no GPU or window. |

## Browser DOM

| Renderer | Platforms | Scope | Display | Dependency | Tested | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `CANVAS` | Web | 2D | window | none | broad matrix | Browser 2D canvas context. Needs no WebGL at all. |
| `HTML_DOM` | Web | 2D | window | none | broad matrix | Draws sprites as pooled CSS-transformed <div> elements. No WebGL. |
| `SVG_DOM` | Web | 2D | window | none | broad matrix | Draws sprites as real <svg>/<image> elements. No WebGL. |

## Portable middleware

| Renderer | Platforms | Scope | Display | Dependency | Tested | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `FNA3D` | Linux, Windows, macOS | 2D+3D | window | FNA3D + MojoShader via FetchContent; needs Python3 | broad matrix | The XNA-shaped C library FNA renders through. Picks SDL_GPU/D3D11/GL at runtime. |

## Legacy APIs

| Renderer | Platforms | Scope | Display | Dependency | Tested | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `GDI` | Windows | 2D | window | Win32 GDI | not on CI runners | Classic Win32 GDI, 2D only. |

## DirectX

| Renderer | Platforms | Scope | Display | Dependency | Tested | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `DIRECTX11` | Windows | 2D+3D | window | d3d11.h/dxgi.h | not on CI runners | Direct3D 11. |
| `DIRECTX12` | Windows | 2D+3D | window | d3d12.h/dxgi.h | not on CI runners | Direct3D 12. |
| `DIRECTX9` | Windows | 2D+3D | window | d3d9.h | not on CI runners | Direct3D 9. |

## Presets

Renderers in common use have a ready-made preset; the rest are selected with `-DCNA_GRAPHICS_RENDERER=<NAME>` against any preset or a plain build directory. Every one of the 22 renderers above is selectable either way.

```bash
cmake --list-presets                 # what is available
cmake --preset opengles3             # configure
cmake --build --preset opengles3     # build
ctest --preset opengles3             # smoke test
```

<sub>Cross-checked against CNA's own canonical list of 22 renderers at generation time.</sub>
