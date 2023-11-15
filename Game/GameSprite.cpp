#include "GameSprite.h"

vbl::GameSprite::GameSprite(maf::fvec2 dimensions, SDL_Texture* tex, bool useDimensionsForBox)
	: Sprite(dimensions, tex, useDimensionsForBox)
{

}

void vbl::GameSprite::spawnParticle(uint32_t lifespan, maf::fvec2 pos, maf::fvec2 vel, SDL_Texture* texture, SDL_Rect box, float rotation, float rotationSpeed, float var)
{
	if (var)
	{
		int randomSize = (int)maf::random(-var, var);
		this->queuedSpawns.push_back(Particle(
			lifespan + uint32_t(maf::random(-var, var) / 2),
			{ pos.x + maf::random(-var, var), pos.y + maf::random(-var, var) },
			{ maf::random(-var / 2, var / 2),maf::random(-var / 2, var / 2) },
			texture,
			{ 0,0, box.w + randomSize, box.h + randomSize },
			rotation,
			rotationSpeed + maf::random(-var, var)));
	}
	else
	{
		this->queuedSpawns.push_back(Particle(lifespan, pos, vel, texture, box, rotation, rotationSpeed));
	}
}

void vbl::GameSprite::setParticle(const std::string& name, SDL_Texture* texture)
{
	if (this->particleMap.count(name))
	{
		SDL_DestroyTexture(this->particleMap[name]);
	}
	this->particleMap[name] = texture;
}