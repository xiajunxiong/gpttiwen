#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/funopen/funopen.hpp"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/uniqueidgenerator.hpp"

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "protocol/randactivity/msgrashencihuafu.hpp"
#include "config/randactivityconfig/impl/randactivityshencihuafuconfig.hpp"
#include "roleactivityshencihuafu.hpp"

RoleActivityShenCiHuaFu::RoleActivityShenCiHuaFu(int type) :RoleActivity(type)
{
}

RoleActivityShenCiHuaFu::~RoleActivityShenCiHuaFu()
{
}

void RoleActivityShenCiHuaFu::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityShenCiHuaFu::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityShenCiHuaFu::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityShenCiHuaFu::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
	{
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity * activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(ramgr->GetRandActivityBeginTime(m_role_activity_type));

		this->ResetReward();
		this->SetRoleDataChange();
	}
}

void RoleActivityShenCiHuaFu::OnRAClose()
{
	this->GiveUnFetchReward();
}

void RoleActivityShenCiHuaFu::OnLoginCheck()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity * activity = ramgr->ForceGetRandActivity(this->GetRoleActivityType());
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(this->GetRoleActivityType(), m_param.ra_begin_timestamp);
	if (in_activity_time)
	{
		return;
	}

	this->GiveUnFetchReward();
}

void RoleActivityShenCiHuaFu::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.ra_day_buy_times); ++i)
	{
		m_param.ra_day_buy_times[i] = 0;
	}

	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityShenCiHuaFu::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	switch (raor->opera_type)
	{
		case 0:  //请求下发
			{
				this->SendInfo();
			}
			break;
		case 1:	//购买 p1 奖励reward_seq 
			{
				long long unique_id = 0;
				int ret_num = this->OnBuyItemCheck(raor->param_1, unique_id);

				if (ret_num == 0)
				{
					Protocol::SCCmdBuyItemCheckRet protocol;
					protocol.unique_id = unique_id;
					protocol.cfg_ver = raor->cfg_ver;
					protocol.buy_type = BUY_TYPE;

					m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
				}
			}
			break;
		case 2:	//非直购购买 p1 配置seq
			{
				this->OnBuyItemByMoneyType(raor->param_1);
			}
		case 3:	//设置自选  p1 p2 p3 选的奖励reward_seq
			{
				int select_reward_seq_list[] = { raor->param_1 ,raor->param_2,raor->param_3 };
				this->SetReward(select_reward_seq_list);
			}
			break;
		case 4:	//抽奖 p1 次数
			{
				this->DrwardReward(raor->param_1);
			}
		case 5:	//兑换奖励 p1 配置seq
			{
				this->ExchangeShop(raor->param_1);
			}

	}
}

int RoleActivityShenCiHuaFu::SetReward(const int(&select_reward_seq_list)[SHEN_CI_HUA_FU_SELECT_NUM_SELECT])
{
	if (this->IsAllSelect())
	{
		ret_line;
	}

	std::set<int> duplicate_removal_reward_seq_list;
	for (int i = 0; i < SHEN_CI_HUA_FU_SELECT_NUM_SELECT; ++i)
	{
		const int & curr = select_reward_seq_list[i];
		if (duplicate_removal_reward_seq_list.find(curr) != duplicate_removal_reward_seq_list.end())
		{
			ret_line;
		}

		duplicate_removal_reward_seq_list.insert(curr);
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		ret_line;
	}

	const RandActivityShenCiHuaFuConfig * config = static_cast<const RandActivityShenCiHuaFuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		ret_line;
	}

	for (int i = 0; i < SHEN_CI_HUA_FU_SELECT_NUM_SELECT; ++i)
	{
		const int & reward_seq = select_reward_seq_list[i];
		RAShenCiHuaFuItem & select_item = m_param.item_list_select[i];

		const RandActivityShenCiHuaFuRewardGroupItemCfg * cfg = config->GetRandActivityShenCiHuaFuRewardGroupItemCfg(ramgr, RAND_ACTIVITY_SHEN_CI_HUA_FU_REWARD_TYPE_SELECT, reward_seq);
		if (!cfg)
		{
			ret_line;
		}

		select_item.is_select = 1;
		select_item.reward_group_id = cfg->reward_group_id;
		select_item.reward_seq = cfg->reward_seq;
	}

	this->SendInfo(SHEN_CI_HUA_FU_INFO_RESON_SET);
	this->SetRoleDataChange();

	return 0;
}

