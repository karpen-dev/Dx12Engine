cmake_minimum_required(VERSION 3.16)
project(Dx12EngineCore VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	add_compile_definitions(DX12_ENGINE_EXPORTS)
endif()

find_path(D3D12_INCLUDE_DIR d3d12.h)
find_path(DXGI_INCLUDE_DIR dxgi.h)
find_path(D3DCOMPILER_INCLUDE_DIR d3dcompiler.h)

find_library(D3D12_LIBRARY d3d12.lib)
find_library(DXGI_LIBRARY dxgi.lib)
find_library(D3DCOMPILER_LIBRARY d3dcompiler.lib)
find_library(DXGUID_LIBRARY dxguid.lib)

add_library(Dx12EngineCore SHARED
		core/Types.hpp
		core/window/Window.cpp
		core/window/Window.hpp
		core/shader/Shader.hpp
		core/shader/Shader.cpp
		core/graphicsDevice/GraphicsDevice.cpp
		core/graphicsDevice/GraphicsDevice.hpp
		core/mesh/Mesh.hpp
		core/mesh/Mesh.cpp
		core/pipeline/Pipeline.hpp
		core/pipeline/Pipeline.cpp
		core/camera/Camera.hpp
		core/camera/Camera.cpp
		core/renderer/Renderer.hpp
		core/renderer/Renderer.cpp
		core/engine/Engine.hpp
		core/engine/Engine.cpp
		core/ConsoleLogger.hpp
		core/systemInfoLogger/SystemInfoLogger.h
		core/systemInfoLogger/SystemInfoLogger.cpp
		core/preloader/Preloader.hpp
		core/preloader/Preloader.cpp
		core/mesh/bufferManager/BufferManager.hpp
		core/mesh/bufferManager/BufferManager.cpp
		core/mesh/modelLoader/ModelLoader.hpp
		core/mesh/modelLoader/ModelLoader.cpp
		core/scene/transform/Transform.hpp
		core/scene/transform/Transform.cpp
		core/scene/meshRenderer/MeshRenderer.hpp
		core/scene/meshRenderer/MeshRenderer.cpp
		core/scene/Scene.hpp
		core/scene/Scene.cpp

		libs/imgui/imgui.cpp
		libs/imgui/imgui_draw.cpp
		libs/imgui/imgui_tables.cpp
		libs/imgui/imgui_widgets.cpp
		libs/imgui/imgui_demo.cpp

		libs/imgui/backends/imgui_impl_win32.cpp
		libs/imgui/backends/imgui_impl_dx12.cpp
		core/guiAPI/GuiAPI.cpp
		core/guiAPI/GuiAPI.hpp
)

target_include_directories(Dx12EngineCore PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
		${D3D12_INCLUDE_DIR}
		${DXGI_INCLUDE_DIR}
		${D3DCOMPILER_INCLUDE_DIR}

		${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui
		${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/backends
)

target_link_libraries(Dx12EngineCore
		${D3D12_LIBRARY}
		${DXGI_LIBRARY}
		${D3DCOMPILER_LIBRARY}
		${DXGUID_LIBRARY}
)

if(WIN32)
	target_compile_definitions(Dx12EngineCore PRIVATE DX12_ENGINE_EXPORTS)
	if(MSVC)
		set_target_properties(Dx12EngineCore PROPERTIES
				WINDOWS_EXPORT_ALL_SYMBOLS TRUE
		)
	endif()
endif()

if(MSVC)
	target_compile_options(Dx12EngineCore PRIVATE /EHsc /W4 /WX-)
	target_compile_definitions(Dx12EngineCore PRIVATE _CRT_SECURE_NO_WARNINGS)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
	target_compile_options(Dx12EngineCore PRIVATE -Wall -Wextra -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(Dx12EngineCore PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS Dx12EngineCore
		EXPORT Dx12EngineTargets
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib/static
		INCLUDES DESTINATION include
)

install(DIRECTORY core/
		DESTINATION include/Dx12Engine/core
		FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)

export(EXPORT Dx12EngineTargets
		FILE "${CMAKE_CURRENT_BINARY_DIR}/Dx12EngineTargets.cmake"
)