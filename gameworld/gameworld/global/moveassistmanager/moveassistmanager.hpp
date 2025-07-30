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

	//Character没有标记id  用这简单判断一下是不是之前发起请求的Character
	unsigned int m_move_assist_creat_id;

	typedef std::map<int, unsigned int> MoveAssistCDMap;
	MoveAssistCDMap m_move_assist_cd_map;
};


#endif