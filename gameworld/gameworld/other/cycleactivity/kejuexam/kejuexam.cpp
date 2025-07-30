#include "kejuexam.hpp"

KeJuExam::KeJuExam():m_cur_npc(0)
{
}

KeJuExam::~KeJuExam()
{
}

void KeJuExam::Init(RoleModuleManager * module_mgr, const KeJuExamParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;
}

void KeJuExam::GetInitParam(KeJuExamParam * param)
{
	*param = m_param;
}

void KeJuExam::OnLogin()
{
	return;

	this->SendInfo();
}

void KeJuExam::Update(unsigned long interval, time_t now_second)
{
	return;

	if (m_param.answer_timeout_timestamp != 0 && now_second >= m_param.answer_timeout_timestamp)
	{
		this->AnswerTimeOut();
	}
}

void KeJuExam::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	return;

	if (old_dayid == now_dayid) return;
	

	m_param.DayReset();
	this->SendInfo();
}

void KeJuExam::OnClientOpReq(Protocol::CSKeJuExamCSReq * req)
{
	return;

	if (NULL == req) return;

	if (!this->IsCanOperator()) return;

	switch (req->op_type)
	{
	case CSKeJuExamCSReq::OP_TYPE_SEND_INFO:
		{
			this->SendInfo();
		}
		break;
	case CSKeJuExamCSReq::OP_TYPE_CREATE_NPC:
		{
			KeJuExamManager::Instance().CreateNPCQuestion(m_module_mgr->GetRole());
		}
		break;
	case CSKeJuExamCSReq::OP_TYPE_START_ANSWERING:
		{
			KeJuExamManager::Instance().StartAnswering(m_module_mgr->GetRole());
		}
		break;
	case CSKeJuExamCSReq::OP_TYPE_ANSWER:
		{
			if (m_module_mgr->GetUid() != m_param.answer_uid)
			{
				m_module_mgr->NoticeNum(errornum::EN_KE_JU_EXAM_CAN_NOT_ANSWER);
				return;
			}

			KeJuExamManager::Instance().Answer(m_module_mgr, m_param.cur_question_id, req->param_1);
		}
		break;
	case CSKeJuExamCSReq::OP_TYPE_USED_CARD:
		{
			this->UsedCard();
		}
		break;
	case CSKeJuExamCSReq::OP_TYPE_PRAISE:
		{
			this->Praise(req->param_1);
		}
		break;
	case CSKeJuExamCSReq::OP_TYPE_LAST_BATTLE:
		{
			this->OnStartLastBattle();
		}
		break;
	case CSKeJuExamCSReq::OP_TYPE_APPLY_FRIEND:
		{
			this->ApplyFriend(req->param_1);
		}
		break;
	case CSKeJuExamCSReq::OP_TYPE_SEEK_HELP:
		{
			this->SeekHelp();
		}
		break;
	case CSKeJuExamCSReq::OP_TYPE_SUGGEST:
		{
			this->Suggest(req->param_1);
		}
		break;
	}
}

bool KeJuExam::IsCanOperator()
{
	if (!m_module_mgr->GetRole()->InTeam())
	{
		return false;
	}

	Team* team = m_module_mgr->GetRole()->GetMyTeam();
	if (NULL == team) return false;

	if (team->GetMemberCount() < LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetOtherCfg().min_member)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_TEAM_MEMBER_NOT_ENGOUH);
		return false;
	}

	return true;
}

void KeJuExam::GetUsedQuestion(std::set<int>& used_questions)
{
	if (m_param.used_question_count == 0) return;

	for (int i = 0; i < m_param.used_question_count && i < MAX_SAVE_QUESTION_NUM; ++i)
	{
		used_questions.insert(m_param.used_questions[i]);
	}
}

void KeJuExam::SetGroupOrder(std::pair<int, int> group_order)
{
	m_param.group_order[0] = group_order.first;
	m_param.group_order[1] = group_order.second;
}

void KeJuExam::SetNpcQuestion(int npc_seq, int question_id)
{
	m_param.cur_question_id = question_id;
	m_cur_npc = npc_seq;
	m_param.cur_question_answered = 0;

	if (m_param.start_task_flag == 0)
	{
		this->DayGetCard();
		m_param.start_task_flag = 1;
	}

	this->SendNpcInfo();
	this->SendInfo();
}



