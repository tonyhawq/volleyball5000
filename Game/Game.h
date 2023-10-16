#pragma once

#include <vector>
#include <unordered_map>

#include "Guy.h"
#include "Ball.h"
#include "Geometry.h"
#include "Renderer.h"

namespace vbl
{
	class Map
	{
	public:
		bool collides(const MAABB& box) const;
		
		inline const Geometry& getGeometry() const { return this->geometry; }
		inline void setGeometry(const Geometry& geometry) { this->geometry = geometry; }

		void setSpawnPoint(uint16_t team, maf::fvec2 where);
		maf::fvec2 getSpawnPoint(uint16_t team);

		int addGuy(const std::string& name, SDL_Texture* tex);
		Guy* getGuy(const std::string& name);

		void addBall(SDL_Texture* tex);

		void respawnGuy(Guy* guy, uint16_t team);

		void resetMap();

		maf::fvec2 getRandomSpawnDir();
		inline void setBallSpawnVel(maf::fvec2 vels) { this->ballSpawnVel = vels; }
	//private:

		maf::fvec2 ballSpawnPoint;
		float gravity = 0;
		maf::fvec2 ballSpawnVel;
		std::vector<Ball*> balls;
		std::unordered_map<std::string, size_t> guyMap;
		std::vector<Guy*> guys;
		std::vector<Sprite*> actors;
		std::vector<maf::fvec2> spawnpoints;
		Geometry geometry;
	};

	class Game
	{
	public:
		Game(uint32_t width, uint32_t height, float scale);

		void run();

		void input();
		void update();
		void updatePaused();
		void updateWaiting();
		void updateGame();
		void render();

		inline void quit() { this->isRunning = false; }
		inline bool running() const { return this->isRunning; }

		int makeController(const std::string& name, uint16_t team);
		int makeGuy(const std::string& name, const std::string& path);
		void makeBall(maf::fvec2 pos, const std::string& path);

		void respawnGuys();
		
		int linkController(const std::string& controllerName, const std::string& guyName);
		void unlink(const std::string& guyName);
		Controller* getController(const std::string& name);

		void resetGame();
		void score(uint16_t team);

		enum class GameState
		{
			STATE_PAUSED,
			STATE_WAITING_FOR_INPUT,
			STATE_RUNNING,
		};
	//private:
		GameState state = GameState::STATE_WAITING_FOR_INPUT;
		Renderer renderer;
		Map map;
		std::unordered_map<std::string, size_t> controllerMap;
		std::vector<Controller*> controllers;
		bool isRunning = false;
		SpriteTexture waitingForInputSprite;
		SDL_Color bg = { 110, 190, 255, 255 };
	};
}
