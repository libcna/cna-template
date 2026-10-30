# =============================================================================
# cna-template — a starter project for the CNA game framework.
# =============================================================================
# Everything renderer-specific in this file is driven by cmake/renderers.json
# (see cmake/CnaRenderers.cmake). There is deliberately no per-renderer
# if/elseif chain here: CNA's renderer set changes (it currently has 10 public
# identities), so the template
# treats the renderer set as data rather than as code.
#
# CMake 3.23 is the floor because the easy-gl/meta-gl siblings backing the
# OPENGLES3/OPENGL33/WEBGL2 renderers require it (FILE_SET
# HEADERS). CNA itself only needs 3.20, but a template that could not build the
# GL family would be a poor starting point.
cmake_minimum_required(VERSION 3.23)

project(CnaTemplate VERSION 0.1.0 LANGUAGES C CXX)

# --- Release identity (docs/releasing.md) -------------------------------------
# project(VERSION) accepts numeric components only, so the Semantic Versioning
# pre-release identifier lives beside it and is joined back on here. A normal
# variable, not a cache entry: a cached copy would keep an already-configured
# build directory reporting the previous release after a bump.
# CNA_TEMPLATE_VERSION_STRING is the single spelling the rest of the build reads.
set(CNA_TEMPLATE_VERSION_PRERELEASE "")
if(CNA_TEMPLATE_VERSION_PRERELEASE)
    set(CNA_TEMPLATE_VERSION_STRING "${PROJECT_VERSION}-${CNA_TEMPLATE_VERSION_PRERELEASE}")
else()
    set(CNA_TEMPLATE_VERSION_STRING "${PROJECT_VERSION}")
endif()
message(STATUS "cna-template: version ${CNA_TEMPLATE_VERSION_STRING}")

# --- Customization point: rename your application here -----------------------
# Drives the executable name and the CTest name. Renaming the C++ class itself
# is a separate step -- see "Making this your own project" in README.md.
set(CNA_TEMPLATE_APP_NAME "HelloGame" CACHE STRING
    "Name of the application executable produced by this template")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CnaRenderers)
include(CnaTemplateVersion)
include(CnaTemplateDependencies)

set(CNA_TEMPLATE_CNA_MODE "SOURCE" CACHE STRING
    "How to consume CNA: SOURCE sibling checkout or installed PACKAGE")
set_property(CACHE CNA_TEMPLATE_CNA_MODE PROPERTY STRINGS SOURCE PACKAGE)
string(TOUPPER "${CNA_TEMPLATE_CNA_MODE}" CNA_TEMPLATE_CNA_MODE)
if(NOT CNA_TEMPLATE_CNA_MODE MATCHES "^(SOURCE|PACKAGE)$")
    message(FATAL_ERROR
        "cna-template: CNA_TEMPLATE_CNA_MODE must be SOURCE or PACKAGE, got "
        "'${CNA_TEMPLATE_CNA_MODE}'.")
endif()