void KeJuExam::SetTimeOut(int answer_uid, int timeout_timestamp)
{
	m_param.answer_timeout_timestamp = timeout_timestamp;
	m_param.team_answering = 1;
	m_param.answer_uid = answer_uid;

	this->SendTimeOutInfo();
}

void KeJuExam::AnswerReward(const KeJuExamQuestionResultCfg & reward_cfg, int answer, int result)
{
	if (result == KeJuExamQuestionResultCfg::CORRECT)
	{
		m_param.cur_satisfaction = m_param.cur_satisfaction >= LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetOtherCfg().max_satisfaction ?
			m_param.cur_satisfaction : m_param.cur_satisfaction + reward_cfg.satisfaction_up;

		// 当前队伍的临时满意度
		m_param.team_info.temp_satisfaction = m_param.team_info.temp_satisfaction >= LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetOtherCfg().max_satisfaction ?
			m_param.team_info.temp_satisfaction : m_param.team_info.temp_satisfaction + reward_cfg.satisfaction_up;
	}

	// 没有奖励次数不给奖励
	if (m_param.used_question_count < LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetOtherCfg().question_num)
	{
		if (m_module_mgr->GetKnapsack()->CheckForPutList2(reward_cfg.reward_list))
		{
			m_module_mgr->GetKnapsack()->PutList((short)reward_cfg.reward_list.size(), &reward_cfg.reward_list[0], PUT_REASON_KE_JU_EXAM_ANSWER_REWARD, false);
		}

		std::string log_str;
		for (int i = 0; i < (int)reward_cfg.reward_list.size(); ++i)
		{
			log_str += STRING_SPRINTF(" <%d, %d>", reward_cfg.reward_list[i].item_id, reward_cfg.reward_list[i].num);
		}

		if (m_param.used_question_count >= 0 && m_param.used_question_count < KE_JU_MAX_SAVE_QUESTION_NUM)
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

	if (m_param.answer_uid == m_module_mgr->GetUid())
	{		// 当前队伍里的答题信息
		m_param.team_info.answered_question_num += 1;
		m_param.team_info.true_question_num = result ? m_param.team_info.true_question_num + 1 : m_param.team_info.true_question_num;

		// 气泡信息
		int word_id = LOGIC_CONFIG->GetCycleKeJuExamConfig()->RandWordCfg(CycleKeJuExamConfig::WORD_TYPE_ANSWER, answer);
		if (!(word_id <= 0))
		{
			m_param.word_id = word_id;
			KeJuExamManager::Instance().SendPraiseToTeamMember(m_module_mgr->GetRole());
		}
	}

	this->ClearUsedCard();

	{
		Protocol::SCKeJuExamAnswerResultInfo msg;
		msg.answer = answer;
		msg.result = result;

		m_module_mgr->NetSend(&msg, sizeof(msg));
	}

	//时间戳置0发给客户端 答题人id不置0
	{	
		Protocol::SCKeJuExamTimeOutInfo timestamp_msg;
		timestamp_msg.answer_timeout_timestamp = m_param.answer_timeout_timestamp;
		timestamp_msg.answerer_role_id = m_param.answer_uid;
		m_module_mgr->NetSend(&timestamp_msg, sizeof(timestamp_msg));
	}

	this->SendInfo();
}

void KeJuExam::AnswerTimeOut()
{
	int result = 0;
	//答题超时,默认答错
	const KeJuExamQuestionResultCfg * result_cfg = LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetAnswerResultCfg(m_param.cur_question_id, -1, result);
	if (result_cfg == NULL) return;

	//没有奖励次数不给奖励
	if (m_param.used_question_count < LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetOtherCfg().question_num)
	{
		if (m_module_mgr->GetKnapsack()->CheckForPutList2(result_cfg->reward_list))
		{
			m_module_mgr->GetKnapsack()->PutList((short)result_cfg->reward_list.size(), &result_cfg->reward_list[0], PUT_REASON_KE_JU_EXAM_ANSWER_REWARD, false);
		}

		std::string log_str;
		for (int i = 0; i < (int)result_cfg->reward_list.size(); ++i)
		{
			log_str += STRING_SPRINTF(" <%d, %d>", result_cfg->reward_list[i].item_id, result_cfg->reward_list[i].num);
		}

		if (m_param.used_question_count >= 0 && m_param.used_question_count < KE_JU_MAX_SAVE_QUESTION_NUM)
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

	//时间戳置0发给客户端
	{	
		Protocol::SCKeJuExamTimeOutInfo timestamp_msg;
		timestamp_msg.answer_timeout_timestamp = m_param.answer_timeout_timestamp;
		timestamp_msg.answerer_role_id = m_param.answer_uid;
		m_module_mgr->NetSend(&timestamp_msg, sizeof(timestamp_msg));
	}

	{
		Protocol::SCKeJuExamAnswerResultInfo msg;
		msg.answer = 0;
		msg.result = result;

		m_module_mgr->NetSend(&msg, sizeof(msg));
	}

	this->SendInfo();
}

void KeJuExam::Praise(int user_id)
{
	if (m_module_mgr->GetUid() == user_id) return;

	Role * accept_role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL == accept_role) return;

	int word_id = (short)LOGIC_CONFIG->GetCycleKeJuExamConfig()->RandWordCfg(CycleKeJuExamConfig::WORD_TYPE_PRAISE);
	if (word_id <= 0) return;

	{	//被点赞玩家漂字
		Protocol::SCKeJuExamNotifyInfo notify;
		notify.op_type = SCKeJuExamNotifyInfo::OP_TYPE_PRAISE;
		memcpy(notify.name, m_module_mgr->GetRoleName(), sizeof(notify.name));
		notify.param = 0;

		accept_role->GetRoleModuleManager()->NetSend(&notify, sizeof(notify));
	}

	this->SetWord(word_id);

	accept_role->GetRoleModuleManager()->GetKeJuExam()->AddPraiseNum();

	KeJuExamManager::Instance().SendPraiseToTeamMember(m_module_mgr->GetRole());
}



void KeJuExam::SeekHelp()
{
	if (m_module_mgr->GetUid() != m_param.answer_uid)
	{
		m_module_mgr->NoticeNum(errornum::EN_KE_JU_EXAM_CAN_NOT_SEEK_HELP);
		return;
	}

	if (!this->IsAnswering())
	{
		return;
	}

	int word_id = LOGIC_CONFIG->GetCycleKeJuExamConfig()->RandWordCfg(CycleKeJuExamConfig::WORD_TYPE_SEEK_HELP);
	if (word_id <= 0) return;

	m_param.word_id = word_id;

	KeJuExamManager::Instance().SendPraiseToTeamMember(m_module_mgr->GetRole());
}

void KeJuExam::Suggest(int suggest_answer)
{
	if (m_module_mgr->GetUid() == m_param.answer_uid)
	{
		m_module_mgr->NoticeNum(errornum::EN_KE_JU_EXAM_CAN_NOT_SUGGEST);
		return;
	}

	if (!this->IsAnswering())
	{
		return;
	}

	int word_id = LOGIC_CONFIG->GetCycleKeJuExamConfig()->RandWordCfg(CycleKeJuExamConfig::WORD_TYPE_SUGGEST, suggest_answer);
	if (word_id <= 0) return;

	m_param.word_id = word_id;

	KeJuExamManager::Instance().SendPraiseToTeamMember(m_module_mgr->GetRole());
}

void KeJuExam::ApplyFriend(int uid)
{
	Role * accept_role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
	if (NULL == accept_role) return;

	Protocol::SCKeJuExamNotifyInfo apply_info;
	apply_info.op_type = SCKeJuExamNotifyInfo::OP_TYPE_FRIEND;
	memcpy(apply_info.name, m_module_mgr->GetRoleName(), sizeof(apply_info.name));
	apply_info.param = 0;

	EngineAdapter::Instance().NetSend(accept_role->GetNetId(), &apply_info, sizeof(apply_info));
}

void KeJuExam::OnStartLastBattle()
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

	if (team->GetMemberCount() < LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetOtherCfg().min_member)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_TEAM_MEMBER_NOT_ENGOUH);
		return;
	}

	if (m_param.is_receive_last_reward != 0) return;

	if (m_param.used_question_count < LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetOtherCfg().question_num)
	{
		return;
	}

	if (!m_module_mgr->GetRole()->GetScene()->GetSpecialLogic()->CanJoinOtherActivity())
	{
		m_module_mgr->NoticeNum(errornum::EN_PLAY_ACTIVITY_NOW_ERR);
		return;
	}

	int monster_id = LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetOtherCfg().monster_group;

	if (BattleManagerLocal::GetInstance().StartBattleModeFightReq(m_module_mgr->GetRole(), BATTLE_MODE_KE_JU_EXAM, monster_id, 0, 0, true))
	{
		return;
	}
}

