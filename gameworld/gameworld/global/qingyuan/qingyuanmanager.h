#ifndef _QING_YUAN_MANAGER_HPP_
#define _QING_YUAN_MANAGER_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"
#include "global/commonsave/commonsavemanager.hpp"

#include "servercommon/qingyuandef.h"
#include "global/qingyuan/qingyuan.h"


#include <map>
#include <vector>
#include <list>
#include <set>


struct QingYuanInviteInfo
{
	int target_role_id;
	int item_type;
	time_t invite_time;
};

class QingYuanManagerLoadSuccCallback :public ICommonSaveLoadSuccCallback
{
public:
	QingYuanManagerLoadSuccCallback();
	~QingYuanManagerLoadSuccCallback();
	virtual void Callback();
};

enum QING_YUAN_SYNC_TYPE
{
	QING_YUAN_SYNC_TYPE_START,
	QING_YUAN_SYNC_TYPE_UPDATE,
	QING_YUAN_SYNC_TYPE_END,
};

class Role;
class CommonSave;
class QingYuanManager
{
public:
	static QingYuanManager & Instance();

	void Init();

	void OnGameServerConnect(std::set<int> server_id);	
	void OnUserLogin(Role * role);
	void OnUserLogout(Role * role);
	void OnUserLoginOriginGame(int role_id);
	void OnUserLogoutOriginGame(int role_id);

	void OnDayChange();

	void Update(unsigned long interval, time_t now_second);
	void UpdateQingYuan(unsigned long interval, time_t now_second);

	int Invite(Role * role, int target_role_id,int item_type);			//邀请别人
	int InviteReply(Role * role, int inviter_role_id,bool is_agree);			//回复 邀请人
	int QingYuanCreate(int item_type, const std::vector<CommonSaveQingYuanRoleInfoItem> & vec);			//回复 邀请人

	void Gmop(Role * role,int type, int p1, int p2);
public:
	bool HasQingYuan(int role_id);
	QingYuan * GetQingYuanByRoleId(int role_id);
	QingYuan * GetQingYuanByQingYuanId(long long qing_yuan_id);
	//由角色ID获取情缘对象的信息
	void GetQingYuanInfoByRoleId(int role_id, int & qing_yuan_role_id, GameName & qing_yuan_name);

	void DismissQingYuan(long long qing_yuan_id,const char * reson);
	void OnRoleLeave(int role_id, long long qing_yuan_id, const char * reason);
	void OnRoleAdd(long long qing_yuan_id, int role_id, const char * reason);

	void SyncAllQingYuanInfoToGame(int server_id = 0);
	void OnSyncQingYuanInfoFromHiddenStart();
	void OnSyncQingYuanInfoFromHidden(const CommonSaveQingYuan & qing_yuan, bool is_create = false, int item_type = 0);
	void OnSyncQingYuanInfoFromHiddenEnd();

	void UpDataQingYuanCommond(long long qingyuan_id, CommonSaveQingYuanCommonData & common_data);
	void UpDataQingYuanLeiChongAddChongZhi(long long qingyuan_id, int add_chongzhi);
private:
	QingYuanManager();
	virtual ~QingYuanManager();
private:
	std::map<int, QingYuanInviteInfo> m_invite_map;

	std::set<long long> m_sync_qing_yuan_id_list;
	std::map<long long, QingYuan> m_qing_yuan_info;
	std::map<int, long long> m_role_id_to_qing_yuan_id;
};

#endif

