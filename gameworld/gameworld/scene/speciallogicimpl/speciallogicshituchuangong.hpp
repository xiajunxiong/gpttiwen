#ifndef __SPECIAL_LOGIC_SHI_TU_CHUAN_GONG_HPP__
#define __SPECIAL_LOGIC_SHI_TU_CHUAN_GONG_HPP__

//师徒传功 FB

#include "scene/speciallogic.hpp"
#include "servercommon/shitudef.h"

class SpecialLogicShiTuChuanGong : public SpecialLogic
{
public:
	SpecialLogicShiTuChuanGong(Scene* scene);
	virtual ~SpecialLogicShiTuChuanGong() {};

	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnRoleEnterScene(Role *role);
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);

	virtual bool CanDestroy() { return true; }

	//不可战斗
	virtual bool CanRoleStartFight(Role * role, CanRoleStartFightParam &fight_param) { return false; };

	//设置场景信息
	void SetRoleInfo(int uid, int shi_tu_type);
	void SetEnd();

	//玩家操作
	void OnReq(Role *role, int req_type, int p_1, int p_2);

	//场景动画信息
	void SendSceneShowInfo(Role *role);
	void SendGameInfo(Role *role, int type, unsigned int param);
private:
	//就绪
	void RoleGameReady(Role* role);
	//玩家游戏积分设置
	void RoleGameSorceSet(Role *role, int base_score, int game_score);
	//游戏结算
	void GameEnd();
	//中断游戏
	void BreakOff(Role *role);

	Role* GetRole(int uid);

	//策划需求  中途有玩家 掉线/退场 则踢出双方
	int m_is_end;	//副本结束标记
	time_t m_game_begin;
	time_t m_game_end;

	time_t m_next_send_game_info_time;

	//师徒信息
	struct RoleInfo
	{
		RoleInfo()
		{
			role_uid = 0;

			other_uid = 0;
			type = 0;

			game_score = 0;

			base_score = 0;
			other_score = 0;
			game_ready = 0;
		}
		int role_uid;	//角色uid

		int other_uid;	//对方uid
		int type;	//师徒类型

		int game_score;	//游戏总分

		int base_score;	//基础积分
		int other_score;//对方基础积分

		int game_ready;	//游戏状态
	};

	//角色信息map<uid, 信息>
	RoleInfo m_role_info_[SHI_TU_SEEKING_TYPE_MAX];

	//角色场景信息
	RoleInfo * GetRoleInfo(int uid);
	RoleInfo * GetRoleInfoByType(int shi_tu_type);

	//准备就绪计数
	int m_game_ready_count;
	//积分通知
	int m_need_send_score;
};

#endif