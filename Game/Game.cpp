#include "Game.h"

#include <chrono>
#include <fstream>
#include <sstream>

#include "random.h"
#include "strhelp.h"
#include "loadutils.h"

#include "GDebug/logtools.h"

void vbl::Map::load(const std::string& path)
{
	this->gravity = 0;
	this->geometry.empty();
	this->spawnpoints.clear();
	this->clearGuys();
	this->clearActors();
	this->clearBalls();
	this->ballSpawnPoint = { 0,0 };
	this->ballSpawnVel = { 0,0 };
	std::ifstream rawFile(path);
	std::stringstream stream;
	stream << rawFile.rdbuf();
	std::string file = stream.str();
	size_t index = 0;
	size_t found = 0;
	strhelp::findAndRemove(file, '\n');
	LOG(std::format("loading map {}", path));
	DEBUG_LOG(std::format("formatted map is:\n {}", file));
	FileLoadingMode mode = FileLoadingMode::NONE;
	bool isModeLine = false;
	while (true)
	{
		isModeLine = false;
		found = file.find(';', index);
		if (found == std::string::npos)
		{
			break;
		}
		std::string loaded = file.substr(index, found - index);
		if (loaded == "gravity") { mode = FileLoadingMode::GRAVITY; isModeLine = true; DEBUG_LOG("MAPLOADING: loading gravity"); }
		else if (loaded == "boxes") { mode = FileLoadingMode::BOXES; isModeLine = true; DEBUG_LOG("MAPLOADING: loading boxes"); }
		else if (loaded == "colors") { mode = FileLoadingMode::COLORS; isModeLine = true; DEBUG_LOG("MAPLOADING: loading colors"); }
		else if (loaded == "spawnpoints") { mode = FileLoadingMode::SPAWNPOINTS; isModeLine = true; DEBUG_LOG("MAPLOADING: loading spawnpoints"); }
		else if (loaded == "spawnvel") { mode = FileLoadingMode::SPAWNVEL; isModeLine = true; DEBUG_LOG("MAPLOADING: loading spawnvels"); }
		if (isModeLine)
		{
			index = found + 1;
			continue;
		}
		switch (mode)
		{
		case FileLoadingMode::GRAVITY:
		{
			size_t inIndex = 0;
			size_t inFound = 0;
			bool run = true;
			while (run)
			{
				inFound = loaded.find(',', inIndex);
				if (inFound == std::string::npos)
				{
					inFound = loaded.length();
					run = false;
				}
				std::string valueStr = loaded.substr(inIndex, inFound - inIndex);
				float loaded = 0;
				//TODO: exception handling
				loaded = std::stof(valueStr);
				this->gravity = loaded;
				inIndex = inFound + 1;
			}
		}
			break;
		case FileLoadingMode::BOXES:
		{
			size_t inIndex = 0;
			size_t inFound = 0;
			maf::frect rect{};
			int team = 0;
			bool isTrigger = false;
			maf::fvec2 foundVec = loadutils::loadVec2(loaded, &inIndex);
			rect.x = foundVec.x;
			rect.y = foundVec.y;
			foundVec = loadutils::loadVec2(loaded, &inIndex);
			rect.w = foundVec.x;
			rect.h = foundVec.y;
			// 1,2,3,4 are x,y,w,h
			// 5 is team
			// 6 is istrigger
			int current = 4;
			bool run = true;
			while (run)
			{
				current++;
				inFound = loaded.find(',', inIndex);
				if (inFound == std::string::npos)
				{
					inFound = loaded.length();
					run = false;
				}
				std::string valueStr = loaded.substr(inIndex, inFound - inIndex);
				switch (current)
				{
				case 5:
					team = std::stoi(valueStr);
					break;
				case 6:
					isTrigger = (valueStr == "t");
					//TODO: exception handling
					break;
				default:
					//TODO: exception handling
					break;
				}
				inIndex = inFound + 1;
			}
			this->geometry.add(GeometryBox{ rect, (uint16_t)team, isTrigger });
		}
			break;
		case FileLoadingMode::COLORS:
		{
			size_t inIndex = 0;
			maf::frect rect{};
			SDL_Color clr{};
			maf::fvec2 vec = loadutils::loadVec2(loaded, &inIndex);
			rect.x = vec.x;
			rect.y = vec.y;
			vec = loadutils::loadVec2(loaded, &inIndex);
			rect.w = vec.x;
			rect.h = vec.y;
			vec = loadutils::loadVec2(loaded, &inIndex);
			clr.r = (Uint8)vec.x;
			clr.g = (Uint8)vec.y;
			vec = loadutils::loadVec2(loaded, &inIndex);
			clr.b = (Uint8)vec.x;
			clr.a = (Uint8)vec.y;
			this->geometry.add(ColoredRect{ rect, clr });
		}
			break;
		case FileLoadingMode::SPAWNPOINTS:
		{
			size_t inIndex = 0;
			size_t inFound = 0;
			maf::fvec2 pos;
			// if 0 then is ball spawn point
			uint16_t team = 0;
			maf::fvec2 vec = loadutils::loadVec2(loaded, &inIndex);
			pos.x = vec.x;
			pos.y = vec.y;
			bool run = true;
			while (run)
			{
				inFound = loaded.find(',', inIndex);
				if (inFound == std::string::npos)
				{
					inFound = loaded.length();
					run = false;
				}
				std::string valueStr = loaded.substr(inIndex, inFound - inIndex);
				team = (valueStr == "b") ? 0 : (uint16_t)std::stoi(valueStr);
				//TODO: exception handling
				inIndex = inFound + 1;
			}
			if (team == 0)
			{
				this->ballSpawnPoint = pos;
				break;
			}
			if (this->spawnpoints.size() <= team)
			{
				this->spawnpoints.resize((size_t)team + 1, {0});
			}
			this->spawnpoints[team] = pos;
		}
			break;
		case FileLoadingMode::SPAWNVEL:
		{
			maf::fvec2 vel = loadutils::loadVec2(loaded);
			this->ballSpawnVel = vel;
		}
			break;
		default:
			break;
		}
		index = found + 1;
	}
}

