#include "astar.h"
#include <sstream>

AStar::AStar()
{
}

AStar::~AStar()
{
}

bool AStar::InitFromFile(const std::string & fname)
{
	std::ifstream ifile(fname);
	std::stringstream stream;
	std::string line, ignore;
	std::uint16_t nlink = 0u;
	_meshs.clear();
	if (ifile)
	{
		while (std::getline(ifile, line))
		{
			Mesh mesh;
			stream.str(line);
			stream
				>> ignore >> ignore
				>> mesh.center.x >> mesh.center.y	//	cp
				>> ignore >> ignore
				>> mesh.tri.pt1.x >> mesh.tri.pt1.y	//	pt1
				>> ignore >> ignore
				>> mesh.tri.pt2.x >> mesh.tri.pt2.y	//	pt2
				>> ignore >> ignore
				>> mesh.tri.pt3.x >> mesh.tri.pt3.y	//	pt3
				>> ignore >> ignore
				>> nlink;
			for (std::uint16_t link; nlink != 0;
				 --nlink, stream >> link,
				 mesh.links.push_back(link));
			mesh.ID = (std::uint16_t)_meshs.size();
			_meshs.push_back(mesh);
		}
		return true;
	}
	return false;
}

bool AStar::GetPath(std::uint16_t startID, const math::Vec2 & startpt, std::uint16_t endID, const math::Vec2 & endpt, std::vector<math::Vec2>& waypoints)
{
	ASSERT(startID < _meshs.size());
	ASSERT(endID < _meshs.size());
	if (startID == endID)
	{
		waypoints.push_back(endpt);
	}
	else
	{
		HeapQueue<WayPoint> opens;
		std::vector<WayPoint> closes;
		std::vector<std::uint16_t> navmesh;
		opens.emplace(startID, startID, 0, Distance(startpt, endpt));
		if (GetNavMesh(WayPoint(endID, 0, 0, 0), opens, closes, navmesh))
		{
			GetWayPoints(endpt, navmesh, waypoints);
		}
	}
	return !waypoints.empty();
}

std::uint16_t AStar::GetMeshID(const math::Vec2 & pt) const
{
	auto find = std::find_if(_meshs.begin(), _meshs.end(), [&pt](const Mesh & mesh)
							 {
								 return math::IsPointInTriangle(mesh.tri.pt1, mesh.tri.pt2, mesh.tri.pt3, pt);
							 });
	return find != _meshs.end() ? std::distance(_meshs.begin(), find) : UINT16_MAX;
}

const std::vector<AStar::Mesh>& AStar::GetMeshs() const
{
	return _meshs;
}

bool AStar::IsNewWayPoint(std::uint16_t id, const HeapQueue<WayPoint> & opens, const std::vector<WayPoint> & closes)
{
	auto comp = [id](const WayPoint & wp) { return wp.ID == id; };
	return opens.cend() == std::find_if(opens.cbegin(), opens.cend(), comp)
		&& closes.cend() == std::find_if(closes.cbegin(), closes.cend(), comp);
}

bool AStar::GetNavMesh(const WayPoint & endWayPoint, HeapQueue<WayPoint>& opens, std::vector<WayPoint>& closes, std::vector<std::uint16_t>& navmesh)
{
	while (!opens.empty())
	{
		auto top = opens.top();
		closes.push_back(top);
		opens.pop();
		if (top.ID == endWayPoint.ID)
		{
			break;
		}
		for (auto link : _meshs.at(top.ID).links)
		{
			if (IsNewWayPoint(link, opens, closes))
			{
				opens.emplace(link, top.ID, top.G + 1, 
							  Distance(_meshs.at(top.ID).center, _meshs.at(endWayPoint.ID).center));
			}
		}
	}

	if (!closes.empty() && closes.back().ID == endWayPoint.ID)
	{
		auto currit = closes.crbegin();
		auto parent = currit->parent;
		auto currid = currit->ID;
		auto compfn = [&parent](const WayPoint & wp) { return wp.ID == parent; };
		for (navmesh.push_back(currid); currid != parent; navmesh.push_back(currid))
		{
			currit = std::find_if(currit, closes.crend(), compfn);
			parent = currit->parent;
			currid = currit->ID;
		}
	}

	return !navmesh.empty();
}

void AStar::GetWayPoints(const math::Vec2 & endPoint, const std::vector<std::uint16_t>& navmesh, std::vector<math::Vec2>& waypoints)
{
	for (auto & id : navmesh)
	{
		waypoints.push_back(_meshs.at(id).center);
	}
}

inline std::uint16_t AStar::Distance(const math::Vec2 & a, const math::Vec2 & b)
{
	return (std::uint16_t)(std::abs(b.x - a.x) + std::abs(b.y - a.y));
}