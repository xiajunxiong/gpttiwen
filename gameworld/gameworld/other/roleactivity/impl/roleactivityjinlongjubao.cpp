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
#include "protocol/randactivity/msgrajinlongjubao.hpp"
#include "config/randactivityconfig/impl/randactivityjinlongjubaoconfig.hpp"
#include "roleactivityjinlongjubao.hpp"

RoleActivityJinLongJuBao::RoleActivityJinLongJuBao(int type) :RoleActivity(type)
{
}

RoleActivityJinLongJuBao::~RoleActivityJinLongJuBao()
{
}

void RoleActivityJinLongJuBao::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityJinLongJuBao::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityJinLongJuBao::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityJinLongJuBao::CheckInitRandActivity(int rand_act_type, bool is_force)
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

void RoleActivityJinLongJuBao::OnRAClose()
{
	this->GiveUnFetchReward();
}

void RoleActivityJinLongJuBao::OnLoginCheck()
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

void RoleActivityJinLongJuBao::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.ra_day_buy_times); ++i)
	{
		m_param.ra_day_buy_times[i] = 0;
	}

	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityJinLongJuBao::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
		case 2:	//领取轮次奖励 p1选的奖励reward_seq
			{
				this->FetchTimesReward(raor->param_1);
			}
			break;
		case 3:	//设置自选  p1 自选格子奖励，p2 选的奖励reward_seq
			{
				this->SetReward(raor->param_1, raor->param_2);
			}
			break;
		case 4:	//抽奖
			{
				this->DrwardReward(raor->param_1);
			}
	}
}

int RoleActivityJinLongJuBao::FetchTimesReward(int reward_seq)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityJinLongJuBaoConfig * config = static_cast<const RandActivityJinLongJuBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RandActivityJinLongJuBaoTimesRewardCfg * cfg = config->GetRandActivityJinLongJuBaoTimesRewardCfg(ramgr, reward_seq);
	if (!cfg)
	{
		return -__LINE__;
	}

	if (cfg->seq >= BitCount(m_param.times_reward_fetch_flag))
	{
		return -__LINE__;
	}

	if (IsSetBit(m_param.times_reward_fetch_flag, cfg->seq))
	{
		return -__LINE__;
	}

	if (m_param.draw_times < cfg->times)
	{
		return -__LINE__;
	}

	const std::vector<ItemConfigData> & rewards = cfg->rewards;
	if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(rewards))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HAS_NO_SPACING);
		return -__LINE__;
	}

	m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_RA_JIN_LONG_JU_BAO_FETCH);
	SetBit(m_param.times_reward_fetch_flag, cfg->seq);

	this->SendInfo(JIN_LONG_JU_BAO_INFO_RESON_FETCH);
	this->SetRoleDataChange();

	gamelog::g_log_jin_long_ju_bao.printf(LL_INFO, "%s line:%d | role[%d %s] reward_seq:%d draw_times:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		cfg->seq, m_param.draw_times);


	return 0;
}

int RoleActivityJinLongJuBao::SetReward(int index, int reward_seq)
{
	if (index < 0 || index >= ARRAY_LENGTH(m_param.item_list_select))
	{
		return -__LINE__;
	}

	RAJinLongJuBaoItem & select_item = m_param.item_list_select[index];
	if (select_item.is_get)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityJinLongJuBaoConfig * config = static_cast<const RandActivityJinLongJuBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RandActivityJinLongJuBaoRewardGroupItemCfg * cfg = config->GetRandActivityJinLongJuBaoRewardGroupItemCfg(ramgr, RAND_ACTIVITY_JIN_LONG_JU_BAO_REWARD_TYPE_SELECT, reward_seq);
	if (!cfg)
	{
		return -__LINE__;
	}

	select_item.is_select = 1;
	select_item.reward_group_id = cfg->reward_group_id;
	select_item.reward_seq = cfg->reward_seq;

	this->SendInfo(JIN_LONG_JU_BAO_INFO_RESON_SET);
	this->SetRoleDataChange();

	return 0;
}