void vbl::Map::clearGuys()
{
	this->guys.clear();
}

void vbl::Map::clearActors()
{
	this->actors.clear();
}

void vbl::Map::clearBalls()
{
	this->balls.clear();
}

vbl::Game::Game(uint32_t width, uint32_t height, float scale)
	:renderer(width, height, 2048, 2048, scale), sound(10), nextPowerupTick(0)
{
	this->renderer.setMissingTexture("missing.png");
	this->traceEndTexture = SpriteTexture("trace_end", { 0,0,44,44 }, 0);
	this->particleManager.game = this;
}

bool vbl::Map::collides(const MAABB& box) const
{
	return geometry.collides(box);
}

int vbl::Map::addGuy(const std::string& name, const std::string& picture, maf::ivec2 spriteDim)
{
	if (guyMap.count(name))
	{
		return 1;
	}
	guyMap[name] = guys.size();
	guys.push_back(std::make_shared<Guy>(name, 80, picture));
	std::shared_ptr<Guy> guy = guys[guys.size() - 1];
	guy->changeTexture().setPicture(picture);
	guy->changeTexture().setSpriteWidth(spriteDim.x);
	guy->changeTexture().setSpriteHeight(spriteDim.y);
	actors.push_back(guy);
	return 0;
}

std::shared_ptr<vbl::Guy> vbl::Map::getGuy(const std::string& name)
{
	if (!guyMap.count(name))
	{
		return NULL;
	}
	return guys[guyMap[name]];
}

std::shared_ptr<vbl::Ball> vbl::Map::addBall(const std::string& picture, const std::string& glowPicture)
{
	balls.push_back(std::make_shared<Ball>(picture, glowPicture, 80));
	std::shared_ptr<Ball> ball = balls[balls.size() - 1];
	ball->reset(this->ballSpawnCooldown);
	ball->setPos(this->ballSpawnPoint);
	ball->setVel(this->getRandomSpawnDir());
	return ball;
}

void vbl::Map::removeBall(int* i)
{
	if (!i)
	{
		return;
	}
	if (*i >= this->balls.size())
	{
		return;
	}
	this->balls.erase(this->balls.begin() + *i);
	(*i)--;
}

