#include "Sprite.h"


vbl::SpriteTexture::SpriteTexture(SDL_Texture* texture, SDL_Rect box, float rotation)
	:texture(texture), textureBox(box), rotation(rotation)
{
	
}

vbl::SpriteTexture::SpriteTexture()
	:texture(NULL), textureBox({0,0,0,0}), rotation(0)
{

}

vbl::Sprite::Sprite(maf::fvec2 dimensions, SDL_Texture* tex, bool useDimensionsForBox)
{
	if (useDimensionsForBox)
	{
		this->box = MAABB({ { 0, 0, dimensions.x, dimensions.y} });
	}
	this->texture.resize({(int)dimensions.x, (int)dimensions.y});
	this->texture.setTexture(tex);
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
