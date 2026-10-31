#include <stdio.h>
#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

const int screenW = 640;
const int screenH = 480;

const int levelW = 1280;
const int levelH = 960;

const int screen_fps = 60;
const Uint64 ns_per_frame = 1000000000 / screen_fps;

SDL_Window* gWin = NULL;
SDL_Renderer* gRenderer = NULL;

typedef struct Tex {
    SDL_Texture* Texture;
    int Width;
    int Height;
} Tex;

void Texture_Destroy(Tex* t) {
    if (t->Texture) {
        SDL_DestroyTexture(t->Texture);
    }
    t->Height=0;
    t->Width=0;
}

bool Texture_FromFile(Tex* t, const char* path) {
    Texture_Destroy(t);

    SDL_Surface* s = IMG_Load(path);
    if (s == NULL) {
        return false;
    }
    bool ok = SDL_SetSurfaceColorKey(s, true, SDL_MapSurfaceRGB(s, 0x00, 0xFF,0xFF)); // color key cyan

    SDL_Texture* new_tex = SDL_CreateTextureFromSurface(gRenderer, s);

    if (new_tex != NULL) {
        t->Texture = new_tex;
        t->Width = s->w;
        t->Height = s->h;
    }

    SDL_DestroySurface(s);

    return ok && t->Texture != NULL;
}

void Texture_Render(Tex* t, float x, float y, SDL_FRect* clip) {
    SDL_FRect dst_rect =  {
        .x = x, .y = y,
        .w = t->Width,
        .h = t->Height
    };
    SDL_RenderTexture(gRenderer, t->Texture, clip, &dst_rect);
}

const int DotVel = 3;
const int dot_width = 20;
const int dot_height = 20;
typedef struct Dot {
    int PosX;
    int PosY;
    int VelX;
    int VelY;
} Dot;

void Dot_HandleEvent(Dot* dot, SDL_Event* e) {
    if (e->type == SDL_EVENT_KEY_DOWN && e->key.repeat == 0) {
        switch(e->key.key) {
            case SDLK_UP: {
                dot->VelY -= DotVel;
            } break;
            case SDLK_DOWN: {
                dot->VelY += DotVel;
            } break;
            case SDLK_LEFT: {
                dot->VelX -= DotVel;
            } break;
            case SDLK_RIGHT: {
                dot->VelX += DotVel;
            } break;
        }
    } else if (e->type == SDL_EVENT_KEY_UP && e->key.repeat == 0) {
        switch(e->key.key) {
            case SDLK_UP: {
                dot->VelY += DotVel;
            } break;
            case SDLK_DOWN: {
                dot->VelY -= DotVel;
            } break;
            case SDLK_LEFT: {
                dot->VelX += DotVel;
            } break;
            case SDLK_RIGHT: {
                dot->VelX -= DotVel;
            } break;
        }
    }
}

void Dot_Move(Dot* dot) {
    dot->PosX += dot->VelX;
    if (dot->PosX < 0 || dot->PosX + dot_width > levelW) {
        dot->PosX -= dot->VelX;
    }

    dot->PosY += dot->VelY;
    if (dot->PosY < 0 || dot->PosY + dot_height > levelH) {
        dot->PosY -= dot->VelY;
    }
}



void Close() {
    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
    SDL_DestroyWindow(gWin);
    gWin = NULL;
    SDL_Quit();
}

int main(int argc, char* argv[]) {


    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer("Best Window", screenW, screenH, 0, &gWin, &gRenderer);
    
    Tex dot_texture = {0};
    Texture_FromFile(&dot_texture, "assets/dot.png");

    Tex bg_texture = {0};
    Texture_FromFile(&bg_texture, "assets/scroll_background.png");

    Dot dot = {0};
    dot.PosX = 100;
    dot.PosY = 10;

    SDL_FRect cam = {
        .x = 0, .y = 0, .w = screenW, .h = screenH
    };

    bool quit = false;
    SDL_Event e;
    SDL_zero(e);

    while(!quit) {
        Uint64 frame_start_ns = SDL_GetTicksNS();

        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }

            Dot_HandleEvent(&dot, &e);
        }

        // stuff
        Dot_Move(&dot);
        cam.x = dot.PosX + dot_width  / 2 - screenW / 2;
        cam.y = dot.PosY + dot_height / 2 - screenH / 2;


        SDL_SetRenderDrawColor(gRenderer, 0x11,0x22,0x33,0xff);
        SDL_RenderClear(gRenderer);

        // render stuff
        Texture_Render(&bg_texture, 0, 0, &cam);
        Texture_Render(&dot_texture, dot.PosX - cam.x, dot.PosY - cam.y, NULL);

        SDL_RenderPresent(gRenderer);

        Uint64 elapsed_ns = SDL_GetTicksNS() - frame_start_ns;
        if (elapsed_ns < ns_per_frame) {
            SDL_DelayNS(ns_per_frame - elapsed_ns);
        }

    }

    Texture_Destroy(&dot_texture);
    Texture_Destroy(&bg_texture);

    Close();

    printf("Done!\n");
    return 0;
}