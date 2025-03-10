#include <iostream>
#include <vector>
#include "sdl2_wrapper.hpp"

#undef main

constexpr const char* WINDOW_TITLE = "Rect";

constexpr uint32_t WINDOW_WIDTH = 600;
constexpr uint32_t WINDOW_HEIGHT = 480;

constexpr uint32_t FRAME_RATE = 60;
constexpr uint32_t FRAME_MILLI_SECONDS = 1000/ FRAME_RATE;

constexpr uint32_t DEFAULT_FREQUENCY = 48000;
constexpr uint32_t DEFAULT_CHANNEL_NUM = 8;
constexpr uint32_t DEFAULT_CHUNK_SIZE = 2048;

void draw_rect(sdl2::Window& window) {
    SDL_Surface* surface = sdl2::sdl_window_get_surface(window);
    
    // background color.
    SDL_Rect rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    sdl2::sdl_fill_rect(surface, &rect, 0xffffffff);

    // rect.
    SDL_Rect mvRect = { 10, 10, 30, 30 };
    sdl2::sdl_fill_rect(surface, &mvRect, 0xff00ff00);

    sdl2::sdl_update_window_surface(window);
}

void draw_img(sdl2::Surface& img, sdl2::Window& window) {
    SDL_Surface* windowSurf = sdl2::sdl_window_get_surface(window);
    SDL_Rect rect = { 0, 0, img->w, img->h };

    sdl2::sdl_blit_surface(img.get(), &rect, windowSurf, &rect);
    sdl2::sdl_update_window_surface(window);
}

void draw_bmp(sdl2::Bmp& bmp, sdl2::Window& window) {
    SDL_Surface* windowSurf = sdl2::sdl_window_get_surface(window);
    SDL_Rect rect = { 0, 0, bmp->w, bmp->h };

    sdl2::sdl_blit_surface(bmp.get(), &rect, windowSurf, &rect);
    sdl2::sdl_update_window_surface(window);
}

void draw_text(sdl2::Font& font, sdl2::Window& window) {
    SDL_Color color = { 255, 255, 255, 255 };
    sdl2::Surface textSurface = sdl2::ttf_render_utf8_blended(font, "Hatsune Miku", color);

    SDL_Rect srcRect = { 0, 0, textSurface->w, textSurface->h };
    SDL_Rect dstRect = { 30, 30, textSurface->w, textSurface->h };
    SDL_Surface* windowSurface = sdl2::sdl_window_get_surface(window);

    sdl2::sdl_blit_surface(textSurface.get(), &srcRect, windowSurface, &dstRect);
    sdl2::sdl_update_window_surface(window);
}

void render_simple_rect(sdl2::Renderer& renderer) {
    // clear the background, with color white.
    sdl2::sdl_set_render_draw_color(renderer, 255, 255, 255, 255);
    sdl2::sdl_render_clear(renderer);

    // render 2 rects.
    sdl2::sdl_set_render_draw_blend_mode(renderer, SDL_BLENDMODE_BLEND);

    sdl2::sdl_set_render_draw_color(renderer, 57, 197, 187, 100);
    SDL_Rect rect_1 = { 0, 0, 100, 100 };
    sdl2::sdl_render_fill_rect(renderer, &rect_1);

    sdl2::sdl_set_render_draw_color(renderer, 198, 53, 63, 155);
    SDL_Rect rect_2 = { 50, 50, 100, 100 };
    sdl2::sdl_render_fill_rect(renderer, &rect_2);

    // must call at last.
    sdl2::sdl_render_present(renderer);
}

void render_line(sdl2::Renderer& renderer) {
    sdl2::sdl_set_render_draw_color(renderer, 255, 255, 255, 255);
    sdl2::sdl_render_clear(renderer);

    sdl2::sdl_set_render_draw_color(renderer, 255, 128, 0, 255);
    
    std::vector<SDL_Point> points;
    points.emplace_back(SDL_Point{ 50, 50 });
    points.emplace_back(SDL_Point{ 200, 200 });
    points.emplace_back(SDL_Point{ 300, 400 });

    sdl2::sdl_render_draw_lines(renderer, points.data(), points.size());

    sdl2::sdl_render_present(renderer);
}

