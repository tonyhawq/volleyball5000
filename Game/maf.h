#pragma once
#include <cmath>

namespace maf
{
	constexpr double pi = 3.14159265358979323846;
	constexpr double oneeighty_over_pi = 180 / pi;
	constexpr double pi_over_oneeighty = pi / 180;

	inline double radToDegrees(double rad) {
		return rad * oneeighty_over_pi;
	}

	inline double degreesToRad(double degrees) {
		return degrees * pi_over_oneeighty;
	}

	struct ivec2
	{
		int x = 0, y = 0;
		//operator fvec2()
		//{
		//	fvec2 p;
		//	p.x = this->x;
		//	p.y = this->y;
		//	return p;
		//}
		ivec2 operator+ (const ivec2& other)
		{
			return { this->x + other.x, this->y + other.y };
		}
	};
	struct fvec2
	{
		float x = 0, y = 0;
		operator ivec2()
		{
			ivec2 p;
			p.x = (int)this->x;
			p.y = (int)this->y;
			return p;
		}
		fvec2 operator+ (const fvec2& other)
		{
			return { this->x + other.x, this->y + other.y };
		}
	};
	struct irect
	{
		int x = 0, y = 0, w = 0, h = 0;
	};
	struct frect
	{
		float x = 0, y = 0, w = 0, h = 0;
	};

	/// <summary>
	/// clamp A between B min and C max
	/// </summary>
	inline int clamp(int a, int b, int c)
	{
		return std::max(std::min(a, b), c);
	}
	
	inline float clamp(float a, float b, float c)
	{
		return std::max(std::min(a, b), c);
	}

	inline int sign(int i)
	{
		return clamp(i, -1, 1);
	}

	inline float sign(float i)
	{
		return (float)ceil(clamp(i, -1.0f, 1.0f));
	}
	
	inline bool collides(irect a, irect b)
	{
		if (
			a.x < b.x + b.w &&
			a.x + a.w > b.x &&
			a.y < b.y + b.h &&
			a.y + a.h > b.y
			)
		{
			return true;
		}
		return false;
	}
	inline bool collides(frect a, frect b)
	{
		if (
			a.x < b.x + b.w &&
			a.x + a.w > b.x &&
			a.y < b.y + b.h &&
			a.y + a.h > b.y
			)
		{
			return true;
		}
		return false;
	}

	inline ivec2 makeBinary(ivec2 in)
	{
		return { sign(in.x), sign(in.y) };
	}

	inline double pointTowards(maf::fvec2 from, maf::fvec2 to)
	{
		return (std::atan2(to.y - from.y, to.x - from.x) + 1.57079632679);
	}

	inline fvec2 setMiddle(maf::fvec2 pos, maf::fvec2 dim)
	{
		return
		{
			pos.x - dim.x / 2,
			pos.y - dim.y / 2
		};
	}
}