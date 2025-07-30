#include "yaoxingshilian.hpp"
#include "yaoxingshilianconfig.hpp"
#include "other/funopen/funopen.hpp"
#include "other/event/eventhandler.hpp"
#include "gameworld/config/logicconfigmanager.hpp"
#include "battle/battle_business_local.hpp"
#include "gameworld/battle/battle_manager_local.hpp"
#include "protocol/msgother.h"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "global/team/team.hpp"
#include "gamelog.h"
#include "scene/scene.h"
#include "scene/speciallogic.hpp"
#include "other/capability/capability.hpp"
#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "servercommon/uniqueidgenerator.hpp"

YaoXingShiLian::YaoXingShiLian()
{
}

YaoXingShiLian::~YaoXingShiLian()
{
}

void YaoXingShiLian::Init(RoleModuleManager* module_mgr, const YaoXingShiLianParam &param)
{
	m_module_mgr = module_mgr;
	m_param = param;
}

void YaoXingShiLian::GetInitParam(YaoXingShiLianParam * param)
{
	*param = m_param;
}

void YaoXingShiLian::OnRoleLogin()
{
	this->SendInfo();
}

void YaoXingShiLian::OnBattleFinishWin(bool is_win)
{
	//发放通关奖励
	if (is_win)
	{
		
		const YaoXingShiLianLevelCfg * level_cfg = LOGIC_CONFIG->GetYaoXingShiLianConfig()->GetLevelCfg(m_param.pass_battle_seq + 1);
		if (NULL != level_cfg)
		{
			if (level_cfg->coin > 0)m_module_mgr->GetMoney()->AddCoinBind(level_cfg->coin, __FUNCTION__);

			if (level_cfg->reward_vec.size() > 0)
			{
				if (!m_module_mgr->GetKnapsack()->PutList((short)level_cfg->reward_vec.size(), &level_cfg->reward_vec[0], PUT_REASON_YAO_XING_SHI_LIAN))
				{
					m_module_mgr->GetKnapsack()->SendMail(level_cfg->reward_vec);
				}
			}
		}
		m_param.pass_battle_seq++;
		this->SendInfo();
		EventHandler::Instance().OnPassYaoXingShiLian(m_module_mgr->GetRole(), m_param.pass_battle_seq);
	}
}

void YaoXingShiLian::OnStartBattle()
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_YAO_XING_SHI_LIAN))return;
	if (!m_module_mgr->GetRole()->GetScene()->GetSpecialLogic()->CanJoinOtherActivity())
	{
		m_module_mgr->NoticeNum(errornum::EN_PLAY_ACTIVITY_NOW_ERR);
		return;
	}

	if (m_module_mgr->GetRole()->InTeam())
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return;
	}

	//检查这一关卡是否存在
	const YaoXingShiLianLevelCfg * monster_cfg = LOGIC_CONFIG->GetYaoXingShiLianConfig()->GetLevelCfg(m_param.pass_battle_seq + 1);
	if (NULL == monster_cfg)return;

	if (monster_cfg->level_limit > m_module_mgr->GetRole()->GetLevel())
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return;
	}

	int monster_id = m_module_mgr->GetCapability()->GetRoleCapability() < monster_cfg->score ? monster_cfg->monster_one : monster_cfg->monster_two;

	if (BattleManagerLocal::GetInstance().StartBattleModeFightReq(m_module_mgr->GetRole(), BATTLE_MODE_YAO_XING_SHI_LIAN, monster_id, 0, 0, true))
	{
		return;
	}
}

