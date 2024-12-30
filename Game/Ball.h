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

		struct Trace
		{
			std::vector<maf::ivec2> points;
			uint32_t length;
		};

		Ball(const IDedPicture& picture, const IDedPicture& glowPicture, float diameter);
		Ball(const IDedPicture& picture, const IDedPicture& glowPicture, float diameter, PowerupType powerup);

		inline void setGravity(float gravity) { this->gravity = gravity; }

		void trigger(const GeometryBox* box);
		inline bool triggered() const { return this->wasTriggered; }
		inline uint16_t team() const { return this->triggeredTeam; }
		inline PowerupType isPowerup() const { return this->powerup; }
		inline bool isTrigger() const { return !this->powerup; }

		uint8_t collidesWithGeometryBox(const GeometryBox* box);
		const Actor::Ref collidesWithActor(const std::vector<Actor::Ref>& actors);

		const Trace& trace(const Geometry& geometry, const std::vector<Actor::Ref>& actors, float resolution, uint32_t length, int bounceLimit = 1000, maf::fvec2* ended_pos = NULL);

		void bounceOff(const Geometry& geometry, const Actor::Ref sprite, bool simulated = false);
		void moveWithCollision(const Geometry& geometry, const std::vector<Actor::Ref>& actors, float resolution, bool simulated = false);
		void update(Game* game) override;
		void abstractUpdate(uint32_t tick, const Geometry& geometry, const std::vector<Actor::Ref>& actors, float resolution, bool simulated);

		void reset(uint32_t spawnTime);

		inline void setPowerup(PowerupType power) { this->powerup = power; }

		inline uint32_t isSpawning() const { return this->spawning; }
		inline uint8_t getGlow() const { return this->glow; }
		inline const SpriteTexture& getGlowTex() { this->glowTexture.setPos(this->pos); return this->glowTexture; }
		inline bool hitGuy() const { return this->lastBoink > 0; }
		inline float hitStrength() const { return this->lastBoinkStrength; }

		void collisionParticle(int count);

		Trace tracer;
	private:
		std::vector<uint32_t> cached_res;
		uint32_t spawnTime = 0;
		uint32_t spawning = 0;
		SpriteTexture glowTexture;
		uint8_t glow = 0;
		bool bounced = false;
		PowerupType powerup = POWERUP_NONE;
		float gravity = 0;
		bool wasTriggered;
		uint16_t triggeredTeam;
		float lastBoink = 0;
		float lastBoinkStrength = 0;
	};
}
