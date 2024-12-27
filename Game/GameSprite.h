#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "Sprite.h"
#include "Particles.h"
#include "random.h"

class Atlas;

namespace vbl
{
	typedef const std::string& strref;
	class Game;

	class Actor : public Sprite
	{
	public:
		Actor(maf::fvec2 dimensions, const IDedPicture& picture, bool useDimensionsForBox = false);
		Actor(const Actor& other);
		Actor(Actor&&) = default;

		virtual void update(Game* game);

		typedef std::shared_ptr<Actor> Ref;
	};

	class GameSprite : public Actor
	{
	public:
		GameSprite(maf::fvec2 dimensions, const IDedPicture& picture, bool useDimensionsForBox = false);
		GameSprite(const GameSprite& other);
		GameSprite(GameSprite&&) = default;

		void update(Game* game) override;
		
		inline std::vector<std::unique_ptr<vbl::Particle>>& getParticles() { return this->queuedSpawns; }
		void spawnParticle(uint32_t lifespan, maf::fvec2 pos, maf::fvec2 vel, const IDedPicture& picture, SDL_Rect box, float rotation, float rotationSpeed, float var = 0);
		void setParticle(const std::string& name, const std::string& picture, const Atlas* atlas = NULL);
		inline IDedPicture* getParticle(const std::string& name) { if (!particleMap.count(name)) { return NULL; } return &particleMap[name]; }
		inline void setSound(const std::string& name, int id) { this->soundMap[name] = id; }
		inline void playSound(const std::string& name) { if (!this->soundMap.count(name)) { return; } this->queuedSounds.push_back(soundMap[name]); }
		inline std::vector<int>& getSounds() { return this->queuedSounds; }

	protected:
		std::vector<int> queuedSounds;
		std::vector<std::unique_ptr<vbl::Particle>> queuedSpawns;
		std::unordered_map<std::string, IDedPicture> particleMap;
		std::unordered_map<std::string, int> soundMap;
	};
}
