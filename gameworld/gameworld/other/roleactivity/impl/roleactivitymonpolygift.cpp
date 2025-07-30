#include "roleactivitymonpolygift.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitymonpolygiftconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "battle/battle_manager_local.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "config/logicconfigmanager.hpp"
#include "other/wst_question/wstquestionconfig.hpp"
#include "other/roleattribute/roleexpconfig.hpp"

#include "other/roleactivity/impl/roleactivitywanlinggongying.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"

RoleActivityMonpolyGift::RoleActivityMonpolyGift(int type)
	:RoleActivity(type)
{

}

RoleActivityMonpolyGift::~RoleActivityMonpolyGift()
{

}

void RoleActivityMonpolyGift::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAMonpolyGiftParam));
}

void RoleActivityMonpolyGift::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RAMonpolyGiftParam));
}

void RoleActivityMonpolyGift::Update(time_t now_second)
{
	if (m_param.m_show_tamp > 0 && m_param.m_show_tamp < now_second)
	{
		this->ShowDiceEnd();
	}
}

void RoleActivityMonpolyGift::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.OnDayChnage();
	this->SendInfo();
	this->SetRoleDataChange();
}

void RoleActivityMonpolyGift::OnLoginSendInfo()
{
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_MONPOLY_GIFT))
	{
		return;
	}

	this->SendInfo();
}

void RoleActivityMonpolyGift::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_MONPOLY_GIFT != rand_act_type)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity *activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.begin_time);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.begin_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		int rand_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_MONPOLY_GIFT);
		const RandActivityMonpolyGiftConfig * config = static_cast<const RandActivityMonpolyGiftConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_MONPOLY_GIFT));
		if (NULL == config)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
		m_param.open_day = rand_open_day;
		m_param.now_map_id = 1;
		this->SetRoleDataChange();
		this->SendInfo();
	}
}

void RoleActivityMonpolyGift::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
{
	if (NULL == raor)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_MONPOLY_GIFT))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_MONPOLY_GIFT);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->SendInfo();
		}
		break;
	case 1://随机点数
		{
			this->UseDice();//先随机点数发给客户端，等客户端回复再执行步骤（或者等执行时间结束）
		}
		break;
	case 2://展示结束
		{
			this->ShowDiceEnd();
		}
		break;
	case 3:// 特殊玩法
		{
			this->SpecialPlay();
		}
		break;
	case 4://答题
		{
			this->Answer(raor->param_1);
		}
		break;
	case 5://兑换
		{
			this->ExchangeItem(raor->param_1);
		}
		break;
	}
}

void RoleActivityMonpolyGift::OnBattleFinish(bool is_win,int battle_mode)
{
	if (is_win && battle_mode == BATTLE_MODE_MONPOLY_GIFT)
	{
		this->GetRandGift();
	}
}

