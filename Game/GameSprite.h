#pragma once

#include <unordered_map>
#include <string>

#include "Sprite.h"
#include "Particles.h"
#include "random.h"

namespace vbl
{
	class GameSprite : public Sprite
	{
	public:
		GameSprite(maf::fvec2 dimensions, SDL_Texture* tex, bool useDimensionsForBox = false);
		
		inline std::vector<Particle>& getParticles() { return this->queuedSpawns; }
		void spawnParticle(uint32_t lifespan, maf::fvec2 pos, maf::fvec2 vel, SDL_Texture* texture, SDL_Rect box, float rotation, float rotationSpeed, float var = 0);
		void setParticle(const std::string& name, SDL_Texture* texture);
		inline SDL_Texture* getParticle(const std::string& name) { if (!particleMap.count(name)) { return NULL; } return particleMap[name]; }
		inline void setSound(const std::string& name, int id) { this->soundMap[name] = id; }
		inline void playSound(const std::string& name) { if (!this->soundMap.count(name)) { return; } this->queuedSounds.push_back(soundMap[name]); }
		inline std::vector<int>& getSounds() { return this->queuedSounds; }
	protected:
		std::vector<int> queuedSounds;
		std::vector<Particle> queuedSpawns;
		std::unordered_map<std::string, SDL_Texture*> particleMap;
		std::unordered_map<std::string, int> soundMap;
	};
}
