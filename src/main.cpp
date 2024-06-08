#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <vector>

#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Math.hpp"


int main(int argc, char *args[])
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        std::cout << "SDL crashed. Error: " << SDL_GetError();
    }

    if (!IMG_Init(IMG_INIT_PNG))
    {
        std::cout << "SDL_image crashed. Error: " << SDL_GetError();
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        // success = false;
    }

    RenderWindow window("GAME v1.0", 1280, 720);

    SDL_Texture *sampleTex = window.loadTexture("res/gfx/logo.png");
    
    Mix_Chunk *test = window.loadSound("res/sounds/magic.wav");

    // dynamic array of entities
    std::vector<Entity> entities = {
        Entity(0, 0, sampleTex, 766, 766)};

    bool gameRunning = true;

    SDL_Event event;

    while (gameRunning)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameRunning = false;
            }

            if (event.key.keysym.sym == SDLK_SPACE)
            {
                Mix_PlayChannel( -1, test, 0 );
            }
        }

        window.clearWindow();

        for (Entity &e : entities)
        {
            window.renderEntity(e);
        }

        window.display();
    }

    window.cleanUp();
    return 0;
}