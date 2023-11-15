#include "Game/Game.h"

int main(int args, char* argc[])
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	vbl::Game game = vbl::Game(1920, 1080, 0.25);
	game.sound.loadSound({ "content/sound/boink1.wav", "content/sound/boink2.wav", "content/sound/boink3.wav" }, "boink");
	game.sound.loadSound({ "content/sound/crunch1.wav", "content/sound/crunch2.wav", "content/sound/crunch3.wav" }, "crunch");

	game.map.load("content/maps/1.vbl5");

	game.makeController("Player 1", 1);
	game.makeGuy("guy-1", "content/graphics/guy_sheet.png", {20,20});
	game.linkController("Player 1", "guy-1");
	vbl::Controller* ctrl = game.getController("Player 1");
	ctrl->setKey((int)SDLK_w, vbl::Controller::InputIDX::INPUT_UP);
	ctrl->setKey((int)SDLK_a, vbl::Controller::InputIDX::INPUT_LEFT);
	ctrl->setKey((int)SDLK_s, vbl::Controller::InputIDX::INPUT_DOWN);
	ctrl->setKey((int)SDLK_d, vbl::Controller::InputIDX::INPUT_RIGHT);
	ctrl->setKey((int)SDLK_LSHIFT, vbl::Controller::InputIDX::INPUT_DASH);

	game.makeController("Player 2", 2);
	game.makeGuy("guy-2", "content/graphics/guy_sheet2.png", {20,20});
	game.linkController("Player 2", "guy-2");
	ctrl = game.getController("Player 2");
	ctrl->setKey((int)SDLK_UP, vbl::Controller::InputIDX::INPUT_UP);
	ctrl->setKey((int)SDLK_LEFT, vbl::Controller::InputIDX::INPUT_LEFT);
	ctrl->setKey((int)SDLK_DOWN, vbl::Controller::InputIDX::INPUT_DOWN);
	ctrl->setKey((int)SDLK_RIGHT, vbl::Controller::InputIDX::INPUT_RIGHT);
	ctrl->setKey((int)SDLK_RSHIFT, vbl::Controller::InputIDX::INPUT_DASH);

	game.loadEffectSprite(vbl::Ball::POWERUP_TRIPLEJUMP, "content/graphics/stat_triple.png");
	game.loadEffectSprite(vbl::Ball::POWERUP_DOUBLEPOINTS, "content/graphics/stat_doublepoints.png");

	game.setWaitingScreen(1, { 0, 0, 1920 / 2, 1080 }, "content/graphics/waiting_left.png");
	game.setWaitingScreen(2, { 1920 / 2, 0, 1920 / 2, 1080 }, "content/graphics/waiting_right.png");

	game.renderer.loadChars("text/font_1.png", {255, 255, 255, 255});
	game.makeBall({ 0,0 }, "content/graphics/ball.png", "content/graphics/ball_glow.png");
	game.map.resetMap();
	vbl::Game::TeamData* team = NULL;
	team = game.changeTeam(1);
	team->scoreTextPos = { 480, 940 };
	team = game.changeTeam(2);
	team->scoreTextPos = { 1440, 940 };
	game.sound.loadMusic("content/Gymnopedie No 1.mp3", "goime500");
	game.sound.playMusic("goime500");
	game.run();
	return 0;
}