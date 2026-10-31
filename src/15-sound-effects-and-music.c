#include <stdio.h>
#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

const int screenW = 680;
const int screenH = 480;

TTF_Font* gFont = NULL;

MIX_Mixer* gMixer = NULL;
MIX_Track* gEffectTrack = NULL;

bool Init() {
    bool ok = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    ok = SDL_CreateWindowAndRenderer("Best Audio", screenW, screenH, 0, &gWindow, &gRenderer);


    if ( ok = MIX_Init()) {
        gMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
        if (gMixer == NULL) {
            SDL_Log("Failed to create mixer device! err: %s", SDL_GetError());
        } else {
            ok = true;
        }
    }

    ok = TTF_Init();

    return ok;
}

int main(int mainc, char* argv[]) {
    if (!Init()) {
        SDL_Log("failed ot init! err: %s", SDL_GetError());
        return 1;
    }

    gFont = TTF_OpenFont("assets/Figtree-Light.ttf", 25);

    SDL_Color text_color = {0};
    text_color.a = 0xFF;

    SDL_Surface* surface = TTF_RenderText_Blended(gFont, 
        "Press 1, 2, 3, or 4 for sounds! SPACE or 0 for music!", 
        0, text_color);
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(gRenderer, surface);
    if (text_texture == NULL) {
        SDL_Log("Failed to load texture! err: %s", SDL_GetError());
        return 1;
    }
    SDL_FRect dest_rect = {
        .w = surface->w,
        .h = surface->h,
        .x = (screenW - surface->w) / 2,
        .y = (screenH - surface->h) / 2,
    };

    MIX_Audio* audio_high = MIX_LoadAudio(gMixer, "assets/high.wav", true);
    MIX_Audio* audio_medium = MIX_LoadAudio(gMixer, "assets/medium.wav", true);
    MIX_Audio* audio_low = MIX_LoadAudio(gMixer, "assets/low.wav", true);
    MIX_Audio* audio_scratch = MIX_LoadAudio(gMixer, "assets/scratch.wav", true);
    gEffectTrack = MIX_CreateTrack(gMixer);

    MIX_Audio* audio_music = MIX_LoadAudio(gMixer, "assets/beat.wav", true);
    MIX_Track* music_track = MIX_CreateTrack(gMixer);
    MIX_SetTrackAudio(music_track, audio_music);

    bool is_running = true;
    SDL_Event e;
    SDL_zero(e);

    char event_desc_buff[256];

    while(is_running) {
        while(SDL_PollEvent(&e)){
            if (e.type == SDL_EVENT_QUIT) {
                is_running = false;
            } else if (e.type == SDL_EVENT_KEY_DOWN) {

                SDL_GetEventDescription(&e, event_desc_buff, sizeof(event_desc_buff));
                printf("Key: %d event: %s\n", e.type, event_desc_buff);

                switch(e.key.key) {
                    case SDLK_1: {
                        MIX_SetTrackAudio(gEffectTrack, audio_high);
                        MIX_PlayTrack(gEffectTrack, 0);
                    } break;
                    case SDLK_2: {
                        MIX_SetTrackAudio(gEffectTrack, audio_medium);
                        MIX_PlayTrack(gEffectTrack, 0);
                    } break;
                    case SDLK_3: {
                        MIX_SetTrackAudio(gEffectTrack, audio_low);
                        MIX_PlayTrack(gEffectTrack, 0);
                    } break;
                    case SDLK_4: {
                        MIX_SetTrackAudio(gEffectTrack, audio_scratch);
                        MIX_PlayTrack(gEffectTrack, 0);
                    } break;

                    case SDLK_SPACE: {
                        if (MIX_TrackPlaying(music_track) || MIX_TrackPaused(music_track)) {
                            if (MIX_TrackPlaying(music_track)) {
                                MIX_PauseTrack(music_track);
                            } else {
                                MIX_ResumeTrack(music_track);
                            }
                        } else {
                            SDL_PropertiesID props = SDL_CreateProperties();
                            SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
                            MIX_PlayTrack(music_track, props);
                            SDL_DestroyProperties(props);
                        }
                    } break;
                    case SDLK_0: {
                        MIX_StopTrack(music_track, 5);
                    } break;
                }
            }
        } // event loop

        SDL_SetRenderDrawColor(gRenderer, 0xAA,0x88,0x55,0xFF);
        SDL_RenderClear(gRenderer);

        SDL_RenderTexture(gRenderer, text_texture, NULL, &dest_rect);

        SDL_RenderPresent(gRenderer);

        SDL_Delay(1000/60);
    }


    return 0;
}