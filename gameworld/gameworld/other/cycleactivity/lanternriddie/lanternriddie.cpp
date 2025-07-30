#include "lanternriddie.hpp"
#include "other/funopen/funopen.hpp"

void LanternRiddie::OnClientOpReq(Protocol::LanternRiddieCSReq* req)
{
	if (NULL == req) return;
	if (!ZhouQiManager::Instance().IsInZhouQi(ZHOU_QI_PLAY_TYPE_LANTERN_RIDDIE) || !ZhouQiManager::Instance().IsDayGameOpen(ZHOU_QI_PLAY_TYPE_LANTERN_RIDDIE)) return;

	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_ZHOU_QI))return;

	if (!this->IsCanOperator()) return;

	switch (req->op_type)
	{
	case 0:		//下发信息
		{
			this->SendInfo();
		}
		break;
	case 1:		//请求npc位置
		{
			LanernRiddieManager::Instance().CreateNPCQuestion(m_module_mgr->GetRole());
		}
		break;
	case 2:		//开始答题
		{
			LanernRiddieManager::Instance().StartAnswering(m_module_mgr->GetRole());
		}
		break;
	case 3:		//玩家选择的答案
		{
			LanernRiddieManager::Instance().Answer(m_module_mgr, m_param.cur_question_id, req->param_1);
		}
		break;
	case 4:		//请求战斗
		{
			this->OnStartLastBattle();
		}
		break;
	case 5:		// 使用特殊道具
		{
			this->UsedCard();
		}
		break;
	case 6:		// 玩家点赞, param1-被点赞人uid
		{
			this->Praise(req->param_1);
		}
		break;
	case 7:		// 好友申请,param-被加好友人uid
		{
			this->ApplyFriend(req->param_1);
		}
		break;
	}

}

bool LanternRiddie::IsCanOperator()
{
	if (!m_module_mgr->GetRole()->InTeam())
	{
		return false;
	}

	Team* team = m_module_mgr->GetRole()->GetMyTeam();
	if (NULL == team) return false;

	if (team->GetMemberCount() < LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetOtherCfg().member)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_TEAM_MEMBER_NOT_ENGOUH);
		return false;
	}
	return true;
}

void LanternRiddie::GetUsedQuestion(std::set<int>& used_questions)
{
	if (m_param.used_question_count == 0) return;
	for (int i = 0; i < MAX_SAVE_QUESTION_NUM; ++i)
	{
		used_questions.insert(m_param.used_questions[i]);
	}
}


void LanternRiddie::AnswerReward(const CALRQuestionResultCfg& reward_cfg, int answer_user, int answer, int result)
{
	if (result == CALRQuestionResultCfg::CORRECT)
	{
		m_param.cur_satisfaction = m_param.cur_satisfaction >= LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetOtherCfg().satisfaction ?
			m_param.cur_satisfaction : m_param.cur_satisfaction + reward_cfg.satisfaction_up;

		// 当前队伍的临时满意度
		m_param.team_info.temp_satisfaction = m_param.team_info.temp_satisfaction >= LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetOtherCfg().satisfaction ?
			m_param.team_info.temp_satisfaction : m_param.team_info.temp_satisfaction + reward_cfg.satisfaction_up;
	}

	//没有奖励次数不给奖励
	if(m_param.used_question_count < LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetOtherCfg().question_num)
	{
		if (m_module_mgr->GetKnapsack()->CheckForPutList2(reward_cfg.reward_list))
		{
			m_module_mgr->GetKnapsack()->PutList((short)reward_cfg.reward_list.size(), &reward_cfg.reward_list[0], PUT_REASON_ANSWER_LANTERN_RIDDIE, false);
		}

		std::string log_str;
		for (int i = 0; i < (int)reward_cfg.reward_list.size(); ++i)
		{
			log_str += STRING_SPRINTF(" <%d, %d>", reward_cfg.reward_list[i].item_id, reward_cfg.reward_list[i].num);
		}

		if (m_param.used_question_count >= 0 && m_param.used_question_count < MAX_SAVE_QUESTION_NUM)
		{
			m_param.used_questions[m_param.used_question_count] = m_param.cur_question_id;
		}
		m_param.used_question_count += 1;

		gamelog::g_log_cycleactivity.printf(LL_INFO, ":%s role[%d--%s] used_question_count[%d] rightness[%d] reward[%s]", __FUNCTION__,
			m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), m_param.used_question_count, result, log_str.c_str());
	}
	m_param.cur_question_answered = 1;
	m_param.answer_timeout_timestamp = 0;
	m_param.team_answering = 0;

	if(answer_user == m_module_mgr->GetUid())
	{		// 当前队伍里的答题信息
		m_param.team_info.answered_question_num += 1;
		m_param.team_info.true_question_num = result ? m_param.team_info.true_question_num + 1 : m_param.team_info.true_question_num;
	}
	this->ClearUsedCard();

	Protocol::SALanternRiddieAnswerResultInfo msg;
	msg.answerer_id = answer;
	msg.cur_satisfaction = m_param.cur_satisfaction;
	msg.result = result;
	msg.answerer_answer = answer_user;

	m_module_mgr->NetSend(&msg, sizeof(msg));

	{	//时间戳置0发给客户端
		Protocol::SALanternRiddieTimeOutInfo timestamp_msg;
		timestamp_msg.answer_timeout_timestamp = m_param.answer_timeout_timestamp;
		m_module_mgr->NetSend(&timestamp_msg, sizeof(timestamp_msg));
	}

	this->SendInfo();
}

