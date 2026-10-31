#include <stdio.h>
#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>


const int screenW = 680;
const int screenH = 480;
const int screenFPS = 60;

#define NS_PER_SEC 1'000'000'000ULL
const Uint64 ns_per_frame = NS_PER_SEC / screenFPS;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;

typedef struct Texture {
    SDL_Texture* SDL_Texture;
    int Width;
    int Height;
} Texture;

Texture Load_Texture(const char* path) {
    Texture t = {0};
    SDL_Surface* s = IMG_Load(path);
    if (!s) {
        return t;
    }

    SDL_SetSurfaceColorKey(s, true, SDL_MapSurfaceRGB(s, 0x00, 0xFF, 0xFF));

    SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, s);

    t.SDL_Texture = texture;
    t.Width = s->h;
    t.Height = s->h;

    SDL_DestroySurface(s);

    return t;
}

bool Init() {
    bool ok = SDL_Init(SDL_INIT_VIDEO);
    ok = SDL_CreateWindowAndRenderer("Best State", screenW, screenH, 0, &gWindow, &gRenderer);
    if (!ok) {
        SDL_Log("Failed to init! sdl error: %s", SDL_GetError());
        return false;
    }
    if (!TTF_Init()) {
        SDL_Log("Failed to init TTF! sdl error: %s", SDL_GetError());
        return false;
    }
    return ok;
}

bool LoadMedia() {
    gFont = TTF_OpenFont("assets/Figtree-Light.ttf", 20);
    if (gFont == NULL) {
        SDL_Log("Failed to load font! sdl error: %s", SDL_GetError());
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {

    // init
    if (!Init()) {
        return 1;
    }

    // load media

    if (!LoadMedia()) {
        return 2;
    }

    Texture bg_texture = Load_Texture("assets/green-overworld.png");
    if (bg_texture.SDL_Texture == NULL) {
        SDL_Log("Failed to load bg texture! err %s", SDL_GetError());
        return 1;
    }
    Texture dot_texture = Load_Texture("assets/dot.png");
    if (dot_texture.SDL_Texture == NULL) {
        SDL_Log("Failed to load dot texture! err %s", SDL_GetError());
        return 1;
    }

    char fps_buff[256];
    Uint64 last_fps = 0;
    
    bool quit = false;
    SDL_Event e;
    SDL_zero(e);

    // basic loop with fixed frame rate
    while(!quit)  {

        Uint64 frame_start_ns = SDL_GetTicksNS();

        while(SDL_PollEvent(&e)) {
            //  handle vents
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        //  run game logic

        SDL_SetRenderDrawColor(gRenderer, 0x11, 0x33,0x44, 0xff);
        SDL_RenderClear(gRenderer);

        //  render

        SDL_FRect dst_rect = {.x =0, .y =0, .w = bg_texture.Width, .h = bg_texture.Height};
        SDL_RenderTexture(gRenderer, bg_texture.SDL_Texture, NULL, &dst_rect);
        
        SDL_FRect dst_rect_dot = {.x = 150, .y =100, .w = dot_texture.Width, .h = dot_texture.Height};
        SDL_RenderTexture(gRenderer, dot_texture.SDL_Texture, NULL, &dst_rect_dot);

        // render fps counter
        if (last_fps > 0) {
            snprintf(fps_buff, sizeof(fps_buff), "FPS: %llu", last_fps);
            SDL_Color fps_color = {0xff, 0xff, 0xff, 0xff};
            SDL_Surface* fps_surface = TTF_RenderText_Blended(gFont, fps_buff, 0, fps_color);
            SDL_Texture* fps_texture = SDL_CreateTextureFromSurface(gRenderer, fps_surface);
            SDL_FRect fps_rect = {.w = fps_surface->w, .h = fps_surface->h, .y = 0, .x = screenW  - fps_surface->w * 1.1};
            SDL_RenderTexture(gRenderer, fps_texture, NULL, &fps_rect);
            SDL_DestroySurface(fps_surface);
            SDL_DestroyTexture(fps_texture);
        }

        SDL_RenderPresent(gRenderer);

        Uint64 elapsed_ns = SDL_GetTicksNS() - frame_start_ns;
        if (elapsed_ns < ns_per_frame) {
            SDL_DelayNS(ns_per_frame - elapsed_ns);
            //printf("elapsed microseconds: %llu\n", elapsed_ns / 1000);
        }
        last_fps = NS_PER_SEC / (SDL_GetTicksNS() - frame_start_ns);
    }

    printf("Done\n");

    return 0;
}