#include "Game/Game.h"
#include "Game/Atlas.h"
#include "Game/AIs/SimpleAi.h"

#include "Game/GDebug/logtools.h"

#include <SDL_image.h>

int main(int args, char* argc[])
{
	vbl::GameSprite texture = vbl::GameSprite({ 0,0 }, "sigma.png", false);
	printf("has %s\n", texture.getTexture().getPicture().picture.c_str());
	vbl::GameSprite copied = texture;
	printf("has %s\n", copied.getTexture().getPicture().picture.c_str());
	vbl::GameSprite* cptr = new vbl::GameSprite(texture);
	printf("has %s\n", cptr->getTexture().getPicture().picture.c_str());

	BEGIN_DEBUG_LOG("current.log");
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	IMG_Init(IMG_INIT_PNG);
	vbl::Game game = vbl::Game(1920, 1080, 0.25);

	//
	game.seeBoxes = true;
	//

	game.sound.setChannels(48);
	game.sound.loadSound({ "content/sound/boink1.wav", "content/sound/boink2.wav", "content/sound/boink3.wav" }, "boink");
	game.sound.loadSound({ "content/sound/crunch1.wav", "content/sound/crunch2.wav", "content/sound/crunch3.wav" }, "crunch");

	game.map.load("content/maps/1.vbl5");
	game.makeController("Player 1", 1);
	game.makeGuy("guy-1", "guy_sheet", { 20,20 });
	game.linkController("Player 1", "guy-1");
	vbl::Controller* ctrl = game.getController("Player 1");
	ctrl->setKey((int)SDLK_w, vbl::Controller::InputIDX::INPUT_UP);
	ctrl->setKey((int)SDLK_a, vbl::Controller::InputIDX::INPUT_LEFT);
	ctrl->setKey((int)SDLK_s, vbl::Controller::InputIDX::INPUT_DOWN);
	ctrl->setKey((int)SDLK_d, vbl::Controller::InputIDX::INPUT_RIGHT);
	ctrl->setKey((int)SDLK_LSHIFT, vbl::Controller::InputIDX::INPUT_DASH);

	game.makeController("Player 2", 2);
	game.makeGuy("guy-2", "guy_2_sheet", { 20,20 });
	game.linkController("Player 2", "guy-2");
	ctrl = game.getController("Player 2");
	ctrl->setKey((int)SDLK_UP, vbl::Controller::InputIDX::INPUT_UP);
	ctrl->setKey((int)SDLK_LEFT, vbl::Controller::InputIDX::INPUT_LEFT);
	ctrl->setKey((int)SDLK_DOWN, vbl::Controller::InputIDX::INPUT_DOWN);
	ctrl->setKey((int)SDLK_RIGHT, vbl::Controller::InputIDX::INPUT_RIGHT);
	ctrl->setKey((int)SDLK_RSHIFT, vbl::Controller::InputIDX::INPUT_DASH);
	vbl::SimpleAI* ai = new vbl::SimpleAI(ctrl);
	game.registerAI(ai);
	ctrl->marry(ai);

	game.loadEffectSprite(vbl::Ball::POWERUP_TRIPLEJUMP, "stat_triple");
	game.loadEffectSprite(vbl::Ball::POWERUP_DOUBLEPOINTS, "stat_doublepoints");

	game.setWaitingScreen(1, { 0, 0, 1920 / 2, 1080 }, "waiting_left");
	game.setWaitingScreen(2, { 1920 / 2, 0, 1920 / 2, 1080 }, "waiting_right");

	game.renderer.loadChars("text/font_1.png", { 255, 255, 255, 255 });
	game.makeBall({ 0,0 }, "ball", "ball_glow");
	game.map.resetMap();
	vbl::Game::TeamData* team = NULL;
	team = game.changeTeam(1);
	team->scoreTextPos = { 480, 940 };
	team->color = { 255, 255, 0, 255 };
	team = game.changeTeam(2);
	team->scoreTextPos = { 1440, 940 };
	team->color = { 255, 0, 0, 255 };
	game.sound.loadMusic("content/sound/Gymnopedie No 1.mp3", "goime500");
	game.sound.playMusic("goime500");
	LOG("Beginning to load texture atlas.");
	std::vector<pair_str_t> paths = {
		pair_str_t{ "missing.png", "NO_ASSIGN" },
		pair_str_t{ "content/graphics/air_particle.png", "air_particle" },
		pair_str_t{ "content/graphics/ball.png", "ball"},
		pair_str_t{ "content/graphics/ball_ex_1.png", "ball_explosion"},
		pair_str_t{ "content/graphics/ball_glow.png", "ball_glow"},
		pair_str_t{ "content/graphics/ball_particle.png", "ball_particle"},
		pair_str_t{ "content/graphics/boink.png", "boink"},
		pair_str_t{ "content/graphics/doublepoints.png", "doublepoints"},
		pair_str_t{ "content/graphics/flash.png", "flash"},
		pair_str_t{ "content/graphics/glow.png", "glow"},
		pair_str_t{ "content/graphics/ground_particle.png", "ground_particle"},
		pair_str_t{ "content/graphics/guy_sheet.png", "guy_sheet"},
		pair_str_t{ "content/graphics/guy_sheet2.png", "guy_2_sheet"},
		pair_str_t{ "content/graphics/offscreen_arrow.png", "offscreen_arrow"},
		pair_str_t{ "content/graphics/stat_doublepoints.png", "stat_doublepoints"},
		pair_str_t{ "content/graphics/stat_flash.png", "stat_flash"},
		pair_str_t{ "content/graphics/stat_triple.png", "stat_triple"},
		pair_str_t{ "content/graphics/trace_end.png", "trace_end"},
		pair_str_t{ "content/graphics/triplejump.png", "triplejump"},
		pair_str_t{ "content/graphics/waiting_left.png", "waiting_left"},
		pair_str_t{ "content/graphics/waiting_right.png", "waiting_right"},
		pair_str_t{ "content/graphics/white_gradient.png", "white_gradient"},
		pair_str_t{ "content/graphics/pointer.png", "point"},
		pair_str_t{ "content/graphics/casing.png", "casing"},
		pair_str_t{ "content/graphics/blaster.png", "blaster"},
	};
	size_t rejects = game.renderer.atlas.addBulk(paths).size();
	LOG("Texture atlas finished loading.");
	LOG_F("Loaded {}/{} textures. {}/{} of those loaded had names.", paths.size() - rejects, paths.size(), names.size(), paths.size());
	vbl::Gun* pistol = game.makeGun({ 60, 60 }, "pistol", "blaster", "blaster_shoot", "bullet", { "casing" }, { "pistol_fire", "pistol_fire_2" }, 999, 10.0f, { 50.0f, 0.0f }, { 0 });
	pistol->offset = { 50, 50 };
	pistol->barrelOffset = pistol->offset + maf::fvec2{ (float)pistol->getTexture().dimensions().x / 2, (float)pistol->getTexture().dimensions().y / 2};
	for (auto& guy : game.map.guys)
	{
		guy->giveGun(new vbl::Gun(*game.getGun("pistol")));
	}
	game.run();
	LOG("Goodbye.")
	END_DEBUG_LOG();
	return 0;
}