void RoleActivityMonpolyGift::SendInfo(int old_point, int rand_point)
{
	Protocol::SCRAMonpolyGiftInfo info;

	info.m_show_tamp = m_param.m_show_tamp;
	info.now_map_id = m_param.now_map_id;
	info.old_map_index = old_point;
	info.now_map_index = m_param.now_map_index;
	info.rand_point = rand_point;
	info.has_play_special_plan = m_param.has_play_special_plan;
	memcpy(info.today_buy_times, m_param.today_buy_times, sizeof(info.today_buy_times));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityMonpolyGift::SendQuestionInfo()
{
	Protocol::SCRAMonpolyGiftQuestionInfo info;

	info.question_id = m_param.question_id;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityMonpolyGift::UseDice()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityMonpolyGiftConfig * config = static_cast<const RandActivityMonpolyGiftConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_MONPOLY_GIFT));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	//检查原位置的详细信息
	const RAMonpolyGiftMapCfg * old_point_cfg = config->GetMapInfoCfg(ramgr, m_param.now_map_id, m_param.now_map_index);
	if (old_point_cfg == NULL)
	{
		//新玩家
		m_param.now_map_id = 1;//初始地图
		old_point_cfg = config->GetMapInfoCfg(ramgr, m_param.now_map_id, m_param.now_map_index);
		if (NULL == old_point_cfg)return;
	}

	if (old_point_cfg->layer_number_type != RAMonpolyGiftMapCfg::MAP_TYPE_NULL)
	{
		if (m_param.has_play_special_plan == 0)
		{
			return;//特殊操作未进行不能投骰子
		}
	}

	const RAMonpolyGiftOtherCfg & other_cfg = config->GetOtherCfg();

	if (NULL == ITEMPOOL->GetItem(other_cfg.cost_item))
	{
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->HasItem(other_cfg.cost_item, 1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	//随机点数
	int rand_num = config->GetRandPoint();

	int old_map = m_param.now_map_id;
	int old = m_param.now_map_index;
	if (rand_num > 0)
	{
		m_param.now_map_index += rand_num;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.cost_item, 1, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	//检查新位置的详细信息
	const RAMonpolyGiftMapCfg * new_point_cfg = config->GetMapInfoCfg(ramgr, m_param.now_map_id, m_param.now_map_index);
	if (new_point_cfg == NULL)
	{
		//新位置换图了
		m_param.now_map_id += 1;
		if (m_param.now_map_id > MAX_MAP_COUNT)
		{
			m_param.now_map_id = 1;
		}
		m_param.now_map_index = 0;
		new_point_cfg = config->GetMapInfoCfg(ramgr, m_param.now_map_id, m_param.now_map_index);
		if (new_point_cfg == NULL)return;

		this->OnNewNowMap();
	}
	else if (new_point_cfg->layer_number_type == RAMonpolyGiftMapCfg::MAP_TYPE_NEXT_MAP)
	{
		//新位置换图了
		m_param.now_map_id += 1;
		if (m_param.now_map_id > MAX_MAP_COUNT)
		{
			m_param.now_map_id = 1;
		}
		m_param.now_map_index = 0;
		new_point_cfg = config->GetMapInfoCfg(ramgr, m_param.now_map_id, m_param.now_map_index);
		if (new_point_cfg == NULL)return;

		this->OnNewNowMap();
	}

	if (new_point_cfg->layer_number_type != RAMonpolyGiftMapCfg::MAP_TYPE_NULL)
	{
		m_param.has_play_special_plan = 0;
	}
	else
	{
		m_param.has_play_special_plan = 1;
	}
	m_param.m_show_tamp = static_cast<unsigned int>(EngineAdapter::Instance().Time() + SHOW_TAMP);

	this->SendInfo(old, rand_num);
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityMonpolyGift::UseDice user[%d, %s], level[%d], type[%d], reward_item[%d] num[%d] old_map[%d] old[%d] now_map[%d] now_index[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_MONPOLY_GIFT, other_cfg.cost_item, 1, old_map, old, m_param.now_map_id, m_param.now_map_index);
}

void RoleActivityMonpolyGift::ShowDiceEnd()
{
	//展示结束
	const RandActivityMonpolyGiftConfig * config = static_cast<const RandActivityMonpolyGiftConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_MONPOLY_GIFT));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	//检查原位置的详细信息
	const RAMonpolyGiftMapCfg * point_cfg = config->GetMapInfoCfg(ramgr, m_param.now_map_id, m_param.now_map_index);
	if (point_cfg == NULL)
	{
		return;
	}

	if (point_cfg->layer_number_type != RAMonpolyGiftMapCfg::MAP_TYPE_FORWARD)
	{
		m_param.m_show_tamp = 0;
		if (point_cfg->layer_number_type == RAMonpolyGiftMapCfg::MAP_TYPE_NULL)
		{
			m_param.has_play_special_plan = 1;
		}
		return;
	}
	int old = m_param.now_map_index;
	if (point_cfg->parameter > 0)
	{
		m_param.now_map_index += point_cfg->parameter;
	}

	//检查新位置的详细信息
	const RAMonpolyGiftMapCfg * new_point_cfg = config->GetMapInfoCfg(ramgr, m_param.now_map_id, m_param.now_map_index);
	if (new_point_cfg == NULL)
	{
		//新位置换图了
		m_param.now_map_id += 1;
		if (m_param.now_map_id > MAX_MAP_COUNT)
		{
			m_param.now_map_id = 1;
		}
		new_point_cfg = config->GetMapInfoCfg(ramgr, m_param.now_map_id, m_param.now_map_index);
		if (new_point_cfg == NULL)return;

		this->OnNewNowMap();
	}

	if (new_point_cfg->layer_number_type != RAMonpolyGiftMapCfg::MAP_TYPE_NULL)
	{
		m_param.has_play_special_plan = 0;
	}
	else
	{
		m_param.has_play_special_plan = 1;
	}
	m_param.m_show_tamp = static_cast<unsigned int>(EngineAdapter::Instance().Time() + SHOW_TAMP);

	this->SendInfo(old);
	this->SetRoleDataChange();
}

