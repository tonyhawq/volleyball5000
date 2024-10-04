#pragma once

#include <SDL.h>
#include <string>
#include <unordered_map>
#include <memory>

#include "Geometry.h"
#include "Particles.h"
#include "Atlas.h"
#include "Text.h"
#include "Ball.h"

namespace vbl
{
	class Renderer
	{
	public:
		Renderer(uint32_t width, uint32_t height, uint32_t atlasWidth, uint32_t atlasHeight, float renderScale);

		void clearFrame();
		void clearFrame(SDL_Color clr);
		void presentFrame();

		void renderSpriteConst(const SpriteTexture& sprite, uint8_t alpha = 255);
		void renderSprite(SpriteTexture& sprite, uint8_t alpha = 255);
		void renderText(std::shared_ptr<RawTexture> text);
		void renderRect(maf::frect rect, SDL_Color color);
		void renderBoundingBox(const MAABB& box, SDL_Color clr = {0, 255, 0, 255});
		void renderGeometry(const Geometry& geometry);
		void renderGeometry(const Geometry& geometry, bool debug);
		void renderTrace(const vbl::Ball::Trace& trace, SpriteTexture& endTex);

		void debugRender(SDL_Surface* surface);
		void debugRender(SDL_Texture* texture);

		void loadChars(const std::string& fontPath, SDL_Color clr);

		void scaleRect(SDL_Rect* rect);
		void mapRect(SDL_Rect* rect);
		void mapRect(maf::frect* rect);
		void mapRect(SDL_FRect* rect);

		/// <summary>
		/// texture is default responsible
		/// </summary>
		std::shared_ptr<RawTexture> makeText(const std::string& str);

		SDL_Texture* load(const std::string& path, bool ignoreMissing = false);
		inline void setMissingTexture(const std::string& str) { this->missingTex = str; }

		inline SDL_Renderer* getRenderer() { return this->renderer; }

		inline maf::fvec2 getPos() const { return this->pos; }
		inline void setPos(maf::fvec2 pos) { this->pos = pos; }
		inline void move(maf::fvec2 vel) { this->pos.x += vel.x; this->pos.y += vel.y; }

		Atlas atlas;
	private:
		maf::fvec2 pos;
		SDL_Surface* fontSurface = NULL;
		std::unordered_map<char, int> charMap;
		float renderScale;
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Texture* target;
		std::string missingTex;
	};
}
