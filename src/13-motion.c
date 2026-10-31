#include <stdio.h>
#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

const int screenW = 680;
const int screenH = 480;
const int FPS = 60;
const Uint64 NS_PER_FRAME = 1000000000 / FPS;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

typedef struct Dot {
    int PosX;
    int PosY;
    int VelX;
    int VelY;
} Dot;

bool Init() {
    SDL_Init(SDL_INIT_VIDEO);
    return SDL_CreateWindowAndRenderer("The Best Window", screenW, screenH, 0, &gWindow, &gRenderer);
}

void Close() {
    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    SDL_Quit();
}

int main(int argc, char* argv[]) {

    if (!Init()) {
        SDL_Log("Failed to init! SDL Error: %s", SDL_GetError());
        return 1;
    }

    const int dot_size = 25;
    Dot dot = {0};
    dot.PosX = (screenW - dot_size) / 2;
    dot.PosY = (screenH - dot_size) / 2;

    SDL_Surface* surface = IMG_Load("assets/dot.png");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surface);
    SDL_FRect dest_rect = {0};
    dest_rect.w = dot_size;
    dest_rect.h = dot_size;
    dest_rect.x = dot.PosX;
    dest_rect.y = dot.PosY;


    SDL_DestroySurface(surface);

    bool is_running = true;
    SDL_Event e;
    SDL_zero(e);
    while(is_running) {

        Uint64 frame_start_ticks = SDL_GetTicksNS();

        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                is_running = false;
            }
            else if (e.type == SDL_EVENT_KEY_DOWN) {
                switch(e.key.key) {
                    case SDLK_SPACE: {
                        dot.VelX = 0;
                        dot.VelY = 0;
                    } break;
                    case SDLK_LEFT: {
                        dot.VelX = -2;
                        dot.VelY = 0;
                    } break;
                    case SDLK_RIGHT: {
                        dot.VelX = 2;
                        dot.VelY = 0;
                    } break;
                    case SDLK_UP: {
                        dot.VelX = 0;
                        dot.VelY = -2;
                    } break;
                    case SDLK_DOWN: {
                        dot.VelX = 0;
                        dot.VelY = 2;
                    } break;
                }
            }
        }

        // MOVE DOT
        dot.PosX += dot.VelX;
        dot.PosY += dot.VelY;
        if (dot.PosX < 0 || dot.PosX + dot_size > screenW) {
            dot.PosX -= dot.VelX;
        }
        if (dot.PosY < 0 || dot.PosY + dot_size > screenH) {
            dot.PosY -= dot.VelY;
        }
        
        
        SDL_SetRenderDrawColor(gRenderer, 0x88, 0x88,0x22, 0xff);
        SDL_RenderClear(gRenderer);
        
        // RENDER MY STUFF
        dest_rect.x = dot.PosX;
        dest_rect.y = dot.PosY;
        SDL_RenderTexture(gRenderer, texture, NULL, &dest_rect);

        // PRESENT
        SDL_RenderPresent(gRenderer);

        Uint64 elapsed_ns = SDL_GetTicksNS() - frame_start_ticks;
        double instant_fps = 1e9 / (double)elapsed_ns;
        
        
        if (elapsed_ns < NS_PER_FRAME) {
            Uint64 sleep_time = NS_PER_FRAME - elapsed_ns;
            SDL_DelayNS(sleep_time);
        }
    }

    SDL_DestroyTexture(texture);
    Close();


    printf("Done\n");
    return 0;
}