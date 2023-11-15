#include "Game.h"

#include <chrono>
#include <fstream>
#include <sstream>

#include "random.h"
#include "strhelp.h"
#include "loadutils.h"

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
	printf("file is\n%s\n", file.c_str());
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
		if (loaded == "gravity") { mode = FileLoadingMode::GRAVITY; isModeLine = true; printf("begin loading gravity\n"); }
		else if (loaded == "boxes") { mode = FileLoadingMode::BOXES; isModeLine = true; printf("begin loading boxes\n"); }
		else if (loaded == "colors") { mode = FileLoadingMode::COLORS; isModeLine = true; printf("begin loading colors\n"); }
		else if (loaded == "spawnpoints") { mode = FileLoadingMode::SPAWNPOINTS; isModeLine = true; printf("begin loading spawnpoints\n"); }
		else if (loaded == "spawnvel") { mode = FileLoadingMode::SPAWNVEL; isModeLine = true; printf("begin loading spawnvels\n"); }
		if (isModeLine)
		{
			index = found + 1;
			continue;
		}
		switch (mode)
		{
		case FileLoadingMode::GRAVITY:
		{
			printf("loading gravity\n");
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
			printf("loading bounding boxes\n");
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
			printf("loading colored rects\n");
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
				this->spawnpoints.resize((size_t)team + 1);
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
	:renderer(width, height, scale), sound(10), nextPowerupTick(0)
{
	this->renderer.setMissingTexture("missing.png");
	this->traceEndTexture  = this->renderer.load("content/graphics/trace_end.png");
	this->ballExplosionTex = this->renderer.load("content/graphics/flash.png");
	this->glowTexture      = this->renderer.load("content/graphics/glow.png");
	this->offscreenArrow = this->renderer.load("content/graphics/offscreen_arrow.png");
}

bool vbl::Map::collides(const MAABB& box) const
{
	return geometry.collides(box);
}

int vbl::Map::addGuy(const std::string& name, SDL_Texture* tex, maf::ivec2 spriteDim)
{
	if (guyMap.count(name))
	{
		return 1;
	}
	guyMap[name] = guys.size();
	guys.push_back(std::make_shared<Guy>(name, 80, tex));
	std::shared_ptr<Guy> guy = guys[guys.size() - 1];
	guy->changeTexture().setTexture(tex);
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

std::shared_ptr<vbl::Ball> vbl::Map::addBall(SDL_Texture* tex, SDL_Texture* glowTex)
{
	balls.push_back(std::make_shared<Ball>(tex, glowTex, 80));
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

int vbl::Game::makeGuy(const std::string& name, const std::string& path, maf::ivec2 spriteDim)
{
	int result = map.addGuy(name, this->renderer.load(path), spriteDim);
	if (!result)
	{
		std::shared_ptr<vbl::Guy> guy = map.getGuy(name);
		guy->setParticle("grd", this->renderer.load("content/graphics/ground_particle.png"));
		guy->setParticle("air", this->renderer.load("content/graphics/air_particle.png"));
		guy->changeTexture().setAnimated(true);
	}
	return result;
}

std::shared_ptr<vbl::Ball> vbl::Game::makeBall(maf::fvec2 pos, const std::string& path, const std::string& glowPath)
{
	map.ballSpawnPoint = pos;
	std::shared_ptr<vbl::Ball> ball = map.addBall(this->renderer.load(path), this->renderer.load(glowPath));
	ball->setParticle("cld", this->renderer.load("content/graphics/ball_particle.png"));
	ball->setParticle("bnk", this->renderer.load("content/graphics/boink.png"));
	ball->setSound("boink", this->sound.getID("boink"));
	this->particleManager.spawnParticle(120, maf::setMiddle(ball->getVisMid(), {256,256}), {0,0}, this->glowTexture, {0, 0, 256, 256}, 0, 0);
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
		printf("team %u has powerup %i %i times\n", team, (int)power, dteam->teamPowerups[power]);
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
	this->isRunning = true;
	while (this->isRunning)
	{
		timer.set();
		this->update();
		this->updateTime = timer.time();
		this->render();
		this->frameTime = timer.time();
		this->renderTime = this->frameTime - this->updateTime;
		if (this->frameTime < 15)
		{
			SDL_Delay(15 - (Uint32)this->frameTime);
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
		default:
			break;
		}
	}
	maf::ivec2 mousePos{};
	SDL_GetMouseState(&mousePos.x, &mousePos.y);
	SpriteTexture* mousePosText = queueText(std::to_string(mousePos.x) + ", " + std::to_string(mousePos.y));
	mousePosText->setPos(mousePos);
	SpriteTexture* text = queueText(std::to_string(this->tick));
	text->setPos({ 0, 30 });
	text = queueText(std::to_string(this->nextPowerupTick));
	text->setPos({ 0, 60 });
	text = queueText(std::to_string(maf::random(500, 1500)));
	text->setPos({ 0, 90 });
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
			printf("controller %s ready\n", c->getName().c_str());
			this->teamData[c->getTeam()].ready = true;
		}
	}
	for (auto& teamState : this->teamData)
	{
		if (!teamState.second.ready)
		{
			return;
		}
		printf("team %u ready\n", teamState.first);
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
		printf("%f\n", guy->getPos().y);
	}
	for (int i = 0; i < this->map.balls.size(); i++)
	{
		std::shared_ptr<vbl::Ball> ball = this->map.balls[i];
		ball->setGravity(this->map.gravity);
		ball->update(this->map.getGeometry(), this->map.actors, this->tick);
		this->particleManager.addParticles(ball->getParticles());
		this->sound.takeSounds(ball->getSounds());
		if (ball->hitGuy() && ball->hitStrength() > 15)
		{
			this->cameraShake(2, std::pow(ball->hitStrength() - 15, 2.0f) / 50);
		}
		if (ball->triggered())
		{
			this->particleManager.spewParticles(120, ball->getVisMid(), { 0,0 }, ball->getParticle("cld"), {0,0,24,24}, 0, 0, 50, 10);
			this->particleManager.spawnParticle(120, maf::setMiddle(ball->getVisMid(), {256,256}), {0,0}, this->glowTexture, {0, 0, 256, 256}, 0, 0);
			this->cameraShake(10, 5);
			if (Ball::PowerupType power = ball->isPowerup())
			{
				bool appliesToTeam = false;
				if (power == Ball::POWERUP_DOUBLEPOINTS)
				{
					appliesToTeam = true;
				}
				applyTeamPowerup(ball->team(), power, 600, !appliesToTeam);
				this->map.removeBall(&i);
				continue;
			}
			int scoreMultiplier = this->teamData[ball->team()].teamPowerups[Ball::POWERUP_DOUBLEPOINTS];
			printf("score multiplier is %i\n", scoreMultiplier);
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
		SpriteTexture* text = queueText(std::to_string(team.second.score));
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
		makePowerupBall("content/graphics/triplejump.png", "content/graphics/ball_glow.png", Ball::POWERUP_TRIPLEJUMP);
		break;
	case 1:
		//double points
		makePowerupBall("content/graphics/doublepoints.png", "content/graphics/ball_glow.png", Ball::POWERUP_DOUBLEPOINTS);
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
	this->queueText(std::to_string(this->updateTime) + "ms "
		+ std::to_string(this->renderTime) + "ms "
		+ std::to_string(this->frameTime) + "ms "
		+ ((this->updateTime > this->renderTime) ? "update" : "render"));
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
		this->renderer.renderSprite(guy->getTexture());
		//this->renderer.renderBoundingBox(guy->getBox());
		renderStatusEffects({guy->getPos().x - 4, guy->getPos().y - 4}, guy->getPowers());
	}
	for (const auto& ball : this->map.balls)
	{
		this->renderer.renderSprite(ball->getTexture());
		if (ball->getGlow())
		{
			this->renderer.renderSprite(SpriteTexture(ball->getGlowTex(), *ball->getTexture().getRect(), 0.0f, false), ball->getGlow());
		}
		if (ball->getVisMid().y < 0)
		{
			SDL_Rect ballRect = *ball->getTexture().getRect();
			this->renderer.renderSprite(SpriteTexture(this->offscreenArrow, { int(ball->getPos().x), 0, ballRect.w, ballRect.h }, 0, false));
		}
		//this->renderer.renderBoundingBox(ball->getBox());
		if (this->state == GameState::STATE_WAITING_FOR_INPUT)
		{
			continue;
		}
		this->renderer.renderTrace(ball->trace(this->map.geometry, this->map.actors, 500, 1, 4), this->traceEndTexture);
	}
	if (this->state == GameState::STATE_WAITING_FOR_INPUT)
	{
		for (const auto& teamState : this->teamData)
		{
			if (teamState.second.ready)
			{
				continue;
			}
			this->renderer.renderSprite(teamState.second.waitingSprite, 64);
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
	const std::list<Particle>& particles = this->particleManager.getParticles();
	for (const auto& p : particles)
	{
		this->renderer.renderSprite(p.getTexture(), p.getAlpha());
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
	SDL_Texture* toUse = this->renderer.load("MSSNG");
	if (type < this->effectSpritesByType.size())
	{
		toUse = this->effectSpritesByType[type];
	}
	this->renderer.renderSprite(SpriteTexture(
		toUse,
		{ (int)pos.x, (int)pos.y, this->effectSpriteDim.x, this->effectSpriteDim.y },
	0, false));
}

void vbl::Game::loadEffectSprite(Ball::PowerupType effect, const std::string& path)
{
	int idx = (int)effect;
	if (idx >= this->effectSpritesByType.size())
	{
		this->effectSpritesByType.resize(idx + 1, NULL);
	}
	if (this->effectSpritesByType[idx])
	{
		SDL_DestroyTexture(this->effectSpritesByType[idx]);
	}
	this->effectSpritesByType[idx] = this->renderer.load(path);
}

vbl::SpriteTexture* vbl::Game::queueText(const std::string& str)
{
	this->textToRender.push_back(this->renderer.renderTextIrresponsible(str));
	return &this->textToRender[this->textToRender.size() - 1];
}

void vbl::Game::renderText()
{
	while (this->textToRender.size())
	{
		SpriteTexture& tex = this->textToRender[this->textToRender.size() - 1];
		this->renderer.renderSprite(tex);
		tex.destroyTexture();
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

void vbl::Game::setWaitingScreen(uint16_t team, SDL_Rect box, const std::string& path)
{
	if (this->teamData.count(team))
	{
		this->teamData[team].waitingSprite.destroyTexture();
	}
	this->teamData[team].waitingSprite = SpriteTexture(this->renderer.load(path), box, 0, false);
}

void vbl::Game::cameraShake(float length, float amplitude)
{
	this->remainingShake = length;
	this->shakeAmp = amplitude;
}
