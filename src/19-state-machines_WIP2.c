#include <stdio.h>
#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>


const int screenW = 680;
const int screenH = 480;
const int screen_FPS = 60;
const Uint64 ns_per_frame = 1'000'000'000 / screen_FPS;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;


typedef struct Tex {
    SDL_Texture* SDL_Texture;
    int Width;
    int Height;
} Tex;

void Tex_Destroy(Tex* t) {
    if (t->SDL_Texture) {
        SDL_DestroyTexture(t->SDL_Texture);
    }
    t->Height = 0;
    t->Width = 0;
}

bool Tex_Load(Tex* out_tex, const char* path) {
    SDL_Surface* s = IMG_Load(path);
    if (!s) {
        SDL_Log("Failed to load surface %s! err: %s", path, SDL_GetError());
        return false;
    }
    SDL_SetSurfaceColorKey(s, true, SDL_MapSurfaceRGB(s, 0x00, 0xff, 0xff)); // cyan

    SDL_Texture* t = SDL_CreateTextureFromSurface(gRenderer, s);

    if (t != NULL) {
        out_tex->SDL_Texture = t;
        out_tex->Height = s->h;
        out_tex->Width = s->w;
    } else {
        SDL_Log("failed to create texture from surface %s! error: ", path, SDL_GetError());
    }
    SDL_DestroySurface(s);
    
    return t != NULL;
}

void Tex_Render(Tex t, float x, float y) {
    SDL_FRect* src_rect = NULL;
    SDL_FRect dest_rect = {
        .x = x,
        .y = y,
        .w = t.Width,
        .h = t.Height
    };
    SDL_RenderTexture(gRenderer, t.SDL_Texture, src_rect, &dest_rect);
}

bool Init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failed to init sdl! sdl error: %s", SDL_GetError());
        return false;
    }
    SDL_WindowFlags wflags = 0;
    if (!SDL_CreateWindowAndRenderer("Best window", screenW, screenH, wflags, &gWindow, &gRenderer)) {
        SDL_Log("Failed to init sdl! sdl error: %s", SDL_GetError());
        return false;
    };
    
    if (!TTF_Init()) {
        SDL_Log("Failed to init TTF! sdl error: %s", SDL_GetError());
        return false;
    }

    return true;
}

bool LoadMedia() {
    gFont = TTF_OpenFont("assets/Figtree-Light.ttf", 20);
    if (!gFont) {
        SDL_Log("Failed to load font! err: %s", SDL_GetError());
    }
    return gFont != NULL;
}

int main (int arg_count, char* argv[]) {

    if (!Init()) {
        return 1;
    }

    if (!LoadMedia()) {
        return 2;
    }

    Tex dot_tex = {0};
    if (!Tex_Load(&dot_tex, "assets/dot.png")) {
        return 3;
    }
    Tex bg_tex = {0};
    if (!Tex_Load(&bg_tex, "assets/green-overworld.png")) {
        return 3;
    }
    Tex house_tex = {0};
    if (!Tex_Load(&house_tex, "assets/blue-house.png")) {
        return 3;
    }
    Tex red_house_tex = {0};
    if (!Tex_Load(&red_house_tex, "assets/red-house.png")) {
        return 3;
    }

    bool quit = false;
    SDL_Event e;
    SDL_zero(e);
    while(!quit) {
        Uint64 frame_start_ns = SDL_GetTicksNS();
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
            // input
        }

        // logic

        SDL_SetRenderDrawColor(gRenderer, 0x00,0x33,0x55, 0xff);
        SDL_RenderClear(gRenderer);


        // DO RENDER
        Tex_Render(bg_tex, 0,0);
        Tex_Render(dot_tex, 200, 200);
        Tex_Render(red_house_tex, 10, 10);
        Tex_Render(house_tex, 50, 150);


        SDL_RenderPresent(gRenderer);

        Uint64 frame_elapsed_ns = SDL_GetTicksNS() - frame_start_ns;
        if (frame_elapsed_ns < ns_per_frame) {
            SDL_DelayNS(ns_per_frame - frame_elapsed_ns);
        }
    }

    return 0;
}