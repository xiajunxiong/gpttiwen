#ifndef __SPECIAL_LOGIC_FANTASY_FB_HPP__
#define __SPECIAL_LOGIC_FANTASY_FB_HPP__

#include "scene/speciallogic.hpp"
#include "servercommon/servercommon.h"
#include "other/fantasyfb/fantasyfbconfig.hpp"
#include "gameworld/protocol/msgcrystalfb.h"
#include "gameworld/item/knapsack.h"

class SpecialLogicFantasyFb :public SpecialLogic
{
public:
	SpecialLogicFantasyFb(Scene* scene);
	virtual ~SpecialLogicFantasyFb();

	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnRoleEnterScene(Role *role);	//!< ���ǽ��볡��
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< �����뿪����

	virtual bool CanDestroy() { return true; }
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);

	virtual void OnTransport(Role *role, SpecialTransport * transport_obj);
	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_FANTASY_FB; }
	virtual bool IsForceEncounterMineMonster();
	virtual bool CanEncounterMineMonster(Role * role);
	virtual bool OnOpenTreasureChest(Role *role, TreasureChest * transport_obj);
	virtual void OnRoleLeaveTeam(Role * role);
	virtual bool IsAutoRecover() { return true; }

	virtual void OnGather(Role *role, int gather_id, int seq_id, const Posi &pos);
	void OnLeaveFBSituation(Role *role, int param1);

	void SetInfo(int layer, Posi transport_pos, std::map<int, int>* level_seq = NULL, std::map<int, std::vector<ItemConfigData> >* item_list = NULL, std::vector<Event_NPC_FB_Cfg> *has_create_npc_vec = NULL);
	void SendSceneInfo(Role *role = NULL);
	void SendSceneObjInfo(Role *role = NULL);
	void SendRewardInfo(Role * role = NULL);

private:
	void CheckFirstPassFB(Role *role);
	void SendNotifyGetItemList(Role *role, int notice = GET_REASON_FANTASY_FB_COUNT_REWARD);
	void CreateNpcOrBox();
	void CreateMonster();
	void CreateTransport(const Posi & pos);
	bool IsLastLayer(int layer = 0);
	void TransportNextLayer(Role *role, SpecialTransport * transport_obj);
	void SendMail(Role * role, ItemDataWrapper * item_info, int count);
	void ItemStack(Role *role, const ItemConfigData *item);
	void TeamGoNextLayer(Team* team, int target_scene_id, int target_scene_key, const Posi &target_pos);
	void ClearSceneBox();

	bool m_is_init;
	int m_layer;
	std::map<int, int> m_level_seq;		//����ø����ȼ����seq

	int m_boss_id;
	int kill_boss_flag;					//boss��ɱ��־ 0:û��ɱ  1:�ѻ�ɱ

	int m_box_num;						//!<�������
	int m_gather_num;					//!<�ɼ�������

	int m_box_num_max;					//!<��������
	int m_gather_num_max;				//!<���ɼ�������

	Posi m_transport_pos;				//!<����������
	std::map<int, std::vector<ItemConfigData> > m_reward_item_list;					//��ֹ���ˢ����,ʹ������ȱ��渱���ĵ���,ͨ��ʱ�ٸ����

	std::map<int, std::vector<SpecialLogicFantasyObjInfo> > m_obj_info_list;		//��¼�����и���npc��λ����Ϣ ��NPC_TYPEΪkey
	std::vector<Event_NPC_FB_Cfg> m_has_create_npc_vec;								//�Ѵ�������NPC�б�
};


#endif
