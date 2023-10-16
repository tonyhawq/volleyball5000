#pragma once

#include <unordered_map>
#include <bitset>
#include <functional>

#include "vbl.h"
#include "Sprite.h"
#include "Geometry.h"

namespace vbl
{
	class Controller
	{
	public:
		Controller(const std::string& name, uint16_t team);

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

		maf::ivec2 getInput() const;
		inline const std::string& getName() const { return this->name; }
		inline uint16_t getTeam() const { return this->team; }

		inline const std::unordered_map<int, InputIDX>& getMap() const { return this->keymap; }
		void setKey(int key, InputIDX input);
		void unsetKey(int key);

		inline bool ready() const { return this->hadInput; }
	private:
		bool hadInput = false;

		uint16_t team;
		std::string name;
		maf::ivec2 input;
		std::unordered_map<int, InputIDX> keymap;
		std::bitset<8> inputs;
		uint32_t dashQueuedown;
		uint32_t jumpQueuedown;
	};

	class Guy : public Sprite
	{
	public:
		Guy(const std::string& name, float diameter, SDL_Texture* texture, Controller* controller = NULL);

		void link(Controller* controller);

		bool collidesWithGeometryBox(const GeometryBox* box);
		void moveWithCollision(const Geometry& geometry);
		void update(const Geometry& geometry);

		void jump();
		inline bool canJump() const { return (jumps < maxJumps); };
		void dash();
		inline bool canDash() const { return (dashes < maxDashes); };
		inline void setGravity(float grav) { this->gravity = grav; }
		
		void touchedGround(maf::fvec2 pos);
		void reset();

		static MAABB makeCircle(float diameter);
		inline const Controller* getController() const { return this->controller; }
	private:
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
