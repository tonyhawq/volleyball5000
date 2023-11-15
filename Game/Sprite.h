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
		SpriteTexture(SDL_Texture* texture, SDL_Rect box, float rotation, bool responsible = true);
		SpriteTexture(SDL_Texture* texture, SDL_Rect box, float rotation, maf::ivec2 spriteDimensions, bool responsible = true);
		SpriteTexture();
		~SpriteTexture();

		inline void setPos(maf::ivec2 pos) { this->textureBox.x = pos.x; this->textureBox.y = pos.y; }
		inline void resize(maf::ivec2 dimensions) { this->textureBox.w = dimensions.x; this->textureBox.h = dimensions.y; }
		inline void resize(float by) { this->textureBox.w *= by; this->textureBox.h *= by; }
		inline void rotate(float degrees) { this->rotation += degrees; }
		inline float getRotation() const { return this->rotation; }
		inline void setMiddle(maf::ivec2 pos) { setPos({ pos.x - this->textureBox.w / 2, pos.y - this->textureBox.h / 2 }); }
		inline maf::ivec2 getMiddle() const { return { this->textureBox.x + this->textureBox.w / 2,this->textureBox.y + this->textureBox.h / 2 }; }
		inline maf::ivec2 feet() const { return { this->textureBox.x + this->textureBox.w / 2, this->textureBox.y + this->textureBox.h }; }
		inline void destroyTexture() { SDL_DestroyTexture(this->texture); this->texture = NULL; }

		inline const SDL_Rect* getRect() const { return &textureBox; }
		inline SDL_Texture* getTexture() const { return texture; }
		inline SDL_Texture* setTexture(SDL_Texture* texture) { SDL_Texture* temp = this->texture; this->texture = texture; return temp; }

		inline void setAnimated(bool is) { this->animated = is; }
		inline bool isAnimated() const { return this->animated; }

		inline void setAnimState(int state) { this->animState = state; }
		inline int animstate() const { return this->animState; }
		inline void setSpriteWidth(int w) { this->spriteDimensions.x = w; }
		inline void setSpriteHeight(int h) { this->spriteDimensions.y = h; }
		inline SDL_Rect getClippingRect() const { return {this->spriteDimensions.x * this->animState, 0, this->spriteDimensions.x, spriteDimensions.y}; }
	
		bool responsible = true;
	private:
		bool animated = false;
		maf::ivec2 spriteDimensions;
		int animState;
		float rotation;
		SDL_Texture* texture;
		SDL_Rect textureBox;
	};

	class Sprite
	{
	public:
		Sprite(maf::fvec2 dimensions, SDL_Texture* tex, bool useDimensionsForBox = false);
		Sprite(maf::fvec2 dimensions, SDL_Texture* tex, maf::ivec2 spriteDimensions, bool useDimensionsForBox = false);

		void move(maf::fvec2 much);
		void setPos(maf::fvec2 pos);
		inline void setVel(maf::fvec2 vel) { this->vel = vel; }
		inline maf::fvec2 getPos() const { return this->pos; }
		inline maf::fvec2 getVel() const { return this->vel; }
		inline maf::fvec2 getVisMid() const { return { (float)this->texture.getMiddle().x, (float)this->texture.getMiddle().y }; }

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
