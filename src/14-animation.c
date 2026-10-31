#include <stdio.h>
#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

const int screenW = 640;
const int screenH = 480;
const int FPS = 60;
const Uint64 NS_PER_FRAME = 1000000000 / FPS;

typedef struct Tex {
    SDL_Texture* Texture;
    int Width;
    int Height;
} Tex;

void Tex_Destroy(Tex* tex) {
    if (tex->Texture) {
        SDL_DestroyTexture(tex->Texture);
    }
    tex->Height = 0;
    tex->Width = 0;
}

bool Tex_LoadFromFile(Tex* tex, const char* path) {
    Tex_Destroy(tex);

    SDL_Surface* surface = IMG_Load(path);
    if (surface) {
        
        // color key cyan
        SDL_SetSurfaceColorKey(surface, true, SDL_MapSurfaceRGB(surface, 0x00, 0xFF,0xFF));
        tex->Texture = SDL_CreateTextureFromSurface(gRenderer, surface);
        if (tex->Texture) {
            tex->Width = surface->w;
            tex->Height = surface->h;
        }
        SDL_DestroySurface(surface);
    }

    return tex->Texture != NULL;
}

bool Init() {
    SDL_Init(SDL_INIT_VIDEO);
    return SDL_CreateWindowAndRenderer("Best window", screenW, screenH, 0, &gWindow, &gRenderer);
}

int main(int argc, char* argv[]) {

    if (!Init()) {
        return 1;
    }

    // load tex
    Tex tex = {0};
    const int foo_width = 64;
    if (!Tex_LoadFromFile(&tex, "assets/foo-sprites.png")) {
        SDL_Log("Failed to load texture! err: %s", SDL_GetError());
        return 1;
    }
    SDL_FRect dest_rect = {
        .x = (screenW - foo_width) / 2,
        .y = (screenH - tex.Height) / 2,
        .w = foo_width,
        .h = tex.Height
    };

    int anim_sprite_index = 0;
    SDL_FRect clip_rect = {
        .x = anim_sprite_index * foo_width,
        .y = 0,
        .w = foo_width,
        .h = tex.Height
    };
    Uint64 last_anim_ns = SDL_GetTicksNS();
    Uint64 anim_interval_ns = NS_PER_FRAME * 8; // show same sprite for 8 frames

    SDL_Event e;
    SDL_zero(e);
    bool is_running = true;

    while(is_running) {

        Uint64 frame_start_ns = SDL_GetTicksNS();

        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                is_running = false;
            }
        }

        // ANIM ENGIINE
        if (last_anim_ns < frame_start_ns && frame_start_ns - last_anim_ns >= anim_interval_ns) {
            last_anim_ns = SDL_GetTicksNS();
            if (anim_sprite_index < 3) {
                anim_sprite_index++;
            } else {
                anim_sprite_index = 0;
            }
            clip_rect.x = anim_sprite_index * foo_width;
        }

        SDL_SetRenderDrawColor(gRenderer, 0x99, 0x99,0x88, 0xff);
        SDL_RenderClear(gRenderer);

        
        SDL_RenderTexture(gRenderer, tex.Texture, &clip_rect, &dest_rect);


        SDL_RenderPresent(gRenderer);

        Uint64 elapsed_ns = SDL_GetTicksNS() - frame_start_ns;
        if (elapsed_ns < NS_PER_FRAME) {
            SDL_DelayNS(NS_PER_FRAME - elapsed_ns);
        }
    }

    return 0;
}