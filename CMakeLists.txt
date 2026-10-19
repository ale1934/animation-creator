cmake_minimum_required(VERSION 3.20)

project(Animation-Creator
    VERSION 1.0
    DESCRIPTION "2D animation creator for general use"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(animation-creator
    src/main.cpp
    src/frame.cpp
    src/animation.cpp
    src/ui.cpp
    src/widgets.cpp
	src/settings.cpp
)

target_include_directories(animation-creator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

find_package(raylib CONFIG REQUIRED)
target_link_libraries(animation-creator PRIVATE raylib)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(animation-creator PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copy resources next to the executable after each build
add_custom_command(TARGET animation-creator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:animation-creator>/resources)
