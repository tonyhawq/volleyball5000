#include "Game/Game.h"
#include "Game/Atlas.h"

#include "Game/Debug/logtools.h"

#include <SDL_image.h>

int main(int args, char* argc[])
{
	BEGIN_DEBUG_LOG("current.log");
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	vbl::Game game = vbl::Game(1920, 1080, 0.25);
	game.sound.loadSound({ "content/sound/boink1.wav", "content/sound/boink2.wav", "content/sound/boink3.wav" }, "boink");
	game.sound.loadSound({ "content/sound/crunch1.wav", "content/sound/crunch2.wav", "content/sound/crunch3.wav" }, "crunch");

	game.map.load("content/maps/1.vbl5");
	game.makeController("Player 1", 1);
	game.makeGuy("guy-1", "guy_sheet", {20,20});
	game.linkController("Player 1", "guy-1");
	vbl::Controller* ctrl = game.getController("Player 1");
	ctrl->setKey((int)SDLK_w, vbl::Controller::InputIDX::INPUT_UP);
	ctrl->setKey((int)SDLK_a, vbl::Controller::InputIDX::INPUT_LEFT);
	ctrl->setKey((int)SDLK_s, vbl::Controller::InputIDX::INPUT_DOWN);
	ctrl->setKey((int)SDLK_d, vbl::Controller::InputIDX::INPUT_RIGHT);
	ctrl->setKey((int)SDLK_LSHIFT, vbl::Controller::InputIDX::INPUT_DASH);

	game.makeController("Player 2", 2);
	game.makeGuy("guy-2", "guy_2_sheet", {20,20});
	game.linkController("Player 2", "guy-2");
	ctrl = game.getController("Player 2");
	ctrl->setKey((int)SDLK_UP, vbl::Controller::InputIDX::INPUT_UP);
	ctrl->setKey((int)SDLK_LEFT, vbl::Controller::InputIDX::INPUT_LEFT);
	ctrl->setKey((int)SDLK_DOWN, vbl::Controller::InputIDX::INPUT_DOWN);
	ctrl->setKey((int)SDLK_RIGHT, vbl::Controller::InputIDX::INPUT_RIGHT);
	ctrl->setKey((int)SDLK_RSHIFT, vbl::Controller::InputIDX::INPUT_DASH);

	game.loadEffectSprite(vbl::Ball::POWERUP_TRIPLEJUMP, "stat_triple");
	game.loadEffectSprite(vbl::Ball::POWERUP_DOUBLEPOINTS, "stat_doublepoints");

	game.setWaitingScreen(1, { 0, 0, 1920 / 2, 1080 }, "waiting_left");
	game.setWaitingScreen(2, { 1920 / 2, 0, 1920 / 2, 1080 }, "waiting_right");

	game.renderer.loadChars("text/font_1.png", {255, 255, 255, 255});
	game.makeBall({ 0,0 }, "ball", "ball_glow");
	game.map.resetMap();
	vbl::Game::TeamData* team = NULL;
	team = game.changeTeam(1);
	team->scoreTextPos = { 480, 940 };
	team = game.changeTeam(2);
	team->scoreTextPos = { 1440, 940 };
	game.sound.loadMusic("content/Gymnopedie No 1.mp3", "goime500");
	game.sound.playMusic("goime500");
	LOG("Beginning to load texture atlas.");
	std::vector<std::string> paths = {
		"content/graphics/missing.png",
		"content/graphics/air_particle.png",
		"content/graphics/ball.png",
		"content/graphics/ball_ex_1.png",
		"content/graphics/ball_glow.png",
		"content/graphics/ball_particle.png",
		"content/graphics/boink.png",
		"content/graphics/doublepoints.png",
		"content/graphics/flash.png",
		"content/graphics/glow.png",
		"content/graphics/ground_particle.png",
		"content/graphics/guy_sheet.png",
		"content/graphics/guy_sheet2.png",
		"content/graphics/offscreen_arrow.png",
		"content/graphics/stat_doublepoints.png",
		"content/graphics/stat_flash.png",
		"content/graphics/stat_triple.png",
		"content/graphics/trace_end.png",
		"content/graphics/triplejump.png",
		"content/graphics/waiting_left.png",
		"content/graphics/waiting_right.png"};
	std::vector<std::string> names = {
		"NO_ASSIGN",
		"air_particle",
		"ball",
		"ball_explosion",
		"ball_glow",
		"ball_particle",
		"boink",
		"doublepoints",
		"flash",
		"glow",
		"ground_particle",
		"guy_sheet",
		"guy_2_sheet",
		"offscreen_arrow",
		"stat_doublepoints",
		"stat_flash",
		"stat_triple",
		"trace_end",
		"triplejump",
		"waiting_left",
		"waiting_right"
	};
	size_t rejects = game.renderer.atlas.addBulk(paths, names).size();
	LOG("Texture atlas finished loading.");
	LOG_F("Loaded {}/{} textures. {}/{} of those loaded had names.", paths.size() - rejects, paths.size(), names.size(), paths.size());
	game.run();
	END_DEBUG_LOG();
	return 0;
}