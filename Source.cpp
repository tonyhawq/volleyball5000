#include "Game/Game.h"

int main(int args, char* argc[])
{
	SDL_Init(SDL_INIT_VIDEO);
	vbl::Game game = vbl::Game(1920, 1080, 0.25);
	game.makeController("Player 1", 1);
	game.makeGuy("guy-1", "guy.png");
	game.linkController("Player 1", "guy-1");
	vbl::Controller* ctrl = game.getController("Player 1");
	ctrl->setKey((int)SDLK_w, vbl::Controller::InputIDX::INPUT_UP);
	ctrl->setKey((int)SDLK_a, vbl::Controller::InputIDX::INPUT_LEFT);
	ctrl->setKey((int)SDLK_s, vbl::Controller::InputIDX::INPUT_DOWN);
	ctrl->setKey((int)SDLK_d, vbl::Controller::InputIDX::INPUT_RIGHT);
	ctrl->setKey((int)SDLK_LSHIFT, vbl::Controller::InputIDX::INPUT_DASH);

	game.makeController("Player 2", 2);
	game.makeGuy("guy-2", "guy2.png");
	game.linkController("Player 2", "guy-2");
	ctrl = game.getController("Player 2");
	ctrl->setKey((int)SDLK_UP, vbl::Controller::InputIDX::INPUT_UP);
	ctrl->setKey((int)SDLK_LEFT, vbl::Controller::InputIDX::INPUT_LEFT);
	ctrl->setKey((int)SDLK_DOWN, vbl::Controller::InputIDX::INPUT_DOWN);
	ctrl->setKey((int)SDLK_RIGHT, vbl::Controller::InputIDX::INPUT_RIGHT);
	ctrl->setKey((int)SDLK_RSHIFT, vbl::Controller::InputIDX::INPUT_DASH);

	game.unlink("guy-1");
	game.linkController("Player 1", "guy-1");

	float netWidth = 20;

	vbl::Geometry geometry = vbl::Geometry({
		{ {  0,    790, 960,  1000 }, 2, true },
		{ {  960,  790, 960,  1000 }, 1, true },
		{ { -2000, 800, 4000, 1000 }, 0, false },
		{ { -10,   -500,   10,   2000 }, 0, false },
		{ {  1920, -500,   10,   2000 }, 0, false },
		{ {  1920 / 2 - netWidth / 2,  400, netWidth,  1000 }, 0, false },
		{ {  0,    0,   1920 / 2 - netWidth / 2 + netWidth, 1000 }, 2, false },
		{ {  1920 / 2 - netWidth / 2,  0,   1920 / 2 - netWidth / 2 + netWidth, 1000 }, 1, false },
		}, {
		{ {-10, 800, 2000, 2000}, {50, 168, 82, 255} },
		{ {1920 / 2 - netWidth / 2, 400, netWidth, 1000}, {107, 80, 35, 255} },
		});
	game.map.setGeometry(geometry);
	game.map.gravity = 0.25;
	game.map.setSpawnPoint(1, { 20,700 });
	game.map.setSpawnPoint(2, { 1820, 700 });
	game.respawnGuys();
	game.makeBall({ 920, 100 }, "ball.png");
	game.map.setBallSpawnVel({ 10, -5 });
	game.map.resetMap();
	game.run();
	return 0;
}