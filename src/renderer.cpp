#include "renderer.h"

void Renderer::draw(SDL_Texture* texture, int x, int y, int w, int h, int sx, int sy, int sw, int sh) {
	Sprite spr;
	spr.texture = texture;
	spr.src.x = sx;
	spr.src.y = sy;
	spr.src.w = sw;
	spr.src.h = sh;
	spr.dest.x = x;
	spr.dest.y = y;
	spr.dest.w = w;
	spr.dest.h = h;
	m_sprites.push_back(spr);
}

void Renderer::tile(SDL_Texture* texture, int x, int y, int w, int h, int index, int cols, int rows) {
	int txw = 0, txh = 0;
	SDL_QueryTexture(texture, nullptr, nullptr, &txw, &txh);

	const int tw = txw / cols;
	const int th = txh / rows;
	int tx = (index % cols) * tw;
	int ty = (index / cols) * th;
	draw(texture, x, y, w, h, tx, ty, tw, th);
}

void Renderer::render(int screenWidth, int screenHeight) {
	std::vector<Sprite> culledSprites;
	culledSprites.reserve(m_sprites.size());

	for (auto spr : m_sprites) {
		if (spr.dest.x + spr.dest.w < 0 ||
			spr.dest.x >= screenWidth ||
			spr.dest.y + spr.dest.h < 0 ||
			spr.dest.y >= screenHeight)
			continue;
		if (spr.texture == nullptr) continue;
		culledSprites.push_back(spr);
	}

	for (auto& spr : culledSprites) {
		SDL_RenderCopy(m_renderer, spr.texture, &spr.src, &spr.dest);
	}

	m_sprites.clear();
}