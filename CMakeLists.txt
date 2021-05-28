cmake_minimum_required(VERSION 3.20)
project(OpenVice VERSION 1.0)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(./glfw_source)
add_subdirectory(./cglm_source)
add_subdirectory(./glew_source/build/cmake)

find_package(OpenGL REQUIRED)

include_directories(include)
include_directories(./glew_source/include)

set(HEADERS ./include/file_img.h ./include/file_dir.h ./include/file_dff.h ./include/window.h ./include/camera.h)
set(SOURCES ./src/main.c ./src/file_img.c ./src/file_dir.c ./src/file_dff.c ./src/window.c ./src/camera.c)

add_executable(OpenVice ${SOURCES} ${HEADERS})

target_link_libraries(OpenVice glfw)
target_link_libraries(OpenVice glew)
target_link_libraries(OpenVice OpenGL::GL)
target_link_libraries(OpenVice cglm)
