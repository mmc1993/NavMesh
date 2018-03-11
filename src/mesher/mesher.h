#pragma once

#include "../base.h"
#include "../math/math.h"

class Mesher {
public:
	class Line {
	public:
		Line(const math::Vec2 & _pt1 = math::Vec2::s_ZERO,
			const math::Vec2 & _pt2 = math::Vec2::s_ZERO);
		bool operator==(const Line & other) const;
		bool operator!=(const Line & other) const;

		math::Vec2 pt1, pt2;
	};

	class Triangle {
	public:
		Triangle(const math::Vec2 & _pt1 = math::Vec2::s_ZERO,
			const math::Vec2 & _pt2 = math::Vec2::s_ZERO,
			const math::Vec2 & _pt3 = math::Vec2::s_ZERO);
		math::Vec2 GetCenterPoint() const;
		bool IsExistsLine(const Line & line) const;
		bool operator==(const Triangle & other) const;
		bool operator!=(const Triangle & other) const;
		bool QueryCommonLine(const Triangle & triangle, Line * out = nullptr) const;

		math::Vec2 pt1, pt2, pt3;
	};

public:
	void AppendVertex(const math::Vec2 & vertex);
	void RemoveVertex(const math::Vec2 & vertex);
	void SetHelperVertex(const std::array<math::Vec2, 4> & vertexs);

	const std::vector<math::Vec2> & GetVertexs() { return _vertexs; }
	const std::vector<Triangle> & GetTriangles() { return _triangles; }
	void Clear() { _lines.clear(); _vertexs.clear(); _triangles.clear(); }

private:
	void UpdateVertex(const math::Vec2 & vertex);
	bool IsExistsTriangle(const Triangle & triangle);
	template <class T>
	void RemoveFromVector(std::vector<T> & container, const T & value);
	template <class T, class Pred>
	void RemoveFromVector(std::vector<T> & container, const Pred & pred);
	template <class T>
	void AppendToVector(std::vector<T> & container, const T & value);

private:
	std::array<math::Vec2, 4> _helper;
	std::vector<math::Vec2> _vertexs;
	std::vector<Triangle> _triangles;
	std::vector<Line> _lines;
};

template<class T>
inline void Mesher::RemoveFromVector(std::vector<T>& container, const T & value)
{
	auto iter = std::remove(container.begin(), container.end(), value);
	if (iter != container.end())
	{
		container.erase(iter);
	}
}

template<class T, class Pred>
inline void Mesher::RemoveFromVector(std::vector<T>& container, const Pred & pred)
{
	container.erase(std::remove_if(container.begin(), container.end(), pred), container.end());
}

template<class T>
inline void Mesher::AppendToVector(std::vector<T>& container, const T & value)
{
	auto iter = std::find(container.begin(), container.end(), value);
	if (iter == container.end())
	{
		container.push_back(value);
	}
}
