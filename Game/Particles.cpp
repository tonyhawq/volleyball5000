#include "Particles.h"

#include <ppl.h>

#include "random.h"
#include "Game.h"

vbl::Particle::Particle(uint32_t lifespan, maf::fvec2 pos, maf::fvec2 vel, const IDedPicture& picture , SDL_Rect box, float rotation, float rotationSpeed)
	:lifespan(lifespan), pos(pos), vel(vel), texture(picture, box, rotation), rotationSpeed(rotationSpeed), maxLifespan(lifespan), type(PType::Basic)
{
	this->texture.setPos(this->pos);
}

void vbl::Particle::update()
{
	if (this->lifespan)
	{
		this->lifespan--;
	}
	if (this->fade)
	{
		this->alpha = uint8_t((float)this->lifespan / (float)this->maxLifespan * 255.0f);
	}
	this->texture.rotate(this->rotationSpeed);
	this->pos = this->pos + this->vel;
	this->vel.x *= 0.95f;
	this->vel.y *= 0.95f;
	this->texture.setPos(this->pos);
}

vbl::PBRParticle::PBRParticle(uint32_t lifespan, maf::fvec2 pos, maf::fvec2 vel, const IDedPicture& picture, SDL_Rect box, float rotation, float rotationSpeed, const MAABB& hull)
	:Particle(lifespan, pos, vel, picture, box, rotation, rotationSpeed), hull(hull)
{
	this->type = PType::PBR;
}

void vbl::PBRParticle::update(Game* game)
{
	if (this->lifespan)
	{
		this->lifespan--;
	}
	if (this->fade)
	{
		this->alpha = uint8_t((float)this->lifespan / (float)this->maxLifespan * 255.0f);
	}
	didx = false;
	didy = false;
	this->texture.rotate(this->rotationSpeed);
	this->vel.x *= 0.985f;
	this->vel.y += 0.25f;
	this->pos.x += this->vel.x;
	this->hull.setPos(this->pos);
	if (game->map.geometry.collidesNotrigger(this->hull))
	{
		didx = true;
		this->pos.x -= this->vel.x;
		this->vel.x = this->vel.x * -0.6f;
	}
	this->pos.y += this->vel.y;
	this->hull.setPos(this->pos);
	if (game->map.geometry.collidesNotrigger(this->hull))
	{
		didy = true;
		this->pos.y -= this->vel.y;
		this->vel.y = this->vel.y * -0.3f;
	}
	if (didx || didy)
	{
		this->rotationSpeed *= 0.9f;
	}
	this->texture.setPos(this->pos);
	this->hull.setPos(this->pos);
}

vbl::Particle* vbl::ParticleManager::spawnParticle(uint32_t lifespan, maf::fvec2 pos, maf::fvec2 vel, const IDedPicture& picture, SDL_Rect box, float rotation, float rotationSpeed)
{
	particles.push_front(std::move(std::make_unique<Particle>(lifespan, pos, vel, picture, box, rotation, rotationSpeed)));
	return particles.front().get();
}

void vbl::ParticleManager::spawnCustom(Particle* particle)
{
	particles.push_front(std::move(std::unique_ptr<Particle>(particle)));
}

void vbl::ParticleManager::spewParticles(uint32_t lifespan, maf::fvec2 pos, maf::fvec2 vel, const IDedPicture& picture, SDL_Rect box, float rotation, float rotationSpeed, uint32_t variations, float varDist)
{
	for (uint32_t i = 0; i < variations; i++)
	{
		spawnParticle(
			lifespan + uint32_t(maf::random(-varDist, varDist)),
			{ pos.x + maf::random(-varDist, varDist), pos.y + maf::random(-varDist, varDist) },
			{ vel.x + maf::random(-varDist, varDist), vel.y + maf::random(-varDist, varDist) },
			picture,
			box,
			rotation + maf::random(-varDist, varDist),
			rotationSpeed + maf::random(-varDist, varDist)
			);
	}
}

void vbl::ParticleManager::process()
{
	if (this->particles.empty())
	{
		return;
	}
	bool eraseLast = false;
	std::list<std::unique_ptr<vbl::Particle>>::iterator lastIter = this->particles.begin();
	for (auto iter = this->particles.begin(); iter != this->particles.end(); iter++)
	{
		if (eraseLast)
		{
			this->particles.erase(lastIter);
			eraseLast = false;
		}
		switch ((*iter)->type)
		{
		case PType::Basic:
			(*iter)->update();
			break;
		case PType::PBR:
			PBRParticle* casted = reinterpret_cast<PBRParticle*>((*iter).get());
			casted->update(this->game);
			break;
		}
		if (!(*iter)->lifespan)
		{
			eraseLast = true;
		}
		lastIter = iter;
	}
}

void vbl::ParticleManager::addParticles(std::vector<std::unique_ptr<vbl::Particle>>& particles)
{
	while (particles.size())
	{
		// size_t narrow
		int index = int(particles.size()) - 1;
		if (!particles[index].get())
		{
			particles.pop_back();
			continue;
		}
		this->particles.push_back(std::move(particles[index]));
		particles.pop_back();
	}
}