struct RoleActivityJinLongJuBaoDrawItem
{
	RoleActivityJinLongJuBaoDrawItem()
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
	RAJinLongJuBaoItem * item;
	const RandActivityJinLongJuBaoRewardGroupItemCfg * cfg;
};

int RoleActivityJinLongJuBao::DrwardReward(int times)
{
	Protocol::SCRAJinLongJuBaoDrawRet draw_ret;
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
		this->SendInfo(JIN_LONG_JU_BAO_INFO_RESON_RESET);
	}

	return 0;
}

int RoleActivityJinLongJuBao::DrawHelper(Protocol::SCRAJinLongJuBaoDrawRetItem & item)
{
	if (!this->IsAllSelect() || this->IsAllGet())
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityJinLongJuBaoConfig * config = static_cast<const RandActivityJinLongJuBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	std::vector<RoleActivityJinLongJuBaoDrawItem> bao_di_cfg_list;
	std::set<int> bao_di_id_list;

	int rand_count = 0;
	std::vector<RoleActivityJinLongJuBaoDrawItem> draw_list;
	for (int i = 0; i < ARRAY_LENGTH(m_param.item_list_select); ++i)
	{
		RAJinLongJuBaoItem & curr = m_param.item_list_select[i];
		if (curr.is_get)
		{
			continue;
		}

		const RandActivityJinLongJuBaoRewardGroupItemCfg * cfg = config->GetRandActivityJinLongJuBaoRewardGroupItemCfg(ramgr,
			RAND_ACTIVITY_JIN_LONG_JU_BAO_REWARD_TYPE_SELECT, curr.reward_seq);
		if (!cfg)
		{
			return -__LINE__;
		}

		rand_count += cfg->rate;

		RoleActivityJinLongJuBaoDrawItem tmp;
		tmp.rate = cfg->rate;
		tmp.reward_type = RAND_ACTIVITY_JIN_LONG_JU_BAO_REWARD_TYPE_SELECT;
		tmp.index = i;
		tmp.item = &curr;
		tmp.cfg = cfg;
		draw_list.push_back(tmp);

		if (cfg->bao_di_times > 0)
		{
			bao_di_cfg_list.push_back(tmp);
			bao_di_id_list.insert(cfg->bao_di_id);
		}
	}

	for (int i = 0; i < ARRAY_LENGTH(m_param.item_list_rand); ++i)
	{
		RAJinLongJuBaoItem & curr = m_param.item_list_rand[i];
		const RandActivityJinLongJuBaoRewardGroupItemCfg * cfg = config->GetRandActivityJinLongJuBaoRewardGroupItemCfg(ramgr,
			RAND_ACTIVITY_JIN_LONG_JU_BAO_REWARD_TYPE_RAND, curr.reward_seq);
		if (!cfg)
		{
			return -__LINE__;
		}

		rand_count += cfg->rate;

		RoleActivityJinLongJuBaoDrawItem tmp;
		tmp.rate = cfg->rate;
		tmp.reward_type = RAND_ACTIVITY_JIN_LONG_JU_BAO_REWARD_TYPE_RAND;
		tmp.index = i;
		tmp.item = &curr;
		tmp.cfg = cfg;
		draw_list.push_back(tmp);

		if (cfg->bao_di_times > 0)
		{
			bao_di_cfg_list.push_back(tmp);
			bao_di_id_list.insert(cfg->bao_di_id);
		}
	}

	const RoleActivityJinLongJuBaoDrawItem * get_draw_item = NULL;
	int r = RandomNum(rand_count);
	for (int i = 0; i < (int)draw_list.size(); ++i)
	{
		const RoleActivityJinLongJuBaoDrawItem & curr = draw_list[i];
		if (r < curr.rate)
		{
			get_draw_item = &curr;
			break;
		}

		r -= curr.rate;
	}

	for (int bao_di_index = 0; bao_di_index < (int)bao_di_cfg_list.size(); ++bao_di_index)
	{
		const RoleActivityJinLongJuBaoDrawItem & curr = bao_di_cfg_list[bao_di_index];

		BaoDiInfoItem * bao_di_info_item = this->GetBaoDiInfoItem(curr.cfg->bao_di_id);
		if (bao_di_info_item)
		{
			if (bao_di_info_item->is_get != 0)
			{
				continue;
			}

			if (bao_di_info_item->times + 1 >= curr.cfg->bao_di_times)
			{
				get_draw_item = &curr;

				gamelog::g_log_jin_long_ju_bao.printf(LL_INFO, "%s %d | role_id:[%d %s] get_bao_di_reward bao_di_id:%d reward_seq:%d need_times:%d draw_times:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRoleName(),
					curr.cfg->bao_di_id, curr.cfg->reward_seq, curr.cfg->bao_di_times, bao_di_info_item->times + 1);

				break;
			}
		}
	}

	if (!get_draw_item)
	{
		ret_line;
	}

	const RandActivityJinLongJuBaoOtherCfg & other_cfg = config->GetOtherCfg();
	if (!m_role_module_mgr->GetKnapsack()->HasItem(other_cfg.draw_need_item_id, other_cfg.draw_need_item_num))
	{
		ret_line;
	}

	const std::vector<ItemConfigData> & rewards = get_draw_item->cfg->rewards;
	if (m_role_module_mgr->GetKnapsack()->CheckForPutList2(rewards))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_RA_JIN_LONG_JU_BAO_DRAW);
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

	this->AddRecord(rewards);

	m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.draw_need_item_id, other_cfg.draw_need_item_num, __FUNCTION__);

	get_draw_item->item->is_get = 1;
	m_param.draw_times += 1;

	this->SetBaoDiInfoAfterDraw(get_draw_item->cfg->bao_di_id, bao_di_id_list);
	this->SetRoleDataChange();

	item.time = (unsigned int)EngineAdapter::Instance().Time();
	item.times = m_param.draw_times;
	item.type = get_draw_item->reward_type;
	item.index = get_draw_item->index;

	if (get_draw_item->reward_type == RAND_ACTIVITY_JIN_LONG_JU_BAO_REWARD_TYPE_SELECT && !rewards.empty())
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_randactivity_get_item_broadcast, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
				      m_role_activity_type, rewards[0].item_id, m_role_activity_type);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}
	}


	gamelog::g_log_jin_long_ju_bao.printf(LL_INFO, "%s line:%d | role[%d %s] type:%d group_id:%d reward_seq:%d draw_times:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		get_draw_item->reward_type, get_draw_item->cfg->reward_group_id, get_draw_item->cfg->reward_seq, m_param.draw_times);

	return 0;
}

