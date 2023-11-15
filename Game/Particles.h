#pragma once

#include <list>

#include "Sprite.h"

namespace vbl
{
	class Particle
	{
	public:
		Particle(uint32_t lifespan, maf::fvec2 pos, maf::fvec2 vel, SDL_Texture* texture, SDL_Rect box, float rotation, float rotationSpeed);

		inline const SpriteTexture& getTexture() const { return this->texture; }
		inline uint8_t getAlpha() const { return this->alpha; }
		void update();
		uint32_t lifespan;
	private:
		uint32_t maxLifespan;
		bool fade = true;
		uint8_t alpha = 255;
		float rotationSpeed;
		maf::fvec2 vel;
		maf::fvec2 pos;
		SpriteTexture texture;
	};

	class ParticleManager
	{
	public:
		void spawnParticle(uint32_t lifespan, maf::fvec2 pos, maf::fvec2 vel, SDL_Texture* texture, SDL_Rect box, float rotation, float rotationSpeed);
		void spewParticles(uint32_t lifespan, maf::fvec2 pos, maf::fvec2 vel, SDL_Texture* texture, SDL_Rect box, float rotation, float rotationSpeed, uint32_t variations, float varDist);
		
		void addParticles(std::vector<Particle>& particles);
		void process();

		inline const std::list<Particle>& getParticles() const { return this->particles; }
	private:
		std::list<Particle> particles;
	};
}
