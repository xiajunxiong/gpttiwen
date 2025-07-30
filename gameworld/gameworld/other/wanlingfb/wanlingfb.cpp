#include "wanlingfb.hpp"
#include "wanlingfbconfig.hpp"
#include "speciallogicwanlingfb.hpp"

#include "protocol/msgwanlingfb.h"

#include "engineadapter.h"
#include "gamelog.h"
#include "servercommon/errornum.h"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"

#include "item/knapsack.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/levelcomplement/levelcomplement.h"
#include "battle/battle_manager_local.hpp"
#include "other/event/eventhandler.hpp"
#include "other/rolejoinactivityreward/rolejoinactivityreward.hpp"

RoleWanLingFB::RoleWanLingFB()
{
}

RoleWanLingFB::~RoleWanLingFB()
{
}

void RoleWanLingFB::Init(RoleModuleManager * module_mgr, const WanLingFBParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;
}

void RoleWanLingFB::GetInitParam(WanLingFBParam * param)
{
	*param = m_param;
}

void RoleWanLingFB::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid)
	{
		return;
	}
	m_param.day_fetch_battle_reward = 0;
	m_param.day_fetch_pass_reward = 0;

	this->SendInfo();
}

void RoleWanLingFB::OnReq(int req_type, int param_1, int param_2)
{
	switch (req_type)
	{
	case Protocol::CSWanLingFBReq::REQ_TYPE_TASK_FINISH:
		{
			this->FinishTask();
		}
		break;
	case Protocol::CSWanLingFBReq::REQ_TYPE_FB_FETCH_SET:
		{
			this->SetFBFetchFlag(param_1, param_2);
		}
		break;
	case Protocol::CSWanLingFBReq::REQ_TYPE_TASK_FIGHT:
		{
			this->TaskFight();
		}
		break;
		
	default:
		return;
	}
}

bool RoleWanLingFB::CanEnterFB(int fb_index)
{
	return true;
}

void RoleWanLingFB::FBPass(int fb_index)
{
	//配置
	const WanLingFB::OtherCfg & other_cfg
		= LOGIC_CONFIG->GetWanLingFBConfig()->GetOtherCfg();

	const WanLingFB::FB_Cfg* fb_cfg
		= LOGIC_CONFIG->GetWanLingFBConfig()->GetFBCfg(fb_index);
	if (NULL == fb_cfg)
	{
		return;
	}

	//可领
	if (other_cfg.day_pass_reward_time <= m_param.day_fetch_pass_reward)
	{
		m_module_mgr->NoticeNum(errornum::EN_WAN_LING_FB_PASS_MAX);
		return;
	}

	if (IsSetBit(m_param.fb_fetch_flag, fb_index))
	{
		return;
	}

	//记录
	m_param.day_fetch_pass_reward++;
	this->SendInfo();
	EventHandler::Instance().OnWanLingFBPass(m_module_mgr->GetRole(), fb_index);

	//奖励总存
	std::vector<ItemConfigData> temp_reward_vec;
	{
		temp_reward_vec.insert(temp_reward_vec.end(), fb_cfg->pass_reward_vec.begin(), fb_cfg->pass_reward_vec.end());
	}
	{
		for (int i = 0; i < (int)fb_cfg->pass_drop_grop_id_vec.size(); i++)
		{
			int is_broadcast = 0;

			const ItemConfigData* item_cfg
				= LOGIC_CONFIG->GetWanLingFBConfig()->GetDropItem(fb_cfg->pass_drop_grop_id_vec[i],
					m_module_mgr->GetRole()->GetProfession(), is_broadcast);

			if (NULL != item_cfg)
			{
				temp_reward_vec.push_back(*item_cfg);

				if (1 == is_broadcast)
				{
					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_wan_ling_fb_reward_notice, m_module_mgr->GetRoleName(), item_cfg->item_id);
					if (length > 0)
					{
						World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
					}
				}
			}
		}
	}

	//奖励
	
	long long real_add_exp = m_module_mgr->GetRole()->AddExp(fb_cfg->pass_exp, ADD_EXP_TYPE_WAN_LING_FB);
	m_module_mgr->GetLevelComplement()->OnAddExp(real_add_exp);
	EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_module_mgr->GetRole(), fb_cfg->pass_exp, __FUNCTION__);
	if (!temp_reward_vec.empty())
	{
		if (!m_module_mgr->GetKnapsack()->CheckForPutList((short)temp_reward_vec.size(), &temp_reward_vec[0], true))
		{
			//发邮件
			m_module_mgr->GetKnapsack()->SendMail(&temp_reward_vec[0], (short)temp_reward_vec.size(), SNED_MAIL_TYPE_DEFAULT, true);
		}
		else
		{
			m_module_mgr->GetKnapsack()->PutList((short)temp_reward_vec.size(), &temp_reward_vec[0], PUT_REASON_WAN_LING_FB);
		}

		temp_reward_vec.push_back(ItemConfigData(ItemNamespace::VIRTUAL_ITEM_EXP, false, real_add_exp));
		
		//奖励通知
		m_module_mgr->GetKnapsack()->NotifyGetItemList((short)temp_reward_vec.size(), &temp_reward_vec[0], GET_REASON_WAN_LING_FB);
	}
	//log
	gamelog::g_log_fb_other.printf(LL_INFO, "%s role[%d %s] fb_index:%d day_fetch_battle_reward:%d.",
		__FUNCTION__,
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		fb_index, m_param.day_fetch_pass_reward);
}

