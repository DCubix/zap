#include <iostream>
#include <cmath>

#if __has_include("SDL2/SDL.h")
#	include "SDL2/SDL.h"
#else
#	include "SDL.h"
#endif

void drawTile(
	SDL_Renderer* renderer,
	SDL_Texture* tex,
	int index,
	int x, int y,
	int rows, int cols
) {
	//int w, h;
	//SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);

	const int tw = 32;
	const int th = 32;
	int tx = (index % cols) * tw;
	int ty = (index / cols) * th;

	SDL_Rect src = { tx, ty, tw, th };
	SDL_Rect dst = { x, y, tw, th };
	SDL_RenderCopy(renderer, tex, &src, &dst);
}

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow(
		"ZAP",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600,
		SDL_WINDOW_SHOWN
	);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Surface* tilesSurf = SDL_LoadBMP("tiles.bmp");
	SDL_SetColorKey(tilesSurf, 1, SDL_MapRGB(tilesSurf->format, 0, 255, 0));
	SDL_Texture* tiles = SDL_CreateTextureFromSurface(renderer, tilesSurf);
	SDL_FreeSurface(tilesSurf);

	SDL_Event event;
	bool running = true;
	int mx = 0, my = 0;

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) running = false;

			if (event.type == SDL_MOUSEMOTION) {
				mx = std::floor(event.motion.x / 32) * 32;
				my = std::floor(event.motion.y / 32) * 32;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 50, 100, 255);
		SDL_RenderClear(renderer);

		for (int y = 0; y < 600; y+=32) {
			for (int x = 0; x < 800; x+=32) {
				drawTile(renderer, tiles, 10, x, y, 10, 4);
			}
		}
		drawTile(renderer, tiles, 11, mx, my, 10, 4);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	return 0;
}