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
			for (std::uint16_t link; nlink != 0; --nlink, stream >> link, mesh.links.push_back(link));
			mesh.ID = (std::uint16_t)_meshs.size();
			_meshs.push_back(mesh);
		}
		return true;
	}
	return false;
}

bool AStar::GetPath(std::uint16_t startID, const math::Vec2 & startpt, 
					std::uint16_t endID, const math::Vec2 & endpt, 
					std::vector<std::uint16_t> & navmesh, 
					std::vector<math::Vec2>& waypoints)
{
if (startID >= _meshs.size() || endID >= _meshs.size())
{
	return false;
}
if (startID == endID)
{
	waypoints.push_back(startpt);
	waypoints.push_back(endpt);
}
else
{
	_closes.clear();
	_opens.clear();
	_opens.emplace(startID, startID, 0, Distance(startpt, endpt));
	if (GetNavMesh({ endID, 0, 0, 0 }, navmesh))
	{
		GetWayPoints(startpt, endpt, navmesh, waypoints);
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

bool AStar::IsNewWayPoint(std::uint16_t id) const
{
	auto comp = [id](const WayPoint & wp) { return wp.ID == id; };
	return _opens.cend() == std::find_if(_opens.cbegin(), _opens.cend(), comp)
		&& _closes.cend() == std::find_if(_closes.cbegin(), _closes.cend(), comp);
}

inline std::uint32_t AStar::Distance(const math::Vec2 & a, const math::Vec2 & b) const
{
	return (std::uint32_t)(std::abs(b.x - a.x) + std::abs(b.y - a.y));
}

bool AStar::GetNavMesh(const WayPoint & endWayPoint, std::vector<std::uint16_t>& navmesh)
{
	while (!_opens.empty())
	{
		auto top = _opens.top();
		_closes.push_back(top);
		_opens.pop();
		if (top.ID == endWayPoint.ID) { break; }
		for (auto link : _meshs.at(top.ID).links)
		{
			if (IsNewWayPoint(link))
			{
				const auto & curr = _meshs.at(link);
				auto g = (_meshs.at(top.ID).center - curr.center).Length();
				auto h = Distance(curr.center, _meshs.at(endWayPoint.ID).center);
				_opens.emplace(link, top.ID, top.G + (std::uint32_t)g, h);
			}
		}
	}

	if (!_closes.empty() && _closes.back().ID == endWayPoint.ID)
	{
		auto currit = _closes.crbegin();
		auto parent = currit->parent;
		auto currid = currit->ID;
		auto compfn = [&parent](const WayPoint & wp) { return wp.ID == parent; };
		for (navmesh.push_back(currid); currid != parent; navmesh.push_back(currid))
		{
			currit = std::find_if(currit, _closes.crend(), compfn);
			parent = currit->parent;
			currid = currit->ID;
		}
	}

	return !navmesh.empty();
}

void AStar::GetWayPoints(const math::Vec2 & startpt, const math::Vec2 & endpt,
						 const std::vector<std::uint16_t> & navmesh,
						 std::vector<math::Vec2>& waypoints)
{
	//	修改拐点计算
	waypoints.push_back(startpt);
	auto currIdx = navmesh.size() - 1;
	auto curr = startpt, newcurr = startpt;
	auto up = startpt, newup = startpt;
	auto dn = startpt, newdn = startpt;
	GetWayPoint((std::uint16_t)navmesh.at(currIdx),
				(std::uint16_t)navmesh.at(currIdx - 1),
				up, dn, curr, up, dn, curr);
	for (auto i = (int)currIdx - 1; i != -1; --i)
	{
		if (i != 0)
		{
			if (GetWayPoint(
				(std::uint16_t)navmesh.at(i),
				(std::uint16_t)navmesh.at(i - 1),
				newup, newdn, curr, newup, newdn, newcurr))
			{
				auto[isok, isup] = CheckSight(curr, up, dn, newcurr);
				if (isok)
				{
					waypoints.push_back(newcurr);
					curr = newcurr;
					currIdx = i;
					up = newup;
					dn = newdn;
				}
				else if (isup)
				{
					waypoints.push_back(up);
					curr = newup = newdn = up;
					i = currIdx;
				}
				else
				{
					waypoints.push_back(dn);
					curr = newup = newdn = dn;
					i = currIdx;
				}
			}
		}
		else
		{
			auto[isok, isup] = CheckSight(curr, newup, newdn, endpt);
			if (isok)
			{
				waypoints.push_back(endpt);
			}
			else if (isup)
			{
				waypoints.push_back(up);
				curr = newup = newdn = up;
				i = currIdx;
			}
			else
			{
				waypoints.push_back(dn);
				curr = newup = newdn = dn;
				i = currIdx;
			}
		}
	}
}

bool AStar::GetWayPoint(const std::uint16_t cmeshID, const std::uint16_t nmeshID, 
						const math::Vec2 & up, const math::Vec2 & dn, const math::Vec2 & curr, 
						math::Vec2 & outup, math::Vec2 & outdn, math::Vec2 & outcurr) const
{
	const auto &[pt1, pt2] = GetLinkLine(_meshs.at(cmeshID), _meshs.at(nmeshID));
	//	更新 left
	if (up - curr == math::Vec2::s_ZERO || (up - curr).Cross(pt1 - curr) > 0)
	{
		outup = pt1;
	}
	else if ((up - curr).Cross(pt2 - curr) < 0)
	{
		outcurr = up; 
		outup = pt1; 
		outdn = pt2; 
		return true;
	}

	//	更新 right
	if (dn - curr == math::Vec2::s_ZERO || (dn - curr).Cross(pt2 - curr) < 0)
	{
		outdn = pt2;
	}
	else if ((dn - curr).Cross(pt1 - curr) > 0)
	{
		outcurr = dn; 
		outup = pt1; 
		outdn = pt2; 
		return true;
	}
	return false;
}

std::tuple<bool, bool> AStar::CheckSight(const math::Vec2 & curr, const math::Vec2 & up, const math::Vec2 & dn, const math::Vec2 & pt)
{
	return (up != curr && (up - curr).Cross(pt - curr) < 0) ? std::make_tuple(false, true)
		: (dn != curr && (dn - curr).Cross(pt - curr) >= 0) ? std::make_tuple(false, false)
		: std::make_tuple(true, true);
}

std::tuple<const math::Vec2&, const math::Vec2&> AStar::GetLinkLine(const Mesh & mesh1, const Mesh & mesh2) const
{
	return mesh2.tri.IsExistsLine({ mesh1.tri.pt1, mesh1.tri.pt2 }) ? std::make_tuple(std::cref(mesh1.tri.pt1), std::cref(mesh1.tri.pt2))
		: mesh2.tri.IsExistsLine({ mesh1.tri.pt2, mesh1.tri.pt3 }) ? std::make_tuple(std::cref(mesh1.tri.pt2), std::cref(mesh1.tri.pt3))
		: std::make_tuple(std::cref(mesh1.tri.pt3), std::cref(mesh1.tri.pt1));
}
