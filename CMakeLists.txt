cmake_minimum_required(VERSION 3.10)
project(PixelInvaders C)

# Use C11 standard
set(CMAKE_C_STANDARD 11)

# Find SDL2 and its extensions
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

# Add source files
set(SOURECES
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

add_executable(PixelInvaders ${SOURCES})

# Include SDL2 headers
target_include_directories(PixelInvaders, PRIVATE
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

# Link SDL2 for Windows
if (WIN32)
    set(SDL2_DIR "${CMAKE_SOURCE_DIR}/SDL2/cmake")
    set(SDL2_IMAGE_DIR "${CMAKE_SOURCE_DIR}/SDL2_image/cmake")
    set(SDL2_TTF_DIR "${CMAKE_SOURCE_DIR}/SDL2_ttf/cmake")
    set(SDL2_MIXER_DIR "${CMAKE_SOURCE_DIR}/SDL2_mixer/cmake")

    set(CMAKE_PREFIX_PATH "${SDL2_DIR};${SDL2_IMAGE_DIR};${SDL2_TTF_DIR};${SDL2_MIXER_DIR}")
endif()

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)
