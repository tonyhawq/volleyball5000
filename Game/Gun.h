#pragma once

#include "Gamesprite.h"

namespace vbl
{
	class Game;

	class Gun : public GameSprite
	{
	public:
		Gun(maf::fvec2 dim, strref picture, strref shoot_picture, strref flash_picture, strref bullet_picture, const std::vector<std::string>& casings, strref firing_sound, int ammo, float power, maf::fvec2 offset, maf::fvec2 barrelOffset, int firingDelay);
		Gun(const Gun& other) = default;
		Gun(Gun&&) = default;
		// fuck std::unordered_map and std::unique_ptr
		// i have changed my standing.
		Gun& operator=(const Gun&) = default;

		inline maf::fvec2 getBarrelPos() const { return this->barrelPos; }
		inline void setAbout(maf::fvec2 about) { this->rotatingAbout = about; }
		void setRot(float dir);

		void trigger(Game* world, float dir, int tick);
		void shoot(Game* world, float dir, int tick = -1);

		maf::fvec2 offset;
		maf::fvec2 barrelOffset;
	private:
		maf::fvec2 barrelPos;
		maf::fvec2 rotatingAbout;
		std::string picture;
		std::string shoot_picture;
		std::string bullet_picture;
		std::string flash_picture;
		std::vector<std::string> casing_pictures;
		std::string firing_sound;
		int ammo;
		float power;
		int firingDelay;
		int nextFireTick;
	};
}