void RoleActivityMonpolyGift::SpecialPlay()
{
	const RandActivityMonpolyGiftConfig * config = static_cast<const RandActivityMonpolyGiftConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_MONPOLY_GIFT));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	//检查原位置的详细信息
	const RAMonpolyGiftMapCfg * point_cfg = config->GetMapInfoCfg(ramgr, m_param.now_map_id, m_param.now_map_index);
	if (point_cfg == NULL)
	{
		return;
	}
	if (point_cfg->layer_number_type == RAMonpolyGiftMapCfg::MAP_TYPE_NULL || point_cfg->layer_number_type == RAMonpolyGiftMapCfg::MAP_TYPE_FORWARD)
	{
		return;
	}
	if (m_param.has_play_special_plan == 1)
	{
		return;//已经玩过一次了
	}
	switch (point_cfg->layer_number_type)
	{
	case RAMonpolyGiftMapCfg::MAP_TYPE_FIGHT:
		{
			Team* m_team = m_role_module_mgr->GetRole()->GetMyTeam();
			if (m_team != NULL)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_ROLE_IN_TEAM);
				return;
			}
			BattleManagerLocal::GetInstance().StartBattleModeFightReq(m_role_module_mgr->GetRole(), BATTLE_MODE_MONPOLY_GIFT,
				point_cfg->parameter, 0, 0, true);
		}
		break;
	case RAMonpolyGiftMapCfg::MAP_TYPE_ANSWER:
		{
			//下发题目
			const WSTQuestionBankCfg* question_group_cfg = LOGIC_CONFIG->GetWstQuestionConfig()->GetQuestionBankByGroupCfg(point_cfg->parameter);
			if (NULL != question_group_cfg)
			{
				if (m_param.question_id == 0)
				{
					m_param.question_id = question_group_cfg->question_id;
				}
			}
			this->SendQuestionInfo();
		}
		break;
	case RAMonpolyGiftMapCfg::MAP_TYPE_GIFT:
		{
			this->GetRandGift();
		}
		break;
	default:
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		break;
	}

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityMonpolyGift::Answer(int answer_id)
{
	//下发题目
	const WSTQuestionBankCfg* question_group_cfg = LOGIC_CONFIG->GetWstQuestionConfig()->GetQuestionBankCfg(m_param.question_id);
	if (NULL != question_group_cfg)
	{
		if (question_group_cfg->answer_id == answer_id)
		{
			m_param.question_id = 0;
			m_param.has_play_special_plan = 1;
			this->GetRandGift();
			m_role_module_mgr->NoticeNum(noticenum::NT_RA_MG_ANSWER_SUCC);
		}
		else
		{
			m_param.question_id = 0;
			this->SpecialPlay();
		}
	}

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityMonpolyGift::GetRandGift()
{
	m_param.has_play_special_plan = 1;//标记领取奖励了
	const RandActivityMonpolyGiftConfig * config = static_cast<const RandActivityMonpolyGiftConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_MONPOLY_GIFT));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	//检查原位置的详细信息
	const RAMonpolyGiftMapCfg * point_cfg = config->GetMapInfoCfg(ramgr, m_param.now_map_id, m_param.now_map_index);
	if (point_cfg == NULL)
	{
		return;
	}

	if (point_cfg->exp > 0)
	{
		const RoleLevelCfg* cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetLevelCfg(m_role_module_mgr->GetRole()->GetLevel());
		if (cfg != NULL)
		{
			long long add_exp = cfg->exp * point_cfg->exp / RAMonpolyGiftMapCfg::EXP_PERSENT;
			m_role_module_mgr->GetRole()->AddExp(add_exp, ADD_EXP_TYPE_MONPOLY_GIFT, __FUNCTION__);
		}
	}

	std::vector<ItemConfigData> real_reward_vec(point_cfg->reward_vec);//固定奖励
	 //随机奖励
	const RAMonpolyGiftRewardCfg * rand_reward = config->GetRewardCfg(point_cfg->reward_rate);
	if(NULL != rand_reward)real_reward_vec.push_back(rand_reward->reward);
	//给予
	if (!m_role_module_mgr->GetKnapsack()->PutList(static_cast<short>(real_reward_vec.size()), &real_reward_vec[0], PUT_REASON_RA_MONPOLY_GIFT))
	{
		//放入背包失败，改发邮件
		static MailContentParam contentparam; contentparam.Reset();
		for (int i = 0; i < (int)real_reward_vec.size() && i < MAX_ATTACHMENT_ITEM_NUM; ++i)
		{
			const ItemConfigData & reward = real_reward_vec[i];

			const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);

			if (item_base != NULL)
			{
				contentparam.item_list[i].item_id = reward.item_id;
				contentparam.item_list[i].num = reward.num;
				contentparam.item_list[i].is_bind = (reward.is_bind) ? 1 : 0;
				contentparam.item_list[i].invalid_time = item_base->CalInvalidTime();
			}
		}
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_content, RAND_ACTIVITY_TYPE_MONPOLY_GIFT);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
}

