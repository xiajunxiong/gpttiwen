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
#include "protocol/randactivity/msgzhuanshulibao.hpp"
#include "config/randactivityconfig/impl/randactivityzhuanshulibaoconfig.hpp"
#include "roleactivityzhuanshulibao.hpp"


RoleActivityZhuanShuLiBao::RoleActivityZhuanShuLiBao(int type) :RoleActivity(type)
{
}

RoleActivityZhuanShuLiBao::~RoleActivityZhuanShuLiBao()
{
}

void RoleActivityZhuanShuLiBao::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityZhuanShuLiBao::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityZhuanShuLiBao::OnLogin()
{
	if (this->OnCheckRefresh())
	{
		this->SetRoleDataChange();
	}
	if (0 == m_param.is_data_move_flag)
	{
		const RandActivityZhuanShuLiBaoConfig * config = static_cast<const RandActivityZhuanShuLiBaoConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
		if (NULL == config)
		{
			return;
		}
		int count = 0;
		for (int i = 0; i < ARRAY_LENGTH(m_param.old_day_item) && count < ARRAY_LENGTH(m_param.new_day_item); ++i)
		{
			if(m_param.old_day_item[i].IsInVaild()) continue;
			
			m_param.new_day_item[count] = m_param.old_day_item[i];
			m_param.new_day_item[count].end_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time() + config->GetOtherCfg().duration_time);
			count++;
		}
		m_param.is_data_move_flag = 1;
		this->SetRoleDataChange();
	}
}

void RoleActivityZhuanShuLiBao::OnLoginSendInfo()
{
	this->SendInfo();
	this->OnCheckDayGift(true);
	this->SendDayInfo();
}

bool ZhuanShuLiBaoDaySort(const ZhuanShuLiBaoDayItem & a, const ZhuanShuLiBaoDayItem & b)
{
	if (a.end_timestamp != b.end_timestamp)
	{
		return a.end_timestamp > b.end_timestamp;
	}
	if (a.phase != b.phase)
	{
		return a.phase > b.phase;
	}
	if (a.type != b.type)
	{
		return a.type > b.type;
	}
	return false;
}

void RoleActivityZhuanShuLiBao::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.active_item); ++i)
	{
		m_param.active_item[i].buy_times = 0;
	}
	const RandActivityZhuanShuLiBaoConfig * config = static_cast<const RandActivityZhuanShuLiBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}
	this->OnCheckDayGift();
	if ((unsigned int)EngineAdapter::Instance().Time() >= m_param.check_init_next_zero_timestamp)
	{
		int new_phase = config->GetDayPhaseByLevel(m_role_module_mgr->GetRole()->GetLevel());
		if (new_phase != m_param.last_phase)
		{
			m_param.last_phase = new_phase;
			m_param.login_day = 1;
		}
		else
		{
			if (config->IsExceedMaxDay(m_param.last_phase, m_param.login_day + 1))	//循环礼包
			{
				m_param.login_day = 1;
			}
			else
			{
				m_param.login_day += 1;
			}
		}
		const RAZhuanShuLiBaoDayCfg * day_gift_cfg = config->GetDayGiftCfgByLogin(m_param.last_phase, m_param.login_day);
		if (NULL != day_gift_cfg)
		{
			bool is_has_gift = false;
			for (int i = 0; i < ARRAY_LENGTH(m_param.new_day_item); i++)
			{
				if(m_param.new_day_item[i].IsInVaild()) break;
				if (m_param.new_day_item[i].phase == day_gift_cfg->phase && m_param.new_day_item[i].type == day_gift_cfg->type)	//刷到已存在的则刷新时间
				{
					m_param.new_day_item[i].end_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time() + config->GetOtherCfg().duration_time);
					is_has_gift = true;
					break;
				}
			}
			if (!is_has_gift)
			{
				int index = ARRAY_LENGTH(m_param.new_day_item) - 1;
				m_param.new_day_item[index].phase = day_gift_cfg->phase;
				m_param.new_day_item[index].type = day_gift_cfg->type;
				m_param.new_day_item[index].buy_times = 0;
				m_param.new_day_item[index].reserve_sh = 0;
				m_param.new_day_item[index].end_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time() + config->GetOtherCfg().duration_time);
			}
		}
		m_param.check_init_next_zero_timestamp = (unsigned int)(EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextDayInterval(0, 0, 0));
	}
	std::sort(m_param.new_day_item, m_param.new_day_item + ARRAY_LENGTH(m_param.new_day_item), ZhuanShuLiBaoDaySort);

	this->SendInfo();
	this->SendDayInfo();
	this->SetRoleDataChange();
}

