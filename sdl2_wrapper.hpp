/*
    @author yuanshixi
    @brief  sdl2 wrapper, written in C++11, put SDL, SDL_image, SDL_ttf, SDL_mixer together.
*/
#ifndef __SDL2_WRAPPER_HPP__
#define __SDL2_WRAPPER_HPP__

#include <string>
#include <exception>
#include <cstdint>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

/******************************* sdl2 part. **********************************/
namespace sdl2 {
    class SDL2Exception : public std::exception {
    protected:
        std::string errMsg;
    public:
        SDL2Exception() : errMsg{ "SDL2 exception occurs" } {}

        SDL2Exception(const char* sdlErrMsg) : errMsg { sdlErrMsg } {}

        /*
            because the parameter evaluate order is not specified in C++ standard, 
            so you have to use the following code to save the sdl error message first, 
            then throw this exception:
            
            const char* sdlErrMsg = SDL_GetError();
            throw SDL2Exception{ "Your message", sdlErrMsg };
        */
        SDL2Exception(const std::string& userMsg, const char* sdlErrMsg) : errMsg{} {
            errMsg += userMsg;
            errMsg += ", errMsg: ";
            errMsg += sdlErrMsg;
        }

        virtual ~SDL2Exception() {}

        virtual const char* what() const noexcept {
            return errMsg.c_str();
        }
    };

    class SDL2Env {
    public:
        SDL2Env(uint32_t flags) {
            if (SDL_Init(flags) < 0) {
                const char* sdlErrMsg = SDL_GetError();
                throw SDL2Exception{ "SDL_Init() failed", sdlErrMsg };
            }
        }

        SDL2Env(const SDL2Env&) = delete;
        SDL2Env& operator=(const SDL2Env&) = delete;
        SDL2Env(SDL2Env &&) = delete;
        SDL2Env& operator=(SDL2Env &&) = delete;

        ~SDL2Env() {
            SDL_Quit();
        }
    };

    class Window {
        SDL_Window* window;
    public:
        Window(const char* title, int x, int y, int w, int h, Uint32 flags) {
            if ((window = SDL_CreateWindow(title, x, y, w, h, flags)) == nullptr) {
                const char* sdlErrMsg = SDL_GetError();
                throw SDL2Exception{ "SDL_CreateWindow() failed", sdlErrMsg };
            }
        }

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        Window(Window && other) noexcept 
            : window { other.window }  
        {
            other.window = nullptr;
        }

        Window& operator=(Window&& other) noexcept {
            if (this != &other) {
                window = other.window;
                other.window = nullptr;
            }

            return *this;
        }

        ~Window() {
            if (window) {
                SDL_DestroyWindow(window);
            }
        }

        SDL_Window* get() noexcept { 
            return window; 
        }
    };

    class Renderer {
        SDL_Renderer* renderer;
    public:
        Renderer(Window& window, int index, uint32_t flags) {
            if ((renderer = SDL_CreateRenderer(window.get(), index, flags)) == nullptr) {
                const char* sdlErrMsg = SDL_GetError();
                throw SDL2Exception{ "SDL_CreateRenderer() failed", sdlErrMsg };
            }
        }

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        Renderer(Renderer && other) noexcept 
            : renderer { other.renderer }  
        {
            other.renderer = nullptr;
        }

        Renderer& operator=(Renderer&& other) noexcept {
            if (this != &other) {
                renderer = other.renderer;
                other.renderer = nullptr;
            }

            return *this;
        }

        ~Renderer() {
            if (renderer) {
                SDL_DestroyRenderer(renderer);
            }
        }

        SDL_Renderer* get() noexcept {
            return renderer;
        }
    };

    class Surface {
        SDL_Surface* surface;
    public:
        Surface() : surface{ nullptr } {}
        Surface(SDL_Surface* _surf) : surface{ _surf } {}

