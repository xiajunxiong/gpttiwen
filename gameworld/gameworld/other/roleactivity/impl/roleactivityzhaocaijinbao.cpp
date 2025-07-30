#include "roleactivityzhaocaijinbao.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityzhaocaijinbaoconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"
#include "servercommon/string/gameworldstr.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "servercommon/uniqueidgenerator.hpp"

RoleActivityZhaoCaiJinBao::RoleActivityZhaoCaiJinBao(int type) :RoleActivity(type)
{
	m_pool_rate = 0;
}

RoleActivityZhaoCaiJinBao::~RoleActivityZhaoCaiJinBao()
{
}

void RoleActivityZhaoCaiJinBao::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAZhaoCaiJinBaoParam));

	for (int i = 0; i < MAX_RA_ZHAOCAIJINBAO_RECORD_NUM; i++)
	{
		if(m_param.record_list[i].times == 0)
			continue;
		
		RAZhaoCaiJinBaoRecordInfo record;
		record.times = m_param.record_list[i].times;
		record.item_id = m_param.record_list[i].item_id;
		record.num = m_param.record_list[i].num;

		m_record_list.push_back(record);
	}
}

void RoleActivityZhaoCaiJinBao::GetInitParam(RoleActivityRoleData * p)
{
	for (int i = 0; i < (int)m_record_list.size() && i < MAX_RA_ZHAOCAIJINBAO_RECORD_NUM; i++)
	{
		if(m_record_list[i].times == 0)
			continue;

		m_param.record_list[i].times = m_record_list[i].times;
		m_param.record_list[i].item_id = m_record_list[i].item_id;
		m_param.record_list[i].num = m_record_list[i].num;
	}

	memcpy(p->base_data, &m_param, sizeof(RAZhaoCaiJinBaoParam));
}

void RoleActivityZhaoCaiJinBao::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO))
		return;

	this->RASendInfo();
}

void RoleActivityZhaoCaiJinBao::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO != rand_act_type)
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
	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_record_list.clear();

		this->SetRoleDataChange();
	}
}

void RoleActivityZhaoCaiJinBao::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO);

	switch (raor->opera_type)
	{
	case 0:	// 请求信息
		{
			this->RASendInfo();
		}
		break;
	case 1: // 抽奖
		{
			this->RADraw(raor->param_1);
		}
		break;
	case 2: // 元宝刷新奖池
		{
			this->RARefresh(true);
		}
		break;
	case 3: // 道具购买 非直购 param_1: 商城seq
		{
			this->RABuy(raor->param_1);
		}
		break;
	case 4: // 道具购买 直购 param_1: 商城seq
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
	case 5: // 领取充值奖励 param_1: 奖励seq
		{
			this->RAFetchReward(raor->param_1);
		}
		break;
	case 6:	// 抽奖记录下发
		{
			this->RASendRecordListInfo();
		}
		break;
	}
}

void RoleActivityZhaoCaiJinBao::Update(time_t now_second)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO))
	{
		return;
	}

	if (now_second >= m_param.next_refresh_timestamp)
	{
		this->RARefresh();
	}
}

