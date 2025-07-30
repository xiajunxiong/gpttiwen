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

	int GiveGift(Role * role, int gift_type);			//�������Ϊ�Է���ͨ�¿�,�����Լ����Լ���ͨ
	int GiveGiftFromGame(int role_id, int gift_type);			//�������Ϊ�Է���ͨ�¿�,�����Լ����Լ���ͨ
	int ActiveGift(int role_id, int gift_type);
	int GiftFetch(Role * role);			// ��ȡ��ͨ����
	int GiftFetchFromGame(int role_id);			// ��ȡ��ͨ����
	int GiftFetchFromGameRet(int role_id, int gift_type);			// ��ȡ��ͨ����
	int GiveFetchDay(Role * role);			// ��ȡÿ�ս���
	int GiveFetchDayFromGame(int role_id);			// ��ȡÿ�ս���
	int GiveFetchDayFromGameRet(int role_id, int gift_type);		// ��ȡÿ�ս���

	int Leave(Role * role);			//		Э�����
	int LeaveReply(Role * role,bool is_agree);			//Э�����ظ�	
	int SetLeave();			//Э�����

	int LeaveSingle(Role * role);			// ���������
	int LeaveSingleFromGame(int role_id);			// ���������
	int LeaveForce(Role * role);			//  ǿ�����
	int LeaveForceFromGame(int role_id);			//  ǿ�����
	int LeaveCancel(Role * role);		// ���ȡ��
	int LeaveCancelFromGame( int role_id = 0);		// ���ȡ��
	
	int SetSkillNoEffectFlag(Role * role, unsigned long long  skill_no_effect_flag);
	int SetSkillNoEffectFlagFromGame(int role_id, unsigned long long  skill_no_effect_flag);			//

	void SetRAQingYuanShiZhuangParam(unsigned int * ra_ra_shizhuang_join_timestamp, int * buy_mark);
	void GetRAQingYuanShiZhuangParam(unsigned int * ra_ra_shizhuang_join_timestamp, int * buy_mark);
	void SetRAQingYuanLeiChongParam(unsigned int * begin_times, unsigned int * end_times, int * chongzhi_gold);
	void GetRAQingYuanLeiChongParam(unsigned int * begin_times, unsigned int * end_times, int * chongzhi_gold);
	void DismissBuFa(int role_id);			// � ��鲹��
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
	void CheackQingYuanDurationReward();   //�������ս���
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

	std::map<int, time_t> m_leave_req_time;  //�������ʱ��
};

#endif

