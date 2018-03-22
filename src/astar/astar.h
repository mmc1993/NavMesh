#pragma once

#include "../base.h"
#include "../math/math.h"
#include "heap_queue.h"

class AStar {
public:
	struct Mesh {
		std::uint16_t ID;
		math::Vec2 center;
		math::Triangle tri;
		std::vector<std::uint16_t> links;
	};

	struct WayPoint {
		WayPoint(std::uint16_t id, std::uint16_t p, std::uint32_t g, std::uint32_t h)
			: ID(id), G(g), H(h), parent(p)
		{ }

		std::uint16_t F() const
		{
			return G + H;
		}

		bool operator==(const WayPoint & wp) const
		{
			return F() == wp.F();
		}

		bool operator!=(const WayPoint & wp) const
		{
			return !(*this == wp);
		}

		bool operator<(const WayPoint & wp) const
		{
			return F() > wp.F();
		}

		bool operator>(const WayPoint & wp) const
		{
			return F() < wp.F();
		}

		std::uint32_t G, H;
		std::uint16_t ID, parent;
	};

public:
	AStar();
	~AStar();

	bool InitFromFile(const std::string & fname);
	bool GetPath(std::uint16_t startID, const math::Vec2 & startpt, 
				 std::uint16_t endID, const math::Vec2 & endpt, 
				 std::vector<std::uint16_t> & navmesh,
				 std::vector<math::Vec2> & waypoints);
	std::uint16_t GetMeshID(const math::Vec2 & pt) const;
	const std::vector<Mesh> & GetMeshs() const;

private:
	bool IsNewWayPoint(std::uint16_t id) const;
	std::uint32_t Distance(const math::Vec2 & a, const math::Vec2 & b) const;
	bool GetNavMesh(const WayPoint & endWayPoint, std::vector<std::uint16_t> & navmesh);
	void GetWayPoints(const math::Vec2 & startpt, const math::Vec2 & endpt, 
					  const std::vector<std::uint16_t> & navmesh, 
					  std::vector<math::Vec2> & waypoints);
	std::tuple<const math::Vec2 &, const math::Vec2 &> GetLinkLine(const Mesh & mesh1, const Mesh & mesh2) const;

private:
	std::vector<Mesh> _meshs;
	HeapQueue<WayPoint> _opens;
	std::vector<WayPoint> _closes;
};