#include "moveassistmanager.hpp"

MoveAssistManager & MoveAssistManager::Instance()
{
	static MoveAssistManager _ma;
	return _ma;
}

bool MoveAssistManager::CheckModifyRoleMoveAssistCD(int role_id, unsigned int now_second)
{
	MoveAssistCDMap::iterator it = m_move_assist_cd_map.find(role_id);
	if (it == m_move_assist_cd_map.end())
	{
		m_move_assist_cd_map.insert(MoveAssistCDMap::value_type(role_id, now_second));
		return true;
	}
	else if (now_second > it->second + 2) //3秒CD 防止大量机器人向客户端寻路
	{
		it->second = now_second;
		return true;
	}
	return false;
}

unsigned int MoveAssistManager::GetCreatMoveAssistID()
{
	m_move_assist_creat_id++;
	return m_move_assist_creat_id;
}

MoveAssistManager::MoveAssistManager()
{
	m_move_assist_creat_id = 0;
}

MoveAssistManager::~MoveAssistManager()
{
}
