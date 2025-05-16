#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define INITIAL_WIDTH 800
#define INITIAL_HEIGHT 800

int MAX_ITER = 200; // Default value

// Cache to store Mandelbrot set calculations
int *mandelbrot_cache;
int cache_size = INITIAL_WIDTH * INITIAL_HEIGHT;

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

void draw_mandelbrot(SDL_Renderer *renderer, double x_min, double x_max, double y_min, double y_max, int size, int offset_x, int offset_y) {
    for (int px = 0; px < size; px++) {
        for (int py = 0; py < size; py++) {
            double x0 = x_min + (x_max - x_min) * px / size;
            double y0 = y_min + (y_max - y_min) * py / size;

            int cache_index = py * size + px;
            int iter;
            if (mandelbrot_cache[cache_index] != -1) {
                iter = mandelbrot_cache[cache_index];
            } else {
                iter = mandelbrot(x0, y0);
                mandelbrot_cache[cache_index] = iter;
            }

            Uint8 r, g, b;
            get_color(iter, MAX_ITER, &r, &g, &b);

            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_RenderDrawPoint(renderer, px + offset_x, py + offset_y);
        }
    }
}

void clear_cache(int size) {
    for (int i = 0; i < size * size; i++) {
        mandelbrot_cache[i] = -1; // Reset cache to uncomputed state
    }
}

void resize_cache(int new_size) {
    free(mandelbrot_cache);
    mandelbrot_cache = (int *)malloc(new_size * new_size * sizeof(int));
    clear_cache(new_size);
    cache_size = new_size;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        MAX_ITER = atoi(argv[1]);
    }

    mandelbrot_cache = (int *)malloc(INITIAL_WIDTH * INITIAL_HEIGHT * sizeof(int));
    clear_cache(INITIAL_WIDTH);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Mandelbrot Fractal",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          INITIAL_WIDTH, INITIAL_HEIGHT,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        SDL_Quit();
        free(mandelbrot_cache);
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Could not create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        free(mandelbrot_cache);
        return 1;
    }

    double x_min = -2.0, x_max = 2.0;
    double y_min = -2.0, y_max = 2.0;

    SDL_Event event;
    int running = 1;
    int window_width = INITIAL_WIDTH;
    int window_height = INITIAL_HEIGHT;

    // Draw initial Mandelbrot set
    int size = window_width < window_height ? window_width : window_height;
    int offset_x = (window_width - size) / 2;
    int offset_y = (window_height - size) / 2;
    draw_mandelbrot(renderer, x_min, x_max, y_min, y_max, size, offset_x, offset_y);
    SDL_RenderPresent(renderer);

    // Main loop
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int mx = event.button.x - offset_x;
                    int my = event.button.y - offset_y;

                    if (mx >= 0 && mx < size && my >= 0 && my < size) {
                        double x_center = x_min + (x_max - x_min) * mx / size;
                        double y_center = y_min + (y_max - y_min) * my / size;

                        double zoom_factor = 0.2; // Faster zoom
                        double x_span = (x_max - x_min) * zoom_factor;
                        double y_span = (y_max - y_min) * zoom_factor;

                        x_min = x_center - x_span / 2;
                        x_max = x_center + x_span / 2;
                        y_min = y_center - y_span / 2;
                        y_max = y_center + y_span / 2;

                        clear_cache(size); // Clear cache before redrawing

                        draw_mandelbrot(renderer, x_min, x_max, y_min, y_max, size, offset_x, offset_y);
                        SDL_RenderPresent(renderer);
                    }
                }
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_q) {
                    running = 0; // Quit on 'Q' key press
                } else {
                    // Reset zoom to initial values on any key press
                    x_min = -2.0;
                    x_max = 2.0;
                    y_min = -2.0;
                    y_max = 2.0;

                    size = window_width < window_height ? window_width : window_height;
                    offset_x = (window_width - size) / 2;
                    offset_y = (window_height - size) / 2;
                    clear_cache(size); // Clear cache before redrawing

                    draw_mandelbrot(renderer, x_min, x_max, y_min, y_max, size, offset_x, offset_y);
                    SDL_RenderPresent(renderer);
                }
            } else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                window_width = event.window.data1;
                window_height = event.window.data2;

                size = window_width < window_height ? window_width : window_height;
                offset_x = (window_width - size) / 2;
                offset_y = (window_height - size) / 2;
                resize_cache(size);

                draw_mandelbrot(renderer, x_min, x_max, y_min, y_max, size, offset_x, offset_y);
                SDL_RenderPresent(renderer);
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    free(mandelbrot_cache);

    return 0;
}
