#include "Geometry.h"

void vbl::Geometry::empty()
{
	this->boxes.clear();
	this->rects.clear();
}

vbl::Geometry::Geometry(MAABB box)
{
	this->boxes.reserve(box.getBoxes().capacity());
	for (const auto& rect : box.getBoxes())
	{
		this->boxes.push_back({ rect, 0, false });
	}
}

vbl::Geometry::Geometry(const std::vector<GeometryBox>& boxes, const std::vector<ColoredRect>& rects)
{
	this->boxes = boxes;
	this->rects = rects;
}

vbl::MAABB vbl::Geometry::getBox() const
{
	std::vector<maf::frect> mBoxes;
	mBoxes.reserve(this->boxes.capacity());
	for (const auto& rect : this->boxes)
	{
		mBoxes.push_back(rect.box);
	}
	return MAABB(mBoxes);
}

bool vbl::Geometry::collides(const MAABB& other) const
{
	for (const auto& a : boxes)
	{
		for (const auto& b : other.getBoxes())
		{
			if (maf::collides(a.box, b))
			{
				return true;
			}
		}
	}
	return false;
}

std::vector<uint32_t> vbl::Geometry::collidesWithRes(const MAABB& other) const
{
	std::vector<uint32_t> indicies;
	indicies.reserve(this->boxes.size());
	const std::vector<maf::frect> boxes = other.getBoxes();
	for (uint32_t i = 0; i < this->boxes.size(); i++)
	{
		for (uint32_t j = 0; j < boxes.size(); j++)
		{
			if (maf::collides(this->boxes[i].box, boxes[j]))
			{
				indicies.push_back(i);
			}
		}
	}
	return indicies;
}
