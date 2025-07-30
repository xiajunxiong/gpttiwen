#include "petgodfb.hpp"
#include "petgodfbconfig.hpp"

#include "engineadapter.h"
#include "gamelog.h"
#include "servercommon/errornum.h"

#include "item/knapsack.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "other/funopen/funopen.hpp"

#include "config/logicconfigmanager.hpp"
#include "protocol/msgpetgodfb.hpp"

#include "other/rolecrystalfb/rolecrystalfb.h"
#include "other/levelcomplement/levelcomplement.h"

#include "global/drawmanager/drawmanager.hpp"
#include "protocol/msgdraw.h"



PetGodFB::PetGodFB()
{
	m_module_mgr = NULL;
}

PetGodFB::~PetGodFB()
{
}

void PetGodFB::Init(RoleModuleManager * module_mgr, const PetGodFBParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;
}

void PetGodFB::GetInitParam(PetGodFBParam * param)
{
	*param = m_param;
}

void PetGodFB::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid
		|| !FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_PET_GOD_FB))
	{
		return;
	}

	if (0 != m_param.day_pass_times
		|| 0 != m_param.day_buy_times)
	{
		m_param.day_buy_times = 0;
		m_param.day_pass_times = 0;
		this->SendInfo();
	}
}
void PetGodFB::OnReq(int req_type, int param_1)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_PET_GOD_FB))
	{
		return;
	}

	switch (req_type)
	{
	case Protocol::CSPetGodFBReq::REQ_TYPE_BUY_DAY_TMES:
		{
			this->BuyDayTimes();
		}
		break;
	case Protocol::CSPetGodFBReq::REQ_TYPE_BUY_AUTO_FB:
		{
			this->OnFBFinish(param_1, true);
		}
		break;
	default:
		return;
	}

}

bool PetGodFB::CanEnterFB(int fb_seq, bool is_auto)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_PET_GOD_FB))
	{
		return false;
	}

	const PetGodFbConfig::FBCfg * fb_cfg = LOGIC_CONFIG->GetPetGodFbConfig()->GetFBCfg(fb_seq);
	if (NULL == fb_cfg)
	{
		return false;
	}

	if (fb_cfg->role_level_limit > m_module_mgr->GetRole()->GetLevel())
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return false;
	}

	int fb_type_index = fb_cfg->fb_type - 1;
	if (0 > fb_type_index
		|| PET_GOD_FB_MAX_TYPE_NUM <= fb_type_index)
	{
		return false;
	}

	//通关上一层
	if (is_auto)
	{
		if (fb_cfg->fb_level > m_param.fb_info_list[fb_type_index].max_history_pass_level)
		{
			m_module_mgr->GetRole()->NoticeNum(errornum::EN_DREAM_NOTES_CHALLENGE_NOT_FAST_PASS);
			return false;
		}

		if (fb_cfg->auto_limit_cap > m_module_mgr->GetRole()->GetCapabilityWithFightPetAndPartner())
		{
			m_module_mgr->GetRole()->NoticeNum(errornum::EN_CONDITIONS_NOT_REACHED);
			return false;
		}
	}
	else
	{
		if (fb_cfg->fb_level > m_param.fb_info_list[fb_type_index].max_history_pass_level + 1)
		{
			m_module_mgr->GetRole()->NoticeNum(errornum::EN_PET_GOD_FB_NOT_PASS);
			return false;
		}
	}


	//次数不足
	const int &day_base_pase_time = LOGIC_CONFIG->GetPetGodFbConfig()->GetOtherCfg().day_base_times;
	if (0 <= day_base_pase_time
		&& m_param.day_pass_times >= day_base_pase_time + m_param.day_buy_times)
	{
		return false;
	}

	if (m_module_mgr->GetRoleCrystalFb()->GetCrystalFbEnergy() < fb_cfg->cost_power)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_ROLE_CRYSTAL_FB_ENERGY_NOT_ENOUGH);
		return false;
	}

	
	return true;
}

