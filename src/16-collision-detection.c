#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdbool.h>

const int screenW = 680;
const int screenH = 480;

SDL_Window* gWin = NULL;
SDL_Renderer* gRenderer = NULL;

bool Init() {
    bool ok = SDL_Init(SDL_INIT_VIDEO);
    ok = SDL_CreateWindowAndRenderer("Best collision", screenW, screenH, 0, &gWin, &gRenderer);
    return ok;
}

typedef struct BestRect {
    int x;
    int y;
    int w;
    int h;
    int vel_x;
    int vel_y;
} BestRect;

bool AreColliding(SDL_Rect a, SDL_Rect b) {
    bool overlap_x = a.x < b.x + b.w && a.x + a.w > b.x;
    bool overlap_y = a.y < b.y + b.h && a.y + a.h > b.y;
    return overlap_x && overlap_y;
}

int main(int argc, char* argv[]) {

    if (!Init()) {
        SDL_Log("Failed to init! %s", SDL_GetError());
        return 1;
    }

    BestRect wall = {.x = (screenW / 2), .y = 10, .w = 30, .h = screenH - 100};
    BestRect dude = {.x = 20, .y = (screenH  - 80) / 2, .w = 80, .h = 80};
    const int dude_speed = 3;
    SDL_Rect arena_rect = {.x = 0, .y = 0, .w = screenW -1, .h = screenH -1};

    SDL_Event e;
    SDL_zero(e);
    bool quit = false;

    while(!quit) {
        while(SDL_PollEvent(&e)) {
            // INPUT
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            } 
            else if (e.type == SDL_EVENT_KEY_DOWN) {
                SDL_Log("SDL_EVENT_KEY_DOWN %d", e.key.key);
                switch (e.key.key) {
                    case SDLK_LEFT: {
                        SDL_Log("key left %d", e.key.key);
                        dude.vel_x = -dude_speed;
                        dude.vel_y = 0;
                    } break;
                    case SDLK_RIGHT: {
                        SDL_Log("key right %d", e.key.key);
                        dude.vel_x = dude_speed;
                        dude.vel_y = 0;
                    } break;
                    case SDLK_UP: {
                        SDL_Log("key up %d", e.key.key);
                        dude.vel_x = 0;
                        dude.vel_y = -dude_speed;
                    } break;
                    case SDLK_DOWN: {
                        SDL_Log("key down %d", e.key.key);
                        dude.vel_x = 0;
                        dude.vel_y = dude_speed;
                    } break;
                    case SDLK_SPACE: {
                        SDL_Log("key space %d", e.key.key);
                        dude.vel_y = 0;
                        dude.vel_x = 0;
                    } break;
                }
            }
        }

        // MOVEMENT
        dude.x += dude.vel_x;
        dude.y += dude.vel_y;

        // DETECT COLLISION
        SDL_Rect coll_dude = {.x = dude.x, .y = dude.y, .w = dude.w, .h = dude.h};
        SDL_Rect coll_wall = {.x = wall.x, .y = wall.y, .w = wall.w, .h = wall.h};
        if (AreColliding(coll_dude, coll_wall)) {
            dude.x -= dude.vel_x;
            dude.y -= dude.vel_y;
        }
        
        if (dude.x < arena_rect.x || dude.x + dude.w > arena_rect.x + arena_rect.w) {
            dude.x -= dude.vel_x;
        }
        if (dude.y < arena_rect.y || dude.y + dude.h > arena_rect.y + arena_rect.h) {
            dude.y -= dude.vel_y;
        }
        
        // DRAW
        SDL_SetRenderDrawColor(gRenderer, 0x12, 0x12, 0x12, 0xFF);
        SDL_RenderClear(gRenderer);

        // DRAW MY STUFF
        SDL_SetRenderDrawColor(gRenderer, 0x11, 0x99, 0x11, 0xFF);

        SDL_FRect best_rect = {0};
        best_rect.w = dude.w;
        best_rect.h = dude.h;
        best_rect.x = dude.x;
        best_rect.y = dude.y;
        SDL_RenderRect(gRenderer, &best_rect);

        SDL_FRect wall_rect = {.x = wall.x, .y = wall.y, .w = wall.w, .h = wall.h};
        SDL_RenderRect(gRenderer, &wall_rect);

        SDL_FRect draw_rect_arena = {.x = arena_rect.x, .y = arena_rect.y, .w = arena_rect.w, .h = arena_rect.h};
        SDL_RenderRect(gRenderer, &draw_rect_arena);

        // PRESENT
        SDL_RenderPresent(gRenderer);

        // DELAY
        SDL_Delay(1000/60);
    }

    return 0;
}