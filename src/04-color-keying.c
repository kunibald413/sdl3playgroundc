#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>

const int screenW = 640;
const int screenH = 640;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

typedef struct Texture {
    SDL_Texture* Texture;
    int Width;
    int Height;
} Texture;

void Texture_Destroy(Texture* tex)
{
    if (tex->Texture)
    {
        SDL_DestroyTexture(tex->Texture);
    }
    tex->Texture = NULL;
    tex->Width=0;
    tex->Height=0;
}

// load tex
bool Texture_LoadFromFile(Texture* tex, const char* path) 
{
    Texture_Destroy(tex);

    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL) 
    {
        SDL_Log("Failed to load surface %s! Error: ", path, SDL_GetError());
        return false;
    }

    Uint32 key = SDL_MapSurfaceRGB(surface, 0x00, 0xFF, 0xFF);
    if (!SDL_SetSurfaceColorKey(surface, true, key))
    {
        SDL_Log("Failed to set surface color key %s! Error: ", path, SDL_GetError());
    }
    else
    {
        if ((tex->Texture = SDL_CreateTextureFromSurface(gRenderer, surface)) == NULL) 
        {
            SDL_Log("Failed to create texture from surface %s! Error: ", path, SDL_GetError());
        }
        else 
        {
            tex->Width=tex->Texture->w;
            tex->Height=tex->Texture->h;
        }
    }

    SDL_DestroySurface(surface);

    return tex->Texture != NULL;
}

void Texture_Render(Texture* tex, float x, float y)
{
    SDL_FRect dst_rect = {
        .w = (float)tex->Width,
        .h = (float)tex->Height,
        .x = x,
        .y = y
    };
    SDL_RenderTexture(gRenderer, tex->Texture, NULL, &dst_rect);
}

bool Init()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer("Best Window", screenW, screenH, 0, &gWindow, &gRenderer);
    return true;
}

int main(int argc, char* argv[])
{
    // init sdl
    if (!Init()) return 1;

    Texture bg_tex;
    Texture_LoadFromFile(&bg_tex, "assets/background.png");

    Texture char_tex;
    Texture_LoadFromFile(&char_tex, "assets/foo.png");

    Texture unit_tex;
    Texture_LoadFromFile(&unit_tex, "assets/0002_1.png");  

    bool is_running = true;
    SDL_Event e;
    SDL_zero(e);
    while(is_running)
    {
        while (SDL_PollEvent(&e)) 
        {
            if (e.type == SDL_EVENT_QUIT)
            {
                is_running = false;
            }
        }
        SDL_SetRenderDrawColor(gRenderer, 0x20, 0x20, 0x20, 0xFF);
        SDL_RenderClear(gRenderer);

        Texture_Render(&bg_tex, 0,0);
        Texture_Render(&char_tex, 150,300);
        Texture_Render(&unit_tex, 300,350);

        SDL_RenderPresent(gRenderer);

        SDL_Delay(1000 / 60);
    }

    // loop
    //  plain loop, render bg and character
    return 0;
}