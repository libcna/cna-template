# Výsledek sestavení cna-template

> **Historická zpráva (2026-09-28, CNA `d5cf852`).** Matice níže vznikla před kurátorstvím
> rendererů v CNA (RRC-018) a uvádí i renderery, které CNA od té doby odstranilo (GDI, DirectX 12,
> OPENGLES2, WEBGL1, CANVAS, HTML_DOM, SVG_DOM, PORTABLEGL, LLGL, FREEDIRECT, …). Šablona dnes
> podporuje přesně 14 veřejných rendererů CNA; aktuální stav je v `NEXT.md` a `CHANGELOG.md`.

Datum: 2026-09-28. Z 22 aktuálních rendererů CNA byl podle zadání vynechán METAL.
**21 z 21 požadovaných rendererů se úspěšně sestavilo a prošlo smoke testem.**
Po dodatečném povolení zásahů do CNA byly opraveny dva blokátory kompilace
DIRECTX11/12 a následně výslovně schválený blokátor linkování reflexe shaderů.
Opravy jsou v CNA commitu `d5cf852212fb2d3a6930dfb05d95ee96be5aaa24` (DX-271).
Sharp-runtime nebyl změněn; oba checkouts jsou po CNA commitu čisté.

## Použité revize a nástroje

- CNA, původní Linux/Web/GDI/DX9 matice: `92d23c84d12725c97872e3eaf249f789ff7579b6`
- CNA, nové DX11/DX12 sestavení: `d5cf852212fb2d3a6930dfb05d95ee96be5aaa24`
- sharp-runtime: `007280bd1cc789f851f7f454a5041c8ce2479e13`
- easy-gl: `deda7a426c3c166c0e03a4790f1ede610e2e46fb`
- meta-gl: `20c8b2dc5bb80e32706784066db9fd9e15b3f46a`
- CMake 3.31.6, GCC 14.2.0, MinGW GCC 14-win32, mingw-w64 headers 12.0.0-5, Wine 10.0.
- Release sestavení, sdílená ccache `/rv/cnaccache`, `CCACHE_BASEDIR=/rv`.

Piny v `dependencies.lock` a CI nyní používají opravený CNA commit
`d5cf852212fb2d3a6930dfb05d95ee96be5aaa24`. Původních 19 rendererů bylo
ověřeno na původní revizi, nové DX11/DX12 sestavení na opravené revizi;
Linux/Web/GDI/DX9 matice nebyla znovu celá sestavena na novém CNA pinu.

## Matice

| Renderer | Cíl | Sestavení | Smoke test |
| --- | --- | --- | --- |
| SDL_RENDERER | Linux | OK | OK, 3 snímky |
| SDL_GPU | Linux | OK | OK, 3 snímky |
| OPENGLES3 | Linux | OK | OK, 3 snímky |
| OPENGLES2 | Linux | OK | OK, 3 snímky |
| OPENGL33 | Linux | OK | OK, 3 snímky |
| WEBGL2 | Web / Emscripten | OK | OK, Chrome, 3 snímky |
| WEBGL1 | Web / Emscripten | OK | OK, Chrome, 3 snímky |
| OPENGL4 | Linux | OK | OK, 3 snímky |
| VULKAN | Linux | OK | OK, 3 snímky |
| WEBGPU | Linux | OK | OK, 3 snímky |
| HEADLESS | Linux | OK | OK, 3 snímky |
| SOFTWARE | Linux | OK | OK, 3 snímky |
| STUB | Linux | OK | OK, 3 snímky |
| CANVAS | Web / Emscripten | OK | OK, Chrome, 3 snímky |
| HTML_DOM | Web / Emscripten | OK | OK, Chrome, 3 snímky |
| SVG_DOM | Web / Emscripten | OK | OK, Chrome, 3 snímky |
| FNA3D | Linux | OK | OK, 3 snímky |
| GDI | Windows / MinGW | OK | OK, Wine, 3 snímky |
| DIRECTX11 | Windows / MinGW | OK po opravě CNA | OK, Wine + DXVK 2.6, 3 snímky |
| DIRECTX12 | Windows / MinGW | OK po opravě CNA | OK, Wine + vkd3d-proton 3.1 + DXVK DXGI, 3 snímky |
| DIRECTX9 | Windows / MinGW | OK | OK, Wine, 3 snímky |

## Výstupy a spuštění

