#include "Ball.h"

#include "Guy.h"

vbl::Ball::Ball(const IDedPicture& picture, const IDedPicture& glowPicture, float diameter)
	:GameSprite({diameter, diameter}, picture, true), wasTriggered(false), triggeredTeam(0), glowTexture(glowPicture, SDL_Rect{0, 0, (int)diameter, (int)diameter}, 0)
{

}

vbl::Ball::Ball(const IDedPicture& picture, const IDedPicture& glowPicture, float diameter, PowerupType powerup)
	:GameSprite({diameter, diameter}, picture, true), wasTriggered(false), triggeredTeam(0), powerup(powerup), glowTexture(glowPicture, SDL_Rect{ 0, 0, (int)diameter, (int)diameter }, 0)
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
	collisionParticle(int(std::max(speedDiff, 4.0f)));
	// bounded lest it go on ad infinitum...
	int loops = 25;
	maf::fvec2 antithesis = { -std::sin(guyDir), std::cos(guyDir) };
	bool hitGeo = false;
	uint16_t team = 0;
	if (sprite->type() == AType::Guy)
	{
		Guy* casted = reinterpret_cast<Guy*>(sprite.get());
		if (casted->getController())
		{
			team = casted->getController()->getTeam();
		}
	}
	while (loops > 0 && this->box.collide(sprite->getBox()))
	{
		hitGeo = false;
		loops--;
		sprite->move({ antithesis.x, 0 });
		if (geometry.collidesNotriggerTeamed(sprite->getBox(), team))
		{
			antithesis.x = -antithesis.x;
			sprite->move({ antithesis.x, 0 });
			hitGeo = true;
		}
		sprite->move({ 0, antithesis.y });
		if (geometry.collidesNotriggerTeamed(sprite->getBox(), team))
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

void vbl::Ball::moveWithCollision(const Geometry& geometry, const std::vector<std::shared_ptr<vbl::Sprite>> actors, float resolution, bool simulated)
{
	float steps = (abs(vel.x) + abs(vel.y)) / resolution;
	float increment = steps;
	while (increment > 0)
	{
		if (increment < 1)
		{
			steps = steps / increment;
		}
		this->move({ this->vel.x / steps, 0 });
		geometry.collidesWithResNoalloc(this->box, this->cached_res);
		for (const auto result : cached_res)
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
		geometry.collidesWithResNoalloc(this->box, this->cached_res);
		for (const auto result : cached_res)
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

void vbl::Ball::update(const Geometry& geometry, const std::vector<std::shared_ptr<vbl::Sprite>>& actors, uint32_t tick, float resolution, bool simulated)
{
	if (this->spawning > 0 && !simulated)
	{
		this->spawning--;
		this->glow = int((float)spawning / (float)spawnTime * 255);
		return;
	}
	this->wasTriggered = false;
	this->triggeredTeam = 0;
	this->vel.y += this->gravity;
	moveWithCollision(geometry, actors, resolution, simulated);
	const std::shared_ptr<vbl::Sprite> res = collidesWithActor(actors);
	if (res)
	{
		this->bounceOff(geometry, res, simulated);
	}
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
	this->texture.rotate(-this->texture.getRotation());
	this->spawnTime = spawnTime;
	this->spawning = spawnTime;
}

const vbl::Ball::Trace& vbl::Ball::trace(const Geometry& geometry, const std::vector<std::shared_ptr<vbl::Sprite>>& actors, uint32_t length, float resolution, int bounceLimit)
{
	if (tracer.points.size() < length)
	{
		tracer.points.resize(length);
	}
	uint32_t lastSpawning = this->spawning;
	maf::fvec2 startingPos = this->pos;
	maf::fvec2 startingVel = this->vel;
	float rot = this->texture.getRotation();
	int bounces = 0;
	int i = 0;
	while (length > 0 /* && bounces < bounceLimit */)
	{
		update(geometry, actors, 0, resolution, true);
		if (bounced)
		{
			bounces++;
		}
		bounced = false;
		tracer.points[i] = this->texture.getMiddle();
		if (triggered())
		{
			this->wasTriggered = false;
			this->triggeredTeam = 0;
			//break;
		}
		length--;
		i++;
	}
	this->texture.rotate(rot - this->texture.getRotation());
	this->setPos(startingPos);
	this->setVel(startingVel);
	this->spawning = lastSpawning;
	this->tracer.length = i;
	return this->tracer;
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
