#ifndef __SPECIAL_LOGIC_RELIC_LOOTING_HPP__
#define __SPECIAL_LOGIC_RELIC_LOOTING_HPP__

#include "scene/speciallogic.hpp"
#include "protocol/msgscene.h"
#include "servercommon/taskdef.h"
#include "global/activity/activityimpl/activityreliclooting.hpp"
#include "global/team/team.hpp"
#include <vector>
#include <map>
#include "config/activityconfig/reliclooting/activityreliclootingconfig.hpp"
#include <math.h>
#include "servercommon/reliclootingdef.h"

struct RoleSpecialEffect
{
	RoleSpecialEffect() {}
	enum ROLE_SPECIAL_EFFECT_TYPE
	{
		ROLE_SPECIAL_EFFECT_TYPE_NORMAL = 0,	//正常
		ROLE_SPECIAL_EFFECT_TYPE_1 = 1,			//特效 抓捕
		ROLE_SPECIAL_EFFECT_TYPE_2 = 2,			//特效 被抓捕的人
		ROLE_SPECIAL_EFFECT_TYPE_3 = 3,			//特效 疾走
		ROLE_SPECIAL_EFFECT_TYPE_4 = 4,			//特效 传送 天行梭
		ROLE_SPECIAL_EFFECT_TYPE_5 = 5,			//特效 传送 天行梭 的传送落地
		ROLE_SPECIAL_EFFECT_TYPE_6 = 6,			//特效 开锁
		ROLE_SPECIAL_EFFECT_TYPE_7 = 7,			//特效 救援
		ROLE_SPECIAL_EFFECT_TYPE_8 = 8,			//特效 救援 被救援的人
		ROLE_SPECIAL_EFFECT_TYPE_9 = 9,			//特效 隐身符 神隐
		ROLE_SPECIAL_EFFECT_TYPE_10 = 10,		//特效 稻草人
		ROLE_SPECIAL_EFFECT_TYPE_11 = 11,		//特效 轻身符（加速比疾走大）
		ROLE_SPECIAL_EFFECT_TYPE_12 = 12,		//特效 轻身符 的加速结束的减速效果
		ROLE_SPECIAL_EFFECT_TYPE_13 = 13,		//特效 陷阱 踩陷阱的人
		ROLE_SPECIAL_EFFECT_TYPE_14 = 14,		//特效 傀儡 生成的特效
		ROLE_SPECIAL_EFFECT_TYPE_15 = 15,		//特效 挪移尺 的施法特效
		ROLE_SPECIAL_EFFECT_TYPE_16 = 16,		//特效 开箱结束
		ROLE_SPECIAL_EFFECT_TYPE_MAX,
	};
	int uid;
	int effect_type;
};

class SpecialLogicRelicLootingFb : public SpecialLogic
{
public:
	SpecialLogicRelicLootingFb(Scene * scene);
	virtual ~SpecialLogicRelicLootingFb();

	virtual void OnRoleEnterScene(Role *role);					//!< 主角进入场景
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< 主角离开场景
	virtual void OnRoleLeaveTeam(Role * role);
	virtual bool CanDestroy() { return true; }
	virtual bool CanMoveOn(Role* role);					//!< 是否允许玩家移动
	virtual void Update(unsigned long interval, time_t now_second);

	//virtual bool CanGather(Role* role, NPCGather* gather) { return false; }	//!< 是否能在场景里采集
	virtual bool CanJoinOtherActivity() { return false; }
	virtual bool CanInvitationRole(Role *role, int uid) { return false; }
	virtual bool CanChangeTeamLeader(Role * role, Team * team) { return false; }
	virtual void OnTeamDismiss(Role * leader_role, Team * team) {}	//!< 整个队伍解散流程中只由队长调用一次，无论队伍里有多少名队员
	virtual void OnAOIEnterRole(Role * role, Role * target_role);	//!< 目标角色进入我的视野

	int GetXYDistend(int x1, int y1, int x2, int y2);

	bool OnEnterFbBeforeSet(Role * role, bool is_defender);//设置
	void TeamRecord(Team * team_info);//记录队伍信息

	//玩家操作
	void GetSkillItem(Role * role, int param1);
	void GiveUpSkillItem(Role * role);
	void OnRoleUseSkill(Role * role, int param1, int param2, int param3, int param4);//使用技能
	void GiveUpSkillStatus(Role * role);//中断当前技能（放弃技能效果）
	void OnRoleTs(Role*role, int member_uid);//接受传送
	void SendMake(Role * role, int mark_type, int pos_x, int pos_y);

