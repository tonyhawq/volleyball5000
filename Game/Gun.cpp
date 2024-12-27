#include "Gun.h"

#include "Game.h"

vbl::Gun::Gun(maf::fvec2 dim, strref picture, strref shoot_picture, strref bullet_picture, const std::vector<std::string>& casings, const std::vector<std::string>& firing_sounds, int ammo, float power, maf::fvec2 offset, maf::fvec2 barrelOffset)
	:GameSprite(dim, Picture{ picture }, true),
	ammo(ammo),
	power(power),
	picture(picture),
	shoot_picture(shoot_picture),
	bullet_picture(bullet_picture),
	casing_pictures(casings),
	firing_sounds(firing_sounds),
	offset(offset),
	barrelOffset(barrelOffset)
{

}

void vbl::Gun::trigger(Game* world, float dir, int tick)
{
	if (tick < this->nextFireTick)
	{
		return;
	}
	this->nextFireTick = tick + this->firingDelay;
	this->shoot(world);
}

void vbl::Gun::shoot(Game* world, float dir, int tick)
{
	if (tick >= 0)
	{
		this->nextFireTick = tick + this->firingDelay;
	}
	if (!ammo)
	{
		return;
	}
	MAABB box;
	box.add({ 0, 0, 20, 20 });
	queuedSpawns.push_back(std::move(std::unique_ptr<Particle>(reinterpret_cast<Particle*>(
		new PBRParticle(600,
			this->getVisMid(),
			maf::fvec2{ maf::random(-10.0f, 10.0f),maf::random(-10.0f, 10.0f) },
			// size_t narrow
			this->casing_pictures[maf::random(0, int(this->casing_pictures.size()) - 1)],
			SDL_Rect{ 0, 0, 10, 30 },
			0, 10, box)))));
}
