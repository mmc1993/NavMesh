#pragma once

#include "../base.h"
#include <ctype.h>

#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max

namespace math {

class Vec2 {
public:
	static Vec2 s_ZERO;

public:
	Vec2(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y)
	{ }

	bool operator==(const Vec2 & other) const
	{
		return std::abs(x - other.x) < FLT_EPSILON
			&& std::abs(y - other.y) < FLT_EPSILON;
	}

	bool operator!=(const Vec2 & other) const
	{
		return !operator==(other);
	}

	Vec2 operator+(const Vec2 & other) const
	{
		return { x + other.x, y + other.y };
	}

	Vec2 operator-(const Vec2 & other) const
	{
		return { x - other.x, y - other.y };
	}

	Vec2 operator*(const Vec2 & other) const
	{
		return { x * other.x, y * other.y };
	}
	void Func();
	Vec2 operator*(float other) const
	{
		return { x * other, y * other };
	}

	Vec2 operator/(const Vec2 & other) const
	{
		return { x / other.x, y / other.y };
	}

	Vec2 operator/(float other) const
	{
		return { x / other, y / other };
	}

	float Cross(const Vec2 & other) const
	{
		return x * other.y - y * other.x;
	}

	float Dot(const Vec2 & other) const
	{
		return x * other.x + y * other.y;
	}

	float LengthSqrt() const
	{
		return Dot(*this);
	}

	float Length() const
	{
		return std::sqrtf(LengthSqrt());
	}

	Vec2 Normal() const
	{
		auto length = Length();
		if (0.0f != length)
		{
			auto s = 1.0f / length;
			return { x * s, y * s };
		}
		return {};
	}

	float x, y;
};

inline bool IsPointOnline(const Vec2 & pt1, const Vec2 & pt2, const Vec2 & pt)
{
	return pt.x >= std::min(pt1.x, pt2.x) && pt.x <= std::max(pt1.x, pt2.x)
		&& pt.y >= std::min(pt1.y, pt2.y) && pt.y <= std::max(pt1.y, pt2.y)
		&& std::abs((pt - pt1).Cross(pt2 - pt)) < FLT_EPSILON;
}

inline bool IsPointInTriangle(const Vec2 & pt1, const Vec2 & pt2, const Vec2 & pt3, const Vec2 & pt)
{
	auto a = (pt2 - pt1).Cross(pt - pt1);
	auto b = (pt3 - pt2).Cross(pt - pt2);
	auto c = (pt1 - pt3).Cross(pt - pt3);
	return a * b >= 0 && b * c >= 0;
}

inline bool IsSegmentIntersect(const Vec2 & pt1, const Vec2 & pt2, const Vec2 & pt3, const Vec2 & pt4)
{
	return !(std::max(pt1.x, pt2.x) < std::min(pt3.x, pt4.x))
		&& !(std::min(pt1.x, pt2.x) > std::max(pt3.x, pt4.x))
		&& !(std::max(pt1.y, pt2.y) < std::min(pt3.y, pt4.y))
		&& !(std::min(pt1.y, pt2.y) > std::max(pt3.y, pt4.y))
		&& (pt2 - pt1).Cross(pt3 - pt1) * (pt2 - pt1).Cross(pt4 - pt1) >= 0.0f;
}

inline bool IsSegmentIntersectTriangle(const Vec2 & a, const Vec2 & b, const Vec2 & pt1, const Vec2 & pt2, const Vec2 & pt3)
{
	return IsSegmentIntersect(a, b, pt1, pt2)
		|| IsSegmentIntersect(a, b, pt2, pt3)
		|| IsSegmentIntersect(a, b, pt3, pt1);
}

inline std::tuple<bool, float, Vec2> Circumcircle(const Vec2 & pt1, const Vec2 & pt2, const Vec2 & pt3)
{
	if ((pt2 - pt1).Cross(pt3 - pt2) == 0.0f)
	{
		return { false, 0.0f, Vec2::s_ZERO };
	}
	auto t1 = pt1.Dot(pt1);
	auto t2 = pt2.Dot(pt2);
	auto t3 = pt3.Dot(pt3);
	auto tmp = pt1.x*pt2.y + pt2.x*pt3.y + pt3.x*pt1.y - pt1.x*pt3.y - pt2.x*pt1.y - pt3.x*pt2.y;
	auto c = Vec2(
		(t2*pt3.y + t1*pt2.y + t3*pt1.y - t2*pt1.y - t3*pt2.y - t1*pt3.y) / tmp / 2,
		(t3*pt2.x + t2*pt1.x + t1*pt3.x - t1*pt2.x - t2*pt3.x - t3*pt1.x) / tmp / 2
	);
	auto r = (pt1 - c).Length();
	return { true, r, c };
}

}

#pragma pop_macro("min")
#pragma pop_macro("max")