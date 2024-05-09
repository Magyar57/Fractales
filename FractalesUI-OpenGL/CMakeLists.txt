cmake_minimum_required(VERSION 3.6.0)
project(Visualisation VERSION 0.1)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

if(APPLE)
	include_directories("/opt/homebrew/include")
endif(APPLE)

# Libs installées

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
link_libraries(${OPENGL_LIBRARIES})

find_package(glfw3 3.3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
link_libraries(glfw)

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
link_libraries(GLEW::GLEW)

find_package(glm REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})

# Libs installées localement (dossier ExternalLibs)

file(GLOB_RECURSE LIBS_SOURCES SRC/ExternalLibs/*.cpp)
include_directories(SRC/ExternalLibs/)

# Exécutables

file(GLOB CODE_HEADERS SRC/*.hpp)
file(GLOB CODE_SOURCES SRC/*.cpp)
add_executable(fractales ${CODE_HEADERS} ${CODE_SOURCES} ${LIBS_SOURCES})

add_custom_command(TARGET fractales POST_BUILD COMMAND ${CMAKE_COMMAND} -E 
	copy_directory ${CMAKE_SOURCE_DIR}/Assets $<TARGET_FILE_DIR:fractales>/Assets)
add_custom_command(TARGET fractales POST_BUILD COMMAND ${CMAKE_COMMAND} -E 
	copy_directory ${CMAKE_SOURCE_DIR}/SRC/Shaders $<TARGET_FILE_DIR:fractales>/Shaders)
