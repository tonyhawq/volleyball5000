#include "GameSprite.h"

#include "Atlas.h"

vbl::GameSprite::GameSprite(maf::fvec2 dimensions, const IDedPicture& picture, bool useDimensionsForBox)
	: Sprite(dimensions, picture, useDimensionsForBox)
{
	itype = AType::GameSprite;
}

vbl::GameSprite::GameSprite(const GameSprite& other)
	: Sprite(other), particleMap(other.particleMap), soundMap(other.soundMap)
{
	itype = AType::GameSprite;
}

void vbl::GameSprite::spawnParticle(uint32_t lifespan, maf::fvec2 pos, maf::fvec2 vel, const IDedPicture& picture, SDL_Rect box, float rotation, float rotationSpeed, float var)
{
	if (var)
	{
		int randomSize = (int)maf::random(-var, var);
		this->queuedSpawns.push_back(std::move(std::make_unique<Particle>(
			lifespan + uint32_t(maf::random(-var, var) / 2),
			maf::fvec2{ pos.x + maf::random(-var, var), pos.y + maf::random(-var, var) },
			maf::fvec2{ maf::random(-var / 2, var / 2),maf::random(-var / 2, var / 2) },
			picture,
			SDL_Rect{ 0,0, box.w + randomSize, box.h + randomSize },
			rotation,
			rotationSpeed + maf::random(-var, var))));
	}
	else
	{
		this->queuedSpawns.push_back(std::move(std::make_unique<Particle>(lifespan, pos, vel, picture, box, rotation, rotationSpeed)));
	}
}

void vbl::GameSprite::setParticle(const std::string& name, const std::string& picture, const Atlas* atlas)
{
	this->particleMap.insert(std::make_pair(name, IDedPicture{ picture, Atlas::INVALID_CACHED }));
	if (!atlas)
	{
		return;
	}
	IDedPicture& mapped = this->particleMap.at(name);
	atlas->get(mapped.picture, mapped.id);
}