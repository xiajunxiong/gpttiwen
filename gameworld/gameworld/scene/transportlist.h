#ifndef __TRANSPORTLIST_H__
#define __TRANSPORTLIST_H__

#include <map>
#include <vector>

#include "gamedef.h"

struct ConfigSceneDoor;
typedef std::vector<ConfigSceneDoor> ConfigSceneDoorList;

struct Transport
{
	Transport() : door_id(0), target_scene(0), require_level(0) {}
	Transport(int i, const Posi &l, int scene, const Posi &t_pos, Attribute rl)
		: door_id(i), location(l), target_scene(scene), target_pos(t_pos), require_level(rl) {}
	int			door_id;
	Posi		location;
	int			target_scene;
	Posi		target_pos;
	Attribute	require_level;
};

class TransportList
{
public:
	TransportList();
	~TransportList();

	void Init(const ConfigSceneDoorList &door_list);
	void Add(int door_id, const Posi &location, int target_scene, const Posi &target_pos, Attribute require_level);

	Transport * GetTransport(int tid);
	Transport * GetOneTransport();

protected:
	typedef std::map<int, Transport> TransportMap;
	typedef std::map<int, Transport>::iterator TransportMapIt;

	TransportMap m_transport_list;
};

#endif