void RoleActivityZhuanShuLiBao::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
	{
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity * activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}
	const RandActivityZhuanShuLiBaoConfig * config = static_cast<const RandActivityZhuanShuLiBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		m_param.is_data_move_flag = 1;
		m_param.login_day = 1;
		m_param.last_phase = config->GetDayPhaseByLevel(m_role_module_mgr->GetRole()->GetLevel());
		const RAZhuanShuLiBaoDayCfg * day_gift_cfg = config->GetDayGiftCfgByLogin(m_param.last_phase, m_param.login_day);
		if (NULL != day_gift_cfg)
		{
			m_param.new_day_item[0].phase = day_gift_cfg->phase;
			m_param.new_day_item[0].type = day_gift_cfg->type;
			m_param.new_day_item[0].buy_times = 0;
			m_param.new_day_item[0].reserve_sh = 0;
			m_param.new_day_item[0].end_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time() + config->GetOtherCfg().duration_time);
		}

		m_param.check_init_next_zero_timestamp = (unsigned int)(EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextDayInterval(0, 0, 0));
		this->OnRoleLevelUp(0, m_role_module_mgr->GetRole()->GetLevel());
		this->SetRoleDataChange();
	}
}

bool ZhuanShuLiBaoSort(const ZhuanShuLiBaoAciveItem & item1, const ZhuanShuLiBaoAciveItem & item2)
{
	if (item1.is_active < item2.is_active)
		return true;
	if (item1.is_active > item2.is_active)
		return false;

	if (item1.end_time < item2.end_time)
		return true;
	if (item1.end_time > item2.end_time)
		return false;

	if (item1.type < item2.type)
		return true;
	if (item1.type > item2.type)
		return false;

	return false;
}

void RoleActivityZhuanShuLiBao::OnRoleLevelUp(int old_level, int to_level)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr || !ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}

	const RandActivityZhuanShuLiBaoConfig * config = static_cast<const RandActivityZhuanShuLiBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}
	
	std::vector<const RAZhuanShuLiBaoItemCfg *> item_cfg = config->GetRAZhuanShuLiBaoItemCfg(m_param.active_role_level, to_level, m_role_module_mgr->GetMaxChongZhiRecord());

	std::sort(m_param.active_item, m_param.active_item + ARRAY_LENGTH(m_param.active_item), ZhuanShuLiBaoSort);

	int active_index = 0;
	for (int index  = 0; index < (int)item_cfg.size(); ++index)
	{
		active_index = active_index % ARRAY_LENGTH(m_param.active_item);
		m_param.active_item[active_index].Reset();
		m_param.active_item[active_index].is_active = 1;
		m_param.active_item[active_index].type = item_cfg[index]->type;
		m_param.active_item[active_index].buy_times = 0;
		m_param.active_item[active_index].end_time = (unsigned int)(EngineAdapter::Instance().Time() + config->GetOtherCfg().duration_time);

		active_index += 1;
	}

	if (to_level > m_param.active_role_level)
	{
		m_param.active_role_level = to_level;
	}
	if (this->OnCheckRefresh())
	{
		this->SendDayInfo();
	}
	this->SendInfo();
	this->SetRoleDataChange();	
}

