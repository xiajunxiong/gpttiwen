#ifndef __SPECIAL_LOGIC_GUILD_HONOR_BATTLE_HPP__
#define __SPECIAL_LOGIC_GUILD_HONOR_BATTLE_HPP__

#include "servercommon/guildhonorbattledef.hpp"
#include "scene/speciallogic.hpp"
#include <map>
#include <set>
#include <vector>

enum GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE
{
	GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE_DEFAULT,
	GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE_BATTLE,
	GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE_FINISH,
};

struct GuildHonorBattleRoleInfo
{
	GuildHonorBattleRoleInfo()
	{
		role_id = 0;
		//role_battle_state = GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE_DEFAULT;
	}

	GuildHonorBattleRoleInfo(int role_id)
	{
		this->role_id = role_id;
		//role_battle_state = GUILD_HONOR_BATTLE_ROLE_BATTLE_STATE_DEFAULT;
	}

	bool operator==(const GuildHonorBattleRoleInfo & other)
	{
		if (other.role_id == this->role_id)
		{
			return true;
		}

		return false;
	}

	int role_id;
	//int role_battle_state;
};

struct GuildHonorBattleGuildInfo
{
	GuildHonorBattleGuildInfo()
	{
		guild_id = 0;
		ArrayFillZero(guild_name);
		finish_reward_type = ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_INVAILD;
	}

	bool IsNobodyEachZone();
	void AutoAssigned();

	int guild_id;
	GuildName guild_name;
	int finish_reward_type;

	std::map<int, long long> zone_battle_total_damage;  //每个区域战斗伤害，用来强制结束的判断谁赢
	std::map<int, int> zone_remain_role_num;		//剩余人数
	std::map<int, int> zone_id_to_battle_role_index;		//每个区域下一个轮到谁战斗 [index)
	std::map<int, std::vector<GuildHonorBattleRoleInfo> > zone_role_info;		//3个区域所在玩家的信息
	std::map<int, int> role_to_zone_id;		//在本次活动中，玩家在准备期间退出准备场景时，需要保存其所在分区信息，再次进入后依旧加入退出前所在的战区
};

enum GUILD_HONOR_CHANGE_RESON
{
	GUILD_HONOR_CHANGE_RESON_MOV,		//移动 ，zone_id 移动到的区域
	GUILD_HONOR_CHANGE_RESON_DEL,		//删除
};

struct GuildHonorBattleFigtherInfo;

class SpecialLogicGuildHonorBattle : public SpecialLogic
{
public:
	SpecialLogicGuildHonorBattle(Scene* scene);
	virtual ~SpecialLogicGuildHonorBattle();

	virtual void OnRoleEnterScene(Role *role) ;					//!< 主角进入场景
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< 主角离开场景
	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight * ack);
	virtual bool CanDestroy();
	virtual bool CanLeaveGuild() { return false; }
	virtual bool CanEnterTeam(Role * role, Team * team, bool is_from_create);
	virtual bool CanInvitationRole(Role * role, int uid);

	void AutoAssignedRole(Role * role);
	void AssignedRole(Role * role, int zone_id, int role_id);

	void SetGuildInfo(int win_rank, int curr_round, const GuildHonorBattleFigtherInfo * guild1, const GuildHonorBattleFigtherInfo * guild2);
	void OnActivityGuildHonorBattleRoundFinish();
	void StartBattle(long long battle_length_of_time);

	void BattleAddBackUp(int battle_id,int need_attacker_role, int need_defender_num,int zone_id);
	void BattleChangeBackupRoleNumNotice(int battle_id, int attacker_backup_exist_role_num, int defender_backup_exist_role_num, int zone_id);
	void BattleStateChangeNotice(bool is_attack,std::vector<int> start_battle_role_id, std::vector<int> battle_end_role_id, int zone_id);	//start_battle_role_id 增员列表开始战斗role_id  ||  battle_end_role_id 战斗完成人员role-id
	void BattleRoundTotalDamage(long long ack_total_damage, long long def_total_damage, int zone_id);
	void CalZoneRemainRoleNum(bool is_attack, int zone_id = -1);

	void SendRoleEnterScene(Role * enter_role);        //通知其他人有人进来了
	void SendZoneChangeInfo(int guild_id, int role_id, int zond_id, int reason = GUILD_HONOR_CHANGE_RESON_MOV);
	void SendGuildZoneInfo(int guild_id, Role *role = NULL);
	void SendGuildZoneBattleRoleNumInfo(Role * role = NULL);  //战斗和观战界面，增加一个UI，显示“剩余未参战人数”
	void SendZoneBattleResult(int zone_id);
	void SendAllZoneBattleResult(Role * role = NULL);
	void SendBattleStateBuyRoleId(int role_id);
	void SendBattleState(Role * role );

	bool HasRole(int role_id);
private:
	void SetResult(int zone_id, int winner_guild_id, const char * reason);
	void SetZoneBattleId(int zone_id, int battle_id);
	void CheakFinish(bool is_force = false);
	void SendFinalBattleResult();
	void SendReward();
	GuildHonorBattleGuildInfo * GetGuildHonorBattleGuildInfo(int guild_id);
	int GetRoleBattleState(int role_id);

	void AutoAssignedBeforStartBattle();		//开始战斗前自动分配不在区域里的人
	void CheckRoleReallyInBattle(Role* role);

	std::string GetBaseInfoString();

	bool m_is_finish;
	bool m_is_close;
	bool m_start_battle;
	
	int m_battle_id[GUILD_HONOR_BATTLE_MAX_ZONE];		 //每个区域战斗id,给别人观战
	int m_battle_result[GUILD_HONOR_BATTLE_MAX_ZONE];		//0是未分出胜负。-1是平手，不然就是胜利的guild_id

	int m_win_rank;
	int m_round;
	int m_win_guild_id;			//无法判断就用谁赢就用这个，大乱斗名单排名高的赢
	std::map<int, GuildHonorBattleGuildInfo *> m_guild_battle_info;
	GuildHonorBattleGuildInfo m_guild_battle_info1;
	GuildHonorBattleGuildInfo m_guild_battle_info2;

	std::map<int, int> m_role_battle_state;
};

#endif