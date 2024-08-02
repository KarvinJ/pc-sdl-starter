#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include "sdl_starter.h"
#include "sdl_assets_loader.h"

const int SPEED = 600;
int score = 0;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

Mix_Chunk *actionSound = nullptr;
Mix_Music *music = nullptr;

Sprite alienSprite;

SDL_Texture *title = nullptr;
SDL_Rect titleRect;

TTF_Font *fontSquare = nullptr;

void quitGame()
{
    Mix_FreeChunk(actionSound);
    SDL_DestroyTexture(alienSprite.texture);
    SDL_DestroyTexture(title);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void handleEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE)
        {
            quitGame();
            exit(0);
        }

        // To handle key pressed more precise, I use this method for handling pause the game or jumping.
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE)
        {
            Mix_PlayChannel(-1, actionSound, 0);

            score++;
            std::string string = std::to_string(score);

            updateTextureText(title, string.c_str(), fontSquare, renderer);
        }
    }
}

void update(float deltaTime)
{
    const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);

    if (currentKeyStates[SDL_SCANCODE_W] && alienSprite.textureBounds.y > 0)
    {
        alienSprite.textureBounds.y -= SPEED * deltaTime;
    }

    else if (currentKeyStates[SDL_SCANCODE_S] && alienSprite.textureBounds.y < SCREEN_HEIGHT - alienSprite.textureBounds.h)
    {
        alienSprite.textureBounds.y += SPEED * deltaTime;
    }

    else if (currentKeyStates[SDL_SCANCODE_A] && alienSprite.textureBounds.x > 0)
    {
        alienSprite.textureBounds.x -= SPEED * deltaTime;
    }

    else if (currentKeyStates[SDL_SCANCODE_D] && alienSprite.textureBounds.x < SCREEN_WIDTH - alienSprite.textureBounds.w)
    {
        alienSprite.textureBounds.x += SPEED * deltaTime;
    }
}

void renderSprite(Sprite sprite)
{
    SDL_RenderCopy(renderer, sprite.texture, NULL, &sprite.textureBounds);
}

void render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_QueryTexture(title, NULL, NULL, &titleRect.w, &titleRect.h);
    titleRect.x = SCREEN_WIDTH / 2 - titleRect.w / 2;
    titleRect.y = SCREEN_HEIGHT / 2 - titleRect.h / 2;
    // After I use the &titleRect.w, &titleRect.h in the SDL_QueryTexture.
    //  I get the width and height of the actual texture
    SDL_RenderCopy(renderer, title, NULL, &titleRect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    renderSprite(alienSprite);

    SDL_RenderPresent(renderer);
}

int main(int argc, char *args[])
{
    window = SDL_CreateWindow("My Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (startSDL(window, renderer) > 0)
    {
        return 1;
    }

    // load font
    fontSquare = TTF_OpenFont("res/fonts/square_sans_serif_7.ttf", 36);

    // load title
    updateTextureText(title, "0", fontSquare, renderer);

    alienSprite = loadSprite(renderer, "res/sprites/alien_1.png", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

    actionSound = loadSound("res/sounds/magic.wav");

    // method to reduce the volume of the sound in half.
    Mix_VolumeChunk(actionSound, MIX_MAX_VOLUME / 2);

    // Load music file (only one data piece, intended for streaming)
    music = loadMusic("res/music/music.wav");

    // reduce music volume in half
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);

    // Start playing streamed music, put -1 to loop indifinitely
    Mix_PlayMusic(music, -1);

    Uint32 previousFrameTime = SDL_GetTicks();
    Uint32 currentFrameTime = previousFrameTime;
    float deltaTime = 0.0f;

    while (true)
    {
        currentFrameTime = SDL_GetTicks();
        deltaTime = (currentFrameTime - previousFrameTime) / 1000.0f;
        previousFrameTime = currentFrameTime;

        handleEvents();
        update(deltaTime);
        render();
    }
}