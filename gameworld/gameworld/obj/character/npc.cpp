#include "npc.h"
#include "npc/npcmanager.h"

NPC::NPC(ObjType obj_type) : Obj(obj_type), m_npc_id(0), m_npc_seq(0)
{
	m_npc_seq = NpcManager::Instance().GenerateNpcSeq();
	NpcManager::Instance().RegisterServerNPC(m_npc_seq, this);
}

NPC::~NPC()
{
	NpcManager::Instance().UnRegisterServerNPC(m_npc_seq);
}
