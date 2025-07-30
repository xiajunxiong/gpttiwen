#ifndef __SPECIAL_LOGIC_MI_JING_QI_WEN_HPP__
#define __SPECIAL_LOGIC_MI_JING_QI_WEN_HPP__

#include "scene/speciallogic.hpp"
#include <map>
#include <set>
#include "protocol/msgscene.h"
#include "scene/scene.h"
#include "config/randactivityconfig/impl/randactivitymijingqiwenconfig.hpp"
#include "speciallogiccrystalfb.hpp"

struct BuyInfo
{
	BuyInfo() :buy_times(0) {}
	int buy_times;
};

struct BuyList
{
	BuyList() {}

	BuyInfo m_buy_info[Protocol::SCRAMiJingQiWenShopInfo::MAX_SHOP_ITEM_NUM];
};

struct MiJingItemInfo
{
	MiJingItemInfo() :item_id(0), num(0), is_bind(0), price_type(PRICE_TYPE_0), price(0), buy_times(0), discount(0) {}
	enum PRICE_TYPE
	{
		PRICE_TYPE_0 = 0,//Ԫ��
		PRICE_TYPE_2 = 2,//ͭǮ
	};
	ItemID item_id;
	short num;
	int is_bind;
	int price_type;
	int price;
	int buy_times;
	int discount;
};

struct MiJingShopInfo
{
	MiJingShopInfo() {}

	std::map<int, MiJingItemInfo> shop_info;
};

class SpecialLogicMiJingQiWenFB : public SpecialLogic
{
public:
	SpecialLogicMiJingQiWenFB(Scene* scene);
	virtual ~SpecialLogicMiJingQiWenFB();

	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnRoleEnterScene(Role *role);	//!< ���ǽ��볡��
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);

	virtual bool CanDestroy() { return true; }
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);

	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_RA_MIJINGQIWEN; }
	virtual bool IsForceEncounterMineMonster();
	virtual bool CanEncounterMineMonster(Role * role);
	virtual bool CanUseItem(Role *role, ItemID item_id, int num);
	virtual void OnUseItem(Role *role, ItemID item_id, int num);
	virtual void OnTransport(Role *role, SpecialTransport * transport_obj);
	virtual void OnRoleLeaveTeam(Role * role);
	virtual bool IsAutoRecover() { return false; }
	virtual void OnNPCChosedReward(Role *role, ObjID npc_objid, int option_index);
	virtual void OnGather(Role *role, int gather_id, int seq_id, const Posi &pos);
	virtual void OnNPCChosedTalk(Role *role, ObjID npc_objid);
	void OnRoleAnswerQuestion(Role * role, ObjID npc_objid, int answer_id);
	void OnRoleBuyItem(Role *role, ObjID npc_objid, int item_idx);

	void ItemStack(Role *role, const ItemConfigData *item);

	void SetInfo(int layer, int level_seq, Posi transport_pos, std::vector<new_Event_NPC_cfg> * has_create_npc_vec = NULL, std::map<int, std::vector<ItemConfigData> >* item_list = NULL);
	void SendSceneInfo(Role *role = NULL);
	void SendSceneObjInfo(Role *role = NULL);
	void SendQuestionInfo(ObjID npc_objid, Role *role = NULL);
	void SendShopInfo(Role *role);
private:
	void CreateNpc();
	void CreateMonster();
	void CreateBox();
	void CreateTransport();
	void CreateLeaveTransport(const Posi & pos);
	bool IsLastLayer(int layer = 0);
	void TransportNextLayer(Role *role, SpecialTransport * transport_obj);
	void RoleAloneGetRewardByCfg(Role *role, const MiJingQiWenFbNPCRewardCfg * npc_cfg, ObjID *targer_obj = NULL);
	void RoleTeamGetRewardByCfg(Role *role, const MiJingQiWenFbNPCRewardCfg * npc_cfg, ObjID *targer_obj = NULL);
	void AddRoleBuff(Role *role, const BuffListConfig*  buff_cfg);
	void SendBuffEffectToRole(Role *buff_geter, int buff_type);
	void TeamGoNextLayer(Team* team, int target_scene_id, int target_scene_key, const Posi &target_pos);


	bool m_is_init;
	int m_layer;
	int m_level_seq;

	int m_monster_num;		//!<���ָ���
	int m_box_num;			//!<�������
	int m_npc_num;			//!<NPC����
	int m_talk_npc_num;		//!<��������NPC����

	int m_boss_obj_id;

	int m_gouhuo_num;		//!<��������
	int m_coin_num;			//!<ͭ������
	int m_gather_num;		//!<�ɼ�������

	int m_monster_num_max;		//!<������ָ���
	int m_box_num_max;			//!<��������
	int m_npc_num_max;			//!<���NPC����
	int m_gouhuo_num_max;		//!<�����������
	int m_coin_num_max;			//!<���ͭ������
	int m_gather_num_max;		//!<���ɼ�������

	Posi m_transport_pos;

	std::map<int, std::vector<SpecialLogicCrystalObjInfo> > m_obj_info_list;			// ����Ŀ��λ�ü�¼ ��Ŀ������Ϊkey

	std::set<int> m_already_rand_scene_id;

	//�Ѵ�������NPC�б�
	std::vector<new_Event_NPC_cfg> m_has_create_npc_vec;

	std::map<int, std::vector<ItemConfigData> > m_item_list;					//�ݴ�һЩ�����ͼ�����Ľ���

	MiJingShopInfo m_shop_info;//�̵���Ϣ
	std::map<UserID, BuyList> scene_user_shop_map;//����̵���Ϣ

	std::map<ObjID, Protocol::MiJingQuestion> m_question;

	std::map<int, time_t> kick_out_tamp;
};

#endif