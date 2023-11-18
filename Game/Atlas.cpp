#include "Atlas.h"

#include <SDL_image.h>

#include "Debug/logtools.h"

Atlas::Atlas(int w, int h)
	:surf(NULL), texture(NULL), lastUsed(NULL)
{
	surf = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA8888);
	SDL_SetSurfaceBlendMode(this->surf, SDL_BLENDMODE_BLEND);
}

Atlas::~Atlas()
{
	SDL_FreeSurface(this->surf);
	if (this->texture)
	{
		SDL_DestroyTexture(this->texture);
	}
}

SDL_Texture* Atlas::getTexture(SDL_Renderer* renderer)
{
	if (!renderer)
	{
		return this->texture;
	}
	if (renderer != this->lastUsed)
	{
		if (this->texture)
		{
			SDL_DestroyTexture(this->texture);
		}
		this->texture = SDL_CreateTextureFromSurface(renderer, this->surf);
		SDL_SetTextureBlendMode(this->texture, SDL_BLENDMODE_BLEND);
		this->lastUsed = renderer;
	}
	return this->texture;
}

maf::ivec2 Atlas::findOrMakeScanline(maf::ivec2 dimensions)
{
	DEBUG_LOG_F("adding item with dimensions %i %i", dimensions.x, dimensions.y);
	if (dimensions.x > this->surf->w)
	{
		DEBUG_LOG_F("cannot create or get scanline for item %i wide, larger than surf width %i", dimensions.x, this->surf->w);
		return { 0,0 };
	}
	int currentHeight = 0;
	int currentY = 0;
	int nextLineY = 0;
	for (size_t i = 0; i < this->scanlineHeight.size(); i++)
	{
		int lineHeight = this->scanlineHeight[i];
		int currentX = this->scanlineCurrentX[i];
		currentHeight += lineHeight;
		currentY = currentHeight - lineHeight;
		nextLineY = currentY + lineHeight;
		DEBUG_LOG_F("current y is{}", currentY);
		if (dimensions.y > lineHeight)
		{
			DEBUG_LOG_F("height of {} is larger than line height of {}", dimensions.y, lineHeight);
			continue;
		}
		if (dimensions.y + currentY > this->surf->h || dimensions.y + currentY > nextLineY)
		{
			DEBUG_LOG_F("item with height of {} is too large, next scanline starts at {} and item ends at {}", dimensions.y, nextLineY, dimensions.y + currentY);
			continue;
		}
		if (dimensions.x + currentX > this->surf->w)
		{
			DEBUG_LOG_F("item with width of {} is too large when adding to scanline, combined width of {} vs surf width of {}", dimensions.x, dimensions.x + currentX, this->surf->w);
			continue;
		}
		this->scanlineCurrentX[i] += dimensions.x;
		return { currentX, currentY };
	}
	currentHeight = 0;
	for (size_t i = 0; i < this->scanlineHeight.size(); i++)
	{
		currentHeight += this->scanlineHeight[i];
	}
	if (currentHeight + dimensions.y > this->surf->h)
	{
		DEBUG_LOG(std::format("current height of {} is greater than surf height of {}, aborting (ABORT FINAL)", currentHeight + dimensions.y, this->surf->h));
		return { -1 };
	}
	DEBUG_LOG(std::format("adding new scanline with height of {}, current y will be {}, surf height is {}", dimensions.y, currentHeight + dimensions.y, this->surf->h));
	this->scanlineHeight.push_back(dimensions.y);
	this->scanlineCurrentX.push_back(0);
	this->scanlineCurrentX[scanlineCurrentX.size() - 1] += dimensions.x;
	return { 0, currentHeight };
}

std::vector<std::string> Atlas::addBulk(std::vector<std::string> paths, std::vector<std::string> names)
{
	std::vector<std::string> rejects;
	for (size_t i = 0; i < paths.size(); i++)
	{
		std::string path = paths[i];
		DEBUG_LOG(std::format("beginning loading {} into atlas.", path));
		std::string name = path;
		if (i < names.size())
		{
			name = names[i];
		}
		if (add(path, name))
		{
			LOG(std::format("Atlas rejected file {}.", path));
			rejects.push_back(path);
		}
	}
	return rejects;
}

int Atlas::add(std::string path, std::string name)
{
	if (this->indexMap.count(name))
	{
		return 4;
	}
	SDL_Surface* loaded = IMG_Load(path.c_str());
	if (!loaded)
	{
		return 1;
	}
	maf::ivec2 pos = this->findOrMakeScanline({ loaded->w, loaded->h });
	if (pos.x < 0 || pos.y < 0)
	{
		SDL_FreeSurface(loaded);
		return 3;
	}
	SDL_Rect clipping = { pos.x, pos.y, loaded->w, loaded->h };
	if (SDL_BlitScaled(loaded, NULL, this->surf, &clipping))
	{
		DEBUG_LOG(std::format("SDL error while blitting {} into atlas, {}", path, SDL_GetError()))
		SDL_FreeSurface(loaded);
		return 5;
	}
	DEBUG_LOG_F("ATLAS: added {} to the atlas. It's clipping rect is {},{} {}x{}.", name, clipping.x, clipping.y, clipping.w, clipping.h);
	this->clippingRects.push_back(clipping);
	this->indexMap[name] = this->clippingRects.size() - 1;
	return 0;
}


SDL_Rect Atlas::get(size_t i)
{
	if (i >= this->clippingRects.size() || i < 0)
	{
		DEBUG_LOG_F("No rect exists for id {}.", i);
		return { 0 };
	}
	DEBUG_LOG_F("ATLAS: getting rect for {}.", i);
	return this->clippingRects[i];
}

SDL_Rect Atlas::get(const std::string& name)
{
	if (!this->indexMap.count(name))
	{
		DEBUG_LOG_F("No rect exists with name {}.", name);
		return { 0 };
	}
	DEBUG_LOG_F("ATLAS: getting rect for {} and discarding it's id.", name);
	return this->clippingRects[this->indexMap[name]];
}

SDL_Rect Atlas::get(const std::string& name, size_t& i)
{
	if (!this->indexMap.count(name))
	{
		DEBUG_LOG_F("No rect exists with name {}.", name);
		return { 0 };
	}
	i = this->indexMap[name];
	DEBUG_LOG_F("ATLAS: getting rect for {} and caching id as {}.", name, i);
	return this->clippingRects[i];
}
