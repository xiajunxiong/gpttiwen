#ifndef __ROLE_ACTIVITY_MAZE_SUMMER_HPP__
#define __ROLE_ACTIVITY_MAZE_SUMMER_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ramazesummerparam.hpp"

class RoleActivityMazeSummer : public RoleActivity
{
public:
	RoleActivityMazeSummer(int type);
	virtual ~RoleActivityMazeSummer();
	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void OnBattleFinish(bool is_win, int battle_mode);
private:
	RAMazeSummerMap * GetMyPosMapInfo();
	Posi GridToPosi(int grid);
	int PosiToGrid(Posi pos);
	bool IsHouseMap(int grid);
	void StartPlay();
	void MovePos(Posi pos);
	void StartSpecialPlay();
	void QuestionAnswer(int answer_id);
	void EndPlay();
	void BuyItem(int buy_idx);

	void CheckMoveUnlockMap(Posi pos);
	void CheckFinishUnlockMap();
	bool CheckMyPosCanMove();

	void OnFinishMap(bool is_pass);//Í¨¹Ø

	void GetTaskReward(int seq);

	void AddTaskParam(int task_type, int num = 1);
	void SendTaskInfo();
	void SendMapInfo();
	void SendShopInfo();
	void SendQuestionInfo(RAMazeSummerMap * question_info);
	void SendSingleMapChange(Posi change_pos);
	void SendMyPos();
	void SendPlayStatus();

	RAMazeSummerParam m_param;
};


#endif
