#pragma once

#include "Sprite.h"

namespace vbl
{
	struct GeometryBox
	{
		maf::frect box;
		uint16_t team;
		bool trigger;
	};

	struct ColoredRect
	{
		maf::frect box;
		SDL_Color clr;
	};

	class Geometry
	{
	public:
		Geometry(const std::vector<GeometryBox>& boxes, const std::vector<ColoredRect>& rects);
		Geometry(MAABB box);
		Geometry() = default;

		MAABB getBox() const;
		inline const std::vector<GeometryBox>& getBoxes() const { return this->boxes; }
		inline const std::vector<ColoredRect>& getRects() const { return this->rects; }

		inline const GeometryBox* get(uint32_t i) const { if (i >= this->boxes.size()) { return NULL; } return &this->boxes[i]; }

		bool collides(const MAABB& other) const;
		std::vector<uint32_t> collidesWithRes(const MAABB& other) const;
	private:
		std::vector<GeometryBox> boxes;
		std::vector<ColoredRect> rects;
	};
}
