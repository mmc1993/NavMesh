#include "mesher.h"
#include <tuple>

void Mesher::AppendVertex(const math::Vec2 & vertex)
{
	auto iter = std::find(_vertexs.begin(), _vertexs.end(), vertex);
	if (iter == _vertexs.end())
	{
		UpdateVertex(vertex);
	}
}

void Mesher::RemoveVertex(const math::Vec2 & vertex)
{
	if (_helper.end() == std::find(_helper.begin(), _helper.end(), vertex))
	{
		auto iter = std::remove(_vertexs.begin(), _vertexs.end(), vertex);
		if (iter != _vertexs.end())
		{
			_vertexs.erase(iter);
			auto helper = std::move(_helper);
			auto vertexs = decltype(_vertexs)(
				_vertexs.begin() + 4,
				_vertexs.end());

			_triangles.clear();
			_vertexs.clear();
			_lines.clear();
			SetHelperVertex(helper);
			std::for_each(vertexs.begin(), vertexs.end(), BIND_1(Mesher::AppendVertex, this));
		}
	}
}

void Mesher::SetHelperVertex(const std::array<math::Vec2, 4> & vertexs)
{
	std::copy(vertexs.begin(), vertexs.end(), std::back_inserter(_vertexs));
	_triangles.emplace_back(vertexs.at(0), vertexs.at(1), vertexs.at(2));
	_triangles.emplace_back(vertexs.at(0), vertexs.at(2), vertexs.at(3));
	_lines.emplace_back(vertexs.at(0), vertexs.at(1));
	_lines.emplace_back(vertexs.at(0), vertexs.at(2));
	_lines.emplace_back(vertexs.at(0), vertexs.at(3));
	_lines.emplace_back(vertexs.at(2), vertexs.at(1));
	_lines.emplace_back(vertexs.at(2), vertexs.at(3));
	_helper = vertexs;
}

void Mesher::UpdateVertex(const math::Vec2 & vertex)
{
	std::vector<math::Triangle> commonTriangles;
	for (const auto & triangle: _triangles)
	{
		auto ret = math::Circumcircle(triangle.pt1, triangle.pt2, triangle.pt3);
		if (ret.has_value())
		{
			if (ret.value().first >= (vertex - ret.value().second).Length())
			{
				commonTriangles.push_back(triangle);
			}
		}
	}

	if (commonTriangles.size() == 1)
	{
		const auto & triangle = commonTriangles.at(0);
		RemoveFromVector(_triangles, triangle);
		AppendToVector(_lines, { vertex, triangle.pt1 });
		AppendToVector(_lines, { vertex, triangle.pt2 });
		AppendToVector(_lines, { vertex, triangle.pt3 });
		auto triangle1 = math::Triangle(vertex, triangle.pt1, triangle.pt2);
		auto triangle2 = math::Triangle(vertex, triangle.pt2, triangle.pt3);
		auto triangle3 = math::Triangle(vertex, triangle.pt3, triangle.pt1);
		if (IsExistsTriangle(triangle1))
		{
			AppendToVector(_triangles, triangle1);
		}
		if (IsExistsTriangle(triangle2))
		{
			AppendToVector(_triangles, triangle2);
		}
		if (IsExistsTriangle(triangle3))
		{
			AppendToVector(_triangles, triangle3);
		}
	}
	else if (commonTriangles.size() > 1)
	{
		math::Line commonLine;
		for (auto i = 0; i != commonTriangles.size() - 1; ++i)
		{
			for (auto j = i + 1; j != commonTriangles.size(); ++j)
			{
				const auto & triangle1 = commonTriangles.at(i);
				const auto & triangle2 = commonTriangles.at(j);
				if (triangle1.QueryCommonLine(triangle2, &commonLine))
				{
					RemoveFromVector(_lines, commonLine);
					RemoveFromVector(_triangles, [&commonLine](const math::Triangle & triangle) 
						{ 
							return triangle.IsExistsLine(commonLine);
						});

					AppendToVector(_lines, { vertex, triangle1.pt1 });
					AppendToVector(_lines, { vertex, triangle1.pt2 });
					AppendToVector(_lines, { vertex, triangle1.pt3 });
					AppendToVector(_lines, { vertex, triangle2.pt1 });
					AppendToVector(_lines, { vertex, triangle2.pt2 });
					AppendToVector(_lines, { vertex, triangle2.pt3 });

					auto newTriangle1 = math::Triangle(vertex, triangle1.pt1, triangle1.pt2);
					auto newTriangle2 = math::Triangle(vertex, triangle1.pt2, triangle1.pt3);
					auto newTriangle3 = math::Triangle(vertex, triangle1.pt3, triangle1.pt1);
					auto newTriangle4 = math::Triangle(vertex, triangle2.pt1, triangle2.pt2);
					auto newTriangle5 = math::Triangle(vertex, triangle2.pt2, triangle2.pt3);
					auto newTriangle6 = math::Triangle(vertex, triangle2.pt3, triangle2.pt1);
					if (IsExistsTriangle(newTriangle1)) { AppendToVector(_triangles, newTriangle1); }
					if (IsExistsTriangle(newTriangle2)) { AppendToVector(_triangles, newTriangle2); }
					if (IsExistsTriangle(newTriangle3)) { AppendToVector(_triangles, newTriangle3); }
					if (IsExistsTriangle(newTriangle4)) { AppendToVector(_triangles, newTriangle4); }
					if (IsExistsTriangle(newTriangle5)) { AppendToVector(_triangles, newTriangle5); }
					if (IsExistsTriangle(newTriangle6)) { AppendToVector(_triangles, newTriangle6); }
				}
			}
		}
	}
	AppendToVector(_vertexs, vertex);
}

bool Mesher::IsExistsTriangle(const math::Triangle & triangle)
{
	auto line1 = math::Line(triangle.pt1, triangle.pt2);
	auto line2 = math::Line(triangle.pt2, triangle.pt3);
	auto line3 = math::Line(triangle.pt3, triangle.pt1);
	auto count = 0;
	for (const auto & line : _lines)
	{
		if (line1 == line) count |= 0x1;
		if (line2 == line) count |= 0x2;
		if (line3 == line) count |= 0x4;
	}
	return count == (0x1 | 0x2 | 0x4);
}
