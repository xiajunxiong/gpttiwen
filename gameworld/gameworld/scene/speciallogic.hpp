#ifndef __SPECIAL_LOGIC_HPP__
#define __SPECIAL_LOGIC_HPP__

#include "servercommon/serverdef.h"
#include "changescenedelayer.h"
#include "servercommon/internalprotocal/battleprotocol.h"
#include "other/attributelist.hpp"
#include "servercommon/teamdef.hpp"
#include <set>

class Role;
class Scene;
class Obj;
class NPCGather;
class SpecialTransport;
class TreasureChest;
class Team;

class SpecialLogic
{
public:
	SpecialLogic(Scene *scene);
	virtual ~SpecialLogic() {};

	Scene* GetScene() const { return m_scene; }
	int GetSceneType();
	static SpecialLogic* CreateSpecialLogic(Scene* scene, int scene_type);

	virtual void Update(unsigned long interval, time_t now_second) {}

	virtual void OnAddObj(Obj *obj) {}								//!< 新建obj时
	virtual void OnRemoveObj(Obj *obj) {}							//!< obj消失时

	virtual void OnRoleEnterScene(Role *role) {};					//!< 主角进入场景
	virtual void OnRoleLeaveScene(Role *role, bool is_logout) {}	//!< 主角离开场景
	
	virtual bool CanJoinOtherActivity() { return true; }			//!< 是否允许加入其它活动

	//采集,如果是直接给玩家则put/putlist的reason需要使用PUT_REASON_GATHER_OBJECT 如果是暂存,则应该先调用采集技能CheckIsGather接口,先触发采集技能,最后给玩家时put/putlist的reason不要使用前者
	virtual void OnGather(Role *role, int gather_id, int seq_id, const Posi &pos) {}    
	virtual void OnStartGather(Role *role, int gather_id, int seq_id, const Posi &pos) {}
	virtual void OnAddExp(Role* role, int add_exp, int reason) {}
	virtual void OnFish(Role* role) {}

	virtual bool CanDestroy() = 0;									//!< 场景是否能被摧毁 必须实现的逻辑

	virtual bool CanTransport(Role *role) { return true; }			//!< 是否能在场景里传送
	virtual bool CanGather(Role* role, NPCGather* gather) { return true; }	//!< 是否能在场景里采集
	virtual bool CanMoveOn(Role* role) { return true; }						//!< 是否允许玩家移动
	virtual bool CanUseItem(Role *role, ItemID item_id, int num) { return true; }
	virtual bool CanLeaveGuild() { return true; }					//!< 是否允许离开家族
	virtual bool CanFish(Role * role) { return false; } 			//!< 是否能在场景中钓鱼

	virtual void OnUseItem(Role *role, ItemID item_id, int num) {}
	virtual void OnGetItemByGather(Role * role, ItemID item_id, int num) {}

	virtual bool IsDropCard() { return true; }						//!< 是否掉落图鉴卡
	virtual bool IsLevelOneMonsterOccur() { return true; }			//!< 1级怪是否出现 (用于抓宠物）

	virtual void OnLeaveFB(Role *role) {};							//!< 角色主动离开副本
	virtual void OnNPCChosedReward(Role *role,ObjID npc_objid,int option_index) {};	//!< NPC选项操作直接奖励
	virtual void OnNPCChosedTalk(Role *role, ObjID npc_objid) {};//!< NPC对话
	virtual bool OnGatherActiveAdventures() { return false; }		//!< 是否激活奇遇任务