struct RoleActivityShenCiHuaFuDrawItem
{
	RoleActivityShenCiHuaFuDrawItem()
	{
		rate = 0;
		index = 0;
		reward_type = 0;
		item = NULL;
		cfg = NULL;
	}

	int rate;
	int index;
	int reward_type;
	RAShenCiHuaFuItem * item;
	const RandActivityShenCiHuaFuRewardGroupItemCfg * cfg;
};

int RoleActivityShenCiHuaFu::DrwardReward(int times)
{
	Protocol::SCRAShenCiHuaFuDrawRet draw_ret;
	draw_ret.count = 0;
	for (int i = 0; i < times && draw_ret.count < ARRAY_LENGTH(draw_ret.item_list); ++i)
	{
		if (this->DrawHelper(draw_ret.item_list[draw_ret.count]) != 0)
		{
			break;
		}

		draw_ret.count += 1;
	}

	int send_len = sizeof(draw_ret) - (ARRAY_LENGTH(draw_ret.item_list) - draw_ret.count) * sizeof(draw_ret.item_list[0]);
	m_role_module_mgr->NetSend(&draw_ret, send_len);

	if (this->CheckNextTimes())
	{
		this->SendInfo(SHEN_CI_HUA_FU_INFO_RESON_RESET);
	}

	return 0;
}

int RoleActivityShenCiHuaFu::DrawHelper(Protocol::SCRAShenCiHuaFuRetItem & item)
{
	if (!this->IsAllSelect() || this->IsAllGet())
	{
		ret_line;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		ret_line;
	}

	const RandActivityShenCiHuaFuConfig * config = static_cast<const RandActivityShenCiHuaFuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		ret_line;
	}

	int rand_count = 0;
	std::vector<RoleActivityShenCiHuaFuDrawItem> draw_list;
	for (int i = 0; i < ARRAY_LENGTH(m_param.item_list_select); ++i)
	{
		RAShenCiHuaFuItem & curr = m_param.item_list_select[i];
		if (curr.is_get)
		{
			continue;
		}

		const RandActivityShenCiHuaFuRewardGroupItemCfg * cfg = config->GetRandActivityShenCiHuaFuRewardGroupItemCfg(ramgr,
			RAND_ACTIVITY_SHEN_CI_HUA_FU_REWARD_TYPE_SELECT, curr.reward_seq);
		if (!cfg)
		{
			ret_line;
		}

		rand_count += cfg->rate;

		RoleActivityShenCiHuaFuDrawItem tmp;
		tmp.rate = cfg->rate;
		tmp.reward_type = RAND_ACTIVITY_SHEN_CI_HUA_FU_REWARD_TYPE_SELECT;
		tmp.index = i;
		tmp.item = &curr;
		tmp.cfg = cfg;
		draw_list.push_back(tmp);
	}

	for (int i = 0; i < ARRAY_LENGTH(m_param.item_list_rand); ++i)
	{
		RAShenCiHuaFuItem & curr = m_param.item_list_rand[i];
		if (curr.is_get)
		{
			continue;
		}

		const RandActivityShenCiHuaFuRewardGroupItemCfg * cfg = config->GetRandActivityShenCiHuaFuRewardGroupItemCfg(ramgr,
			RAND_ACTIVITY_SHEN_CI_HUA_FU_REWARD_TYPE_RAND, curr.reward_seq);
		if (!cfg)
		{
			ret_line;
		}

		rand_count += cfg->rate;

		RoleActivityShenCiHuaFuDrawItem tmp;
		tmp.rate = cfg->rate;
		tmp.reward_type = RAND_ACTIVITY_SHEN_CI_HUA_FU_REWARD_TYPE_RAND;
		tmp.index = i;
		tmp.item = &curr;
		tmp.cfg = cfg;
		draw_list.push_back(tmp);
	}

	const RoleActivityShenCiHuaFuDrawItem * get_draw_item = NULL;
	int r = RandomNum(rand_count);
	for (int i = 0; i < (int)draw_list.size(); ++i)
	{
		const RoleActivityShenCiHuaFuDrawItem & curr = draw_list[i];
		if (r < curr.rate)
		{
			get_draw_item = &curr;
			break;
		}

		r -= curr.rate;
	}

	if (!get_draw_item)
	{
		ret_line;
	}

	const RandActivityShenCiHuaFuOtherCfg & other_cfg = config->GetOtherCfg();
	if (!m_role_module_mgr->GetKnapsack()->HasItem(other_cfg.draw_need_item_id, 1))
	{
		ret_line;
	}

	const std::vector<ItemConfigData> & rewards = get_draw_item->cfg->rewards;
	if (m_role_module_mgr->GetKnapsack()->CheckForPutList2(rewards))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_RA_SHEN_CI_HUA_FU_DRAW);
	}
	else
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNASACK_NO_SPACE_SEND_MAIL);
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_act_full_bag_title, m_role_activity_type);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_act_full_bag_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.draw_need_item_id, 1, __FUNCTION__);

	get_draw_item->item->is_get = 1;

	this->SetRoleDataChange();

	item.type = get_draw_item->reward_type;
	item.index = get_draw_item->index;

	if (get_draw_item->cfg->is_broadcast && !rewards.empty())
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_randactivity_get_item_broadcast, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
				      m_role_activity_type, rewards[0].item_id, m_role_activity_type);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}
	}


	gamelog::g_log_ra_shen_ci_hua_fu.printf(LL_INFO, "%s line:%d | role[%d %s] type:%d group_id:%d reward_seq:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		get_draw_item->reward_type, get_draw_item->cfg->reward_group_id, get_draw_item->cfg->reward_seq);

	return 0;
}

