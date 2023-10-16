#include "Guy.h"

vbl::Controller::Controller(const std::string& name, uint16_t team)
	:name(name), dashQueuedown(0), jumpQueuedown(0), team(team)
{
	
}

void vbl::Controller::update()
{
	if (jumpQueuedown > 0)
	{
		jumpQueuedown--;
	}
	if (dashQueuedown > 0)
	{
		dashQueuedown--;
	}
}

bool vbl::Controller::keyDown(int key)
{
	InputIDX in = keymap[key];
	if (in == InputIDX::INPUT_NONE)
	{
		return false;
	}
	this->hadInput = true;
	this->inputs[(int)in] = true;
	switch (in)
	{
	case InputIDX::INPUT_UP:
		queueJump();
		break;
	case InputIDX::INPUT_DASH:
		queueDash();
		break;
	}
	return true;
}

bool vbl::Controller::keyUp(int key)
{
	InputIDX in = keymap[key];
	if (in == InputIDX::INPUT_NONE)
	{
		return false;
	}
	this->inputs[(int)in] = false;
	return true;
}

maf::ivec2 vbl::Controller::getInput() const
{
	return { inputs[(int)InputIDX::INPUT_RIGHT] - inputs[(int)InputIDX::INPUT_LEFT], inputs[(int)InputIDX::INPUT_UP] - inputs[(int)InputIDX::INPUT_DOWN] };
}

void vbl::Controller::setKey(int key, InputIDX input)
{
	this->keymap[key] = input;
}

void vbl::Controller::unsetKey(int key)
{
	if (!this->keymap.count(key))
	{
		return;
	}
	this->keymap.erase(key);
}

/*
. . # # # # . .
. # # # # # # .
# # # # # # # #
# # # # # # # #
# # # # # # # #
# # # # # # # #
. # # # # # # .
. . # # # # . .
*/

vbl::MAABB vbl::Guy::makeCircle(float diameter)
{
	return MAABB({
		{ 1.0f / 8.0f * diameter, 1.0f / 8.0f * diameter, 6.0f / 8.0f * diameter, 6.0f / 8.0f * diameter },
		{ 2.0f / 8.0f * diameter, 0.0f / 8.0f * diameter, 4.0f / 8.0f * diameter, 8.0f / 8.0f * diameter },
		{ 0.0f / 8.0f * diameter, 2.0f / 8.0f * diameter, 8.0f / 8.0f * diameter, 4.0f / 8.0f * diameter },
		});
}

vbl::Guy::Guy(const std::string& name, float diameter, SDL_Texture* texture, Controller* controller)
	:Sprite({ diameter, diameter }, texture)
{
	this->name = name;
	if (controller)
	{
		this->link(controller);
	}
	//this->box = Guy::makeCircle(diameter);
	this->box = MAABB({ {0,0,diameter, diameter} });
}

void vbl::Guy::link(Controller* controller)
{
	this->controller = controller;
}

void vbl::Guy::jump()
{
	if (!canJump())
	{
		return;
	}
	this->vel.y = float(- int(this->jumpPower));
	this->dashing = 0;
	this->vel.x *= 1.2f;
	this->jumps++;
}

void vbl::Guy::dash()
{
	if (!canDash())
	{
		return;
	}
	this->dashing = this->dashDuration;
	if (this->controller)
	{
		this->dashDirection = this->controller->getInput();
	}
	if (!this->dashDirection.x && !this->dashDirection.y)
	{
		this->dashDirection = maf::makeBinary({ (int)ceil(this->vel.x), (int)ceil(this->vel.y) });
	}
	this->dashDirection.y = 0;
	this->dashes++;
}

void vbl::Guy::touchedGround(maf::fvec2 pos)
{
	if (!this->controller)
	{
		return;
	}
}

void vbl::Guy::reset()
{
	this->vel = { 0,0 };
	this->jumps = this->maxJumps;
	this->dashes = this->maxDashes;
}

bool vbl::Guy::collidesWithGeometryBox(const GeometryBox* box)
{
	if (!box)
	{
		return false;
	}
	if (box->trigger)
	{
		return false;
	}
	if (!box->team)
	{
		return true;
	}
	if (!this->controller)
	{
		return false;
	}
	if (this->controller->getTeam() == box->team)
	{
		return true;
	}
	return false;
}

void vbl::Guy::moveWithCollision(const Geometry& geometry)
{
	onGround = false;
	float steps = abs(this->vel.x) + abs(this->vel.y);
	float increment = steps;
	while (increment > 0)
	{
		this->move({ this->vel.x / steps, 0 });
		std::vector<uint32_t> xres = geometry.collidesWithRes(this->box);
		for (const auto res : xres)
		{
			if (collidesWithGeometryBox(geometry.get(res)))
			{
				this->move({ -this->vel.x / steps, 0 });
				onGround = true;
				this->vel.x = 0;
				touchedGround(this->pos);
				break;
			}
		}
		this->move({ 0, this->vel.y / steps });
		std::vector<uint32_t> yres = geometry.collidesWithRes(this->box);
		for (const auto res : yres)
		{
			if (collidesWithGeometryBox(geometry.get(res)))
			{
				this->move({ 0, -this->vel.y / steps });
				onGround = true;
				this->vel.y = 0;
				this->jumps = 0;
				touchedGround(this->pos);
				break;
			}
		}
		increment--;
	}
}

void vbl::Guy::update(const Geometry& geometry)
{
	this->vel.y += this->gravity;
	if (this->vel.y > this->terminalVelocity)
	{
		this->vel.y = this->terminalVelocity;
	}
	if (this->controller)
	{
		if (this->controller->queuedJump())
		{
			this->jump();
			this->controller->jumped();
		}
		if (this->controller->queuedDash())
		{
			this->dash();
			this->controller->dashed();
		}
	}
	if (this->dashing)
	{
		this->vel.x = this->dashDirection.x * this->dashPower;
		this->vel.y = this->dashDirection.y * this->dashPower;
		this->dashing--;
	}
	else
	{
		if (this->controller)
		{
			this->vel.x += this->controller->getInput().x * this->runspeed;
		}
	}
	this->moveWithCollision(geometry);
	if (onGround)
	{
		this->vel.x *= 1 - this->groundFriction;
		this->dashCurrent++;
		if (this->dashCurrent > this->dashCooldown && this->dashes > 0)
		{
			this->dashes--;
			this->dashCurrent = 0;
		}
	}
	else
	{
		this->vel.x *= 1 - this->airFriction;
	}
	if (abs(this->vel.x) < 0.5)
	{
		this->vel.x = 0;
	}
}