        Surface(const Surface&) = delete;
        Surface& operator=(const Surface&) = delete;

        Surface(Surface && other) noexcept 
            : surface { other.surface }  
        {
            other.surface = nullptr;
        }

        Surface& operator=(Surface&& other) noexcept {
            if (this != &other) {
                surface = other.surface;
                other.surface = nullptr;
            }

            return *this;
        }

        ~Surface () {
            if (surface) {
                SDL_FreeSurface(surface);
            }
        }

        SDL_Surface* get() noexcept {
            return surface;
        }

        SDL_Surface* operator->() noexcept {
            return surface;
        }

        void set(SDL_Surface* _surf) noexcept {
            surface = _surf;
        }
    };

    class Texture {
        SDL_Texture* texture;
    public:
        Texture() : texture{ nullptr } {}
        Texture(SDL_Texture* _texture) : texture{ _texture } {}

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture && other) noexcept 
            : texture { other.texture }  
        {
            other.texture = nullptr;
        }

        Texture& operator=(Texture&& other) noexcept {
            if (this != &other) {
                texture = other.texture;
                other.texture = nullptr;
            }

            return *this;
        }

        ~Texture() {
            if (texture) {
                SDL_DestroyTexture(texture);
            }
        }

        SDL_Texture* get () noexcept {
            return texture;
        }

        SDL_Texture* operator->() noexcept {
            return texture;
        }

