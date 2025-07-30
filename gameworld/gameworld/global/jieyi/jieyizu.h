#ifndef _JIE_YI_ZU_HPP_
#define _JIE_YI_ZU_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"

#include <map>
#include <vector>
#include <list>
#include <set>

enum JIE_YI_ZU_INFO_TYPE
{
	JIE_YI_ZU_INFO_TYPE_DEFAULT,
	JIE_YI_ZU_INFO_TYPE_QUERY,
};

class Role;
class JieYiZu
{
public:
	JieYiZu(long long unique_id);
	~JieYiZu();
public:	
	void OnUserLogin(Role * role);
	void OnUserLoginOriginGame(int role_id);
	void OnUserLogout(Role * role);
	void OnDayChange(Role * role);
	void ModifyMemberInfo(int role_id);
	void NoticeOnlineStatus(int role_id, int online_status);

	void Update(unsigned long interval, time_t now_second);
	void UpdateLeave(unsigned long interval, time_t now_second);
	void UpdateTanHe(unsigned long interval, time_t now_second);

	void JieYiZuMemberName(Role * role, const JieYIZuMemberName & name);//成员修改名字
	void JieYiZuName(Role * role, const JieYIZuName name);			//修改结义组名字
	void JieYiZuMemberName(int role_id, const JieYIZuMemberName & name);		//成员修改名字	
	void JieYiZuNameHelper(const JieYIZuName name);//修改结义组名字

	void JieYiZuText(int role_id, const JieYIZuText & name);		//修改公告文字
	void JieYiZuLeave(int role_id, bool is_cancel);				//离开
	int JieYiZuKick(int role_id, int target_role_id);				//踢人
	void JieYiZuInvite(int role_id, int target_role_id);			//邀请
	int JieYiZuInviteReply(int role_id, bool is_agree);			//邀请回复

	int TanHe(int role_id);	//弹劾
	int TanHeVote(int role_id, int target_role_id);	//弹劾投票
	int CheckBreakTanHe(int login_role_id);	//组长登录，中断弹劾

	void SendInfo(Role * role = NULL, int info_type = JIE_YI_ZU_INFO_TYPE_DEFAULT);
	void SendDismissInfo(const std::set<int> & leave_role_id);
	void SendMsgAllMember(const void * msg,unsigned int lenght);
	void SendMsgAllMemberAny(const void * msg, unsigned int lenght);
public:
	void GetName(JieYIZuName * name, JieYIZuMemberName * member_name = 0, int role_id = 0);
	int GetJieYiCreateDay();
	int GetJieYiMutuaIntimacyMin();
	void AutoSetZuZhang();
	int GetMemberCount();
	std::vector<int> GetMemberList();
	int GetZuZhangRoleId();
	void OnDismissJieYiZu();
	long long GetJieYiZuId();
	bool IsZuMember(int role_id);
	bool IsZuZhang(int role_id);
	unsigned int GetZuCreateTime();
	CommonSaveJieYiZuRoleInfoItem * GetZuMemberData(int role_id, bool is_set_dirty = true);
	CommonSaveJieYiZu * GetCommonSaveJieYiZu(bool is_set_dirty = true);
	bool SetCreateData(const JieYIZuName & jie_yi_name, const std::vector<CommonSaveJieYiZuRoleInfoItem> & list);
	int SetSyncData(const CommonSaveJieYiZu & data);
	int SetSyncRoleData(const CommonSaveJieYiZuRoleInfoItem & data);
	void DeleteData();
	void SyncDataHiddenToGame(int server_id = 0);
	void SyncMemberDataHiddenToGame(int role_id);
	void OnHiddenGameRoleChange(const CommonSaveJieYiZuRoleInfoItem * role_info);
	JieYIZuName GetJieYiZuName();

	void SendMailAllMember(int reason, const MailContentParam& contentparam, int exclude_role_id = 0);
private:
	void SetDataChange();
	void AddJieYiMember(const CommonSaveJieYiZuRoleInfoItem& role_info, bool is_from_create = false);
	void OnRoleLeave(int role_id, const char * reason);
	void OnRoleAdd(int role_id, const char * reason);
	CommonSaveJieYiZuRoleInfoItem * GetEmptyRoleInfo();
	int GetJoinJieYiDay(unsigned int create_time, unsigned int join_time);

	long long m_unique_id;
	CommonSave * m_common_save;
	CommonSaveJieYiZu * m_data;	
};

#endif

