#include "servercommon/errornum.h"
#include "wst_question.hpp"
#include "wstquestionconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "obj/character/role.h"
#include "item/money.h"
#include "item/knapsack.h"
#include "protocol/msgother.h"
#include "other/funopen/funopen.hpp"
#include "gamelog.h"
#include "other/dayreward/dayrewardconfig.hpp"
#include "other/levelcomplement/levelcomplement.h"
#include "global/worldstatus/worldstatus.hpp"

WstQuestion::WstQuestion()
{
}

WstQuestion::~WstQuestion()
{
}

void WstQuestion::Init(RoleModuleManager * role_module_manager )
{
	m_role_module_mgr = role_module_manager;
}

void WstQuestion::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{

}

void WstQuestion::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;
	this->OnFindDataReset();

	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_ANSWER))	return;

	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	m_role_module_mgr->GetCommonData().wst_answer_num = 0;
	m_role_module_mgr->GetCommonData().wst_question_add_coin = 0;
	m_role_module_mgr->GetCommonData().wst_question_add_exp = 0;
	m_role_module_mgr->GetCommonData().wst_questions_group = 1;	//重置时默认第一组
	m_role_module_mgr->GetCommonData().wst_questions_id = 0;

	// 如果昨天未上线就拿当前等级计算昨日奖励的次数
	const WSTQuestionRewardCfg *reward_cfg = LOGIC_CONFIG->GetWstQuestionConfig()->GetRewardCfg(m_role_module_mgr->GetRole()->GetLevel());
	if (NULL != reward_cfg)
	{
		if (common_data.offine_day >= 2)
		{
			common_data.wst_question_yesterday_buy_exp_count = reward_cfg->num;
			common_data.wst_question_today_buy_exp_count = reward_cfg->num;
		}
		else		// 否则就把 今日奖励的次数放到昨日
		{
			common_data.wst_question_yesterday_buy_exp_count = common_data.wst_question_today_buy_exp_count;
			common_data.wst_question_today_buy_exp_count = reward_cfg->num;
		}
	}

	this->SendAllInfo();
}

bool WstQuestion::OnWstQuestionFindReward(int consume_type, int count)
{
	return false;//屏蔽神算子玩法
	if (count > m_role_module_mgr->GetCommonData().wst_answer_yesterday_num)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FIND_COUNT_NOT_ENOUGH);
		return false;
	}
	/*int reward_group = LOGIC_CONFIG->GetWstQuestionConfig()->GetRewardGroup(m_role_module_mgr->GetRole()->GetLevel());
	if (reward_group == -1) return false;*/

	int exp = 0;
	int coin = 0;
	for (int i = 0; i < count; ++i)
	{
		int temp_exp = 0;
		int temp_coin = 0;
		if (LOGIC_CONFIG->GetWstQuestionConfig()->GetFindReward(m_role_module_mgr->GetRole()->GetLevel(), consume_type, &temp_coin, &temp_exp))
		{
			if (temp_exp > 0)exp += temp_exp;
			if (temp_coin > 0)coin += temp_coin;
		}
		else
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FIND_REWARD_ERROR);
			return false;
		}
	}

	m_role_module_mgr->GetMoney()->AddCoinBind(coin, __FUNCTION__);
	long long real_add_exp = m_role_module_mgr->GetRole()->AddExp(exp, ADD_EXP_TYPE_FIND_REWARD, __FUNCTION__);
	m_role_module_mgr->GetLevelComplement()->OnAddExp(real_add_exp);

	m_role_module_mgr->GetCommonData().wst_answer_yesterday_num -= count;
	m_role_module_mgr->GetCommonData().today_activity_type_find_count[DAY_ACTIVITY_TYPE_WST_QUESTION] += count;
	m_role_module_mgr->GetCommonData().today_find_reward_flag.SetBit(DAY_ACTIVITY_TYPE_WST_QUESTION);
	EventHandler::Instance().OnAnswerQuestion(m_role_module_mgr->GetRole(), true, count, true);
	return true;
}

void WstQuestion::OnFindDataReset()
{
	const DayRewardFindCfg *cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(DAY_ACTIVITY_TYPE_WST_QUESTION);
	if (NULL == cfg || m_role_module_mgr->GetRole()->GetLevel() < cfg->find_level) return;

	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	const WSTQuestionOtherCfg * wst_other = LOGIC_CONFIG->GetWstQuestionConfig()->GetOtherCfg();
	if (NULL != wst_other)
	{
		if (common_data.offine_day >= 2)
		{
			common_data.wst_answer_yesterday_num = wst_other->question_num;
		}
		else
		{
			if (wst_other->question_num >= common_data.wst_answer_num)
			{
				common_data.wst_answer_yesterday_num = wst_other->question_num - common_data.wst_answer_num;
			}
			else
			{
				common_data.wst_answer_yesterday_num = 0;
			}
		}
	}
}

