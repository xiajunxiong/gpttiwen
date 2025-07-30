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
#include "protocol/randactivity/msgrashenshoujianglin.hpp"
#include "global/randactivity/randactivityimpl/randactivityshenshoujianglin.hpp"
#include "config/randactivityconfig/impl/randactivityshenshoujianglinconfig.hpp"
#include "other/roleactivity/impl/roleactivityshenshoujianglin.hpp"


RoleActivityShenShouJiangLin::RoleActivityShenShouJiangLin(int type) :RoleActivity(type)
{

}

RoleActivityShenShouJiangLin::~RoleActivityShenShouJiangLin()
{

}

void RoleActivityShenShouJiangLin::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityShenShouJiangLin::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityShenShouJiangLin::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.ra_day_buy_times); ++i)
	{
		m_param.ra_day_buy_times[i] = 0;
	}

	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityShenShouJiangLin::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityShenShouJiangLin::CheckInitRandActivity(int rand_act_type, bool is_force)
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

		this->SetRoleDataChange();
	}
}

void RoleActivityShenShouJiangLin::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	switch (raor->opera_type)
	{
		case 0:
			{
				RandActivityShenShouJiangLin * act = (RandActivityShenShouJiangLin *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN);
				if (NULL == act) return;

				act->Draw(m_role_module_mgr->GetRole());

				this->OnParticipateRA(__FUNCTION__);
			}
			break;
		case 1: // 神兽降临直购 param1 配置里seq
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

					this->OnParticipateRA(__FUNCTION__);
				}
			}
			break;
	}
}

int RoleActivityShenShouJiangLin::OnBuyGiftItem(int seq, ARG_OUT int * price)
{
	if (seq < 0 || seq >= RA_SHEN_SHOU_JIANG_SHOU_MAX_ITEM)
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

	const RandActivityShenShouJiangLinConfig * config = static_cast<const RandActivityShenShouJiangLinConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}

	const RAShenShouJiangLinBuyItemCfg * buy_cfg = config->GetRAShenShouJiangLinBuyItemCfg(ramgr, seq);
	if (buy_cfg == NULL)
	{
		return 1;
	}

	if (buy_cfg->need_chong_zhi <= 0)
	{
		return 1;
	}

	if (NULL != price)
	{
		*price = buy_cfg->need_chong_zhi;
		return 0;
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
	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList2(buy_cfg->rewards, false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&buy_cfg->rewards[0], (short)buy_cfg->rewards.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutVec(buy_cfg->rewards, PUT_REASON_SHEN_SHOU_JIANG_LIN_BUY);
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

	return 0;
}

int RoleActivityShenShouJiangLin::OnBuyItemCheck(int seq, long long ARG_OUT & unique_key)
{
	unique_key = 0;

	if (seq < 0 || seq >= RA_SHEN_SHOU_JIANG_SHOU_MAX_ITEM)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return -__LINE__;
	}

	const RandActivityShenShouJiangLinConfig * config = static_cast<const RandActivityShenShouJiangLinConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RAShenShouJiangLinBuyItemCfg * buy_cfg = config->GetRAShenShouJiangLinBuyItemCfg(ramgr, seq);
	if (buy_cfg == NULL)
	{
		return -__LINE__;
	}

	if (buy_cfg->need_chong_zhi <= 0)
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

	buy_item->cfg_vesion = config->GetConfigVer(ramgr);
	buy_item->need_gold = buy_cfg->need_chong_zhi;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = seq;
	buy_item->param2 = buy_cfg->limit_type;
	::ItemConfigDataVecToBuyItem(buy_cfg->rewards, buy_item->rewards_item, &buy_item->rewards_num);
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_shen_shou_jiang_lin, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_shen_shou_jiang_lin.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityShenShouJiangLin::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_shen_shou_jiang_lin.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
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

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_SHEN_SHOU_JIANG_LIN_BUY);
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
		int seq = buy_item->param1;
		int limit_type = buy_item->param2;
	
		gamelog::g_log_shen_shou_jiang_lin.printf(LL_INFO, "%s line:%d | role[%d %s] limit_type:%d seq:%d", __FUNCTION__, __LINE__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), limit_type, seq);

		if (0 <= seq && seq < RA_SHEN_SHOU_JIANG_SHOU_MAX_ITEM)
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

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_shen_shou_jiang_lin, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	return 0;
}

BaoDiInfoItem * RoleActivityShenShouJiangLin::GetBaoDiInfoItem(int bao_di_id)
{
	int index = bao_di_id - 1;
	if (index < 0 || index >= ARRAY_LENGTH(m_param.bao_di_info_list))
	{
		return NULL;
	}

	return &m_param.bao_di_info_list[index];
}

void RoleActivityShenShouJiangLin::SetBaoDiInfoAfterDraw(int getreward_bao_di_id, std::set<int> add_times_list)
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

void RoleActivityShenShouJiangLin::GetAllBaoDiInfoItem(BaoDiInfoItem(&bao_di_info_list)[RA_SHEN_SHOU_JIANG_SHOU_MAX_ITEM])
{
	SameArrayTypeCopy(bao_di_info_list, m_param.bao_di_info_list);
}

void RoleActivityShenShouJiangLin::SendInfo()
{
	Protocol::SCRAShenShouJiangLinBuyInfo protocol;
	for (int i = 0; i < RA_SHEN_SHOU_JIANG_SHOU_MAX_ITEM; ++i)
	{
		protocol.ra_buy_times[i] = m_param.ra_buy_times[i];
		protocol.ra_day_buy_times[i] = m_param.ra_day_buy_times[i];
	}

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}


