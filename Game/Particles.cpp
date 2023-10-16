#include "Particles.h"

vbl::Particle::Particle(uint32_t lifespan, maf::fvec2 pos, maf::fvec2 vel, SDL_Texture* texture, SDL_Rect box, float rotation, float rotationSpeed)
	:lifespan(lifespan), pos(pos), vel(vel), texture(texture, box, rotation), rotationSpeed(rotationSpeed), maxLifespan(lifespan)
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
		this->alpha = (float)this->lifespan / (float)this->maxLifespan * 255;
		SDL_SetTextureAlphaMod(this->texture.getTexture(), this->alpha);
	}
	this->texture.rotate(this->rotationSpeed);
	this->pos = this->pos + this->vel;
	this->vel.x *= 0.95;
	this->vel.y *= 0.95;
	this->texture.setPos(this->pos);
}


void vbl::ParticleManager::spawnParticle(uint32_t lifespan, maf::fvec2 pos, maf::fvec2 vel, SDL_Texture* texture, SDL_Rect box, float rotation, float rotationSpeed)
{
	particles.push_front(Particle(lifespan, pos, vel, texture, box, rotation, rotationSpeed));
}

void vbl::ParticleManager::spewParticles(uint32_t lifespan, maf::fvec2 pos, maf::fvec2 vel, SDL_Texture* texture, SDL_Rect box, float rotation, float rotationSpeed, uint32_t variations, float varDist)
{
	for (uint32_t i = 0; i < variations; i++)
	{
		spawnParticle(
			lifespan + rand() % (int)varDist,
			{ pos.x + float(rand() % (int)varDist * 20) / 20.0f, pos.y + float(rand() % (int)varDist * 20) / 20.0f },
			{ vel.x + float(rand() % (int)varDist * 20) / 20.0f, vel.y + float(rand() % (int)varDist * 20) / 20.0f },
			texture,
			box,
			rotation + float(rand() % (int)varDist * 20) / 20.0f,
			rotationSpeed + float(rand() % (int)varDist * 20) / 20.0f
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
	std::list<Particle>::iterator lastIter = this->particles.begin();
	for (auto iter = this->particles.begin(); iter != this->particles.end(); iter++)
	{
		if (eraseLast)
		{
			this->particles.erase(lastIter);
			eraseLast = false;
		}
		iter->update();
		if (!iter->lifespan)
		{
			eraseLast = true;
		}
		lastIter = iter;
	}
}