- Linux: `build/HelloGame`, sdílený runtime `build/CNA/libcna.so`; 12 rendererů v jedné binárce.
- Windows GDI/DX9: `build-consumer/HelloGame-gdi-directx9.exe`.
- Windows DX11/DX12: `build-consumer/HelloGame-directx11-directx12.exe` a stejná kopie `build-consumer/HelloGame.exe`; výchozí renderer DIRECTX11.
- Windows společné soubory: `build-consumer/Content/` a MinGW runtime DLL vedle aplikací. Použito `CNA_PLATFORM=WIN32`, `CNA_AUDIO_PLATFORM=NULL`, `CNA_ENABLE_SDL=OFF`, `CNA_ENABLE_NET=OFF`.
- Web: `build-probe/HelloGame.js`, `.wasm`, `.data` a pět spouštěcích stránek `HelloGame-WEBGL1.html`, `HelloGame-WEBGL2.html`, `HelloGame-CANVAS.html`, `HelloGame-HTML_DOM.html`, `HelloGame-SVG_DOM.html`. Každá nastavuje svůj renderer před inicializací.

Příklady z kořene repozitáře:

```bash
CNA_GRAPHICS_RENDERER=VULKAN ./build/HelloGame
CNA_GRAPHICS_RENDERER=GDI wine ./build-consumer/HelloGame-gdi-directx9.exe --smoke-test
CNA_GRAPHICS_RENDERER=DIRECTX9 WINEDLLOVERRIDES=d3d9=b wine ./build-consumer/HelloGame-gdi-directx9.exe --smoke-test
python3 -m http.server --directory build-probe 8000
```

Windows testy běžely ve vlastním Xvfb serveru s existujícím prefixem
`/home/robertvokac/deps/wineprefix-cna-dxvk`. DIRECTX9 prošel s vestavěným Wine D3D9
(`WINEDLLOVERRIDES=d3d9=b`). Varianta s nativním DXVK skončila kódem 1 po inicializaci;
příčina tohoto selhání nebyla potvrzena. Stejná binárka prošla přes Wine D3D9.

Webové testy v headless Chrome/SwiftShader ověřily skutečný renderer, tři vykreslené
snímky a návrat z `Game::Run()`. Výsledky jsou v `build-probe/browser-smoke-results.json`.
Linux: `build/linux-smoke.log`. Windows: `build/windows-gdi-smoke.log` a
`build/windows-directx9-wined3d-smoke.log`.

## Nalezené chyby CNA — aktuální stav

### 1. Kombinace GDI a DirectX 11/12 s výchozím GDI

`../cna/modules/renderers/CMakeLists.txt:127` přidává `common/d3d` pouze podle
výchozího `CNA_GRAPHICS_RENDERER`, nikoli podle všech členů `CNA_GRAPHICS_RENDERERS`.

Konfigurace `CNA_GRAPHICS_RENDERER=GDI` a
`CNA_GRAPHICS_RENDERERS=GDI;DIRECTX9;DIRECTX11;DIRECTX12` projde konfigurací,
ale kompilace DirectX 12 nenajde existující hlavičky společného modulu:

```text
fatal error: CNA/Internal/Renderers/D3DCommon/ID3DDeviceRecoverableEXT.hpp: No such file or directory
fatal error: CNA/Internal/Renderers/D3DCommon/D3DProgramReflection.hpp: No such file or directory
```

Důkaz: `build/windows-gdi-dx-combined-build.log`.
Pokračovalo se nezávislou konfigurací DirectX a následně kombinací GDI + DIRECTX9.

### 2. DirectX 12: ne-constexpr operátory D3D12 příznaků v MinGW — opraveno

Po výběru výchozího DIRECTX11 je D3DCommon dostupný, ale DirectX 12 selže na
konstantních výrazech používajících přetížený `operator|`, který není v místních
MinGW hlavičkách `constexpr`:

```text
error: call to non-‘constexpr’ function ‘D3D12_RESOURCE_STATES operator|(D3D12_RESOURCE_STATES, D3D12_RESOURCE_STATES)’
error: call to non-‘constexpr’ function ‘D3D12_FORMAT_SUPPORT1 operator|(D3D12_FORMAT_SUPPORT1, D3D12_FORMAT_SUPPORT1)’
```

Dotčené soubory v `../cna/modules/renderers/directx12/src/`:

- `D3D12ComputeShader.cpp:37`
- `D3D12StorageTexture2D.cpp:16`
- `D3D12Texture2DArray.cpp:17`
- `DirectX12Renderer.cpp:3117`

Původní důkaz: `build/windows-directx-build.log`. Oprava přetypuje oba operandy
na `int` před bitovým součtem; výsledný typ, příznaky a `constexpr` zůstávají zachovány.

### 3. DirectX 11: konstanta chybějící v MinGW hlavičkách — opraveno