int RoleActivityJinLongJuBao::OnBuyItemCheck(int seq, long long ARG_OUT & unique_key)
{
	unique_key = 0;

	if (seq < 0 || seq >= JIN_LONG_JU_BAO_MAX_BUY_ITEM)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityJinLongJuBaoConfig * config = static_cast<const RandActivityJinLongJuBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RandActivityJinLongJuBaBuyItemCfg * buy_cfg = config->GetRandActivityJinLongJuBaBuyItemCfg(ramgr, seq);
	if (buy_cfg == NULL)
	{
		return -__LINE__;
	}

	if (buy_cfg->limit_type == 1)
	{
		if (m_param.ra_buy_times[seq] >= buy_cfg->limit_buy_times)
		{
			return -__LINE__;
		}
	}

	if (buy_cfg->limit_type == 2)
	{
		if (m_param.ra_day_buy_times[seq] >= buy_cfg->limit_buy_times_day)
		{
			return -__LINE__;
		}
	}

	if (buy_cfg->rewards.empty())
	{
		return -__LINE__;
	}

	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		unique_key = 0;
		return -__LINE__;
	}

	time_t now = EngineAdapter::Instance().Time();
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = buy_cfg->need_chong_zhi_gold;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = seq;
	buy_item->param2 = buy_cfg->limit_type;
	::ItemConfigDataVecToBuyItem(buy_cfg->rewards, buy_item->rewards_item, &buy_item->rewards_num);
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_jin_long_ju_bao, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_jin_long_ju_bao.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityJinLongJuBao::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_jin_long_ju_bao.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
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
		m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_RA_JIN_LONG_JU_BAO_BUY);
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
		if (0 <= seq && seq < JIN_LONG_JU_BAO_MAX_BUY_ITEM)
		{
			if (limit_type == 1)
			{
				m_param.ra_buy_times[seq] += 1;
			}

			if (limit_type == 2)
			{
				m_param.ra_day_buy_times[seq] += 1;
			}
		}

		this->SendInfo();
		this->SetRoleDataChange();
	}

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_jin_long_ju_bao, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

