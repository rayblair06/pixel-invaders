cmake_minimum_required(VERSION 3.10)
project(PixelInvaders C)

set(CMAKE_C_STANDARD 11)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# -------------------------------
# Source files (common)
# -------------------------------
set(SOURCES
    audio.c
    boss.c
    bullets.c
    constants.c
    collisions.c
    entity.c
    enemies.c
    level_manager.c
    game.c
    main.c
    particles.c
    pickups.c
    player.c
    scene_manager.c
    stats.c
    sprites.c
    ui.c
    upgrades.c
    utils.c
    waves.c
)

# -------------------------------
# Executable target
# -------------------------------
if (WIN32)
    # Add resources.rc only on Windows (to embed .ico)
    list(APPEND SOURCES resources.rc)
    add_executable(PixelInvaders WIN32 ${SOURCES})
else()
    add_executable(PixelInvaders ${SOURCES})
endif()

# -------------------------------
# SDL2 Setup
# -------------------------------
include(cmake/FindSDL2_All.cmake)
target_include_directories(PixelInvaders PRIVATE ${SDL2_ALL_INCLUDE_DIRS})
target_link_libraries(PixelInvaders ${SDL2_ALL_LIBS})

# -------------------------------
# macOS bundle + icon
# -------------------------------
if(APPLE)
    # Homebrew pkg-config path hint (Apple Silicon)
    if(EXISTS "/opt/homebrew/lib/pkgconfig")
        set(ENV{PKG_CONFIG_PATH} "/opt/homebrew/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
    endif()

    # Turn the target into an .app bundle and attach the .icns
    set_target_properties(PixelInvaders PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_ICON_FILE icon.icns
    )

    # Place the .icns into the bundle Resources
    set_source_files_properties(${CMAKE_SOURCE_DIR}/assets/icon.icns
        PROPERTIES MACOSX_PACKAGE_LOCATION "Resources"
    )
    target_sources(PixelInvaders PRIVATE ${CMAKE_SOURCE_DIR}/assets/icon.icns)
endif()

# -------------------------------
# Windows-specific compiler options
# -------------------------------
if (MSVC)
    # Disable sprintf warnings
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    # Disable conversion warnings (int ↔ float)
    target_compile_options(PixelInvaders PRIVATE /wd4244 /wd4305)
endif()

# -------------------------------
# Version Info (generated header)
# -------------------------------
# Get git tag (fallback if not in a repo)
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
configure_file(${CMAKE_SOURCE_DIR}/version.h.in ${CMAKE_SOURCE_DIR}/version.h)
include_directories(${CMAKE_BINARY_DIR})

# -------------------------------
# Install Rules
# -------------------------------
install(TARGETS PixelInvaders
    RUNTIME DESTINATION .
    BUNDLE  DESTINATION .   # for macOS .app
)

# Install assets folder
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    install(DIRECTORY assets/
        DESTINATION assets
    )
endif()

# Post-install SDL2 DLL copy for Windows (adjust paths to your setup)
if(WIN32)
    set(SDL2_BINARIES
        "${CMAKE_BINARY_DIR}/SDL2/lib/x64/SDL2.dll"
        "${CMAKE_BINARY_DIR}/SDL2_image/lib/x64/SDL2_image.dll"
        "${CMAKE_BINARY_DIR}/SDL2_ttf/lib/x64/SDL2_ttf.dll"
        "${CMAKE_BINARY_DIR}/SDL2_mixer/lib/x64/SDL2_mixer.dll"
    )
    install(FILES ${SDL2_BINARIES} DESTINATION .)
endif()
