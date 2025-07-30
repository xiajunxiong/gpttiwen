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
* \brief �ӳٵ�¼��������
* \detail �����ڶ��ŵ�����£��µ�¼�Ŀͻ��˲���Ҫ�����ٷ�һ�ε�¼����
* \todo ��δ���Զ���ͻ���ͬʱ��¼�µı��֣���Ӧֻ���ܵ�һ�����ŵĿͻ��ˣ��������ŵĿͻ���Ӧֱ�ӷ��ص�¼ʧ��
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
	NetIDToRoleIDMap m_netid_to_roleid_map; // ���ڵ��ӳٵ�¼�Ŀͻ��˶Ͽ�����ʱ��ɾ���ӳٵ�¼������

	unsigned int m_next_check_timestamp;

	INSTANCE_FORBID_COPY_AND_EXTERIAL_CREATE(DelayLoginManager);
};

#endif