#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <functional>
#include <stack>
#include <bitset>

#if __has_include("SDL2/SDL.h")
#	include "SDL2/SDL.h"
#else
#	include "SDL.h"
#endif

#include "renderer.h"
#include "sprites.hpp"

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
	int state{ 0 };
};

class Map {
public:
	Map(int size = 100) {
		m_size = size;
		m_tiles.resize(size * size);
		std::fill(m_tiles.begin(), m_tiles.end(), Tile{});

		m_tempTiles.resize(size * size);
		std::fill(m_tempTiles.begin(), m_tempTiles.end(), Tile{});
	}

	Tile* get(int x, int y) {
		if (x < 0 || y < 0 || x >= m_size || y >= m_size) return nullptr;
		return &m_tiles[x + y * m_size];
	}

	Tile* getTemp(int x, int y) {
		if (x < 0 || y < 0 || x >= m_size || y >= m_size) return nullptr;
		return &m_tempTiles[x + y * m_size];
	}

	void place(int x, int y, Tile::Type type, int state = 0) {
		if (x < 0 || y < 0 || x >= m_size || y >= m_size) return;
		const int i = x + y * m_size;
		m_tiles[i].type = type;
		m_tiles[i].state = state;
		m_tempTiles[i].type = type;
		m_tempTiles[i].state = state;
	}

	void setNextState(int x, int y, int state) {
		if (x < 0 || y < 0 || x >= m_size || y >= m_size) return;
		const int i = x + y * m_size;
		m_tempTiles[i].state = state;
	}

	void setCurrentState(int x, int y, int state) {
		if (x < 0 || y < 0 || x >= m_size || y >= m_size) return;
		const int i = x + y * m_size;
		m_tiles[i].state = state;
	}

	int getNextState(int x, int y) {
		if (x < 0 || y < 0 || x >= m_size || y >= m_size) return 0;
		const int i = x + y * m_size;
		return m_tempTiles[i].state;
	}

	int getCurrentState(int x, int y) {
		if (x < 0 || y < 0 || x >= m_size || y >= m_size) return 0;
		const int i = x + y * m_size;
		return m_tiles[i].state;
	}

	Tile::Type has(int x, int y) {
		auto tile = get(x, y);
		if (tile == nullptr) return Tile::None;
		return tile->type;
	}

	void forEachNeighbor(int x, int y, const std::function<void(int, int)>& cb) {
		int pos[8] = {
			x - 1, y,
			x + 1, y,
			x, y - 1,
			x, y + 1
		};
		for (int i = 0; i < 8; i+=2) {
			cb(pos[i], pos[i + 1]);
		}
	}

	void forEachTile(Tile::Type type, const std::function<bool(int, int)>& cb) {
		for (int i = 0; i < m_size * m_size; i++) {
			int x = i % m_size,
				y = i / m_size;
			auto ctile = get(x, y);
			if (ctile->type != type) continue;
			if (cb(x, y)) break;
		}
	}

