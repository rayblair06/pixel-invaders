cmake_minimum_required(VERSION 3.10)
project(PixelInvaders C)

# Use C11 standard
set(CMAKE_C_STANDARD 11)

# -------------------------------
# Platform-specific SDL2 handling
# -------------------------------
if(WIN32)
    # Use prebuilt SDL2 libraries from extracted archives
    include_directories(
        ${CMAKE_SOURCE_DIR}/SDL2/include
        ${CMAKE_SOURCE_DIR}/SDL2_image/include
        ${CMAKE_SOURCE_DIR}/SDL2_ttf/include
        ${CMAKE_SOURCE_DIR}/SDL2_mixer/include
    )

    link_directories(
        ${CMAKE_SOURCE_DIR}/SDL2/lib/x64
        ${CMAKE_SOURCE_DIR}/SDL2_image/lib/x64
        ${CMAKE_SOURCE_DIR}/SDL2_ttf/lib/x64
        ${CMAKE_SOURCE_DIR}/SDL2_mixer/lib/x64
    )

    set(SDL2_LIBS SDL2 SDL2main SDL2_image SDL2_ttf SDL2_mixer)

else()
    find_package(PkgConfig REQUIRED)

    # macOS: Ensure Homebrew pkg-config paths are included
    if(APPLE)
        execute_process(
            COMMAND brew --prefix
            OUTPUT_VARIABLE BREW_PREFIX
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        set(ENV{PKG_CONFIG_PATH}
            "$ENV{PKG_CONFIG_PATH}:${BREW_PREFIX}/lib/pkgconfig:${BREW_PREFIX}/opt/sdl2/lib/pkgconfig"
        )
    endif()

    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)

    include_directories(
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
    )

    set(SDL2_LIBS
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
    )
endif()

# -------------------------------
# Source files
# -------------------------------
set(SOURCES
    audio.c
    boss.c
    bullets.c
    constants.c
    collisions.c
    entity.c
    enemies.c
    game.c
    main.c
    particles.c
    pickups.c
    player.c
    sprites.c
    ui.c
    upgrades.c
    waves.c
)

# -------------------------------
# Build executable
# -------------------------------
add_executable(PixelInvaders ${SOURCES})

# -------------------------------
# Linking
# -------------------------------
target_link_libraries(PixelInvaders ${SDL2_LIBS})

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