void PetGodFB::OnFBFinish(int fb_seq, bool is_auto)
{
	if (!this->CanEnterFB(fb_seq, is_auto))
	{
		return;
	}
	int role_level = m_module_mgr->GetRole()->GetLevel();
	const PetGodFbConfig::FBCfg * fb_cfg = LOGIC_CONFIG->GetPetGodFbConfig()->GetFBCfg(fb_seq);
	if (NULL == fb_cfg)
	{
		return;
	}
	std::vector<ItemConfigData> out_item_list;
	if (!LOGIC_CONFIG->GetPetGodFbConfig()
		->GetFBRewardData(fb_seq, role_level, out_item_list))
	{
		return;
	}

	//扣体力
	if (!m_module_mgr->GetRoleCrystalFb()->UseCrystalFbEnergy(fb_cfg->cost_power, __FUNCTION__, ENTER_ENERGY_CHANGE_TYPE_PET_GOD))
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_ROLE_CRYSTAL_FB_ENERGY_NOT_ENOUGH);
		return;
	}

	//翻牌
	{
		UNSTD_STATIC_CHECK(PET_GOD_FB_DRAW_NUM <= DRAW_GROUP_MAX_ITEM_NUM);
		DrawGroupInfo draw_group_info;

		for (int index = 0;
			index < PET_GOD_FB_DRAW_NUM && index < (int)out_item_list.size();
			index++)
		{
			draw_group_info.draw_count++;

			//无人获得的奖励 拥有者为0
			int role_uid = 0;

			DrawGroupInfo::DrawItem &reward_info = draw_group_info.draw_info_list[index];

			if (index < fb_cfg->draw_num)
			{
				role_uid = m_module_mgr->GetUid();
			}

			reward_info.role_uid = role_uid;

			DrawManager::Instance().PutRewardItem(reward_info, 1, &out_item_list[index], m_module_mgr->GetRole()->GetAppearanceProf());
		}
		//洗牌
		DrawManager::Instance().ShuffDrawGroupByUid(draw_group_info);
		DrawManager::Instance().AddDrawGroup(draw_group_info, is_auto ? DRAW_GROUP_TYPE_PET_GOD_FB_AUTO : DRAW_GROUP_TYPE_PET_GOD_FB);

		Protocol::SCDrawPokerInfo info;

		info.draw_group_id = draw_group_info.draw_group_id;
		info.timeout = draw_group_info.out_time;
		info.draw_group_type = draw_group_info.draw_group_type;

		for (int k = 0; k < SIDE_MAX_ROLE_NUM; k++)
		{
			DrawGroupInfo::DrawItem &reward_info = draw_group_info.draw_info_list[k];
			DrawCrystalPoker * draw_poker = (DrawCrystalPoker *)info.draw_poker[k].data;

			draw_poker->role_uid = reward_info.role_uid;
			draw_poker->index = reward_info.draw_index;
			if (0 != draw_poker->role_uid)
			{
				m_module_mgr->GetRole()->GetName(draw_poker->role_name);
			}


			if (!reward_info.item_list.empty())
			{
				DrawManager::Instance().GetItemInfoType(draw_poker->reward_info, reward_info.item_list[0]);
			}
		}

		EngineAdapter::Instance().NetSendSerializeMsg2(m_module_mgr->GetRole()->GetNetId(), &info);

	}

	long long add_exp = LOGIC_CONFIG->GetPetGodFbConfig()->GetRewardExp(role_level) * fb_cfg->cost_power;
	if (0 < add_exp)
	{
		long long real_add_exp = m_module_mgr->GetRole()->AddExp(add_exp, ADD_EXP_TYPE_PET_GOD_FB, __FUNCTION__);
		m_module_mgr->GetLevelComplement()->OnAddExp(real_add_exp);
		EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_module_mgr->GetRole(), add_exp, __FUNCTION__);
	}

	//历史最高层
	{
		int fb_type_index = fb_cfg->fb_type - 1;
		if (0 <= fb_type_index
			&& PET_GOD_FB_MAX_TYPE_NUM > fb_type_index)
		{
			if (fb_cfg->fb_level > m_param.fb_info_list[fb_type_index].max_history_pass_level)
			{
				m_param.fb_info_list[fb_type_index].max_history_pass_level = fb_cfg->fb_level;

				this->SendInfo();
			}
		}
	}

	EventHandler::Instance().OnPassPetGodFB(m_module_mgr->GetRole(), fb_seq);

	gamelog::g_log_fb_other.printf(LL_INFO, "%s role[%d %s] is_auto:%d fb_seq:%d fb_type:%d fb_level:%d.",
		__FUNCTION__,
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		is_auto ? 1 : 0,
		fb_seq, fb_cfg->fb_type, fb_cfg->fb_level);

}

void PetGodFB::BuyDayTimes()
{
	const int &day_base_pase_time = LOGIC_CONFIG->GetPetGodFbConfig()->GetOtherCfg().day_base_times;
	if (0 > day_base_pase_time)
	{
		return;
	}

	const PetGodFbConfig::BuyTimeCfg * buy_time_cfg = LOGIC_CONFIG->GetPetGodFbConfig()->GetBuyTimeCfg(m_param.day_buy_times + 1);
	if (NULL == buy_time_cfg)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_MATERIAL_LEVEL_BUY_NOT_ENOUGH);
		return;
	}

	//购买
	if (!m_module_mgr->GetMoney()->UseGold(buy_time_cfg->cost_gold, __FUNCTION__))
	{
		return;
	}

	m_param.day_buy_times++;

	gamelog::g_log_fb_other.printf(LL_INFO, "%s role[%d %s] day_buy_times:%d.",
		__FUNCTION__, 
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		m_param.day_buy_times);

	this->SendInfo();
}

void PetGodFB::SendInfo()
{
	Protocol::SCPetGodFBRoleInfo info;
	
	for (int i = 0; i < PET_GOD_FB_MAX_TYPE_NUM; i++)
	{
		info.max_history_pass_level[i] = m_param.fb_info_list[i].max_history_pass_level;
	}

	m_module_mgr->NetSend(&info, sizeof(info));
}

