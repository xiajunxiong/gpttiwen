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
#include "protocol/randactivity/msgrazhuiyueshangdian.hpp"
#include "config/randactivityconfig/impl/randactivityzhuiyueshangdianconfig.hpp"
#include "roleactivityzhuiyueshangdian.hpp"

RoleActivityZhuiYueShangDian::RoleActivityZhuiYueShangDian(int type) :RoleActivity(type)
{
}

RoleActivityZhuiYueShangDian::~RoleActivityZhuiYueShangDian()
{
}

void RoleActivityZhuiYueShangDian::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityZhuiYueShangDian::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityZhuiYueShangDian::OnLoginSendInfo()
{
	this->SendInfo();
	this->SendRecordInfo();
}

void RoleActivityZhuiYueShangDian::CheckInitRandActivity(int rand_act_type, bool is_force)
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

		this->SetRoleDataChange();
	}
}

void RoleActivityZhuiYueShangDian::OnRAClose()
{
	this->GiveUnFetchReward();
}

void RoleActivityZhuiYueShangDian::OnLoginCheck()
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

void RoleActivityZhuiYueShangDian::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.ra_day_buy_times); ++i)
	{
		m_param.ra_day_buy_times[i] = 0;
	}

	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityZhuiYueShangDian::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
		break;
		case 3:	//设置自选  p1 格子index[0,6),p2 reward_seq
		{
			this->SetReward(raor->param_1, raor->param_2);
		}
		break;
		case 4:	//抽奖 p1 次数
		{
			this->DrwardReward(raor->param_1);
		}
		break;
	}
}

int RoleActivityZhuiYueShangDian::SetReward(int index, int reward_seq)
{
	if (index < 0 || index >= ZHUI_YUE_SHANG_DIAN_SELECT_NUM_SELECT)
	{
		ret_line;
	}

	if (this->IsAllSelect())
	{
		ret_line;
	}

	//检查重复选择
	for (int i = 0; i < ARRAY_LENGTH(m_param.item_list_select); ++i)
	{
		if (m_param.item_list_select[i].is_select && m_param.item_list_select[i].reward_seq == reward_seq)
		{
			ret_line;
		}
	}


	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		ret_line;
	}

	const RandActivityZhuiYueShangDianConfig * config = static_cast<const RandActivityZhuiYueShangDianConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		ret_line;
	}

	const RandActivityZhuiYueShangDianRewardGroupItemCfg * cfg = config->GetRandActivityZhuiYueShangDianRewardGroupItemCfg(ramgr, reward_seq);
	if (!cfg)
	{
		ret_line;
	}

	RAZhuiYueShangDianItem & select_item = m_param.item_list_select[index];
	select_item.is_select = 1;
	select_item.reward_group_id = cfg->reward_group_id;
	select_item.reward_seq = cfg->reward_seq;
	this->SetRoleDataChange();

	return 0;
}

struct RoleActivityZhuiYueShangDianDrawItem
{
	RoleActivityZhuiYueShangDianDrawItem()
	{
		rate = 0;
		index = 0;
		cfg = NULL;
	}

	int rate;
	int index;
	const RandActivityZhuiYueShangDianRewardGroupItemCfg * cfg;
};

int RoleActivityZhuiYueShangDian::DrwardReward(int times)
{
	Protocol::SCRAZhuiYueShangDianDrawRet draw_ret;
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

	return 0;
}


