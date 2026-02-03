cmake_minimum_required(VERSION 3.25)
project(SdlImgViewer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
#find_package(Boost REQUIRED COMPONENTS system)

find_program(GLAD_EXECUTABLE glad REQUIRED)
execute_process(
  COMMAND "${GLAD_EXECUTABLE}"
          --api "gl:core=3.3"
          --out-path "src/glad"
          c
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  RESULT_VARIABLE GLAD_RESULT
)

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/MainWindow.hpp
  src/MainWindow.cpp
  src/FileTreeView.hpp src/FileTreeView.cpp
  # your sources...
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/glad/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)


add_library(imgui STATIC
  external/imgui/imgui.h
  external/imgui/imgui.cpp
  external/imgui/imgui_draw.cpp
  external/imgui/imgui_tables.cpp
  external/imgui/imgui_widgets.cpp

  external/imgui/backends/imgui_impl_sdl3.cpp
  external/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)

add_library(glad STATIC
  ${CMAKE_SOURCE_DIR}/src/glad/include/glad/gl.h
  ${CMAKE_SOURCE_DIR}/src/glad/src/gl.c
)

target_include_directories(glad PUBLIC
  ${CMAKE_SOURCE_DIR}/src/glad/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  SDL3::SDL3
  OpenGL::GL
  imgui
  glad
#  Boost::system
)
