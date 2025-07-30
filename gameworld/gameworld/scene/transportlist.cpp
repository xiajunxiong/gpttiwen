#include "transportlist.h"
#include "config/sceneconfig.h"

TransportList::TransportList()
{

}

TransportList::~TransportList()
{

}

void TransportList::Init(const ConfigSceneDoorList &door_list)
{
	m_transport_list.clear();

	for (ConfigSceneDoorList::const_iterator iter = door_list.begin(); iter != door_list.end(); ++iter)
	{
		this->Add(iter->door_id, Posi(iter->x, iter->y), iter->targetscene.sceneid, Posi(iter->targetscene.x, iter->targetscene.y), iter->level);
	}
}

void TransportList::Add(int door_id, const Posi &location, int target_scene, const Posi &target_pos, Attribute require_level)
{
	m_transport_list[door_id] = Transport(door_id, location, target_scene, target_pos, require_level);
}

Transport * TransportList::GetTransport(int tid)
{
	TransportMapIt it = m_transport_list.find(tid);
	if (it != m_transport_list.end())
	{
		return &it->second;
	}

	return NULL;
}

Transport * TransportList::GetOneTransport()
{
	TransportMapIt it = m_transport_list.begin();
	if (it != m_transport_list.end())
	{
		return &it->second;
	}

	return NULL;
}

