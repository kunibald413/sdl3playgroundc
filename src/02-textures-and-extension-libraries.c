
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>

static const int screenW = 640;
static const int screenH = 480;

SDL_Window* globalWindow = NULL;
SDL_Renderer* globalRenderer = NULL;

bool init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
        return false;
    }

    SDL_WindowFlags flags = 0;
    if(!SDL_CreateWindowAndRenderer("Best SDL3 in C", screenW, screenH, flags, &globalWindow, &globalRenderer)) {
        SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

typedef struct BestTexture {
    SDL_Texture* Texture;
    int Width;
    int Height;
} BestTexture;

void BestTexture_Zero(BestTexture* tex) {
    tex->Height = 0;
    tex->Width = 0;
    tex->Texture = NULL;
}

void BestTexture_Destroy(BestTexture* tex) {
    if (tex->Texture != NULL) {
        SDL_DestroyTexture(tex->Texture);
    }
    BestTexture_Zero(tex);
}

bool BestTexture_LoadFromFile(BestTexture* tex, 
    const char* path, 
    SDL_Renderer* renderer) 
{
    BestTexture_Destroy(tex); // prevent leaks

    SDL_Surface* loaded_surface = IMG_Load(path);
    if (loaded_surface == NULL) {
        SDL_Log("Failed to load image %s! SDL_image error: %s\n", path, SDL_GetError());
    } else {
        tex->Texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
        if (tex->Texture == NULL) {
            SDL_Log("Unable to create texture from loaded surface for %s! SDL error: %s\n", path, SDL_GetError());
        } else {
            tex->Width = loaded_surface->w;
            tex->Height = loaded_surface->h;
        }
    
        SDL_DestroySurface(loaded_surface);
    }

    return tex->Texture != NULL;
}

void BestTexture_Render(BestTexture* tex, float x, float y, SDL_Renderer* renderer) {
    SDL_FRect destRect = {
        .x = x,
        .y = y,
        .w = tex->Width,
        .h = tex->Height
    };
    SDL_RenderTexture(renderer, tex->Texture, NULL, &destRect);
}

int main(int argc, char *argv[]) {
    // init
    if (!init()) {
        SDL_Log("Failed to init\n");
        return 1;
    }

    // load media
    BestTexture tex = {0};
    BestTexture_Zero(&tex);
    if (!BestTexture_LoadFromFile(&tex, "assets/loaded.png", globalRenderer)) {
        SDL_Log("Failed to load texture from file!\n");
        return 1;
    }

    // loop
    bool running = true;
    SDL_Event event;
    SDL_zero(event);
    while(running) {
        // input
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(globalRenderer, 0x11, 0x11, 0x11, 0xFF);
        SDL_RenderClear(globalRenderer);

        BestTexture_Render(&tex, 
            screenW / 2 - tex.Width / 2, 
            screenH / 2 - tex.Height / 2, 
            globalRenderer);

        SDL_RenderPresent(globalRenderer);

        SDL_Delay(1000 / 60);
    }

    // waste of cpu/user time?
    BestTexture_Destroy(&tex);
    SDL_DestroyRenderer(globalRenderer);
    SDL_DestroyWindow(globalWindow);
    globalRenderer = NULL;
    globalWindow = NULL;
    SDL_Quit();

    return 0;
}