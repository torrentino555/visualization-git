cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(VisualizationGit)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -lm")

# перечисление папок с исходниками
LIST(APPEND SOURCE_DIRS . graphic)

FOREACH(dir ${SOURCE_DIRS})
  LIST(APPEND LIST_REG_FOR_FILES ${dir}/*.h ${dir}/*.cpp)
  include_directories(${dir})
ENDFOREACH()

file(GLOB project_files ${LIST_REG_FOR_FILES})

add_executable(${PROJECT_NAME} ${project_files})

target_link_libraries(${PROJECT_NAME} GL glfw GLEW)
