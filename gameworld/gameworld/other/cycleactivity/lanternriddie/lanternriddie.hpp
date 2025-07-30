#ifndef __ROLE_ACTIVITY_LANTERN_RIDDLE__
#define __ROLE_ACTIVITY_LANTERN_RIDDLE__

#include "servercommon/cycleactivity/lanternriddieparam.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"
#include "protocol/cycleactivity/msglanternriddie.hpp"
#include "gameworld/other/rolemodulemanager.hpp"
#include "lanternriddieconfig.hpp"
#include "other/cycleactivity/lanternriddie/lanternriddiemanager.hpp"

#include "config/logicconfigmanager.hpp"
#include "gameworld/obj/character/role.h"
#include "servercommon/errornum.h"
#include "global/team/team.hpp"
#include "world.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "gameworld/internalcomm.h"
#include "gameworld/gamelog.h"
#include "battle/battle_business_local.hpp"
#include "gameworld/battle/battle_manager_local.hpp"
#include "global/zhouqimanager/zhouqimanager.h"
#include "servercommon/zhouqidef.hpp"
#include "global/zhouqimanager/zhouqiconfig.hpp"


#include <vector>
#include <set>


class LanternRiddie
{
public:
	LanternRiddie();
	~LanternRiddie();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *module_mgr, const SALanternRiddieParam & param);
	void GetInitParam(SALanternRiddieParam * param);

	void OnLogin();
	void Update(unsigned long interval, time_t now_second);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	void OnClientOpReq(Protocol::LanternRiddieCSReq* req);	
	bool IsCanOperator();

	void GetUsedQuestion(std::set<int>& used_questions);
	int GetAnswerCount()const { return m_param.used_question_count; }
	void AnswerReward(const CALRQuestionResultCfg& reward_cfg,int answer_user, int answer, int result);
	void AnswerTimeOut();
	void OnStartLastBattle();
	void OnBattleFinish(bool is_win);

	void UsedCard();
	bool SkipCard();			//免答卡
	bool RefreshCard();			//换题卡
	bool ReduceErrorCard();		//除错卡

	void DayGetCard();		//每日领取任务随机获得卡片

	void Praise(int user_id);			// 点赞
	void AddPraiseNum() { m_param.team_info.praise_times += 1; };		// 被点赞
	void  AssemblePraiseInfo(LanternRiddiePraise & info);
	void ClearPastQuestionInfo();

	void ApplyFriend(int uid);

	void ClearTeamInfo();			// 进队/离队清除在上一个队伍里的答题信息
	void AssembleLastResultInfo(LanternRiddieLastResult &last_result);	// 组装最后结算信息

	////////////////////
	void SetSpecialCard() { m_param.team_used_card = 1; }		// 设置全队卡片使用
	void ClearUsedCard(){ m_param.team_used_card = 0; }			// 清除卡片的使用标记
	bool GetSpecialCard() { return !!m_param.team_used_card; }	// true-不可使用
	///////////////////
	void SetQuestion(int question_id) { m_param.cur_question_id = question_id; }	// 同步换题卡刷新的题目
	void SetErrorAnswer(int error_answer) { m_param.error_answer = error_answer; }	// 同步排错卡排除的答案
	int GetErrorAnswer() { return m_param.error_answer; }

	bool IsAnswering() { return !!m_param.team_answering; }		// 队伍是否正在答题, true-正在答题不可加入组队
	bool CanSynTeam();
	int GetTeamNpc() { return cur_npc; }
	int GetTeamQuestion() { return m_param.cur_question_id; }

	void SendNpcInfo(int npc_seq, int question_id);
	void SendTimeOutInfo(int answer_timestamp);
private:
	void SendInfo();
	

	RoleModuleManager * m_module_mgr;

	int cur_npc;

	SALanternRiddieParam m_param;
};




#endif