void render_bmp(sdl2::Renderer& renderer, sdl2::Texture& texture) {
    sdl2::sdl_set_render_draw_color(renderer, 255, 255, 255, 255);
    sdl2::sdl_render_clear(renderer);

    int bmpWidth, bmpHeight;
    sdl2::sdl_query_texture(texture, nullptr, nullptr, &bmpWidth, &bmpHeight);

    SDL_Rect rect = { 0, 0, bmpWidth, bmpHeight };
    sdl2::sdl_render_copy(renderer, texture, nullptr, &rect);

    sdl2::sdl_render_present(renderer);
}

void render_bmp_then_flip_and_rotate_and_scale(sdl2::Renderer& renderer, sdl2::Texture& texture) {
    sdl2::sdl_set_render_draw_color(renderer, 255, 255, 255, 255);
    sdl2::sdl_render_clear(renderer);

    int bmpWidth, bmpHeight;
    sdl2::sdl_query_texture(texture, nullptr, nullptr, &bmpWidth, &bmpHeight);

    SDL_Rect rect = { 0, 0, bmpWidth, bmpHeight };
    sdl2::sdl_render_set_scale(renderer, 0.5, 0.5);
    sdl2::sdl_render_copy_ex(renderer, texture, nullptr, &rect, 45, nullptr, SDL_FLIP_HORIZONTAL);

    sdl2::sdl_render_present(renderer);
}

void render_bmp_with_viewport(sdl2::Renderer& renderer, sdl2::Texture& texture) {
    sdl2::sdl_set_render_draw_color(renderer, 255, 255, 255, 255);
    sdl2::sdl_render_clear(renderer);

    int bmpWidth, bmpHeight;
    sdl2::sdl_query_texture(texture, nullptr, nullptr, &bmpWidth, &bmpHeight);

    SDL_Rect rect = { 0, 0, bmpWidth, bmpHeight };
    SDL_Rect viewport = { 0, 0, 300, 260 };

    sdl2::sdl_render_set_viewport(renderer, &viewport);
    sdl2::sdl_render_copy(renderer, texture, nullptr, &rect);

    sdl2::sdl_render_present(renderer);
}

void event_loop(sdl2::Renderer& renderer) {
    SDL_Event event;
    sdl2::Bmp bmp { "./cat.bmp" };
    sdl2::Texture bmpTexture = sdl2::sdl_create_texture_from_surface(renderer, bmp.get());

    while (true) {
        uint32_t begin = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return;
            }
        }

        render_bmp_with_viewport(renderer, bmpTexture);

        uint32_t current = SDL_GetTicks();
        uint32_t cost = current - begin;

        if (FRAME_MILLI_SECONDS > cost) {
            SDL_Delay(FRAME_MILLI_SECONDS - cost);
        }
    }
}

int main() {
    try {
        sdl2::SDL2Env env{ SDL_INIT_VIDEO | SDL_INIT_AUDIO };
        sdl2::SDL2TTF ttf;
        sdl2::SDL2Mixer mixer;

        sdl2::Window window{ WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0 };
        sdl2::Renderer renderer{ window, -1, SDL_RENDERER_ACCELERATED };

        // play a sound.
        sdl2::MixOpenAudio moa{ DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, DEFAULT_CHANNEL_NUM, DEFAULT_CHUNK_SIZE };
        SDL_RWops* ops = sdl2::sdl_rw_from_file("./test.ogg", "rb");
        sdl2::MixChunk chunk = sdl2::mix_load_wav_rw(ops, 1);
        sdl2::mix_play_channel(-1, chunk, -1);

        // render picture.
        event_loop(renderer);
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << "\n";
    }

    return 0;
}