void RoleWanLingFB::FBBattlePass(int fb_index)
{
	//配置
	const WanLingFB::OtherCfg & other_cfg
		= LOGIC_CONFIG->GetWanLingFBConfig()->GetOtherCfg();

	const WanLingFB::FB_Cfg* fb_cfg
		= LOGIC_CONFIG->GetWanLingFBConfig()->GetFBCfg(fb_index);
	if (NULL == fb_cfg)
	{
		return;
	}
	
	//可领
	if (other_cfg.day_battle_reward_time <= m_param.day_fetch_battle_reward)
	{
		//m_module_mgr->NoticeNum(errornum::EN_WAN_LING_FB_BATTLE_MAX);
		return;
	}

	if (IsSetBit(m_param.fb_fetch_flag, fb_index))
	{
		return;
	}

	//记录
	m_param.day_fetch_battle_reward++;
	this->SendInfo();

	//奖励
	m_module_mgr->GetMoney()->AddCoinBind(fb_cfg->battle_pass_coin_bind, __FUNCTION__);

	//log
	gamelog::g_log_fb_other.printf(LL_INFO, "%s role[%d %s] fb_index:%d day_fetch_battle_reward:%d.",
		__FUNCTION__,
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		fb_index, m_param.day_fetch_battle_reward);
}

bool RoleWanLingFB::HasRewardTimes() const
{
	return m_param.day_fetch_pass_reward <
		LOGIC_CONFIG->GetWanLingFBConfig()
		->GetOtherCfg().day_pass_reward_time;
}

bool RoleWanLingFB::IsCanOneClick()
{
	return this->HasRewardTimes();
}

bool RoleWanLingFB::OneClickFinishTask(unsigned int & exp, int & coin, int & jade, std::vector<ItemConfigData>& rewards)
{
	const WanLingFB::FB_Cfg * fb_cfg = LOGIC_CONFIG->GetWanLingFBConfig()->GetFBByLevelCfg(m_module_mgr->GetRole()->GetLevel());
	if (NULL == fb_cfg) return false;

	const WanLingFB::OtherCfg & other_cfg = LOGIC_CONFIG->GetWanLingFBConfig()->GetOtherCfg();

	m_param.day_fetch_pass_reward++;
	this->SendInfo();
	EventHandler::Instance().OnWanLingFBPass(m_module_mgr->GetRole(), fb_cfg->fb_index);

	std::vector<ItemConfigData> temp_reward_vec;
	{
		temp_reward_vec.insert(temp_reward_vec.end(), fb_cfg->pass_reward_vec.begin(), fb_cfg->pass_reward_vec.end());
	}

	{
		for (int i = 0; i < (int)fb_cfg->pass_drop_grop_id_vec.size(); i++)
		{
			int is_broadcast = 0;

			const ItemConfigData* item_cfg = LOGIC_CONFIG->GetWanLingFBConfig()->GetDropItem(fb_cfg->pass_drop_grop_id_vec[i], m_module_mgr->GetRole()->GetProfession(), is_broadcast);

			if (NULL != item_cfg)
			{
				temp_reward_vec.push_back(*item_cfg);
			}
		}
	}

	//奖励
	exp += m_module_mgr->GetRole()->AddExp(fb_cfg->pass_exp, ADD_EXP_TYPE_WAN_LING_FB);
	EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_module_mgr->GetRole(), fb_cfg->pass_exp, __FUNCTION__);
	if (!temp_reward_vec.empty())
	{
		for (size_t i = 0; i < temp_reward_vec.size(); i++)
		{
			RoleJoinActivityReward::AddRewards(rewards, temp_reward_vec[i]);
		}
	}

	this->SendInfo();
	return true;
}

