#ifndef __SPECIAL_LOGIC_CRYSTAL_FB_HPP__
#define __SPECIAL_LOGIC_CRYSTAL_FB_HPP__

#include "scene/speciallogic.hpp"
#include <map>
#include <set>
#include "other/rolecrystalfb/rolecrystalfbconfig.hpp"
#include "obj/character/robot.h"

struct SpecialLogicCrystalUseItemInfo
{
	SpecialLogicCrystalUseItemInfo()
	{
	}

	std::map<int, int> use_type_to_use_times;		//��ͬʹ��������Ʒ��Ӧʹ�ô���
};

struct SpecialLogicCrystalObjInfo
{
	SpecialLogicCrystalObjInfo():npc_id(0),obj_type(0),obj_id(0),pos_x(0),pos_y(0)
	{
	}
	int npc_id;
	short obj_type;
	ObjID obj_id;
	short pos_x;
	short pos_y;
};


class SpecialLogicCrystalFB : public SpecialLogic
{
public:
	SpecialLogicCrystalFB(Scene* scene);
	virtual ~SpecialLogicCrystalFB();

	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnRoleEnterScene(Role *role);	//!< ���ǽ��볡��
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);

	virtual bool CanDestroy() { return true; }
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);

	virtual int GetMineMonsterBattleMode() { return battle_mod; }
	virtual bool IsForceEncounterMineMonster();
	virtual bool CanEncounterMineMonster(Role * role);
	virtual bool CanUseItem(Role *role, ItemID item_id, int num);
	virtual void OnUseItem(Role *role, ItemID item_id, int num);
	virtual void OnTransport(Role *role, SpecialTransport * transport_obj);
	virtual bool OnOpenTreasureChest(Role *role, TreasureChest * transport_obj);
	virtual void OnRoleLeaveTeam(Role * role);
	virtual bool IsAutoRecover() { return false; }
	virtual void OnNPCChosedReward(Role *role, ObjID npc_objid,int option_index);
	virtual void OnGather(Role *role, int gather_id, int seq_id, const Posi &pos);
	virtual void OnNPCChosedTalk(Role *role, ObjID npc_objid);

	static bool IsBattleModeBelongToCrystalFB(int battle_mode);

	void ItemStack(Role *role, const ItemConfigData *item);
	bool IsHasConsume(int uid);

	void SetInfo(int layer, int level_seq, Posi transport_pos, int coster, int team_max_level, std::vector<Event_NPC_cfg> * has_create_npc_vec = NULL, 
		std::map<int, std::vector<ItemConfigData> >* item_list = NULL, std::set<int> * consume_uid_list = NULL);
	void OnAddConsumeUid(int uid);
	void SendUseItemInfo(Role * role);
	void OnReqNextLevel(Role *role) const;
	void SendSceneInfo(Role *role = NULL);
	void SendSceneObjInfo(Role *role = NULL);
	void SendAllSceneObjInfo(Role *role = NULL);

	int GetLevelSeq() { return m_level_seq; }
private:
	void CheckFristPassFb(Role *role,std::vector<int> * drop_list);
	void CreateNpc();
	void CreateMonster();
	void CreateBox();
	void CreateTransport();
	void CreateLeaveTransport(const Posi & pos);
	bool IsLastLayer(int layer = 0);
	void TransportNextLayer(Role *role, SpecialTransport * transport_obj);
	void RoleAloneGetRewardByCfg(Role *role, const CrystalFbNPCRewardCfg * npc_cfg);
	void RoleTeamGetRewardByCfg(Role *role, const CrystalFbNPCRewardCfg * npc_cfg);

	void AddRoleBuff(Role *role, const BuffListConfig*  buff_cfg);
	void RecoveryRoleStatus(Role *role, const BuffListConfig*  buff_cfg);
	void RecoveryRobotStatus(Robot *robot, const BuffListConfig*  buff_cfg);

	void SendBuffEffectToRole(Role *buff_geter,int buff_type);
	void TeamGoNextLayer(Team* team, int target_scene_id, int target_scene_key, const Posi &target_pos);
	void SetBattleMode(int level_seq);

	int battle_mod;
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

	int consumable_player;
	Posi m_transport_pos;
	std::set<int> m_consume_uid_list;			//��Ӣ����,��¼����uid�б�
	int m_open_box_consume_uid;					//�����ֱ�������UID(��Ӣ����ʹ��)

	std::map<int, std::vector<SpecialLogicCrystalObjInfo> > m_obj_info_list;			// ����Ŀ��λ�ü�¼ ��Ŀ������Ϊkey
	std::vector<SpecialLogicCrystalObjInfo> all_npc_list;						// ����NPC��Ϣ

	std::map<int, SpecialLogicCrystalUseItemInfo> m_role_id_to_use_info;		//��Ʒ��ʹ����Ϣ���
	std::set<int> m_already_rand_scene_id;

	//�Ѵ�������NPC�б�
	std::vector<Event_NPC_cfg> m_has_create_npc_vec;

	std::map<int, std::vector<ItemConfigData> > m_item_list;					//�ݴ�һЩ�����ͼ�����Ľ���

	int m_team_max_level;			// ������ߵȼ�

	int m_battle_mode;
	int m_normal_monster_battle_mode;
};

#endif