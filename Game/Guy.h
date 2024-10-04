#pragma once

#include <unordered_map>
#include <bitset>
#include <functional>

#include "vbl.h"
#include "Ball.h"
#include "GameSprite.h"
#include "Geometry.h"
#include "Particles.h"
#include "Gun.h"

namespace vbl
{
	class AI;
	class Controller
	{
	public:
		Controller(const std::string& name, uint16_t team);
		~Controller();

		inline void reset() { this->hadInput = false; }
		void update();

		bool keyDown(int key);
		bool keyUp(int key);

		enum class InputIDX
		{
			INPUT_NONE  = 0,
			INPUT_LEFT  = 1,
			INPUT_RIGHT = 2,
			INPUT_UP    = 3,
			INPUT_DOWN  = 4,
			INPUT_DASH  = 5,
		};

		inline void queueJump() { jumpQueuedown = 10; }
		inline void queueDash() { dashQueuedown = 10; }
		inline bool queuedDash() { return dashQueuedown > 0; }
		inline bool queuedJump() { return jumpQueuedown > 0; }
		inline void jumped() { jumpQueuedown = 0; }
		inline void dashed() { dashQueuedown = 0; }
		
		void widow();
		void marry(AI* wife);

		inline const AI* wife() const { return this->controlledBy; }
		inline AI* wife() { return this->controlledBy; }

		maf::ivec2 getInput() const;
		inline const std::string& getName() const { return this->name; }
		inline uint16_t getTeam() const { return this->team; }

		inline const std::unordered_map<int, InputIDX>& getMap() const { return this->keymap; }
		void setKey(int key, InputIDX input);
		void unsetKey(int key);

		inline bool ready() const { return this->hadInput; }
	private:
		AI* controlledBy;
		bool hadInput = false;

		uint16_t team;
		std::string name;
		maf::ivec2 input;
		std::unordered_map<int, InputIDX> keymap;
		std::bitset<8> inputs;
		uint32_t dashQueuedown;
		uint32_t jumpQueuedown;
	};

	class Guy : public GameSprite
	{
	public:
		struct Powerup
		{
			Ball::PowerupType type;
			float remaining;
		};

		Guy(const std::string& name, float diameter, const std::string& picture, Controller* controller = NULL);

		void link(Controller* controller);

		bool collidesWithGeometryBox(const GeometryBox* box);
		void moveWithCollision(const Geometry& geometry);
		void update(const Geometry& geometry, uint16_t tick);
		void updatePowerups();
		void clearPowerups();

		void applyPower(Ball::PowerupType type);
		void removePower(Ball::PowerupType type);

		void jump();
		inline bool canJump() const { return (jumps < maxJumps); };
		void dash();
		inline bool canDash() const { return (dashes < maxDashes); };
		inline void setGravity(float grav) { this->gravity = grav; }

		inline const std::vector<Powerup>& getPowers() const { return this->powers; }
		
		void touchedGround(maf::fvec2 pos);
		void reset();

		void powerup(Ball::PowerupType powerup, float duration);

		static MAABB makeCircle(float diameter);
		inline const Controller* getController() const { return this->controller; }
		inline Controller* changeController() { return this->controller; }

		inline Gun* gun() { return this->firearm; }
		inline bool hasGun() const { return (!!firearm); }
	private:
		Gun* firearm;

		std::vector<Powerup> powers;

		Controller* controller = NULL;

		std::string name;
		float terminalVelocity = 20.0f;
		float gravity = 0.0f;
		uint32_t jumpPower = 10;
		uint32_t jumps = 0;
		uint32_t maxJumps = 2;
		uint32_t dashes = 0;
		uint32_t maxDashes = 2;
		uint32_t dashing = 0;
		uint32_t dashDuration = 4;
		uint32_t dashCooldown = 60;
		uint32_t dashCurrent = 0;
		float runspeed = 0.6f;
		float dashPower = 30.0f;
		float airFriction = 0.04f;
		float groundFriction = 0.06f;
		bool onGround = false;
		maf::ivec2 dashDirection;
	};
}