void vbl::Map::setSpawnPoint(uint16_t team, maf::fvec2 where)
{
	spawnpoints.resize(std::max(spawnpoints.size(), (size_t)team) + 1);
	spawnpoints[team] = where;
}

maf::fvec2 vbl::Map::getSpawnPoint(uint16_t team)
{
	if (team >= spawnpoints.size())
	{
		return { 0,0 };
	}
	return spawnpoints[team];
}

maf::fvec2 vbl::Map::getRandomSpawnDir()
{
	float modifier = 1;
	if (rand() % 2)
	{
		modifier = -1;
	}
	return { this->ballSpawnVel.x * modifier, this->ballSpawnVel.y };
}

void vbl::Map::respawnGuy(std::shared_ptr<vbl::Guy> guy, uint16_t team)
{
	guy->setPos(this->getSpawnPoint(team));
	guy->reset();
}

void vbl::Map::resetMap()
{
	for (auto guy : guys)
	{
		uint16_t team = 0;
		if (guy->getController())
		{
			team = guy->getController()->getTeam();
		}
		respawnGuy(guy, team);
	}
	for (int i = 0; i < this->balls.size(); i++)
	{
		std::shared_ptr<vbl::Ball> ball = this->balls[i];
		if (!ball->isTrigger())
		{
			balls.erase(balls.begin() + i);
			i--;
			continue;
		}
		ball->reset(this->ballSpawnCooldown);
		ball->setPos(this->ballSpawnPoint);
		ball->setVel(this->getRandomSpawnDir());
	}
}

int vbl::Game::makeController(const std::string& name, uint16_t team)
{
	if (controllerMap.count(name))
	{
		return 1;
	}
	controllerMap[name] = controllers.size();
	controllers.push_back(new Controller(name, team));
	this->teamData[team].ready = false;
	return 0;
}

void vbl::Game::registerAI(vbl::AI* ai)
{

}

int vbl::Game::makeGuy(const std::string& name, const std::string& picture, maf::ivec2 spriteDim)
{
	int result = map.addGuy(name, picture, spriteDim);
	if (!result)
	{
		std::shared_ptr<vbl::Guy> guy = map.getGuy(name);
		guy->changeTexture().setAnimated(true);
		guy->setParticle("grd", "ground_particle");
		guy->setParticle("air", "air_particle");
	}
	return result;
}

std::shared_ptr<vbl::Ball> vbl::Game::makeBall(maf::fvec2 pos, const std::string& picture, const std::string& glowPicture)
{
	std::shared_ptr<vbl::Ball> ball = map.addBall(picture, glowPicture);
	ball->setParticle("cld", "ball_particle");
	ball->setParticle("bnk", "boink");
	ball->setSound("boink", this->sound.getID("boink"));
	Particle* particle = this->particleManager.spawnParticle(120, maf::setMiddle(ball->getVisMid(), {256,256}), {0,0}, "glow", {0, 0, 256, 256}, 0, 0);
	particle->changeTexture().usesSpecialBlendmode = true;
	particle->changeTexture().specialBlendingMode = SDL_BLENDMODE_ADD;
	return ball;
}

std::shared_ptr<vbl::Ball> vbl::Game::makePowerupBall(const std::string& path, const std::string& glowPath, Ball::PowerupType power)
{
	std::shared_ptr<vbl::Ball> ball = makeBall(this->map.ballSpawnPoint, path, glowPath);
	ball->setPowerup(power);
	return ball;
}

void vbl::Game::respawnGuys()
{
	for (auto guy : map.guys)
	{
		const Controller* ctrl = guy->getController();
		if (!ctrl)
		{
			map.respawnGuy(guy, 0);
			continue;
		}
		map.respawnGuy(guy, ctrl->getTeam());
	}
}


void vbl::Game::resetGame()
{
	clearPowerups();
	selectNextPowerupTick();
	this->map.resetMap();
	for (auto& c : controllers)
	{
		c->reset();
	}
	for (auto& teamState : this->teamData)
	{
		teamState.second.ready = false;
	}
	this->state = GameState::STATE_WAITING_FOR_INPUT;
}