void RoleActivityZhuanShuLiBao::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	switch (raor->opera_type)
	{
		case RA_ZHUAN_SHU_LI_BAO_REQ_TYPE_INFO:
		{
			this->SendInfo();
		}
		break;
		case RA_ZHUAN_SHU_LI_BAO_REQ_TYPE_BUY_ITEM:
		{
			long long unique_id = 0;
			int ret_num = this->OnBuyItemCheck(raor->param_1, unique_id);  //购买专属礼包  param1 活动协议下发的type

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
		case RA_ZHUAN_SHU_LI_BAO_REQ_TYPE_DAY_INFO:
		{
			this->SendDayInfo();
		}
		break;
		case RA_ZHUAN_SHU_LI_BAO_REQ_TYPE_BUY_DAY_INFO:
		{
			this->OnBuyDayItemCheck(raor->param_1, raor->param_2);
		}
		break;
	}
}

bool RoleActivityZhuanShuLiBao::OnBuyItemOld(int type, int chong_zhi_gold, time_t time)
{
	int index = -1;
	for (int i = 0; i < ARRAY_LENGTH(m_param.active_item); ++i)
	{
		if (!m_param.active_item[i].is_active || m_param.active_item[i].type != type || time > (time_t)m_param.active_item[i].end_time + 120)
		{
			continue;
		}

		index = i;
		break;
	}

	if (index == -1)
	{
		return false;
	}

	const RandActivityZhuanShuLiBaoConfig * config = static_cast<const RandActivityZhuanShuLiBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return false;
	}

	const RAZhuanShuLiBaoItemCfg * buy_cfg = config->GetRAZhuanShuLiBaoItemCfg(type);
	if (buy_cfg == NULL)
	{
		return false;
	}
	if (buy_cfg->need_chong_zhi_gold != chong_zhi_gold)
	{
		return false;
	}


	if (m_param.active_item[index].buy_times >= buy_cfg->limit_buy_times)
	{
		return false;
	}

	if (buy_cfg->rewards.empty() && buy_cfg->add_gold == 0)
	{
		return false;
	}

	m_role_module_mgr->GetMoney()->AddGold(buy_cfg->add_gold, __FUNCTION__);
	if (!buy_cfg->rewards.empty())
	{
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)buy_cfg->rewards.size(), &buy_cfg->rewards[0]))
		{
			m_role_module_mgr->GetKnapsack()->PutList((short)buy_cfg->rewards.size(), &buy_cfg->rewards[0], PUT_REASON_ZHUAN_SHU_LI_BAO);
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
	}


	m_param.active_item[index].buy_times += 1;
	this->SendInfo();

	this->SetRoleDataChange();
	return true;
}

int RoleActivityZhuanShuLiBao::OnBuyGiftItem(int type, ARG_OUT int * price)
{
	unsigned int now_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	int index = -1;
	for (int i = 0; i < ARRAY_LENGTH(m_param.active_item); ++i)
	{
		if (!m_param.active_item[i].is_active || m_param.active_item[i].type != type || now_time > m_param.active_item[i].end_time + 120)
		{
			continue;
		}

		index = i;
		break;
	}

	if (index == -1)
	{
		return 1;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_ZHUAN_SHU_LI_BAO))
	{
		return 1;
	}

	const RandActivityZhuanShuLiBaoConfig * config = static_cast<const RandActivityZhuanShuLiBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}

	const RAZhuanShuLiBaoItemCfg * buy_cfg = config->GetRAZhuanShuLiBaoItemCfg(type);
	if (buy_cfg == NULL)
	{
		return 1;
	}
	if (NULL != price)
	{
		*price = buy_cfg->need_chong_zhi_gold;
		return 0;
	}

	if (m_param.active_item[index].buy_times >= buy_cfg->limit_buy_times)
	{
		return 2;
	}

	if (buy_cfg->rewards.empty() && buy_cfg->add_gold == 0)
	{
		return 1;
	}

	m_role_module_mgr->GetMoney()->AddGold(buy_cfg->add_gold, __FUNCTION__);
	if (!buy_cfg->rewards.empty())
	{
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)buy_cfg->rewards.size(), &buy_cfg->rewards[0]))
		{
			m_role_module_mgr->GetKnapsack()->PutList((short)buy_cfg->rewards.size(), &buy_cfg->rewards[0], PUT_REASON_ZHUAN_SHU_LI_BAO);
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
	}


	m_param.active_item[index].buy_times += 1;
	this->SendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

