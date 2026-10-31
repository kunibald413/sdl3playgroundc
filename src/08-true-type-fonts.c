#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

SDL_Window* gWin = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;

typedef struct Tex {
    SDL_Texture* Texture;
    int Width;
    int Height;
} Tex;

void Tex_Destroy(Tex* tex) {
    if (tex->Texture) {
        SDL_DestroyTexture(tex->Texture);
    }
    tex->Width=0;
    tex->Height=0;
    tex->Texture=NULL;
}

bool Tex_LoadFromRenderedText(Tex* tex, const char* text, SDL_Color textColor) {
    Tex_Destroy(tex);

    size_t text_length = 0;
    SDL_Surface* surface = TTF_RenderText_Blended(gFont, text, text_length, textColor);

    if (surface != NULL) {
        if (tex->Texture = SDL_CreateTextureFromSurface(gRenderer, surface)) {
            tex->Height=surface->h;
            tex->Width=surface->w;
        }
        SDL_DestroySurface(surface);
    } else {
        SDL_Log("Failed to load ttf surface for text: %s! SDL error: %s", text, SDL_GetError());
    }

    return tex->Texture != NULL;
}

bool Init() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer("Best Fonts", 640, 420, 0, &gWin, &gRenderer);
    return TTF_Init();
}

void Close() {
    TTF_CloseFont(gFont);
    gFont = NULL;

    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
    SDL_DestroyWindow(gWin);
    gWin=NULL;

    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {

    if (!Init()) {
        return 1;
    }

    gFont = TTF_OpenFont("assets/lazy.ttf", 28);
    if (gFont == NULL) {
        SDL_Log("Failed to load font! %s", SDL_GetError());
        return 1;
    }
    Tex textTexture = {0};
    SDL_Color text_color = {0x66, 0x66, 0x55, 0xFF};
    Tex_LoadFromRenderedText(&textTexture, "Hello World!", text_color);

    SDL_Event e;
    SDL_zero(e);
    bool is_running = true;
    while(is_running) {
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                is_running = false;
            }
        }

        SDL_SetRenderDrawColor(gRenderer, 0x22,0x22,0x15, 0xFF);
        SDL_RenderClear(gRenderer);

        SDL_FRect dst_rect = {15, 15, textTexture.Width, textTexture.Height};
        SDL_RenderTexture(gRenderer, textTexture.Texture, NULL, &dst_rect);

        dst_rect.x += 100;
        dst_rect.y += 100;
        double angle = 33.0;
        SDL_RenderTextureRotated(gRenderer, textTexture.Texture, NULL, &dst_rect, angle, NULL, 0);

        SDL_RenderPresent(gRenderer);
        SDL_Delay(1);
    }

    // redundant sinec os wil clean up
    // just for practice
    Tex_Destroy(&textTexture);
    Close();

    return 0;
}