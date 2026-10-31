#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>


const int screenW = 640;
const int screenH = 420;

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
    tex->Height=0;
    tex->Width=0;
    tex->Texture=NULL;
}

bool Texture_LoadFromFile(Texture* tex, const char* path)
{
    Texture_Destroy(tex);

    SDL_Surface* surface = IMG_Load(path);

    if (surface)
    {
        SDL_SetSurfaceColorKey(surface, true, SDL_MapSurfaceRGB(surface, 0x00, 0xFF, 0xFF));

        if (tex->Texture = SDL_CreateTextureFromSurface(gRenderer, surface)) 
        {
            tex->Width = surface->w;
            tex->Height = surface->h;
        }
        else 
        {
            SDL_Log("Faiiled to create texture from surface %s! Error: %s", path, SDL_GetError());
        }
        SDL_DestroySurface(surface);
    }
    else 
    {
        SDL_Log("Faiiled to load surface %s! Error: %s", path, SDL_GetError());
    }

    return tex->Texture != NULL;
}

bool Init()
{
    return SDL_Init(SDL_INIT_VIDEO) 
        && SDL_CreateWindowAndRenderer("Best Window", screenW, screenH, 0, &gWindow, &gRenderer);
}

int main(int argc, char* argv[])
{
    // init sdl
    if (!Init())
    {
        SDL_Log("Failed to init! Error: %s", SDL_GetError());
        return 1;
    }

    // load texture

    Texture arrow = {0};
    Texture_LoadFromFile(&arrow, "assets/0002_1.png");

    // loop
    bool is_running = true;
    SDL_Event e;
    SDL_zero(e);

    SDL_FlipMode flip_mode = SDL_FLIP_NONE;
    double angle = 0.0;
    while(is_running)
    {
        while(SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
            {
                is_running = false;
            }
            else if (e.type == SDL_EVENT_KEY_DOWN)
            {
                switch(e.key.key)
                {
                    case SDLK_LEFT:
                    {
                        angle -= 10;
                    } break;
                    case SDLK_RIGHT:
                    {
                        angle += 10;
                    } break;
                    case SDLK_1:
                    {
                        flip_mode = SDL_FLIP_NONE;
                    } break;
                    case SDLK_2:
                    {
                        flip_mode = SDL_FLIP_HORIZONTAL;
                    } break;
                    case SDLK_3:
                    {
                        flip_mode = SDL_FLIP_VERTICAL;
                    } break;
                    case SDLK_4:
                    {
                        flip_mode = SDL_FLIP_HORIZONTAL_AND_VERTICAL;
                    } break;
                }
            }
        }

        SDL_SetRenderDrawColor(gRenderer, 0x13, 0x33, 0x44, 0xFF);
        SDL_RenderClear(gRenderer);

        SDL_FRect dst_rect = {
            .x = screenW / 2 - arrow.Width / 2, 
            .y = screenH / 2 - arrow.Height / 2, 
            .w = arrow.Width, 
            .h = arrow.Height
        };
        SDL_RenderTextureRotated(gRenderer, arrow.Texture, NULL, &dst_rect, angle, NULL, flip_mode);

        SDL_RenderPresent(gRenderer);
    }
    return 0;
}