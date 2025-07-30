#ifndef _QING_YUAN_HPP_
#define _QING_YUAN_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"

#include <map>
#include <vector>
#include <list>
#include <set>

class Role;
class QingYuan
{
public:
	QingYuan(long long unique_id);
	~QingYuan();
public:	
	void OnUserLogin(Role * role);
	void OnUserLogout(Role * role);
	void ModifyMemberInfo(int role_id);
	void OnDayChange();

	void Update(unsigned long interval, time_t now_second);
	void UpdateLeave(unsigned long interval, time_t now_second);

	int GiveGift(Role * role, int gift_type);			//赠送礼物，为对方开通月卡,不能自己给自己开通
	int GiveGiftFromGame(int role_id, int gift_type);			//赠送礼物，为对方开通月卡,不能自己给自己开通
	int ActiveGift(int role_id, int gift_type);
	int GiftFetch(Role * role);			// 领取开通奖励
	int GiftFetchFromGame(int role_id);			// 领取开通奖励
	int GiftFetchFromGameRet(int role_id, int gift_type);			// 领取开通奖励
	int GiveFetchDay(Role * role);			// 领取每日奖励
	int GiveFetchDayFromGame(int role_id);			// 领取每日奖励
	int GiveFetchDayFromGameRet(int role_id, int gift_type);		// 领取每日奖励

	int Leave(Role * role);			//		协议离婚
	int LeaveReply(Role * role,bool is_agree);			//协议离婚回复	
	int SetLeave();			//协议离婚

	int LeaveSingle(Role * role);			// 单方面离婚
	int LeaveSingleFromGame(int role_id);			// 单方面离婚
	int LeaveForce(Role * role);			//  强制离婚
	int LeaveForceFromGame(int role_id);			//  强制离婚
	int LeaveCancel(Role * role);		// 离婚取消
	int LeaveCancelFromGame( int role_id = 0);		// 离婚取消
	
	int SetSkillNoEffectFlag(Role * role, unsigned long long  skill_no_effect_flag);
	int SetSkillNoEffectFlagFromGame(int role_id, unsigned long long  skill_no_effect_flag);			//

	void SetRAQingYuanShiZhuangParam(unsigned int * ra_ra_shizhuang_join_timestamp, int * buy_mark);
	void GetRAQingYuanShiZhuangParam(unsigned int * ra_ra_shizhuang_join_timestamp, int * buy_mark);
	void SetRAQingYuanLeiChongParam(unsigned int * begin_times, unsigned int * end_times, int * chongzhi_gold);
	void GetRAQingYuanLeiChongParam(unsigned int * begin_times, unsigned int * end_times, int * chongzhi_gold);
	void DismissBuFa(int role_id);			// 活动 离婚补发
	void RAQingYuanLeiChongAddChongZhi(int add_chongzhi);

	void SendInfo(Role * role = NULL);
	void SendDismissInfo(const std::set<int> & leave_role_id);
	void SendMsgAllMember(const void * msg, unsigned int lenght);
	void SendMsgAllMemberAny(const void * msg, unsigned int lenght);
	void SendUpDataActivityInfo();

	void Gmop(Role * role, int type, int p1, int p2);
public:
	int GetQinyYuanIntimacy();
	int GetOtherSideRoleId(int role_id);
	std::set<int> GetSkillIdList(int role_id);
	std::string ToString();
	void OnCreateQingYuan(int item_type);
	void OnDismissQingYuan(const char * reson);
	bool CanDismissQingYuan();
	long long GetQingYuanId();
	int GetDaySinceCreate();
	std::vector<int> GetMemberList();
	std::set<int> GetQingYuanTitle();
	void CheackTitle();
	void ClearTitle();
	void CheackQingYuanDurationReward();   //结婚纪念日奖励
	void ResetGiftDayFetchFlag();

	CommonSaveQingYuanRoleInfoItem * GetMemberData(int role_id, bool is_set_dirty = true);
	CommonSaveQingYuanRoleInfoItem * GetOtherSideMemberData(int role_id, bool is_set_dirty = true);
	CommonSaveQingYuan * GetCommonSaveQingYuan(bool is_set_dirty = true);
	bool SetCreateData(const std::vector<CommonSaveQingYuanRoleInfoItem> & list);
	int SetSyncData(const CommonSaveQingYuan & data);
	int SetSyncCommonData(const CommonSaveQingYuanCommonData & data);
	void DeleteData();
	void SyncDataHiddenToGame(int server_id = 0, bool is_create = false, int item_type = 0);

	void SendMailAllMember(int reason, const MailContentParam& contentparam, int exclude_role_id = 0);
private:
	void SetDataChange();
	void OnRoleLeave(int role_id, const char * reason);
	void OnRoleAdd(int role_id, const char * reason);

	long long m_unique_id;
	CommonSave * m_common_save;
	CommonSaveQingYuan * m_data;
	bool m_can_dismiss;

	std::map<int, time_t> m_leave_req_time;  //离婚请求时间
};

#endif