int RoleActivityJinLongJuBao::OnBuyGiftItem(int seq, ARG_OUT int * price)
{
	if (seq < 0 || seq >= JIN_LONG_JU_BAO_MAX_BUY_ITEM)
	{
		return 1;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}

	const RandActivityJinLongJuBaoConfig * config = static_cast<const RandActivityJinLongJuBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}

	const RandActivityJinLongJuBaBuyItemCfg * buy_cfg = config->GetRandActivityJinLongJuBaBuyItemCfg(ramgr, seq);
	if (buy_cfg == NULL)
	{
		return 1;
	}

	if (buy_cfg->limit_type == 1)
	{
		if (m_param.ra_buy_times[seq] >= buy_cfg->limit_buy_times)
		{
			return 2;
		}
	}

	if (buy_cfg->limit_type == 2)
	{
		if (m_param.ra_day_buy_times[seq] >= buy_cfg->limit_buy_times_day)
		{
			return 2;
		}
	}

	if (buy_cfg->rewards.empty())
	{
		return 1;
	}
	
	if (NULL != price)
	{
		*price = buy_cfg->need_chong_zhi_gold;
		return 0;
	}

	bool is_put_succ = false;
	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(buy_cfg->rewards))
	{
		if (m_role_module_mgr->GetKnapsack()->PutVec(buy_cfg->rewards, PUT_REASON_RA_JIN_LONG_JU_BAO_BUY))
		{
			is_put_succ = true;
		}
	}
	if (!is_put_succ)
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

	if (buy_cfg->limit_type == 1)
	{
		m_param.ra_buy_times[seq] += 1;
	}

	if (buy_cfg->limit_type == 2)
	{
		m_param.ra_day_buy_times[seq] += 1;
	}

	this->SendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

void RoleActivityJinLongJuBao::SendInfo(int reason)
{
	Protocol::SCRAJinLongJuBaoInfo protocol;

	protocol.reason = reason;
	SameArrayTypeCopy(protocol.ra_day_buy_times, m_param.ra_day_buy_times);
	SameArrayTypeCopy(protocol.ra_buy_times, m_param.ra_buy_times);
	protocol.draw_times = m_param.draw_times;
	protocol.times_reward_fetch_flag = m_param.times_reward_fetch_flag;
	SameArrayTypeCopy(protocol.item_list_select, m_param.item_list_select);
	SameArrayTypeCopy(protocol.item_list_rand, m_param.item_list_rand);
	SameArrayTypeCopy(protocol.record_list, m_param.record_list);

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

void RoleActivityJinLongJuBao::GiveUnFetchReward()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityJinLongJuBaoConfig * config = static_cast<const RandActivityJinLongJuBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RandActivityJinLongJuBaoTimesRewardSectionCfg * cfg = config->GetRandActivityJinLongJuBaoTimesRewardSectionCfg(ramgr, m_param.ra_begin_timestamp);
	if (!cfg)
	{
		return;
	}

	for (std::map<int, RandActivityJinLongJuBaoTimesRewardCfg> ::const_iterator iter = cfg->item_list.begin(); iter != cfg->item_list.end(); ++iter)
	{
		const RandActivityJinLongJuBaoTimesRewardCfg & curr = iter->second;

		if (curr.seq >= BitCount(m_param.times_reward_fetch_flag))
		{
			continue;
		}

		if (IsSetBit(m_param.times_reward_fetch_flag, curr.seq))
		{
			continue;
		}

		if (m_param.draw_times < curr.times)
		{
			continue;
		}

		SetBit(m_param.times_reward_fetch_flag, curr.seq);

		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, curr.rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_unfetch_reward_title, m_role_activity_type);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_unfetch_reward_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}

		gamelog::g_log_jin_long_ju_bao.printf(LL_INFO, "%s line:%d |  role[%d %s] seq:%d need_times:%d,times:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
			curr.seq, curr.times, m_param.draw_times);
	}

	this->SetRoleDataChange();
}

