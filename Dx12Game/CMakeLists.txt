cmake_minimum_required(VERSION 3.16)
project(Game)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DX12_ENGINE_PATH "../Dx12Engine" CACHE PATH "Path to Dx12Engine source")

if(NOT TARGET Dx12EngineCore)
	add_subdirectory(${DX12_ENGINE_PATH} ${CMAKE_CURRENT_BINARY_DIR}/Dx12Engine_build)
endif()

add_executable(Game
		main.cpp
		game/GameScene.hpp
		game/gameObject/GameObject.hpp
)

target_include_directories(Game PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Game
		Dx12EngineCore
)

if(WIN32)
	add_custom_command(TARGET Game POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:Dx12EngineCore>
			$<TARGET_FILE_DIR:Game>
			COMMENT "Copying Dx12Engine.dll to output directory"
	)
endif()

if(MSVC)
	target_compile_options(Game PRIVATE /EHsc /W4)
endif()