void LanternRiddie::AnswerTimeOut()
{
	int result = 0;
	//答题超时,默认答错
	const CALRQuestionResultCfg * result_cfg = LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetAnswerResultCfg(m_param.cur_question_id, -1, result);	
	if (result_cfg == NULL) return;

	//没有奖励次数不给奖励
	if (m_param.used_question_count < LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetOtherCfg().question_num)
	{
		if (m_module_mgr->GetKnapsack()->CheckForPutList2(result_cfg->reward_list))
		{
			m_module_mgr->GetKnapsack()->PutList((short)result_cfg->reward_list.size(), &result_cfg->reward_list[0], PUT_REASON_ANSWER_LANTERN_RIDDIE, false);
		}

		std::string log_str;
		for (int i = 0; i < (int)result_cfg->reward_list.size(); ++i)
		{
			log_str += STRING_SPRINTF(" <%d, %d>", result_cfg->reward_list[i].item_id, result_cfg->reward_list[i].num);
		}
	
		if (m_param.used_question_count >= 0 && m_param.used_question_count < MAX_SAVE_QUESTION_NUM)
		{
			m_param.used_questions[m_param.used_question_count] = m_param.cur_question_id;
		}
		m_param.used_question_count += 1;

		gamelog::g_log_cycleactivity.printf(LL_INFO, ":%s role[%d--%s] used_question_count[%d] rightness[%d] reward[%s]", __FUNCTION__,
			m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), m_param.used_question_count, result, log_str.c_str());
	}
	m_param.cur_question_answered = 1;
	m_param.answer_timeout_timestamp = 0;
	m_param.team_answering = 0;
	this->ClearUsedCard();

	{	//时间戳置0发给客户端
		Protocol::SALanternRiddieTimeOutInfo timestamp_msg;
		timestamp_msg.answer_timeout_timestamp = m_param.answer_timeout_timestamp;
		m_module_mgr->NetSend(&timestamp_msg, sizeof(timestamp_msg));
	}

	Protocol::SALanternRiddieAnswerResultInfo msg;
	msg.answerer_id = 0;
	msg.cur_satisfaction = m_param.cur_satisfaction;
	msg.result = result;
	msg.answerer_answer = 0;

	m_module_mgr->NetSend(&msg, sizeof(msg));
	this->SendInfo();
}

void LanternRiddie::OnStartLastBattle()
{
	Team * team = m_module_mgr->GetRole()->GetMyTeam();
	if (team == NULL)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		return;
	}

	if (!m_module_mgr->GetRole()->IsLeader())
	{
		return;
	}

	if (team->HasDismissMember())
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
		return;
	}

	if (team->GetMemberCount() < LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetOtherCfg().member)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_TEAM_MEMBER_NOT_ENGOUH);
		return;
	}

	if (m_param.is_receive_last_reward != 0) return;

	if (m_param.used_question_count < LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetOtherCfg().question_num)
	{
		return;
	}

	if (!m_module_mgr->GetRole()->GetScene()->GetSpecialLogic()->CanJoinOtherActivity())
	{
		m_module_mgr->NoticeNum(errornum::EN_PLAY_ACTIVITY_NOW_ERR);
		return;
	}

	int monster_id = LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetOtherCfg().monster_group;

	if (BattleManagerLocal::GetInstance().StartBattleModeFightReq(m_module_mgr->GetRole(), BATTLE_MODE_LANTERN_RIDDIR, monster_id, 0, 0, true))
	{
		return;
	}

}

