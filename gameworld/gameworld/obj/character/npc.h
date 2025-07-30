#ifndef __NPC_H__
#define __NPC_H__

#include "obj/obj.h"

class NPC : public Obj
{
public:
	NPC(ObjType obj_type);
	virtual ~NPC();

	int GetNpcId() const { return m_npc_id; }
	void SetNpcId(int id) { m_npc_id = id; }
	void SetDir(Dir _dir) { m_dir = _dir; }
	int GetNpcSeq() const { return m_npc_seq; }

protected:
	int m_npc_id;
	int m_npc_seq;
};

#endif