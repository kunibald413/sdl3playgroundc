#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>

const int screenW = 680;
const int screenH = 480;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

typedef struct Tex {
    SDL_Texture* Texture;
    int Width;
    int Height;
} Tex;

void Tex_Destroy(Tex* tex) {
    if (tex->Texture != NULL) {
        SDL_DestroyTexture(tex->Texture);
    }
    tex->Height=0;
    tex->Width=0;
    tex->Texture=NULL;
}

bool Tex_Load(Tex* tex, const char* path) {
    printf("0 Tex load\n");
    Tex_Destroy(tex);
    
    printf("1 img load\n");
    SDL_Surface* surface = IMG_Load(path);
    
    if (!surface) {
        SDL_Log("Failed to load surface %s! %s", path, SDL_GetError());
        return false;
    }
    
    printf("2 img crate tex from surface\n");
    if (tex->Texture = SDL_CreateTextureFromSurface(gRenderer, surface)){
        tex->Height=surface->h;
        tex->Width=surface->w;
    }
    
    printf("3 destroy surface\n");
    SDL_DestroySurface(surface);

    return tex->Texture != NULL;
}


bool Init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failed to init SDL! Error: %s", SDL_GetError());
        return false;
    }
    if (!SDL_CreateWindowAndRenderer("Best", screenW, screenH, 0, &gWindow, &gRenderer)) {
        SDL_Log("Failed create window and renderer! Error: %s", SDL_GetError());
        return false;
    }
    return true;
}

int main(int argc, char* argv[])  {

    if (!Init()) {
        return 1;
    }

    #if 1
    Tex tex = {0};
    if (!Tex_Load(&tex, "assets/colors.png")) {
        SDL_Log("Failed to load texture! %s", SDL_GetError());
        return 1;
    }
    printf("Tex loaded w: %d h: %d\n", tex.Width, tex.Height);
    #endif

    bool is_running = true;
    SDL_Event e;
    SDL_zero(e);
    SDL_Color clear_color = {0x33, 0x33, 0x33, 0xFF};

    float normalized_y = 0.0;
    float normed_x = 1.0;
    while(is_running) {
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                is_running = false;
            } else if (e.type == SDL_EVENT_MOUSE_MOTION) {
                normalized_y = e.motion.y / screenH;
                normed_x = e.motion.x / screenW;
                // printf("yrel: %.3f y: %.3f normalized_y: %.3f\n", yrel, y, normalized_y);
            }
        }

        SDL_SetRenderDrawColor(gRenderer, clear_color.r, clear_color.g, clear_color.b, clear_color.a);
        SDL_RenderClear(gRenderer);

        // DO STUFF
        SDL_SetTextureAlphaMod(tex.Texture, 0xFF *  (1 - normalized_y));
        SDL_SetTextureColorMod(tex.Texture, 0xFF * normed_x, 0xFF * normed_x, 0xFF * normed_x);
        SDL_SetTextureBlendMode(tex.Texture, SDL_BLENDMODE_BLEND);
        SDL_FRect dst_rect = {.x = 10, .y = 10, .w = tex.Width, .h = tex.Height};
        SDL_RenderTexture(gRenderer, tex.Texture, NULL, &dst_rect);

        // PRESENT
        SDL_RenderPresent(gRenderer);

        SDL_Delay(1);
    }

    return 0;
}