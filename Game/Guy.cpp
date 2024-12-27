#include "Guy.h"
#include "random.h"
#include "AI.h"

vbl::Controller::Controller(const std::string& name, uint16_t team)
	:name(name), dashQueuedown(0), jumpQueuedown(0), team(team), controlledBy(NULL)
{
	
}

vbl::Controller::~Controller()
{
	if (this->controlledBy)
	{
		this->controlledBy->widow();
	}
}

void vbl::Controller::widow()
{
	if (this->controlledBy)
	{
		this->controlledBy->widow();
	}
	this->controlledBy = NULL;
}

void vbl::Controller::marry(AI* wife)
{
	if (!wife)
	{
		this->controlledBy = NULL;
		return;
	}
	this->widow();
	this->controlledBy = wife;
}

void vbl::Controller::update(Game* game)
{
	if (jumpQueuedown > 0)
	{
		jumpQueuedown--;
	}
	if (dashQueuedown > 0)
	{
		dashQueuedown--;
	}
	if (this->controlledBy)
	{
		this->controlledBy->input(game);
	}
}

bool vbl::Controller::keyDown(int key)
{
	InputIDX in = keymap[key];
	return this->setInput(in, true);
}

bool vbl::Controller::setInput(vbl::Controller::InputIDX input, bool value)
{
	if (input == InputIDX::INPUT_NONE)
	{
		return false;
	}
	if (input == InputIDX::INPUT_ALL)
	{
		if (value)
		{
			this->inputs.set();
		}
		else
		{
			this->inputs.reset();
		}
		return true;
	}
	this->hadInput = value;
	this->inputs[(int)input] = value;
	if (!value)
	{
		return true;
	}
	switch (input)
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

vbl::Guy::Guy(const std::string& name, float diameter, const std::string& picture, Controller* controller)
	: GameSprite({ diameter, diameter }, picture)
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
	if (!this->onGround)
	{
		for (int i = 0; i < 20; i++)
		{
			this->spawnParticle(
				120,
				{ (float)this->texture.getMiddle().x, (float)this->texture.getMiddle().y },
				{ 0,0 },
				*this->getParticle("air"),
				{ 0,0,24,24 },
				0,
				0,
				10
			);
		}
	}
	if (this->vel.y - float(this->jumpPower) / 2 > -float(this->jumpPower))
	{
		this->vel.y = -float(this->jumpPower);
	}
	else
	{
		this->vel.y -= float(this->jumpPower) / 2;
	}
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
		std::vector<uint32_t> xres = geometry.collidesWithIndicies(this->box);
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
		std::vector<uint32_t> yres = geometry.collidesWithIndicies(this->box);
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

void vbl::Guy::update(const Geometry& geometry, uint16_t tick)
{
	updatePowerups();
	maf::ivec2 input = { 0,0 };
	if (this->controller)
	{
		input = this->controller->getInput();
	}
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
		if (abs(this->vel.x) > 0.5f)
		{
			if (!(tick % (11 - (int)maf::clamp(abs(this->vel.x), 10.0f, 0.0f))))
			{
				this->spawnParticle(
					120,
					{ (float)this->getTexture().feet().x, (float)this->getTexture().feet().y },
					{ 0,0 },
					*this->getParticle("grd"),
					{ 0,0,16,16 },
					0,
					0,
					abs(this->vel.x)
				);
			}
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
	if (this->jumps > 1 && (!(tick % 8)))
	{
		this->spawnParticle(
			120,
			{ (float)this->texture.getMiddle().x, (float)this->texture.getMiddle().y },
			{ 0,0 },
			*this->getParticle("air"),
			{ 0,0,24,24 },
			0,
			0,
			10
		);
	}
	if (this->controller)
	{
		int animState = 0;
		if (abs(input.x))
		{
			animState++;
			if (input.x > 0)
			{
				animState++;
			}
		}
		if (input.y > 0)
		{
			animState = 3;
		}
		this->texture.setAnimState(animState);
	}
}

void vbl::Guy::updatePowerups()
{
	for (int i = 0; i < powers.size(); i++)
	{
		Powerup& power = powers[i];
		power.remaining--;
		if (power.remaining < 0)
		{
			removePower(power.type);
			powers.erase(powers.begin() + i);
			i--;
		}
	}
}
void vbl::Guy::clearPowerups()
{
	while (powers.size())
	{
		this->removePower(powers[powers.size() - 1].type);
		powers.pop_back();
	}
}

void vbl::Guy::applyPower(Ball::PowerupType type)
{
	switch (type)
	{
	case Ball::POWERUP_TRIPLEJUMP:
		this->maxJumps++;
		break;
	default:
		break;
	}
}

void vbl::Guy::removePower(Ball::PowerupType type)
{
	switch (type)
	{
	case Ball::POWERUP_TRIPLEJUMP:
		this->maxJumps--;
		break;
	default:
		break;
	}
}

void vbl::Guy::powerup(Ball::PowerupType powerup, float duration)
{
	this->powers.push_back({ powerup, duration });
	applyPower(powerup);
}


