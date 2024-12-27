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

		void empty();

		MAABB getBox() const;
		inline const std::vector<GeometryBox>& getBoxes() const { return this->boxes; }
		inline const std::vector<ColoredRect>& getRects() const { return this->rects; }

		inline void add(GeometryBox box) { this->boxes.push_back(box); }
		inline void add(ColoredRect rect) { this->rects.push_back(rect); }

		inline const GeometryBox* get(uint32_t i) const { if (i >= this->boxes.size()) { return NULL; } return &this->boxes[i]; }

		bool collides(const MAABB& other) const;
		bool collidesNotrigger(const MAABB& other) const;
		std::vector<uint32_t> collidesWithIndicies(const MAABB& other) const;
		std::vector<vbl::GeometryBox> collidesWithBoxes(const MAABB& other) const;
	private:
		std::vector<GeometryBox> boxes;
		std::vector<ColoredRect> rects;
	};
}
