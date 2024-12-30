#include "Gun.h"

#include "Game.h"

vbl::Gun::Gun(maf::fvec2 dim, strref picture, strref shoot_picture, strref flash_picture, strref bullet_picture,
	const std::vector<std::string>& casings, const std::vector<std::string>& firing_sound, int ammo, float power, maf::fvec2 offset, maf::fvec2 barrelOffset, int firingDelay)
	:GameSprite(dim, Picture{ picture }, true),
	ammo(ammo),
	power(power),
	picture(Picture(picture)),
	shoot_picture(Picture(shoot_picture)),
	bullet_picture(Picture(bullet_picture)),
	casing_pictures(),
	firing_sounds(firing_sound),
	offset(offset),
	barrelOffset(barrelOffset),
	flash_picture(flash_picture),
	firingDelay(firingDelay),
	nextFireTick(0)
{
	this->casing_pictures.reserve(casings.size());
	for (const auto& picture : casings)
	{
		this->casing_pictures.push_back(Picture(picture));
	}
}

void vbl::Gun::setRot(float dir)
{
	this->setVisMid(maf::rotatePointRad(this->offset, this->rotatingAbout, dir));
	this->barrelPos = maf::rotatePointRad(this->barrelOffset, this->rotatingAbout, dir);
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
	this->shoot(world, dir, tick);
}

void vbl::Gun::shoot(Game* world, float dir, int tick)
{
	if (tick >= 0)
	{
		this->nextFireTick = tick + this->firingDelay;
	}
	printf("shot");
	MAABB box;
	box.add({ 0, 0, 20, 20 });
	queuedSpawns.push_back(std::move(std::unique_ptr<Particle>(reinterpret_cast<Particle*>(
		new PBRParticle(600,
			this->getVisMid(),
			maf::fvec2{ maf::random(-10.0f, 10.0f),maf::random(-10.0f, 10.0f) },
			// size_t narrow
			maf::randomItem<std::vector<IDedPicture>, IDedPicture>(this->casing_pictures),
			SDL_Rect{ 0, 0, 10, 30 },
			0, 10, box)))));
	maf::fvec2 flash_size = { 26 * 4, 26 * 4 };
	maf::fvec2 spawn_location = barrelPos - flash_size / 2;
	queuedSpawns.push_back(std::move(std::unique_ptr<Particle>(
		new Particle(6,
			spawn_location,
			{ 0 },
			this->flash_picture,
			{ 0, 0, 26 * 4, 26 * 4},
			this->getRotation() + maf::half_turn,
			0
		)
	)));
	printf("spawned particle at %f, %f vs gun pos %f %f\n", spawn_location.x, spawn_location.y, this->pos.x, this->pos.y);
	const auto& picked = maf::randomItem<std::vector<std::string>, std::string>(this->firing_sounds);
	printf("picked %s out of:", picked.c_str());
	for (const auto& pair : this->soundMap)
	{
		printf("  \"%s\"\n", pair.first.c_str());
	}
	queuedSounds.push_back(this->soundMap[picked]);
	Actor::Ref bullet = std::make_shared<Actor>(maf::fvec2{ 2, 2 }, this->bullet_picture, true);
	bullet->setPos(this->pos);
	bullet->setVel({ std::sin(dir) * 4, std::cos(dir) * 4 });
}
