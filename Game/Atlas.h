#pragma once

#include <SDL.h>
#include <vector>
#include <string>
#include <unordered_map>

#include "maf.h"

class Atlas
{
public:
	Atlas(int w, int h);
	~Atlas();
	Atlas(Atlas& t) = delete;

	maf::ivec2 findOrMakeScanline(maf::ivec2 dimensions);

	/// <returns>returns the images that could not be added (lack of space, invalid paths)</returns>
	std::vector<std::string> addBulk(std::vector<std::string> paths, std::vector<std::string> names);

	/// <returns>0: sucess, 1: invalid image, 2: too large, 3: no scanline, 4: name taken, 5: SDL error</returns>
	int add(std::string path, std::string name);

	SDL_Rect get(size_t i);
	SDL_Rect get(const std::string& name);
	SDL_Rect get(const std::string& name, size_t& i);

	SDL_Texture* getTexture(SDL_Renderer* renderer);
private:
	std::vector<int> scanlineHeight;
	std::vector<int> scanlineCurrentX;
	std::vector<SDL_Rect> clippingRects;
	std::unordered_map<std::string, size_t> indexMap;
	SDL_Texture* texture;
	SDL_Surface* surf;
	SDL_Renderer* lastUsed;
};
