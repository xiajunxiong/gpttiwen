#ifndef _JIE_YI_MANAGER_HPP_
#define _JIE_YI_MANAGER_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"
#include "global/commonsave/commonsavemanager.hpp"

#include "servercommon/jieyidef.h"
#include "global/jieyi/jieyizu.h"


#include <map>
#include <vector>
#include <list>
#include <set>

std::string JieYIZuNameToString(const JieYIZuName & jie_yi_name);


struct JieYiZuCreate
{
public:
	JieYiZuCreate();
public:
	bool IsAllConfirm()const;
	JieYiZuCreateRole * GetRoleInfo(int role_id);
	void SendInfo()const;
	void SendCancelInfo(int role_id, int type)const;
	void SendNameUpdate()const;
	void SendConfirm()const;

	int key;
	bool is_confirm_stage;	
	JieYIZuName jie_yi_name; 
	time_t end_time;
	std::map<int, JieYiZuCreateRole> role_info;
};

enum  JIE_YI_CANCEL_TYPE
{
	JIE_YI_CANCEL_TYPE_ROLE,		//玩家主动取消
	JIE_YI_CANCEL_TYPE_LOGOUT,	//离线
	JIE_YI_CANCEL_TYPE_TIMEOUT,
};

enum  JIE_YI_ZU_SYNC_TYPE
{
	JIE_YI_ZU_SYNC_TYPE_START,
	JIE_YI_ZU_SYNC_TYPE_UPDATE,
	JIE_YI_ZU_SYNC_TYPE_END,
};

enum  JIE_YI_XUAN_YAN_TYPE
{
	JIE_YI_XUAN_YAN_TYPE_ROLE,
	JIE_YI_XUAN_YAN_TYPE_ZU,
};

class JieYiZuManagerLoadSuccCallback :public ICommonSaveLoadSuccCallback
{
public:
	JieYiZuManagerLoadSuccCallback();
	~JieYiZuManagerLoadSuccCallback();
	virtual void Callback();
};


class Role;
class CommonSave;
struct CommonSaveJieYiZuRoleInfoItem;
struct CommonSaveJieYiZu;
class JieYiZuManager
{
public:
	static JieYiZuManager & Instance();

	void Init();
	void CheckRepeatMember();

	void OnGameServerConnect(std::set<int> server_id);	
	void OnUserLogin(Role * role);
	void OnUserLogout(Role * role);
	void OnRoleDayChange(Role * role);
	void OnUserLoginOriginGame(int role_id);
	void OnUserLogoutOriginGame(int role_id);


	void Update(unsigned long interval, time_t now_second);
	void UpdateModVer(unsigned long interval, time_t now_second);
	void UpdateCreateInfo(unsigned long interval, time_t now_second);
	void UpdateJieYiZu(unsigned long interval, time_t now_second);
	void UpdateJieYiXuanYan(unsigned long interval, time_t now_second);

	void CreateJieYiZu(Role * role, const JieYIZuName & name);	
	void CreateJieYiZuName(int role_id, const JieYIZuName & name);	//队长命名阶段
	void CreateJieYiZuNameFinish(int role_id);				//队长命名完成
	void CreateJieYiZuMemberConfirm(int role_id);				//队员确认
	void OnCreateAllConfirm(int key);
	void RealCreateJieYiZu(const  JieYIZuName & jie_yi_name, const std::vector<CommonSaveJieYiZuRoleInfoItem> & vec);
	void CancelCreateJieYiZuByRoleId(int role_id,int type);					//取消创建
	void CancelCreateJieYiZuByKey(int key, int type, int role_id = 0);					//取消创建
	
	//宣言
	void RoleXuanYan(int role_id, bool is_delele, const JieYIZuXuanYanText * xuan_yan_text_ptr = NULL);
	void JieYiZuXuanYan(int role_id, const JieYIZuXuanYanText & xuan_yan_text, bool is_delele);
	void JieYiZuXuanYanByJieYiZuId(long long jie_yi_zu_id, bool is_delele, const JieYIZuXuanYanText * xuan_yan_text_ptr=NULL);
	void SendRoleXuanYan(int role_id);
	void SendJieYiZuXuanYan(int role_id);

	void Gmop(Role * role,int type, int p1, int p2);
public:
	JieYiZuCreate * GetCreateInfoByRoleId(int role_id);
	JieYiZuCreate * GetCreateInfoByKey(int key);

	bool HasJieYiZu(int role_id, bool is_check_all = false);
	JieYiZu * GetJieYiZuByRoleId(int role_id);
	JieYiZu * GetJieYiZuByZuId(long long jie_yi_zu_id);
	void GetJieYiZuInfoByRoleId(int role_id, JieYIZuName & jie_yi_name, int max_num, GameName * jie_yi_role_name_list = NULL);

	void DismissJieYiZu(long long jie_yi_zu);
	void OnRoleLeave(int role_id, long long jie_yi_zu, const char * reason);
	void OnRoleAdd(long long jie_yi_zu_id, int role_id, const char * reason);

	void SyncAllJieYiZuInfoToGame(int server_id = 0);
	void OnSyncJieYiZuInfoFromHiddenStart();
	void OnSyncJieYiZuInfoFromHidden(const CommonSaveJieYiZu & jie_yi_zu);
	void OnSyncJieYiZuInfoFromHiddenEnd();
private:
	JieYiZuManager();
	virtual ~JieYiZuManager();
private:
	int GetCreateKey();

	time_t m_next_check_xuan_yan_time;
	std::set<long long> m_sync_jie_yi_zu_id_list;
	std::map<int,JieYiZuCreate> m_jie_yi_zu_create;
	std::map<long long, JieYiZu> m_jie_yi_zu_info;
	std::map<int,long long> m_role_id_to_jie_yi_zu_id;
};

#endif

