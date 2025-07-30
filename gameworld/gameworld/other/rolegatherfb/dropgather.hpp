#ifndef __DROP_GATHER_HPP__
#define __DROP_GATHER_HPP__
#include <map>
#include <vector>
#include "scene/scene.h"
#include "world.h"
#include "servercommon/serverdef.h"
#include "npc/npcmanager.h"

class Role;
class DropGather
{
public:
	DropGather();
	~DropGather();
	struct Gather_info
	{
		Gather_info() {}

		ObjID gather_id;
		Posi gather_pos;
	};
	static DropGather& Instance();
	void OnStarDropGather();											//初始化野外采集
	void OnGather(Scene *scene,Obj* gather_id);							//野外采集物
	void Update(time_t now_second);
	bool HasGatherInPos(int scene_id, Posi gather_pos);
private:
	
	std::map<int, time_t> m_seq_refresh_time_map;						//seq对应的刷新时间
	std::map<int, std::vector<Gather_info> >	m_scene_gather_pos_map;					//各场景对应采集物位置

};

#endif