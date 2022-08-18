cmake_minimum_required (VERSION 3.19)

project(growl-thirdparty::imgui LANGUAGES OBJCXX)

set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

set(IMGUI_SOURCES
	${SOURCE_PREFIX}/imgui.cpp
	${SOURCE_PREFIX}/imgui_tables.cpp
	${SOURCE_PREFIX}/imgui_widgets.cpp
	${SOURCE_PREFIX}/imgui_draw.cpp
	${SOURCE_PREFIX}/imgui_demo.cpp
	)

set(IMGUI_BACKEND_SOURCES
	${SOURCE_PREFIX}/backends/imgui_impl_sdl.cpp
	${SOURCE_PREFIX}/backends/imgui_impl_metal.mm
	)

set(SDL2_BUILDING_LIBRARY 1)
find_package(SDL2 REQUIRED)

add_library(imgui ${IMGUI_SOURCES} ${IMGUI_BACKEND_SOURCES})
target_link_libraries(imgui PRIVATE ${SDL2_LIBRARY})
target_include_directories(
	imgui
	PUBLIC
	${SOURCE_PREFIX}
	${SOURCE_PREFIX}/backends
	PRIVATE
	${SDL2_INCLUDE_DIR}
	)