bool RoleActivityJinLongJuBao::IsAllSelect()
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

bool RoleActivityJinLongJuBao::IsAllGet()
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.item_list_select); ++i)
	{
		if (m_param.item_list_select[i].is_get == 0)
		{
			return false;
		}
	}

	return true;
}

bool RoleActivityJinLongJuBao::CheckNextTimes()
{
	if (!this->IsAllGet())
	{
		return false;
	}

	this->ResetReward();

	return true;
}

void RoleActivityJinLongJuBao::ResetReward()
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.item_list_select); ++i)
	{
		m_param.item_list_select[i].Reset();
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	const RandActivityJinLongJuBaoConfig * config = static_cast<const RandActivityJinLongJuBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (ramgr && config)
	{
		const RandActivityJinLongJuBaoRewardGroupCfg * cfg = config->GetRandActivityJinLongJuBaoRewardGroupCfg(ramgr, RAND_ACTIVITY_JIN_LONG_JU_BAO_REWARD_TYPE_RAND);
		if (!cfg)
		{
			return;
		}

		std::vector<RandActivityJinLongJuBaoRewardGroupItemCfg> rand_list = MapValuesAsVector(cfg->item_list);
		if (rand_list.empty())
		{
			return;
		}

		std::random_shuffle(rand_list.begin(), rand_list.end());
		for (int i = 0; i < ARRAY_LENGTH(m_param.item_list_rand); ++i)
		{
			RAJinLongJuBaoItem & curr = m_param.item_list_rand[i];
			curr.Reset();
			
			int r = i % (int)rand_list.size();
			RandActivityJinLongJuBaoRewardGroupItemCfg & rand_item = rand_list[r];
			curr.reward_group_id = rand_item.reward_group_id;
			curr.reward_seq = rand_item.reward_seq;
		}
	}

	this->SetRoleDataChange();
}

void RoleActivityJinLongJuBao::AddRecord(const std::vector<ItemConfigData> & rewards)
{
	if (rewards.empty())
	{
		return;
	}

	const ItemConfigData & reward = rewards[0];

	int index = m_param.record_index % ARRAY_LENGTH(m_param.record_list);
	RAJinLongJuBaoRecord record;
	record.item_id = reward.item_id;
	record.num = reward.num;
	record.time = (unsigned int)EngineAdapter::Instance().Time();
	record.times = m_param.draw_times;
	m_param.record_list[index] = record;

	m_param.record_index = index + 1;
}

BaoDiInfoItem * RoleActivityJinLongJuBao::GetBaoDiInfoItem(int bao_di_id)
{
	int index = bao_di_id - 1;
	if (index < 0 || index >= ARRAY_LENGTH(m_param.bao_di_info_list))
	{
		return NULL;
	}

	return &m_param.bao_di_info_list[index];
}

void RoleActivityJinLongJuBao::SetBaoDiInfoAfterDraw(int getreward_bao_di_id, std::set<int> add_times_list)
{
	for (std::set<int>::iterator iter = add_times_list.begin(); iter != add_times_list.end(); ++iter)
	{
		BaoDiInfoItem * tmp = this->GetBaoDiInfoItem(*iter);
		if (tmp && tmp->is_get == 0)
		{
			tmp->times += 1;
		}
	}

	BaoDiInfoItem * info = this->GetBaoDiInfoItem(getreward_bao_di_id);
	if (info)
	{
		info->is_get = 1;
	}
}