int RoleActivityZhuiYueShangDian::DrawHelper(int & item)
{
	if (!this->IsAllSelect())
	{
		ret_line;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (!ramgr)
	{
		ret_line;
	}

	const RandActivityZhuiYueShangDianConfig * config = static_cast<const RandActivityZhuiYueShangDianConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (!config)
	{
		ret_line;
	}

	const RandActivityZhuiYueShangDianCfg * draw_cfg = config->GetRandActivityZhuiYueShangDianCfg(ramgr);
	if (!draw_cfg)
	{
		ret_line;
	}

	std::vector<RoleActivityZhuiYueShangDianDrawItem> bao_di_cfg_list;
	std::set<int> bao_di_id_list;

	int rand_count = 0;
	std::vector<RoleActivityZhuiYueShangDianDrawItem> draw_list;
	const RandActivityZhuiYueShangDianRewardGroupItemCfg * special_reward_cfg = config->GetRandActivityZhuiYueShangDianSpecialRewardItemCfg(ramgr);
	if (!special_reward_cfg)
	{
		ret_line;
	}

	{
		RoleActivityZhuiYueShangDianDrawItem tmp;
		tmp.rate = special_reward_cfg->rate;
		tmp.index = -1;
		tmp.cfg = special_reward_cfg;
		draw_list.push_back(tmp);

		if (special_reward_cfg->bao_di_times > 0)
		{
			bao_di_cfg_list.push_back(tmp);
			bao_di_id_list.insert(special_reward_cfg->bao_di_id);
		}
	}

	for (int i = 0; i < ARRAY_LENGTH(m_param.item_list_select); ++i)
	{
		RAZhuiYueShangDianItem & curr = m_param.item_list_select[i];

		const RandActivityZhuiYueShangDianRewardGroupItemCfg * cfg = config->GetRandActivityZhuiYueShangDianRewardGroupItemCfg(ramgr, curr.reward_seq);
		if (!cfg)
		{
			ret_line;
		}

		rand_count += cfg->rate;

		RoleActivityZhuiYueShangDianDrawItem tmp;
		tmp.rate = cfg->rate;
		tmp.index = i;
		tmp.cfg = cfg;
		draw_list.push_back(tmp);

		if (cfg->bao_di_times > 0)
		{
			bao_di_cfg_list.push_back(tmp);
			bao_di_id_list.insert(cfg->bao_di_id);
		}
	}

	const RoleActivityZhuiYueShangDianDrawItem * get_draw_item = NULL;
	int r = RandomNum(rand_count);
	for (int i = 0; i < (int)draw_list.size(); ++i)
	{
		const RoleActivityZhuiYueShangDianDrawItem & curr = draw_list[i];
		if (r < curr.rate)
		{
			get_draw_item = &curr;
			break;
		}

		r -= curr.rate;
	}


	for (int bao_di_index = 0; bao_di_index < (int)bao_di_cfg_list.size(); ++bao_di_index)
	{
		const RoleActivityZhuiYueShangDianDrawItem & curr = bao_di_cfg_list[bao_di_index];

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

				gamelog::g_log_zhui_yue_shang_dian.printf(LL_INFO, "%s %d | role_id:[%d %s] get_bao_di_reward bao_di_id:%d reward_groupid:%d reward_seq:%d need_times:%d draw_times:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRoleName(),
					curr.cfg->bao_di_id, curr.cfg->reward_group_id, curr.cfg->reward_seq, curr.cfg->bao_di_times, bao_di_info_item->times + 1);

				break;
			}
		}
	}

	if (!get_draw_item)
	{
		ret_line;
	}

	const RandActivityZhuiYueShangDianOtherCfg & other_cfg = config->GetOtherCfg();
	if (!m_role_module_mgr->GetKnapsack()->HasItem(draw_cfg->draw_need_item_id, draw_cfg->draw_need_item_num))
	{
		ret_line;
	}

	const std::vector<ItemConfigData> & rewards = get_draw_item->cfg->rewards;
	if (m_role_module_mgr->GetKnapsack()->CheckForPutList2(rewards))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_RA_ZHUI_YUE_SHANG_DIAN_DRAW);
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

	m_role_module_mgr->GetKnapsack()->ConsumeItem(draw_cfg->draw_need_item_id, draw_cfg->draw_need_item_num, __FUNCTION__);
	this->AddRecord(*get_draw_item);
	this->SetBaoDiInfoAfterDraw(get_draw_item->cfg->bao_di_id, bao_di_id_list);

	this->SetRoleDataChange();

	item = get_draw_item->index;

	if (get_draw_item->cfg->is_broadcast && !rewards.empty())
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_randactivity_zhui_yue_shang_dian_get_item_broadcast,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),rewards[0].item_id);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}
	}

	gamelog::g_log_zhui_yue_shang_dian.printf(LL_INFO, "%s line:%d | role[%d %s] group_id:%d reward_seq:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		get_draw_item->cfg->reward_group_id, get_draw_item->cfg->reward_seq);

	return 0;
}


int RoleActivityZhuiYueShangDian::OnBuyItemCheck(int seq, long long ARG_OUT & unique_key)
{
	unique_key = 0;

	if (seq < 0 || seq >= ZHUI_YUE_SHANG_DIAN_MAX_BUY_ITEM)
	{
		ret_line;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		ret_line;
	}

	const RandActivityZhuiYueShangDianConfig * config = static_cast<const RandActivityZhuiYueShangDianConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		ret_line;
	}

	const RandActivityZhuiYueShangDianBuyItemCfg * buy_cfg = config->GetRandActivityZhuiYueShangDianBuyItemCfg(ramgr, seq);
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
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_zhui_yue_shang_dian, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_zhui_yue_shang_dian.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityZhuiYueShangDian::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		ret_line;
	}

	gamelog::g_log_zhui_yue_shang_dian.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
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
		m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_RA_ZHUI_YUE_SHANG_DIAN_BUY);
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
		if (0 <= seq && seq < ZHUI_YUE_SHANG_DIAN_MAX_BUY_ITEM)
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

		this->SendInfo();
		this->SetRoleDataChange();
	}

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_zhui_yue_shang_dian, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