void KeJuExam::OnBattleFinish(bool is_win)
{
	if (m_param.is_receive_last_reward == 1 || m_param.used_question_count < LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetOtherCfg().question_num)
	{
		return;
	}

	if (!is_win)
	{
		int fail_satisfaction = LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetOtherCfg().fail_satisfaction;

		m_param.cur_satisfaction = (m_param.cur_satisfaction - fail_satisfaction < 0) ? 0 : m_param.cur_satisfaction - fail_satisfaction;
		m_param.team_info.temp_satisfaction = (m_param.team_info.temp_satisfaction - fail_satisfaction < 0) ? 0 : m_param.team_info.temp_satisfaction - fail_satisfaction;
	}

	const std::vector<ItemConfigData>* reward_list_cfg = LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetLastRewardCfg(m_param.cur_satisfaction);
	if (NULL == reward_list_cfg) return;

	std::string log_str;
	if (m_module_mgr->GetKnapsack()->CheckForPutList2((*reward_list_cfg)))
	{
		m_module_mgr->GetKnapsack()->PutVec(*reward_list_cfg, PUT_REASON_KE_JU_EXAM_LAST_REWARD);
		m_module_mgr->GetKnapsack()->NotifyGetItemList((short)reward_list_cfg->size(), &(*reward_list_cfg)[0], GET_REASON_KE_JU_EXAM);

		for (unsigned int i = 0; i < (*reward_list_cfg).size(); ++i)
		{
			log_str += STRING_SPRINTF("<%d-%d> ", (*reward_list_cfg)[i].item_id, (*reward_list_cfg)[i].num);
		}
	}
	m_param.is_receive_last_reward = 1;

	gamelog::g_log_cycleactivity.printf(LL_INFO, ":%s role[%d--%s] is_win[%d] reward_list[%s]", __FUNCTION__,
		m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), is_win, log_str.c_str());
	KeJuExamManager::Instance().SendLastInfo(m_module_mgr->GetRole());		// 最终结算信息
	this->SendInfo();
}