void RoleActivityMonpolyGift::ExchangeItem(int item_seq)
{
	if (item_seq < 0 || item_seq >= MAX_COW_LOG_TIMES)
	{
		return;
	}
	const RandActivityMonpolyGiftConfig * config = static_cast<const RandActivityMonpolyGiftConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_MONPOLY_GIFT));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	const RAMonpolyGiftExchangeCfg * exchange_cfg = config->GetExchangeCfg(item_seq);
	if (NULL == exchange_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (item_seq != exchange_cfg->item_seq)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	//购买次数
	short &has_buy_times_today = m_param.today_buy_times[exchange_cfg->item_seq];
	if (has_buy_times_today > 0 && has_buy_times_today >= exchange_cfg->today_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
		return;
	}

	int stuff_count = 0; ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	//检查消耗
	for (int t_cur = 0; t_cur < (short)exchange_cfg->reward_vec.size() && stuff_count < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM; ++t_cur)
	{
		const ItemConfigData & p_mc = exchange_cfg->reward_vec[t_cur];
		if (p_mc.num <= 0) continue;

		const ItemBase * mate_ib = ITEMPOOL->GetItem(p_mc.item_id);
		if (NULL == mate_ib) continue;

		stuff_list[stuff_count] = ItemExtern::ItemConsumeConfig(p_mc.item_id, false, p_mc.num);
		++stuff_count;
	}

	// 消耗
	if (stuff_count > 0)
	{
		static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
		if (!ItemExtern::GetConsumeListByOrder(m_role_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, true))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
			return;
		}

		if (consume_list.count > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, "WorkshopCompoundItem")) return;
	}
	else
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	//检查放入
	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(exchange_cfg->exchange_item.item_id, exchange_cfg->exchange_item.num))
	{
		//放入背包失败，改发邮件
		static MailContentParam contentparam; contentparam.Reset();
		const ItemBase * item_base = ITEMPOOL->GetItem(exchange_cfg->exchange_item.item_id);

		if (item_base != NULL)
		{
			contentparam.item_list[0].item_id = exchange_cfg->exchange_item.item_id;
			contentparam.item_list[0].num = exchange_cfg->exchange_item.num;
			contentparam.item_list[0].is_bind = (exchange_cfg->exchange_item.is_bind) ? 1 : 0;
			contentparam.item_list[0].invalid_time = item_base->CalInvalidTime();
		}
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_content, RAND_ACTIVITY_TYPE_MONPOLY_GIFT);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
	else
	{
		//给予
		if (!m_role_module_mgr->GetKnapsack()->Put(exchange_cfg->exchange_item, PUT_REASON_RA_MONPOLY_GIFT))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}

	int old_times = has_buy_times_today;
	has_buy_times_today += 1;

	this->SendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityMonpolyGift::ExchangeItem user[%d, %s], level[%d], type[%d] exchange_seq[%d] old_times[%d] now_times[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_MONPOLY_GIFT, exchange_cfg->item_seq, old_times, has_buy_times_today);
}

void RoleActivityMonpolyGift::OnNewNowMap()
{
	{
		RoleActivityWanLingGongYing * role_activity = static_cast<RoleActivityWanLingGongYing *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING));
		if (NULL != role_activity)
		{
			role_activity->OnCompleteTask(RAND_ACTIVITY_TYPE_MONPOLY_GIFT);
		}
	}
}
