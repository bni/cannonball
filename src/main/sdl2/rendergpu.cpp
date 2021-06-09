/***************************************************************************
    SDL2 GPU Video Rendering.
    
    Useful References:
    https://grimfang4.github.io/sdl-gpu/index.html

    Copyright Björn Nilsson, Manuel Alfayate, Chris White.
    See license.txt for more details.
***************************************************************************/

#include <iostream>

#include "rendergpu.hpp"
#include "frontend/config.hpp"

Render::Render(void)
{
}

Render::~Render(void)
{
}

bool Render::init(int src_width, int src_height,
                    int scale,
                    int video_mode,
                    int scanlines)
{
    this->src_width  = src_width;
    this->src_height = src_height;
    this->scale      = scale;
    this->video_mode = video_mode;
    this->scanlines  = scanlines;

    this->scanlines  = 0; // Always off, shader does that

    // Setup SDL Screen size
    if (!RenderBase::sdl_screen_size())
        return false;

    int flags = SDL_WINDOW_SHOWN;

    if (video_mode == video_settings_t::MODE_FULL)
    {
	    flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

	    scn_width  = orig_width;
        scn_height = orig_height;

        SDL_ShowCursor(false);
    } else {
        this->video_mode = video_settings_t::MODE_WINDOW;
       
        scn_width  = src_width  * scale;
        scn_height = src_height * scale;

        SDL_ShowCursor(true);
    }

    flags |= SDL_WINDOW_OPENGL;
    flags |= SDL_WINDOW_ALLOW_HIGHDPI;

    const int bpp = 32;

    // Frees (Deletes) existing surface
    if (surface)
        SDL_FreeSurface(surface);

    surface = SDL_CreateRGBSurface(0, src_width, src_height, bpp, 0, 0, 0, 0);

    if (!surface) {
        std::cerr << "Surface creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(
        "OutRun", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scn_width, scn_height,
        (Uint32)flags);

    SDL_SetWindowBrightness(window, 0.7);

    GPU_SetInitWindow(SDL_GetWindowID(window));

    GPU_SetPreInitFlags(GPU_INIT_DISABLE_VSYNC | GPU_INIT_REQUEST_COMPATIBILITY_PROFILE);

    renderer = GPU_Init((Uint16)scn_width, (Uint16)scn_height, GPU_DEFAULT_INIT_FLAGS);

    //std::cout << "src_width: " << src_width << std::endl;
    //std::cout << "src_height: " << src_height << std::endl;
    //std::cout << "scn_width: " << scn_width << std::endl;
    //std::cout << "scn_height: " << scn_height << std::endl;

    // Convert the SDL pixel surface to 32 bit.
    // This is potentially a larger surface area than the internal pixel array.
    screen_pixels = (uint32_t*)surface->pixels;

    // SDL Pixel Format Information
    Rshift = surface->format->Rshift;
    Gshift = surface->format->Gshift;
    Bshift = surface->format->Bshift;
    Rmask  = surface->format->Rmask;
    Gmask  = surface->format->Gmask;
    Bmask  = surface->format->Bmask;

    // *** SHADER SETUP ***
    //GPU_Renderer* renderer = GPU_GetCurrentRenderer();
    //std::cout << "shader_language: " << renderer->shader_language << std::endl;
    //std::cout << "max_shader_version: " << renderer->max_shader_version << std::endl;

    uint32_t vertex = GPU_LoadShader(GPU_VERTEX_SHADER, "vertex.shader");
    if (!vertex) {
        std::cerr << "Failed to load vertex shader: " << GPU_GetShaderMessage() << std::endl;
    }

    uint32_t pixel = GPU_LoadShader(GPU_PIXEL_SHADER, "pixel.shader");
    if (!pixel) {
        std::cerr << "Failed to load pixel shader: " << GPU_GetShaderMessage() << std::endl;
    }

    if (shader) {
        GPU_FreeShaderProgram(shader);
    }

    shader = GPU_LinkShaders(vertex, pixel);
    if (shader) {
        block = GPU_LoadShaderBlock(shader, "gpu_Vertex", "gpu_TexCoord", "gpu_Color", "gpu_ModelViewProjectionMatrix");
    } else {
        std::cerr << "Failed to link shader program: " << GPU_GetShaderMessage() << std::endl;
    }
    // ********************

    return true;
}

void Render::disable()
{
    GPU_Quit();
    SDL_DestroyWindow(window);
}

bool Render::start_frame()
{
    return true;
}

bool Render::finalize_frame()
{
    GPU_Clear(renderer);

    texture = GPU_CopyImageFromSurface(surface);
    GPU_SetAnchor(texture, 0, 0);
    GPU_SetImageFilter(texture, GPU_FILTER_NEAREST);
    GPU_SetImageVirtualResolution(texture, (Uint16)scn_width, (Uint16)scn_height);

    // *** SHADER DRAW ***
    GPU_ActivateShaderProgram(shader, &block);

    int32_t w, h;
    SDL_GetWindowSize(window, &w, &h);

    const float aspect = src_width/float(src_height);
    const float scaleW = w/float(src_width);
    const float scaleH = h/float(src_height);

    GPU_Rect rect;
    if (scaleW < scaleH) {
        rect.w = w;
        rect.h = w/aspect;
        rect.x = 0;
        rect.y = (h - rect.h)/2;
    } else {
        rect.w = h*aspect;
        rect.h = h;
        rect.x = (w - rect.w)/2;
        rect.y = 0;
    }

    GPU_SetUniformf(GPU_GetUniformLocation(shader, "trg_x"), rect.x);
    GPU_SetUniformf(GPU_GetUniformLocation(shader, "trg_y"), rect.y);
    GPU_SetUniformf(GPU_GetUniformLocation(shader, "trg_w"), rect.w);
    GPU_SetUniformf(GPU_GetUniformLocation(shader, "trg_h"), rect.h);
    GPU_SetUniformf(GPU_GetUniformLocation(shader, "scr_w"), (float)w);
    GPU_SetUniformf(GPU_GetUniformLocation(shader, "scr_h"), (float)h);

    GPU_Blit(texture, NULL, renderer, 0, 0);

    GPU_DeactivateShaderProgram();
    // *******************

    // Mask out the edges with black bars in fullscreen 16:9 mode
    if (video_mode == video_settings_t::MODE_FULL) {
        float blackBarWidth = (float)(scn_width * 0.1);
        float rightStartPos = (float)scn_width - blackBarWidth;

        SDL_Color color = {0, 0, 0, 255};
        GPU_RectangleFilled(renderer, 0.0, 0.0, blackBarWidth, (float)scn_height, color);
        GPU_RectangleFilled(renderer, rightStartPos, 0.0, rightStartPos+blackBarWidth, (float)scn_height, color);
    }

    GPU_Flip(renderer);

    return true;
}

void Render::draw_frame(uint16_t* pixels)
{
    uint32_t* spix = screen_pixels;

    // Lookup real RGB value from rgb array for backbuffer
    for (int i = 0; i < (src_width * src_height); i++)
        *(spix++) = rgb[*(pixels++)];
}
