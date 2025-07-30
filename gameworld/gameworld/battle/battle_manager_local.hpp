#ifndef __BATTLE_MANAGER_LOCAL_HPP__
#define __BATTLE_MANAGER_LOCAL_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/internalprotocal/battleprotocol.h"

#include <map>
#include <deque>
#include <set>

struct RoleBattleInfo
{
	RoleBattleInfo() : role_id(0), battle_id(0), battle_server_index(0), is_ob(false) {}

	int role_id;
	int battle_id;
	int battle_server_index;
	bool is_ob;
};

class Role;
class Team;
class BattleBusinessLocal;
class Character;
struct BattleData_Business;

struct BattleLocalInfo
{
	BattleLocalInfo() : battle_id(0), battle_mode(0), monster_group_id(0), monster_gen_id(0), battle_server_index(0), role_count(0),
		scene_id(0), scene_key(0), is_battle_start(false), local_info_create_time(0u), 
		last_check_start_failed_time(0u), local_impl(NULL)
	{
		memset(role_list, 0, sizeof(role_list));
	}

	void AddObserver(int role_id)
	{
		observer_set.insert(role_id);
	}

	void RemoveObserver(int role_id)
	{
		observer_set.erase(role_id);
	}

	int battle_id;
	short battle_mode;
	int monster_group_id;
	int monster_gen_id;
	int battle_server_index;
	
	short role_count;
	int role_list[BATTLE_MAX_ROLE_NUM]; // 永远把leader放第0位

	// 记录战斗是发生在什么场景
	int scene_id;
	int scene_key;

	// 记录local_info是什么时候创建的，如果长时间收不到Start回包，那么应向BattleManager询问战斗是否开启了
	bool is_battle_start;
	unsigned int local_info_create_time;
	unsigned int last_check_start_failed_time;	// 上次检查开始失败的时间戳

	BattleBusinessLocal* local_impl;

	std::set<int> observer_set;
};

enum START_BATTLE_RESULT
{
	START_BATTLE_RESULT_SUCC = 0,
	RESULT_ATTACKER_FAIL,			//攻击方进入失败
	RESULT_DEFENDER_FAIL,			//防守方进入失败
	RESULT_BOTH_FAIL,				//双方进入失败
};

class Robot;
class BattleManagerLocal
{
public:
	typedef std::map<int, RoleBattleInfo> RoleBattleInfoMap;
	typedef std::map<int, BattleLocalInfo> BattleLocalInfoMap; // battle_id is key
	typedef std::map<int, int> BattleIdToLocalHash;

public:
	static BattleManagerLocal& GetInstance();

	void Update();

	// 战斗请求
	bool StartDemoFightReq(Role* role, int npc_demo_seq = 0, int battle_mode = BATTLE_MODE_DEMO, BattleData_Business* in_business_data = NULL);
	bool StartMonsterFightReq(Role* role, int monster_group_id, int battle_mode, int monster_gen_id);
	bool StartTeamMonsterFightReq(Role* role, int monster_group_id, int battle_mode, int monster_gen_id);
	bool StartBattleModeFightReq(Role * role, short battle_mode, int param1, int param2, int param3, bool is_from_server_req, int use_level = 0);
	bool StartChallengeOthersReq(Role * role, int enemy_uid, int battle_mode);
	bool StartMineMonsterFightReq(Role* role, bool is_real_mine, int head_monster_id = 0, bool is_all_lv1 = false, int npc_seq_id = -1);
	bool StartSinglePVPPraticeReq(Role* attacker, Role* defender);
	bool StartEsortMonsterFightReq(Role* role);
	bool StartTeamPVPReq(int attacker_num, Role** attacker_list, int defender_num, Role** defender_list, int battle_mode);
	int StartGuildHonorPVPReq(int zone_id, long long battle_length_of_time, std::vector<Role *> attacker_list, std::vector<Role *> defender_list, std::vector<Role *> attacker_observer_list, std::vector<Role *> defender_observer_list, int ARG_OUT * battle_id_ptr);
	int StartQingYuanShengHuiReq(std::vector<Role *> attacker_list, std::vector<Role *> defender_lis);
	bool StartSingleArenaReq(Role * role, int defender_uid, int defender_buff_id, int max_round, const RoleBattleData & defender_data);
	bool StartCloudArenaReq(Role* role, const RoleBattleData& attacker_data, int defender_uid, int defender_buff_num, int* defender_buff_list, int max_round, int max_time_s, const RoleBattleData & defender_data, int allow_surrender_round);
	int StartChiefElectionPVPReq(Role* attacker, Role* defender, int which_prof_match, int max_round, int time, int field_buff_id);
	int StartChiefPeakBattlePVPReq(Role* attacker, Role* defender, int max_round, int time);
	bool StartHuanJieZhanChangPVPReq(std::vector<Role *> attacker_list, std::vector<Role *> defender_list, const std::vector<RoleBattleData> & attacker_robot_list, const std::vector<RoleBattleData> & defender_robot_list, int max_round, int fight_seq);
	