        void set(SDL_Texture* _texture) noexcept {
            texture = _texture;
        }
    };

    /*
        SDL2 can load bmp directly, but if you want to load .jpg or others,
        then use the SDL_image.
    */
    class Bmp {
        Surface surface;
    public:
        Bmp(const std::string& bmpFilePath) : surface{} {
            SDL_Surface* surf = SDL_LoadBMP(bmpFilePath.c_str());
            if (surf == nullptr) {
                const char* sdlErrMsg = SDL_GetError();
                throw SDL2Exception{ "SDL_LoadBMP() failed", sdlErrMsg };
            }

            surface.set(surf);
        }

        ~Bmp() {}

        SDL_Surface* get() noexcept {
            return surface.get();
        }

        SDL_Surface* operator->() noexcept {
            return surface.get();
        }
    };

    class Timer {
        SDL_TimerID id;
    public:
        Timer(uint32_t interval, SDL_TimerCallback callback, void *param) {
            id = SDL_AddTimer(interval, callback, param);

            if (id == 0) {
                const char* sdlErrMsg = SDL_GetError();
                throw SDL2Exception{ "SDL_AddTimer() failed", sdlErrMsg };
            }
        }

        Timer(const Timer&) = delete;
        Timer& operator=(const Timer&) = delete;

        Timer(Timer && other) noexcept 
            : id { other.id }  
        {
            other.id = 0;
        }

        Timer& operator=(Timer&& other) noexcept {
            if (this != &other) {
                id = other.id;
                other.id = 0;
            }

            return *this;
        }

        ~Timer() {
            if (id != 0) {
                SDL_RemoveTimer(id);
            }
        }

        SDL_TimerID get() const noexcept {
            return id;
        }
    };

    void sdl_fill_rect(SDL_Surface * dst, const SDL_Rect* rect, uint32_t color) {
        if (SDL_FillRect(dst, rect, color) < 0) {
            const char* sdlErrMsg = SDL_GetError();
            throw SDL2Exception{ "SDL_FillRect() failed", sdlErrMsg };
        }
    }

    /*
        You must never destroy the returned surface, according to the SDL2 document.
        and you can't set the returned surface to Surface.
    */
    SDL_Surface* sdl_window_get_surface(Window& window) noexcept {
        return SDL_GetWindowSurface(window.get());
    }

    void sdl_update_window_surface(Window& window) {
        if (SDL_UpdateWindowSurface(window.get()) < 0) {
            const char* sdlErrMsg = SDL_GetError();
            throw SDL2Exception{ "SDL_UpdateWindowSurface() failed", sdlErrMsg };
        }
    }

    void sdl_blit_surface(SDL_Surface* src, const SDL_Rect* srcRect, SDL_Surface* dst, SDL_Rect* dstRect) {
        if (SDL_BlitSurface(src, srcRect, dst, dstRect) < 0) {
            const char* sdlErrMsg = SDL_GetError();
            throw SDL2Exception{ "SDL_BlitSurface() failed", sdlErrMsg };
        }
    }

    void sdl_set_render_draw_color(Renderer& renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        if (SDL_SetRenderDrawColor(renderer.get(), r, g, b, a) < 0) {
            const char* sdlErrMsg = SDL_GetError();
            throw SDL2Exception{ "SDL_SetRenderDrawColor() failed", sdlErrMsg };
        }
    }

    void sdl_render_clear(Renderer& renderer) {
        if (SDL_RenderClear(renderer.get()) < 0) {
            const char* sdlErrMsg = SDL_GetError();
            throw SDL2Exception{ "SDL_RenderClear() failed", sdlErrMsg };
        }
    }

    void sdl_set_render_draw_blend_mode(Renderer& renderer, SDL_BlendMode blendMode) {
        if (SDL_SetRenderDrawBlendMode(renderer.get(), blendMode) < 0) {
            const char* sdlErrMsg = SDL_GetError();
            throw SDL2Exception{ "SDL_SetRenderDrawBlendMode() failed", sdlErrMsg };
        }
    }

    void sdl_render_fill_rect(Renderer& renderer, const SDL_Rect* rect) {
        if (SDL_RenderFillRect(renderer.get(), rect) < 0) {
            const char* sdlErrMsg = SDL_GetError();
            throw SDL2Exception{ "SDL_RenderFillRect() failed", sdlErrMsg };
        }
    }

    void sdl_render_draw_rect(Renderer& renderer, const SDL_Rect* rect) {
        if (SDL_RenderDrawRect(renderer.get(), rect) < 0) {
            const char* sdlErrMsg = SDL_GetError();
            throw SDL2Exception{ "SDL_RenderDrawRect() failed", sdlErrMsg };
        }
    }

    void sdl_render_draw_line(Renderer& renderer, int x1, int y1, int x2, int y2) {
        if (SDL_RenderDrawLine(renderer.get(), x1, y1, x2, y2) < 0) {
            const char* sdlErrMsg = SDL_GetError();
            throw SDL2Exception{ "SDL_RenderDrawLine() failed", sdlErrMsg };
        }
    }

    void sdl_render_draw_lines(Renderer& renderer, const SDL_Point* points, int pointNum) {
        if (SDL_RenderDrawLines(renderer.get(), points, pointNum) < 0) {
            const char* sdlErrMsg = SDL_GetError();
            throw SDL2Exception{ "SDL_RenderDrawLines() failed", sdlErrMsg };
        }
    }

    void sdl_render_copy(Renderer& renderer, Texture& texture, const SDL_Rect* srcRect, const SDL_Rect* dstRect) {
        if (SDL_RenderCopy(renderer.get(), texture.get(), srcRect, dstRect) < 0) {
            const char* sdlErrMsg = SDL_GetError();
            throw SDL2Exception{ "SDL_RenderCopy() failed", sdlErrMsg };
        }
    }

    void sdl_render_copy_ex(Renderer& renderer, 
                            Texture& texture, 
                            const SDL_Rect* srcRect, 
                            const SDL_Rect* dstRect, 
                            const double angle,
                            const SDL_Point* center,
                            const SDL_RendererFlip flip) {
        if (SDL_RenderCopyEx(renderer.get(), texture.get(), srcRect, dstRect, angle, center, flip) < 0) {
            const char* sdlErrMsg = SDL_GetError();
            throw SDL2Exception{ "SDL_RenderCopyEx() failed", sdlErrMsg };
        }
    }

    void sdl_render_set_scale(Renderer& renderer, float scaleX, float scaleY) {
        if (SDL_RenderSetScale(renderer.get(), scaleX, scaleY) < 0) {
            const char* sdlErrMsg = SDL_GetError();
            throw SDL2Exception{ "SDL_RenderSetScale() failed", sdlErrMsg };
        }
    }

    void sdl_render_set_viewport(Renderer& renderer, const SDL_Rect* rect) {
        if (SDL_RenderSetViewport(renderer.get(), rect) < 0) {
            const char* sdlErrMsg = SDL_GetError();
            throw SDL2Exception{ "SDL_RenderSetViewport() failed", sdlErrMsg };
        }
    }

    void sdl_render_present(Renderer& renderer) noexcept {
        SDL_RenderPresent(renderer.get());
    }

    Texture sdl_create_texture_from_surface(Renderer& renderer, SDL_Surface* surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer.get(), surface);
        if (texture == nullptr) {
            const char* sdlErrMsg = SDL_GetError();
            throw SDL2Exception{ "SDL_CreateTextureFromSurface() failed", sdlErrMsg };
        }

        return Texture{ texture };
    }

    void sdl_query_texture(Texture& texture, uint32_t* format, int* access, int* w, int* h) {
        if (SDL_QueryTexture(texture.get(), format, access, w, h) < 0) {
            const char* sdlErrMsg = SDL_GetError();
            throw SDL2Exception{ "SDL_QueryTexture() failed", sdlErrMsg };
        }
    }
}

