#include <stdio.h>
#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

const int screenW = 680;
const int screenH = 480;
const int screenFps = 60;
const int screen_ns_per_frame = 1000000000 / screenFps;

SDL_Color text_color = {0x99, 0x99,0x99, 0xFF};

SDL_Window* gWin = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;

int main(int argc, char* argv[]) {

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer("Best text", screenW, screenH, 0, &gWin, &gRenderer);
    TTF_Init();

    gFont = TTF_OpenFont("assets/Figtree-Light.ttf", 30);

    if (gFont == NULL) {
        SDL_Log("Failed to load font! err: %s", SDL_GetError());
        return 1;
    }

    SDL_StartTextInput(gWin);
    char input_text[256] = "Hello!";

    bool quit = false;
    SDL_Event e;
    SDL_zero(e);
    while(!quit) {

        Uint64 frame_start_ns = SDL_GetTicksNS();

        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            } else if (e.type == SDL_EVENT_TEXT_INPUT) {
                char event_desc[256];
                SDL_GetEventDescription(&e, event_desc, sizeof(event_desc));
                size_t current_len = strlen(input_text);
                size_t available = sizeof(input_text) - current_len - 1;
                printf("available: %llu current_length: %llu Text input event: %s\n", available, current_len, event_desc);
                if (available > 0) 
                    strncat(input_text, e.text.text, available);
            } else if (e.type == SDL_EVENT_KEY_DOWN) {
                if (e.key.key == SDLK_BACKSPACE) {
                    if (strlen(input_text) > 0) {
                        input_text[strlen(input_text)-1] = '\0';
                        printf("Shortened input to %s\n", input_text);
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(gRenderer, 0x10, 0x44, 0x22, 0xff);;
        SDL_RenderClear(gRenderer);


        SDL_Surface* surface = TTF_RenderText_Blended(gFont, "Enter Text:", 0, text_color);
        SDL_Texture* tex = SDL_CreateTextureFromSurface(gRenderer, surface);
        SDL_FRect dest_rect = {.h = surface->h, .w = surface->w, .x = 150, .y = 75};
        SDL_RenderTexture(gRenderer, tex, NULL, &dest_rect);
        SDL_DestroySurface(surface);
        SDL_DestroyTexture(tex);

        if (strlen(input_text) > 0) {
            surface = TTF_RenderText_Blended(gFont, input_text, 0, text_color);
            tex = SDL_CreateTextureFromSurface(gRenderer, surface);
            SDL_FRect dest_rect2 = {.h = surface->h, .w = surface->w, .x = 150, .y = 100};
            SDL_RenderTexture(gRenderer, tex, NULL, &dest_rect2);
            SDL_DestroySurface(surface);
            SDL_DestroyTexture(tex);
        }



        SDL_RenderPresent(gRenderer);

        Uint64 ns_for_frame = SDL_GetTicksNS() - frame_start_ns;
        if (ns_for_frame < screen_ns_per_frame) {
            SDL_DelayNS(screen_ns_per_frame - ns_for_frame);
        }

    }

    printf("Done!\n");
    return 0;
}