#pragma once

#include <vector>
#include <SDL.h>

#include "maf.h"

namespace vbl
{
	class MAABB
	{
	public:
		MAABB(const std::vector<maf::frect>& rects);
		MAABB();

		void setPos(maf::fvec2 pos);
		void move(maf::fvec2 vel);

		static bool collide(const MAABB& one, const MAABB& two);
		bool collide(const MAABB& other) const;
		void add(maf::frect box);

		void recalculateDimensions();

		inline maf::fvec2 getMiddle() const { return { this->pos.x + this->dimensions.x / 2, this->pos.y + this->dimensions.y / 2 }; }
		inline maf::fvec2 getPos() const { return this->pos; }

		inline const std::vector<maf::frect>& getBoxes() const { return boxes; }
	private:
		maf::fvec2 pos;
		maf::fvec2 dimensions;
		std::vector<maf::frect> boxes;
	};

	class SpriteTexture
	{
	public:
		SpriteTexture(SDL_Texture* texture, SDL_Rect box, float rotation);
		SpriteTexture();

		inline void setPos(maf::ivec2 pos) { this->textureBox.x = pos.x; this->textureBox.y = pos.y; }
		inline void resize(maf::ivec2 dimensions) { this->textureBox.w = dimensions.x; this->textureBox.h = dimensions.y; }
		inline void rotate(float degrees) { this->rotation += degrees; }
		inline float getRotation() const { return this->rotation; }
		inline maf::ivec2 getMiddle() const { return { this->textureBox.x + this->textureBox.w / 2,this->textureBox.y + this->textureBox.h / 2 }; }

		inline const SDL_Rect* getRect() const { return &textureBox; }
		inline SDL_Texture* getTexture() const { return texture; }
		inline SDL_Texture* setTexture(SDL_Texture* texture) { SDL_Texture* temp = this->texture; this->texture = texture; return temp; }
	private:
		float rotation;
		SDL_Texture* texture;
		SDL_Rect textureBox;
	};

	class Sprite
	{
	public:
		Sprite(maf::fvec2 dimensions, SDL_Texture* tex, bool useDimensionsForBox = false);

		void move(maf::fvec2 much);
		void setPos(maf::fvec2 pos);
		inline void setVel(maf::fvec2 vel) { this->vel = vel; }
		inline maf::fvec2 getPos() const { return this->pos; }
		inline maf::fvec2 getVel() const { return this->vel; }

		inline const SpriteTexture& getTexture() const { return this->texture; }
		inline SpriteTexture& changeTexture() { return this->texture; }
		inline const MAABB& getBox() const { return this->box; }
		inline MAABB& changeBox() { return this->box; }
		inline void setBox(const MAABB& box) { this->box = box; }
	protected:
		/// <summary>
		/// top left of sprite
		/// </summary>
		maf::fvec2 pos;
		maf::fvec2 vel;
		MAABB box;
		SpriteTexture texture;
	};
}
