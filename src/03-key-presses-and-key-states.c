
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>

const int screenW = 640;
const int screenH = 420;

SDL_Window* globalWindow = NULL;
SDL_Renderer* globalRenderer = NULL;


typedef struct Texture {
    SDL_Texture* Texture;
    int Width;
    int Height;
} Texture;

Texture globalTextures[4];


void Texture_Zero(Texture* tex)
{
    tex->Height=0;
    tex->Width=0;
    tex->Texture = NULL;
}

void Texture_Destroy(Texture* tex) 
{
    if (tex->Texture)
    {
        SDL_DestroyTexture(tex->Texture);
    }
    Texture_Zero(tex);
}

bool Texture_LoadFromFile(Texture* tex, char* path)
{
    Texture_Destroy(tex);

    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL)
    {
        SDL_Log("Failed to load surface of %s! SDL Error %s\n", path, SDL_GetError());
        return false;
    }

    tex->Texture = SDL_CreateTextureFromSurface(globalRenderer, surface);
    if (tex->Texture == NULL) 
    {
        SDL_Log("Failed to create texture from surface %s! SDL Error: %s\n", path, SDL_GetError());
    }
    else
    {
        tex->Width = tex->Texture->w;
        tex->Height = tex->Texture->h;
    }

    SDL_DestroySurface(surface);

    return tex->Texture != NULL;
}

void Texture_Render(Texture* tex, float x, float y) 
{
    SDL_FRect dst_rect = {
        .x = x,
        .y = y,
        .h = tex->Height,
        .w = tex->Width
    };
    SDL_RenderTexture(globalRenderer, tex->Texture, NULL, &dst_rect);
}

bool LoadTextures()
{
    char* paths[4] = {
        "assets/up.png",
        "assets/right.png",
        "assets/down.png",
        "assets/left.png",
    };
    for (int i = 0; i < 4; i ++) {
        Texture t1;
        Texture_Zero(&t1);
        if (!Texture_LoadFromFile(&t1, paths[i]))
        {
            SDL_Log("Failed to load textures %s", paths[i]);
            return false;
        }
        globalTextures[i] = t1; // copy
    }
    return true;
}

void DestroyTextures() 
{
    for (int i = 0; i < 4; i++) {
        Texture_Destroy(&globalTextures[i]);
    }
}

bool Init() 
{
    if (!SDL_Init(SDL_INIT_VIDEO)) 
    {
        SDL_Log("Failed to init SDL! SDL_Error: %s", SDL_GetError());
        return false;
    }

    SDL_WindowFlags flags = 0;
    if (!SDL_CreateWindowAndRenderer("Best window", screenW, screenH, flags, &globalWindow, &globalRenderer)) 
    {
        SDL_Log("Failed to create window and renderer! SDL_Error: %s", SDL_GetError());
        return false;
    }

    return true;
}

void Close()
{
    DestroyTextures();
    SDL_DestroyRenderer(globalRenderer);
    globalRenderer=NULL;
    SDL_DestroyWindow(globalWindow);
    globalWindow=NULL;
    SDL_Quit();
}



int main(int argc, char* argv[]) {
    
    // init sdl
    if (!Init()) 
    {
        SDL_Log("Failed to init");
        return 1;
    }

    // load media
    if (!LoadTextures()) 
    {
        DestroyTextures();
        return 1;
    }

    // loop
    SDL_Event e;
    SDL_zero(e);
    bool is_running = true;

    SDL_Color bg_color = {0x14, 0x14, 0x14, 0xFF};
    Texture* last_texture = &globalTextures[0];
    while (is_running) 
    {
        //  poll events
        while (SDL_PollEvent(&e))
        {
            //  handle events
            if (e.type == SDL_EVENT_QUIT) 
            {
                is_running = false;
            }
            //  render different png and background color based on keypress
            else if (e.type == SDL_EVENT_KEY_DOWN)
            {
                if (e.key.key == SDLK_UP)
                {
                    last_texture = &globalTextures[0];
                }
                else if (e.key.key == SDLK_RIGHT)
                {
                    last_texture = &globalTextures[1];
                }
                else if (e.key.key == SDLK_DOWN)
                {
                    last_texture = &globalTextures[2];
                }
                else if (e.key.key == SDLK_LEFT)
                {
                    last_texture = &globalTextures[3];
                }
            }
        }

        // reset
        bg_color.r = 0x14;
        bg_color.g = 0x14;
        bg_color.b = 0x14;
        bg_color.a = 0xff;
        // read keyboard state and set bg color based on that
        const bool* key_states = SDL_GetKeyboardState(NULL);
        if (key_states[SDL_SCANCODE_UP])
        {
            bg_color.b = 0x33;
        }
        else if (key_states[SDL_SCANCODE_RIGHT])
        {
            bg_color.r = 0x33;
        }
            else if (key_states[SDL_SCANCODE_DOWN])
        {
            bg_color.g = 0x33;
        }
        else if (key_states[SDL_SCANCODE_LEFT])
        {
            bg_color.b = 0x33;
            bg_color.g = 0x33;
        }

        SDL_SetRenderDrawColor(globalRenderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
        SDL_RenderClear(globalRenderer);

        Texture_Render(last_texture, 0.f,0.f);

        SDL_RenderPresent(globalRenderer); // swap the backbuffer to "front"
    
        SDL_Delay(1000 / 60);
    }

    // quit
    SDL_Log("Ok done, bye!");
    Close();
    return 0;
}