#include "Ball.h"

vbl::Ball::Ball(SDL_Texture* texture, SDL_Texture* glowTexture, float diameter)
	:GameSprite({diameter, diameter}, texture, true), wasTriggered(false), triggeredTeam(0), glowTexture(glowTexture)
{

}

vbl::Ball::Ball(SDL_Texture* texture, SDL_Texture* glowTexture, float diameter, PowerupType powerup)
	:GameSprite({diameter, diameter}, texture, true), wasTriggered(false), triggeredTeam(0), powerup(powerup), glowTexture(glowTexture)
{

}

void vbl::Ball::trigger(const GeometryBox* box)
{
	if (!box)
	{
		this->wasTriggered = false;
		this->triggeredTeam = 0;
		return;
	}
	if (!box->trigger)
	{
		return;
	}
	this->wasTriggered = true;
	this->triggeredTeam = box->team;
}

void vbl::Ball::bounceOff(const std::shared_ptr<vbl::Sprite> sprite, bool simulated)
{
	if (!sprite)
	{
		return;
	}
	if (sprite == this->wasInside)
	{
		return;
	}
	bounced = true;
	maf::fvec2 otherPos = sprite->getBox().getMiddle();
	maf::fvec2 otherVel = sprite->getVel();
	float speedDiff = abs(this->vel.x - otherVel.x) + abs(this->vel.y - otherVel.y);
	float dir = (float)maf::pointTowards(this->box.getMiddle(), otherPos);
	float sin = std::sin(dir);
	float cos = std::cos(dir);
	this->texture.rotate((float)maf::radToDegrees(dir) - this->texture.getRotation());
	if (speedDiff > 10 && !simulated && lastBoink <= 0)
	{
		maf::fvec2 boingsize = { 8 * speedDiff, 4 * speedDiff };
		this->spawnParticle(
			30,
			maf::setMiddle(this->getVisMid(), boingsize),
			{ 0,0 },
			this->getParticle("bnk"),
			{ 0,0,(int)boingsize.x,(int)boingsize.y },
			this->texture.getRotation(),
			0
		);
		this->playSound("boink");
		lastBoink = 15;
		lastBoinkStrength = speedDiff;
	}
	this->vel.x = -sin * 5 + otherVel.x;
	this->vel.y = cos * 5 + otherVel.y;
	if (simulated)
	{
		return;
	}
	collisionParticle(std::max(speedDiff, 4.0f));
}

const std::shared_ptr<vbl::Sprite> vbl::Ball::collidesWithActor(const std::vector<std::shared_ptr<vbl::Sprite>>& actors)
{
	for (const auto& actor : actors)
	{
		if (this->box.collide(actor->getBox()))
		{
			return actor;
		}
	}
	return NULL;
}

uint8_t vbl::Ball::collidesWithGeometryBox(const GeometryBox* box)
{
	if (!box)
	{
		return 0;
	}
	if (box->trigger)
	{
		return 2;
	}
	if (box->team)
	{
		return 0;
	}
	return 1;
}

void vbl::Ball::moveWithCollision(const Geometry& geometry, const std::vector<std::shared_ptr<vbl::Sprite>> actors, bool simulated)
{
	float steps = abs(vel.x) + abs(vel.y);
	float increment = steps;
	while (increment > 0)
	{
		this->move({ this->vel.x / steps, 0 });
		std::vector<uint32_t> res = geometry.collidesWithRes(this->box);
		for (const auto result : res)
		{
			const GeometryBox* box = geometry.get(result);
			uint8_t collisionType = this->collidesWithGeometryBox(box);
			if (!collisionType)
			{
				continue;
			}
			if (collisionType == 1)
			{
				bounced = true;
				this->move({ -this->vel.x / steps, 0 });
				this->vel.x *= -0.9f;
				if (!simulated)
				{
					this->collisionParticle(8);
				}
				continue;
			}
			this->trigger(box);
		}
		this->move({ 0, this->vel.y / steps });
		res = geometry.collidesWithRes(this->box);
		for (const auto result : res)
		{
			const GeometryBox* box = geometry.get(result);
			uint8_t collisionType = this->collidesWithGeometryBox(box);
			if (!collisionType)
			{
				continue;
			}
			if (collisionType == 1)
			{
				bounced = true;
				this->move({ 0, -this->vel.y / steps });
				this->vel.y *= -0.9f;
				if (!simulated)
				{
					this->collisionParticle(8);
				}
				continue;
			}
			this->trigger(box);
		}
		increment--;
	}
}

void vbl::Ball::update(const Geometry& geometry, const std::vector<std::shared_ptr<vbl::Sprite>> actors, uint32_t tick, bool simulated)
{
	if (this->spawning > 0)
	{
		this->spawning--;
		this->glow = int((float)spawning / (float)spawnTime * 255);
		return;
	}
	this->wasTriggered = false;
	this->triggeredTeam = 0;
	this->vel.y += this->gravity;
	moveWithCollision(geometry, actors, simulated);
	const std::shared_ptr<vbl::Sprite> res = collidesWithActor(actors);
	if (res)
	{
		this->bounceOff(res, simulated);
	}
	this->wasInside = res;
	if (this->lastBoink > 0)
	{
		this->lastBoink -= !simulated;
	}
	if (!(tick % 15) && !simulated)
	{
		collisionParticle(1);
	}
}

void vbl::Ball::reset(uint32_t spawnTime)
{
	this->vel = { 0,0 };
	this->wasInside.reset();
	this->texture.rotate(-this->texture.getRotation());
	this->spawnTime = spawnTime;
	this->spawning = spawnTime;
	printf("spawn time is %u\n", this->spawnTime);
}

const std::vector<maf::ivec2>& vbl::Ball::trace(const Geometry& geometry, const std::vector<std::shared_ptr<vbl::Sprite>>& actors, uint32_t length, float res, int bounceLimit)
{
	tracePoints.clear();
	uint32_t lastSpawning = this->spawning;
	maf::fvec2 startingPos = this->pos;
	maf::fvec2 startingVel = this->vel;
	float rot = this->texture.getRotation();
	int bounces = 0;
	while (length > 0 && bounces < bounceLimit)
	{
		update(geometry, actors, 0, true);
		if (bounced)
		{
			bounces++;
		}
		bounced = false;
		tracePoints.push_back(this->texture.getMiddle());
		if (triggered())
		{
			this->wasTriggered = false;
			this->triggeredTeam = 0;
			break;
		}
		length--;
	}
	this->texture.rotate(rot - this->texture.getRotation());
	this->setPos(startingPos);
	this->setVel(startingVel);
	this->spawning = lastSpawning;
	return this->tracePoints;
}

void vbl::Ball::collisionParticle(int count)
{
	for (int i = 0; i < count; i++)
	{
		this->spawnParticle(
			120,
			this->getVisMid(),
			{ 0,0 },
			this->getParticle("cld"),
			{ 0,0,24,24 },
			0,
			0,
			10);
	}
}
