cmake_minimum_required(VERSION 3.10)

# Helper macro to download and extract SDL2 package
macro(download_sdl2_package name version url)
    set(archive "${CMAKE_BINARY_DIR}/${name}.zip")
    set(dest "${CMAKE_BINARY_DIR}/${name}")

    if(NOT EXISTS ${dest})
        message(STATUS "Downloading ${name} ${version}...")
        file(DOWNLOAD ${url} ${archive} SHOW_PROGRESS)
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${archive}
                        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
        file(GLOB extracted_dir "${CMAKE_BINARY_DIR}/${name}-*")
        file(RENAME "${extracted_dir}" "${dest}")
    endif()

    list(APPEND SDL2_ALL_INCLUDE_DIRS "${dest}/include")
    list(APPEND SDL2_ALL_LIB_DIRS "${dest}/lib/x64")
endmacro()

# SDL2 main
download_sdl2_package("SDL2" "2.30.4"
    "https://github.com/libsdl-org/SDL/releases/download/release-2.30.4/SDL2-devel-2.30.4-VC.zip")

# SDL2_image
download_sdl2_package("SDL2_image" "2.8.2"
    "https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.2/SDL2_image-devel-2.8.2-VC.zip")

# SDL2_ttf
download_sdl2_package("SDL2_ttf" "2.22.0"
    "https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/SDL2_ttf-devel-2.22.0-VC.zip")

# SDL2_mixer
download_sdl2_package("SDL2_mixer" "2.6.3"
    "https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.6.3/SDL2_mixer-devel-2.6.3-VC.zip")

set(SDL2_ALL_INCLUDE_DIRS ${SDL2_ALL_INCLUDE_DIRS} PARENT_SCOPE)
set(SDL2_ALL_LIB_DIRS ${SDL2_ALL_LIB_DIRS} PARENT_SCOPE)
