cmake_minimum_required(VERSION 3.25)
project(SdlImgViewer LANGUAGES CXX)

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
  # your sources...
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  SDL3::SDL3
  OpenGL::GL
#  Boost::system
)
