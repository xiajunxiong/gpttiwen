#ifndef __DELAY_LOGIN_MANAGER_HPP__
#define __DELAY_LOGIN_MANAGER_HPP__

#include <map>

#include "servercommon/serverdef.h"
#include "inetworkmodule.h"
#include "protocol/msgentergs.h"
#include "servercommon/servercommon.h"

struct DelayLoginInfo
{
	DelayLoginInfo() : ip(0), ready_to_login_timestamp(0u), info_create_timestamp(0u){}
	GSNetID gsnetid;
	IP ip;
	Protocol::CSUserEnterGSReq req_msg;
	unsigned int ready_to_login_timestamp;
	unsigned int info_create_timestamp;
};


/**
* \brief 延迟登录管理器类
* \detail 用于在顶号的情况下，新登录的客户端不需要重新再发一次登录请求
* \todo 尚未测试多个客户端同时登录下的表现，但应只接受第一个顶号的客户端，其他顶号的客户端应直接返回登录失败
*/
class DelayLoginManager
{
public:
	typedef std::map<int, DelayLoginInfo> DelayLoginInfoMap; // <role_id, DelayLoginInfo>
	typedef std::map<GSNetID, int> NetIDToRoleIDMap; // <GSNetID, role_id>

	static DelayLoginManager& GetInstance();

	void Update();

	bool AddDelayLoginRequest(const UserID& user_id, const GSNetID& gsnetid, IP ip, const Protocol::CSUserEnterGSReq* msg_ptr);
	bool IsUserDelayLoginInfoExist(const UserID& user_id);
	void OnUserLogout(const UserID& user_id);
	void OnCrossUserKicked(const UserID& user_id);
	void OnCrossUserAllowedEnterHidden(const UserID& user_id);
	void RemoveUserDelayLoginRequest(const UserID& user_id);

	void OnNetidDisconnect(const GSNetID &netid);
private:
	void ReadyToLogin(const UserID& user_id);

	DelayLoginInfoMap m_info_map;
	NetIDToRoleIDMap m_netid_to_roleid_map; // 用于当延迟登录的客户端断开连接时，删除延迟登录的请求

	unsigned int m_next_check_timestamp;

	INSTANCE_FORBID_COPY_AND_EXTERIAL_CREATE(DelayLoginManager);
};

#endif