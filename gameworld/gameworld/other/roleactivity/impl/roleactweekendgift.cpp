#include "roleactweekendgift.hpp"
#include "config/randactivityconfig/impl/raweekendgiftconfig.hpp"

#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "obj/character/role.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/route/mailroute.hpp"
#include "other/rolemodulemanager.hpp"
#include "servercommon/string/gameworldstr.h"

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "servercommon/uniqueidgenerator.hpp"

RoleACTWeekendGift::RoleACTWeekendGift(int type)
	: RoleActivity(type)
{
}

RoleACTWeekendGift::~RoleACTWeekendGift()
{
}

void RoleACTWeekendGift::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RoleRAWeekendGiftParam));
}

void RoleACTWeekendGift::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RoleRAWeekendGiftParam));
}

void RoleACTWeekendGift::OnLogin()
{
	this->CheckPutGift(EngineAdapter::Instance().DayID());
}

void RoleACTWeekendGift::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleACTWeekendGift::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid)
	{
		return;
	}
	
	this->CheckPutGift(now_dayid);
}

void RoleACTWeekendGift::CheckInitRandActivity(int rand_act_type, bool is_force)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	
		this->CheckPutGift(EngineAdapter::Instance().DayID());
		this->SetRoleDataChange();
	}
}

void RoleACTWeekendGift::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	this->CheckInitRandActivity(m_role_activity_type);

	switch (raor->opera_type)
	{
	case RA_WEEKEND_GIFT_REQ_TYPE_CMD_BUY:
		{
			long long unique_id = 0;
			int ret_num = this->BuyItemCheck(raor->cfg_ver, raor->param_1, unique_id);
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
	default:
		return;
	}

}

int RoleACTWeekendGift::BuyItemCheck(int cfg_ver, int gift_index, long long ARG_OUT & unique_key)
{
	unique_key = 0;

	const RAWeekendGiftConfig * config = static_cast<const RAWeekendGiftConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -1;
	}

	const RAWeekendGift::OtherCfg &other_cfg = config->GetOtherCfg();
	if (other_cfg.cfg_ver != cfg_ver)
	{
		return -2;
	}

	const RAWeekendGift::GiftCfg *gift_cfg = config->GetGiftCfg(gift_index);
	if (NULL == gift_cfg)
	{//无配置
		return -3;
	}

	int save_index = this->GetSaveIndex(gift_index);
	if (0 > save_index
		|| RAWeekendGift::ROLE_GIFT_MAX <= save_index)
	{
		return -4;
	}

	time_t now = EngineAdapter::Instance().Time();
	if (now > m_param.gift_list[save_index].out_time_s)
	{//已失效
		return -5;
	}

	if (gift_cfg->limit_num <= m_param.gift_list[save_index].buy_times)
	{//超限购次数
		return -6;
	}

	//允许购买
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		unique_key = 0;
		return -7;
	}

	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE);
	if (buy_item == NULL)
	{
		return -8;
	}

	buy_item->cfg_vesion = other_cfg.cfg_ver;
	buy_item->need_gold = gift_cfg->buy_money;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = gift_index;
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleACTWeekendGift::BuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -1;
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u  %s",
		__FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -2;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return -3;
	}

	const int gift_type_index = buy_item->param1;
	const RAWeekendGiftConfig * config = static_cast<const RAWeekendGiftConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -4;
	}

	const RAWeekendGift::OtherCfg &other_cfg = config->GetOtherCfg();
	if (other_cfg.cfg_ver != buy_item->cfg_vesion)
	{
		return -5;
	}

	
	const RAWeekendGift::GiftCfg *gift_cfg = config->GetGiftCfg(gift_type_index);
	if (NULL == gift_cfg)
	{//无配置
		return -6;
	}

	//礼包中途可能失效
	int buy_times = -1;

	int save_index = this->GetSaveIndex(gift_type_index);
	if (0 <= save_index
		&& RAWeekendGift::ROLE_GIFT_MAX > save_index
		&& gift_type_index == m_param.gift_list[save_index].gift_index)
	{//未失效 记录
		m_param.gift_list[save_index].buy_times++;
		buy_times = m_param.gift_list[save_index].buy_times;
	}
	else
	{//失效
		//奖励照发
	}

	this->SendInfo();
	this->SetRoleDataChange();

	//奖励
	if (!gift_cfg->item_vec.empty())
	{
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)gift_cfg->item_vec.size(), &gift_cfg->item_vec[0]))
		{
			m_role_module_mgr->GetKnapsack()->PutList((short)gift_cfg->item_vec.size(), &gift_cfg->item_vec[0], PUT_REASON_RA_WEEKEND_GIFT);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&gift_cfg->item_vec[0], (short)gift_cfg->item_vec.size(), SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP, true, m_role_activity_type);
		}
	}

	//日志
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s succ |  role[%d %s] gift_index[%d] buy_times[%d].",
		__FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		gift_cfg->gift_type_index, buy_times);


	//注意 buy_item 野指针警告 Save必须放最后
	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

