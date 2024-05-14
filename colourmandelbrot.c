#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 800
#define MAX_ITER 200 // Making this larger is slower but more accurate. 

// Function to compute the Mandelbrot set
int mandelbrot(double x0, double y0) {
    double x = 0.0;
    double y = 0.0;
    int iteration = 0;
    while (x * x + y * y <= 4.0 && iteration < MAX_ITER) {
        double xtemp = x * x - y * y + x0;
        y = 2 * x * y + y0;
        x = xtemp;
        iteration++;
    }
    return iteration;
}

// Function to generate a color based on the number of iterations
void get_color(int iteration, int max_iter, Uint8 *r, Uint8 *g, Uint8 *b) {
    if (iteration == max_iter) {
        *r = 0;
        *g = 0;
        *b = 0;
    } else {
        double t = (double)iteration / max_iter;
        *r = (Uint8)(9 * (1 - t) * t * t * t * 255);
        *g = (Uint8)(15 * (1 - t) * (1 - t) * t * t * 255);
        *b = (Uint8)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
    }
}

void draw_mandelbrot(SDL_Renderer *renderer, double x_min, double x_max, double y_min, double y_max) {
    for (int px = 0; px < WIDTH; px++) {
        for (int py = 0; py < HEIGHT; py++) {
            double x0 = x_min + (x_max - x_min) * px / WIDTH;
            double y0 = y_min + (y_max - y_min) * py / HEIGHT;

            int iter = mandelbrot(x0, y0);
            Uint8 r, g, b;
            get_color(iter, MAX_ITER, &r, &g, &b);

            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_RenderDrawPoint(renderer, px, py);
        }
    }
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Mandelbrot Fractal",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WIDTH, HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Could not create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    double x_min = -2.0, x_max = 2.0;
    double y_min = -2.0, y_max = 2.0;

    SDL_Event event;
    int running = 1;

    // Draw initial Mandelbrot set
    draw_mandelbrot(renderer, x_min, x_max, y_min, y_max);
    SDL_RenderPresent(renderer);

    // Main loop
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int mx = event.button.x;
                    int my = event.button.y;

                    double x_center = x_min + (x_max - x_min) * mx / WIDTH;
                    double y_center = y_min + (y_max - y_min) * my / HEIGHT;

                    double zoom_factor = 0.1; // Faster zoom
                    double x_span = (x_max - x_min) * zoom_factor;
                    double y_span = (y_max - y_min) * zoom_factor;

                    x_min = x_center - x_span / 2;
                    x_max = x_center + x_span / 2;
                    y_min = y_center - y_span / 2;
                    y_max = y_center + y_span / 2;

                    draw_mandelbrot(renderer, x_min, x_max, y_min, y_max);
                    SDL_RenderPresent(renderer);
                }
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
