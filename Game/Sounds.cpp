#include "Sounds.h"

#include "Debug/logtools.h"

vbl::Sounds::Sounds(int openChannels)
	:openChannels(openChannels), currentChannel(0)
{
	Mix_OpenAudio(48000, AUDIO_S16SYS, 1, 2048);
	Mix_AllocateChannels(openChannels);
}

void vbl::Sounds::setChannels(int amount)
{
	Mix_AllocateChannels(amount);
}

void vbl::Sounds::loadSound(const std::string& path, const std::string& name)
{
	Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
	if (!sound)
	{
		return;
	}
	SoundData dat{};
	dat.variations = 0;
	dat.id = this->currentID++;
	Mix_Chunk* alreadyThere = this->sounds[name];
	if (alreadyThere)
	{
		Mix_FreeChunk(alreadyThere);
	}
	this->sounds[name] = sound;
	this->soundData[name] = dat;
	this->idMap[dat.id] = name;
}

void vbl::Sounds::loadSound(const std::vector<std::string>& path, const std::string& name)
{
	DEBUG_LOG(std::format("Loading sound {} with {} variations.", name, path.size()));
	std::vector<Mix_Chunk*> chunks;
	chunks.reserve(path.size());
	for (int i = 0; i < path.size(); i++)
	{
		Mix_Chunk* sound = Mix_LoadWAV(path[i].c_str());
		if (!sound)
		{
			DEBUG_LOG(std::format("Loading sound {} variation {} failed, {}", name, path[i].c_str(), SDL_GetError()));
			continue;
		}
		chunks.push_back(sound);
	}
	if (!chunks.size())
	{
		DEBUG_LOG(std::format("Loading sound {} failed, no variations loaded.", name));
		return;
	}
	clearSound(name);
	SoundData dat{};
	dat.variations = chunks.size();
	dat.id = this->currentID++;
	DEBUG_LOG(std::format("Loading sound {} complete, only {}/{} variations loaded.", name, chunks.size(), path.size()));
	if (dat.variations == 1)
	{
		dat.variations = 0;
		this->soundData[name] = dat;
		this->idMap[dat.id] = name;
		this->sounds[name] = chunks[0];
		return;
	}
	for (int i = 0; i < chunks.size(); i++)
	{
		this->sounds[name + std::to_string(i)] = chunks[i];
	}
	this->soundData[name] = dat;
	this->idMap[dat.id] = name;
}

void vbl::Sounds::loadMusic(const std::string& path, const std::string& name)
{
	Mix_Music* music = Mix_LoadMUS(path.c_str());
	if (!music)
	{
		return;
	}
	if (this->music[name])
	{
		Mix_FreeMusic(this->music[name]);
	}
	this->music[name] = music;
}

void vbl::Sounds::takeSounds(std::vector<int>& sounds)
{
	for (int id : sounds)
	{
		playSound(id);
	}
	sounds.clear();
}

void vbl::Sounds::playSound(int id)
{
	if (!this->idMap.count(id))
	{
		DEBUG_LOG_F("SOUND PLAYBACK ERROR: no sound exists with id {}.", id);
		return;
	}
	this->playSound(this->idMap[id]);
}

void vbl::Sounds::playSound(const std::string& name)
{
	if (!this->soundData.count(name))
	{
		DEBUG_LOG_F("SOUND PLAYBACK ERROR: No sound exists with name {}.", name);
		return;
	}
	SoundData dat = this->soundData[name];
	Mix_Chunk* sound = NULL;
	if (dat.variations)
	{
		int var = rand() % dat.variations;
		sound = this->sounds[name + std::to_string(var)];
		DEBUG_LOG_F("Selected variation {}/{} for sound {}.", var, dat.variations, name);
	}
	else
	{
		sound = this->sounds[name];
	}
	if (Mix_PlayChannel(this->currentChannel, sound, 0) == -1)
	{
		DEBUG_LOG_F("SOUND PLAYBACK ERROR: SDL_Mixer cannot play sound {}.", name);
		return;
	}
	erasePlaying(this->currentChannel);
	this->playingSounds[name] = this->currentChannel;
	this->playingSoundsByChannel[this->currentChannel] = name;
	this->currentChannel++;
	if (this->currentChannel > this->openChannels)
	{
		this->currentChannel = 0;
	}
}

void vbl::Sounds::stopSound(const std::string& name)
{
	if (!this->playingSounds.count(name))
	{
		return;
	}
	Mix_HaltChannel(this->playingSounds[name]);
	erasePlaying(name);
}

void vbl::Sounds::playMusic(const std::string& name)
{
	Mix_PlayMusic(this->music[name], 0);
}

void vbl::Sounds::stopMusic()
{
	Mix_HaltMusic();
}

void vbl::Sounds::erasePlaying(int channel)
{
	if (!this->playingSoundsByChannel.count(channel))
	{
		return;
	}
	const std::string& name = this->playingSoundsByChannel[channel];
	this->playingSounds.erase(name);
	this->playingSoundsByChannel.erase(channel);
}

void vbl::Sounds::erasePlaying(const std::string& name)
{
	if (!this->playingSounds.count(name))
	{
		return;
	}
	int channel = this->playingSounds[name];
	this->playingSoundsByChannel.erase(channel);
	this->playingSounds.erase(name);
}

void vbl::Sounds::clearSound(const std::string& name)
{
	if (!this->soundData.count(name))
	{
		return;
	}
	SoundData dat = this->soundData[name];
	if (dat.variations)
	{
		for (int i = 0; i < dat.variations; i++)
		{
			Mix_FreeChunk(this->sounds[name + std::to_string(i)]);
			this->sounds.erase(name + std::to_string(i));
		}
		return;
	}
	Mix_FreeChunk(this->sounds[name]);
	this->sounds.erase(name);
	this->soundData.erase(name);
	this->idMap.erase(dat.id);
}

int vbl::Sounds::getID(const std::string& name)
{
	{ if (!soundData.count(name)) { return -1; } return soundData[name].id; }
}