void YaoXingShiLian::GetReward(int seq)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_YAO_XING_SHI_LIAN))return;
	
	const YaoXingShiLianRewardCfg * cfg = LOGIC_CONFIG->GetYaoXingShiLianConfig()->GetRewardCfg(seq);
	if (cfg != NULL)
	{
		if (cfg->num > m_param.pass_battle_seq)
		{
			m_module_mgr->NoticeNum(errornum::EN_YAO_XING_SHI_LIAN_PASS_ERR);
			return;
		}

		if (m_param.m_reward_bit_map.IsBitSet(seq))
		{
			m_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_HAS_GET_REWARD);
			return;
		}

		if (!m_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->reward_vec.size(), &cfg->reward_vec[0]))
		{
			m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
			return;
		}

		if (cfg->achievement != YAO_XING_ACH_TYPE_NO)
		{
			switch (cfg->achievement)
			{
			case YAO_XING_ACH_TYPE_1:
				{
					if (cfg->param2 > 0 && !m_module_mgr->GetMoney()->UseMoneyType(cfg->param1, cfg->param2, __FUNCTION__))
					{
						return;
					}
				}
				break;
			case YAO_XING_ACH_TYPE_2:
				{
					if (cfg->param1 > 0 && m_module_mgr->GetRole()->GetLevel() >= cfg->param1)
					{
						m_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
						return;
					}
				}
				break;
			case YAO_XING_ACH_TYPE_3:
				{
					if (cfg->param1 > 0 && m_module_mgr->GetCapability()->GetRoleCapability() >= cfg->param1)
					{
						m_module_mgr->NoticeNum(errornum::EN_YAO_XING_SHI_LIAN_CAP_NOT_ENOUGH);
						return;
					}
				}
				break;
			case YAO_XING_ACH_TYPE_4:
				{
					if (cfg->param1 > 0 && !m_param.m_cmd_map.IsBitSet(cfg->param1))
					{
						m_module_mgr->NoticeNum(errornum::EN_CONDITIONS_NOT_REACHED);
						return;
					}
				}
				break;
			}
		}

		m_module_mgr->GetKnapsack()->PutList((short)cfg->reward_vec.size(), &cfg->reward_vec[0], PUT_REASON_YAO_XING_SHI_LIAN);
		m_param.m_reward_bit_map.SetBit(seq);
		this->SendInfo();
	}
}

void YaoXingShiLian::GetReward2(int seq)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_YAO_XING_SHI_LIAN))return;
	
	const YaoXingShiLianReward2Cfg * cfg = LOGIC_CONFIG->GetYaoXingShiLianConfig()->GetReward2Cfg(seq);
	if (cfg != NULL)
	{
		if (cfg->num > m_param.pass_battle_seq)
		{
			m_module_mgr->NoticeNum(errornum::EN_YAO_XING_SHI_LIAN_PASS_ERR);
			return;
		}

		if (cfg->achievement > 0)
		{
			int reward_1_count = 0;
			for (int i = 1; i <= cfg->achievement && i < MAX_YAOXINGSHILIAN_REWARD_LIST; i++)
			{
				if (m_param.m_reward_bit_map.IsBitSet(i))
				{
					reward_1_count += 1;
				}
			}

			if (cfg->achievement > reward_1_count)
			{
				m_module_mgr->NoticeNum(errornum::EN_YAO_XING_SHI_LIAN_PASS_ERR);
				return;
			}
		}

		if (cfg->achievement_2 != YAO_XING_ACH_TYPE_NO)
		{
			switch (cfg->achievement_2)
			{
			case YAO_XING_ACH_TYPE_1:
				{
					if (cfg->param2 > 0 && !m_module_mgr->GetMoney()->UseMoneyType(cfg->param1, cfg->param2, __FUNCTION__))
					{
						return;
					}
				}
				break;
			case YAO_XING_ACH_TYPE_2:
				{
					if (cfg->param1 > 0 && m_module_mgr->GetRole()->GetLevel() >= cfg->param1)
					{
						m_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
						return;
					}
				}
				break;
			case YAO_XING_ACH_TYPE_3:
				{
					if (cfg->param1 > 0 && m_module_mgr->GetCapability()->GetRoleCapability() >= cfg->param1)
					{
						m_module_mgr->NoticeNum(errornum::EN_YAO_XING_SHI_LIAN_CAP_NOT_ENOUGH);
						return;
					}
				}
				break;
			case YAO_XING_ACH_TYPE_4:
				{
					if (cfg->param1 > 0 && !m_param.m_cmd_map.IsBitSet(cfg->param1))
					{
						m_module_mgr->NoticeNum(errornum::EN_CONDITIONS_NOT_REACHED);
						return;
					}
				}
				break;
			}
		}

		if (m_param.m_reward_bit_map2.IsBitSet(seq))
		{
			m_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_HAS_GET_REWARD);
			return;
		}

		if (!m_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->reward_vec.size(), &cfg->reward_vec[0]))
		{
			m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
			return;
		}

		m_module_mgr->GetKnapsack()->PutList((short)cfg->reward_vec.size(), &cfg->reward_vec[0], PUT_REASON_YAO_XING_SHI_LIAN);

		m_param.m_reward_bit_map2.SetBit(seq);
		this->SendInfo();
	}
}

