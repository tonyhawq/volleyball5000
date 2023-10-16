#pragma once

#include "vbl.h"
#include "Geometry.h"

namespace vbl
{
	class Ball : public Sprite
	{
	public:
		Ball(SDL_Texture* texture, float diameter);

		inline void setGravity(float gravity) { this->gravity = gravity; }

		void trigger(const GeometryBox* box);
		inline bool triggered() const { return this->wasTriggered; }
		inline uint16_t team() const { return this->triggeredTeam; }

		uint8_t collidesWithGeometryBox(const GeometryBox* box);
		const Sprite* collidesWithActor(const std::vector<Sprite*>& actors);

		const std::vector<maf::ivec2>& trace(const Geometry& geometry, const std::vector<Sprite*>& actors, uint32_t length, float res);

		void bounceOff(const Sprite* sprite);
		void moveWithCollision(const Geometry& geometry, const std::vector<Sprite*> actors);
		void update(const Geometry& geometry, const std::vector<Sprite*> actors);

		void reset();
	private:
		std::vector<maf::ivec2> tracePoints;
		const Sprite* wasInside = NULL;
		float gravity = 0;
		bool wasTriggered;
		uint16_t triggeredTeam;
	};
}
