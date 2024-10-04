#include "SimpleAi.h"

#include "../Game.h"

vbl::SimpleAI::SimpleAI(vbl::Controller* linked)
	:AI(linked)
{}

void vbl::SimpleAI::input(Game* game)
{
	if (game->map.balls.size() == 0)
	{
		return;
	}
	this->children.clear();
	for (int i = 0; i < game->map.guys.size(); i++)
	{
		if (game->map.guys[i]->getController() == this->linked)
		{
			this->children.push_back(game->map.guys[i]);
		}
	}
	// assume one guy ( for now )
	if (this->children.size() == 0)
	{
		return;
	}
	std::shared_ptr<Guy> baby = this->children[0];
	std::shared_ptr<Ball> target;
	maf::fvec2 targetPos{999999,999999};
	maf::fvec2 targetVel{0,0};
	for (int i = 0; i < game->map.balls.size(); i++)
	{
		std::shared_ptr<Ball> prospect = game->map.balls[i];
		maf::fvec2 pos = prospect->getPos();
		maf::fvec2 vel = prospect->getVel();
		if (pos.y * vel.y < targetPos.y * targetVel.y)
		{
			target = prospect;
		}
	}
	float predictedX = targetPos.x;
}

void vbl::SimpleAI::output()
{
	// why is this a thing
}
