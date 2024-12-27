#pragma once

#include <memory>

#include "vbl.h"
#include "Geometry.h"
#include "GameSprite.h"

namespace vbl
{
	class Ball : public GameSprite
	{
	public:
		enum PowerupType
		{
			POWERUP_NONE = 0,
			POWERUP_TRIPLEJUMP,
			POWERUP_DOUBLEPOINTS,
			POWERUP_SLOWTIME,
		};

		Ball(const std::string& picture, const std::string& glowPicture, float diameter);
		Ball(const std::string& picture, const std::string& glowPicture, float diameter, PowerupType powerup);

		inline void setGravity(float gravity) { this->gravity = gravity; }

		void trigger(const GeometryBox* box);
		inline bool triggered() const { return this->wasTriggered; }
		inline uint16_t team() const { return this->triggeredTeam; }
		inline PowerupType isPowerup() const { return this->powerup; }
		inline bool isTrigger() const { return !this->powerup; }

		uint8_t collidesWithGeometryBox(const GeometryBox* box);
		const std::shared_ptr<vbl::Sprite> collidesWithActor(const std::vector<std::shared_ptr<vbl::Sprite>>& actors);

		const std::vector<maf::ivec2>& trace(const Geometry& geometry, const std::vector<std::shared_ptr<vbl::Sprite>>& actors, uint32_t length, float res, int bounceLimit = 9999, maf::fvec2* ended_pos = NULL);

		void bounceOff(const Geometry& geometry, const std::shared_ptr<vbl::Sprite> sprite, bool simulated = false);
		void moveWithCollision(const Geometry& geometry, const std::vector<std::shared_ptr<vbl::Sprite>> actors, bool simulated = false);
		void update(const Geometry& geometry, const std::vector<std::shared_ptr<vbl::Sprite>> actors, uint32_t tick, bool simulated = false);

		void reset(uint32_t spawnTime);

		inline void setPowerup(PowerupType power) { this->powerup = power; }

		inline uint32_t isSpawning() const { return this->spawning; }
		inline uint8_t getGlow() const { return this->glow; }
		inline const std::string& getGlowTex() { return this->glowTexture; }
		inline bool hitGuy() const { return this->lastBoink > 0; }
		inline float hitStrength() const { return this->lastBoinkStrength; }

		void collisionParticle(int count);

		std::vector<maf::ivec2> tracePoints;
	private:
		uint32_t spawnTime = 0;
		uint32_t spawning = 0;
		const std::string& glowTexture;
		uint8_t glow = 0;
		bool bounced = false;
		PowerupType powerup = POWERUP_NONE;
		std::shared_ptr<vbl::Sprite> wasInside = NULL;
		float gravity = 0;
		bool wasTriggered;
		uint16_t triggeredTeam;
		float lastBoink = 0;
		float lastBoinkStrength = 0;
	};
}
