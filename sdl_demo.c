#include <SDL.h>
#include <stdio.h>

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
const int RECT_SPEED = 5;

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_bool done = SDL_FALSE;
    SDL_Rect movingRect = {0, 0, 60, 40}; // x, y, width, height
    int dx = RECT_SPEED; // Horizontal speed
    int dy = RECT_SPEED; // Vertical speed

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("SDL2 Animation Demo",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    while (!done) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = SDL_TRUE;
            }
        }

        movingRect.x += dx;
        movingRect.y += dy;

        if (movingRect.x < 0 || movingRect.x > WINDOW_WIDTH - movingRect.w) {
            dx = -dx;
        }
        if (movingRect.y < 0 || movingRect.y > WINDOW_HEIGHT - movingRect.h) {
            dy = -dy;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue background
        SDL_RenderClear(renderer);

        // Draw the rectangle in white
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &movingRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
