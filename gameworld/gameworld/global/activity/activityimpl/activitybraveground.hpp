#ifndef __ACTIVITY_BRAVE_GROUND_HPP__
#define __ACTIVITY_BRAVE_GROUND_HPP__

#include "global/activity/activity.hpp"
#include "servercommon/commonsavedef.h"
#include <vector>


class Role;
class CommonSave;
class ActivityBraveGround : public Activity
{
public:
	ActivityBraveGround(ActivityManager *activity_manager);
	~ActivityBraveGround();

	virtual void OnActivityStatusChange(int from_status, int to_status);
	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnUserLogin(Role * role);	
	virtual void OnWeekChange();

	virtual void OnConnectCrossServerSucc();
	virtual void OnUserLoginOriginGame(int role_id);
	virtual void OnGameServerConnect(std::set<int> server_id);	//隐藏服 并且 是跨服活动 才会调

	void SendRankInfo(Role * role);
	void SendInfo(Role * role = NULL);
	void HiddenToGameSendInfo(int role_id);
	void HiddenToGameSendRankInfo(int role_id);
	void SendActivityBraveGroundCommonInfo(Role * role = NULL, int role_id = 0);
	
	int GetRoleReachedLevel(int role_id);
	void SendLevelRewardInfo(int start_level, int end_level, Role * role);

	int GetRecordKey();
	void UpdateTeamRecord(int record_key, const CommonSaveBraveGroundTeamReceordData & team_record_data);

	void UpdateTopRole(int(&top_role_id)[BRAVE_GROUND_TOP_ITEM_NUM][BRAVE_GROUND_TOP_ROLE_NUM], bool is_notice);
	int GetTopRoleTitle(int role_id);

	//
	void GameHiddenSetRoleInfoReq(int role_id, int reached_level);
	int SaoDang(Role * role);  //扫荡
private:
	bool static SortRankFunc(const CommonSaveBraveGroundTeamReceordData &rank_item_1, const CommonSaveBraveGroundTeamReceordData &rank_item_2);

	void SetRoleReachedLevel(int role_id, int reached_level);

	void UpdateRank();
	void SendReward();
	void ClearData();
	void RewardReissue(Role * role);//奖励补发,2022-2-8 前A2版本，通关奖励没给到，补发函数=

	int m_record_key;			// 用来识别哪个队伍

	CommonSave * m_role_save_data;
	CommonSave * m_team_record_save_data;

	bool m_is_need_update_rank;
	time_t m_next_updata_rank_time;
	std::vector<CommonSaveBraveGroundTeamReceordData> m_rank_info;

	bool m_is_init;
	int m_top_role_id[BRAVE_GROUND_TOP_ITEM_NUM][BRAVE_GROUND_TOP_ROLE_NUM];

	bool m_is_sync_reached_level_flag;
};

#endif