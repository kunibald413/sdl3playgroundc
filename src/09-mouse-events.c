#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <stdbool.h>

const int screenW = 680;
const int screenH = 420;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

typedef struct Tex {
    SDL_Texture* Texture;
    int Width;
    int Height;
} Tex;


void Tex_Destroy(Tex* tex) {
    SDL_DestroyTexture(tex->Texture);
    tex->Height=0;
    tex->Width=0;
}

bool Tex_LoadFromFile(Tex* tex, const char* path) {
    Tex_Destroy(tex);

    SDL_Surface* surface = IMG_Load(path);

    tex->Texture = SDL_CreateTextureFromSurface(gRenderer, surface);
    tex->Height = surface->h;
    tex->Width = surface->w;

    return tex->Texture != NULL;
}


typedef enum ButtonSpriteType {
    MouseOut = 0,
    MouseOverMotion = 1,
    MouseDown = 2,
    MouseUp = 3
} ButtonSpriteType;

typedef struct Button {
    SDL_FPoint Position;
    ButtonSpriteType CurrentSpriteType;
    int Width;
    int Height;
} Button;

void Button_Handlevent(Button* btn, SDL_Event* e) {
    if (e->type == SDL_EVENT_MOUSE_MOTION
        || e->type == SDL_EVENT_MOUSE_BUTTON_DOWN
        || e->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        float x,y;
        SDL_GetMouseState(&x, &y);
        
        bool is_inside = false;
        if (x >= btn->Position.x && x <= btn->Position.x + btn->Width
            && y >= btn->Position.y && y <= btn->Position.y + btn->Height) {
            btn->CurrentSpriteType = MouseOverMotion;
            is_inside = true;
        }
        
        ButtonSpriteType new_type = MouseOut;
        if (is_inside) {
            switch(e->type) {
                case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                    new_type = MouseDown;
                } break;
                case SDL_EVENT_MOUSE_BUTTON_UP: {
                    new_type = MouseUp;
                } break;
                case SDL_EVENT_MOUSE_MOTION: {
                    new_type = MouseOverMotion;
                } break;
            }
            btn->CurrentSpriteType = e->type == SDL_EVENT_MOUSE_BUTTON_DOWN 
                ? MouseDown 
                : MouseOverMotion;
        }
        btn->CurrentSpriteType = new_type;
    }
}

void Button_Render(Button* btn, Tex* tex) {
    SDL_FRect clip = {
        .x = 0,
        .y = btn->Height * btn->CurrentSpriteType, // button tex is one "column" of sprites
        .w = btn->Width,
        .h = btn->Height
    };
    SDL_FRect dst_rect = {
        .x = btn->Position.x,
        .y = btn->Position.y,
        .w = btn->Width,
        .h = btn->Height
    };
    SDL_RenderTexture(gRenderer, tex->Texture, &clip, &dst_rect);
}

bool Init() {
    SDL_Init(SDL_INIT_VIDEO);
    return SDL_CreateWindowAndRenderer("Best Window", screenW, screenH, 0, &gWindow, &gRenderer);
}

int main(int argc, char* argv[]) {
    if (!Init()) {
        return 1;
    }

    // load texture
    Tex tex = {0};
    if (!Tex_LoadFromFile(&tex, "assets/button.png")) {
        SDL_Log("Failed to load texture! err: %s", SDL_GetError());
        return 1;
    }

    const int btn_count = 4;
    Button btns[btn_count];
    const int padding = 5;
    for (int i = 0; i < btn_count; i ++) {
        Button b = {0};
        b.Height = 200;
        b.Width = 300;
        b.Position.x = i == 1 || i == 3 
            ? b.Width + padding 
            : 0;
        b.Position.y = i >= 2 
            ? b.Height + padding 
            : 0;
        btns[i] = b;
    }



    SDL_Event e;
    SDL_zero(e);
    bool is_running = true;
    while(is_running) {

        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                is_running = false;
            }
            for (int i = 0; i < btn_count; i++) {
                Button_Handlevent(&btns[i], &e);
            }
        }

        SDL_SetRenderDrawColor(gRenderer, 0x77,0x77,0x77,0xff);
        SDL_RenderClear(gRenderer);

        // render texture
        // SDL_RenderTexture(gRenderer, tex.Texture, NULL, NULL);
        for (int i = 0; i < btn_count; i++) {
            Button_Render(&btns[i], &tex);
        }

        SDL_RenderPresent(gRenderer);

        SDL_Delay(1);
    }

    return 0;
}