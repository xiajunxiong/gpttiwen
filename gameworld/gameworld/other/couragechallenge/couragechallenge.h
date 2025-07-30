#ifndef _COURAGE_CHALLENGE_HPP_
#define _COURAGE_CHALLENGE_HPP_

#include "servercommon/couragechallengedef.hpp"

class Role;
class RoleModuleManager;

struct CourageChallengeKey
{
	CourageChallengeKey(): layer_level(0), level_seq(0) {}
	CourageChallengeKey(short _layer_level, char _seq): layer_level(_layer_level), level_seq(_seq) {}

	bool operator< (const CourageChallengeKey& index) const
	{
		if (layer_level != index.layer_level)
		{
			return layer_level < index.layer_level;
		}
		else
		{
			return level_seq < index.level_seq;
		}
	}

	short layer_level;		// 分层等级段
	char level_seq;			// 该分层等级段中的seq
};

class CourageChallenge
{
public:
	typedef std::map<CourageChallengeKey, unsigned char> LevelInfoMap;
	typedef std::map<CourageChallengeKey, char> DirtyGroupMap;

public:
	CourageChallenge();
	~CourageChallenge();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const CourageChallengeParam & param, const RoleCourageChallengeLevelParamList & param_list);					//获得初始化Param
	void GetInitParam(CourageChallengeParam * param, RoleCourageChallengeLevelParamList * list);
	void ClearDirtyMark();
	void OnResetData(unsigned int old_dayid, unsigned int now_dayid);
	void OnRoleLogin();

	void SendPassReward(int monster_group_id, unsigned char flag);
	void SetLayerLevelCondition(unsigned char flag, CourageChallengeKey level_key);

	void OnRoleOpenUI();
	void FetchEveryDayReward(int layer_level, int layer_level_seq);
	void FetchMaxScoreReward(int layer_level);		//领取该层满星大奖
	int GetLayerScoreNum(int layer_level);			//获得该层已获得的星数数量
	int GetReachStarScoreCount();					//获得此时已达成的总星数数量
	int GetPassMaxLayerLevel();						//获得玩家此时参与的最高的等级段
	int GetPassMaxSeq();							//获取玩家已通关的最高的关卡序号seq
	bool IsFinishCurLayer(int layer_level);			

	void UpdateRoleRank(bool is_change = false);							//更新玩家锢魔之塔排行信息

	void SendAllInfo();
	void SendSingleInfo(CourageChallengeKey level_key);
	void SendScoreSingleInfo(int layer_level);
	inline int GetPetHelperNum() const { return m_param.pet_helper_num; }
private:
	bool IsInvalid(CourageChallengeKey level_key);
	bool IsInvalid(int layer_level, int level_seq);
	bool IsScoreInvalid(int layer_level);
	ScoreLayerLevelNode & GetScoreNode(int score_index);
	void CalculatePetHelperNum();
	bool UpdatePetHelperNum(int pet_helper_num);
	void SendPetHelperNumUpdateNotice();

	RoleModuleManager * m_role_module_mgr;
	CourageChallengeParam m_param;

	//std::map<CourageChallengeKey, short> m_data_node_map;			// second对应此时param中data_node数组下标
	//std::map<int, short> m_layer_level_star_map;					// key为层数等级段 second对应此时param中score_data_node数组下标

	ScoreLayerLevelNode score_data[MAX_COURAGE_CHALLENGE_SCORE_NUM];		// 登陆计算的星数数据
	LevelInfoMap m_level_data_map;											// 关卡数据
	DirtyGroupMap m_dirty_group_map;
	DirtyGroupMap m_snap_dirty_group_map;
};


#endif 
