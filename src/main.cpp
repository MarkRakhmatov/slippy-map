#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <set>
#include <string>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


int main(int, char*[]){
	if(!SDL_Init(SDL_INIT_VIDEO)){
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
		exit(-1);
	}
	int width = 1200, height = 800;
	SDL_Window* window = SDL_CreateWindow("slippymap", width, height, SDL_WINDOW_RESIZABLE);
    
	SDL_Surface *screen = SDL_GetWindowSurface(window);
    int offsetx = 0;
    int offsety = 0;
    int x = 0, y = 0;
    int zoom = 12;
    const int tilesize = 256;
    SDL_Rect dest{x * tilesize - offsetx, y * tilesize - offsety, tilesize, tilesize};
	while(dest.x <= -tilesize){
		dest.x += (1 << zoom) * tilesize;
	}
	SDL_FillSurfaceRect(
        screen,
        &dest,
        SDL_MapRGB(SDL_GetPixelFormatDetails(screen->format),
        SDL_GetSurfacePalette(screen), 255, 255, 255));
	SDL_UpdateWindowSurface(window);
	return 0;
}