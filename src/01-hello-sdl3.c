#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdbool.h>    /* for bool, true, false */
#include <stdio.h>      /* optional, only used if you need extra logging */

/* Constants */
static const int kScreenWidth = 640;
static const int kScreenHeight = 480;

/* Function Prototypes */
bool init(void);
bool loadMedia(void);
void close(void);

/* Global Variables */
SDL_Window*   gWindow      = NULL;
SDL_Surface*  gScreenSurface = NULL;
SDL_Surface*  gHelloWorld  = NULL;


/* Function Implementations */
bool init(void)
{
    bool success = true;

    /* Initialize SDL video */
    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
        success = false;
    } else {
        /* Create window */
        gWindow = SDL_CreateWindow("SDL3 Tutorial: Hello SDL3",
                                   kScreenWidth, kScreenHeight, 0);
        if (gWindow == NULL) {
            SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
            success = false;
        } else {
            /* Get window surface */
            gScreenSurface = SDL_GetWindowSurface(gWindow);
        }
    }

    return success;
}

bool loadMedia(void)
{
    bool success = true;

    /* Load splash image – using a C string */
    const char* imagePath = "assets/hello-sdl3.bmp";
    gHelloWorld = SDL_LoadBMP(imagePath);
    if (gHelloWorld == NULL) {
        SDL_Log("Unable to load image %s! SDL error: %s\n",
                imagePath, SDL_GetError());
        success = false;
    }

    return success;
}

void close(void) {
    /* Deallocate surface */
    SDL_DestroySurface(gHelloWorld);
    gHelloWorld = NULL;

    /* Destroy window */
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gScreenSurface = NULL;    /* surface is now invalid */

    /* Quit SDL subsystems */
    SDL_Quit();
}

int main(int argc, char* args[]) {
    int exitCode = 0;

    // comments like this one is fine
    char event_desc_buf[256];

    /* Initialize */
    if (init() == false) {
        SDL_Log("Unable to initialize program!\n");
        exitCode = 1;
    } else {
        /* Load media */
        if (loadMedia() == false) {
            SDL_Log("Unable to load media!\n");
            exitCode = 2;
        } else {
            bool quit = false;
            SDL_Event e;
            SDL_zero(e);

            /* Main loop */
            while (quit == false) {
                /* Process events */
                while (SDL_PollEvent(&e) == true) {
                    if (e.type == SDL_EVENT_QUIT) {
                        quit = true;
                    }
                }

                /* Fill the surface with white */
                SDL_FillSurfaceRect(gScreenSurface, NULL,
                                    SDL_MapSurfaceRGB(gScreenSurface,
                                                      0xFF, 0xFF, 0xFF));

                /* Blit the loaded image onto the window surface */
                SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);

                /* Update the window */
                SDL_UpdateWindowSurface(gWindow);

                SDL_Delay(1000 / 60);
            }
        }
    }

    /* Clean up */
    close();

    return exitCode;
}