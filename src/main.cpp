#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>

const int SPEED = 600;
const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 544;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

Mix_Chunk *test = nullptr;
SDL_Texture *sprite = nullptr;

SDL_Rect spriteBounds = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 38, 34};

SDL_Texture *title = nullptr;
SDL_Rect titleRect;

SDL_Texture *subtitle = nullptr;
SDL_Rect subtitleRect;

SDL_Surface *pressStartSurface = nullptr;

TTF_Font *fontSquare = nullptr;
SDL_Color fontColor = {255, 255, 255};

void updateTitle(const char *text)
{
    SDL_Surface *surface1 = TTF_RenderUTF8_Blended(fontSquare, text, fontColor);
    if (surface1 == NULL)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to load create title! SDL Error: %s\n", SDL_GetError());
        exit(3);
    }
    SDL_DestroyTexture(title);
    title = SDL_CreateTextureFromSurface(renderer, surface1);
    if (title == NULL)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to load texture for image block.bmp! SDL Error: %s\n", SDL_GetError());
    }
    SDL_FreeSurface(surface1);
}

SDL_Texture *loadSprite(const char *file, SDL_Renderer *renderer)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer, file);
    return texture;
}

void renderSprite(SDL_Texture *sprite, SDL_Renderer *renderer, SDL_Rect spriteBounds)
{
    SDL_QueryTexture(sprite, NULL, NULL, &spriteBounds.w, &spriteBounds.h);
    SDL_RenderCopy(renderer, sprite, NULL, &spriteBounds);
}

Mix_Chunk *loadSound(const char *p_filePath)
{
    Mix_Chunk *sound = nullptr;

    sound = Mix_LoadWAV(p_filePath);
    if (sound == nullptr)
    {
        printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
    }

    return sound;
}

void quitGame()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
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

        if (event.key.keysym.sym == SDLK_SPACE)
        {
            Mix_PlayChannel(-1, test, 0);
        }
    }
}

void update(float deltaTime)
{
    const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);

    if (currentKeyStates[SDL_SCANCODE_W] && spriteBounds.y > 0)
    {
        spriteBounds.y -= SPEED * deltaTime;
    }

    else if (currentKeyStates[SDL_SCANCODE_S] && spriteBounds.y < SCREEN_HEIGHT - spriteBounds.h)
    {
        spriteBounds.y += SPEED * deltaTime;
    }

    else if (currentKeyStates[SDL_SCANCODE_A] && spriteBounds.x > 0)
    {
        spriteBounds.x -= SPEED * deltaTime;
    }

    else if (currentKeyStates[SDL_SCANCODE_D] && spriteBounds.x < SCREEN_WIDTH - spriteBounds.w)
    {
        spriteBounds.x += SPEED * deltaTime;
    }
}

void render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_QueryTexture(title, NULL, NULL, &titleRect.w, &titleRect.h);
    titleRect.x = SCREEN_WIDTH / 2 - titleRect.w / 2;
    titleRect.y = SCREEN_HEIGHT / 2 - titleRect.h / 2;
    SDL_RenderCopy(renderer, title, NULL, &titleRect);
    SDL_RenderCopy(renderer, subtitle, NULL, &subtitleRect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    renderSprite(sprite, renderer, spriteBounds);

    SDL_RenderPresent(renderer);
}

int main(int argc, char *args[])
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        std::cout << "SDL crashed. Error: " << SDL_GetError();
    }

    window = SDL_CreateWindow("My Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr)
    {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (!IMG_Init(IMG_INIT_PNG))
    {
        std::cout << "SDL_image crashed. Error: " << SDL_GetError();
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }

    if (TTF_Init() == -1)
    {
        return 5;
    }
    // open font files
    TTF_Font *pressStartFont = TTF_OpenFont("res/fonts/PressStart2P.ttf", 16);
    if (pressStartFont == nullptr)
    {
        printf("TTF_OpenFont font2P: %s\n", TTF_GetError());
        return 2;
    }
    fontSquare = TTF_OpenFont("res/fonts/square_sans_serif_7.ttf", 64);
    if (fontSquare == nullptr)
    {
        printf("TTF_OpenFont fontSquare: %s\n", TTF_GetError());
        return 2;
    }
    // load title
    updateTitle("Hello! test");

    // load subtitle
    pressStartSurface = TTF_RenderUTF8_Blended(pressStartFont, "Press start to quit", fontColor);
    if (pressStartSurface == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to load create subtitle! SDL Error: %s\n", SDL_GetError());
        return 3;
    }
    subtitle = SDL_CreateTextureFromSurface(renderer, pressStartSurface);
    if (title == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to load texture for image block.bmp! SDL Error: %s\n", SDL_GetError());
        return 4;
    }
    SDL_FreeSurface(pressStartSurface);

    SDL_QueryTexture(subtitle, NULL, NULL, &subtitleRect.w, &subtitleRect.h);
    // After use the &subtitleRect.w, &subtitleRect.h in the SDL_QueryTexture.
    //  I get the width and height of the actual texture
    subtitleRect.x = SCREEN_WIDTH / 2 - subtitleRect.w / 2;
    subtitleRect.y = subtitleRect.h / 2;

    sprite = loadSprite("res/sprites/alien_1.png", renderer);

    test = loadSound("res/sounds/magic.wav");

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

    return 0;
}