void WstQuestion::OnBuyExp(int buy_type)
{
	if (buy_type < 0 || buy_type > 1) return;
	const WSTQuestionRewardCfg *reward_cfg = LOGIC_CONFIG->GetWstQuestionConfig()->GetRewardCfg(m_role_module_mgr->GetRole()->GetLevel());
	if (NULL == reward_cfg) return;
	//////////////////////////////////////////检查////////////////////////////////////////////////////
	char* buy_count = NULL;
	if (0 == buy_type)
	{
		buy_count = &m_role_module_mgr->GetCommonData().wst_question_today_buy_exp_count;
	}
	else
	{
		buy_count = &m_role_module_mgr->GetCommonData().wst_question_yesterday_buy_exp_count;
	}

	if (NULL == buy_count) return;
	if (*buy_count <= 0)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_WST_NO_BUY_EXP_COUNT);
		return;
	}

	int max_question = LOGIC_CONFIG->GetWstQuestionConfig()->GetOtherCfg()->question_num;
	if (max_question > m_role_module_mgr->GetCommonData().wst_answer_num)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_CONDITIONS_NOT_REACHED);
		return;
	}

	int use_gold = reward_cfg->gold;
	if (m_role_module_mgr->GetMoney()->GetGold() < use_gold)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}
	//////////////////////////////////////////消耗////////////////////////////////////////////////////
	m_role_module_mgr->GetMoney()->UseGold(use_gold, __FUNCTION__);
	//////////////////////////////////////////给予////////////////////////////////////////////////////
	int add_exp = reward_cfg->exp_buy;
	long long real_add_exp = m_role_module_mgr->GetRole()->AddExp(add_exp, ADD_EXP_TYPE_QUESTION_BUY_EXP, __FUNCTION__);
	m_role_module_mgr->GetLevelComplement()->OnAddExp(real_add_exp);
	EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_role_module_mgr->GetRole(), add_exp, __FUNCTION__);
	--(*buy_count);

	this->SendAllInfo();
}

void WstQuestion::OnQuestion(int answer_id, bool _is_onekey_finish)
{
	int max_question = LOGIC_CONFIG->GetWstQuestionConfig()->GetOtherCfg()->question_num;
	if (max_question <= m_role_module_mgr->GetCommonData().wst_answer_num)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_WST_COUNT_LIMIT);
		return;
	}
	const WSTQuestionBankCfg * question_cfg = LOGIC_CONFIG->GetWstQuestionConfig()->GetQuestionBankCfg(m_role_module_mgr->GetCommonData().wst_questions_id);
	if (NULL == question_cfg) return;
	const WSTQuestionRewardCfg *reward_cfg = LOGIC_CONFIG->GetWstQuestionConfig()->GetRewardCfg(m_role_module_mgr->GetRole()->GetLevel());
	if (NULL == reward_cfg) return;

	// 答案是否正确
	bool is_correct = false;
	if (answer_id == question_cfg->answer_id)
	{
		is_correct = true;
	}

	int add_exp = reward_cfg->exp;
	int add_coin = reward_cfg->coin;
	if (is_correct)
	{
	}
	else
	{
		add_coin -= (add_coin * LOGIC_CONFIG->GetWstQuestionConfig()->GetOtherCfg()->coin_reduction_ratio / 100);
	}

	long long real_add_exp = m_role_module_mgr->GetRole()->AddExp(add_exp, ADD_EXP_TYPE_QUESTION, __FUNCTION__);
	m_role_module_mgr->GetLevelComplement()->OnAddExp(real_add_exp);
	EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_role_module_mgr->GetRole(), add_exp, __FUNCTION__);
	if(!_is_onekey_finish) m_role_module_mgr->GetMoney()->AddCoinBind(add_coin, __FUNCTION__);
	if (!_is_onekey_finish) m_role_module_mgr->GetCommonData().wst_question_add_coin += add_coin;
	m_role_module_mgr->GetCommonData().wst_question_add_exp += add_exp;
	m_role_module_mgr->GetCommonData().wst_answer_num++;
	this->SendWstInfo();
	//int rand_id = RandomNum(LOGIC_CONFIG->GetWstQuestionConfig()->GetQuestionBankNum());
	// 
	const WSTQuestionBankCfg* question_group_cfg = LOGIC_CONFIG->GetWstQuestionConfig()->GetQuestionBankByGroupCfg(m_role_module_mgr->GetCommonData().wst_questions_group + 1);
	if (question_group_cfg == NULL)//没有下一题了，下发一次状态
	{
		if (max_question <= m_role_module_mgr->GetCommonData().wst_answer_num)
		{
			m_role_module_mgr->GetCommonData().wst_question_today_buy_exp_count = reward_cfg->num;
		}
		this->SendAllInfo();
		EventHandler::Instance().OnAnswerQuestion(m_role_module_mgr->GetRole(), is_correct, 1);
		return;
	}

	m_role_module_mgr->GetCommonData().wst_questions_id = question_group_cfg->question_id;
	m_role_module_mgr->GetCommonData().wst_questions_group++;

	this->SendAllInfo();
	EventHandler::Instance().OnAnswerQuestion(m_role_module_mgr->GetRole(), is_correct, 1);
	WorldStatus::Instance().LogFunctionStats(m_role_module_mgr->GetRole(), "WstQuestion");
}

