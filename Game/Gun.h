#pragma once

#include "Gamesprite.h"

namespace vbl
{
	class Gun : public GameSprite
	{
	public:
		void shoot();
	private:
		int ammo;
	};
}
