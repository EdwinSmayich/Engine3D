cmake_minimum_required(VERSION 3.20)
project(Engine3D LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Put all binaries in build/bin so the .exe and any DLLs sit together
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# ---------------------------------------------------------------------------
# GLFW (built from source in ThirdParty/glfw)
# ---------------------------------------------------------------------------
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(ThirdParty/glfw)

# ---------------------------------------------------------------------------
# GLAD 2.0 - the loader is generated at configure time (requires Python).
# REPRODUCIBLE = use the bundled spec instead of fetching it from Khronos.
# API gl:core=3.3 = OpenGL 3.3 Core. Change the version here if needed.
# ---------------------------------------------------------------------------
add_subdirectory(ThirdParty/glad/cmake glad_cmake)
glad_add_library(glad STATIC REPRODUCIBLE API gl:core=3.3)

# ---------------------------------------------------------------------------
# GLM (header-only, target glm::glm)
# ---------------------------------------------------------------------------
add_subdirectory(ThirdParty/glm)

# ---------------------------------------------------------------------------
# IMGUI
# ---------------------------------------------------------------------------
set(IMGUI_SRC
        ThirdParty/imgui/imgui.cpp
        ThirdParty/imgui/imgui_draw.cpp
        ThirdParty/imgui/imgui_tables.cpp
        ThirdParty/imgui/imgui_widgets.cpp
        ThirdParty/imgui/imgui_demo.cpp
        ThirdParty/imgui/ImGuizmo.cpp
        ThirdParty/imgui/backends/imgui_impl_glfw.cpp
        ThirdParty/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC
        ${IMGUI_SRC}
)

target_include_directories(imgui PUBLIC
        ThirdParty/imgui
        ThirdParty/imgui/backends
)

target_link_libraries(imgui PUBLIC
        glfw
        glad
)
# ---------------------------------------------------------------------------
# Assimp (built from source in ThirdParty/assimp)
# Turn off everything we don't need - a full build is slow and we only load
# a couple of model formats.
# ---------------------------------------------------------------------------
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)          # Static linking, without DLLs
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)

add_subdirectory(ThirdParty/assimp)

# Vendored third-party code is not ours to fix - silence its warnings so our own stay visible
foreach(DEP assimp zlibstatic)
    if (TARGET ${DEP})
        target_compile_options(${DEP} PRIVATE -w)
    endif()
endforeach()

# ---------------------------------------------------------------------------
# Engine executable
# ---------------------------------------------------------------------------
add_executable(Engine3D Engine3D/Engine3D.cpp
        Engine3D/Renderer/Shader.h
        Engine3D/Renderer/Shader.cpp
        Engine3D/Renderer/Mesh.h
        Engine3D/Renderer/Mesh.cpp
        Textures/stb_image.h
        Textures/std_image.cpp
        Engine3D/Camera/Camera.h
        Engine3D/Camera/Camera.cpp
        Engine3D/Core/DebugSettings.h
        Engine3D/ImGui/ImGuiLayer.h
        Engine3D/ImGui/ImGuiLayer.cpp
        Engine3D/Core/Light.h
        Engine3D/Core/AppContext.h
        Engine3D/Core/AppContext.cpp
        Engine3D/Core/Materials.h
        Engine3D/Core/SceneObject.h
        Engine3D/Math/Math.h
        Engine3D/Math/Math.cpp
        Engine3D/Math/Transform.h
        Engine3D/Math/Transform.cpp)

target_link_libraries(Engine3D PRIVATE
        glfw
        glad
        glm::glm
        imgui
        assimp)

# Make Engine3D the startup project in generated Visual Studio solutions
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Engine3D)

# ---------------------------------------------------------------------------
# Inject the absolute path of the source Shaders/ folder as the SHADER_DIR macro.
# The program loads shaders straight from the source tree, so editing a .vs/.fs
# and re-running applies the change instantly - no copying, no CMake cache reset.
# (For a portable/shippable build later, copy shaders next to the .exe instead.)
# ---------------------------------------------------------------------------
target_compile_definitions(Engine3D PRIVATE GLM_ENABLE_EXPERIMENTAL)
target_compile_definitions(Engine3D PRIVATE SHADER_DIR="${CMAKE_SOURCE_DIR}/Shaders")
target_compile_definitions(Engine3D PRIVATE TEXTURE_DIR="${CMAKE_SOURCE_DIR}/Textures/Images")