	bool StartPlatformBattleReq(Role* attacker, int atk_pet_num,
				    PetBattleData* atk_pet_list, int * atk_pet_pos_info, int atk_partner_num, PartnerBattleData* atk_partner_list, int * atk_partner_pos_info,
				    UserID def_user_id, int def_pet_num, PetBattleData* def_pet_list, int *def_pet_pos_info, int def_partner_num,
				    PartnerBattleData* def_partner_list, int * def_partner_pos_info, const BattleData_Business& business_data);

	bool StartPartnerPVEReq(Role* attacker, int atk_partner_num, PartnerBattleData* atk_partner_list, int * atk_partner_pos_info, int monster_group_id, const BattleData_Business& business_data);
	bool StartSinglePVPReq2(Character* attacker, Character* defender, int battle_mode, const BattleData_Business& business_data, ARG_OUT bool* attacker_failed, ARG_OUT bool* defender_failed);

	bool StartSingleFightShadowReq(Role* role, const RoleBattleData& defender_data, 
		int battle_mode, ARG_IN const BattleData_Business& business_data);
	bool StartMultiFightShadowReq(int attacker_num, Role** attacker_list, int defender_num, 
		const RoleBattleData* defender_data_list, int battle_mode, const BattleData_Business& business_data);

	bool StartSpecialBattleJoyReq(Role *attacker, Role* defender);

	bool SendQieCuoReqToTarget(Role * role, int enemy_uid);
	bool OnQieCuoRequestReply(Role * role, int target_uid, bool is_agree);
	void SendQieCuoDequeFront(Role * role);

	void OnUserLogin(const UserID& user_id);
	void OnRoleLogout(Role* role);
	void OnBattleServerDisconnect(int battle_server_index); // 如果断开了连接，应重置玩家战斗状态

	bool IsBattleServerConnected(int battle_server_index);
	bool HasBattleServerConnected();
	bool IsRoleInBattle(int role_id);	
	
	bool PrepareFinishReq(Role* role);
	bool AnimationFinishReq(Role* role);
	bool CountDownFinishReq(Role* role);
	bool ForceFinishFightReq(Role* role);
	bool ForceFinishFightReq2(Role * role, bool is_notice = false);
	void ForceFinishBattleMode(int  mode, int param1 = 0, int specify_battle_idx = -1);
	void BattleGiveupReq(Role* role);
	void BattleSurrenderReq(Role* role);

	bool RoleOperateReq(Role* role, int operate_type, int param1, int param2, int param3, int param4);
	bool RoleOBReq(Role* role, int watch_uid, bool is_forbid_quit_ob = false);
	bool RoleOBSideReq(Role* role, int battle_id, short watch_side);
	bool RoleBattleRealTimeCommentReq(Role* role, const char* msg, unsigned int length);
	bool RoleModifyAnimationSpeedReq(Role* role, int speed_type);
	bool RoleSetCustomInstructionReq(Role* role, const char* istr, short grid_idx);
	bool RoleSkipBattleReq(Role* role);
	bool RoleFastForwardBattleReq(Role* role);

	bool JoySeeksJavorsOperateReq(Role* role, int operate_type, int param1, int param2, int param3, int param4,int param5);

	// battle返回
	void OnFightStart(battlegameprotocol::BattleGameStartFightAck* ack);
	void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	void OnFightSpecialFinish(battlegameprotocol::BattleGameSpecialFightResultAck* ack);
	void OnBattleForceFinish(battlegameprotocol::BattleGameForceFinishFight* ack);
	void OnFightStartFailed(battlegameprotocol::BattleGameStartFightFailed* ack);
	void OnCheckFightStartHasFailed(battlegameprotocol::BattleGameCheckBattleHasFailAck* ack);
	void OnBattleInstanceReleaseNotice(int battle_id);
	void OnForceFinishBattleModeResp(battlegameprotocol::BattleGameForceFinishBattleModeResp* resp);
	void SendBattleOperateFailNotice(Role* role, int reason);
	