void RoleActivityZhaoCaiJinBao::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO))
	{
		return;
	}

	const RandActivityZhaoCaiJinBaoConfig * config = static_cast<const RandActivityZhaoCaiJinBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO));
	if (NULL == config)
		return;

	const std::vector<RAZhaoCaiJinBaoBuyCfg> * buy_cfg = config->GetBuyMapCfg(ramgr);
	if (NULL == buy_cfg)
		return;

	for (int i = 0; i < (int)buy_cfg->size() && i < MAX_RA_ZHAOCAIJINBAO_BUY_NUM; i++)
	{
		if ((*buy_cfg)[i].limit_type == 2)
			m_param.buy_list[i] = 0;
	}

	const RAZhaoCaiJinBaoChongZhiMap * chongzhi_map = config->GetTopUpMapCfg(ramgr);
	if (NULL == chongzhi_map)
		return;

	for (int i = 0; i < (int)chongzhi_map->chongzhi_cfg.size() && i < MAX_RA_ZHAOCAIJINBAO_CHONGZHI_NUM; i++)
	{
		if (chongzhi_map->chongzhi_cfg[i].get_type == 2)	// 每日领取
		{
			m_param.chongzhi_list[i] = 0;
			m_param.chongzhi_time[i] = 0;
		}
	}

	m_param.day_chongzhi = 0;

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityZhaoCaiJinBao::OnAddChongZhi(long long chongzhi)
{
	if (chongzhi <= 0)
		return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO))
	{
		return;
	}

	const RandActivityZhaoCaiJinBaoConfig * config = static_cast<const RandActivityZhaoCaiJinBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO));
	if (NULL == config)
		return;

	const RAZhaoCaiJinBaoChongZhiMap * map_cfg = config->GetTopUpMapCfg(ramgr);
	if (NULL == map_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (0 == map_cfg->top_up_type)
	{
		int seq = 0;
		for ( ; seq < (int)map_cfg->chongzhi_cfg.size() && seq < MAX_RA_ZHAOCAIJINBAO_CHONGZHI_NUM; seq++)
		{
			if(chongzhi == map_cfg->chongzhi_cfg[seq].top_up_num)
				break;
		}

		if (m_param.chongzhi_time[seq] >= map_cfg->chongzhi_cfg[seq].get_times)
			return;

		m_param.chongzhi_time[seq] += 1;
	}
	else
	{
		m_param.history_chongzhi += chongzhi;
		m_param.day_chongzhi += chongzhi;
	}

	this->RASendInfo();
	this->SetRoleDataChange();
}

int RoleActivityZhaoCaiJinBao::OnBuyItemCheck(int shop_index, long long ARG_OUT & unique_key)
{
	if (shop_index < 0 || shop_index >= MAX_RA_ZHAOCAIJINBAO_BUY_NUM)
		return -__LINE__;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityZhaoCaiJinBaoConfig * config = static_cast<const RandActivityZhaoCaiJinBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RAZhaoCaiJinBaoBuyCfg * cfg = config->GetBuyCfg(ramgr, shop_index);
	if (NULL == cfg)
	{
		return -__LINE__;
	}

	if (cfg->price_type != -1)
	{
		return -__LINE__;
	}

	if (m_param.buy_list[shop_index] >= cfg->buy_times)
	{
		return -__LINE__;
	}

	unique_key = 0;
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
	buy_item->need_gold = cfg->buy_money;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = shop_index;
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_role_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_role_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityZhaoCaiJinBao::OnCmdBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_role_activity.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityZhaoCaiJinBaoConfig * config = static_cast<const RandActivityZhaoCaiJinBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RAZhaoCaiJinBaoBuyCfg * cfg = config->GetBuyCfg(ramgr, buy_item->param1);
	if (NULL == cfg)
	{
		return -__LINE__;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->reward_item.size(), &cfg->reward_item[0], false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item[0], (short)cfg->reward_item.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)cfg->reward_item.size(), &cfg->reward_item[0], PUT_REASON_RA_ZHAOCAIJINBAO);
	}

	m_param.buy_list[buy_item->param1] += 1;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] shop_index[%d], shop_time[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO,
		buy_item->param1, m_param.buy_list[buy_item->param1]);

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_role_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	return 0;

}

int RoleActivityZhaoCaiJinBao::OnBuyGiftItem(int seq, ARG_OUT int * price)
{
	if (seq < 0 || seq >= MAX_RA_HORCRUX_TIANCHENG_SHOP_INDEX)
	{
		return 1;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return 1;
	}

	const RandActivityZhaoCaiJinBaoConfig * config = static_cast<const RandActivityZhaoCaiJinBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO));
	if (NULL == config)
	{
		return 1;
	}

	const RAZhaoCaiJinBaoBuyCfg * cfg = config->GetBuyCfg(ramgr, seq);
	if (NULL == cfg)
	{
		return 1;
	}
	if (NULL != price)
	{
		*price = cfg->buy_money;
		return 0;
	}

	if (-1 != cfg->price_type)
	{
		return 1;
	}
	if (m_param.buy_list[seq] >= cfg->buy_times)
	{
		return 2;
	}
	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->reward_item.size(), &cfg->reward_item[0], false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item[0], (short)cfg->reward_item.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)cfg->reward_item.size(), &cfg->reward_item[0], PUT_REASON_RA_ZHAOCAIJINBAO);
	}

	m_param.buy_list[seq] += 1;
	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] shop_index[%d], shop_time[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		seq, m_param.buy_list[seq]);
	
	return 0;
}