	virtual bool OnTimeout() { return false; }

	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack) {}
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack) {}
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack) {}

	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader) { return false; }

	//角色可开始战斗
	struct CanRoleStartFightParam
	{
		CanRoleStartFightParam()
		{
			battle_mode = 0;
		}
		int battle_mode;
	};
	virtual bool CanRoleStartFight(Role * role, CanRoleStartFightParam &fight_param) { return true; };

	void DelayKickOutRole(int role_id, int reason = CHANGE_SCENE_REASON_INVALID, int delay_ms = 0);
	void DelayKickOutRole(Role *role, int reason = CHANGE_SCENE_REASON_INVALID, int delay_ms = 0);
	void DelayKickOutAllRole(int reason = CHANGE_SCENE_REASON_INVALID, int delay_ms = 0);

	void ClearAllNpcMonster();

	virtual bool CanChallengeOthers(Role * role, Role * target) { return true; }

	/**
	* \brief 获取场景的战斗倍速设定
	* \param role 玩家实例
	* \param speed 用于返回倍速设定
	* \param is_force_speed 用于返回是否强制倍速设定，玩家在战斗中无法更改
	* \return 场景是否有战斗倍速设定
	*/
	virtual bool GetBattleAnimationSpeedType(Role * role, ARG_OUT int* speed, ARG_OUT bool* is_force_speed) { return false; }
	
	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_MINE_MONSTER; }

	/**
	* @brief 是否允许玩家创建或加入队伍。适用于加入队伍有特殊逻辑的场景使用（例如限制场景内不允许组队、限制只有满足特定条件的玩家入队等）
	* @note 注意，此函数的 role和team 参数都有可能是 NULL，所以在使用的时候请务必考虑 role和team 参数为 NULL 的情况；
	*		目前已知 team 参数为空时的检测时机为：玩家创建队伍时、自动匹配以加入其它队伍时；
	*		目前已知 role 参数为空时的检测时机为：队伍在副本中并处于匹配状态
	*/
	virtual bool CanEnterTeam(Role * role, Team * team, bool is_from_create);
	virtual bool CanInvitationRole(Role *role, int uid) { return true; }
	virtual int CreateTeamType() { return TEAM_TYPE_DEFAULT; }
	virtual bool CanChangeTeamLeader(Role * role, Team * team) { return true; }
	virtual void OnRoleLeaveTeam(Role * role) {}
	virtual void OnTeamDismiss(Role * leader_role, Team * team) {}	//!< 整个队伍解散流程中只由队长调用一次，无论队伍里有多少名队员
	virtual void OnAOIEnterRole(Role * role, Role * target_role) {}	//!< 目标角色进入我的视野

	/**
	* @brief 场景是否强制遇暗怪，如果是的话，会在进入场景时开始倒计时
	* @note 如果强制遇暗怪，还需要在OnRoleEnterScene中开启遇暗怪, 且在OnRoleLeaveScene中设置还原进入前的暗怪设置
	*/
	virtual bool IsForceEncounterMineMonster() { return false; }

	/**
	* @brief 判断某玩家是否遭遇暗怪
	* @note 放在倒计时计算前的最后一个判断，以减少调用次数
	*/
	virtual bool CanEncounterMineMonster(Role * role) { return true; }
	virtual long long GetMineMonsterEncounterInterval(Role * role);

	virtual void OnTransport(Role *role, SpecialTransport * transport_obj) {}
	virtual bool OnOpenTreasureChest(Role *role, TreasureChest * transport_obj) { return true; }

	virtual const AttributeList & ReCalcAttrPer(AttributeList &base_add, const AttributeList & cpy_attr_list);
	virtual bool IsAutoRecover() { return true; }				//是否可以自动回复血量魔量

	void UseGoldRecoveryHp(Role * role,int param1, int param2,int param3);
	void UseGoldRecoveryMp(Role * role, int param1, int param2, int param3);

	//---能否转职
	virtual  bool CanChangeProf(Role * role);
protected:
	bool IsAllMonsterDead();
	void CheckSceneMonsterStatus(unsigned long interval, time_t now_second);
	virtual void OnAllMonsterDead() {}

	void SyncHpMp(const char* sync_data);

	//决斗 战斗完成
	void DuelOnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	void OnDuelFightOver(std::set<int> uid_list, int duel_type, bool is_attack, bool is_win, int level_diff, bool target_is_team, bool target_is_red, GameName name, UInt64 coin);

	Scene * m_scene;
	AttributeList m_attr_inc;

	unsigned int m_last_check_monster_status_time;				    // 上次检查场景状态时间
};

#endif