int RoleActivityShenCiHuaFu::OnBuyItemCheck(int seq, long long ARG_OUT & unique_key)
{
	unique_key = 0;

	if (seq < 0 || seq >= SHEN_CI_HUA_FU_MAX_BUY_ITEM)
	{
		ret_line;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		ret_line;
	}

	const RandActivityShenCiHuaFuConfig * config = static_cast<const RandActivityShenCiHuaFuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		ret_line;
	}

	const RandActivityShenCiHuaFuBuyItemCfg * buy_cfg = config->GetRandActivityShenCiHuaFuBuyItemCfg(ramgr, seq);
	if (buy_cfg == NULL)
	{
		ret_line;
	}

	if (!buy_cfg->is_need_chong_zhi)
	{
		ret_line;
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		if (m_param.ra_buy_times[seq] >= buy_cfg->limit_buy_times)
		{
			ret_line;
		}
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		if (m_param.ra_day_buy_times[seq] >= buy_cfg->limit_buy_times_day)
		{
			ret_line;
		}
	}

	if (buy_cfg->rewards.empty())
	{
		ret_line;
	}

	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		unique_key = 0;
		ret_line;
	}

	time_t now = EngineAdapter::Instance().Time();
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE);
	if (buy_item == NULL)
	{
		ret_line;
	}

	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = buy_cfg->need_chong_zhi_gold;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = seq;
	buy_item->param2 = buy_cfg->limit_type;
	::ItemConfigDataVecToBuyItem(buy_cfg->rewards, buy_item->rewards_item, &buy_item->rewards_num);
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_ra_shen_ci_hua_fu, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_ra_shen_ci_hua_fu.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityShenCiHuaFu::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		ret_line;
	}

	gamelog::g_log_ra_shen_ci_hua_fu.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		ret_line;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return  -__LINE__;
	}

	std::vector<ItemConfigData> rewards = ::BuyItemToItemConfigDataVec(buy_item->rewards_num, buy_item->rewards_item);
	if (rewards.empty())
	{
		return  -__LINE__;
	}

	int seq = buy_item->param1;
	int limit_type = buy_item->param2;

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(rewards))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_RA_SHEN_CI_HUA_FU_BUY);
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	//如果时间相等就说明同一次活动里购买，要增加次数。
	if (buy_item->ra_begin_timestamp == m_param.ra_begin_timestamp)
	{
		if (0 <= seq && seq < SHEN_CI_HUA_FU_MAX_BUY_ITEM)
		{
			if (limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
			{
				m_param.ra_buy_times[seq] += 1;
			}

			if (limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
			{
				m_param.ra_day_buy_times[seq] += 1;
			}
		}

		this->SendInfo(SHEN_CI_HUA_FU_INFO_RESON_BUY);
		this->SetRoleDataChange();
	}

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_ra_shen_ci_hua_fu, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

int RoleActivityShenCiHuaFu::OnBuyItemByMoneyType(int seq)
{
	if (seq < 0 || seq >= SHEN_CI_HUA_FU_MAX_BUY_ITEM)
	{
		ret_line;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		ret_line;
	}

	const RandActivityShenCiHuaFuConfig * config = static_cast<const RandActivityShenCiHuaFuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		ret_line;
	}

	const RandActivityShenCiHuaFuBuyItemCfg * buy_cfg = config->GetRandActivityShenCiHuaFuBuyItemCfg(ramgr, seq);
	if (buy_cfg == NULL)
	{
		ret_line;
	}

	if (buy_cfg->is_need_chong_zhi)
	{
		ret_line;
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		if (m_param.ra_buy_times[seq] >= buy_cfg->limit_buy_times)
		{
			ret_line;
		}
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		if (m_param.ra_day_buy_times[seq] >= buy_cfg->limit_buy_times_day)
		{
			ret_line;
		}
	}

	if (buy_cfg->rewards.empty())
	{
		ret_line;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(buy_cfg->rewards))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HAS_NO_SPACING);
		ret_line;
	}

	if (!m_role_module_mgr->GetMoney()->UseMoney(buy_cfg->money_type, buy_cfg->money_value, __FUNCTION__))
	{
		ret_line;
	}

	m_role_module_mgr->GetKnapsack()->PutVec(buy_cfg->rewards, PUT_REASON_RA_SHEN_CI_HUA_FU_BUY);

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		m_param.ra_buy_times[seq] += 1;
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		m_param.ra_day_buy_times[seq] += 1;
	}

	this->SetRoleDataChange();
	this->SendInfo(SHEN_CI_HUA_FU_INFO_RESON_BUY);

	return 0;
}