void RoleActivityZhaoCaiJinBao::RASendInfo()
{
	Protocol::SCRAZhaoCaiJinBaoRoleInfo info;
	info.next_refresh_timestamp = m_param.next_refresh_timestamp;

	info.history_chongzhi = m_param.history_chongzhi;
	info.day_chongzhi = m_param.day_chongzhi;
	memcpy(info.chongzhi_time, m_param.chongzhi_time, sizeof(info.chongzhi_time));

	memcpy(info.pool, m_param.pool, sizeof(info.pool));
	memcpy(info.buy_list, m_param.buy_list, sizeof(info.buy_list));
	memcpy(info.chongzhi_list, m_param.chongzhi_list, sizeof(info.chongzhi_list));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityZhaoCaiJinBao::RADraw(int times)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
		return;

	const RandActivityZhaoCaiJinBaoConfig * config = static_cast<const RandActivityZhaoCaiJinBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO));
	if (NULL == config)
		return;

	if (!m_role_module_mgr->GetKnapsack()->HasItem(config->GetOtherCfg().item_id, config->GetOtherCfg().item_num * times, false))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	const RAZhaoCaiJinBaoGroupMap * cfg = config->GetGroupCfg(ramgr);
	if (NULL == cfg)
		return;

	std::vector<const RAZhaoCaiJinBaoGroupCfg *> bao_di_cfg_list;
	std::set<int> bao_di_id_list;

	m_pool_rate = 0;
	m_pool_map.clear();
	for (int i = 0; i < MAX_RA_ZHAOCAIJINBAO_REFRESH_NUM; i++)
	{
		int seq = m_param.pool[i];
		if (seq < 0 || seq >= (int)cfg->group_cfg.size())
			continue;

		m_pool_rate += cfg->group_cfg[seq].extract_rate;
		m_pool_map[m_pool_rate] = seq;

		if (cfg->group_cfg[seq].bao_di_times > 0)
		{
			bao_di_cfg_list.push_back(&cfg->group_cfg[seq]);
			bao_di_id_list.insert(cfg->group_cfg[seq].bao_di_id);
		}
	}
	
	Protocol::SCRAZhaoCaiJinBaoDrawResult info;
	info.count = 0;

	for (int i = 0; i < times && times < 50; i++)
	{
		int rate = RandomNum(m_pool_rate);
		std::map<int, int>::iterator it = m_pool_map.upper_bound(rate);
		if (it == m_pool_map.end())
			return;

		if (it->second < 0 || it->second >= (int)cfg->group_cfg.size())
			return;

		const RAZhaoCaiJinBaoGroupCfg * reward_cfg = &cfg->group_cfg[it->second];
		if (NULL == reward_cfg)
			return;

		for (int bao_di_index = 0; bao_di_index < (int)bao_di_cfg_list.size(); ++bao_di_index)
		{
			const RAZhaoCaiJinBaoGroupCfg * curr = bao_di_cfg_list[bao_di_index];
			if (!curr)
			{
				continue;
			}

			if (curr->bao_di_times > 0)
			{
				BaoDiInfoItem * bao_di_info_item = this->GetBaoDiInfoItem(curr->bao_di_id);
				if (bao_di_info_item)
				{
					if (bao_di_info_item->is_get != 0)
					{
						continue;
					}

					if (bao_di_info_item->times + 1 >= curr->bao_di_times)
					{
						reward_cfg = curr;


						gamelog::g_log_role_activity.printf(LL_INFO, "%s %d | role_id:[%d %s] get_bao_di_reward bao_di_id:%d reward_seq:%d need_times:%d draw_times:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRoleName(),
							curr->bao_di_id, curr->seq, curr->bao_di_times, bao_di_info_item->times + 1);


						break;
					}
				}
			}
		}

		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(config->GetOtherCfg().item_id, config->GetOtherCfg().item_num, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
			return;
		}

		RAZhaoCaiJinBaoRecordInfo record;
		record.times = (unsigned int)EngineAdapter::Instance().Time();

		switch (reward_cfg->reward_type)
		{
		case 0://道具奖励
			{
				if (!m_role_module_mgr->GetKnapsack()->CheckForPut2(reward_cfg->reward_item, false))
				{
					m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward_item, 1, SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO);
				}
				else
				{
					m_role_module_mgr->GetKnapsack()->Put(reward_cfg->reward_item, PUT_REASON_RA_ZHAOCAIJINBAO_DRAW);
				}

				gamelog::g_log_role_activity.printf(LL_INFO, "RoleActivityZhaoCaiJinBao::Draw role[%d,%s] seq[%d] item[%d, %d]",
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), reward_cfg->seq, reward_cfg->reward_item.item_id, reward_cfg->reward_item.num);

				record.item_id = reward_cfg->reward_item.item_id;
				record.num = reward_cfg->reward_item.num;
			}
			break;
		case 1://元宝奖励
			{
				m_role_module_mgr->GetMoney()->AddGold(reward_cfg->gold_num, __FUNCTION__, false, false, false);

				gamelog::g_log_role_activity.printf(LL_INFO, "RandActivityHorcruxTianCheng::Draw role[%d,%s] gold[%d]",
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), reward_cfg->gold_num);

				record.item_id = GOLD_ITEM_ID;
				record.num = reward_cfg->gold_num;
			}
			break;
		}

		if (reward_cfg->is_hearsay && !CrossConfig::Instance().IsHiddenServer())
		{
			std::string broadcast_str = STRING_SPRINTF(gamestring::g_randactivity_zhaocaijinbao_broadcast, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
				reward_cfg->reward_item.item_id);

			// 传闻
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), "%s", broadcast_str.c_str());
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_ROLLING_NOTICE));
			}
		}
		{
			m_record_list.push_front(record);
			if (m_record_list.size() > MAX_RA_ZHAOCAIJINBAO_RECORD_NUM)
			{
				m_record_list.pop_back();
			}
		}

		{
			info.seq_list[info.count] = reward_cfg->seq;
			info.count++;
		}

		this->SetBaoDiInfoAfterDraw(reward_cfg->bao_di_id,bao_di_id_list);
	}

	int length = sizeof(info) - sizeof(info.seq_list) + info.count * sizeof(info.seq_list[0]);
	m_role_module_mgr->NetSend(&info, length);

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityZhaoCaiJinBao::RARefresh(bool is_client)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
		return;

	const RandActivityZhaoCaiJinBaoConfig * config = static_cast<const RandActivityZhaoCaiJinBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO));
	if (NULL == config)
		return;

	if (is_client && !m_role_module_mgr->GetMoney()->GoldMoreThan(config->GetOtherCfg().refresh_gold))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}

	const RAZhaoCaiJinBaoGroupMap * cfg = config->GetGroupCfg(ramgr);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (is_client && !m_role_module_mgr->GetMoney()->UseGold(config->GetOtherCfg().refresh_gold, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}

	std::vector<RAZhaoCaiJinBaoGroupCfg> group(cfg->group_cfg.begin(), cfg->group_cfg.end());
	short new_pool[MAX_RA_ZHAOCAIJINBAO_REFRESH_NUM] = { 0 };

	int MAX_RATE = cfg->MAX_RATE;
	m_pool_rate = 0;
	m_pool_map.clear();

	for (int i = 0; i < MAX_RA_ZHAOCAIJINBAO_REFRESH_NUM; i++)
	{
		int rand_num = RandomNum(MAX_RATE);
		int rate_num = 0;
		for (int n = 0; n < (int)group.size(); n++)
		{
			rate_num += group[n].select_rate;
			if (rand_num < rate_num)
			{
				MAX_RATE -= group[n].select_rate;
				new_pool[i] = group[n].seq;

				m_pool_rate += group[n].extract_rate;
				m_pool_map[m_pool_rate] = group[n].seq;

				group.erase(group.begin() + n);
				break;
			}
		}
	}

	memcpy(m_param.pool, new_pool, sizeof(m_param.pool));
	m_param.next_refresh_timestamp = EngineAdapter::Instance().Time() + 12 * 60 * 60;

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityZhaoCaiJinBao::RABuy(int seq)
{
	if (seq < 0 || seq >= MAX_RA_ZHAOCAIJINBAO_BUY_NUM)
		return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
		return;

	const RandActivityZhaoCaiJinBaoConfig * config = static_cast<const RandActivityZhaoCaiJinBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO));
	if (NULL == config)
		return;

	const RAZhaoCaiJinBaoBuyCfg * cfg = config->GetBuyCfg(ramgr, seq);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	// 直购不用这个
	if (cfg->price_type == -1)
		return;

	if (m_param.buy_list[seq] >= cfg->buy_times)
		return;

	if (cfg->price_type == 0)
	{
		if (!m_role_module_mgr->GetMoney()->UseGold(cfg->buy_money, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
	}
	else
	{
		if (!m_role_module_mgr->GetMoney()->UseOtherCoin(cfg->buy_money, cfg->price_type, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_OTHER_MONEY_NOT_ENOUGH);
			return;
		}
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList2(cfg->reward_item, false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item[0], (int)cfg->reward_item.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)cfg->reward_item.size(), &cfg->reward_item[0], PUT_REASON_RA_ZHAOCAIJINBAO);
	}

	m_param.buy_list[seq] += 1;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	// log
	gamelog::g_log_role_activity.printf(LL_INFO, "RoleActivityZhaoCaiJinBao::RABuy role[%d,%s] seq[%d] buy_time[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), seq, m_param.buy_list[seq]);
}

void RoleActivityZhaoCaiJinBao::RAFetchReward(int seq)
{
	if (seq < 0 || seq >= MAX_RA_ZHAOCAIJINBAO_CHONGZHI_NUM)
		return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
		return;

	const RandActivityZhaoCaiJinBaoConfig * config = static_cast<const RandActivityZhaoCaiJinBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO));
	if (NULL == config)
		return;

	const RAZhaoCaiJinBaoChongZhiCfg * cfg = config->GetTopUpCfg(ramgr, seq);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (0 == cfg->top_up_type)
	{
		if (m_param.chongzhi_list[seq] >= m_param.chongzhi_time[seq])
		{
			m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
			return;
		}
	}
	else
	{
		if (m_param.chongzhi_list[seq] >= cfg->get_times)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
			return;
		}

		if (cfg->get_type == 2)	
		{ // 每日累充
			if (m_param.day_chongzhi < cfg->top_up_num)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_CUMULATIVE_RECHARGE_ACC_PRICE_ERR);
				return;
			}
		}
		else
		{ // 活动累充
			if (m_param.history_chongzhi < cfg->top_up_num)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_CUMULATIVE_RECHARGE_ACC_PRICE_ERR);
				return;
			}
		}
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList2(cfg->reward_item, false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item[0], (int)cfg->reward_item.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)cfg->reward_item.size(), &cfg->reward_item[0], PUT_REASON_RA_ZHAOCAIJINBAO);
	}

	m_param.chongzhi_list[seq] += 1;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	// log
	gamelog::g_log_role_activity.printf(LL_INFO, "RoleActivityZhaoCaiJinBao::RAFetchReward role[%d,%s] seq[%d] fetch_time[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), seq, m_param.chongzhi_list[seq]);
}

