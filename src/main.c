#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "SDL3_ttf/SDL_ttf.h"

SDL_Window* gWindow;
SDL_Renderer* gRenderer;
TTF_Font* gFont = NULL;
TTF_Font* gFont2 = NULL;

int main(int argc, char* argv[]) {

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_CreateWindowAndRenderer("best font", 680, 480, 0, &gWindow, &gRenderer);

    gFont = TTF_OpenFont("assets/WeBeLt__.ttf", 25);
    gFont2 = TTF_OpenFont("assets/Antqua.ttf", 25);

    SDL_Color text_color = {.r = 0xff, .g = 0xff, .b = 0xff, .a=0xff};
    SDL_Surface* s1 = TTF_RenderText_Blended(gFont, "Hello from WeBeLt font!", 0, text_color);
    SDL_Texture* tex1 = SDL_CreateTextureFromSurface(gRenderer, s1);

    SDL_Surface* s2 = TTF_RenderText_Blended(gFont2, "Hello from Antqua font!", 0, text_color);
    SDL_Texture* tex2 = SDL_CreateTextureFromSurface(gRenderer, s2);

    bool is_running = true;
    SDL_Event e;
    SDL_zero(e);

    while(is_running) {
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                is_running = false;
            }
        }
        SDL_SetRenderDrawColor(gRenderer, 0x11,0x22,0x33, 0xff);
        SDL_RenderClear(gRenderer);

        SDL_FRect dest_rect = {.x = 10, .y = 10, .w = s1->w, .h = s1->h};
        SDL_RenderTexture(gRenderer, tex1, NULL, &dest_rect);

        dest_rect.w = s2->w;
        dest_rect.h = s2->h;
        dest_rect.x += dest_rect.w + 20;
        SDL_RenderTexture(gRenderer, tex2, NULL, &dest_rect);

        SDL_RenderPresent(gRenderer);

        SDL_Delay(1000 / 60);
    }


    return 0;
}