	//技能效果
	void SkillEffect(Role *role, RelicLootingFBSkillInfo *effect_skill, bool *is_delete = NULL);
	void OpenLock(Role *role, const RelicLootingSkillCfg * skill_info, RelicLootingFBSkillInfo * skill);//开锁
	void HelpTeamMember(Role *role, const RelicLootingSkillCfg * skill_info, RelicLootingFBSkillInfo * skill);//救援
	void CatchPlayer(Role * role, const RelicLootingSkillCfg * skill_info, RelicLootingFBSkillInfo * skill);//抓捕
	void Galloping(Role *role, const RelicLootingSkillCfg * skill_info);//疾走
	void Transmt(Role *role, Posi* transmt_pos = NULL);//传送or天行梭
	void RefreshPos(Role * role, Posi * transmt_pos, int distance);
	void SoftBody(Role *role, const RelicLootingSkillCfg * skill_info);//轻身符
	void DaoCaoRen(Role *role, const RelicLootingSkillCfg * skill_info);//稻草人
	void ChuanSongFaZhen(Role *role, const RelicLootingSkillCfg * skill_info);//传送法阵

	//干涉他人技能效果
	void InquiryTransmt(Role *role, Role * member, unsigned int end_tamp);

	void SendObjInfo(Role * role = NULL);//下发所有obj信息
	void SendTeamMemberPos();//下发阵营队友位置--刷新1秒一次
	void SendTeamInfo(Role *role = NULL);

	//所有玩家都进场了就调用这个准备
	void OnMemberAllEnterFb();

	virtual bool CanEnterTeam(Role * role, Team * team, bool is_from_create) { return true; }
private:
	void OnGameBegin();//正式开始游戏
	void CreateBox();

	void RefreshSkillBox();
	void ClearAllPayerEffect();

	bool HasUser(int uid, bool *is_defender = NULL);
	void OnGameEndMakeRoleInTeam();//玩家离场，重新组队
	void SendSceneTampInfoToAll(int tamp_type, unsigned int tamp, Role * role = NULL);
	void OnPlayEnd(int win_type);

	void OnCheckAllBoxBeSteal();//检查所有宝藏是否都被夺取
	void OnAllBoxBeSteal();//所有宝藏都被夺取

	void OnCheckAllAttackerBeCatch();//检查盗宝者是否都被抓住了
	void OnAllAttackerBeCATCH();//所有盗宝者被抓捕
	void SendPlayerSkillInfo(Role * role);//玩家技能信息

	void HasUserSkillBelong(Role *role);
	//------------------------------------------------------------------------
	void OnRoleStartCatch(int uid);
	void OnRoleStartBeCatch(int uid);
	void OnRoleStartSprint(int uid);
	void OnRoleStartTransfer(int uid);
	void OnRoleStartTransferBack(int uid);
	void OnRoleStartUnlock(int uid);
	void OnRoleStartHelp(int uid);
	void OnRoleStartBeHelp(int uid);
	void OnRoleStartSneack(int uid);
	void OnRoleStartDaoCaoRen(int uid);
	void OnRoleStartSoftBody(int uid);
	void OnRoleStartSoftBodyEnd(int uid);
	void OnRoleStartBeTrap(int uid);
	void OnRoleStartDoll(int uid);
	void OnRoleStartRefresh(int uid);

	void OnRoleStopMoveStatus(int uid);

	void SetSpecialEffect(int uid, int effect_type);
	void SendRoleSpecialEffect(Role * role, RoleSpecialEffect *info);

	void CheckAllTeamHasOutFollow();//检查所有队伍成员是否暂离

	unsigned int check_out_follow_tamp;//检查队伍全员暂离
	bool all_member_ready;//是否所有玩家都准备好了
	unsigned int member_enter_wait_tamp;//进场等待10秒-等客户端返回玩家到位完毕才准备倒计时
	unsigned int fb_ready_tamp;// 准备时间
	unsigned int m_fb_end_tamp;//游戏结束时间戳
	unsigned int m_kick_out_tamp;
	unsigned int m_drop_skill_tamp;//掉落技能时间戳
	unsigned int play_time;	//游戏开始时间戳
	int win_type;//胜利类型

	bool allow_move;//玩家是否允许移动

	PlayerInfo * GetPlayerInfo(int uid ,bool* is_defender = NULL);
	RoleSpecialEffect * GetRoleSpecialEffect(int uid);

	int defender_num;
	PlayerInfo defender_list[RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM];//守卫者

	int attacker_num;
	PlayerInfo attacker_list[RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM];//盗宝者

	//std::vector<RelicLootingTeamRecodeInfo> m_team_recode_vec;//队伍记录

	std::map<ObjID, Protocol::SCRelicLootingObjInfo::Objinfo> m_obj_map;
	//std::map<ObjID ,Protocol::SCRelicLootingObjInfo::Objinfo> m_skill_vec;
	std::map<UserID, Protocol::SCRelicLootingObjInfo::Objinfo> skill_belog_map;

	std::map<int, RoleSpecialEffect> m_special_effect_map;

	unsigned next_refresh_team_member_pos_tamp;
};

#endif