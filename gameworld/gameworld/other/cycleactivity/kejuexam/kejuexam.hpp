#ifndef __KE_JU_EXAM__
#define __KE_JU_EXAM__

#include "servercommon/cycleactivity/kejuexamparam.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"
#include "protocol/cycleactivity/msgkejuexam.hpp"
#include "gameworld/other/rolemodulemanager.hpp"
#include "kejuexamconfig.hpp"
#include "other/cycleactivity/kejuexam/kejuexammanager.hpp"

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


class KeJuExam
{
public:
	KeJuExam();
	~KeJuExam();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *module_mgr, const KeJuExamParam & param);
	void GetInitParam(KeJuExamParam * param);

	void OnLogin();
	void Update(unsigned long interval, time_t now_second);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	void OnClientOpReq(Protocol::CSKeJuExamCSReq* req);
	bool IsCanOperator();

	// 获取玩家当前已答题数量
	int GetAnswerCount()const { return m_param.used_question_count; }
	// 获取当前题目id
	int GetTeamQuestion() { return m_param.cur_question_id; }
	// 获取玩家已经答过的题列表, 并添加到set
	void GetUsedQuestion(std::set<int>& used_questions);
	// 获取/设置npc_id, 用于生成npc时避免连续重复生成同一个
	int GetCurTeamNpc() { return m_cur_npc; }
	void SetCurTeamNpc(int npc_id) { m_cur_npc = npc_id; }
	// 获取/设置答题顺序组
	std::pair<int, int> GetGroupOrder() { return std::make_pair(m_param.group_order[0], m_param.group_order[1]); }
	void SetGroupOrder(std::pair<int, int> group_order);

	// 更新当前npc和题目
	void SetNpcQuestion(int npc_seq, int question_id);
	// 开始答题并设置超时时间
	void SetTimeOut(int answer_uid, int timeout_timestamp);
	// 答题奖励
	void AnswerReward(const KeJuExamQuestionResultCfg& reward_cfg, int answer, int result);
	// 答题超时
	void AnswerTimeOut();

	// 点赞
	void Praise(int user_id);
	// 设置气泡
	void SetWord(int word_id) { m_param.word_id = word_id; }
	// 被点赞
	void AddPraiseNum() { m_param.team_info.praise_times += 1; };		

	// 求助/建议
	void SeekHelp();
	void Suggest(int suggest_answer);

	// 添加好友
	void ApplyFriend(int uid);

	// 战斗
	void OnStartLastBattle();
	void OnBattleFinish(bool is_win);

	//每日领取任务随机获得卡片
	void DayGetCard();

	void UsedCard();
	bool SkipCard();			//免答卡
	bool RefreshCard();			//换题卡
	bool ReduceErrorCard();		//除错卡

	// 设置/获取/清除队伍使用卡片信息
	void SetTeamUsedCard() { m_param.team_used_card = 1; }
	bool IsUsedSpecialCard() { return !!m_param.team_used_card; }
	void ClearUsedCard() { m_param.team_used_card = 0; }

	// 设置/获取队伍除错卡排除的答案
	void SetTeamReduceErrorCard(int error_answer) { m_param.error_answer = error_answer; }
	int GetTeamReduceErrorCard() { return m_param.error_answer; }

	// 同步换题卡刷新的题目
	void SetQuestion(int question_id) { m_param.cur_question_id = question_id; }	

	// 同步排错卡排除的答案
	void SetErrorAnswer(int error_answer) { m_param.error_answer = error_answer; }	

	// 队伍是否正在答题, true-正在答题, 不可加入组队
	bool IsAnswering() { return !!m_param.team_answering; }	
	// 是否可同步题目信息
	bool CanSynTeam();
	// 获取答题人uid
	int GetAnswerUid() { return m_param.answer_uid; }

	// 清掉上一题的信息
	void ClearPastQuestionInfo();
	// 进队/离队清除在上一个队伍里的答题信息
	void ClearTeamInfo();
	// 组装当前队伍队内点赞信息
	void AssemblePraiseInfo(KeJuExamPraise & info);	
	// 组装最后结算信息
	void AssembleLastResultInfo(KeJuExamLastResult &last_result);	

private:
	void SendTimeOutInfo();
	void SendNpcInfo();
	void SendInfo();

	RoleModuleManager * m_module_mgr;

	int m_cur_npc;
	KeJuExamParam m_param;
};


#endif