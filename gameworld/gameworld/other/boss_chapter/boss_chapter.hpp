#ifndef __BOSS_CHAPTER_HPP__
#define __BOSS_CHAPTER_HPP__

#include <map>
#include "servercommon/bosschapterdef.hpp"

class RoleModuleManager;
class BossChapter
{
public:
	typedef std::map<int, BossChapterInfo> BossChapterInfoMap;

public:
	BossChapter();
	~BossChapter();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* mgr, const BossChapterParam& param);
	void GetInitParam(BossChapterParam* param);

	void ClaimReward(int monster_group_id);
	void ClaimChapterReward(int chapter);
	void StartBattleReq(int monster_group_id);
	void OnKillMonsterGroup(int monster_group_id);

	void SendAllInfo();
	void SendOnAdd(BossChapterInfo* info);
	void SendOnInfoChange(BossChapterInfo* info);
	void SendOtherInfoChange();
private:
	BossChapterInfo* GetChapterInfo(int monster_group_id);

	RoleModuleManager* m_mgr;
	BossChapterInfoMap m_info_map;
	long long m_chapter_reward_flag;
};

#endif