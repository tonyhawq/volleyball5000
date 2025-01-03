#pragma once

#include <SDL.h>
#include <vector>
#include <string>
#include <unordered_map>

#include "maf.h"

typedef std::pair<std::string, std::string> pair_str_t;

struct IDedPicture
{
	IDedPicture(const std::string& picture, size_t id);
	IDedPicture(const std::string& picture);
	IDedPicture(const char* c_str);
	IDedPicture();
	IDedPicture(const IDedPicture& other);
	IDedPicture(IDedPicture&& other) noexcept;
	IDedPicture& operator=(const IDedPicture&) = default;
	std::string picture;
	size_t id;
};

typedef IDedPicture Picture;

class Atlas
{
public:
	Atlas(int w, int h);
	~Atlas();
	Atlas(Atlas& t) = delete;

	maf::ivec2 findOrMakeScanline(maf::ivec2 dimensions);

	/// <returns>returns the images that could not be added (lack of space, invalid paths)</returns>
	std::vector<std::string> addBulk(std::vector<pair_str_t> paths);

	/// <returns>0: sucess, 1: invalid image, 2: too large, 3: no scanline, 4: name taken, 5: SDL error</returns>
	int add(std::string path, std::string name);

	SDL_Rect get(size_t i) const;
	SDL_Rect get(const std::string& name) const;
	SDL_Rect get(const std::string& name, size_t& i) const;

	IDedPicture create(const std::string& name);

	SDL_Texture* getTexture(SDL_Renderer* renderer);

	static size_t INVALID_CACHED;
private:
	std::vector<int> scanlineHeight;
	std::vector<int> scanlineCurrentX;
	std::vector<SDL_Rect> clippingRects;
	std::unordered_map<std::string, size_t> indexMap;
	SDL_Texture* texture;
	SDL_Surface* surf;
	SDL_Renderer* lastUsed;
};
