#pragma once

#include "../AI.h"

namespace vbl
{
	class SimpleAI : public AI
	{
	public:
		SimpleAI(Controller* linked);

		void input(Game*);
		void output();

		std::vector<std::shared_ptr<Guy>> children;

		// helper state functions

	};
}
