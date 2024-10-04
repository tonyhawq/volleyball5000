#pragma once

#include <SDL.h>

#include "maf.h"

namespace vbl
{
	/// <summary>
	/// a container for an SDL_Texture*, when copying the memory is leaked.
	/// </summary>
	class RawTexture
	{
	public:
		/// <summary>
		/// own
		/// </summary>
		RawTexture(SDL_Rect pos, SDL_Texture* texture);
		RawTexture();
		~RawTexture();

		inline void setPos(maf::ivec2 pos) { this->pos.x = pos.x; this->pos.y = pos.y; }
		inline SDL_Rect* changePos() { return &this->pos; }
		inline SDL_Rect getPos() { return this->pos; }
		inline SDL_Texture* getTexture() { return this->texture; }
		inline SDL_Texture* changeTexture(SDL_Texture* newTexture) { SDL_Texture* temp = this->texture; this->texture = newTexture; return temp; }
		inline void setMiddle(maf::ivec2 pos) { this->pos.x = pos.x - this->pos.w / 2; this->pos.y = pos.y - this->pos.h / 2; }
		inline void resize(float percent) { this->pos.w = int((float)this->pos.w * percent); this->pos.h = int((float)this->pos.h * percent); }
		inline void setRect(SDL_Rect rect) { this->pos = rect; }

		inline void beResponsible() { this->responsible = true; }
		inline void beIrresponsible() { this->responsible = false; }
	private:
		bool responsible = true;
		SDL_Rect pos;
		SDL_Texture* texture;
	};
}
