#include "Gun.h"

#include "Game.h"

vbl::Gun::Gun(maf::fvec2 dim, strref picture, strref shoot_picture, strref flash_picture, strref bullet_picture, const std::vector<std::string>& casings, strref firing_sound, int ammo, float power, maf::fvec2 offset, maf::fvec2 barrelOffset, int firingDelay)
	:GameSprite(dim, Picture{ picture }, true),
	ammo(ammo),
	power(power),
	picture(picture),
	shoot_picture(shoot_picture),
	bullet_picture(bullet_picture),
	casing_pictures(casings),
	firing_sound(firing_sound),
	offset(offset),
	barrelOffset(barrelOffset),
	flash_picture(flash_picture),
	firingDelay(firingDelay),
	nextFireTick(0)
{

}

void vbl::Gun::setRot(float dir)
{
	this->setVisMid(maf::rotatePoint(this->offset, this->rotatingAbout, dir));
	this->barrelPos = maf::rotatePoint(this->barrelOffset, this->rotatingAbout, dir);
}

void vbl::Gun::trigger(Game* world, float dir, int tick)
{
	if (tick < this->nextFireTick)
	{
		return;
	}
	if (!ammo)
	{
		return;
	}
	ammo--;
	this->nextFireTick = tick + this->firingDelay;
	this->shoot(world, dir);
}

void vbl::Gun::shoot(Game* world, float dir, int tick)
{
	if (tick >= 0)
	{
		this->nextFireTick = tick + this->firingDelay;
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
	queuedSpawns.push_back(std::move(std::unique_ptr<Particle>(
		new Particle(30,
			barrelPos - maf::fvec2{ 13.0f, 13.0f },
			{ 0 },
			this->flash_picture,
			{ 26, 26 },
			this->getRotation(),
			0
		)
	)));
	queuedSounds.push_back(this->soundMap[this->firing_sound]);
	world->spawnActor({barrelPos.x, barrelPos.y, 6, 6}, {power * sin(dir), power * cos(dir)}, {this->bullet_picture}, dir);
}
