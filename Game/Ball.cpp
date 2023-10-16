#include "Ball.h"

vbl::Ball::Ball(SDL_Texture* texture, float diameter)
	:Sprite({diameter, diameter}, texture, true), wasTriggered(false), triggeredTeam(0)
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

void vbl::Ball::bounceOff(const Sprite* sprite)
{
	if (!sprite)
	{
		return;
	}
	if (sprite == this->wasInside)
	{
		return;
	}
	maf::fvec2 otherPos = sprite->getBox().getMiddle();
	maf::fvec2 otherVel = sprite->getVel();
	float dir = (float)maf::pointTowards(this->box.getMiddle(), otherPos);
	float sin = std::sin(dir);
	float cos = std::cos(dir);
	this->vel.x = -sin * 5 + otherVel.x;
	this->vel.y = cos * 5 + otherVel.y;
	this->texture.rotate((float)maf::radToDegrees(dir) - this->texture.getRotation());
}

const vbl::Sprite* vbl::Ball::collidesWithActor(const std::vector<Sprite*>& actors)
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

void vbl::Ball::moveWithCollision(const Geometry& geometry, const std::vector<Sprite*> actors)
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
				this->move({ -this->vel.x / steps, 0 });
				this->vel.x *= -0.9f;
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
				this->move({ 0, -this->vel.y / steps });
				this->vel.y *= -0.9f;
				continue;
			}
			this->trigger(box);
		}
		increment--;
	}
}

void vbl::Ball::update(const Geometry& geometry, const std::vector<Sprite*> actors)
{
	this->wasTriggered = false;
	this->triggeredTeam = 0;
	this->vel.y += this->gravity;
	moveWithCollision(geometry, actors);
	const Sprite* res = collidesWithActor(actors);
	if (res)
	{
		this->bounceOff(res);
	}
	this->wasInside = res;
}

void vbl::Ball::reset()
{
	this->vel = { 0,0 };
	this->wasInside = NULL;
	this->texture.rotate(-this->texture.getRotation());
}

const std::vector<maf::ivec2>& vbl::Ball::trace(const Geometry& geometry, const std::vector<Sprite*>& actors, uint32_t length, float res)
{
	tracePoints.clear();
	maf::fvec2 startingPos = this->pos;
	maf::fvec2 startingVel = this->vel;
	while (length > 0)
	{
		update(geometry, actors);
		tracePoints.push_back(this->texture.getMiddle());
		if (triggered())
		{
			this->wasTriggered = false;
			this->triggeredTeam = 0;
			break;
		}
		length--;
	}
	this->setPos(startingPos);
	this->setVel(startingVel);
	return this->tracePoints;
}
