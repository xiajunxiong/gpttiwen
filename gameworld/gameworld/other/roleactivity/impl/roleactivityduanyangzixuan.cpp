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
#include "protocol/randactivity/msgraduanyangzixuan.hpp"
#include "config/randactivityconfig/impl/randactivityduanyangzixuanconfig.hpp"
#include "roleactivityduanyangzixuan.hpp"


RoleActivityDuanYangZiXuan::RoleActivityDuanYangZiXuan(int type) :RoleActivity(type)
{
}

RoleActivityDuanYangZiXuan::~RoleActivityDuanYangZiXuan()
{
}

void RoleActivityDuanYangZiXuan::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityDuanYangZiXuan::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityDuanYangZiXuan::OnLoginSendInfo()
{
	this->SendInfo();
}


void RoleActivityDuanYangZiXuan::CheckInitRandActivity(int rand_act_type, bool is_force)
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
		this->OnCheckInitBonus();

		this->SetRoleDataChange();
	}
}

void RoleActivityDuanYangZiXuan::OnRAClose()
{
	this->GiveUnFetchReward();
}

void RoleActivityDuanYangZiXuan::OnLoginCheck()
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

void RoleActivityDuanYangZiXuan::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	switch (raor->opera_type)
	{
		case 0://请求下发
			{
				this->SendInfo();
			}
			break;
		case 1:
			{
				long long unique_id = 0;
				int ret_num = this->OnBuyItemCheck(unique_id);	//购买定制宝箱	 param1 配置seq，param2 选择的宝箱按位来

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
		case 2:
			{
				this->FetchReward(raor->param_1, raor->param_2);		//领取奖励 p1 是哪天的 p2 选的奖励reward_seq
			}
			break;
	}
}

int RoleActivityDuanYangZiXuan::FetchReward(int day, int seq)
{
	if (!m_param.is_buy_reward)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityDuanYangZiXuanConfig * config = static_cast<const RandActivityDuanYangZiXuanConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RandActivityDuanYangZiXuanCfg * cfg = config->GetRandActivityDuanYangZiXuanCfg(ramgr, day);
	if (!cfg)
	{
		return -__LINE__;
	}

	const RandActivityDuanYangZiXuanRewardGroupCfg * reward_cfg = config->GetRandActivityDuanYangZiXuanRewardGroupCfg(ramgr, cfg->reward_group_id, seq);
	if (!reward_cfg)
	{
		return -__LINE__;
	}

	int act_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_DUAN_YANG_ZI_XUAN);
	if (cfg->day > act_open_day)
	{
		return -__LINE__;
	}

	int index = cfg->day - 1;
	if (index < 0 || index >= BitCount(m_param.fetch_reward_flag) || index >= DUAN_YANG_ZI_XUAN_BUY_ITEM_MAX_NUM)
	{
		return -__LINE__;
	}

	if (IsSetBit(m_param.fetch_reward_flag, index))
	{
		return -__LINE__;
	}

	const std::vector<ItemConfigData> & rewards = reward_cfg->rewards;
	if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(rewards))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HAS_NO_SPACING);
		return -__LINE__;
	}

	m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_RA_DUAN_YANG_ZI_XUAN);

	SetBit(m_param.fetch_reward_flag, index);
	m_param.select_reward_seq[index] = (unsigned char)seq;
	this->SendInfo();
	this->SetRoleDataChange();

	return 0;
}

int RoleActivityDuanYangZiXuan::OnBuyItemCheck(long long ARG_OUT & unique_key)
{
	unique_key = 0;

	if (m_param.is_buy_reward)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityDuanYangZiXuanConfig * config = static_cast<const RandActivityDuanYangZiXuanConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const  RandActivityDuanYangZiXuanOtherCfg & buy_cfg = config->GetOtherCfg();

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
	buy_item->need_gold = buy_cfg.need_chong_zhi_gold;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_duan_yang_zi_xuan, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_duan_yang_zi_xuan.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityDuanYangZiXuan::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_duan_yang_zi_xuan.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return  -__LINE__;
	}

	m_param.is_buy_reward = 1;
	this->SetRoleDataChange();
	this->SendInfo();

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_duan_yang_zi_xuan, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

int RoleActivityDuanYangZiXuan::OnBuyGiftItem(ARG_OUT int * price)
{
	if (m_param.is_buy_reward)
	{
		return 2;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}

	const RandActivityDuanYangZiXuanConfig * config = static_cast<const RandActivityDuanYangZiXuanConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}

	const RandActivityDuanYangZiXuanOtherCfg & buy_cfg = config->GetOtherCfg();
	if (NULL != price)
	{
		*price = buy_cfg.need_chong_zhi_gold;
		return 0;
	}

	m_param.is_buy_reward = 1;
	this->SetRoleDataChange();
	this->SendInfo();
	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

void RoleActivityDuanYangZiXuan::SendInfo()
{
	Protocol::SCRADuanYanZiXuanInfo protocol;
	protocol.is_buy_reward = m_param.is_buy_reward;
	protocol.fetch_reward_flag = m_param.fetch_reward_flag;
	SameArrayTypeCopy(protocol.select_reward_seq, m_param.select_reward_seq);

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

void RoleActivityDuanYangZiXuan::GiveUnFetchReward()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!m_param.is_buy_reward)
	{
		return;
	}

	const RandActivityDuanYangZiXuanConfig * config = static_cast<const RandActivityDuanYangZiXuanConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RandActivityDuanYangZiXuanSectionCfg * cfg = config->GetRandActivityDuanYangZiXuanSectionCfg(ramgr, m_param.ra_begin_timestamp);
	if (!cfg)
	{
		return;
	}

	for (std::map<int, RandActivityDuanYangZiXuanCfg> ::const_iterator iter = cfg->item_list.begin(); iter != cfg->item_list.end(); ++iter)
	{
		const RandActivityDuanYangZiXuanCfg & curr = iter->second;
		int index = curr.day - 1;

		if (IsSetBit(m_param.fetch_reward_flag, index))
		{
			continue;
		}

		const RandActivityDuanYangZiXuanRewardGroupCfg * reward_cfg = config->GetRandActivityDuanYangZiXuanRewardGroupCfg(ramgr, curr.reward_group_id, 0);  //每天3个选一个奖励领取，补发默认发第一个
		if (!reward_cfg)
		{
			continue;
		}

		SetBit(m_param.fetch_reward_flag, index);

		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, reward_cfg->rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_unfetch_reward_title, m_role_activity_type);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_unfetch_reward_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	m_param.is_buy_reward = 0;
	this->SetRoleDataChange();
}

