#include "sdl.h"

void init_sdl() {
	printf("Initializing SDL2 ...\n");

    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_EVENTS) > 0) {
        printf("SDL2 initialization failed!\n");
        SDL_GetError();
    }

    if(!(IMG_Init(IMG_INIT_PNG))) {
        printf("SDL2 IMG_Init of PNG failed!\n");
        SDL_GetError();
    }
}

SDL_Window*  init_window(const char* title, const u_int16_t width, const u_int16_t height) {
	SDL_Window*   top_win  = NULL;
    top_win = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
    
    if(!top_win) {
        printf("Initializing SDL_Window failed!\n");
        SDL_GetError();
        SDL_Quit();
    }
    
    return top_win;
}

SDL_Renderer* init_renderer(const int gpu, SDL_Window* top_win_ptr) {
	int renderCount = SDL_GetNumRenderDrivers();
    printf("Total render drivers: %d\n", renderCount);
    for(int i = 0; i < renderCount; i++) {
        printf("Render driver: %d\n", i);
        SDL_RendererInfo info;
        memset(&info, 0, sizeof(info));
        if(!SDL_GetRenderDriverInfo(i, &info)) {
            printf("\tName: %s\n", info.name);
            printf("\tAccelerated: %s\n", info.flags & SDL_RENDERER_ACCELERATED ? "YES" : "NO");
        } else {
            printf("No driver info available\n");
        }
}
	
	SDL_Renderer* renderer = NULL;
    renderer = SDL_CreateRenderer(top_win_ptr, -1, SDL_RENDERER_ACCELERATED);

    if(!renderer) {
        printf("Initializing SDL_Renderer failed!\n");
        SDL_GetError();
        SDL_DestroyWindow(top_win_ptr);
        SDL_Quit();
    }
    
    return renderer;
}

float get_runtime_in_seconds()
{
    return SDL_GetTicks() * 0.001f;
}

int get_refresh_rate(SDL_Window* win)
{
    int display_index = SDL_GetWindowDisplayIndex(win);
    SDL_DisplayMode mode;
    SDL_GetDisplayMode(display_index, 0, &mode);

    return mode.refresh_rate;
}

SDL_Texture* load_texture(SDL_Texture* texture_ptr, const char* texture_path, SDL_Window* top_win, SDL_Renderer* renderer)
{
	if(texture_ptr != NULL) {
		printf("Texture cannot be filled because not null! Exiting with one!\n");
		exit(1);
	}
	
	texture_ptr = IMG_LoadTexture(renderer, texture_path);
	
	if(!texture_ptr) {
        printf("Initializing SDL_Texture* %p FAILED!\n", texture_ptr);
        SDL_GetError();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(top_win);
        SDL_Quit();
    }
    
    printf("Initializing SDL_Texture* %p SUCCESSFUL!\n", texture_ptr);
    
    return texture_ptr;
}

SDL_Rect* create_sdl_rect(int x, int y, int w, int h) 
{
	SDL_Rect* rect = malloc(1 * sizeof(SDL_Rect));
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;
    
    return rect;
}

void deinit_sdl(super_world* sw, SDL_Window* sdl_window, SDL_Renderer* sdl_renderer) {
	free_ecs(sw);
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
}
