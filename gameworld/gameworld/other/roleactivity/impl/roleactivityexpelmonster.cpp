#include "roleactivityexpelmonster.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityexpelmonsterconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/funopen/funopen.hpp"

#include "protocol/randactivity/msgraexpelmonster.hpp"

#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

enum EXPEL_MONSTER_MONEY_TYPE
{
	EXPEL_MONSTER_MONEY_TYPE_GOLD = 0, // 元宝
	EXPEL_MONSTER_MONEY_TYPE_COIN = 1, // 铜币
};

RoleActivityExpelMonster::RoleActivityExpelMonster(int type)
	:RoleActivity(type)
{

}

RoleActivityExpelMonster::~RoleActivityExpelMonster()
{

}

void RoleActivityExpelMonster::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAExpelMonsterParam));
}

void RoleActivityExpelMonster::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RAExpelMonsterParam));
}

void RoleActivityExpelMonster::OnLoginSendInfo()
{
	this->SendExpelMonsterInfo();
}

void RoleActivityExpelMonster::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.ra_expel_monster_num = 0;
	this->SendExpelMonsterInfo();

	this->SetRoleDataChange();
}

void RoleActivityExpelMonster::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_EXPEL_MONSTER != rand_act_type)
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


	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_expel_monster_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.ra_expel_monster_num = 0;
		m_param.ra_expel_monster_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_param.ra_expel_role_level = m_role_module_mgr->GetRole()->GetLevel();

		this->SetRoleDataChange();
	}
}

void RoleActivityExpelMonster::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_EXPEL_MONSTER, true))
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(raor->rand_activity_type))
	{
		//m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}
	this->CheckInitRandActivity(raor->rand_activity_type);

	switch (raor->opera_type)
	{
	case Protocol::RA_EXPEL_MONSTER_OPERA_TYPE_BUY:
		{
			this->ExpelMonsterBuy(raor->param_1);
		}
		break;
	}
}

void RoleActivityExpelMonster::OnExpelMonster()
{
	m_param.ra_expel_monster_num += 1;
	this->SendExpelMonsterInfo();

	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityExpelMonster::SendExpelMonsterInfo()
{
	Protocol::SCRAExpelMonsterInfo info;
	info.today_times = m_param.ra_expel_monster_num;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityExpelMonster::ExpelMonsterBuy(int seq)
{
	const RandActivityExpelMonsterConfig * config = static_cast<const RandActivityExpelMonsterConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_EXPEL_MONSTER));
	if (NULL == config)
	{
		return;
	}
	const ExpelMonsterConsumeCfg * item_cfg = config->GetConsumeCfg(seq);
	if (item_cfg != NULL)
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPut(item_cfg->item.item_id, item_cfg->item.num))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
		bool has_pay = false;
		switch (item_cfg->consume_type)
		{
		case EXPEL_MONSTER_MONEY_TYPE_COIN:
			{
				if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(item_cfg->need_gold))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
					return;
				}
				else
				{
					has_pay = m_role_module_mgr->GetMoney()->UseCoinBind(item_cfg->need_gold, __FUNCTION__);
				}

			}
			break;
		case EXPEL_MONSTER_MONEY_TYPE_GOLD:
			{
				if (!m_role_module_mgr->GetMoney()->GoldMoreThan(item_cfg->need_gold))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
					return;
				}
				else
				{
					has_pay = m_role_module_mgr->GetMoney()->UseGold(item_cfg->need_gold, __FUNCTION__);
				}
			}
			break;
		default:
			return;
		}
		if (has_pay)
		{
			m_role_module_mgr->GetKnapsack()->Put(item_cfg->item, PUT_REASON_EXPEL_MONSTER_BUY_ITEM);
			gamelog::g_log_rand_activity.printf(LL_INFO, "OnRaExpelMonsterOp::buyitem player uid[%d] name[%s] [%d,%d,%d]",
				m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
				item_cfg->item.item_id, item_cfg->item.num, item_cfg->item.is_bind ? 1 : 0);
		}
	}
}

