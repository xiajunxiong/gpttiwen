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

	virtual void OnRoleEnterScene(Role *role);	//!< 主角进入场景
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< 主角离开场景

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
	std::map<int, int> m_level_seq;		//保存该副本等级组的seq

	int m_boss_id;
	int kill_boss_flag;					//boss击杀标志 0:没击杀  1:已击杀

	int m_box_num;						//!<宝箱个数
	int m_gather_num;					//!<采集物数量

	int m_box_num_max;					//!<最大宝箱个数
	int m_gather_num_max;				//!<最大采集物数量

	Posi m_transport_pos;				//!<传送阵坐标
	std::map<int, std::vector<ItemConfigData> > m_reward_item_list;					//防止玩家刷奖励,使用这个先保存副本的道具,通关时再给玩家

	std::map<int, std::vector<SpecialLogicFantasyObjInfo> > m_obj_info_list;		//记录场景中各个npc的位置信息 以NPC_TYPE为key
	std::vector<Event_NPC_FB_Cfg> m_has_create_npc_vec;								//已创建过的NPC列表
};


#endif
