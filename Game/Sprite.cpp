#include "Sprite.h"

size_t vbl::SpriteTexture::INVALID_CACHED = SIZE_MAX;

vbl::SpriteTexture::SpriteTexture(const std::string& picture, SDL_Rect box, float rotation)
	:picture(picture), textureBox(box), rotation(rotation), spriteDimensions({box.w, box.h}), animState(0)
{

}

vbl::SpriteTexture::SpriteTexture()
	:picture("NO_ASSIGN"), textureBox({0,0,0,0}), rotation(0), animState(0)
{

}

vbl::SpriteTexture::SpriteTexture(const std::string& picture, SDL_Rect box, float rotation, maf::ivec2 spriteDimensions)
	:picture(picture), textureBox(box), rotation(rotation), spriteDimensions(spriteDimensions), animState(0)
{
	
}

vbl::SpriteTexture::~SpriteTexture()
{

}

vbl::Sprite::Sprite(maf::fvec2 dimensions, const std::string& picture, bool useDimensionsForBox)
	:texture(picture, {0, 0, (int)dimensions.x, (int)dimensions.y}, 0)
{
	if (useDimensionsForBox)
	{
		this->box = MAABB({ { 0, 0, dimensions.x, dimensions.y} });
	}
	this->setPos({ 0,0 });
}

vbl::Sprite::Sprite(maf::fvec2 dimensions, const std::string& picture, maf::ivec2 spriteDimensions, bool useDimensionsForBox)
	:texture(picture, {0,0,(int)dimensions.x,(int)dimensions.y}, 0, spriteDimensions)
{
	if (useDimensionsForBox)
	{
		this->box = MAABB({ {0,0,dimensions.x, dimensions.y} });
	}
	this->setPos({ 0,0 });
}

void vbl::Sprite::move(maf::fvec2 much)
{
	this->pos = this->pos + much;
	this->box.setPos(this->pos);
	this->texture.setPos(this->pos);
}

void vbl::Sprite::setPos(maf::fvec2 pos)
{
	this->pos = pos;
	this->box.setPos(pos);
	this->texture.setPos(pos);
}

void vbl::MAABB::move(maf::fvec2 vel)
{
	for (auto& box : boxes)
	{
		box.x += vel.x;
		box.y += vel.y;
	}
	this->pos = this->pos + vel;
}

void vbl::MAABB::setPos(maf::fvec2 pos)
{
	this->move({ pos.x - this->pos.x, pos.y - this->pos.y });
}

vbl::MAABB::MAABB(const std::vector<maf::frect>& rects)
{
	boxes = rects;
}

vbl::MAABB::MAABB(maf::frect rect)
{
	boxes = { rect };
}

vbl::MAABB::MAABB(maf::fvec2 pos, float resolution)
{
	boxes = { {pos.x - resolution / 2, pos.y - resolution / 2, resolution, resolution} };
}

vbl::MAABB::MAABB() {}

void vbl::MAABB::add(maf::frect box)
{
	boxes.push_back(box);
	recalculateDimensions();
}

void vbl::MAABB::recalculateDimensions()
{
	this->dimensions = { 0,0 };
	for (const auto& box : boxes)
	{
		if (box.x + box.w - this->pos.x > this->dimensions.x)
		{
			this->dimensions.x = box.x + box.w - this->pos.x;
		}
		if (box.y + box.h - this->pos.y > this->dimensions.y)
		{
			this->dimensions.y = box.y + box.h - this->pos.y;
		}
	}
}

bool vbl::MAABB::collide(const MAABB& one, const MAABB& two)
{
	for (const auto& a : one.boxes)
	{
		for (const auto& b : two.boxes)
		{
			if (maf::collides(a, b))
			{
				return true;
			}
		}
	}
	return false;
}

bool vbl::MAABB::collide(const MAABB& other) const
{
	return collide(*this, other);
}
