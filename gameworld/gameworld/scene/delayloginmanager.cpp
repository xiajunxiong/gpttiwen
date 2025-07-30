#include "delayloginmanager.hpp"
#include "engineadapter.h"
#include "world.h"
#include "scene/scenemanager.hpp"

DelayLoginManager::DelayLoginManager() : m_next_check_timestamp(0u)
{

}

DelayLoginManager::~DelayLoginManager()
{

}

DelayLoginManager& DelayLoginManager::GetInstance()
{
	static DelayLoginManager instance;

	return instance;
}

void DelayLoginManager::Update()
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	if (now < m_next_check_timestamp) return;

	m_next_check_timestamp = now + 1; // 每秒执行一次

	static DelayLoginInfo login_info_list[10];
	static int uid_list[10];
	int login_info_count = 0;

	DelayLoginInfoMap::iterator it = m_info_map.begin();
	for (; it != m_info_map.end(); )
	{
		bool is_erase = false;
		
		if (it->second.ready_to_login_timestamp > 0u && now >= it->second.ready_to_login_timestamp && login_info_count < 10)
		{
			login_info_list[login_info_count] = it->second;	
			uid_list[login_info_count] = it->first;
			++login_info_count;
		}
		else if (it->second.info_create_timestamp + 5 < now)
		{
			is_erase = true;
		}

		if (is_erase) // erase掉超时的
		{
			m_netid_to_roleid_map.erase(it->second.gsnetid);
			m_info_map.erase(it++);
		}
		else
		{
			++it;
		}
	}

	// 先发登录请求，再从info_map中删除， 避免再次顶号导致无限循环顶号（尝试一次延迟登录如果有问题就应该下发通知，不应无限顶号）
	for (int i = 0; i < login_info_count; ++i)
	{
//#ifdef _DEBUG
//		printf("DelayLoginManager::Update Call OnRecv role[%d,%s]\n", login_info_list[i].req_msg.role_id, login_info_list[i].req_msg.role_name);
//#endif
		World::GetInstWorld()->GetSceneManager()->OnRecv(login_info_list[i].gsnetid, login_info_list[i].ip, (const char*)&login_info_list[i].req_msg, sizeof(login_info_list[i].req_msg));
		m_netid_to_roleid_map.erase(login_info_list[i].gsnetid);
		m_info_map.erase(uid_list[i]);
	}
}

bool DelayLoginManager::AddDelayLoginRequest(const UserID& user_id, const GSNetID& gsnetid, IP ip, const Protocol::CSUserEnterGSReq* msg_ptr)
{
	if (NULL == msg_ptr) return false;

	if (this->IsUserDelayLoginInfoExist(user_id))
	{
		return false;
	}

	DelayLoginInfo info;
	info.gsnetid = gsnetid;
	info.ip = ip;
	info.req_msg = *msg_ptr;
	info.info_create_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	info.ready_to_login_timestamp = 0u;

	m_info_map.insert(DelayLoginInfoMap::value_type(UidToInt(user_id), info));

//#ifdef _DEBUG
//	printf("DelayLoginManager::AddDelayLoginRequest role[%d,%s]\n", info.req_msg.role_id, info.req_msg.role_name);
//#endif

	m_netid_to_roleid_map.insert(NetIDToRoleIDMap::value_type(gsnetid, UidToInt(user_id)));

	return true;
}

bool DelayLoginManager::IsUserDelayLoginInfoExist(const UserID& user_id)
{
	DelayLoginInfoMap::iterator it = m_info_map.find(UidToInt(user_id));
	if (it != m_info_map.end())
	{
		return true;
	}

	return false;
}

void DelayLoginManager::OnUserLogout(const UserID& user_id)
{
	this->ReadyToLogin(user_id);
}

void DelayLoginManager::OnCrossUserKicked(const UserID& user_id)
{
	this->ReadyToLogin(user_id);
}

void DelayLoginManager::OnCrossUserAllowedEnterHidden(const UserID& user_id)
{
	this->ReadyToLogin(user_id);
}

void DelayLoginManager::RemoveUserDelayLoginRequest(const UserID& user_id)
{
	DelayLoginInfoMap::iterator it = m_info_map.find(UidToInt(user_id));
	if (it != m_info_map.end())
	{
		m_netid_to_roleid_map.erase(it->second.gsnetid);
		m_info_map.erase(it);
	}
}

void DelayLoginManager::OnNetidDisconnect(const GSNetID &netid)
{
	NetIDToRoleIDMap::iterator it = m_netid_to_roleid_map.find(netid);
	if (it != m_netid_to_roleid_map.end())
	{
		m_info_map.erase(it->second);
		m_netid_to_roleid_map.erase(it);
	}
}

void DelayLoginManager::ReadyToLogin(const UserID& user_id)
{
	DelayLoginInfoMap::iterator it = m_info_map.find(UidToInt(user_id));
	if (it == m_info_map.end())
	{
		return;
	}

	it->second.ready_to_login_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 1; // 下一秒可以登录
}

