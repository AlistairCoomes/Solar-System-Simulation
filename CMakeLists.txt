cmake_minimum_required(VERSION 3.20)
project(SolarSystemApp LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Header search paths
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Executable sources
add_executable(SolarSystem
    main.cpp
    include/glad/glad.c
    include/SolarSystemHeader/SolarSystem.cpp
)

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

# Platform-specific dependency linking
if (WIN32)
    # Windows linking
    target_link_directories(SolarSystem PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    target_link_libraries(SolarSystem PRIVATE glfw3 opengl32 user32 gdi32 shell32 uuid)
elseif (APPLE)
    # macOS linking (uses system/brew GLFW and Apple OpenGL framework)
    find_package(glfw3 REQUIRED)
    find_package(OpenGL REQUIRED)
    target_link_libraries(SolarSystem PRIVATE glfw OpenGL::GL "-framework Cocoa" "-framework IOKit")
endif()

add_custom_command(
    TARGET SolarSystem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/ShaderSources"
        "$<TARGET_FILE_DIR:SolarSystem>/ShaderSources"
)