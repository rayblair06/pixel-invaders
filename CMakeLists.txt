cmake_minimum_required(VERSION 3.10)
project(PixelInvaders C)

set(CMAKE_C_STANDARD 11)

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

add_executable(PixelInvaders ${SOURCES})

# -------------------------------
# Platform SDL2
# -------------------------------
if(WIN32)
    target_include_directories(PixelInvaders PRIVATE
        ${CMAKE_SOURCE_DIR}/SDL2/include/SDL2
        ${CMAKE_SOURCE_DIR}/SDL2_image/include/SDL2
        ${CMAKE_SOURCE_DIR}/SDL2_ttf/include/SDL2
        ${CMAKE_SOURCE_DIR}/SDL2_mixer/include/SDL2
    )
    target_link_directories(PixelInvaders PRIVATE
        ${CMAKE_SOURCE_DIR}/SDL2/lib/x64
        ${CMAKE_SOURCE_DIR}/SDL2_image/lib/x64
        ${CMAKE_SOURCE_DIR}/SDL2_ttf/lib/x64
        ${CMAKE_SOURCE_DIR}/SDL2_mixer/lib/x64
    )
    target_link_libraries(PixelInvaders SDL2 SDL2main SDL2_image SDL2_ttf SDL2_mixer)

elseif(APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
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

else() # Linux
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
    pkg_check_modules(SDL2_MIXER REQUIRED sdl2_mixer)
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
        m # Fix sqrtf error
    )
endif()
