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
	void OnStarDropGather();											//��ʼ��Ұ��ɼ�
	void OnGather(Scene *scene,Obj* gather_id);							//Ұ��ɼ���
	void Update(time_t now_second);
	bool HasGatherInPos(int scene_id, Posi gather_pos);
private:
	
	std::map<int, time_t> m_seq_refresh_time_map;						//seq��Ӧ��ˢ��ʱ��
	std::map<int, std::vector<Gather_info> >	m_scene_gather_pos_map;					//��������Ӧ�ɼ���λ��

};

#endif