void LanternRiddie::OnBattleFinish(bool is_win)
{
	if (m_param.is_receive_last_reward == 1 || m_param.used_question_count < LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetOtherCfg().question_num)
	{
		return;
	}

	if (!is_win)
	{
		int fail_satisfaction = LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetOtherCfg().fail_satisfaction;
		
		m_param.cur_satisfaction = (m_param.cur_satisfaction - fail_satisfaction < 0) ? 0 : m_param.cur_satisfaction - fail_satisfaction;
		m_param.team_info.temp_satisfaction = (m_param.team_info.temp_satisfaction - fail_satisfaction < 0) ? 0 : m_param.team_info.temp_satisfaction - fail_satisfaction;
	}

	const std::vector<ItemConfigData>* reward_list_cfg = LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetLastRewardCfg(m_param.cur_satisfaction);
	if (NULL == reward_list_cfg) return;

	std::string log_str;
	if (m_module_mgr->GetKnapsack()->CheckForPutList2((*reward_list_cfg)))
	{
		m_module_mgr->GetKnapsack()->PutVec(*reward_list_cfg, PUT_REASON_LANTERN_RIDDIE);
		m_module_mgr->GetKnapsack()->NotifyGetItemList((short)reward_list_cfg->size(), &(*reward_list_cfg)[0], GET_REASON_LANTERN_RIDDIE);

		for (unsigned int i = 0; i < (*reward_list_cfg).size(); ++i)
		{
			log_str += STRING_SPRINTF("<%d-%d> ", (*reward_list_cfg)[i].item_id, (*reward_list_cfg)[i].num);
		}
	}
	m_param.is_receive_last_reward = 1;

	gamelog::g_log_cycleactivity.printf(LL_INFO, ":%s role[%d--%s] is_win[%d] reward_list[%s]", __FUNCTION__,
		m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), is_win, log_str.c_str());
	LanernRiddieManager::Instance().SendLastInfo(m_module_mgr->GetRole());		// 最终结算信息
	this->SendInfo();
}

void LanternRiddie::UsedCard()
{
	if (m_param.special_card == 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	if (this->GetSpecialCard())
	{
		m_module_mgr->NoticeNum(errornum::EN_NOT_LANTERN_IRDDIE_USED_SPECIAL_CARD);
		return;
	}

	if (!this->IsAnswering())
	{
		return;
	}

	bool ret = false;
	switch (m_param.special_card)
	{
	case CALRCardCfg::CARD_TYPE_SKIP:
		{
			ret = this->SkipCard();
		}
		break;
	case CALRCardCfg::CARD_TYPE_REFRESH:
		{
			ret = this->RefreshCard();
		}
		break;
	case CALRCardCfg::CARD_TYPE_REDUCE_ERROR:
		{
			ret = this->ReduceErrorCard();
		}
		break;
	}
	if (!ret) return;

	{	// 使用道具漂字
		Protocol::SCLanternRiddieNotifyInfo notify;
		notify.op_type = SCLanternRiddieNotifyInfo::OP_TYPE_USECARD;
		memcpy(notify.name, m_module_mgr->GetRoleName(), sizeof(notify.name));
		notify.param = m_param.special_card;
		LanernRiddieManager::Instance().SendInfoToTeamMember(m_module_mgr->GetRole(), (const char*)&notify, sizeof(notify));
	}

	m_param.special_card = 0;
	LanernRiddieManager::Instance().SynCardUse(m_module_mgr->GetRole());
	LanernRiddieManager::Instance().SendPraiseToTeamMember(m_module_mgr->GetRole());
		
	this->SendInfo();
}

bool LanternRiddie::SkipCard()
{
	int answer = LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetTrueAnswer(m_param.cur_question_id);
	if (answer < 0) return false;

	Protocol::SCLanternRiddieUseCardInfo msg;
	msg.card_type = CALRCardCfg::CARD_TYPE_SKIP;
	msg.uid = m_module_mgr->GetUid();
	msg.param = 0;
	LanernRiddieManager::Instance().SendInfoToTeamMember(m_module_mgr->GetRole(), (const char*)&msg, sizeof(msg));

	LanernRiddieManager::Instance().Answer(m_module_mgr, m_param.cur_question_id, answer);
	
	return true;
}

bool LanternRiddie::RefreshCard()
{
	Team* team = m_module_mgr->GetRole()->GetMyTeam();
	if (NULL == team) return false;

	std::set<int> used_questions;
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)		//统计队伍里用过的题
	{
		TeamMember * member = team->GetMemberInfo(i);
		if (NULL == member) continue;

		Role * role_member = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->uid));
		if (NULL == role_member) continue;

		role_member->GetRoleModuleManager()->GetLanternRiddie()->GetUsedQuestion(used_questions);
	}

	m_param.cur_question_id = LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->RandQuestionCfg(used_questions);
	m_param.answer_timeout_timestamp = (unsigned int)EngineAdapter::Instance().Time() + LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetOtherCfg().answer_time;
	LanernRiddieManager::Instance().SynTimeOut(m_module_mgr->GetRole(), m_param.answer_timeout_timestamp);

	LanernRiddieManager::Instance().SynQuestion(m_module_mgr->GetRole(), m_param.cur_question_id);		// 同步队员题目

	Protocol::SCLanternRiddieUseCardInfo msg;
	msg.card_type = CALRCardCfg::CARD_TYPE_REFRESH;
	msg.uid = m_module_mgr->GetUid();
	msg.param = m_param.cur_question_id;

	LanernRiddieManager::Instance().SendInfoToTeamMember(m_module_mgr->GetRole(), (const char*)&msg, sizeof(msg));

	return true;
}

