#ifndef SDL_H
#define SDL_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_vulkan.h>

#include "ecs.h"

/* #################################################################### */
/* ================ START FUNCTION PROTOTYPES ========================= */
/* #################################################################### */
void          init_sdl();
SDL_Window*   init_window(const char* title, const u_int16_t width, const u_int16_t height);
SDL_Renderer* init_renderer(const int gpu, SDL_Window* top_win_ptr);
void          deinit_sdl(super_world* sw, SDL_Window* sdl_window, SDL_Renderer* sdl_renderer);

float         get_runtime_in_seconds();
int           get_refresh_rate(SDL_Window* win);
SDL_Texture*  load_texture(SDL_Texture* texture_ptr, const char* texture_path, SDL_Window* top_win, SDL_Renderer* renderer);
SDL_Rect*     create_sdl_rect(int x, int y, int w, int h);
/* #################################################################### */
/* ================ END FUNCTION PROTOTYPES =========================== */
/* #################################################################### */
#endif
