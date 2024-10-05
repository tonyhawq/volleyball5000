#include "Sprite.h"

#include "Atlas.h"

vbl::SpriteTexture::SpriteTexture(const IDedPicture& picture, SDL_Rect box, float rotation)
	:picture(picture), textureBox(box), rotation(rotation), spriteDimensions({box.w, box.h}), animState(0)
{

}

vbl::SpriteTexture::SpriteTexture(const IDedPicture& picture, SDL_Rect box, float rotation, maf::ivec2 spriteDimensions)
	:picture(picture), textureBox(box), rotation(rotation), spriteDimensions(spriteDimensions), animState(0)
{
	printf("%s was passed %s\n", __FUNCTION__, picture.picture.c_str());
}

vbl::SpriteTexture::~SpriteTexture()
{

}

void vbl::SpriteTexture::cache(const Atlas* atlas)
{
	if (!atlas)
	{
		return;
	}
	atlas->get(this->picture.picture, this->picture.id);
}

void vbl::Sprite::cacheTexture(const Atlas* atlas)
{
	this->texture.cache(atlas);
}

vbl::Sprite::Sprite(maf::fvec2 dimensions, const IDedPicture& picture, bool useDimensionsForBox)
	:texture(picture, {0, 0, (int)dimensions.x, (int)dimensions.y}, 0), itype(AType::Sprite)
{
	if (useDimensionsForBox)
	{
		this->box = MAABB({ { 0, 0, dimensions.x, dimensions.y} });
	}
	this->setPos({ 0,0 });
}

vbl::Sprite::Sprite(maf::fvec2 dimensions, const IDedPicture& picture, maf::ivec2 spriteDimensions, bool useDimensionsForBox)
	:texture(picture, {0,0,(int)dimensions.x,(int)dimensions.y}, 0, spriteDimensions), itype(AType::Sprite)
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

void vbl::Sprite::setVisMid(maf::fvec2 pos)
{
	this->setPos({
		pos.x - this->texture.dimensions().x / 2,
		pos.y - this->texture.dimensions().y / 2,
		});
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