	void simulationStep() {
		forEachTile(Tile::PowerSupply, [&](int x, int y) {
			bool left   = has(x - 1, y) == Tile::None;
			bool right  = has(x + 1, y) == Tile::None;
			bool top    = has(x, y - 1) == Tile::None;
			bool bottom = has(x, y + 1) == Tile::None;

			uint8_t flag;
			if (left) flag |= 0b1000;
			if (right) flag |= 0b0100;
			if (top) flag |= 0b0010;
			if (bottom) flag |= 0b0001;

			switch (flag) {
				default: break;
				case 0b1011: setNextState(x + 1, y, 1); break;
				case 0b0111: setNextState(x - 1, y, 1); break;
				case 0b1110: setNextState(x, y + 1, 1); break;
				case 0b1101: setNextState(x, y - 1, 1); break;
				case 0b1010: setNextState(x, y + 1, 1); setNextState(x + 1, y, 1); break;
				case 0b0110: setNextState(x, y + 1, 1); setNextState(x - 1, y, 1); break;
				case 0b0101: setNextState(x, y - 1, 1); setNextState(x - 1, y, 1); break;
				case 0b1001: setNextState(x, y - 1, 1); setNextState(x + 1, y, 1); break;
				case 0b1000: setNextState(x + 1, y, 1); setNextState(x, y - 1, 1); setNextState(x, y + 1, 1); break;
				case 0b0100: setNextState(x - 1, y, 1); setNextState(x, y - 1, 1); setNextState(x, y + 1, 1); break;
				case 0b0010: setNextState(x + 1, y, 1); setNextState(x - 1, y, 1); setNextState(x, y + 1, 1); break;
				case 0b0001: setNextState(x + 1, y, 1); setNextState(x - 1, y, 1); setNextState(x, y - 1, 1); break;
			}

			return false;
		});

		forEachTile(Tile::Wire, [&](int x, int y) {
			bool left   = has(x - 1, y) == Tile::None;
			bool right  = has(x + 1, y) == Tile::None;
			bool top    = has(x, y - 1) == Tile::None;
			bool bottom = has(x, y + 1) == Tile::None;

			uint8_t flag;
			if (left) flag |= 0b1000;
			if (right) flag |= 0b0100;
			if (top) flag |= 0b0010;
			if (bottom) flag |= 0b0001;

			switch (flag) {
				default: break;
				case 0b1011: setNextState(x + 1, y, 1); break;
				case 0b0111: setNextState(x - 1, y, 1); break;
				case 0b1110: setNextState(x, y + 1, 1); break;
				case 0b1101: setNextState(x, y - 1, 1); break;
				case 0b1010: setNextState(x, y + 1, 1); setNextState(x + 1, y, 1); break;
				case 0b0110: setNextState(x, y + 1, 1); setNextState(x - 1, y, 1); break;
				case 0b0101: setNextState(x, y - 1, 1); setNextState(x - 1, y, 1); break;
				case 0b1001: setNextState(x, y - 1, 1); setNextState(x + 1, y, 1); break;
				case 0b1000: setNextState(x + 1, y, 1); setNextState(x, y - 1, 1); setNextState(x, y + 1, 1); break;
				case 0b0100: setNextState(x - 1, y, 1); setNextState(x, y - 1, 1); setNextState(x, y + 1, 1); break;
				case 0b0010: setNextState(x + 1, y, 1); setNextState(x - 1, y, 1); setNextState(x, y + 1, 1); break;
				case 0b0001: setNextState(x + 1, y, 1); setNextState(x - 1, y, 1); setNextState(x, y - 1, 1); break;
			}

			return false;
			return false;
		});
		
		swap();
	}

