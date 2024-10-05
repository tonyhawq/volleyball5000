#pragma once

#include "Gamesprite.h"

namespace vbl
{
	class Gun : public GameSprite
	{
	public:
		Gun(strref picture, strref shoot_picture, strref bullet_picture, const std::vector<std::string>& casings, const std::vector<std::string>& firing_sounds, int ammo, float power);
		Gun(const Gun& other) = default;
		Gun(Gun&&) = default;
		// fuck std::unordered_map and std::unique_ptr
		Gun& operator=(const Gun&)
		{
			return *this;
		}

		void shoot();

		inline Gun* clone() const { return new Gun(*this); };
	private:
		std::string picture;
		std::string shoot_picture;
		std::string bullet_picture;
		std::vector<std::string> casing_pictures;
		std::vector<std::string> firing_sounds;
		int ammo;
		float power;
	};
}