bool LanternRiddie::ReduceErrorCard()
{
	int answer = LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->GetTrueAnswer(m_param.cur_question_id);
	if (answer < 0) return false;

	//除错卡使用,随机一个不是正确答案的发给客户端
	int array[3] = { 1,2,3 };		//写死
	RandomShuffle(&array[0], 3);
	m_param.error_answer = array[0] == answer ? array[1] : array[0];		//能跑就是好代码=v=

	Protocol::SCLanternRiddieUseCardInfo msg;
	msg.card_type = CALRCardCfg::CARD_TYPE_REDUCE_ERROR;
	msg.uid = m_module_mgr->GetUid();
	msg.param = m_param.error_answer;

	LanernRiddieManager::Instance().SynErrorAnswer(m_module_mgr->GetRole(), m_param.error_answer);
	LanernRiddieManager::Instance().SendInfoToTeamMember(m_module_mgr->GetRole(), (const char*)&msg, sizeof(msg));

	return true;
}

void LanternRiddie::DayGetCard()
{
	if (m_param.special_card != 0 || m_param.start_task_flag == 1) return;

	int sp_card = LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->RandCardCfg();
	if (sp_card <= 0) return;

	m_param.special_card = sp_card;
}

void LanternRiddie::Praise(int user_id)
{
	Role * accept_role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL == accept_role) return;

	{	//被点赞玩家漂字
		Protocol::SCLanternRiddieNotifyInfo notify;
		notify.op_type = SCLanternRiddieNotifyInfo::OP_TYPE_PRAISE;
		memcpy(notify.name, m_module_mgr->GetRoleName(), sizeof(notify.name));
		notify.param = 0;

		accept_role->GetRoleModuleManager()->NetSend(&notify, sizeof(notify));
	}

	m_param.is_praise = 1;			// 记录自己的点赞信息
	if (m_param.is_praise != 1)		// 如果本题第一次点赞,更新信息
	{
		LanernRiddieManager::Instance().SendPraiseToTeamMember(m_module_mgr->GetRole());
	}

	m_param.word_id = (short)LOGIC_CONFIG->GetCycleActLanternRiddleConfig()->RandWordCfg();
	accept_role->GetRoleModuleManager()->GetLanternRiddie()->AddPraiseNum();

	LanernRiddieManager::Instance().SendPraiseToTeamMember(m_module_mgr->GetRole());
}

 void LanternRiddie::AssemblePraiseInfo(LanternRiddiePraise & info)
{
	info.Reset();
	info.uid = m_module_mgr->GetUid();
	info.is_praise = m_param.is_praise;
	info.cur_card = m_param.special_card;
	info.word_id = m_param.word_id;
}

void LanternRiddie::ClearPastQuestionInfo()
{
	m_param.error_answer = 0;
	m_param.is_praise = 0;
	m_param.word_id = 0;
	m_param.team_used_card = 0;		// 顺便再清一下使用特殊卡
}