vbl::Controller* vbl::Game::getController(const std::string& name)
{
	if (!controllerMap.count(name))
	{
		return NULL;
	}
	return controllers[controllerMap[name]];
}

int vbl::Game::linkController(const std::string& controllerName, const std::string& guyName)
{
	std::shared_ptr<vbl::Guy> guy = map.getGuy(guyName);
	Controller* controller = this->getController(controllerName);
	if (!guy)
	{
		return 1;
	}
	if (!controller)
	{
		return 2;
	}
	guy->link(controller);
	return 0;
}

void vbl::Game::unlink(const std::string& guyName)
{
	std::shared_ptr<vbl::Guy> guy = map.getGuy(guyName);
	if (!guy)
	{
		return;
	}
	guy->link(NULL);
}

void vbl::Game::applyTeamPowerup(uint16_t team, Ball::PowerupType power, float length, bool onGuy)
{
	if (!onGuy)
	{
		length = 999999999.0f;
		TeamData* dteam = changeTeam(team);
		if (!team)
		{
			return;
		}
		dteam->teamPowerups[power];
		dteam->teamPowerups[power]++;
		DEBUG_LOG(std::format("Team {} has powerup {} and has it {} times.", team, (int)power, dteam->teamPowerups[power]));
	}
	for (auto& guy : this->map.guys)
	{
		if (!guy->getController())
		{
			continue;
		}
		if (!team || guy->getController()->getTeam() == team)
		{
			guy->powerup(power, length);
		}
	}
}

void vbl::Game::run()
{
	LOG_F("{} BEGAN GAME LOOP", (void*)this);
	this->isRunning = true;
	while (this->isRunning)
	{
		timer.set();
		FLUSH_LOG();
		try
		{
			this->update();
			this->updateTime = timer.time();
			this->render();
			this->frameTime = timer.time();
			this->renderTime = this->frameTime - this->updateTime;
			if (this->frameTime < 15)
			{
				SDL_Delay(15 - (Uint32)this->frameTime);
			}
		}
		catch (const std::bad_alloc& e)
		{
			LOG_WARN("Caught and silenced BAD_ALLOC");
			printf("fatal error.\n");
		}
		catch (...)
		{
			LOG_WARN("Caught and silenced exception");
		}
		printf("update %fms\n", this->updateTime);
		printf("render %fms\n", this->renderTime);
		printf("total %fms\n", this->frameTime);
	}
}

void vbl::Game::input()
{
	for (auto& c : controllers)
	{
		c->update();
	}
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			this->quit();
			break;
		case SDL_KEYDOWN:
			for (auto& c : controllers)
			{
				c->keyDown(e.key.keysym.sym);
			}
			switch (e.key.keysym.sym)
			{
			case SDLK_r:
				map.resetMap();
				break;
			case SDLK_e:
				applyTeamPowerup(1, vbl::Ball::POWERUP_TRIPLEJUMP, 600);
				break;
			}
			break;
		case SDL_KEYUP:
			for (auto& c : controllers)
			{
				c->keyUp(e.key.keysym.sym);
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (e.button.button)
			{
			case SDL_BUTTON_LEFT:
				this->lmb = true;
				break;
			case SDL_BUTTON_RIGHT:
				this->rmb = true;
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch (e.button.button)
			{
			case SDL_BUTTON_LEFT:
				this->lmb = false;
				break;
			case SDL_BUTTON_RIGHT:
				this->rmb = false;
				break;
			}
			break;
		default:
			break;
		}
	}
	maf::ivec2 mousePos{};
	SDL_GetMouseState(&mousePos.x, &mousePos.y);
	std::shared_ptr<RawTexture> mousePosText = queueText(std::to_string(mousePos.x) + ", " + std::to_string(mousePos.y));
	mousePosText->setPos(mousePos);
	std::shared_ptr<RawTexture> text = queueText(std::to_string(this->tick));
	text->setPos({ 0, 30 });
	text = queueText(std::to_string(this->nextPowerupTick));
	text->setPos({ 0, 60 });
	text = queueText(std::to_string(maf::random(500, 1500)));
	text->setPos({ 0, 90 });
	if (this->lmb)
	{
		MAABB box;
		box.add({ 0, 0, 20, 20 });
		particleManager.spawnCustom(new PBRParticle(600,
			maf::fvec2{ (float)mousePos.x, (float)mousePos.y },
			maf::fvec2{ maf::random(-10.0f, 10.0f),maf::random(-10.0f, 10.0f) },
			"casing",
			SDL_Rect{ 0, 0, 10, 30 },
			0, 10, box));
	}
}

