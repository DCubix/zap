#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

#if __has_include("SDL2/SDL.h")
#	include "SDL2/SDL.h"
#else
#	include "SDL.h"
#endif

class Renderer {
public:
	Renderer() = default;
	explicit Renderer(SDL_Renderer* base) : m_renderer(base) {}

	void draw(SDL_Texture* texture, int x, int y, int w, int h, int sx=0, int sy=0, int sw=0, int sh=0);
	void tile(SDL_Texture* texture, int x, int y, int w, int h, int index=0, int cols=1, int rows=1);

	void render(int screenWidth, int screenHeight);

private:
	struct Sprite {
		SDL_Texture* texture;
		SDL_Rect src;
		SDL_Rect dest;
	};

	std::vector<Sprite> m_sprites;

	SDL_Renderer* m_renderer;
};

#endif // RENDERER_H
