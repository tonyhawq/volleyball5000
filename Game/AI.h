#pragma once

#include <stdint.h>
#include "Guy.h"

namespace vbl
{
	class Game;
	class AI
	{
	public:
		AI(vbl::Controller* linkTo);
		~AI();

		void widow();
		void marry(vbl::Controller* wife);

		virtual void input(Game*) = 0;
		virtual void output() = 0;
		vbl::Controller* linked;
		uint64_t stateA;
		uint64_t stateB;
	};
}