	// 战斗中回传
	void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* msg);
	void OnRoleSetAuto(int role_id, int is_auto);
	void OnRoleSetAutoMove(int role_id, short first_move_type, short first_skill_id, short first_skill_level, short second_move_type, short second_skill_id, short second_skill_level);
	void OnRoleUseItem(int role_id, short item_column, short item_index, ItemID item_id);
	void OnRoleSealPet(int role_id, int monster_id, int monster_group_id, int pet_id, ItemID seal_card_item_id, bool is_succ, bool is_sure_give_pet_on_succ, bool is_auto_seal_pet, int battle_mode, bool is_catch_bind_pet);
	void OnRoleInstKillMonster(int role_id, int monster_id, ItemID inst_kill_item_id, bool is_succ);
	void OnCheckRoleReallyInBattle(int role_id, bool is_in_battle, int battle_id, int battle_server_idx);
	void OnGuildHonorFightRequestBackup(battlegameprotocol::BattleGameGuildHonorFightRequestBackup* msg);
	void OnGuildHonorFightRoleDieNotice(battlegameprotocol::BattleGameGuildHonorRoleDieNotice* msg);
	void OnGuildHonorFightRoleStateChangeNotice(battlegameprotocol::BattleGameGuildHonorRoleStateChangeNotice* msg);
	void OnGuildHonorFightSyncTotalDamage(battlegameprotocol::BattleGameGuildHonorSyncTotalDamage* msg);
	void OnRoleReinforceBattle(int role_id, int battle_id);
	void OnRoleReinforceBattleEnd(int role_id, int battle_id);
	void OnRoleSyncOBStatus(battlegameprotocol::BattleGameRoleSyncOBStatus* msg);

	const RoleBattleInfo* GetRoleBattleInfoConst(int role_id);
	const BattleLocalInfo* GetBattleLocalInfoConst(int battle_id);

	static bool IsBattleModeMustInTeam(short battle_mode, short* out_member_num_at_least, short * min_level , short *real_player_num_at_least);
	static bool IsBattleModeMustNotInTeam(short battle_mode);

	void CheckRoleReallyInBattle(Role* role, const char* func);
	static int GetBattleModeSceneBuff(Role * role,  int battle_mode);
	static int GetBattleModeNoticeNum(Role * role, int battle_mode);

	void ForceStopRoleOB(Role* role);
	void OnUserLoginOriginGame(int role_id);
	bool CheckRoleCanStartBattle(int role_id, int battle_mode);
