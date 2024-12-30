#pragma once
#include <cmath>
#include <SDL_rect.h>

namespace maf
{
	constexpr double pi = 3.14159265358979323846;
	constexpr double oneeighty_over_pi = 180 / pi;
	constexpr double pi_over_oneeighty = pi / 180;
	// EVERYTHING IS IN DEGREES. PLEASE
	
	// degrees
	constexpr double half_turn = 90;

	inline double radToDegrees(double rad) {
		return rad * oneeighty_over_pi;
	}

	inline double degreesToRad(double degrees) {
		return degrees * pi_over_oneeighty;
	}

	struct fvec2;

	struct ivec2
	{
		int x = 0, y = 0;
		ivec2 operator+ (const ivec2& other)
		{
			return { this->x + other.x, this->y + other.y };
		}
		operator fvec2();
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
		fvec2 operator/ (const float& other)
		{
			return { this->x / other, this->y / other };
		}
		fvec2 operator+ (const fvec2& other)
		{
			return { this->x + other.x, this->y + other.y };
		}
		fvec2 operator- (const fvec2& other)
		{
			return { this->x - other.x, this->y - other.y };
		}
		fvec2 operator- ()
		{
			return { -this->x, -this->y };
		}
	};
	struct irect
	{
		int x = 0, y = 0, w = 0, h = 0;
	};
	struct frect
	{
		float x = 0, y = 0, w = 0, h = 0;
		inline SDL_FRect SDL() const
		{
			return { x, y, w, h };
		};
		inline SDL_Rect SDLI() const
		{
			return { (int)x, (int)y, (int)w, (int)h };
		}
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
		return std::min(std::max(a, b), c);
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

	inline double pointTowardsRawRad(maf::fvec2 from, maf::fvec2 to)
	{
		return std::atan2(to.y - from.y, to.x - from.x);
	}

	inline double pointTowardsRad(maf::fvec2 from, maf::fvec2 to)
	{
		return (pointTowardsRawRad(from, to) + 1.57079632679);
	}

	inline fvec2 setMiddle(maf::fvec2 pos, maf::fvec2 dim)
	{
		return
		{
			pos.x - dim.x / 2,
			pos.y - dim.y / 2
		};
	}

	inline fvec2 rotatePointRad(maf::fvec2 point, float angle)
	{
		return { point.x * std::cos(angle) - point.y * std::sin(angle), point.y * std::cos(angle) + point.x * std::sin(angle) };
	}

	inline fvec2 rotatePointRad(maf::fvec2 point, maf::fvec2 about, float angle)
	{
		return rotatePointRad(point, angle) + about;
	}
}