set(SDL2_ALL_FOUND FALSE)
set(SDL2_ALL_LIBS "")
set(SDL2_ALL_INCLUDE_DIRS "")

if(WIN32)
    include(${CMAKE_SOURCE_DIR}/cmake/SetupSDL2.cmake)
    link_directories(${SDL2_ALL_LIB_DIRS})
    set(SDL2_ALL_LIBS SDL2 SDL2main SDL2_image SDL2_ttf SDL2_mixer)
    set(SDL2_ALL_FOUND TRUE)

elseif(APPLE)
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
    )
    set(SDL2_ALL_FOUND TRUE)

else() # Linux
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
        m # math library for Linux
    )
    set(SDL2_ALL_FOUND TRUE)
endif()

if(NOT SDL2_ALL_FOUND)
    message(FATAL_ERROR "SDL2 libraries not found!")
endif()

# Expose vars
set(SDL2_ALL_INCLUDE_DIRS ${SDL2_ALL_INCLUDE_DIRS} PARENT_SCOPE)
set(SDL2_ALL_LIBS ${SDL2_ALL_LIBS} PARENT_SCOPE)
