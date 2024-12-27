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

bool vbl::Geometry::collidesNotriggerTeamed(const MAABB& other, uint16_t team) const
{
	for (const auto& a : boxes)
	{
		if (a.trigger)
		{
			continue;
		}
		if (a.team && a.team != team)
		{
			continue;
		}
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

bool vbl::Geometry::collidesNotrigger(const MAABB& other) const
{
	return collidesNotriggerTeamed(other, 0);
}

std::vector<uint32_t> vbl::Geometry::collidesWithRes(const MAABB& other) const
{
	std::vector<uint32_t> indicies;
	indicies.reserve(this->boxes.size());
	const std::vector<maf::frect>& boxes = other.getBoxes();
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

void vbl::Geometry::collidesWithResNoalloc(const MAABB& other, std::vector<uint32_t>& vec) const
{
	vec.clear();
	if (vec.capacity() < this->boxes.size())
	{
		vec.reserve(this->boxes.size());
	}
	const std::vector<maf::frect>& boxes = other.getBoxes();
	for (uint32_t this_i = 0; this_i < this->boxes.size(); this_i++)
	{
		for (uint32_t other_i = 0; other_i < boxes.size(); other_i++)
		{
			if (maf::collides(this->boxes[this_i].box, boxes[other_i]))
			{
				vec.push_back(this_i);
			}
		}
	}
}

const std::vector<uint32_t>& vbl::Geometry::collidesWithResSelfNoalloc(const MAABB& other)
{
	collidesWithResNoalloc(other, this->cached_indicies);
	return this->cached_indicies;
}
