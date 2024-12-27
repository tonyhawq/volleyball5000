#pragma once

#include "Gamesprite.h"

namespace vbl
{
	class Game;

	class Gun : public GameSprite
	{
	public:
		Gun(maf::fvec2 dim, strref picture, strref shoot_picture, strref bullet_picture, const std::vector<std::string>& casings, const std::vector<std::string>& firing_sounds, int ammo, float power, maf::fvec2 offset, maf::fvec2 barrelOffset);
		Gun(const Gun& other) = default;
		Gun(Gun&&) = default;
		// fuck std::unordered_map and std::unique_ptr
		// i have changed my standing.
		Gun& operator=(const Gun&) = default;

		void trigger(Game* world, float dir, int tick);
		void shoot(Game* world, float dir, int tick = -1);

		maf::fvec2 offset;
		maf::fvec2 barrelOffset;
	private:
		IDedPicture picture;
		IDedPicture shoot_picture;
		IDedPicture bullet_picture;
		std::vector<IDedPicture> casing_pictures;
		std::vector<std::string> firing_sounds;
		int ammo;
		float power;
		int firingDelay;
		int nextFireTick;
	};
}