int RoleActivityShenCiHuaFu::OnBuyGiftItem(int seq, ARG_OUT int * price)
{
	if (seq < 0 || seq >= SHEN_CI_HUA_FU_MAX_BUY_ITEM)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	const RandActivityShenCiHuaFuConfig * config = static_cast<const RandActivityShenCiHuaFuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	const RandActivityShenCiHuaFuBuyItemCfg * buy_cfg = config->GetRandActivityShenCiHuaFuBuyItemCfg(ramgr, seq);
	if (buy_cfg == NULL)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	if (!buy_cfg->is_need_chong_zhi)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		if (m_param.ra_buy_times[seq] >= buy_cfg->limit_buy_times)
		{
			return BUY_GIFT_ITEM_RET_TYPE_2;
		}
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		if (m_param.ra_day_buy_times[seq] >= buy_cfg->limit_buy_times_day)
		{
			return BUY_GIFT_ITEM_RET_TYPE_2;
		}
	}

	if (buy_cfg->rewards.empty())
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	if (NULL != price)
	{
		*price = buy_cfg->need_chong_zhi_gold;
		return 0;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(buy_cfg->rewards))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(buy_cfg->rewards, PUT_REASON_RA_SHEN_CI_HUA_FU_BUY);
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, buy_cfg->rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		m_param.ra_buy_times[seq] += 1;
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		m_param.ra_day_buy_times[seq] += 1;
	}

	this->SendInfo(SHEN_CI_HUA_FU_INFO_RESON_BUY);
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