void KeJuExam::DayGetCard()
{
	if (m_param.start_task_flag == 1) return;
	m_param.special_card = LOGIC_CONFIG->GetCycleKeJuExamConfig()->RandCardCfg();
}

void KeJuExam::UsedCard()
{
	if (m_param.special_card == 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	if (this->IsUsedSpecialCard())
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
	case KeJuExamCardCfg::CARD_TYPE_SKIP:
		{
			ret = this->SkipCard();
		}
		break;
	case KeJuExamCardCfg::CARD_TYPE_REFRESH:
		{
			ret = this->RefreshCard();
		}
		break;
	case KeJuExamCardCfg::CARD_TYPE_REDUCE_ERROR:
		{
			ret = this->ReduceErrorCard();
		}
		break;
	}
	if (!ret) return;

	{	// 使用道具漂字
		Protocol::SCKeJuExamNotifyInfo notify;
		notify.op_type = SCKeJuExamNotifyInfo::OP_TYPE_USECARD;
		memcpy(notify.name, m_module_mgr->GetRoleName(), sizeof(notify.name));
		notify.param = m_param.special_card;
		KeJuExamManager::Instance().SendInfoToTeamMember(m_module_mgr->GetRole(), (const char*)&notify, sizeof(notify));
	}

	m_param.special_card = 0;
	KeJuExamManager::Instance().SynCardUse(m_module_mgr->GetRole());
	KeJuExamManager::Instance().SendPraiseToTeamMember(m_module_mgr->GetRole());

	this->SendInfo();
}

bool KeJuExam::SkipCard()
{
	int answer = LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetTrueAnswer(m_param.cur_question_id);
	if (answer < 0) return false;

	Protocol::SCKeJuExamUseCardInfo msg;
	msg.card_type = KeJuExamCardCfg::CARD_TYPE_SKIP;
	msg.uid = m_module_mgr->GetUid();
	msg.param = 0;
	KeJuExamManager::Instance().SendInfoToTeamMember(m_module_mgr->GetRole(), (const char*)&msg, sizeof(msg));

	KeJuExamManager::Instance().Answer(m_module_mgr, m_param.cur_question_id, answer);

	return true;
}

