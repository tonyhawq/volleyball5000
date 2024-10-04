#pragma once

#include <list>
#include <memory>

#include "Sprite.h"

namespace vbl
{
	enum class PType
	{
		Basic = 0,
		PBR = 1,
	};
	class Game;

	class Particle
	{
	public:
		Particle(uint32_t lifespan, maf::fvec2 pos, maf::fvec2 vel, const std::string& picture, SDL_Rect box, float rotation, float rotationSpeed);

		inline SpriteTexture& getTexture() { return this->texture; }
		inline const SpriteTexture& getTexture() const { return this->texture; }
		inline SpriteTexture& changeTexture() { return this->texture; }
		inline uint8_t getAlpha() const { return this->alpha; }
		void update();
		inline void fades(bool doesIt) { this->fade = doesIt; }
		uint32_t lifespan;

		PType type;
	protected:
		uint8_t alpha = 255;
		uint32_t maxLifespan;
		bool fade = true;
		float rotationSpeed;
		maf::fvec2 vel;
		maf::fvec2 pos;
		SpriteTexture texture;
	};

	class PBRParticle : public Particle
	{
	public:
		PBRParticle(uint32_t lifespan, maf::fvec2 pos, maf::fvec2 vel, const std::string& picture, SDL_Rect box, float rotation, float rotationSpeed, const MAABB& hull);
	
		void update(Game* game);
		inline const MAABB& getHull() const { return this->hull; }
		bool didx = false;
		bool didy = false;
	protected:
		MAABB hull;
	};

	class ParticleManager
	{
	public:
		vbl::Particle* spawnParticle(uint32_t lifespan, maf::fvec2 pos, maf::fvec2 vel, const std::string& picture, SDL_Rect box, float rotation, float rotationSpeed);
		void spewParticles(uint32_t lifespan, maf::fvec2 pos, maf::fvec2 vel, const std::string& picture, SDL_Rect box, float rotation, float rotationSpeed, uint32_t variations, float varDist);
		void spawnCustom(Particle* particle);

		void addParticles(std::vector<std::unique_ptr<vbl::Particle>>& particles);
		void process();

		inline const std::list<std::unique_ptr<vbl::Particle>>& getParticles() const { return this->particles; }
		Game* game;
	private:
		std::list<std::unique_ptr<vbl::Particle>> particles;
	};
}
