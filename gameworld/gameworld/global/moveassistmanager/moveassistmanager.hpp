#ifndef __MOVE_ASSIST_MANAGER_HPP__
#define __MOVE_ASSIST_MANAGER_HPP__

#include "servercommon/servercommon.h"

class MoveAssistManager
{
public:
	static MoveAssistManager & Instance();

	void OnDayChange();

	bool CheckModifyRoleMoveAssistCD(int role_id, unsigned int now_second);

	unsigned int GetCreatMoveAssistID();

private:
	MoveAssistManager();
	~MoveAssistManager();

	//Characterû�б��id  ������ж�һ���ǲ���֮ǰ���������Character
	unsigned int m_move_assist_creat_id;

	typedef std::map<int, unsigned int> MoveAssistCDMap;
	MoveAssistCDMap m_move_assist_cd_map;
};


#endif