void YaoXingShiLian::OnBuyItemCfgCheck(int param1, int param2)
{
	const YaoXingShiLianReward2Cfg * reward_cfg = LOGIC_CONFIG->GetYaoXingShiLianConfig()->GetReward2Cfg(param1);
	if (NULL == reward_cfg)return;

	const YaoXingShiLianOtherCfg & other_cfg = LOGIC_CONFIG->GetYaoXingShiLianConfig()->GetOhterCfg();

	if (reward_cfg->param1 != param2)return;//检查是否正确标记

	if (m_param.m_cmd_map.IsBitSet(param2))
	{
		return;
	}

	RoleCommonSave * rcs = m_module_mgr->GetRoleCommonSaveManager()->GetRoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_YAO_XING_SHI_LIAN);
	if (rcs == NULL)
	{
		return;
	}
	RoleCommonSaveCmdBuyItem * buy_item = rcs->GetTypeData<RoleCommonSaveCmdBuyItem>(0);
	if (buy_item == NULL)
	{
		return;
	}
	time_t now = EngineAdapter::Instance().Time();
	if (now < (time_t)buy_item->create_time + CMD_BUY_ITEM_WAIT_TIME)
	{
		m_module_mgr->NoticeNum(errornum::EN_HAS_ORDER);

		return;
	}
	long long unique_key = 0;
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		return;
	}
	if (buy_item->create_time != 0)
	{
		std::string log_str1 = ::GetString(*buy_item);
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), log_str1.c_str());
	}

	buy_item->cfg_vesion = 0;
	buy_item->need_gold = other_cfg.rmb;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = 0;
	buy_item->unique_id = unique_key;
	buy_item->param1 = param1;
	buy_item->param2 = param2;
	rcs->SetDataChange(0);
	m_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), log_str2.c_str());

	Protocol::SCCmdBuyItemCheckRet info;
	info.buy_type = BUY_TIME_TYPE_YAO_XING_SHI_LIAN;
	info.cfg_ver = buy_item->cfg_vesion;
	info.unique_id = unique_key;

	m_module_mgr->NetSend(&info, sizeof(info));
	return;
}

int YaoXingShiLian::OnBuyGiftItem(int type,int seq, int ARG_OUT * price)
{
	const YaoXingShiLianReward2Cfg * reward_cfg = LOGIC_CONFIG->GetYaoXingShiLianConfig()->GetReward2Cfg(type);
	if (NULL == reward_cfg)
	{
		return 1;
	}

	const YaoXingShiLianOtherCfg & other_cfg = LOGIC_CONFIG->GetYaoXingShiLianConfig()->GetOhterCfg();

	if (reward_cfg->param1 != seq) 
	{
		return 1;
	}

	if (m_param.m_cmd_map.IsBitSet(seq))
	{
		return 2;
	}

	if (price)
	{
		*price = other_cfg.rmb;
		return 0;
	}

	m_param.m_cmd_map.SetBit(seq);
	this->SendInfo();

	return 0;
}

void YaoXingShiLian::SendInfo()
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_YAO_XING_SHI_LIAN))return;
	Protocol::SCYaoXingShiLianInfo protocol;
	
	protocol.pass_battle_seq = m_param.pass_battle_seq;
	protocol.m_reward_bit_map = m_param.m_reward_bit_map;
	protocol.m_reward_bit_map2 = m_param.m_reward_bit_map2;
	protocol.m_cmd_map = m_param.m_cmd_map;;
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), &protocol, sizeof(protocol));
}

int YaoXingShiLian::OnCmd(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_module_mgr->GetRole(), ROLE_COMMON_SAVE_DATA_TYPE_YAO_XING_SHI_LIAN, unique_id);
	if (NULL == buy_item)
	{
		return -__LINE__;
	}
	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (buy_item->need_gold != chong_zhi_gold)
	{
		return -__LINE__;
	}

	m_param.m_cmd_map.SetBit(1);
	this->SendInfo();
	return 0;
}

void YaoXingShiLian::GmSetParam(int type, int param)
{
	switch (type)
	{
	case 1:
		m_param.pass_battle_seq = param;
		break;
	default:
		break;
	}
	this->SendInfo();
}
