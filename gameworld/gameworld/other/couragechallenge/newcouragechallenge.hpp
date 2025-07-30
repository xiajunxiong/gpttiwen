#ifndef _NEW_COURAGE_CHALLENGE_HPP_
#define _NEW_COURAGE_CHALLENGE_HPP_

#include "servercommon/newcouragechallengedef.hpp"

class Role;
class RoleModuleManager;

struct NewCourageChallengeKey
{
	NewCourageChallengeKey() : layer_level(0), level_seq(0) {}
	NewCourageChallengeKey(short _layer_level, char _seq) : layer_level(_layer_level), level_seq(_seq) {}

	bool operator< (const NewCourageChallengeKey& index) const
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

//弑神之塔
class NewCourageChallenge
{
public:
	typedef std::map<NewCourageChallengeKey, unsigned char> NewLevelInfoMap;
	typedef std::map<NewCourageChallengeKey, char> NewDirtyGroupMap;

public:
	NewCourageChallenge();
	~NewCourageChallenge();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const NewCourageChallengeParam & param, const NewCourageChallengeLevelParamList & param_list);
	void GetInitParam(NewCourageChallengeParam * param, NewCourageChallengeLevelParamList * list);
	void ClearDirtyMark();

	void SendPassReward(int monster_group_id, unsigned char flag);
	void SetLayerLevelCondition(unsigned char flag, NewCourageChallengeKey level_key);

	bool OnFetchCommomReward(int seq, bool is_notify = true);
	bool OnFetchTokenReward(int seq, bool is_notify = true);
	void OnFetchOneKey();

	void SendAllInfo();
	void SendLevelListInfo();
	void SendLevelSingleInfo(NewCourageChallengeKey level_key);
	void SendRewardInfo();

	void OnBuyItemCheck();
	int OnCmdBuyItem(int chong_zhi_gold, long long unique_id);
	int OnBuyGiftItem(ARG_OUT int * price);

	int GetPassMaxSeq();							//获取玩家已通关的最高的关卡序号seq

	const long long GetAllStarCount() { return m_total_star; }
	const int GetMaxLayerLevel() { return m_max_layer; }
private:
	void CalcStarCount();
	bool IsInvalid(NewCourageChallengeKey level_key);
	bool IsInvalid(int layer_level, int level_seq);

	RoleModuleManager * m_mgr;
	NewCourageChallengeParam m_param;

	NewLevelInfoMap m_level_data_map;				// 关卡数据
	NewDirtyGroupMap m_dirty_map;
	std::vector<NewCourageChallengeKey> m_temp_dirty_vec;

	long long m_total_star;
	int m_max_layer;
};


#endif 

