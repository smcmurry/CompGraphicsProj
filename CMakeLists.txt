cmake_minimum_required(VERSION 3.14)
project(lab05)
set(CMAKE_CXX_STANDARD 17)
set(SOURCE_FILES main.cpp Lab05Engine.cpp Lab05Engine.hpp Zennia.cpp Zennia.hpp Engine.cpp Engine.hpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Windows with MinGW Installations
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND MINGW)
    # if working on Windows but not in the lab
    # update the include directory location
    include_directories("../include")

    # update the lib directory location
    target_link_directories(${PROJECT_NAME} PUBLIC "../lib")
    target_link_libraries(${PROJECT_NAME} opengl32 glfw3 glew32.dll gdi32)

# OS X Installations
elseif(APPLE AND ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # update the include directory location
    include_directories("/usr/local/include")

    # update the lib directory location
    target_link_directories(${PROJECT_NAME} PUBLIC "/usr/local/lib")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo" glfw3 glew)

# Blanket *nix Installations
elseif(UNIX AND ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # update the include directory location
    include_directories("/usr/local/include")

    # update the lib directory location
    target_link_directories(${PROJECT_NAME} PUBLIC "/usr/local/lib")
    target_link_libraries(${PROJECT_NAME} opengl glfw GLEW)
endif()