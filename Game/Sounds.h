#pragma once

#include <unordered_map>
#include <SDL_mixer.h>
#include <string>

namespace vbl
{
	struct SoundData
	{
		int variations;
		int id;
	};
	class Sounds
	{
	public:
		Sounds(int openChannels);

		void setChannels(int amount);

		void loadSound(const std::string& path, const std::string& name);
		void loadSound(const std::vector<std::string>& path, const std::string& name);
		void loadMusic(const std::string& path, const std::string& name);

		void takeSounds(std::vector<int>& sounds);

		void playSound(int id);
		void playSound(const std::string& name);
		void stopSound(const std::string& name);
		void playMusic(const std::string& name);
		void stopMusic();

		void erasePlaying(int channel);
		void erasePlaying(const std::string& name);
		void clearSound(const std::string& name);
		int getID(const std::string& name);
	private:
		int currentID = 0;
		std::unordered_map<int, std::string> idMap;
		std::unordered_map<std::string, SoundData> soundData;
		std::unordered_map<std::string, Mix_Chunk*> sounds;
		std::unordered_map<std::string, int> playingSounds;
		std::unordered_map<int, std::string> playingSoundsByChannel;
		int openChannels;
		int currentChannel;
		std::unordered_map<std::string, Mix_Music*> music;
	};
}