/******************************* sdl2 ttf part. **********************************/
namespace sdl2 {
    class SDL2TTF {
    public:
        SDL2TTF() {
            if (TTF_Init() < 0) {
                const char* ttfErrMsg = TTF_GetError();
                throw SDL2Exception{ "TTF_Init() failed", ttfErrMsg };
            }
        }

        SDL2TTF(const SDL2TTF&) = delete;
        SDL2TTF& operator=(const SDL2TTF&) = delete;
        SDL2TTF(SDL2TTF&&) = delete;
        SDL2TTF& operator=(SDL2TTF&&) = delete;

        ~SDL2TTF() {
            TTF_Quit();
        }
    };

    class Font {
        TTF_Font* font;
    public:
        Font(const std::string& fontFilePath, int pointSize) {
            if ((font = TTF_OpenFont(fontFilePath.c_str(), pointSize)) == nullptr) {
                const char* ttfErrMsg = TTF_GetError();
                throw SDL2Exception{ "TTF_OpenFont() failed", ttfErrMsg };
            }
        }

        Font(const Font&) = delete;
        Font& operator=(const Font&) = delete;

        Font(Font && other) noexcept 
            : font { other.font }  
        {
            other.font = nullptr;
        }

        Font& operator=(Font&& other) noexcept {
            if (this != &other) {
                font = other.font;
                other.font = nullptr;
            }

            return *this;
        }

        ~Font() {
            if (font) {
                TTF_CloseFont(font);
            }
        }

        TTF_Font* get() {
            return font;
        }
    };

    Surface ttf_render_utf8_blended(Font& font, const std::string& text, SDL_Color fg) {
        SDL_Surface* surf = TTF_RenderUTF8_Blended(font.get(), text.c_str(), fg);

        if (surf == nullptr) {
            const char* ttfErrMsg = TTF_GetError();
            throw SDL2Exception{ "TTF_RenderUTF8_Blended() failed", ttfErrMsg };
        }

        return Surface{ surf };
    }

    Surface ttf_render_text_solid(Font& font, const std::string& text, SDL_Color fg) {
        SDL_Surface* surf = TTF_RenderText_Solid(font.get(), text.c_str(), fg);
        if (surf == nullptr) {
            const char* ttfErrMsg = TTF_GetError();
            throw SDL2Exception{ "TTF_RenderText_Solid() failed", ttfErrMsg };
        }

        return Surface{ surf };
    }
}