void RoleWanLingFB::SendInfo()
{
	Protocol::SCWanLingFBRoleInfo info;

	info.day_fetch_battle_reward = m_param.day_fetch_battle_reward;
	info.day_fetch_pass_reward = m_param.day_fetch_pass_reward;
	info.fb_fetch_flag = m_param.fb_fetch_flag;

	m_module_mgr->NetSend(&info, sizeof(info));
}

void RoleWanLingFB::SetFBFetchFlag(int fb_index, int set_type)
{
	if (0 > fb_index
		|| WanLingFB::FB_MAX <= fb_index)
	{
		return;
	}

	switch (set_type)
	{
	case WanLingFB::FB_FETCH_TYPE_CAN_FETCH:
		{
			WipeBit(m_param.fb_fetch_flag, fb_index);
		}
		break;
	case WanLingFB::FB_FETCH_TYPE_NOT_FETCH:
		{
			SetBit(m_param.fb_fetch_flag, fb_index);
		}
		break;
	default:
		return;
	}

	this->SendInfo();
}

void RoleWanLingFB::FinishTask()
{
	Role * role = m_module_mgr->GetRole();
	if (NULL == role)
	{
		return;
	}
	if (!role->IsLeader())
	{
		return;
	}

	Scene * scene = role->GetScene();
	if (NULL == scene)
	{
		return;
	}

	if (Scene::SCENE_TYPE_WAN_LING_FB != scene->GetSceneType())
	{
		return;
	}

	SpecialLogic *speial_logic = scene->GetSpecialLogic();
	if (NULL == speial_logic)
	{
		return;
	}
	SpecialLogicWanLingFB* wan_ling_fb_logic = (SpecialLogicWanLingFB *)speial_logic;

	wan_ling_fb_logic->OnTaskFinish(true);
}

void RoleWanLingFB::TaskFight()
{
	Role * role = m_module_mgr->GetRole();
	if (NULL == role)
	{
		return;
	}
	if (!role->IsLeader())
	{
		return;
	}

	Scene * scene = role->GetScene();
	if (NULL == scene)
	{
		return;
	}

	if (Scene::SCENE_TYPE_WAN_LING_FB != scene->GetSceneType())
	{
		return;
	}

	SpecialLogic *speial_logic = scene->GetSpecialLogic();
	if (NULL == speial_logic)
	{
		return;
	}
	SpecialLogicWanLingFB* wan_ling_fb_logic = (SpecialLogicWanLingFB *)speial_logic;
	
	int fb_index = wan_ling_fb_logic->GetFBIndex();
	int task_seq = wan_ling_fb_logic->GetTaskSeq();

	const WanLingFB::TaskCfg* task_cfg
		= LOGIC_CONFIG->GetWanLingFBConfig()->GetTaskCfg(fb_index, task_seq);
	if (NULL == task_cfg)
	{
		return;
	}

	int monster_group_id = 0;
	switch (task_cfg->task_type)
	{
	case WanLingFB::TASK_TYPE_BATTLE:
		{//任务参数是怪物组
			monster_group_id = task_cfg->npc_action;
		}
		break;
	default:
		return;
	}

	BattleManagerLocal::GetInstance().StartBattleModeFightReq(role, BATTLE_MODE_WAN_LING_FB,
		monster_group_id, fb_index, task_seq, false);
}
