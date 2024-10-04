#include "AI.h"

vbl::AI::AI(vbl::Controller* linkTo)
	:linked(linkTo), stateA(0), stateB(0)
{

}

vbl::AI::~AI()
{
	if (this->linked)
	{
		this->linked->widow();
	}
}

void vbl::AI::widow()
{
	if (this->linked)
	{
		this->linked->marry(NULL);
	}
	this->linked = NULL;
}

void vbl::AI::marry(vbl::Controller* wife)
{
	if (!wife)
	{
		this->linked = NULL;
		return;
	}
	this->widow();
	this->linked = wife;
}
