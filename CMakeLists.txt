cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(OpenGL-Base CXX)

#===========================================================================================
# OPTIONAL LIBRARiES
#
# The following libraries can be pulled down and added to the project by turnning the
# desired options on. Open an issue for additional default support.
# See: https://github.com/Hoshiningen/OpenGL-Template/issues

#set(ENABLE_ASSIMP   ON CACHE BOOL "Add Open Asset Import Library (assimp) to the project" FORCE)
set(ENABLE_GLM   ON CACHE BOOL "Add GLM to the project" FORCE)
set(ENABLE_STB   ON CACHE BOOL "Add STB to the project" FORCE)

#===========================================================================================
# GLAD CONFIGURATION
#
# Configure GLAD loader generation variables (lines below pulled from the GLAD top-level
# CMakeLists.txt file, with slight modifications.
# Refer to https://github.com/Dav1dde/glad for more information)

set(GLAD_OUT_DIR        "${CMAKE_CURRENT_BINARY_DIR}" CACHE STRING "Output directory" FORCE)
set(GLAD_PROFILE        "core" CACHE STRING "OpenGL profile" FORCE)
set(GLAD_API            "gl=4.6" CACHE STRING "API type/version pairs, like \"gl=3.2,gles=\", no version means latest" FORCE)
set(GLAD_GENERATOR      "c" CACHE STRING "Language to generate the binding for" FORCE)
set(GLAD_EXTENSIONS     "" CACHE STRING "Path to extensions file or comma separated list of extensions, if missing all extensions are included" FORCE)
set(GLAD_SPEC           "gl" CACHE STRING "Name of the spec" FORCE)
set(GLAD_ALL_EXTENSIONS OFF CACHE BOOL "Include all extensions instead of those specified by GLAD_EXTENSIONS" FORCE)
set(GLAD_NO_LOADER      OFF CACHE BOOL "No loader" FORCE)
set(GLAD_REPRODUCIBLE   OFF CACHE BOOL "Reproducible build" FORCE)

#===========================================================================================

set_property(GLOBAL PROPERTY USE_FOLDERS OFF)

# Set directory paths
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TEXTURE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/texture)
set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shader)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(SRC ${SOURCE_DIR}/main.cpp
        ${SOURCE_DIR}/openglbase.h
        ${SOURCE_DIR}/stb.cpp 
        ${SOURCE_DIR}/shader.h
        ${SOURCE_DIR}/shader.cpp
        ${SOURCE_DIR}/camera.h
        ${SOURCE_DIR}/camera.cpp
        ${SOURCE_DIR}/flyingCamera.h
        ${SOURCE_DIR}/flyingCamera.cpp)

add_executable(opengl-base ${SRC})

# Perform dependency linkage
include(${CMAKE_DIR}/LinkGLFW.cmake)
LinkGLFW(opengl-base PRIVATE)

include(${CMAKE_DIR}/LinkGLAD.cmake)
LinkGLAD(opengl-base PRIVATE)

find_package(OpenGL REQUIRED)
if (OpenGL_FOUND)
    target_include_directories(opengl-base PRIVATE ${OPENGL_INCLDUE_DIRS})
    target_link_libraries(opengl-base PRIVATE ${OPENGL_LIBRARIES})
else()
    message(FATAL_ERROR "OpenGL was not found on the system")
endif()

# Handle the optional libraries
if (${ENABLE_ASSIMP})
    include(${CMAKE_DIR}/LinkASSIMP.cmake)
    LinkASSIMP(opengl-base PRIVATE)
endif()

if (${ENABLE_GLM})
    include(${CMAKE_DIR}/LinkGLM.cmake)
    LinkGLM(opengl-base PRIVATE)
endif()

if (${ENABLE_STB})
    include(${CMAKE_DIR}/LinkSTB.cmake)
    LinkSTB(opengl-base PRIVATE)
endif()

# Enable C++17
set_target_properties(opengl-base PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

# Set project folders
set_target_properties(opengl-base PROPERTIES FOLDER ${PROJECT_NAME})

# Copy resoures folder to the binary directory
file(COPY ${TEXTURE_DIR} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY ${SHADER_DIR} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
