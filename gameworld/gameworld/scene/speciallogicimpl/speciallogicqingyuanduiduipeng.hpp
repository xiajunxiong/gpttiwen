#ifndef __SPECIAL_LOGIC_QINGYUAN_DUIDUIPENG_HPP__
#define __SPECIAL_LOGIC_QINGYUAN_DUIDUIPENG_HPP__

#include "scene/speciallogic.hpp"
#include <map>


class SpecialLogicQingYuanDuiDuiPeng : public SpecialLogic
{
public:
	SpecialLogicQingYuanDuiDuiPeng(Scene * scene);
	virtual ~SpecialLogicQingYuanDuiDuiPeng();

	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnRoleEnterScene(Role * role);	//!< 主角进入场景
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);

	virtual bool CanDestroy() { return true; }

	//virtual void OnNPCChosedReward(Role *role, ObjID npc_objid, int option_index);

	void OnClientReq(Role * role, void * msg);

private:
	void OnStartGame(Role * role);
	void CompareForm();							// 比较双方幻化类型
	void OnFinish(bool is_finish);				// 副本结束发奖励
	void OnNpcForm(Role * role, ObjID npc_objid);

	void SendGameStart();
	void SendRoleForm();
	void SendFbInfo(Role * role = NULL);

	struct RoleData
	{
		void ResetForm()
		{
			seq = 0;
			npc_obj_id = INVALID_OBJ_ID;
			npc_id = 0;			
		}

		RoleData() : user_id(INVALID_USER_ID)/*, start_game(false)*/
		{
			this->ResetForm();
		}

		UserID user_id;
		//bool start_game;			// 是否开始游戏

		char seq;
		ObjID npc_obj_id;			// 幻化的npc
		int npc_id;	
	};

	bool m_is_init;

	unsigned int begin_game_times;					// 开始时间戳
	unsigned int end_game_times;					// 结束时间戳
	unsigned int role_leave_times;					// 副本结束角色离开时间戳

	struct NpcData
	{
		NpcData() {}
		NpcData(int _seq, int _npc_id): seq(_seq), npc_id(_npc_id) {}

		int seq;
		int npc_id;
	};

	std::map<ObjID, NpcData> m_npc_list;				// <npc, npc_data>

	RoleData role_list[2];

	int integral;				// 积分
	int continuity;				// 连续次数
	int succ_match;				// 成功匹配次数

	int reward_level;							// 奖励档次
};

#endif