void LanternRiddie::ApplyFriend(int uid)
{
	Role * accept_role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
	if (NULL == accept_role) return;

	Protocol::SCLanternRiddieNotifyInfo apply_info;
	apply_info.op_type = SCLanternRiddieNotifyInfo::OP_TYPE_FRIEND;
	memcpy(apply_info.name, m_module_mgr->GetRoleName(), sizeof(apply_info.name));
	apply_info.param = 0;

	EngineAdapter::Instance().NetSend(accept_role->GetNetId(), &apply_info, sizeof(apply_info));
}

void LanternRiddie::ClearTeamInfo()
{
	m_param.team_info.Reset();
}

void LanternRiddie::AssembleLastResultInfo(LanternRiddieLastResult & last_result)
{
	last_result.uid = m_module_mgr->GetUid();
	last_result.cur_satisfaction = m_param.team_info.temp_satisfaction;
	last_result.praise_times = m_param.team_info.praise_times;
	last_result.true_question_num = m_param.team_info.true_question_num;
	last_result.answered_question_num = m_param.team_info.answered_question_num;
}


bool LanternRiddie::CanSynTeam()
{
	bool is_can_syn = false;

	if (1 == m_param.start_task_flag && 0 == m_param.cur_question_answered) is_can_syn = true;

	return is_can_syn;
}

void LanternRiddie::SendNpcInfo(int npc_seq, int question_id)
{
	m_param.cur_question_id = question_id;
	cur_npc = npc_seq;
	m_param.cur_question_answered = 0;

	if (m_param.start_task_flag == 0)
	{
		this->DayGetCard();
		m_param.start_task_flag = 1;		
	}

	Protocol::SALanternRiddieNPCInfo msg;
	msg.npc_seq = npc_seq;
	msg.question_id = question_id;

	m_module_mgr->NetSend(&msg, sizeof(msg));
	this->SendInfo();
}

void LanternRiddie::SendTimeOutInfo(int answer_timestamp)
{
	m_param.answer_timeout_timestamp = answer_timestamp;
	m_param.team_answering = 1;

	Protocol::SALanternRiddieTimeOutInfo msg;
	msg.answer_timeout_timestamp = answer_timestamp;

	m_module_mgr->NetSend(&msg, sizeof(msg));
}

void LanternRiddie::SendInfo()
{
	Protocol::SALanternRiddieBaseInfo msg;
	msg.start_task_flag = m_param.start_task_flag;
	msg.is_receive_last_reward = (char)m_param.is_receive_last_reward;
	msg.used_question_count = m_param.used_question_count;
	msg.cur_satisfaction = m_param.cur_satisfaction;
	msg.cur_card = m_param.special_card;

	m_module_mgr->NetSend(&msg, sizeof(msg));
}

LanternRiddie::LanternRiddie()
{
}

LanternRiddie::~LanternRiddie()
{
}

void LanternRiddie::Init(RoleModuleManager * module_mgr, const SALanternRiddieParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;
}

void LanternRiddie::GetInitParam(SALanternRiddieParam * param)
{
	*param = m_param;
}

void LanternRiddie::OnLogin()
{
	if (!ZhouQiManager::Instance().IsInZhouQi(ZHOU_QI_PLAY_TYPE_LANTERN_RIDDIE) || !ZhouQiManager::Instance().IsDayGameOpen(ZHOU_QI_PLAY_TYPE_LANTERN_RIDDIE)) return;

	this->SendInfo();
}

void LanternRiddie::Update(unsigned long interval, time_t now_second)
{
	if (!ZhouQiManager::Instance().IsInZhouQi(ZHOU_QI_PLAY_TYPE_LANTERN_RIDDIE) || !ZhouQiManager::Instance().IsDayGameOpen(ZHOU_QI_PLAY_TYPE_LANTERN_RIDDIE)) return;

	const ZhouQiStartPointCfg * ZQstart_cfg = LOGIC_CONFIG->GetZhouQiConfig()->GetZhouQiStartPointCfg(now_second);
	if (NULL == ZQstart_cfg) return;
	if (m_param.activity_start_timestamp != ZQstart_cfg->start_time)
	{
		m_param.Reset();
		m_param.activity_start_timestamp = ZQstart_cfg->start_time;
	}

	if (m_param.answer_timeout_timestamp != 0 && now_second >= m_param.answer_timeout_timestamp)
	{
		this->AnswerTimeOut();
	}
}

void LanternRiddie::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;
	if (!ZhouQiManager::Instance().IsInZhouQi(ZHOU_QI_PLAY_TYPE_LANTERN_RIDDIE)) return;

	m_param.DayReset();
	this->SendInfo();
}
