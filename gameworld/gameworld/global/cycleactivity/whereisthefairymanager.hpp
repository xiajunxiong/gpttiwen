#ifndef __WHERE_IS_THE_FAIRY_MANAGER_HPP__
#define __WHERE_IS_THE_FAIRY_MANAGER_HPP__

#include "servercommon/servercommon.h"

class Role;
class WhereIsTheFairyManager
{
public:
	static WhereIsTheFairyManager & Instance();

	void Update(time_t now_second);

	void SendInfo(Role * role = NULL);

private:
	WhereIsTheFairyManager();
	~WhereIsTheFairyManager();

	void OnCreateNpc();
	void OnDeleteNpc();
	
	int m_last_scene_id;
	int m_last_obj_id;
	unsigned int m_next_refresh_timestamp;
};


#endif
