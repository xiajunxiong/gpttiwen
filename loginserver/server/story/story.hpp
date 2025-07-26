#ifndef __STORY_HPP__
#define __STORY_HPP__

#include "servercommon/userprotocal/msgbattle.h"
#include "inetworkmodule.h"

class Story
{
public:
	static Story& Instance();

	void OnStoryBattleInfoReq(NetID netid);
	void OnStoryRoundResultReq(NetID netid, int round);

	void OnSyncBattleInfoMsg(const char* data, int length);
	void OnSyncRoundResultMsg(int round, const char* data, int length);

private:
	Story();
	~Story();

	Story(const Story&);
	Story& operator= (const Story&);

	Protocol::SCBattleInfo m_battle_info_msg;

	int m_result_num;
	Protocol::SCBattleRoundResult m_result_msg_list[BATTLE_STORY_ROUND_NUM];
};

#endif