cmake_minimum_required(VERSION 3.10)
project(PixelInvaders C)

# Use C11 standard
set(CMAKE_C_STANDARD 11)

# Windows-specific SDL2 paths
if (WIN32)
    set(SDL2_DIR "${CMAKE_SOURCE_DIR}/SDL2/cmake")
    set(SDL2_IMAGE_DIR "${CMAKE_SOURCE_DIR}/SDL2_image/cmake")
    set(SDL2_TTF_DIR "${CMAKE_SOURCE_DIR}/SDL2_ttf/cmake")
    set(SDL2_MIXER_DIR "${CMAKE_SOURCE_DIR}/SDL2_mixer/cmake")
    set(CMAKE_PREFIX_PATH "${SDL2_DIR};${SDL2_IMAGE_DIR};${SDL2_TTF_DIR};${SDL2_MIXER_DIR}")
endif()

# Find SDL2 and its extensions
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

# Source files
set(SOURCES
    main.c
    game.c
    player.c
    bullets.c
    enemies.c
    waves.c
    audio.c
    sprites.c
    ui.c
    pickups.c
)

# Create executable
add_executable(PixelInvaders ${SOURCES})

# Include SDL2 headers
target_include_directories(PixelInvaders PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
)

# Link SDL2 libraries
target_link_libraries(PixelInvaders
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)

# --- Versioning ---
# Get Git tag or commit hash
execute_process(
    COMMAND git describe --tags --always
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Fallback if no version is found
if(NOT GIT_TAG)
    set(GIT_TAG "v0.0.0-unknown")
endif()

# Build date
string(TIMESTAMP BUILD_DATE "%Y-%m-%d")

# Configure version.h
configure_file(
    ${CMAKE_SOURCE_DIR}/version.h.in
    ${CMAKE_BINARY_DIR}/version.h
)

# Include generated version.h
include_directories(${CMAKE_BINARY_DIR})