void WstQuestion::OneKeyFinishWstQuation()
{
	return;//屏蔽神算子玩法
	int max_question = LOGIC_CONFIG->GetWstQuestionConfig()->GetOtherCfg()->question_num; //最大可回答次数
	if (max_question <= m_role_module_mgr->GetCommonData().wst_answer_num)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TASK_ALREADY_FINISHED);
		return;
	}
	int surplus_question = max_question - m_role_module_mgr->GetCommonData().wst_answer_num; //剩余回答的题目个数
	if (surplus_question == 0) return;

	const DayRewardFindCfg *dr_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(DAY_ACTIVITY_TYPE_WST_QUESTION);
	if (!dr_cfg) return;
	const DayRewardFindPriceCfg * price_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindRewardPriceCfg(DAY_ACTIVITY_TYPE_WST_QUESTION, m_role_module_mgr->GetRole()->GetLevel());
	if (NULL == price_cfg) return;

	int consume_money = 0;
	int consume_money_type = LOGIC_CONFIG->GetWstQuestionConfig()->GetFindType(m_role_module_mgr->GetRole()->GetLevel());
	if (consume_money_type != RB_TYPE_COIN) return;

	switch (consume_money_type)
	{
	case RB_TYPE_COIN:
		{
			consume_money = (price_cfg->coin_price * surplus_question * price_cfg->coin_scale) / 100;
			if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(consume_money))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
			m_role_module_mgr->GetMoney()->UseCoinBind(consume_money, __FUNCTION__);
		}
		break;
/*	case RB_TYPE_GOLD:
		{
			consume_money = surplus_question * dr_cfg->gold_price;
			if (!m_role_module_mgr->GetMoney()->GoldMoreThan(consume_money))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
			m_role_module_mgr->GetMoney()->UseGold(consume_money, __FUNCTION__);
		}
		break;*/
	default:
		return;
		break;
	}


	for (int time = 0; time < surplus_question;++time)
	{
		const WSTQuestionBankCfg * question_cfg = LOGIC_CONFIG->GetWstQuestionConfig()->GetQuestionBankCfg(m_role_module_mgr->GetCommonData().wst_questions_id);
		if (NULL == question_cfg) break;

		this->OnQuestion(question_cfg->answer_id,true);
	}
	gamelog::g_log_task.printf(LL_INFO, "wst_question sell[%d] gold suc", consume_money);
	EventHandler::Instance().OnAnswerQuestion(m_role_module_mgr->GetRole(), true, surplus_question);
}

void WstQuestion::SendAllInfo()
{
	// 防止策划删题目 导致题目id取不到配置
	const WSTQuestionBankCfg * question_cfg = LOGIC_CONFIG->GetWstQuestionConfig()->GetQuestionBankCfg(m_role_module_mgr->GetCommonData().wst_questions_id);
	if (NULL == question_cfg)
	{
		//从原来那一组进行随机 || 估计有些账号今天已经重置过了添加一层判断
		if(m_role_module_mgr->GetCommonData().wst_questions_group == 0) m_role_module_mgr->GetCommonData().wst_questions_group = 1;
		const WSTQuestionBankCfg* question_group_cfg = LOGIC_CONFIG->GetWstQuestionConfig()->GetQuestionBankByGroupCfg(m_role_module_mgr->GetCommonData().wst_questions_group);
		if (question_group_cfg == NULL)
		{
			//一般来到这里都是超过范围 此时直接选取最后一组 
			question_group_cfg = LOGIC_CONFIG->GetWstQuestionConfig()->GetQuestionBankByGroupCfg(LOGIC_CONFIG->GetWstQuestionConfig()->GetOtherCfg()->question_num);
			if (question_group_cfg == NULL) return;
		}

		m_role_module_mgr->GetCommonData().wst_questions_id = question_group_cfg->question_id;
	}
		
	Protocol::SCAnswerAllInfo info;
	info.wst_questions_id = m_role_module_mgr->GetCommonData().wst_questions_id;
	info.wst_question_today_buy_exp_count = m_role_module_mgr->GetCommonData().wst_question_today_buy_exp_count;
	info.wst_question_yesterday_buy_exp_count = m_role_module_mgr->GetCommonData().wst_question_yesterday_buy_exp_count;
	info.wst_answer_num = m_role_module_mgr->GetCommonData().wst_answer_num;
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void WstQuestion::SendWstInfo()
{
	static Protocol::SCAnswerOverInfo info;
	info.add_coin = m_role_module_mgr->GetCommonData().wst_question_add_coin;
	info.add_exp = m_role_module_mgr->GetCommonData().wst_question_add_exp;
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}
