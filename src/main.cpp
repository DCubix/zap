#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdint>

#if __has_include("SDL2/SDL.h")
#	include "SDL2/SDL.h"
#else
#	include "SDL.h"
#endif

#include "renderer.h"

struct Tile {
	enum Type {
		None = 0,
		Wire,
		Connector,
		PowerSupply,
		LED,
		AND,
		OR,
		Inverter,
		Switch
	};

	Type type{ Type::None };
	int orientation{ 0 };
	bool state{ false };
};

class Map {
public:
	Map(int size = 100) {
		m_size = size;
		m_tiles.resize(size * size);
		std::fill(m_tiles.begin(), m_tiles.end(), Tile{});
	}

	Tile* get(int x, int y) {
		if (x < 0 || y < 0 || x >= m_size || y >= m_size) return nullptr;
		return &m_tiles[x + y * m_size];
	}

	void drawLayer(Renderer& ren, SDL_Texture* tiles, Tile::Type type) {
		if (type == Tile::None) return;
		for (int y = 0; y < m_size; y++) {
			for (int x = 0; x < m_size; x++) {
				int ax = x * 32;
				int ay = y * 32;
				auto tile = get(x, y);
				if (tile->type != type) continue;

				switch (type) {
					default: break;
					case Tile::Wire: {
						uint8_t flag = 0;
						uint8_t cflag = 0;

						auto left = get(x - 1, y); // 2
						auto right = get(x + 1, y); // 4
						auto top = get(x, y - 1); // 8
						auto bottom = get(x, y + 1); // 16
						auto sides = { left, right, top, bottom };

						int i = 0;
						for (auto t : sides) {
							if (t == nullptr) {
								i++;
								continue;
							}
							flag |= (1 << (i+1));
							i++;
						}

						switch (flag) {
							default: ren.tile(tiles, ax, ay, 32, 32,  0,  4, 10); break;
							case 2: ren.tile(tiles, ax, ay, 32, 32,  connector[0] ? 7 : 1,  4, 10); break;
							case 4: ren.tile(tiles, ax, ay, 32, 32,  connector[1] ? 9 : 1,  4, 10); break;
							case 6: ren.tile(tiles, ax, ay, 32, 32,  1,  4, 10); break;
							case 8: ren.tile(tiles, ax, ay, 32, 32,  connector[2] ? 6 : 0,  4, 10); break;
							case 16: ren.tile(tiles, ax, ay, 32, 32,  connector[3] ? 8 : 0,  4, 10); break;
							case 24: ren.tile(tiles, ax, ay, 32, 32,  0,  4, 10); break;

							case 18: ren.tile(tiles, ax, ay, 32, 32,  connector[0] ? 43,  4, 10); break;

							case 10: ren.tile(tiles, ax, ay, 32, 32,  3,  4, 10); break;
							case 20: ren.tile(tiles, ax, ay, 32, 32,  4,  4, 10); break;
							case 12: ren.tile(tiles, ax, ay, 32, 32,  5,  4, 10); break;
							case 28: ren.tile(tiles, ax, ay, 32, 32,  32,  4, 10); break;
							case 22: ren.tile(tiles, ax, ay, 32, 32,  33,  4, 10); break;
							case 26: ren.tile(tiles, ax, ay, 32, 32,  34,  4, 10); break;
							case 14: ren.tile(tiles, ax, ay, 32, 32,  35,  4, 10); break;
							case 30: ren.tile(tiles, ax, ay, 32, 32,  36,  4, 10); break;
						}
					} break;
				}
			}
		}
	}

private:
	int m_size;
	std::vector<Tile> m_tiles;
};

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

	int windowWidth = 0,
		windowHeight = 0;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	const int mapSize = 100;
	Map map{};

	Renderer ren(renderer);

	SDL_Event event;
	bool running = true, drag = false;
	int mx = 0, my = 0;

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) running = false;

			if (event.type == SDL_MOUSEMOTION) {
				mx = std::floor(event.motion.x / 32) * 32;
				my = std::floor(event.motion.y / 32) * 32;
				if (drag) {
					auto tile = map.get(mx / 32, my /32);
					if (tile != nullptr) {
						tile->type = Tile::Wire;
					}
				}
			}

			if (event.type == SDL_MOUSEBUTTONDOWN) {
				mx = std::floor(event.button.x / 32) * 32;
				my = std::floor(event.button.y / 32) * 32;
				auto tile = map.get(mx / 32, my /32);
				if (tile != nullptr) {
					tile->type = Tile::Wire;
				}
				drag = true;
			} else if (event.type == SDL_MOUSEBUTTONUP) {
				drag = false;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 50, 100, 255);
		SDL_RenderClear(renderer);

		// BACKGROUND
		for (int y = 0; y < mapSize; y++) {
			for (int x = 0; x < mapSize; x++) {
				ren.tile(tiles, x * 32, y * 32, 32, 32, 10, 4, 10);
			}
		}

		map.drawLayer(ren, tiles, Tile::Wire);

		ren.tile(tiles, mx, my, 32, 32, 11, 4, 10);

		ren.render(windowWidth, windowHeight);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	return 0;
}