void vbl::Game::updatePaused()
{
	
}

void vbl::Game::updateWaiting()
{
	for (auto& c : this->controllers)
	{
		if (c->ready())
		{
			if (!this->teamData[c->getTeam()].ready)
			{
				DEBUG_LOG(std::format("Controller {} of team {} readied up", c->getName(), c->getTeam()));
			}
			this->teamData[c->getTeam()].ready = true;
		}
	}
	for (auto& teamState : this->teamData)
	{
		if (!teamState.second.ready)
		{
			return;
		}
	}
	this->selectNextPowerupTick();
	this->state = GameState::STATE_RUNNING;
}

void vbl::Game::score(uint16_t team, int amount)
{
	if (!this->teamData.count(team))
	{
		this->teamData[team].score = 0;
	}
	this->teamData[team].score += amount;
}

void vbl::Game::updateGame()
{
	for (auto& guy : this->map.guys)
	{
		guy->setGravity(this->map.gravity);
		guy->update(this->map.getGeometry(), this->tick);
		this->particleManager.addParticles(guy->getParticles());
		this->sound.takeSounds(guy->getSounds());
	}
	for (int i = 0; i < this->map.balls.size(); i++)
	{
		std::shared_ptr<vbl::Ball> ball = this->map.balls[i];
		ball->setGravity(this->map.gravity);
		ball->update(this->map.getGeometry(), this->map.actors, this->tick, 1.0f);
		ball->trace(this->map.geometry, this->map.actors, 2000, 16);
		this->particleManager.addParticles(ball->getParticles());
		this->sound.takeSounds(ball->getSounds());
		if (ball->hitGuy() && ball->hitStrength() > 15)
		{
			this->cameraShake(2, std::pow(ball->hitStrength() - 15, 2.0f) / 50);
		}
		if (ball->triggered())
		{
			this->particleManager.spewParticles(120, ball->getVisMid(), { 0,0 }, *ball->getParticle("cld"), {0,0,24,24}, 0, 0, 50, 10);
			this->particleManager.spawnParticle(120, maf::setMiddle(ball->getVisMid(), {256,256}), {0,0}, "ball_glow", {0, 0, 256, 256}, 0, 0);
			this->cameraShake(10, 5);
			if (Ball::PowerupType power = ball->isPowerup())
			{
				bool appliesToTeam = false;
				if (power == Ball::POWERUP_DOUBLEPOINTS)
				{
					appliesToTeam = true;
				}
				applyTeamPowerup(ball->team(), power, 600, !appliesToTeam);
				this->spawnFadeUp(ball->team(), 60);
				this->map.removeBall(&i);
				continue;
			}
			int scoreMultiplier = this->teamData[ball->team()].teamPowerups[Ball::POWERUP_DOUBLEPOINTS];
			if (scoreMultiplier)
			{
				score(ball->team(), scoreMultiplier);
			}
			score(ball->team());
			this->sound.playSound("crunch");
			this->scoredCooldown = 15;
			this->state = GameState::STATE_SCORED;
			continue;
		}
	}
	processParticles();
	for (const auto& team : this->teamData)
	{
		std::shared_ptr<RawTexture> text = queueText(std::to_string(team.second.score));
		text->setMiddle(team.second.scoreTextPos);
		text->resize(4);
	}
	if (this->tick > nextPowerupTick)
	{
		spawnRandomPowerup();
		selectNextPowerupTick();
	}
}

void vbl::Game::spawnRandomPowerup()
{
	switch (maf::random(0, 1))
	{
	case 0:
		//triplejump
		makePowerupBall("triplejump", "ball_glow", Ball::POWERUP_TRIPLEJUMP);
		break;
	case 1:
		//double points
		makePowerupBall("doublepoints", "ball_glow", Ball::POWERUP_DOUBLEPOINTS);
		break;
	}
}

