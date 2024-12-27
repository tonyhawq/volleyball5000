#include "Ball.h"

vbl::Ball::Ball(const std::string& picture, const std::string& glowPicture, float diameter)
	:GameSprite({diameter, diameter}, picture, true), wasTriggered(false), triggeredTeam(0), glowTexture(glowPicture)
{

}

vbl::Ball::Ball(const std::string& picture, const std::string& glowPicture, float diameter, PowerupType powerup)
	:GameSprite({diameter, diameter}, picture, true), wasTriggered(false), triggeredTeam(0), powerup(powerup), glowTexture(glowPicture)
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

void vbl::Ball::bounceOff(const Geometry& geometry, const std::shared_ptr<vbl::Sprite> sprite, bool simulated)
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
	maf::fvec2 prevVel = this->vel;
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
			*this->getParticle("bnk"),
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
	float imparted = (abs(vel.x - prevVel.x) + abs(vel.y - prevVel.y)) / 2;
	float guyDir = (float)maf::pointTowards(sprite->getBox().getMiddle(), this->getBox().getMiddle());
	sprite->setVel(sprite->getVel() + maf::fvec2{ maf::clamp(-imparted * std::sin(guyDir), -5.0f, 5.0f), maf::clamp(imparted * std::cos(guyDir), -5.0f, 5.0f) });
	collisionParticle(std::max(speedDiff, 4.0f));
	// bounded lest it go on ad infinitum...
	int loops = 25;
	maf::fvec2 antithesis = { -std::sin(guyDir), std::cos(guyDir) };
	bool hitGeo = false;
	while (loops > 0 && this->box.collide(sprite->getBox()))
	{
		hitGeo = false;
		loops--;
		sprite->move({ antithesis.x, 0 });
		if (geometry.collidesNotrigger(sprite->getBox()))
		{
			antithesis.x = -antithesis.x;
			sprite->move({ antithesis.x, 0 });
			hitGeo = true;
		}
		sprite->move({ 0, antithesis.y });
		if (geometry.collidesNotrigger(sprite->getBox()))
		{
			antithesis.y = -antithesis.y;
			sprite->move({ 0, antithesis.y });
			if (hitGeo)
			{
				break;
			}
		}
	}
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
		std::vector<uint32_t> res = geometry.collidesWithIndicies(this->box);
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
		res = geometry.collidesWithIndicies(this->box);
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
		this->bounceOff(geometry, res, simulated);
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
	this->wasTriggered = false;
	this->triggeredTeam = 0;
}

const std::vector<maf::ivec2>& vbl::Ball::trace(const Geometry& geometry, const std::vector<std::shared_ptr<vbl::Sprite>>& actors, uint32_t length, float res, int bounceLimit, maf::fvec2* ended_pos)
{
	tracePoints.clear();
	uint32_t lastSpawning = this->spawning;
	maf::fvec2 startingPos = this->pos;
	maf::fvec2 startingVel = this->vel;
	float rot = this->texture.getRotation();
	int bounces = 0;
	bool prevTriggerState = this->wasTriggered;
	uint16_t prevTriggerTeam = this->triggeredTeam;
	while (length > 0 && bounces <= bounceLimit)
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
	if (ended_pos)
	{
		*ended_pos = this->getPos();
	}
	this->texture.rotate(rot - this->texture.getRotation());
	this->setPos(startingPos);
	this->setVel(startingVel);
	this->spawning = lastSpawning;
	this->wasTriggered = prevTriggerState;
	this->triggeredTeam = prevTriggerTeam;
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
			*this->getParticle("cld"),
			{ 0,0,24,24 },
			0,
			0,
			10);
	}
}
