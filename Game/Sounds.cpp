#include "Sounds.h"

vbl::Sounds::Sounds(int openChannels)
	:openChannels(openChannels), currentChannel(0)
{
	Mix_OpenAudio(48000, AUDIO_S16SYS, 1, 2048);
	Mix_AllocateChannels(openChannels);
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
	printf("begin loading sound %s\n", name.c_str());
	std::vector<Mix_Chunk*> chunks;
	chunks.reserve(path.size());
	for (int i = 0; i < path.size(); i++)
	{
		Mix_Chunk* sound = Mix_LoadWAV(path[i].c_str());
		if (!sound)
		{
			printf("loading %s failed, %s\n", path[i].c_str(), Mix_GetError());
			continue;
		}
		chunks.push_back(sound);
	}
	if (!chunks.size())
	{
		printf("loading %s failed, no sounds loaded\n", name.c_str());
		return;
	}
	clearSound(name);
	SoundData dat{};
	dat.variations = chunks.size();
	dat.id = this->currentID++;
	if (dat.variations == 1)
	{
		dat.variations = 0;
		this->soundData[name] = dat;
		this->idMap[dat.id] = name;
		this->sounds[name] = chunks[0];
		printf("loading %s complete, only one sound loaded\n", name.c_str());
		return;
	}
	for (int i = 0; i < chunks.size(); i++)
	{
		this->sounds[name + std::to_string(i)] = chunks[i];
	}
	this->soundData[name] = dat;
	this->idMap[dat.id] = name;
	printf("loaded sound %s\n", name.c_str());
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
		printf("SOUND ERROR: no sound with id %i\n", id);
		return;
	}
	this->playSound(this->idMap[id]);
}

void vbl::Sounds::playSound(const std::string& name)
{
	printf("playing sound %s on channel %i\n", name.c_str(), currentChannel);
	if (!this->soundData.count(name))
	{
		printf("no sound with name %s\n", name.c_str());
		return;
	}
	SoundData dat = this->soundData[name];
	Mix_Chunk* sound = NULL;
	if (dat.variations)
	{
		int var = rand() % dat.variations;
		sound = this->sounds[name + std::to_string(var)];
	}
	else
	{
		sound = this->sounds[name];
	}
	printf("sound %s has adress %p\n", name.c_str(), sound);
	if (Mix_PlayChannel(this->currentChannel, sound, 0) == -1)
	{
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