void RoleACTWeekendGift::SendInfo()
{
	Protocol::SCRAWeekendGiftInfo info;
	info.count = 0;

	for (int i = 0; i < RAWeekendGift::ROLE_GIFT_MAX; i++)
	{
		if (!m_param.gift_list[i].IsValid())
		{
			continue;
		}
		info.gift_list[info.count].gift_index = m_param.gift_list[i].gift_index;
		info.gift_list[info.count].buy_times = m_param.gift_list[i].buy_times;
		info.gift_list[info.count].out_time_s = m_param.gift_list[i].out_time_s;

		info.count++;
	}
	int send_ln = sizeof(info) - sizeof(info.gift_list[0])* (RAWeekendGift::ROLE_GIFT_MAX - info.count);
	m_role_module_mgr->NetSend(&info, send_ln);
}

void RoleACTWeekendGift::RemoveTimeOut()
{
	unsigned int now_secnd = (unsigned int)EngineAdapter::Instance().Time();

	for (int i = 0; i < RAWeekendGift::ROLE_GIFT_MAX; i++)
	{
		if (!m_param.gift_list[i].IsValid())
		{
			continue;
		}

		if (now_secnd < m_param.gift_list[i].out_time_s)
		{
			continue;
		}

		m_param.gift_list[i].Reset();
	}

	//重排
	std::sort(m_param.gift_list,
		m_param.gift_list + RAWeekendGift::ROLE_GIFT_MAX,
		RoleRAWeekendGiftParam::SortFunnc);
}

void RoleACTWeekendGift::CheckPutGift(unsigned int now_dayid)
{
	if (now_dayid == m_param.last_chack_day_id)
	{//今日已检测
		return;
	}
	//兼容一下gm addday
	m_param.last_chack_day_id = EngineAdapter::Instance().DayID();
	
	//限制开启星期
	const tm *now_tm = EngineAdapter::Instance().LocalTime();
	switch (now_tm->tm_wday)
	{
	case SUNDAY:
	case SATURDAY:
		break;
	default:
		return;
	}

	const RAWeekendGiftConfig * config = static_cast<const RAWeekendGiftConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}
	const RAWeekendGift::OtherCfg &other_cfg = config->GetOtherCfg();

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	//随机礼包索引
	int add_gift_type_index = config->GetRandGiftIndex(ramgr,
			m_role_module_mgr->GetRole()->GetLevel(),
			m_role_module_mgr->GetMaxChongZhiRecord());
	if (0 > add_gift_type_index)
	{
		return;
	}

	int save_index = GetSaveIndex(add_gift_type_index);
	if (0 > save_index
		|| RAWeekendGift::ROLE_GIFT_MAX <= save_index)
	{//未找到相同礼包 覆盖第一个
		save_index = 0;
	}

	//刷一遍数据
	m_param.gift_list[save_index].gift_index = add_gift_type_index;
	m_param.gift_list[save_index].buy_times = 0;
	//刷新时间 (写死 必须周日前结束)
	unsigned int act_end_interval = EngineAdapter::Instance().NextWeekInterval(MONDAY, 0, 0, 0);
	m_param.gift_list[save_index].out_time_s
		= (unsigned int)EngineAdapter::Instance().Time()
		+ ((unsigned int)other_cfg.duration_time > act_end_interval
			? act_end_interval : other_cfg.duration_time);

	//重排
	std::sort(m_param.gift_list,
		m_param.gift_list + RAWeekendGift::ROLE_GIFT_MAX,
		RoleRAWeekendGiftParam::SortFunnc);

	this->SetRoleDataChange();
	this->SendInfo();
}

int RoleACTWeekendGift::GetSaveIndex(const int gift_type_index) const
{
	for (int i = 0; i < RAWeekendGift::ROLE_GIFT_MAX; i++)
	{
		if (!m_param.gift_list[i].IsValid())
		{
			continue;
		}

		if (gift_type_index == m_param.gift_list[i].gift_index)
		{
			return i;
		}
	}
	return -1;
}
