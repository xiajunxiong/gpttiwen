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
#include "roleactivitycharismatictanabata.hpp"
#include "protocol/msgother.h"
#include "global/rank/rankmanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "config/randactivityconfig/impl/randactivitycharismatictanabataconfig.hpp"

RoleActivityCharismaticTanabata::RoleActivityCharismaticTanabata(int type) :RoleActivity(type)
{
}

RoleActivityCharismaticTanabata::~RoleActivityCharismaticTanabata()
{
}

void RoleActivityCharismaticTanabata::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityCharismaticTanabata::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityCharismaticTanabata::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityCharismaticTanabata::CheckInitRandActivity(int rand_act_type, bool is_force)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.begin_time);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.begin_time = static_cast<unsigned int>(ramgr->GetRandActivityBeginTime(m_role_activity_type));

		this->SetRoleDataChange();
	}
}

void RoleActivityCharismaticTanabata::OnRAClose()
{

}

void RoleActivityCharismaticTanabata::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	switch (raor->opera_type)
	{
		case 0:  //请求下发
		{
			this->SendInfo();
		}
		break;
		case 1://领取奖励 param1 奖励类型【0/1】 param2 seq
		{
			this->GetReward(raor->param_1, raor->param_2);
		}
		break;
	}
}

void RoleActivityCharismaticTanabata::GetReward(int reward_type, int seq)
{
	const RandActivityCharismaticTanabataConfig * config = static_cast<const RandActivityCharismaticTanabataConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CHARISMATIC_TANABATA));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	int act_real_open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_CHARISMATIC_TANABATA);

	const CharismaticTanabataBaseConfig * base_cfg = config->GetBaseCfg(act_real_open_day, reward_type, seq);
	if (NULL == base_cfg)return;
	
	switch (reward_type)
	{
	case CharismaticTanabataBaseListConfig::BASE_TYPE_LOVE:
		if (base_cfg->add_intimate > m_param.admiration_value)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RA_CHARISMATIC_TANABATA_LOVE_VALUE_LIMIT);
			return;
		}
		if (m_param.admiration_map.IsBitSet(seq))
		{
			return;
		}
		if (!m_role_module_mgr->GetKnapsack()->PutList((short)base_cfg->reward_vec.size(), &base_cfg->reward_vec[0], PUT_REASON_CHARISMAITC_TANABATA_REWARD))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
			return;
		}
		m_param.admiration_map.SetBit(seq);
		break;
	case CharismaticTanabataBaseListConfig::BASE_TYPE_MEILI:
		if (base_cfg->add_intimate > m_param.charm_value)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RA_CHARISMATIC_TANABATA_MEILI_VALUE_LIMIT);
			return;
		}
		if (m_param.charm_map.IsBitSet(seq))
		{
			return;
		}
		if (!m_role_module_mgr->GetKnapsack()->PutList((short)base_cfg->reward_vec.size(), &base_cfg->reward_vec[0], PUT_REASON_CHARISMAITC_TANABATA_REWARD))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
			return;
		}
		m_param.charm_map.SetBit(seq);
		break;
	}
	this->SendInfo();
}

void RoleActivityCharismaticTanabata::OnRoleSendFlows(int add_value)
{
	m_param.charm_value += add_value;
	RankManager::Instance().SyncPersonRankInfo(m_role_module_mgr->GetRole(), PERSON_RANK_TYPE_MEILI_QIXI);
	this->SendInfo();
}

void RoleActivityCharismaticTanabata::OnSendRoleFlasw(int add_value)
{
	m_param.admiration_value += add_value;
	this->SendInfo();
}

void RoleActivityCharismaticTanabata::SendInfo()
{
	Protocol::SCCharismaitcTanabataInfo protocol;

	protocol.charm_value = m_param.charm_value;
	protocol.admiration_map = m_param.admiration_map;
	protocol.admiration_value = m_param.admiration_value;
	protocol.charm_map = m_param.charm_map;

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}