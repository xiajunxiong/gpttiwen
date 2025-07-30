#ifndef __NPC_CHOSED_H__
#define __NPC_CHOSED_H__

#include "obj/character/npc.h"
#include "npc/npcpool.h"

struct NpcShopItem
{
	struct NpcShopItem_data
	{
		NpcShopItem_data():item_id(0),num(0),is_bind(0), money_type(0),price(0),discount(0){}
		int item_id;
		int num;
		short is_bind;
		short money_type;
		int price;
		int discount;
	};
	NpcShopItem():num(0), max_num(0){}
	NpcShopItem_data item;
	int num;
	int max_num;
};

class NPCChosed : public NPC
{
public:
	NPCChosed();
	virtual ~NPCChosed();
	enum NPC_CHOSED_REQ//!< 对话NPC操作
	{
		NPC_CHOSED_REQ_TALK = 0,	//!< 对话（点击请求下发信息）
		NPC_CHOSED_REQ_OPTION1 = 1,	//!< 选项1
		NPC_CHOSED_REQ_OPTION2 = 2,	//!< 选项2
	};

	void* operator new(size_t c);
	void operator delete(void *m);

	int InitNPCChosed(int battle_mode);

	void SetBattleMode(int mode) { m_battle_mode = mode; }

	int GetBattleMode() const { return m_battle_mode; }

	bool IsMonsterLock() const { return m_is_lock; }
	void SetMonsterLock(bool is_lock) { m_is_lock = is_lock; }
	void SetBattleID(int battle_id) { m_battle_id = battle_id; }
	int GetBattleID() const { return m_battle_id; }
	bool IsNPCMineMonster() const { return NPC_MINE_MONSTER_NPC_ID == m_npc_id; }

	bool CanConsume(Role * gatherer);

	void SetRemoveAfterGatherFlag(bool remove_flag);

	//------------对话
	bool SendTalkInfoToRole(Role* role);
	bool ChosedOption(Role * role,int option_index);//选项操作

	void SendShopInfo(Role* role);
	bool OnBuyItem(Role* role,int item_index,int num);//购买商品
	void AddBuyItem(NpcShopItem* buy_item);//增加商品
	void SetSendMember(bool send_to_member) { m_send_to_member = send_to_member; }
	void SetSendOption(bool is_send_option) { m_is_send_option = is_send_option;}
protected:
	void RemoveAfterBuy();

	int m_battle_mode;

	bool m_is_lock;
	bool m_has_talk;
	bool m_send_to_member; //是否发对话信息给队员
	bool m_is_send_option;	//是否发送选项
	int m_battle_id;
	int shop_num;
	std::map<int, NpcShopItem> shop_list;
};

#endif