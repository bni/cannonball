# -----------------------------------------------------------------------------
# CannonBall macOS Setup
# -----------------------------------------------------------------------------

find_path(
    SDL_GPU_INCLUDE_DIR
    NAMES SDL_gpu.h
    PATHS /usr/local/include /Users/bni/Documents/sdl-gpu/include
)

include_directories(
    ${SDL_GPU_INCLUDE_DIR}
)

find_package(OpenGL REQUIRED)

set(SDL2_GPU_LIBRARY_PATH "/Users/bni/Documents/sdl-gpu/lib/libSDL2_gpu.a")

set(platform_link_libs
    ${OPENGL_LIBRARIES}
    ${SDL2_GPU_LIBRARY_PATH}
)