/******************************* sdl2 image part. **********************************/
namespace sdl2 {
    Surface img_load(const std::string& filePath) {
        SDL_Surface* surf = IMG_Load(filePath.c_str());
        if (surf == nullptr) {
            const char* sdlErrMsg = SDL_GetError();
            throw SDL2Exception{ "IMG_Load() failed", sdlErrMsg };
        }

        return Surface{ surf };
    }
}

/******************************* sdl2 mixer part. **********************************/
namespace sdl2 {
    class SDL2Mixer {
    public:
        SDL2Mixer() {}

        SDL2Mixer(const SDL2Mixer&) = delete;
        SDL2Mixer& operator=(const SDL2Mixer&) = delete;
        SDL2Mixer(SDL2Mixer&&) = delete;
        SDL2Mixer& operator=(SDL2Mixer&&) = delete;

        ~SDL2Mixer() {
            Mix_Quit();
        }

        // this call is optional.
        int do_mixer_init(int flags) noexcept {
            return Mix_Init(flags);
        }
    };

    class MixOpenAudio {
    public:
        MixOpenAudio(int frequency, uint16_t format, int channels, int chunksize) {
            if (Mix_OpenAudio(frequency, format, channels, chunksize) < 0) {
                const char* mixErrMsg = Mix_GetError();
                throw SDL2Exception{ "Mix_OpenAudio() failed", mixErrMsg };
            }
        }

        MixOpenAudio(const MixOpenAudio&) = delete;
        MixOpenAudio& operator=(const MixOpenAudio&) = delete;
        MixOpenAudio(MixOpenAudio&&) = delete;
        MixOpenAudio& operator=(MixOpenAudio&&) = delete;

        ~MixOpenAudio() {
            Mix_CloseAudio();
        }
    };

    class MixChunk {
        Mix_Chunk* chunk;
    public:
        MixChunk() : chunk { nullptr } {}
        MixChunk(Mix_Chunk* _chunk) : chunk{ _chunk } {}

        MixChunk(const MixChunk&) = delete;
        MixChunk& operator=(const MixChunk&) = delete;

        MixChunk(MixChunk && other) noexcept 
            : chunk { other.chunk }  
        {
            other.chunk = nullptr;
        }

        MixChunk& operator=(MixChunk&& other) noexcept {
            if (this != &other) {
                chunk = other.chunk;
                other.chunk = nullptr;
            }

            return *this;
        }

        ~MixChunk() {
            if (chunk) {
                Mix_FreeChunk(chunk);
            }
        }

        Mix_Chunk* get() noexcept {
            return chunk;
        }
    };

    SDL_RWops* sdl_rw_from_file(const std::string& file, const std::string& mode) {
        SDL_RWops* ops = SDL_RWFromFile(file.c_str(), mode.c_str());
        if (ops == nullptr) {
            const char* sdlErrMsg = SDL_GetError();
            throw SDL2Exception{ "SDL_RWFromFile() failed", sdlErrMsg };
        }

        return ops;
    }

    MixChunk mix_load_wav_rw(SDL_RWops* src, int freeSrc) {
        Mix_Chunk* chunk = Mix_LoadWAV_RW(src, freeSrc);
        if (chunk == nullptr) {
            const char* mixErrMsg = Mix_GetError();
            throw SDL2Exception{ "Mix_LoadWAV_RW() failed", mixErrMsg };
        }

        return MixChunk{ chunk };
    }

    void mix_play_channel(int channel, MixChunk& chunk, int loops) {
        if (Mix_PlayChannel(channel, chunk.get(), loops) < 0) {
            const char* mixErrMsg = Mix_GetError();
            throw SDL2Exception{ "Mix_PlayChannel() failed", mixErrMsg };
        }
    }
}

#endif
