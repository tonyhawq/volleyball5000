#pragma once

#include "Geometry.h"
#include "Particles.h"

#include <SDL.h>
#include <string>

namespace vbl
{
	class Renderer
	{
	public:
		Renderer(uint32_t width, uint32_t height, float renderScale);

		void clearFrame();
		void clearFrame(SDL_Color clr);
		void presentFrame();

		void renderSprite(const SpriteTexture& sprite);
		void renderSprite(const SpriteTexture& sprite, uint8_t alpha);
		void renderBoundingBox(const MAABB& box, SDL_Color clr = {0, 255, 0, 255});
		void renderGeometry(const Geometry& geometry);
		void renderGeometry(const Geometry& geometry, bool debug);
		void renderTrace(const std::vector<maf::ivec2>& points);

		void scaleRect(SDL_Rect* rect);

		SDL_Texture* load(const std::string& path);
	private:
		float renderScale;
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Texture* target;
	};
}

