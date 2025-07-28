cmake_minimum_required(VERSION 3.10)

set(SDL2_DOWNLOAD_DIR "${CMAKE_SOURCE_DIR}/build")

# Helper: download a file with curl
function(download_file url dest)
    if(EXISTS ${dest})
        message(STATUS "Already downloaded: ${dest}")
    else()
        message(STATUS "Downloading ${url} -> ${dest}")
        execute_process(
            COMMAND curl -L -C - -o ${dest} ${url}
            RESULT_VARIABLE result
        )
        if(NOT result EQUAL 0)
            message(FATAL_ERROR "Failed to download ${url}")
        endif()
    endif()
endfunction()

# Helper macro to download, extract, and fix SDL2 include structure
macro(download_sdl2_package name version url)
    set(archive "${SDL2_DOWNLOAD_DIR}/${name}.zip")
    set(dest "${SDL2_DOWNLOAD_DIR}/${name}")

    if(NOT EXISTS ${dest})
        download_file(${url} ${archive})
        message(STATUS "Extracting ${name}...")
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${archive}
                        WORKING_DIRECTORY ${SDL2_DOWNLOAD_DIR})
        file(GLOB extracted_dir "${SDL2_DOWNLOAD_DIR}/${name}-*")
        file(RENAME "${extracted_dir}" "${dest}")

        # --- Fix include structure for <SDL2/...> includes ---
        file(MAKE_DIRECTORY "${dest}/include/SDL2")
        file(GLOB SDL_HEADERS "${dest}/include/*.h")
        foreach(header ${SDL_HEADERS})
            file(COPY ${header} DESTINATION "${dest}/include/SDL2")
        endforeach()
    endif()
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
