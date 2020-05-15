cmake_minimum_required(VERSION 3.15)
project(ksp_2d_cw4)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

find_package(PkgConfig REQUIRED)

set(ENV{PKG_CONFIG_PATH} "ENV{PKG_CONFIG_PATH}:/usr/local/Cellar/sdl2_image/2.0.5/lib/pkgconfig:/usr/local/Cellar/sdl2_ttf/2.0.15/lib/pkgconfig:/usr/local/Cellar/sdl2/2.0.10/lib/pkgconfig")

pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
pkg_check_modules(SDL2_TTF REQUIRED IMPORTED_TARGET sdl2_ttf)
pkg_check_modules(SDL2_IMAGE REQUIRED IMPORTED_TARGET sdl2_image)
pkg_check_modules(SDL2_GFX REQUIRED IMPORTED_TARGET sdl2_gfx)

file(GLOB FRAMEWORK
#        "sdl2_framework/*.h"
        "sdl2_framework/*.cpp"
        )

file(GLOB SOURCE
        "src/*/*/*.h"
        "src/*/*/*.cpp"
        "src/*/*.h"
        "src/*/*.cpp"
        "src/*.cpp"
        "src/*.h"
        )

add_executable(ksp_2d_cw4 ${FRAMEWORK} ${SOURCE} src)

target_link_libraries(ksp_2d_cw4
        PkgConfig::SDL2
        PkgConfig::SDL2_TTF
        PkgConfig::SDL2_IMAGE
        PkgConfig::SDL2_GFX
        )

file(GLOB RESOURCES "resources/img/*" "resources/metadata/*")
foreach(resourceFile ${RESOURCES})
    if(IS_DIRECTORY ${resourceFile})
        file(GLOB INNER_DIR ${resourceFile}/*.*)
        #        message(${resourceFile})
        #        get_filename_component(a_dir "${resourceFile}" NAME)
        #        get_filename_component(cmake_dir "${CMAKE_CURRENT_BINARY_DIR}" NAME)
        #        message(${cmake_dir}/${a_dir})
        #        file(MAKE_DIRECTORY ${cmake_dir}/${a_dir})
        foreach(file ${INNER_DIR})
            configure_file(${file} "${CMAKE_SOURCE_DIR}/build" COPYONLY)
        endforeach()
    else()
        configure_file(${resourceFile} "${CMAKE_SOURCE_DIR}/build" COPYONLY)
    endif()
    #    file(COPY ${file} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${file})
endforeach()