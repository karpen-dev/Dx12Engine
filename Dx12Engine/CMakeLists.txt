set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_path(D3D12_INCLUDE_DIR d3d12.h)
find_path(DXGI_INCLUDE_DIR dxgi.h)
find_path(D3DCOMPILER_INCLUDE_DIR d3dcompiler.h)

find_library(D3D12_LIBRARY d3d12.lib)
find_library(DXGI_LIBRARY dxgi.lib)
find_library(D3DCOMPILER_LIBRARY d3dcompiler.lib)
find_library(DXGUID_LIBRARY dxguid.lib)

add_executable(Dx12Engine
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
		main.cpp
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
		api/player/Player.hpp
		api/player/Player.cpp
)

target_include_directories(Dx12Engine PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
		${D3D12_INCLUDE_DIR}
		${DXGI_INCLUDE_DIR}
		${D3DCOMPILER_INCLUDE_DIR}
)

target_link_libraries(Dx12Engine
		${D3D12_LIBRARY}
		${DXGI_LIBRARY}
		${D3DCOMPILER_LIBRARY}
		${DXGUID_LIBRARY}
)

set_target_properties(Dx12Engine PROPERTIES LINKER_LANGUAGE CXX)

if(MSVC)
	target_compile_options(Dx12Engine PRIVATE /EHsc /W4)
endif()