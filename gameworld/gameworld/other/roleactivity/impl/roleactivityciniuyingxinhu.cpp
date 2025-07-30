#include "roleactivityciniuyingxinhu.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityciniuyingxinhuconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"
#include "servercommon/string/gameworldstr.h"

RoleActivityCiNiuYingXinHu::RoleActivityCiNiuYingXinHu(int type) :RoleActivity(type)
{
}

RoleActivityCiNiuYingXinHu::~RoleActivityCiNiuYingXinHu()
{
}

void RoleActivityCiNiuYingXinHu::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RACiNiuYingXinHuParam));
}

void RoleActivityCiNiuYingXinHu::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RACiNiuYingXinHuParam));
}

void RoleActivityCiNiuYingXinHu::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
		return;

	this->RASendInfo();
}

void RoleActivityCiNiuYingXinHu::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
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

void RoleActivityCiNiuYingXinHu::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
	case 0:
		{
			this->RASendInfo();
		}
		break;
	case 1:
		{
			this->RAAddTigerAir(raor->param_1);
		}
		break;
	case 2:
		{
			this->RAFetchReward(raor->param_1, raor->param_2);
		}
		break;
	case 3:
		{
			this->RAAwaken(raor->param_1);
		}
		break;
	case 4:
		{
			this->RADrawAwakenReward();
		}
		break;
	}
}

void RoleActivityCiNiuYingXinHu::RASendInfo()
{
	Protocol::SCRACiNiuYingXinHuInfo info;
	info.awaken_mark = m_param.awaken_mark;
	info.awaken_reward_time = m_param.awaken_reward_time;
	memcpy(info.awaken_param, m_param.awaken_param, sizeof(m_param.awaken_param));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityCiNiuYingXinHu::RAAddTigerAir(int type)
{
	if (type < 0 || type >= RA_CINIU_YINGXINHU_TYPE_MAX)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityCiNiuYingXinHuConfig * config = static_cast<const RandActivityCiNiuYingXinHuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RACiNiuYingXinHuBasisCfg * basis_cfg = config->GetBasisCfg(ramgr, type);
	if (NULL == basis_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	int activity_day = config->GetActOpenDay(ramgr, m_role_activity_type);
	if (activity_day < basis_cfg->activity_days)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_ACTIVITY_DAY_NOT);
		return;
	}

	int max_param = config->GetGiftMaxCount(ramgr, type);
	if (m_param.awaken_param[type].param >= max_param)
	{
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(config->GetOtherCfg()->item_id, config->GetOtherCfg()->item_num, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_LACK_ITEM);
		return;
	}

	m_param.awaken_param[type].param += config->GetOtherCfg()->add_tiger_air;

	m_role_module_mgr->NoticeNum(noticenum::NT_RA_CINIU_YINXINGHU_ADD_TIGER);

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityCiNiuYingXinHu::RAFetchReward(int type, int index)
{
	if (type < 0 || type >= RA_CINIU_YINGXINHU_TYPE_MAX)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (0 != (m_param.awaken_param[type].reward_mark & (1 << index)))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityCiNiuYingXinHuConfig * config = static_cast<const RandActivityCiNiuYingXinHuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RACiNiuYingXinHuGiftCfg * cfg = config->GetGiftCfg(ramgr, type, index);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_param.awaken_param[type].param < cfg->param)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_NOT_ELIGIBLE);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->reward.size(), &cfg->reward[0], false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward[0], (short)cfg->reward.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutVec(cfg->reward, PUT_REASON_RA_CINIU_YINGXINHU);
	}

	m_param.awaken_param[type].reward_mark |= 1 << index;
	this->RASendInfo();
	this->SetRoleDataChange();

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] fetch_type[%d] index[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		type, index);

	this->CheckAwaken(type);
}

void RoleActivityCiNiuYingXinHu::CheckAwaken(int type)
{
	if (type < 0 || type >= RA_CINIU_YINGXINHU_TYPE_MAX)
		return;


	if (0 != (m_param.awaken_mark & (1 << type)))
		return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
		return;

	const RandActivityCiNiuYingXinHuConfig * config = static_cast<const RandActivityCiNiuYingXinHuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
		return;


	bool is_awaken = true;
	int max_gift = config->GetGiftMax(ramgr, type);
	for (int i = max_gift - 1; i >= 0; i--)
	{
		if (0 == (m_param.awaken_param[type].reward_mark & (1 << i)))
		{
			is_awaken = false;
			break;
		}
	}

	if (!is_awaken) return;

	m_param.awaken_mark |= 1 << type;
	m_param.awaken_reward_time += 1;
	this->RASendInfo();
	this->SetRoleDataChange();

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] awaken_type[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		type);
}

void RoleActivityCiNiuYingXinHu::RAAwaken(int type)
{
	if (type < 0 || type >= RA_CINIU_YINGXINHU_TYPE_MAX)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (0 != (m_param.awaken_mark & (1 << type)))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_CINIU_YINGXINGHU_AWAKEN);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityCiNiuYingXinHuConfig * config = static_cast<const RandActivityCiNiuYingXinHuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	bool is_awaken = true;
	int max_gift = config->GetGiftMax(ramgr, type);
	for (int i = max_gift - 1; i >= 0; i--)
	{
		if (0 == (m_param.awaken_param[type].reward_mark & (1 << i)))
		{
			is_awaken = false;
			break;
		}
	}

	if (!is_awaken) return;

	m_param.awaken_mark |= 1 << type;
	m_param.awaken_reward_time += 1;
	this->RASendInfo();
	this->SetRoleDataChange();

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] awaken_type[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		type);
}

void RoleActivityCiNiuYingXinHu::RADrawAwakenReward()
{
	if (m_param.awaken_reward_time <= 0)
		return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityCiNiuYingXinHuConfig * config = static_cast<const RandActivityCiNiuYingXinHuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RACiNiuYingXinHuBasisCfg * basis_cfg = config->GetBasisCfg(ramgr, 0);
	if (NULL == basis_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const RACiNiuYingXinHuGroup * cfg = config->GetGroupCfg(basis_cfg->awaken_reward_group);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->item.item_id, cfg->item.num, false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->item, 1, SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->Put(cfg->item, PUT_REASON_RA_CINIU_YINGXINHU);
	}

	if (cfg->is_hearsay)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_randactivity_ciniu_yinxinghu_notice,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), cfg->item.item_id);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_ROLLING_NOTICE));
		}
	}

	m_param.awaken_reward_time -= 1;
	this->RASendInfo();
	this->SetRoleDataChange();

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] reward_item[%d:%d] surplus_time[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		cfg->item.item_id, cfg->item.num, m_param.awaken_reward_time);
}