`../cna/modules/renderers/directx11/include/CNA/Internal/Renderers/DirectX11/DirectX11Renderer.hpp:362`
používá konstantu, kterou zdejší MinGW hlavičky `d3d11.h` nedefinují:

```text
error: ‘D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT’ was not declared in this scope
```

Selhávají mimo jiné `D3D11RenderTargets.cpp`, `D3D11SpriteBatch.cpp`,
`D3D11Textures.cpp` a `DirectX11Renderer.cpp`.
Původní důkaz: `build/windows-dx9-dx11-build.log`. Oprava používá vlastní konstantu
limitu 32 místo makra chybějícího v SDK, v souladu s
[oficiální specifikací D3D11](https://microsoft.github.io/DirectX-Specs/d3d/archive/D3D11_3_FunctionalSpec.htm).

V sharp-runtime nebyla během této matice zjištěna chyba blokující sestavení.

## Původní změny v cna-template

- Manifest, generované presety a dokumentace byly sladěny s aktuálními 22 identitami CNA.
- Přidány presety GDI a DIRECTX9; build adresáře se sdílejí podle cílové platformy.
- Validace i smoke testy podporují seznam rendererů v jedné binárce.
- Windows konfigurace bez SDL už nevyžaduje import a kopírování SDL knihoven.
- WebGL 1 a 2 mohou sdílet webové sestavení.
- Externí webová aplikace explicitně linkuje dokumentovaný `CNA::EmscriptenAsyncify`.
  Původní `ReferenceError: Asyncify is not defined` byl problém šablony a byl opraven zde.
- Dependency pins a CI používají stejné aktuální revize.

Ověření šablony: generátor `--check`, shoda dependency pins s CI, `git diff --check`.


## Dodatečný blokátor linkování — opraveno

Po opravě kompilace linker ohlásil chybějící specializaci
`__mingw_uuidof<ID3D11ShaderReflection>()` z `D3D11EffectRenderer.cpp` a
`D3D12EffectRenderer.cpp` (v druhém případě přes `IID_PPV_ARGS`).
Po výslovném schválení byl existující IID ze společného `D3DProgramReflection.cpp`
přesunut do `D3DShaderReflectionIid.hpp` a použit ve všech třech voláních `D3DReflect`.
Zůstává zachován výběr IID podle verze D3D compiler ABI.

Úplné sestavení a linkování: `build/windows-dx11-dx12-fixed-link-build.log`.
První kompilace opravených modulů: `build/windows-dx11-dx12-fixed-build.log`;
její následná chyba linkování je historický záznam před opravou IID.

## Nové Wine ověření DX11/DX12

Oba testy běžely přes `../cna/tools/platform/run_gpu_tests_private.sh --exec`
na soukromém Weston/Xwayland displeji s GPU. Každý potvrdil vybraný renderer,
tři vykreslené snímky, návrat z `Game::Run()` a exit 0.

- DX11: prefix `/home/robertvokac/.wine-cna-d3d11`, DXVK 2.6.0.
  Log: `build/windows-directx11-fixed-smoke.log`.
- DX12: prefix `/home/robertvokac/.wine-cna-d3d12`, vkd3d-proton 3.1.0,
  DXVK 2.6.0 DXGI, `WINEDLLOVERRIDES=d3d12,d3d12core,dxgi=n`.
  Log: `build/windows-directx12-dxvk-dxgi-smoke.log`.

Starý DX12 prefix obsahuje vestavěné Wine DXGI. To při kombinaci s nativním
vkd3d-proton spadlo uvnitř `dxgi`/`wined3d` při vytváření swapchainu. Pouhé
nastavení native override pak skončilo chybou načtení DLL. Úspěšný test proto
použil dočasný symlink `build-consumer/dxgi.dll` na
`/usr/lib/dxvk/wine64/dxgi.dll.so`; po testu byl odstraněn. Wine prefix se neměnil.
Toto je podmínka místního Wine prostředí; uvedené smoke testy nenahrazují
plnou kvalifikaci na skutečných Windows.

Příklad zopakování DX12 testu z kořene šablony (symlink vytvořit jen pokud neexistuje):

```bash
ln -s /usr/lib/dxvk/wine64/dxgi.dll.so build-consumer/dxgi.dll
../cna/tools/platform/run_gpu_tests_private.sh --exec env \
  WINEPREFIX=/home/robertvokac/.wine-cna-d3d12 WINEDEBUG=-all \
  WINEDLLOVERRIDES=d3d12,d3d12core,dxgi=n CNA_GRAPHICS_RENDERER=DIRECTX12 \
  wine ./build-consumer/HelloGame-directx11-directx12.exe --smoke-test
rm build-consumer/dxgi.dll
```
