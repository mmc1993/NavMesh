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
		WayPoint(std::uint16_t id, std::uint16_t p, std::uint16_t g, std::uint16_t h)
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

		std::uint16_t G, H;
		std::uint16_t ID, parent;
	};

public:
	AStar();
	~AStar();

	bool InitFromFile(const std::string & fname);
	bool GetPath(std::uint16_t startID, const math::Vec2 & startpt, 
				 std::uint16_t endID, const math::Vec2 & endpt, 
				 std::vector<math::Vec2> & waypoints);
	std::uint16_t GetMeshID(const math::Vec2 & pt) const;
	const std::vector<Mesh> & GetMeshs() const;

private:

	bool GetNavMesh(const WayPoint & endWayPoint, 
					HeapQueue<WayPoint> & opens,
					std::vector<WayPoint> & closes, 
					std::vector<std::uint16_t> & navmesh);
	void GetWayPoints(const math::Vec2 & endPoint, 
					  const std::vector<std::uint16_t> & navmesh,
					  std::vector<math::Vec2> & waypoints);
	bool IsNewWayPoint(std::uint16_t id,
					   const HeapQueue<WayPoint> & opens,
					   const std::vector<WayPoint> & closes);
	std::uint16_t Distance(const math::Vec2 & a, const math::Vec2 & b);

private:
	std::vector<Mesh> _meshs;
};