int RoleActivityZhuanShuLiBao::OnBuyItemCheck(int type, long long ARG_OUT & unique_key)
{
	unique_key = 0;
	time_t time = EngineAdapter::Instance().Time();

	int index = -1;
	for (int i = 0; i < ARRAY_LENGTH(m_param.active_item); ++i)
	{
		if (!m_param.active_item[i].is_active || m_param.active_item[i].type != type || time > (time_t)m_param.active_item[i].end_time + 120)
		{
			continue;
		}

		index = i;
		break;
	}

	if (index == -1)
	{
		return -__LINE__;
	}

	const RandActivityZhuanShuLiBaoConfig * config = static_cast<const RandActivityZhuanShuLiBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RAZhuanShuLiBaoItemCfg * buy_cfg = config->GetRAZhuanShuLiBaoItemCfg(type);
	if (buy_cfg == NULL)
	{
		return -__LINE__;
	}

	if (m_param.active_item[index].buy_times >= buy_cfg->limit_buy_times)
	{
		return -__LINE__;
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
	buy_item->param1 = type;
	buy_item->param2 = buy_cfg->add_gold;
	::ItemConfigDataVecToBuyItem(buy_cfg->rewards, buy_item->rewards_item, &buy_item->rewards_num);
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_zhuan_shu_li_bao, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_zhuan_shu_li_bao.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityZhuanShuLiBao::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_zhuan_shu_li_bao.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
					m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return  -__LINE__;
	}

	int type = buy_item->param1;
	int add_gold = buy_item->param2;

	std::vector<ItemConfigData> rewards = ::BuyItemToItemConfigDataVec(buy_item->rewards_num, buy_item->rewards_item);
	if (rewards.empty())
	{
		return -__LINE__;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_ZHUAN_SHU_LI_BAO);
	}
	else
	{
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}	
	m_role_module_mgr->GetMoney()->AddGold(add_gold, __FUNCTION__);

	//如果时间相等就说明同一次活动里购买，要增加次数。
	if (buy_item->ra_begin_timestamp == m_param.ra_begin_timestamp)
	{
		int index = -1;
		for (int i = 0; i < ARRAY_LENGTH(m_param.active_item); ++i)
		{
			if (!m_param.active_item[i].is_active || m_param.active_item[i].type != type)
			{
				continue;
			}

			index = i;
			break;
		}

		if (index != -1)
		{
			m_param.active_item[index].buy_times += 1;
			this->SendInfo();
			this->SetRoleDataChange();
		}
	}

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_zhuan_shu_li_bao, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

void RoleActivityZhuanShuLiBao::OnBuyDayItemCheck(int phase, int type)
{
	int index = -1;
	time_t now = EngineAdapter::Instance().Time();
	for (int i = 0; i < ARRAY_LENGTH(m_param.new_day_item); ++i)
	{
		if(m_param.new_day_item[i].IsInVaild()) break;
		if (m_param.new_day_item[i].phase != phase || m_param.new_day_item[i].type != type || now >(time_t)m_param.new_day_item[i].end_timestamp + 120)
		{
			continue;
		}

		index = i;
		break;
	}

	if (index == -1)
	{
		return;
	}

	const RandActivityZhuanShuLiBaoConfig * config = static_cast<const RandActivityZhuanShuLiBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RAZhuanShuLiBaoDayCfg * buy_cfg = config->GetDayGiftCfg(phase, type);
	if (buy_cfg == NULL)
	{
		return;
	}
	if (m_param.new_day_item[index].buy_times >= buy_cfg->limit_num)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_BUY_NOT_ENOUGH);
		return;
	}
	if (buy_cfg->item_list.empty())
	{
		return;
	}

	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE_DAY);
	if (buy_item == NULL)
	{
		return;
	}
	long long unique_key = 0;
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		return;
	}
	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = buy_cfg->buy_money;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = phase;
	buy_item->param2 = type;
	buy_item->param3 = buy_cfg->gold_num;
	::ItemConfigDataVecToBuyItem(buy_cfg->item_list, buy_item->rewards_item, &buy_item->rewards_num);
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE_DAY, gamelog::g_log_zhuan_shu_li_bao, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_zhuan_shu_li_bao.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());


	Protocol::SCCmdBuyItemCheckRet info;
	info.buy_type = BUY_TYPE_DAY;
	info.cfg_ver = buy_item->cfg_vesion;
	info.unique_id = unique_key;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

