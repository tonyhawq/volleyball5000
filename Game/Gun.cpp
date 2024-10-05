#include "Gun.h"

vbl::Gun::Gun(strref picture, strref shoot_picture, strref bullet_picture, const std::vector<std::string>& casings, const std::vector<std::string>& firing_sounds, int ammo, float power)
	:GameSprite({ 24, 24 }, picture, true), ammo(ammo), power(power), picture(picture), shoot_picture(shoot_picture), bullet_picture(bullet_picture), casing_pictures(casings), firing_sounds(firing_sounds)
{

}

void vbl::Gun::shoot()
{
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
