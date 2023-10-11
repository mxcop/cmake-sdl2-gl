set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# SDL
# https://github.com/libsdl-org/SDL
set(SDL_LIBC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
add_subdirectory(sdl)
target_link_libraries(main SDL2::SDL2main SDL2::SDL2-static)

# SDL_image
# https://github.com/libsdl-org/SDL_image
# set( SDL2IMAGE_SAMPLES OFF CACHE BOOL "" FORCE )
# set( SDL2IMAGE_INSTALL OFF CACHE BOOL "" FORCE )
# set( SDL2IMAGE_VENDORED ON CACHE BOOL "" FORCE )
# set( SDL2IMAGE_DEPS_SHARED OFF CACHE BOOL "" FORCE )
# add_subdirectory( sdl-image )
# target_link_libraries( main SDL2_image::SDL2_image-static )

# SDL_ttf
# https://github.com/libsdl-org/SDL_ttf
# set( SDL2TTF_SAMPLES OFF CACHE BOOL "" FORCE )
# set( SDL2TTF_INSTALL OFF CACHE BOOL "" FORCE )
# set( SDL2TTF_VENDORED ON CACHE BOOL "" FORCE )
# set( SDL2TTF_HARFBUZZ ON CACHE BOOL "" FORCE )
# add_subdirectory( sdl-ttf )
# target_link_libraries( main SDL2_ttf::SDL2_ttf-static )

# SDL_mixer
# https://github.com/libsdl-org/SDL_mixer
# set( SDL2MIXER_SAMPLES OFF CACHE BOOL "" FORCE )
# set( SDL2MIXER_INSTALL OFF CACHE BOOL "" FORCE )
# set( SDL2MIXER_VENDORED ON CACHE BOOL "" FORCE )
# set( SDL2MIXER_DEPS_SHARED OFF CACHE BOOL "" FORCE )
# set( SDL2MIXER_FLAC OFF CACHE BOOL "" FORCE )
# set( SDL2MIXER_MOD OFF CACHE BOOL "" FORCE )
# set( SDL2MIXER_MP3 OFF CACHE BOOL "" FORCE )
# set( SDL2MIXER_MIDI OFF CACHE BOOL "" FORCE )
# set( SDL2MIXER_OPUS OFF CACHE BOOL "" FORCE )
# add_subdirectory( sdl-mixer )
# target_link_libraries( main SDL2_mixer::SDL2_mixer-static )

# IMGUI
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
add_library(IMGUI STATIC)

target_sources( IMGUI
                PRIVATE
                    ${IMGUI_DIR}/imgui_demo.cpp
                    ${IMGUI_DIR}/imgui_draw.cpp
                    ${IMGUI_DIR}/imgui_tables.cpp
                    ${IMGUI_DIR}/imgui_widgets.cpp
                    ${IMGUI_DIR}/imgui.cpp

                PRIVATE
                    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
                    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
                    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
                )

target_include_directories( IMGUI
                            PUBLIC ${IMGUI_DIR}
                            PUBLIC ${IMGUI_DIR}/backends
                            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/sdl/include
                            )


if(USE_GLES OR RASPBIAN)
    target_compile_definitions(IMGUI PUBLIC -DIMGUI_IMPL_OPENGL_ES2)

    if(RASPBIAN AND RPI_USE_BROADCOM_DRIVER)
        target_link_directories(IMGUI PUBLIC /opt/vc/lib)
        target_link_libraries(IMGUI PUBLIC brcmGLESv2 brcmEGL)
    else()
        target_link_libraries(IMGUI PUBLIC GLESv2)
    endif()
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(IMGUI PUBLIC ${OPENGL_LIBRARIES})
endif()

target_link_libraries(main IMGUI)

# GLM
# https://github.com/g-truc/glm
target_include_directories(main SYSTEM PRIVATE "glm")
target_precompile_headers(main PRIVATE "glm/glm/fwd.hpp")

# GLAD
# https://github.com/Dav1dde/glad
add_subdirectory(glad)
target_link_libraries(main glad)