int RoleActivityZhuanShuLiBao::OnBuyDayItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE_DAY, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_zhuan_shu_li_bao.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return  -__LINE__;
	}

	int phase = buy_item->param1;
	int type = buy_item->param2;
	int add_gold = buy_item->param3;

	std::vector<ItemConfigData> rewards = ::BuyItemToItemConfigDataVec(buy_item->rewards_num, buy_item->rewards_item);
	if (rewards.empty())
	{
		return -__LINE__;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_ZHUAN_SHU_LI_BAO);
	}
	else
	{
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
	m_role_module_mgr->GetMoney()->AddGold(add_gold, __FUNCTION__);

	//如果时间相等就说明同一次活动里购买，要增加次数。
	if (buy_item->ra_begin_timestamp == m_param.ra_begin_timestamp)
	{
		int index = -1;
		for (int i = 0; i < ARRAY_LENGTH(m_param.new_day_item); ++i)
		{
			if (m_param.new_day_item[i].IsInVaild()) break;
			if (m_param.new_day_item[i].phase != phase || m_param.new_day_item[i].type != type)
			{
				continue;
			}

			index = i;
			break;
		}

		if (index != -1)
		{
			m_param.new_day_item[index].buy_times += 1;
			bool is_reset = true;
			const RandActivityZhuanShuLiBaoConfig * config = static_cast<const RandActivityZhuanShuLiBaoConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
			if (NULL != config)
			{
				const RAZhuanShuLiBaoDayCfg * buy_cfg = config->GetDayGiftCfg(phase, type);
				if (NULL != buy_cfg && m_param.new_day_item[index].buy_times < buy_cfg->limit_num)
				{
					is_reset = false;
				}
			}
			if (is_reset)
			{
				m_param.new_day_item[index].Reset();
				std::sort(m_param.new_day_item, m_param.new_day_item + ARRAY_LENGTH(m_param.new_day_item), ZhuanShuLiBaoDaySort);
			}

			this->SendDayInfo();
			this->SetRoleDataChange();
		}
	}

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE_DAY, unique_id, gamelog::g_log_zhuan_shu_li_bao, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

int RoleActivityZhuanShuLiBao::OnBuyGiftDayItem(int phase, int type, ARG_OUT int * price)
{
	int index = -1;
	time_t now = EngineAdapter::Instance().Time();
	for (int i = 0; i < ARRAY_LENGTH(m_param.new_day_item); ++i)
	{
		if (m_param.new_day_item[i].IsInVaild()) break;
		if (m_param.new_day_item[i].phase != phase || m_param.new_day_item[i].type != type || now > (time_t)m_param.new_day_item[i].end_timestamp + 120)
		{
			continue;
		}

		index = i;
		break;
	}

	if (index == -1)
	{
		return 1;
	}

	const RandActivityZhuanShuLiBaoConfig * config = static_cast<const RandActivityZhuanShuLiBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}

	const RAZhuanShuLiBaoDayCfg * buy_cfg = config->GetDayGiftCfg(phase, type);
	if (buy_cfg == NULL)
	{
		return 1;
	}
	if (m_param.new_day_item[index].buy_times >= buy_cfg->limit_num)
	{
		return 2;
	}
	if (buy_cfg->item_list.empty())
	{
		return 1;
	}
	if (NULL != price)
	{
		*price = buy_cfg->buy_money;
		return 0;
	}

	m_role_module_mgr->GetMoney()->AddGold(buy_cfg->gold_num, __FUNCTION__);
	if (!buy_cfg->item_list.empty())
	{
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)buy_cfg->item_list.size(), &buy_cfg->item_list[0]))
		{
			m_role_module_mgr->GetKnapsack()->PutList((short)buy_cfg->item_list.size(), &buy_cfg->item_list[0], PUT_REASON_ZHUAN_SHU_LI_BAO);
		}
		else
		{
			// 发邮件
			static MailContentParam contentparam; contentparam.Reset();
			FillMailContentParam(contentparam, buy_cfg->item_list);

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, m_role_activity_type);
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}


	m_param.new_day_item[index].buy_times += 1;
	bool is_reset = true;
	if (m_param.new_day_item[index].buy_times < buy_cfg->limit_num)
	{
		is_reset = false;
	}
	if (is_reset)
	{
		m_param.new_day_item[index].Reset();
		std::sort(m_param.new_day_item, m_param.new_day_item + ARRAY_LENGTH(m_param.new_day_item), ZhuanShuLiBaoDaySort);
	}
	this->SendDayInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