void vbl::Game::updateScored()
{
	this->processParticles();
	if (this->scoredCooldown)
	{
		this->scoredCooldown--;
	}
	else
	{
		this->resetGame();
	}
}

void vbl::Game::update()
{
	tick++;
	switch (this->state)
	{
	case GameState::STATE_RUNNING:
		this->input();
		updateGame();
		break;
	case GameState::STATE_WAITING_FOR_INPUT:
		this->input();
		updateWaiting();
		break;
	case GameState::STATE_PAUSED:
		this->input();
		updatePaused();
		break;
	case GameState::STATE_SCORED:
		updateScored();
		break;
	}
	if (this->remainingShake > 0)
	{
		this->remainingShake--;
	}
	this->queueText(std::to_string(this->updateTime) + "ms ("
		+ std::to_string(this->max_updateTime) + "ms) "
		+ std::to_string(this->renderTime) + "ms ("
		+ std::to_string(this->max_renderTime) + "ms) "
		+ std::to_string(this->frameTime) + "ms "
		+ ((this->updateTime > this->renderTime) ? "update" : "render"));
	this->max_updateTime = std::max(this->updateTime, this->max_updateTime);
	this->max_renderTime = std::max(this->renderTime, this->max_renderTime);
}

void vbl::Game::render()
{
	this->renderer.clearFrame(bg);
	this->renderer.setPos({ 0,0 });
	if (this->remainingShake > 0)
	{
		this->renderer.setPos({ (float)maf::random(-this->shakeAmp, this->shakeAmp), (float)maf::random(-this->shakeAmp, this->shakeAmp) });
	}
	this->renderer.renderGeometry(this->map.geometry);
	renderParticles();
	for (const auto& guy : this->map.guys)
	{
		this->renderer.renderSpriteConst(guy->getTexture());
		if (this->seeBoxes)
		{
			this->renderer.renderBoundingBox(guy->getBox(), { 0, 255, 0, 255 });
		}
		//this->renderer.renderBoundingBox(guy->getBox());
		renderStatusEffects({guy->getPos().x - 4, guy->getPos().y - 4}, guy->getPowers());
	}
	for (const auto& ball : this->map.balls)
	{
		this->renderer.renderSpriteConst(ball->getTexture());
		if (ball->getGlow())
		{
			SpriteTexture tex(ball->getGlowTex(), *ball->getTexture().getRect(), 0.0f);
			this->renderer.renderSprite(tex, ball->getGlow());
		}
		if (ball->getVisMid().y < 0)
		{
			SDL_Rect ballRect = *ball->getTexture().getRect();
			SpriteTexture tex("offscreen_arrow", { int(ball->getPos().x), 0, ballRect.w, ballRect.h }, 0);
			this->renderer.renderSprite(tex);
		}
		if (this->seeBoxes)
		{
			this->renderer.renderBoundingBox(ball->getBox(), { 0, 0, 255, 255 });
		}
		//this->renderer.renderBoundingBox(ball->getBox());
		if (this->state == GameState::STATE_WAITING_FOR_INPUT)
		{
			continue;
		}
		this->renderer.renderTrace(ball->tracer, this->traceEndTexture);
	}
	if (this->state == GameState::STATE_WAITING_FOR_INPUT)
	{
		for (const auto& teamState : this->teamData)
		{
			if (teamState.second.ready)
			{
				continue;
			}
			this->renderer.renderSpriteConst(teamState.second.waitingSprite, 64);
		}
	}
	if (this->seeBoxes)
	{
		const std::vector<GeometryBox>& boxes = this->map.geometry.getBoxes();
		for (int i = 0; i < boxes.size(); i++)
		{
			if (boxes[i].trigger)
			{
				this->renderer.renderRect(boxes[i].box, this->changeTeam(boxes[i].team)->color);
			}
		}
	}
	this->renderText();
	this->renderer.presentFrame();
}

void vbl::Game::processParticles()
{
	this->particleManager.process();
}

