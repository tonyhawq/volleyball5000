#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "Sounds.h"
#include "Guy.h"
#include "Ball.h"
#include "Geometry.h"
#include "Renderer.h"
#include "Timer.h"

namespace vbl
{
	class AI;
	class Map
	{
	public:
		enum class FileLoadingMode
		{
			NONE,
			GRAVITY,
			BOXES,
			COLORS,
			SPAWNPOINTS,
			SPAWNVEL
		};
		void load(const std::string& path);
		void clearGuys();
		void clearActors();
		void clearBalls();

		bool collides(const MAABB& box) const;
		
		inline const Geometry& getGeometry() const { return this->geometry; }
		inline void setGeometry(const Geometry& geometry) { this->geometry = geometry; }

		void setSpawnPoint(uint16_t team, maf::fvec2 where);
		maf::fvec2 getSpawnPoint(uint16_t team);

		int addGuy(const std::string& name, const std::string& picture, maf::ivec2 spriteDim);
		std::shared_ptr<vbl::Guy> getGuy(const std::string& name);

		std::shared_ptr<vbl::Ball> addBall(const std::string& picture, const std::string& glowPicture);
		void removeBall(int* i);

		void respawnGuy(std::shared_ptr<vbl::Guy> guy, uint16_t team);

		void resetMap();

		maf::fvec2 getRandomSpawnDir();
		inline void setBallSpawnVel(maf::fvec2 vels) { this->ballSpawnVel = vels; }
	//private:

		maf::fvec2 ballSpawnPoint;
		float gravity = 0;
		maf::fvec2 ballSpawnVel;
		std::vector<std::shared_ptr<Ball>> balls;
		std::unordered_map<std::string, size_t> guyMap;
		std::vector<std::shared_ptr<Guy>> guys;
		std::vector<std::shared_ptr<Sprite>> actors;
		std::vector<maf::fvec2> spawnpoints;
		Geometry geometry;
		uint32_t ballSpawnCooldown = 30;
	};

	class Game
	{
	public:
		struct TeamData
		{
			TeamData()
			{
				printf("constructing TeamData\n");
			};
			~TeamData()
			{
				printf("destructing TeamData\n");
			};
			SpriteTexture waitingSprite = SpriteTexture(Picture{ "NO_ASSIGN" }, {0}, 0);
			bool ready = false;
			int score = 0;
			maf::ivec2 scoreTextPos = { 0,0 };
			std::unordered_map<Ball::PowerupType, int> teamPowerups;
			SDL_Color color{};
		};

		Game(uint32_t width, uint32_t height, float scale);

		void run();

		void input();
		void update();
		void updatePaused();
		void updateWaiting();
		void updateGame();
		void updateScored();

		void processParticles();

		void render();
		void renderParticles();

		void renderStatusEffects(maf::fvec2 pos, const std::vector<Guy::Powerup>& powers);
		void renderEffectSprite(maf::fvec2 pos, int type);

		inline void quit() { this->isRunning = false; }
		inline bool running() const { return this->isRunning; }

		void registerAI(AI*);
		int makeController(const std::string& name, uint16_t team);
		int makeGuy(const std::string& name, const std::string& picture, maf::ivec2 spriteDim);
		std::shared_ptr<vbl::Ball> makeBall(maf::fvec2 pos, const std::string& picture, const std::string& glowPicture);
		std::shared_ptr<vbl::Ball> makePowerupBall(const std::string& picture, const std::string& glowPicture, Ball::PowerupType power);

		Gun* makeGun(maf::fvec2 dim, strref name, strref picture, strref shoot_picture, strref bullet, const std::vector<std::string>& casings, const std::vector<std::string>& firing_noises, int ammo, float power, maf::fvec2 offset, maf::fvec2 barrelOffset);
		const Gun* getGun(strref name);

		void applyTeamPowerup(uint16_t team, Ball::PowerupType power, float length, bool onGuy = true);
		void spawnRandomPowerup();
		inline void selectNextPowerupTick() { this->nextPowerupTick = this->tick + maf::random(1*60, 2*60); }
		void score(uint16_t team, int amount = 1);
		void clearPowerups();

		void cameraShake(float length, float amplitude);

		void respawnGuys();
		void setWaitingScreen(uint16_t team, SDL_Rect box, const std::string& name);

		void loadEffectSprite(Ball::PowerupType effect, const std::string& picture);
		
		int linkController(const std::string& controllerName, const std::string& guyName);
		void unlink(const std::string& guyName);
		Controller* getController(const std::string& name);

		std::shared_ptr<RawTexture> queueText(const std::string& str);
		void renderText();

		void resetGame();

		inline TeamData* changeTeam(uint16_t team) { if (!this->teamData.count(team)) { return NULL; } return &this->teamData[team]; }

		void spawnFadeUp(uint16_t team, uint32_t duration);

		enum class GameState
		{
			STATE_PAUSED,
			STATE_WAITING_FOR_INPUT,
			STATE_RUNNING,
			STATE_SCORED,
		};
	//private:
		maf::ivec2 mousePos;
		bool lmb = false;
		bool rmb = false;
		bool seeBoxes = false;
		SpriteTexture traceEndTexture;
		GameState state = GameState::STATE_WAITING_FOR_INPUT;
		Renderer renderer;
		Map map;
		std::unordered_map<std::string, size_t> controllerMap;
		std::vector<Controller*> controllers;
		bool isRunning = false;
		std::unordered_map<uint16_t, TeamData> teamData;
		SDL_Color bg = { 110, 190, 255, 255 };
		uint32_t tick = 0;
		std::vector<std::string> effectSpritesByType;
		maf::ivec2 effectSpriteDim = { 32, 32 };
		ParticleManager particleManager;
		uint32_t scoredCooldown = 0;
		Sounds sound;
		std::vector<std::shared_ptr<RawTexture>> textToRender;
		float updateTime = 0;
		float renderTime = 0;
		float frameTime = 0;
		float max_updateTime = 0;
		float max_renderTime = 0;
		float remainingShake = 0;
		float shakeAmp = 0;
		uint32_t nextPowerupTick;
		Timer timer;
		std::unordered_map<std::string, Gun> guns;
	};
}