bool KeJuExam::RefreshCard()
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

		role_member->GetRoleModuleManager()->GetKeJuExam()->GetUsedQuestion(used_questions);
	}

	m_param.cur_question_id = LOGIC_CONFIG->GetCycleKeJuExamConfig()->RandQuestionCfg(used_questions);
	m_param.answer_timeout_timestamp = (unsigned int)EngineAdapter::Instance().Time() + LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetOtherCfg().answer_time;
	KeJuExamManager::Instance().SynTimeOut(m_module_mgr->GetRole(), m_param.answer_timeout_timestamp);

	KeJuExamManager::Instance().SynQuestion(m_module_mgr->GetRole(), m_param.cur_question_id);

	Protocol::SCKeJuExamUseCardInfo msg;
	msg.card_type = KeJuExamCardCfg::CARD_TYPE_REFRESH;
	msg.uid = m_module_mgr->GetUid();
	msg.param = m_param.cur_question_id;

	KeJuExamManager::Instance().SendInfoToTeamMember(m_module_mgr->GetRole(), (const char*)&msg, sizeof(msg));

	return true;
}

bool KeJuExam::ReduceErrorCard()
{
	int answer = LOGIC_CONFIG->GetCycleKeJuExamConfig()->GetTrueAnswer(m_param.cur_question_id);
	if (answer < 0) return false;

	//除错卡使用,随机一个不是正确答案的发给客户端
	int array[3] = { 1,2,3 };		//写死
	RandomShuffle(&array[0], 3);
	m_param.error_answer = array[0] == answer ? array[1] : array[0];		//能跑就是好代码=v= v2.0

	Protocol::SCKeJuExamUseCardInfo msg;
	msg.card_type = KeJuExamCardCfg::CARD_TYPE_REDUCE_ERROR;
	msg.uid = m_module_mgr->GetUid();
	msg.param = m_param.error_answer;

	KeJuExamManager::Instance().SynErrorAnswer(m_module_mgr->GetRole(), m_param.error_answer);
	KeJuExamManager::Instance().SendInfoToTeamMember(m_module_mgr->GetRole(), (const char*)&msg, sizeof(msg));

	return true;
}


bool KeJuExam::CanSynTeam()
{
	bool is_can_syn = false;

	if (1 == m_param.start_task_flag && 0 == m_param.cur_question_answered) is_can_syn = true;

	return is_can_syn;
}



void KeJuExam::ClearPastQuestionInfo()
{
	m_param.error_answer = 0;
// 	m_param.is_praise = 0;
	m_param.word_id = 0;
	m_param.team_used_card = 0;		// 顺便再清一下使用特殊卡
	m_param.answer_uid = 0;
}

void KeJuExam::ClearTeamInfo()
{
	m_param.TeamReset();
}

void KeJuExam::AssemblePraiseInfo(KeJuExamPraise & info)
{
	info.uid = m_module_mgr->GetUid();
	info.cur_card = m_param.special_card;
	info.word_id = m_param.word_id;
}

void KeJuExam::AssembleLastResultInfo(KeJuExamLastResult & last_result)
{
	last_result.uid = m_module_mgr->GetUid();
	last_result.cur_satisfaction = m_param.team_info.temp_satisfaction;
	last_result.praise_times = m_param.team_info.praise_times;
	last_result.true_question_num = m_param.team_info.true_question_num;
	last_result.answered_question_num = m_param.team_info.answered_question_num;
}

void KeJuExam::SendTimeOutInfo()
{
	Protocol::SCKeJuExamTimeOutInfo msg;
	msg.answer_timeout_timestamp = m_param.answer_timeout_timestamp;
	msg.answerer_role_id = m_param.answer_uid;

	m_module_mgr->NetSend(&msg, sizeof(msg));
}

void KeJuExam::SendNpcInfo()
{
	Protocol::SCKeJuExamNPCInfo msg;
	msg.npc_seq = m_cur_npc;
	msg.question_id = m_param.cur_question_id;

	m_module_mgr->NetSend(&msg, sizeof(msg));
}

void KeJuExam::SendInfo()
{
	Protocol::SCKeJuExamBaseInfo msg;
	msg.start_task_flag = m_param.start_task_flag;
	msg.is_receive_last_reward = (char)m_param.is_receive_last_reward;
	msg.used_question_count = m_param.used_question_count;
	msg.cur_satisfaction = m_param.cur_satisfaction;
	msg.cur_card = m_param.special_card;

	m_module_mgr->NetSend(&msg, sizeof(msg));
}
