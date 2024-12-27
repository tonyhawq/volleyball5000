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
	std::shared_ptr<Ball> target = NULL;
	maf::fvec2 targetPos{999999,999999};
	maf::fvec2 targetVel{0,0};
	for (int i = 0; i < game->map.balls.size(); i++)
	{
		std::shared_ptr<Ball> prospect = game->map.balls[i];
		maf::fvec2 pos = prospect->getPos();
		maf::fvec2 vel = prospect->getVel();
		target = prospect;
	}
	if (!target)
	{
		return;
	}
	maf::fvec2 landed_pos = {};
	target->trace(game->map.getGeometry(), game->map.actors, 500, 1, 5, &landed_pos);
	// assume team sides always will have a team sided collider on it
	bool isOnOtherTeamSide = false;
	std::vector<vbl::GeometryBox> results = game->map.getGeometry().collidesWithBoxes(MAABB(landed_pos));
	for (const auto& result : results)
	{
		if (result.team == this->linked->getTeam())
		{
			isOnOtherTeamSide = true;
			break;
		}
	}
	if (isOnOtherTeamSide)
	{
		this->linked->setInput(vbl::Controller::InputIDX::INPUT_ALL, false);
		return;
	}
	maf::fvec2 pos = baby->getPos();
	bool do_left = false;
	bool do_right = false;
	bool do_up = false;
	bool do_down = false;
	if (pos.x < landed_pos.x)
	{
		do_right = true;
	}
	else
	{
		do_left = true;
	}
	this->linked->setInput(vbl::Controller::InputIDX::INPUT_LEFT, do_left);
	this->linked->setInput(vbl::Controller::InputIDX::INPUT_RIGHT, do_right);
	this->linked->setInput(vbl::Controller::InputIDX::INPUT_UP, do_up);
	this->linked->setInput(vbl::Controller::InputIDX::INPUT_DOWN, do_down);
}

void vbl::SimpleAI::output()
{
	// why is this a thing
}
