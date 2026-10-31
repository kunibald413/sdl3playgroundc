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

void Texture_Zero(Texture* tex)
{
    tex->Texture = NULL;
    tex->Width = 0;
    tex->Height = 0;
}

void Texture_Destroy(Texture* tex)
{
    if (tex->Texture)
    {
        SDL_DestroyTexture(tex->Texture);
    }
    Texture_Zero(tex);
}

bool Texture_LoadFromFile(Texture* tex, const char* path)
{
    Texture_Destroy(tex);

    SDL_Surface* surface = IMG_Load(path);
    if (surface)
    {

        SDL_SetSurfaceColorKey(surface, true, SDL_MapSurfaceRGB(surface,  0x00, 0xFF, 0xFF));

        if (tex->Texture = SDL_CreateTextureFromSurface(gRenderer, surface))
        {
            tex->Height = tex->Texture->h;   
            tex->Width = tex->Texture->w;   
        }
        SDL_DestroySurface(surface);
    } 
    else 
    {
        SDL_Log("Failed to load surface %s! Error: %s", path, SDL_GetError());
    }

    return tex->Texture != NULL;
}

void Texture_Render(Texture* tex, SDL_FRect* clip, float x, float y)
{
    SDL_FRect dst_rect = {
        .h = clip != NULL ? clip->h : (float)tex->Height,
        .w = clip != NULL ? clip->w : (float)tex->Width,
        .x = x,
        .y = y,
    };
    SDL_RenderTexture(gRenderer, tex->Texture, clip, &dst_rect);
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
    Init();

    // load texture

    Texture tex = {0};
    Texture_LoadFromFile(&tex, "assets/dots.png");
    printf("Loaded texture w: %d h: %d\n", tex.Width, tex.Height);

    // loop
    bool is_running = true;
    SDL_Event e;
    SDL_zero(e);
    while (is_running)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
            {
                is_running = false;
            }
        }

        SDL_SetRenderDrawColor(gRenderer, 0x33, 0x33, 0x33, 0xFF);
        SDL_RenderClear(gRenderer);
        
        // render full texture (200 x 200)
        Texture_Render(&tex, NULL, 0, 0);


        // clip and render
        const int sprite_dim = 100;
        const int padding = 10;
        SDL_FRect red_sprite = {
            .x = 0, .y = 0, .w = sprite_dim, .h = sprite_dim
        };
        Texture_Render(&tex, &red_sprite, tex.Width + padding, 0);

        SDL_FRect yellow_sprite = {
            .x = 0, .y = 100, .w = sprite_dim, .h = sprite_dim
        };
        Texture_Render(&tex, &yellow_sprite, tex.Width + yellow_sprite.w + padding, 0);

        SDL_FRect green_sprite = {
            .x = 100, .y = 0, .w = sprite_dim, .h = sprite_dim
        };
        Texture_Render(&tex, &green_sprite, tex.Width + padding, green_sprite.w);

        SDL_FRect blue_sprite = {
            .x = 100, .y = 100, .w = sprite_dim, .h = sprite_dim
        };
        Texture_Render(&tex, &blue_sprite, tex.Width + green_sprite.w + padding, green_sprite.w);

        // stretch horizontally
        SDL_FRect dst_rect = {.x = 400, .y = 250, .w = blue_sprite.w * 1.5f, .h = blue_sprite.h * 0.5f};
        SDL_RenderTexture(gRenderer, tex.Texture, &blue_sprite, &dst_rect);
        
        // stretch vertically
        SDL_FRect dst_rect2 = {.x = 50, .y = 230, .w = green_sprite.w * 0.5f, .h = green_sprite.h * 1.5f};
        SDL_RenderTexture(gRenderer, tex.Texture, &green_sprite, &dst_rect2);

        SDL_FRect double_size = {.x = 150, .y = 210, .w = green_sprite.w * 2.f, .h = green_sprite.h * 2.0f};
        SDL_RenderTexture(gRenderer, tex.Texture, &yellow_sprite, &double_size);

        
        SDL_RenderPresent(gRenderer);

        SDL_Delay(1000 / 60);
    }
    //  render texure
    return 0;
}