	void drawLayer(Renderer& ren, SDL_Texture* tiles, Tile::Type type) {
		if (type == Tile::None) return;
		for (int y = 0; y < m_size; y++) {
			for (int x = 0; x < m_size; x++) {
				int ax = x * 32;
				int ay = y * 32;
				auto ctile = get(x, y);
				if (ctile->type != type) continue;

				switch (type) {
					default: break;
					case Tile::PowerSupply: {
						ren.tile(tiles, ax, ay, 32, 32, Power, 4, 20);
					} break;
					case Tile::Wire: {
						uint8_t flag = 0;

						Tile::Type sides[4] = {
							has(x - 1, y), // 2
							has(x + 1, y), // 4
							has(x, y - 1), // 8
							has(x, y + 1)  // 16
						};

						for (int i = 0; i < 4; i++) {
							if (sides[i] == Tile::None) continue;
							flag |= (1 << (i+1));
						}

						uint8_t tile = 0;
						switch (flag) {
							default: break;
							case 2:
							case 4:
							case 6: tile = ctile->state>0 ? WireHOn : WireH; break;
							case 8:
							case 16:
							case 24: tile = ctile->state>0 ? WireVOn : WireV; break;
							case 18: tile = ctile->state>0 ? WireLBOn : WireLB; break;
							case 10: tile = ctile->state>0 ? WireLTOn : WireLT; break;
							case 20: tile = ctile->state>0 ? WireRBOn : WireRB; break;
							case 12: tile = ctile->state>0 ? WireRTOn : WireRT; break;
							case 28: tile = ctile->state>0 ? WireRTBOn : WireRTB; break;
							case 22: tile = ctile->state>0 ? WireLRBOn : WireLRB; break;
							case 26: tile = ctile->state>0 ? WireLTBOn : WireLTB; break;
							case 14: tile = ctile->state>0 ? WireLRTOn : WireLRT; break;
							case 30: tile = ctile->state>0 ? WireLRTBOn : WireLRTB; break;
						}
						ren.tile(tiles, ax, ay, 32, 32, tile, 4, 20);
						
						if (sides[0] != Tile::Wire && sides[0] != Tile::None) ren.tile(tiles, ax, ay, 32, 32, ConnectorL, 4, 20);
						if (sides[1] != Tile::Wire && sides[1] != Tile::None) ren.tile(tiles, ax, ay, 32, 32, ConnectorR, 4, 20);
						if (sides[2] != Tile::Wire && sides[2] != Tile::None) ren.tile(tiles, ax, ay, 32, 32, ConnectorT, 4, 20);
						if (sides[3] != Tile::Wire && sides[3] != Tile::None) ren.tile(tiles, ax, ay, 32, 32, ConnectorB, 4, 20);
					} break;
				}
			}
		}
	}

private:
	int m_size;
	std::vector<Tile> m_tiles, m_tempTiles;

	void swap() {
		for (int i = 0; i < m_size * m_size; i++) {
			std::swap(m_tiles[i], m_tempTiles[i]);
		}
	}
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

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	int windowWidth = 0,
		windowHeight = 0;
	SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	const int mapSize = 100;
	Map map{};

	Renderer ren(renderer);

	SDL_Event event;
	bool running = true, drag = false;
	int mx = 0, my = 0;

	Tile::Type type = Tile::Wire;

	const double timeStep = 1.0 / 60.0;
	double lastTime = double(SDL_GetTicks()) / 1000.0;
	double accum = 0.0;

	double tickTime = 0.0;

	while (running) {
		double current = double(SDL_GetTicks()) / 1000.0;
		double delta = current - lastTime;
		lastTime = current;
		accum += delta;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) running = false;

			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
				type = type == Tile::Wire ? Tile::PowerSupply : Tile::Wire;
			}

			if (event.type == SDL_MOUSEMOTION) {
				mx = std::floor(event.motion.x / 32) * 32;
				my = std::floor(event.motion.y / 32) * 32;
				if (drag) {
					map.place(mx / 32, my /32, type);
				}
			}

			if (event.type == SDL_MOUSEBUTTONDOWN) {
				mx = std::floor(event.button.x / 32) * 32;
				my = std::floor(event.button.y / 32) * 32;
				map.place(mx / 32, my /32, type);
				drag = true;
			} else if (event.type == SDL_MOUSEBUTTONUP) {
				drag = false;
			}
		}

		while (accum >= timeStep) {
			tickTime += timeStep;
			if (tickTime >= 0.05) {
				map.simulationStep();
				tickTime = 0;
			}
			accum -= timeStep;
		}

		SDL_SetRenderDrawColor(renderer, 0, 50, 100, 255);
		SDL_RenderClear(renderer);

		// BACKGROUND
		// for (int y = 0; y < mapSize; y++) {
		// 	for (int x = 0; x < mapSize; x++) {
		// 		ren.tile(tiles, x * 32, y * 32, 32, 32, Background, 4, 20);
		// 	}
		// }

		map.drawLayer(ren, tiles, Tile::Wire);
		map.drawLayer(ren, tiles, Tile::PowerSupply);

		ren.tile(tiles, mx, my, 32, 32, Cursor, 4, 20);

		ren.render(windowWidth, windowHeight);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	return 0;
}