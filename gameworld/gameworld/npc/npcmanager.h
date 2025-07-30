#ifndef __NPC_MANAGER_H__
#define __NPC_MANAGER_H__

#include "gamedef.h"
#include "npcpool.h"
#define PI 3.14159265f

class NPCMonster;
class NPCGather;
class NPCChosed;
class Scene;
class NPC;
class NpcManager
{
public:
	static const int MAX_NPC_GATHER_LIMIT = 2;
	static const int MIN_NPC_SEQ = 100000;
	static const int MAX_NPC_SEQ = MAX_INT;

	typedef std::map<int, NPC*> ServerNPCMap; // key is seq, generate by GenerateNpcSeq, NPC*在析构时会在Map中去除

public:

	static NpcManager & Instance();

	void Update(time_t now_second);

	bool CanTouchNpc(Role * role, int seq_id);

	void OnNPCPurchaseItem(Role * role, int seq_id, int item_id, int buy_num);
	void OnNPCStartsBattle(Role * role, int seq_id, int group_id, int param);
	void OnNPCCommitChosed(Role *role,int objid,int param1,int param2);

	NPCMonster* CreateNPCMonster(int npc_id, Scene* scene, const Posi& pos, int battle_mode = 0,const Dir* dir = NULL);
	NPCChosed* CreateNPCChosed(int npc_id, Scene* scene, const Posi& pos, int battle_mode = 0, const Dir* dir = NULL);
	NPCGather * CreateNPCGather(int npc_id, Scene * scene, const Posi& pos);
	NPCMonster* GetNPCMonster(int npc_seq);
	int GetMonsterIdByNpcId(int npc_id);
	NPC* GetServerNPC(int npc_seq);
	NPCGather* GetGather(int npc_seq, Scene * scene = NULL);

	int GenerateNpcSeq();
	void RegisterServerNPC(int seq, NPC* npc);
	void UnRegisterServerNPC(int seq);
	bool IsSeqExist(int seq);
private:
	NpcManager();
	~NpcManager();

	int m_npc_seq_generator;

	ServerNPCMap m_server_npc_map;
};

#endif
