#ifndef __NPC_OTHER_H__
#define __NPC_OTHER_H__

#include "obj/character/npc.h"

enum NPC_EXTRA_INFO_TYPE
{
	INFO_TYPE_BRAVE_GROUND = 1, //param1 是使用次数
};

struct NPCExtraInfo
{
	NPCExtraInfo()
	{
		info_type = 0;
		param1 = 0;
		param2 = 0;
		param3 = 0;
	}

	int info_type;
	int param1;
	int param2;
	int param3;
};

class NPCOther : public NPC
{
public:
	NPCOther();
	virtual ~NPCOther();

	void NotifyInfo();

	NPCExtraInfo & GetNpcExtraInfo();
protected:
	NPCExtraInfo m_extra_info;
};

#endif