message(STATUS "cna-template: ${PROJECT_NAME} -> ${CNA_TEMPLATE_APP_NAME}")
message(STATUS "cna-template: platform ${CMAKE_SYSTEM_NAME}, "
               "compiler ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

# -----------------------------------------------------------------------------
# CNA framework consumption
# -----------------------------------------------------------------------------
if(CNA_TEMPLATE_CNA_MODE STREQUAL "SOURCE")
    set(CNA_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../cna" CACHE PATH
        "Path to the CNA framework checkout")
    if(NOT EXISTS "${CNA_ROOT_DIR}/CMakeLists.txt")
        message(FATAL_ERROR
            "cna-template: CNA framework not found at '${CNA_ROOT_DIR}'.\n"
            "Clone CNA next to this repository, pass -DCNA_ROOT_DIR=/path/to/cna,\n"
            "or use -DCNA_TEMPLATE_CNA_MODE=PACKAGE with an installed CNA SDK.")
    endif()

    foreach(_renderer IN LISTS CNA_GRAPHICS_RENDERERS)
        cna_template_validate_renderer("${_renderer}" "${CNA_ROOT_DIR}")
    endforeach()
    if(DEFINED CNA_GRAPHICS_RENDERER AND NOT CNA_GRAPHICS_RENDERER STREQUAL "")
        cna_template_validate_renderer("${CNA_GRAPHICS_RENDERER}" "${CNA_ROOT_DIR}")
        message(STATUS "cna-template: renderer ${CNA_GRAPHICS_RENDERER} (requested)")
    else()
        message(STATUS "cna-template: no renderer requested; using CNA's platform default")
    endif()

    set(CNA_BUILD_TESTS       OFF CACHE BOOL "Build CNA tests"                FORCE)
    set(CNA_BUILD_EXAMPLES    OFF CACHE BOOL "Build CNA example applications" FORCE)
    set(EASYGL_BUILD_TESTS    OFF CACHE BOOL "Build easy-gl tests"            FORCE)
    set(EASYGL_BUILD_EXAMPLES OFF CACHE BOOL "Build easy-gl examples"         FORCE)

    # Keep CNA's persistent SDL cache outside an immutable dependency checkout.
    if(NOT DEFINED CNA_SDL_PREBUILT_ROOT)
        if(EMSCRIPTEN)
            set(_cna_sdl_cache_target emscripten)
        else()
            set(_cna_sdl_cache_target
                "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_C_COMPILER_ID}")
        endif()
        set(CNA_SDL_PREBUILT_ROOT
            "${CMAKE_CURRENT_SOURCE_DIR}/build/cna-sdl-prebuilt-${_cna_sdl_cache_target}"
            CACHE PATH "Persistent CNA SDL build cache outside the CNA checkout")
    endif()

    add_subdirectory("${CNA_ROOT_DIR}" CNA)

    cna_template_require_sibling_version(CNA "${CNA_ROOT_DIR}"
        "${CNA_BINARY_DIR}/generated/include/CNA/Version.hpp"
        CNA_VERSION_STRING "${CNA_TEMPLATE_REQUIRED_CNA_VERSION}")
    cna_template_require_sibling_version(sharp-runtime "${CNA_SHARP_RUNTIME_ROOT}"
        "${SHARP_RUNTIME_BINARY_DIR}/generated/include/SharpRuntime/Version.hpp"
        SHARP_RUNTIME_VERSION_STRING "${CNA_TEMPLATE_REQUIRED_SHARP_RUNTIME_VERSION}")
    if(TARGET easy-gl)
        cna_template_require_sibling_version(easy-gl "${easy-gl_SOURCE_DIR}"
            "${easy-gl_BINARY_DIR}/include/easygl/Version.hpp"
            EASYGL_VERSION_STRING "${CNA_TEMPLATE_REQUIRED_EASYGL_VERSION}")
        cna_template_require_sibling_version(meta-gl "${meta-gl_SOURCE_DIR}"
            "${meta-gl_BINARY_DIR}/include/metagl/Version.hpp"
            METAGL_VERSION_STRING "${CNA_TEMPLATE_REQUIRED_METAGL_VERSION}")
    endif()

    cna_template_assert_no_drift()
    cna_template_validate_renderer("${CNA_GRAPHICS_RENDERER}" "${CNA_ROOT_DIR}")
    set(_cna_template_renderer "${CNA_GRAPHICS_RENDERER}")
    set(_cna_template_link_target CNA)
else()
    find_package(CNA CONFIG REQUIRED)
    if(NOT TARGET CNA::CNA)
        message(FATAL_ERROR "cna-template: installed CNA package does not export CNA::CNA")
    endif()
    if(NOT COMMAND cna_deploy_runtime)
        message(FATAL_ERROR
            "cna-template: installed CNA package predates cna_deploy_runtime(); "
            "install a newer CNA C++ SDK")
    endif()
    if(DEFINED CNA_RUNTIME_VERSION AND
       CNA_RUNTIME_VERSION VERSION_LESS CNA_TEMPLATE_REQUIRED_CNA_VERSION)
        message(FATAL_ERROR
            "cna-template: CNA runtime ${CNA_RUNTIME_VERSION} is older than required "
            "${CNA_TEMPLATE_REQUIRED_CNA_VERSION}")
    endif()

    set(CNA_TEMPLATE_PACKAGE_RENDERER "" CACHE STRING
        "Renderer identity compiled into the installed CNA SDK (descriptive, not a selector)")
    if(CNA_TEMPLATE_PACKAGE_RENDERER STREQUAL "")
        message(FATAL_ERROR
            "cna-template: PACKAGE mode needs -DCNA_TEMPLATE_PACKAGE_RENDERER=<NAME>.\n"
            "The installed SDK is already compiled for that renderer; this value only describes it.")
    endif()
    cna_template_validate_renderer("${CNA_TEMPLATE_PACKAGE_RENDERER}" "")
    set(_cna_template_renderer "${CNA_TEMPLATE_PACKAGE_RENDERER}")
    set(_cna_template_link_target CNA::CNA)
endif()

if(NOT COMMAND cna_deploy_runtime)
    message(FATAL_ERROR
        "cna-template: selected CNA revision predates cna_deploy_runtime(); use a newer CNA")
endif()

cna_template_renderer_field("${_cna_template_renderer}" display
    CNA_TEMPLATE_RENDERER_DISPLAY)
cna_template_renderer_field("${_cna_template_renderer}" dimension
    CNA_TEMPLATE_RENDERER_DIMENSION)
message(STATUS "cna-template: renderer ${_cna_template_renderer} "
               "(${CNA_TEMPLATE_RENDERER_DIMENSION}, display: ${CNA_TEMPLATE_RENDERER_DISPLAY})")

# -----------------------------------------------------------------------------
# The application
# -----------------------------------------------------------------------------
set(CNA_TEMPLATE_SOURCES
    game/src/HelloGame.cpp
    game/src/Program.cpp
)

# On Android the "executable" is a shared library that SDL's Java glue loads by
# name, so the target name is fixed regardless of CNA_TEMPLATE_APP_NAME.
if(ANDROID)
    set(_app main)
    add_library(${_app} SHARED ${CNA_TEMPLATE_SOURCES})
else()
    set(_app ${CNA_TEMPLATE_APP_NAME})
    add_executable(${_app} ${CNA_TEMPLATE_SOURCES})
endif()

target_include_directories(${_app} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/game/include"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/include")   # CnaTemplate/Version.hpp

# CNA propagates CNA_RENDERER_<X> to consumers through its cna_build_flags
# INTERFACE, but applies CNA_GL_PROFILE_<X> with a directory-scoped
# add_compile_definitions() that never reaches a parent-scope consumer. The three
# GL renderers all share the define CNA_RENDERER_EASYGL, so without the profile
# CNA::getCurrentGraphicsRendererName() falls through to its OPENGLES3 default
# and misreports OPENGL33/WEBGL2 in *our* translation units
# while CNA's own report the truth. Re-applying it here restores agreement.
# Upstream fix recorded in missing.md.
cna_template_renderer_field("${_cna_template_renderer}" category _renderer_category)
if(_renderer_category STREQUAL "gl-easygl")
    target_compile_definitions(${_app} PRIVATE "CNA_GL_PROFILE_${_cna_template_renderer}")
endif()

# CNA is an INTERFACE umbrella already carrying every framework module, the
# selected renderer target, the required Sharp Runtime component targets, the
# public compile definitions and the public include directories. Linking either
# a renderer or the legacy all-components SHARP_RUNTIME umbrella by hand is
# obsolete. So is the old -Wl,--start-group wrapper: CNA declares its
# static-library cycles on the targets themselves, and a linker group around an
# INTERFACE library expands to nothing anyway.
target_link_libraries(${_app} PRIVATE ${_cna_template_link_target})

# -----------------------------------------------------------------------------
# Platform specifics
# -----------------------------------------------------------------------------
if(ANDROID)
    # Assets are packaged into the APK -- see android/app/build.gradle.

elseif(EMSCRIPTEN)
    set_target_properties(${_app} PROPERTIES SUFFIX ".html")

    # CNA publishes its exception ABI (-fexceptions, DISABLE_EXCEPTION_CATCHING=0)
    # through its own targets, so this executable inherits it; forcing
    # -fwasm-exceptions here, as the template did for the August CNA, now fails
    # the link. Asyncify is the one part a final executable opts into itself:
    # Game::Run() blocks, and CNA::EmscriptenAsyncify is what lets it yield to
    # the browser (CNA docs/emscripten-mainloop-game-lifetime.md).
    target_link_libraries(${_app} PRIVATE CNA::EmscriptenAsyncify)

    target_link_options(${_app} PRIVATE
        -sALLOW_MEMORY_GROWTH=1
        -sSTACK_SIZE=1048576
        -sINITIAL_MEMORY=134217728
        -sFORCE_FILESYSTEM=1
        # Let main() actually return, so --smoke-test terminates the process.
        -sEXIT_RUNTIME=1
        "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/Content@/Content")

    # WebGL version constraints are per renderer, not global: pinning
    # MIN/MAX_WEBGL_VERSION=2 for every web build would bind every future web
    # renderer to WebGL 2, whatever its manifest says.
    cna_template_renderer_field("${_cna_template_renderer}" webflags _webflags)
    if(_webflags)
        target_link_options(${_app} PRIVATE ${_webflags})
    endif()

else()
    # Native desktop: put Content/ next to the executable so the relative paths
    # the game uses at runtime resolve.
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Content")
        add_custom_command(TARGET ${_app} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/Content"
                "$<TARGET_FILE_DIR:${_app}>/Content"
            COMMENT "Copying game content next to ${_app}")
    endif()
endif()

if(NOT ANDROID)
    cna_deploy_runtime(${_app})
endif()

# -----------------------------------------------------------------------------
# Tests
# -----------------------------------------------------------------------------
option(CNA_TEMPLATE_BUILD_TESTS "Register cna-template's own smoke test" ON)

if(CNA_TEMPLATE_BUILD_TESTS AND NOT ANDROID AND NOT EMSCRIPTEN AND NOT CMAKE_CROSSCOMPILING)
    enable_testing()
    add_test(NAME ${CNA_TEMPLATE_APP_NAME}Smoke COMMAND ${_app} --smoke-test)

    # Label the test by what it needs from the environment, so CI can run the
    # display-free renderers with no X server at all and reserve xvfb-run for
    # the ones that genuinely open a window:
    #   ctest -L headless               # no display required
    #   xvfb-run -a ctest -L display
    if(CNA_TEMPLATE_RENDERER_DISPLAY STREQUAL "none")
        set(_labels headless)
    else()
        set(_labels display)
    endif()

    set_tests_properties(${CNA_TEMPLATE_APP_NAME}Smoke PROPERTIES
        TIMEOUT 30
        LABELS "${_labels}")

    if(CNA_TEMPLATE_CNA_MODE STREQUAL "SOURCE")
        foreach(_renderer IN LISTS CNA_GRAPHICS_RENDERERS)
            cna_template_renderer_field("${_renderer}" display _display)
            if(_display STREQUAL "none")
                set(_labels headless)
            else()
                set(_labels display)
            endif()
            add_test(NAME ${CNA_TEMPLATE_APP_NAME}Smoke_${_renderer}
                COMMAND ${_app} --smoke-test)
            set_tests_properties(${CNA_TEMPLATE_APP_NAME}Smoke_${_renderer} PROPERTIES
                ENVIRONMENT "CNA_GRAPHICS_RENDERER=${_renderer}"
                TIMEOUT 30 LABELS "${_labels}")
        endforeach()
    endif()
endif()