private:
	bool StartSinglePVEReq(Role* role, int monster_group_id, int battle_mode, const BattleData_Business& business_data);
	bool StartTeamPVEReq(Team* team, int monster_group_id, int battle_mode, const BattleData_Business& business_data);
	bool StartSinglePVPReq(Role* attacker, Role* defender, int battle_mode, const BattleData_Business& business_data);
	int StartIntSinglePVPReq(Role* attacker, Role* defender, int battle_mode, const BattleData_Business*  business_data);
	int StartIntSinglePVPReqByRoleId(int attacker,int defender, int battle_mode, const BattleData_Business * business_data);
	bool StartMultiPVPReq(int attacker_num, Role ** attacker_list, int defender_num, Role ** defender_list, int battle_mode, const BattleData_Business & business_data, int attacker_observer_num = 0, Role ** attacker_observer_list = NULL, int defender_observer_num = 0, Role ** defender_observer_list = NULL, int ARG_OUT * out_battle_id_ptr = NULL);
	int StartMultiPVPReq2(std::vector<Role *>  attacker_list, std::vector<Role *> defender_list, const  std::vector<RoleBattleData> & attacker_robot_list, const  std::vector<RoleBattleData> & defender_robot_list, int battle_mode, const BattleData_Business & business_data, int ARG_OUT * out_battle_id_ptr = NULL);
	bool StartSpecialPVPReq(Role* attacker, Role* defender, int battle_mode, const BattleData_Business& business_data);
	bool StartSingleFightShadowReq(Role* role, const RoleBattleData& attacker_data, const RoleBattleData& defender_data,
		int battle_mode, const BattleData_Business& business_data);

	bool RoleUseItemReq(int battle_server_index, Role* role, int move_idx, int target_grid_idx, int item_column, int item_index);
	bool RoleSealPetReq(int battle_server_index, Role* role, int move_idx, int target_grid_idx, int seal_card_type, int monster_id);
	bool RoleInstKillMonsterReq(int battle_server_index, Role* role, int move_idx, int target_grid_idx, int monster_id);

	void OnRoleStartFight(int role_id, int battle_id);
	void OnRoleStartFight(Role* role, int battle_id);
	void OnRoleStartFigntSpecialDeal(Role * role, int monster_group_id);
	void OnRoleStopFight(Role* role);
	void OnRoleStartOB(Role* role);
	void OnRoleStopOB(Role* role);
	void OnRoleFinishFight(int role_id, int battle_id, bool is_win, bool is_flyaway, bool is_leader, bool is_attacker, int battle_mode, BattleData_Business& business_data,
		BattleData_BusinessRole& role_business_data, BattleData_StatisticRole& role_statistic_data, BattleData_Statistic& statistic_data, int pet_stat_num, 
		BattleData_StatisticRole* pet_stat_list, long long * pet_unique_id_list, BattleRoleList battle_role_list);

	int MakeBattleLocalInfoSingle(Role* role, int battle_id, int battle_mode, RoleBattleData* role_battle_data, int monster_group_id, int monster_gen_id);
	int MakeBattleLocalInfoMulti(int role_num, Role** role_list, int battle_id, int battle_mode, RoleBattleData** role_battle_data_list, int monster_group_id, int monster_gen_id);
	void MarkBattleLaunchInfo(BattleData_Business& business_data, Role* role, const char* launch_func_name);

	RoleBattleInfo* GetRoleBattleInfo(int role_id, bool is_create = false);
	BattleLocalInfo* GetBattleLocalInfo(int battle_id);
	void RemoveBattleLocalInfo(int battle_id);
	void RemoveRoleBattleInfo(int role_id);

	int GenBattleID();
	void CheckBattleStartFailed();
	void BattleStartFailed(int battle_id);

	bool CanStartWorldBoss(Role * role);
	bool CanStartWorldBoss2(Role * role);
	bool CanStartWorldBoss3(Role * role);

	bool OnBattleFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	//一些需要依赖于其它玩家之类的模块,可以先放入这里处理,防止之后循环后玩家一些信息改变后正常逻辑出BUG(比如侠义值,依赖于击杀首通之类的)
	void BattleFinishSpecialProcess(battlegameprotocol::BattleGameFightResultAck* ack);	

	static bool IsBattleModeNeedPartner(int battle_mode);
	static bool IsBattleModeNeedTeamStandPos(int battle_mode);

	void SetAutoCatchBusinessData(Role* role, BattleData_Business& business_data);
	void StartInitialBattle(Role* role);
	void OnRoleSealPetGiveReward(Role* role, bool is_get_pet, const ItemConfigData& catch_reward_item, int pet_id, int battle_mode, bool is_give_guide_pet);

	void GetRoleAnimationSpeed(Role* role, int battle_mode, bool is_mine_monster, bool is_in_team, ARG_OUT bool* out_is_force_speed, ARG_OUT int* out_animation_speed);
	bool IsMineMonster(int monster_group_id, int battle_mode, bool business_is_real_mine);

	bool CanRoleStartBattle(Role* role, int battle_mode);

	bool IsRoleBattleRequestSendToBigCross(Role* role);
	void TransferBattleRequestToBigcrossBattle(int uid, const char* data, int length);
	void CheckRoleInBigCrossBattle(Role* role);

	void CheckTeamNoticeDoubleSpeed(battlegameprotocol::BattleGameStartFightAck* ack);

	void CheckElementElf(int monster_group_id, int battle_mode, BattleData_Business& business_data, int today_showup_times);
	void CheckTeamElementElf(Team* team, int monster_group_id, int battle_mode, BattleData_Business& business_data);
	void CheckRoleElementElf(Role* role, int monster_group_id, int battle_mode, BattleData_Business& business_data);
	//int GetAverageCapability(Role* role, const RoleBattleData& rbd, bool is_cal_partner);

	bool IsRobotRunAwayBeforeStart(Robot* robot, Role* leader, int battle_mode, int monster_group_id, const BattleData_Business& business);

	RoleBattleInfoMap m_role_battle_info_map;

	BattleLocalInfoMap m_local_info_map;

	int m_battle_id_generator;

	unsigned int m_next_check_battle_start_failed_time;
	
	INSTANCE_FORBID_COPY_AND_EXTERIAL_CREATE(BattleManagerLocal);

	struct QieCuoReqCon
	{
		QieCuoReqCon(int _id = 0, unsigned int _timestamp = 0) : role_id(_id), time_out_timestamp(_timestamp) {}

		int role_id;
		unsigned int time_out_timestamp;
	};

	std::map<int, std::deque<QieCuoReqCon> > m_qie_cuo_fetch_map;
};

#endif