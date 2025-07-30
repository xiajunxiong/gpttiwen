#include "roleactivitygivesecretkey.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitygivesecretkeyconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/funopen/funopen.hpp"

#include "protocol/randactivity/msgraexpelmonster.hpp"

#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "servercommon/noticenum.h"

enum EXPEL_MONSTER_MONEY_TYPE
{
	EXPEL_MONSTER_MONEY_TYPE_GOLD = 0, // 元宝
	EXPEL_MONSTER_MONEY_TYPE_COIN = 1, // 铜币
};

RoleActivityGiveSecretKey::RoleActivityGiveSecretKey(int type)
	:RoleActivity(type)
{

}

RoleActivityGiveSecretKey::~RoleActivityGiveSecretKey()
{

}

void RoleActivityGiveSecretKey::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAGiveSecretKeyParam));
}

void RoleActivityGiveSecretKey::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RAGiveSecretKeyParam));
}

void RoleActivityGiveSecretKey::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityGiveSecretKey::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	this->SendInfo();
}

void RoleActivityGiveSecretKey::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_GIVE_SECRET_KEY != rand_act_type)
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


	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_give_secret_key_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.ra_give_secret_key_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_param.ra_reward_flag = 0;
		memset(m_param.frist_pass_flag, 0, sizeof(m_param.frist_pass_flag));
		this->SetRoleDataChange();
	}
}

void RoleActivityGiveSecretKey::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_GIVE_SECRET_KEY, true))
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
	case 1://领取奖励 param1 奖励seq
		{
			this->GetReward(raor->param_1);
		}
		break;
	}
}

void RoleActivityGiveSecretKey::SetFristPassCrystalFb(int fb_seq)
{
	if (this->HasFristPassCrystalFb(fb_seq))
	{
		return;
	}
	int index = fb_seq / 32;
	int seq = fb_seq % 32;
	m_param.frist_pass_flag[index] |= 1 << seq;
	this->SendInfo();
}

bool RoleActivityGiveSecretKey::HasFristPassCrystalFb(int fb_seq)
{
	int index = fb_seq / 32;
	int seq = fb_seq % 32;
	if ((m_param.frist_pass_flag[index] & (1 << seq)) > 0)
	{
		return true;
	}
	return false;
}

void RoleActivityGiveSecretKey::SendInfo()
{
	Protocol::SCRAGiveSecretKeyInfo info;
	info.ra_reward_flag = m_param.ra_reward_flag;
	memcpy(info.frist_pass_flag, m_param.frist_pass_flag, sizeof(m_param.frist_pass_flag));
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityGiveSecretKey::GetReward(int seq)
{
	if (seq < 0 || seq >= static_cast<int>(sizeof(int) * 8))
	{
		return;
	}
	const RandActivityGiveSecretKeyConfig * config = static_cast<const RandActivityGiveSecretKeyConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GIVE_SECRET_KEY));
	if (NULL == config)
	{
		return;
	}

	if ((m_param.ra_reward_flag & (1 << seq)) > 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_HAS_GET_REWARD);
		return;
	}

	const GiveSecretKeyCfg * item_cfg = config->GetRewardCfg(seq);
	if (item_cfg != NULL)
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPut(item_cfg->reward_item.item_id, item_cfg->reward_item.num))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}

		switch (item_cfg->type)
		{
		case GiveSecretKeyCfg::GIVE_SECRET_KEY_TYPE_LEVEL:
			{
				if (m_role_module_mgr->GetRole()->GetLevel() < item_cfg->parameter)
				{
					m_role_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_PARAM_NOT_ADV);
					return;
				}
			}
			break;
		case GiveSecretKeyCfg::GIVE_SECRET_KEY_TYPE_FB:
			{
				if (!this->HasFristPassCrystalFb(item_cfg->parameter))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_PARAM_NOT_ADV);
					return;
				}
			}
			break;
		default:
			return;
		}

		if (m_role_module_mgr->GetKnapsack()->CheckForPut(item_cfg->reward_item.item_id, item_cfg->reward_item.num, false))
		{
			if (!m_role_module_mgr->GetKnapsack()->Put(item_cfg->reward_item, PUT_REASON_GIVE_SECRET_KEY))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_NO_SPACE);
				return;
			}
		}
		else
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_NO_SPACE);
			return;
		}
		
		

		m_param.ra_reward_flag |= (1 << seq);
		this->SendInfo();
		m_role_module_mgr->NoticeNum(noticenum::NT_GIVE_SECRET_KEY_GET_REWARD_SUCC);
		this->SetRoleDataChange();
		this->OnParticipateRA(__FUNCTION__);
	}
}

