#include "roleactivityjixinggaozhao.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityjixinggaozhaoconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityimpl/randactivityjixinggaozhao.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "servercommon/uniqueidgenerator.hpp"


RoleActivityJiXingGaoZhao::RoleActivityJiXingGaoZhao(int type) :RoleActivity(type)
{
}

RoleActivityJiXingGaoZhao::~RoleActivityJiXingGaoZhao()
{
}

void RoleActivityJiXingGaoZhao::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAJiXingGaoZhaoParam));
}

void RoleActivityJiXingGaoZhao::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAJiXingGaoZhaoParam));
}

void RoleActivityJiXingGaoZhao::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_JIXING_GAOZHAO))
		return;

	this->RASendInfo();
}

void RoleActivityJiXingGaoZhao::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_JIXING_GAOZHAO != rand_act_type)
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

		this->SetRoleDataChange();
	}
}

void RoleActivityJiXingGaoZhao::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_JIXING_GAOZHAO))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_JIXING_GAOZHAO);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();
		}
		break;
	case 1:
		{
			RandActivityJiXingGaoZhao * act = (RandActivityJiXingGaoZhao *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_JIXING_GAOZHAO);
			if (NULL == act) return;

			if (act->Draw(m_role_module_mgr->GetRole())) m_param.draw_times += 1;
		}
		break;
	case 2:
		{
			this->RAShop(raor->param_1);
		}
		break;
	case 3:
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
	case 4:
		{
			this->RAFetchDrawReward(raor->param_1);
		}
	}
}

void RoleActivityJiXingGaoZhao::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_JIXING_GAOZHAO))
	{
		return;
	}

	const RandActivityJiXingGaoZhaoConfig * config = static_cast<const RandActivityJiXingGaoZhaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_JIXING_GAOZHAO));
	if (NULL == config)
		return;

	const std::vector<RAJiXingGaoZhaoShopCfg> * shop_map_cfg = config->GetShopMapCfg(ramgr);
	if (NULL == shop_map_cfg)
		return;

	for (int i = 0; i < MAX_RA_JIXING_GAOZHAO_SHOP_INDEX && i < (int)shop_map_cfg->size(); i++)
	{
		if ((*shop_map_cfg)[i].limit_type == 2)
			m_param.shop_list[i] = 0;
	}

	this->RASendInfo();
}

int RoleActivityJiXingGaoZhao::OnBuyGiftItem(int shop_index, ARG_OUT int * price)
{
	if (shop_index < 0 || shop_index >= MAX_RA_JIXING_GAOZHAO_SHOP_INDEX)
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

	const RandActivityJiXingGaoZhaoConfig * config = static_cast<const RandActivityJiXingGaoZhaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_JIXING_GAOZHAO));
	if (NULL == config)
	{
		return 1;
	}

	const RAJiXingGaoZhaoShopCfg * cfg = config->GetShopCfg(ramgr, shop_index);
	if (NULL == cfg)
	{
		return 1;
	}

	if (cfg->price_type != 3)
	{
		return 1;
	}
	if (NULL != price)
	{
		*price = cfg->buy_money;
		return 0;
	}

	if (m_param.shop_list[shop_index] >= cfg->limit_times)
	{
		return 2;
	}
	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList2(cfg->reward_item, false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item[0], (short)cfg->reward_item.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutVec(cfg->reward_item, PUT_REASON_RA_JIXING_GAOZHAO);
	}

	m_param.shop_list[shop_index] += 1;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] shop_index[%d], shop_time[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		shop_index, m_param.shop_list[shop_index]);

	return 0;
}

int RoleActivityJiXingGaoZhao::OnBuyItemCheck(int shop_index, long long ARG_OUT & unique_key)
{
	if (shop_index < 0 || shop_index >= MAX_RA_JIXING_GAOZHAO_SHOP_INDEX)
		return -__LINE__;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityJiXingGaoZhaoConfig * config = static_cast<const RandActivityJiXingGaoZhaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_JIXING_GAOZHAO));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RAJiXingGaoZhaoShopCfg * cfg = config->GetShopCfg(ramgr, shop_index);
	if (NULL == cfg)
	{
		return -__LINE__;
	}

	if (cfg->price_type != 3)
	{
		return -__LINE__;
	}

	if (m_param.shop_list[shop_index] >= cfg->limit_times)
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

int RoleActivityJiXingGaoZhao::OnCmdBuyItem(int chong_zhi_gold, long long unique_id)
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

	const RandActivityJiXingGaoZhaoConfig * config = static_cast<const RandActivityJiXingGaoZhaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_JIXING_GAOZHAO));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RAJiXingGaoZhaoShopCfg * cfg = config->GetShopCfg(ramgr, buy_item->param1);
	if (NULL == cfg)
	{
		return -__LINE__;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->reward_item.size(), &cfg->reward_item[0], false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item[0], (short)cfg->reward_item.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_JIXING_GAOZHAO);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutVec(cfg->reward_item, PUT_REASON_RA_JIXING_GAOZHAO);
	}

	m_param.shop_list[buy_item->param1] += 1;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] shop_index[%d], shop_time[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_JIXING_GAOZHAO,
		buy_item->param1, m_param.shop_list[buy_item->param1]);

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_role_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	return 0;
}

void RoleActivityJiXingGaoZhao::RASendInfo()
{
	Protocol::SCRAJiXingGaoZhaoRoleInfo info;
	info.draw_times = m_param.draw_times;
	info.draw_reward_fetch_mark = m_param.draw_reward_fetch_mark;
	memcpy(info.shop_list, m_param.shop_list, sizeof(info.shop_list));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityJiXingGaoZhao::RAShop(int shop_index)
{
	if (shop_index < 0 || shop_index >= MAX_RA_JIXING_GAOZHAO_SHOP_INDEX)
		return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityJiXingGaoZhaoConfig * config = static_cast<const RandActivityJiXingGaoZhaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_JIXING_GAOZHAO));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAJiXingGaoZhaoShopCfg * cfg = config->GetShopCfg(ramgr, shop_index);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_param.shop_list[shop_index] >= cfg->limit_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
		return;
	}

	if (cfg->price_type == 3)
	{
		return;
	}
	else if (cfg->price_type == 0)
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

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->reward_item.size(), &cfg->reward_item[0], false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item[0], (short)cfg->reward_item.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_JIXING_GAOZHAO);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutVec(cfg->reward_item, PUT_REASON_RA_JIXING_GAOZHAO);
	}

	m_param.shop_list[shop_index] += 1;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	//log
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] shop_index[%d], shop_time[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_JIXING_GAOZHAO,
		shop_index, m_param.shop_list[shop_index]);
}

void RoleActivityJiXingGaoZhao::RAFetchDrawReward(int index)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityJiXingGaoZhaoConfig * config = static_cast<const RandActivityJiXingGaoZhaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_JIXING_GAOZHAO));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAJiXingGaoZhaoPhaseCfg * cfg = config->GetPhaseCfg(ramgr, index);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (0 != (m_param.draw_reward_fetch_mark & (1 << index)))
	{
		return;
	}

	if (m_param.draw_times < cfg->lucky_draw_num)
	{
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->reward_item.size(), &cfg->reward_item[0], false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item[0], (short)cfg->reward_item.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_JIXING_GAOZHAO);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutVec(cfg->reward_item, PUT_REASON_RA_JIXING_GAOZHAO);
	}

	m_param.draw_reward_fetch_mark |= 1 << index;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] fetch_draw_index[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_JIXING_GAOZHAO,
		index);
}
