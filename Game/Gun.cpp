#include "Gun.h"

void vbl::Gun::shoot()
{
	if (!ammo)
	{
		return;
	}
	MAABB box;
	box.add({ 0, 0, 20, 20 });
	queuedSpawns.push_back(std::unique_ptr<Particle>(reinterpret_cast<Particle*>(
		new PBRParticle(600,
			this->getVisMid(),
			maf::fvec2{ maf::random(-10.0f, 10.0f),maf::random(-10.0f, 10.0f) },
			"casing",
			SDL_Rect{ 0, 0, 10, 30 },
			0, 10, box))));
}
