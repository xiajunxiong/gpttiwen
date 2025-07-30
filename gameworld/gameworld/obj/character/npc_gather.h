#ifndef __NPC_GATHER_H__
#define __NPC_GATHER_H__

#include "obj/character/npc.h"
#include "npc/npcpool.h"



class NPCGather : public NPC
{
public:
	//采集物子类型 (创建后手动设置)
	enum SubType
	{//										p_1:默认类型 有 写死 的占用, 不要用!
		SUB_TYPE_DEF = 0,	//默认/旧逻辑	
		SUB_TYPE_XUN_YOU,	//巡游宝箱		p_1:0  p_2:巡游类型

		SUB_TYPE_MAX,
	};
public:
	NPCGather();
	virtual ~NPCGather();
	
	void* operator new(size_t c);
	void operator delete(void *m);

	int InitNPCGather(int need_time, int award_count, const int * award_group_list);

	virtual void Update(unsigned long interval, time_t now_second, unsigned int now_dayid);


	int GetNeedTime() { return m_gather_need_time; }
	int GetAwardNum() { return m_award_count; }

	bool CanGather(Role * gatherer);
	bool GatherThis(Role * gatherer);
	void SetRemoveAfterGatherFlag(bool remove_flag);
	void SetParam(const int * p1, const int * p2 = NULL, const int * p3 = NULL);
	void GetParam(int * ARG_OUT p1, int * ARG_OUT p2, int * ARG_OUT p3);
	void SetGatherByTeam(bool team_gather) { m_team_gather = team_gather; }
	bool CanSee(Role *role);

	//设置子类型
	void SetSubType(int sub_type) { m_sub_type = sub_type; };
	int GetSubType() { return m_sub_type; };

	//设置自删时间
	void SetDeleteTime(time_t delete_time_s) { m_delete_time_s = delete_time_s; };
private:
	void RemoveAfterGather();

	int m_gather_need_time;			// 采集所需标准基本时长

	int m_award_count;				// 奖励种数
	int m_award_group[GATHER_ITEM_TYPE_PER_GROUP];// 奖池ID列表

	bool m_already_gathered;		// 是否已经被采集
	bool m_remove_flag;
	bool m_team_gather;				// 是否允许组队状态下采集

	//自删时间
	time_t m_delete_time_s;

	int m_sub_type;
	int m_param1;
	int m_param2;
	int m_param3;

};

#endif