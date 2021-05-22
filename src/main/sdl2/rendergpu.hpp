/***************************************************************************
    SDL2 GPU Video Rendering.
    
    Useful References:
    https://grimfang4.github.io/sdl-gpu/index.html

    Copyright Björn Nilsson, Manuel Alfayate and Chris White.
    See license.txt for more details.
***************************************************************************/

#pragma once

#include <SDL_gpu.h>

#include "renderbase.hpp"

class Render : public RenderBase
{
public:
    Render();
    ~Render();
    bool init(int src_width, int src_height, 
              int scale,
              int video_mode,
              int scanlines);
    void disable();
    bool start_frame();
    bool finalize_frame();
    void draw_frame(uint16_t* pixels);

private:
    // SDL2 window
    SDL_Window *window;

    // SDL2_gpu renderer
    GPU_Target *renderer;

    // SDL2_gpu texture
    GPU_Image *texture;

    uint32_t shader;

    GPU_ShaderBlock block;
};