int RoleActivityShenCiHuaFu::ExchangeShop(int seq)
{
	if (seq < 0 || seq >= SHEN_CI_HUA_FU_MAX_EXCHANGE_SHOP_ITEM)
	{
		ret_line;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		ret_line;
	}

	const RandActivityShenCiHuaFuConfig * config = static_cast<const RandActivityShenCiHuaFuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		ret_line;
	}

	const RandActivityShenCiHuaFuExchangeShopItemCfg * cfg = config->GetRandActivityShenCiHuaFuExchangeShopItemCfg(ramgr, seq);
	if (!cfg)
	{
		ret_line;
	}

	if (cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		if (m_param.ra_exchange_times[seq] >= cfg->limit_times)
		{
			ret_line;
		}
	}

	if (cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		if (m_param.ra_day_exchange_times[seq] >= cfg->limit_times_day)
		{
			ret_line;
		}
	}

	if (!m_role_module_mgr->GetKnapsack()->HasItem(cfg->need_item_id, cfg->need_item_num))
	{
		ret_line;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(cfg->rewards))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HAS_NO_SPACING);
		ret_line;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->need_item_id, cfg->need_item_num, __FUNCTION__))
	{
		ret_line;
	}

	m_role_module_mgr->GetKnapsack()->PutVec(cfg->rewards, PUT_REASON_RA_SHEN_CI_HUA_FU_BUY);

	if (cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		m_param.ra_exchange_times[seq] += 1;
	}

	if (cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		m_param.ra_day_exchange_times[seq] += 1;
	}

	this->SetRoleDataChange();
	this->SendInfo(SHEN_CI_HUA_FU_INFO_RESON_EXCHANGE);

	return 0;
}

void RoleActivityShenCiHuaFu::SendInfo(int reason)
{
	Protocol::SCRAShenCiHuaFuInfo protocol;

	protocol.reason = reason;
	SameArrayTypeCopy(protocol.ra_day_buy_times, m_param.ra_day_buy_times);
	SameArrayTypeCopy(protocol.ra_buy_times, m_param.ra_buy_times);
	SameArrayTypeCopy(protocol.ra_day_exchange_times, m_param.ra_day_exchange_times);
	SameArrayTypeCopy(protocol.ra_exchange_times, m_param.ra_exchange_times);
	SameArrayTypeCopy(protocol.item_list_select, m_param.item_list_select);
	SameArrayTypeCopy(protocol.item_list_rand, m_param.item_list_rand);

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

void RoleActivityShenCiHuaFu::GiveUnFetchReward()
{
}

bool RoleActivityShenCiHuaFu::IsAllSelect()
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.item_list_select); ++i)
	{
		if (m_param.item_list_select[i].is_select == 0)
		{
			return false;
		}
	}

	return true;
}

bool RoleActivityShenCiHuaFu::IsAllGet()
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.item_list_select); ++i)
	{
		if (m_param.item_list_select[i].is_get == 0)
		{
			return false;
		}
	}

	for (int i = 0; i < ARRAY_LENGTH(m_param.item_list_rand); ++i)
	{
		if (m_param.item_list_rand[i].is_get == 0)
		{
			return false;
		}
	}

	return true;
}

bool RoleActivityShenCiHuaFu::CheckNextTimes()
{
	if (!this->IsAllGet())
	{
		return false;
	}

	this->ResetReward();

	return true;
}

void RoleActivityShenCiHuaFu::ResetReward()
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.item_list_select); ++i)
	{
		m_param.item_list_select[i].Reset();
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	const RandActivityShenCiHuaFuConfig * config = static_cast<const RandActivityShenCiHuaFuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (ramgr && config)
	{
		const RandActivityShenCiHuaFuRewardGroupCfg * cfg = config->GetRandActivityShenCiHuaFuRewardGroupCfg(ramgr, RAND_ACTIVITY_SHEN_CI_HUA_FU_REWARD_TYPE_RAND);
		if (!cfg)
		{
			return;
		}

		std::vector<RandActivityShenCiHuaFuRewardGroupItemCfg> rand_list = MapValuesAsVector(cfg->item_list);
		if (rand_list.empty())
		{
			return;
		}

		std::random_shuffle(rand_list.begin(), rand_list.end());
		for (int i = 0; i < ARRAY_LENGTH(m_param.item_list_rand); ++i)
		{
			RAShenCiHuaFuItem & curr = m_param.item_list_rand[i];
			curr.Reset();
			
			int r = i % (int)rand_list.size();
			RandActivityShenCiHuaFuRewardGroupItemCfg & rand_item = rand_list[r];
			curr.reward_group_id = rand_item.reward_group_id;
			curr.reward_seq = rand_item.reward_seq;
		}
	}

	this->SetRoleDataChange();
}
