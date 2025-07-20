cmake_minimum_required(VERSION 3.10)
project(PixelInvaders C)

# Use C11 standard
set(CMAKE_C_STANDARD 11)

# -------------------------------
# Detect platform
# -------------------------------
if(WIN32)
    # Windows-specific SDL2 setup (using prebuilt libraries)
    set(SDL2_DIR "${CMAKE_SOURCE_DIR}/SDL2/cmake")
    set(SDL2_IMAGE_DIR "${CMAKE_SOURCE_DIR}/SDL2_image/cmake")
    set(SDL2_TTF_DIR "${CMAKE_SOURCE_DIR}/SDL2_ttf/cmake")
    set(SDL2_MIXER_DIR "${CMAKE_SOURCE_DIR}/SDL2_mixer/cmake")
    set(CMAKE_PREFIX_PATH "${SDL2_DIR};${SDL2_IMAGE_DIR};${SDL2_TTF_DIR};${SDL2_MIXER_DIR}")

    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(SDL2_mixer REQUIRED)
else()
    # Use pkg-config on macOS and Linux
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
endif()

# -------------------------------
# Source files
# -------------------------------
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

# -------------------------------
# Executable
# -------------------------------
add_executable(PixelInvaders ${SOURCES})

# -------------------------------
# Include directories
# -------------------------------
if(WIN32)
    target_include_directories(PixelInvaders PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
    )
    target_link_libraries(PixelInvaders
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
    )
else()
    target_include_directories(PixelInvaders PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
    )
    target_link_libraries(PixelInvaders
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
    )
endif()

# -------------------------------
# Git version info
# -------------------------------
execute_process(
    COMMAND git describe --tags --always
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT GIT_TAG)
    set(GIT_TAG "v0.0.0-unknown")
endif()

string(TIMESTAMP BUILD_DATE "%Y-%m-%d")

configure_file(
    ${CMAKE_SOURCE_DIR}/version.h.in
    ${CMAKE_BINARY_DIR}/version.h
)

include_directories(${CMAKE_BINARY_DIR})
