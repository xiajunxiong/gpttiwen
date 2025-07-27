#ifndef __RELIC_LOOTING_DEF_H__
#define __RELIC_LOOTING_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#pragma pack(push, 4)

struct RelicLootRankData
{
	RelicLootRankData() { this->Reset(); }

	void Reset()
	{
		uid = 0;
		memset(role_name, 0, sizeof(GameName));
		rank_value = 0;
		flexible_int = 0;
		flexible_ll = 0;
	}

	bool operator< (const RelicLootRankData & _a)
	{
		if (rank_value == _a.rank_value)
		{
			return flexible_ll > _a.flexible_ll;
		}
		return rank_value < _a.rank_value;
	}

	int uid;
	GameName role_name;
	long long rank_value;
	int flexible_int;
	long long flexible_ll;
};

struct RelicLootingTeamRecodeInfo
{
	RelicLootingTeamRecodeInfo()
	{
		min_level_limit = 0;
		max_level_limit = MAX_ROLE_LEVEL;
		leader_uid = 0;
		member_count = 0;
		memset(member_uid_list, 0, sizeof(member_uid_list));
	}
	int min_level_limit;
	int max_level_limit;
	int leader_uid;
	int member_count;
	int member_uid_list[MAX_TEAM_MEMBER_NUM - 1];
};

struct RelicLootingBoxSkill
{
	RelicLootingBoxSkill() : sn(0) {}//道具技能
	int sn;
};

struct RelicLootingFBSkillInfo
{
	RelicLootingFBSkillInfo() : sn(0), special_param(0), cool_down_tamp(0), guide_over_tamp(0), casting_tamp(0) {}

	void ClearSkill()
	{
		sn = 0;
		special_param = 0;
		cool_down_tamp = 0;
		guide_over_tamp = 0;
		casting_tamp = 0;
	}

	void ClearMove()
	{
		special_param = 0;
		guide_over_tamp = 0;
		casting_tamp = 0;
		special_param = 0;
	}

	bool UsingSkill() { return guide_over_tamp != 0 || casting_tamp != 0; }
	int sn;//技能id
	unsigned int special_param;		//技能特殊参数
	unsigned int cool_down_tamp;	//冷却结束时间戳
	unsigned int guide_over_tamp;	//引导结束时间戳
	unsigned int casting_tamp;		//施法结束时间戳
};

struct PlayerSkillList
{
	PlayerSkillList() : common_skill_num(0) {}

	const static int MAX_COMMON_SKILL_NUM = 3;// 阵营技能最大数量
	RelicLootingFBSkillInfo * GetSkillInfo(int skill_id)
	{
		for (int sk_idx = 0; sk_idx < common_skill_num && sk_idx < MAX_COMMON_SKILL_NUM; sk_idx++)
		{
			if (m_common_skill[sk_idx].sn == skill_id)return &m_common_skill[sk_idx];
		}
		if (m_item_skill.sn == skill_id)return &m_item_skill;
		return NULL;
	}

	bool UsingSkill(int * skill_index = NULL)
	{
		if (m_item_skill.UsingSkill())
		{
			*skill_index = -1;
			return true;
		}

		for (int skill_idx = 0; skill_idx < common_skill_num && skill_idx < MAX_COMMON_SKILL_NUM; skill_idx++)
		{
			if (m_common_skill[skill_idx].UsingSkill())
			{
				*skill_index = skill_idx;
				return true;
			}
		}
		return false;
	}

	int common_skill_num;
	RelicLootingFBSkillInfo m_common_skill[MAX_COMMON_SKILL_NUM];//阵营技能
	RelicLootingFBSkillInfo m_item_skill;//特殊道具技能
};

struct PlayerInfo
{
	PlayerInfo() : uid(0), stop_reason(0), gongxun(0), param(0), on_line(0)
	{
		memset(player_name, 0, sizeof(GameName));
	}
	enum STOP_MOVE_REASON
	{
		STOP_MOVE_REASON_NORMAL = 0,
		STOP_MOVE_REASON_BE_CATCH = 1,// 被抓了
		STOP_MOVE_REASON_BE_TRAP = 2,// 被陷阱夹住
	};
	GameName player_name;
	int uid;
	int stop_reason;//是否被抓了
	int gongxun;// 功勋
	unsigned int param;
	int on_line;
	PlayerSkillList skill_list;
};

#pragma pack(pop)

#endif