void RoleActivityZhuanShuLiBao::GmSetNextDayTimestamp()
{
	m_param.check_init_next_zero_timestamp = 0;
	this->SetRoleDataChange();
}

void RoleActivityZhuanShuLiBao::SendInfo()
{
	Protocol::SCRAZhuanShuLiBaoInfo protocol;
	for (int i = 0; i < RA_ZHUAN_SHU_LI_BAO_MAX_EXIST_ITEM; ++i)
	{
		protocol.active_item[i] = m_param.active_item[i];
	}

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

void RoleActivityZhuanShuLiBao::SendDayInfo()
{
	Protocol::SCRAZhuanShuLiBaoDayInfo info;
	for (int i = 0; i < ARRAY_LENGTH(m_param.new_day_item); i++)
	{
		info.day_item[i] = m_param.new_day_item[i];
	}

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityZhuanShuLiBao::OnCheckDayGift(bool is_sort)
{
	const RandActivityZhuanShuLiBaoConfig * config = static_cast<const RandActivityZhuanShuLiBaoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	bool is_change = false;
	for (int i = 0; i < ARRAY_LENGTH(m_param.new_day_item); ++i)
	{
		if(m_param.new_day_item[i].IsInVaild()) break;
	
		if (NULL == config->GetDayGiftCfg(m_param.new_day_item[i].phase, m_param.new_day_item[i].type))
		{
			m_param.new_day_item[i].Reset();
			is_change = true;
		}
	}
	if (is_change)
	{
		if (is_sort)
		{
			std::sort(m_param.new_day_item, m_param.new_day_item + ARRAY_LENGTH(m_param.new_day_item), ZhuanShuLiBaoDaySort);
		}
		this->SetRoleDataChange();
	}
}

bool RoleActivityZhuanShuLiBao::OnCheckRefresh()
{
	const RandActivityZhuanShuLiBaoConfig* config = static_cast<const RandActivityZhuanShuLiBaoConfig*>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return false;
	}
	//表示之前等级不够，还未获得，或者中途调整等级,刚登陆玩家触发检测
	if (m_param.last_phase <= 0 && m_param.new_day_item[0].IsInVaild())
	{
		m_param.login_day = 1;
		m_param.last_phase = config->GetDayPhaseByLevel(m_role_module_mgr->GetRole()->GetLevel());
		const RAZhuanShuLiBaoDayCfg* day_gift_cfg = config->GetDayGiftCfgByLogin(m_param.last_phase, m_param.login_day);
		if (NULL != day_gift_cfg)
		{
			m_param.new_day_item[0].phase = day_gift_cfg->phase;
			m_param.new_day_item[0].type = day_gift_cfg->type;
			m_param.new_day_item[0].buy_times = 0;
			m_param.new_day_item[0].reserve_sh = 0;
			m_param.new_day_item[0].end_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time() + config->GetOtherCfg().duration_time);
		}

		m_param.check_init_next_zero_timestamp = (unsigned int)(EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextDayInterval(0, 0, 0));
		return true;
	}
	return false;
}
