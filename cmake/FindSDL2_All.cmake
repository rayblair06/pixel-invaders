set(SDL2_ALL_FOUND FALSE)
set(SDL2_ALL_LIBS "")
set(SDL2_ALL_INCLUDE_DIRS "")

if (WIN32)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupSDL2.cmake)
    link_directories(${SDL2_ALL_LIB_DIRS})

    set(SDL2_ALL_LIBS
        ${SDL2_ALL_LIB_DIRS}/SDL2.lib
        ${SDL2_ALL_LIB_DIRS}/SDL2main.lib
        ${SDL2_ALL_LIB_DIRS}/SDL2_image.lib
        ${SDL2_ALL_LIB_DIRS}/SDL2_ttf.lib
        ${SDL2_ALL_LIB_DIRS}/SDL2_mixer.lib
    )

    set(SDL2_ALL_FOUND TRUE)

elseif(APPLE)
    # macOS: Use pkg-config for headers, absolute paths for libs
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)

    # Header include directories
    set(SDL2_ALL_INCLUDE_DIRS
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
    )

    # Resolve full library paths using Homebrew
    execute_process(
        COMMAND brew --prefix sdl2
        OUTPUT_VARIABLE SDL2_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND brew --prefix sdl2_image
        OUTPUT_VARIABLE SDL2_IMAGE_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND brew --prefix sdl2_ttf
        OUTPUT_VARIABLE SDL2_TTF_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND brew --prefix sdl2_mixer
        OUTPUT_VARIABLE SDL2_MIXER_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(SDL2_ALL_LIBS
        ${SDL2_PREFIX}/lib/libSDL2.dylib
        ${SDL2_IMAGE_PREFIX}/lib/libSDL2_image.dylib
        ${SDL2_TTF_PREFIX}/lib/libSDL2_ttf.dylib
        ${SDL2_MIXER_PREFIX}/lib/libSDL2_mixer.dylib
    )

    set(SDL2_ALL_FOUND TRUE)

else() # Linux
    # Linux: Use pkg-config (libs are in default linker paths)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)

    set(SDL2_ALL_INCLUDE_DIRS
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
    )
    set(SDL2_ALL_LIBS
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
        m # math library for sqrtf, etc.
    )
    set(SDL2_ALL_FOUND TRUE)
endif()

if(NOT SDL2_ALL_FOUND)
    message(FATAL_ERROR "SDL2 libraries not found!")
endif()