void vbl::Game::renderParticles()
{
	const std::list<std::unique_ptr<vbl::Particle>>& particles = this->particleManager.getParticles();
	for (const auto& p : particles)
	{
		this->renderer.renderSpriteConst(p->getTexture(), p->getAlpha());
		if (seeBoxes)
		{
			if (p->type == PType::PBR)
			{
				PBRParticle* casted = reinterpret_cast<PBRParticle*>(p.get());
				this->renderer.renderBoundingBox(casted->getHull());
				if (casted->didx)
				{
					maf::frect rect = casted->getHull().getBoxes()[0];
					rect.w /= 2;
					this->renderer.renderRect(rect, { 0, 255, 255, 255 });
				}
				if (casted->didy)
				{
					maf::frect rect = casted->getHull().getBoxes()[0];
					rect.w /= 2;
					rect.x += rect.w;
					this->renderer.renderRect(rect, { 255, 255, 0, 255 });
				}
			}
		}
	}
}

void vbl::Game::renderStatusEffects(maf::fvec2 pos, const std::vector<Guy::Powerup>& powers)
{
	std::vector<int> counts;
	//POWERUP COUNT
	counts.reserve(8);
	for (const auto& p : powers)
	{
		int idx = (int)p.type;
		if (idx >= counts.size())
		{
			counts.resize(idx + 1, 0);
		}
		counts[idx]++;
	}
	pos.x -= this->effectSpriteDim.x;
	float startX = pos.x;
	for (int type = 0; type < counts.size(); type++)
	{
		int count = counts[type];
		if (count > 0)
		{
			while (count)
			{
				this->renderEffectSprite(pos, type);
				pos.x -= this->effectSpriteDim.x + 8;
				count--;
			}
			pos.x = startX;
			pos.y += this->effectSpriteDim.y + 8;
		}
	}
}

void vbl::Game::renderEffectSprite(maf::fvec2 pos, int type)
{
	std::string toUse = "NO_ASSIGN";
	if (type < this->effectSpritesByType.size())
	{
		toUse = this->effectSpritesByType[type];
	}
	SpriteTexture tex(
		toUse,
		{ (int)pos.x, (int)pos.y, this->effectSpriteDim.x, this->effectSpriteDim.y },
		0);
	this->renderer.renderSprite(tex);
}

void vbl::Game::loadEffectSprite(Ball::PowerupType effect, const std::string& picture)
{
	int idx = (int)effect;
	if (idx >= this->effectSpritesByType.size())
	{
		this->effectSpritesByType.resize(idx + 1, "NO_ASSIGN");
	}
	this->effectSpritesByType[idx] = picture;
}

std::shared_ptr<vbl::RawTexture> vbl::Game::queueText(const std::string& str)
{
	this->textToRender.push_back(this->renderer.makeText(str));
	return this->textToRender[this->textToRender.size() - 1];
}

void vbl::Game::renderText()
{
	while (this->textToRender.size())
	{
		std::shared_ptr<RawTexture> tex = this->textToRender[this->textToRender.size() - 1];
		this->renderer.renderText(tex);
		this->textToRender.pop_back();
	}
}

void vbl::Game::clearPowerups()
{
	for (auto& guy : this->map.guys)
	{
		guy->clearPowerups();
	}
	for (auto& team : this->teamData)
	{
		team.second.teamPowerups.clear();
	}
}

void vbl::Game::setWaitingScreen(uint16_t team, SDL_Rect box, const std::string& name)
{
	this->teamData[team].waitingSprite = SpriteTexture(name, box, 0);
}

void vbl::Game::cameraShake(float length, float amplitude)
{
	this->remainingShake = length;
	this->shakeAmp = amplitude;
}

void vbl::Game::spawnFadeUp(uint16_t team, uint32_t duration)
{
	const std::vector<vbl::GeometryBox>& boxes = this->map.geometry.getBoxes();
	for (const auto& box : boxes)
	{
		if (!box.trigger)
		{
			continue;
		}
		if (box.team != team)
		{
			continue;
		}
		this->particleManager.spawnParticle(duration, { box.box.x, box.box.y - box.box.h / 4 }, { 0,0 }, "white_gradient", { 0,0,(int)box.box.w, int(box.box.h * 1.25) }, 0, 0);
	}
}
