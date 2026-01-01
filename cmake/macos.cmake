# -----------------------------------------------------------------------------
# CannonBall macOS Setup (with SDL_gpu https://github.com/grimfang4/sdl-gpu)
# -DTARGET=macos.cmake -DSDL_GPU=true
# -----------------------------------------------------------------------------

find_package(OpenGL REQUIRED)

find_path(
        SDL_GPU_INCLUDE_DIR
        NAMES SDL_gpu.h
        PATHS /usr/local/include ${PROJECT_SOURCE_DIR}/SDL_gpu/include/SDL2
)

find_library(
        SDL_GPU_LIBRARY
        libSDL2_gpu.a
        PATHS ${PROJECT_SOURCE_DIR}/SDL_gpu/lib
)

include_directories(
        ${SDL_GPU_INCLUDE_DIR}
)

set (CMAKE_EXE_LINKER_FLAGS -mmacosx-version-min=15.6)

set(platform_link_libs
        ${OPENGL_LIBRARIES}
        ${SDL_GPU_LIBRARY}
)