void RoleActivityZhaoCaiJinBao::RASendRecordListInfo()
{
	Protocol::SCRAZhaoCaiJinBaoRecordListInfo info;
	info.count = 0;
	for (int i = 0; i < (int)m_record_list.size() && i < MAX_RA_ZHAOCAIJINBAO_RECORD_NUM; i++)
	{
		if (m_record_list[i].times == 0)
			continue;

		info.record_list[info.count].times = m_record_list[i].times;
		info.record_list[info.count].item_id = m_record_list[i].item_id;
		info.record_list[info.count].num = m_record_list[i].num;
		info.count++;
	}

	int len = sizeof(info) - sizeof(info.record_list) + info.count * sizeof(info.record_list[0]);
	m_role_module_mgr->NetSend(&info, len);
}

BaoDiInfoItem * RoleActivityZhaoCaiJinBao::GetBaoDiInfoItem(int bao_di_id)
{
	int index = bao_di_id - 1;
	if (index < 0 || index >= ARRAY_LENGTH(m_param.bao_di_info_list))
	{
		return NULL;
	}

	return &m_param.bao_di_info_list[index];
}

void RoleActivityZhaoCaiJinBao::SetBaoDiInfoAfterDraw(int getreward_bao_di_id, std::set<int> add_times_list)
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

	this->SetRoleDataChange();
}
