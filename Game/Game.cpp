#include "Game.h"

#include <chrono>

vbl::Game::Game(uint32_t width, uint32_t height, float scale)
	:renderer(width, height, scale)
{
	this->waitingForInputSprite.setTexture(this->renderer.load("waiting_for_input.png"));
	this->waitingForInputSprite.resize({ 1920, 1080 });
	this->waitingForInputSprite.setPos({ 0,0 });
}

bool vbl::Map::collides(const MAABB& box) const
{
	return geometry.collides(box);
}

int vbl::Map::addGuy(const std::string& name, SDL_Texture* tex)
{
	if (guyMap.count(name))
	{
		return 1;
	}
	guyMap[name] = guys.size();
	guys.push_back(new Guy(name, 80, tex));
	Guy* guy = guys[guys.size() - 1];
	guy->changeTexture().setTexture(tex);
	actors.push_back(guy);
	return 0;
}

vbl::Guy* vbl::Map::getGuy(const std::string& name)
{
	if (!guyMap.count(name))
	{
		return NULL;
	}
	return guys[guyMap[name]];
}

void vbl::Map::addBall(SDL_Texture* tex)
{
	balls.push_back(new Ball(tex, 80));
	Ball* ball = balls[balls.size() - 1];
	ball->setPos(this->ballSpawnPoint);
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

void vbl::Map::respawnGuy(Guy* guy, uint16_t team)
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
	for (auto ball : balls)
	{
		ball->setPos(this->ballSpawnPoint);
		ball->reset();
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
	return 0;
}

int vbl::Game::makeGuy(const std::string& name, const std::string& path)
{
	int result = map.addGuy(name, this->renderer.load(path));
	if (!result)
	{
		Guy* guy = map.getGuy(name);
	}
	return result;
}

void vbl::Game::makeBall(maf::fvec2 pos, const std::string& path)
{
	map.ballSpawnPoint = pos;
	map.addBall(this->renderer.load(path));
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
	this->map.resetMap();
	this->state = GameState::STATE_WAITING_FOR_INPUT;
	for (auto& c : controllers)
	{
		c->reset();
	}
}

void vbl::Game::score(uint16_t team)
{
	this->map.resetMap();
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
	Guy* guy = map.getGuy(guyName);
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
	Guy* guy = map.getGuy(guyName);
	if (!guy)
	{
		return;
	}
	guy->link(NULL);
}

void vbl::Game::run()
{
	this->isRunning = true;
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> duration = std::chrono::high_resolution_clock::now() - start;
	while (this->isRunning)
	{
		start = std::chrono::high_resolution_clock::now();
		this->input();
		this->update();
		this->render();
		duration = std::chrono::high_resolution_clock::now() - start;
		if (duration.count() < 15)
		{
			SDL_Delay(15 - (Uint32)duration.count());
		}
		printf("%fms\n", duration.count());
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
}


void vbl::Game::updatePaused()
{
	
}

void vbl::Game::updateWaiting()
{
	uint32_t ready = 0;
	for (auto& c : this->controllers)
	{
		if (!c->ready())
		{
			break;
		}
		ready++;
	}
	if (ready == this->controllers.size())
	{
		this->state = GameState::STATE_RUNNING;
	}
}

void vbl::Game::updateGame()
{
	for (auto& guy : this->map.guys)
	{
		guy->setGravity(this->map.gravity);
		guy->update(this->map.getGeometry());
		printf("%f\n", guy->getPos().y);
	}
	for (auto& ball : this->map.balls)
	{
		ball->setGravity(this->map.gravity);
		ball->update(this->map.getGeometry(), this->map.actors);
		if (ball->triggered())
		{
			map.resetMap();
			return;
		}
	}
}

void vbl::Game::update()
{
	switch (this->state)
	{
	case GameState::STATE_RUNNING:
		updateGame();
		break;
	case GameState::STATE_WAITING_FOR_INPUT:
		updateWaiting();
		break;
	case GameState::STATE_PAUSED:
		updatePaused();
		break;
	}

}

void vbl::Game::render()
{
	this->renderer.clearFrame(bg);
	this->renderer.renderGeometry(this->map.geometry);
	for (const auto& guy : this->map.guys)
	{
		this->renderer.renderSprite(guy->getTexture());
		this->renderer.renderBoundingBox(guy->getBox());
	}
	for (const auto& ball : this->map.balls)
	{
		this->renderer.renderSprite(ball->getTexture());
		this->renderer.renderBoundingBox(ball->getBox());
		this->renderer.renderTrace(ball->trace(this->map.geometry, this->map.actors, 500, 1));
	}
	if (this->state == GameState::STATE_WAITING_FOR_INPUT)
	{
		this->renderer.renderSprite(this->waitingForInputSprite, 128);
	}
	this->renderer.presentFrame();
}