int RoleActivityZhuiYueShangDian::OnBuyItemByMoneyType(int seq)
{
	if (seq < 0 || seq >= ZHUI_YUE_SHANG_DIAN_MAX_BUY_ITEM)
	{
		ret_line;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		ret_line;
	}

	const RandActivityZhuiYueShangDianConfig * config = static_cast<const RandActivityZhuiYueShangDianConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		ret_line;
	}

	const RandActivityZhuiYueShangDianBuyItemCfg * buy_cfg = config->GetRandActivityZhuiYueShangDianBuyItemCfg(ramgr, seq);
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

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(buy_cfg->rewards))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HAS_NO_SPACING);
		ret_line;
	}

	if (!m_role_module_mgr->GetMoney()->UseMoney(buy_cfg->money_type, buy_cfg->money_value, __FUNCTION__))
	{
		ret_line;
	}

	m_role_module_mgr->GetKnapsack()->PutVec(buy_cfg->rewards, PUT_REASON_RA_ZHUI_YUE_SHANG_DIAN_BUY);

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		m_param.ra_buy_times[seq] += 1;
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		m_param.ra_day_buy_times[seq] += 1;
	}

	this->SetRoleDataChange();
	this->SendInfo();

	gamelog::g_log_zhui_yue_shang_dian.printf(LL_INFO, "%s line:%d | role[%d %s] buy reward_group_seq:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		buy_cfg->seq);

	return 0;
}

int RoleActivityZhuiYueShangDian::OnBuyGiftItem(int seq, ARG_OUT int * price)
{
	if (seq < 0 || seq >= ZHUI_YUE_SHANG_DIAN_MAX_BUY_ITEM)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	const RandActivityZhuiYueShangDianConfig * config = static_cast<const RandActivityZhuiYueShangDianConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	const RandActivityZhuiYueShangDianBuyItemCfg * buy_cfg = config->GetRandActivityZhuiYueShangDianBuyItemCfg(ramgr, seq);
	if (buy_cfg == NULL)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	if (!buy_cfg->is_need_chong_zhi)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	if (NULL != price)
	{
		*price = buy_cfg->need_chong_zhi_gold;
		return 0;
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

	const std::vector<ItemConfigData> & rewards = buy_cfg->rewards;

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(rewards))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_RA_ZHUI_YUE_SHANG_DIAN_BUY);
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


	if (0 <= seq && seq < ZHUI_YUE_SHANG_DIAN_MAX_BUY_ITEM)
	{
		if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
		{
			m_param.ra_buy_times[seq] += 1;
		}

		if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
		{
			m_param.ra_day_buy_times[seq] += 1;
		}
	}

	this->SendInfo();
	this->SetRoleDataChange();

	gamelog::g_log_zhui_yue_shang_dian.printf(LL_INFO, "%s line:%d | role[%d %s] buy reward_group_seq:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		buy_cfg->seq);

	return 0;
}

void RoleActivityZhuiYueShangDian::SendInfo()
{
	Protocol::SCRAZhuiYueShangDianInfo protocol;

	SameArrayTypeCopy(protocol.ra_day_buy_times, m_param.ra_day_buy_times);
	SameArrayTypeCopy(protocol.ra_buy_times, m_param.ra_buy_times);
	SameArrayTypeCopy(protocol.item_list_select, m_param.item_list_select);
	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

template<typename T, typename T2, size_t N>
unsigned int GetProtocolArraySendLenght(T & dst, size_t count, T2(&)[N])
{
	size_t max_num = std::min(count, N);
	size_t cal_len = sizeof(T) - (N - count) * sizeof(T2);
	return std::min(sizeof(T), cal_len);
}

void RoleActivityZhuiYueShangDian::SendRecordInfo()
{
	Protocol::SCRAZhuiYueShangDianDrawRecrod protocol;
	protocol.count = 0;
	for (int i = 0; i < ZHUI_YUE_SHANG_DIAN_RECORD_NUM && i < m_param.add_record_times; ++i)
	{
		protocol.record_list[protocol.count++] = m_param.record_list[i];
	}

	int send_len = GetProtocolArraySendLenght(protocol, protocol.count, protocol.record_list);
	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

void RoleActivityZhuiYueShangDian::GiveUnFetchReward()
{
	//这个活动没要求
}

bool RoleActivityZhuiYueShangDian::IsAllSelect()
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

void RoleActivityZhuiYueShangDian::AddRecord(const RoleActivityZhuiYueShangDianDrawItem & draw_item)
{
	int index = m_param.add_record_times++ % ARRAY_LENGTH(m_param.record_list);
	RAZhuiYueShangDianDrawRecord & record = m_param.record_list[index];
	record.time = (unsigned int)EngineAdapter::Instance().Time();
	record.sort_param = m_param.add_record_times;
	record.reward_index = draw_item.index;
}

BaoDiInfoItem * RoleActivityZhuiYueShangDian::GetBaoDiInfoItem(int bao_di_id)
{
	int index = bao_di_id - 1;
	if (index < 0 || index >= ARRAY_LENGTH(m_param.bao_di_info_list))
	{
		return NULL;
	}

	return &m_param.bao_di_info_list[index];
}

void RoleActivityZhuiYueShangDian::SetBaoDiInfoAfterDraw(int getreward_bao_di_id, std::set<int> add_times_list)
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
