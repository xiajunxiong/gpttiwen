#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "item/itempool.h"
#include "engineadapter.h"
#include "item/knapsack.h"
#include "config/logicconfigmanager.hpp"
#include "servercommon/errornum.h"
#include "item/itemextern.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "gamelog.h"

#include "yinyangeye.hpp"
#include "yinyangeyeconfig.hpp"
#include "protocol/msgyinyangeye.h"
#include "servercommon/string/gameworldstr.h"
#include "other/event/eventhandler.hpp"
#include "other/rolecross/rolecross.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/team/team.hpp"
#include "battle/battle_manager_local.hpp"
#include "task/taskmanager.h"

YinYangEye::YinYangEye()
{
}

YinYangEye::~YinYangEye()
{
}

void YinYangEye::Init(RoleModuleManager * mgr, const YinYangEyeParam & param)
{
	m_mgr = mgr;
	m_param = param;
}

void YinYangEye::GetInitParam(YinYangEyeParam * param)
{
	*param = m_param;
}

void YinYangEye::OnRoleLogin()
{
	this->SendInfo();
}

void YinYangEye::OnOpen()
{
	if (0 == m_param.open_mark)
	{
		m_param.open_mark = 1;
	}
	else
	{
		m_param.open_mark = 0;
	}

	this->SendInfo();
	m_mgr->GetTaskManager()->OnOpenOrCloseYinYangEye(m_param.open_mark);
}

void YinYangEye::OnFetchBox(int seq)
{
	if (0 > seq || seq >= YIN_YANG_EYE_BOX_MAXNUM)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_param.box_flag.IsBitSet(seq))
	{
		m_mgr->NoticeNum(errornum::EN_MAP_EXPLORATION_HAS_CHECK);
		return;
	}

	if (1 != m_param.open_mark)
	{
		//m_mgr->NoticeNum(errornum::EN_YIN_YANG_EYE_PLEASE_OPEN);
		//return;
	}

	const YinYangEyeBoxCfg* box_cfg = LOGIC_CONFIG->GetYinYangEyeConfig()->GetBoxCfg(seq);
	if (NULL == box_cfg)
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	//检测是否在所符合地图位置
	Scene* m_scene = m_mgr->GetRole()->GetScene();
	if (m_scene == NULL) return;

	if (m_scene->GetSceneID() != box_cfg->scene_id) return;

	if ((m_mgr->GetRole()->GetPos() - box_cfg->pos).IsLongerThan(LOGIC_CONFIG->GetYinYangEyeConfig()->GetOtherCfg().box_radius))
	{
		m_mgr->NoticeNum(errornum::EN_NPC_ROLE_IS_FAR_FROM_NPC);
		return;
	}

	//已完成的seq直接进领取逻辑（如今了战斗逻辑）
	if (seq == m_param.cur_box_finish_seq)
	{
		this->OnFetchReward(YIN_YANG_EYE_TYPE_BOX, seq);
		return;
	}

	//战斗逻辑进战斗
	if (YIN_YANG_EYE_NPC_TYPE_BAD == box_cfg->npc_type && 0 < box_cfg->monster_group_id)
	{
		/*Team * m_team = m_mgr->GetRole()->GetMyTeam();
		if (NULL != m_team)
		{
			m_mgr->NoticeNum(errornum::EN_ROLE_IN_TEAM);
			return;
		}*/

		BattleManagerLocal::GetInstance().StartBattleModeFightReq(m_mgr->GetRole(), BATTLE_MODE_YIN_YANG_EYE, box_cfg->monster_group_id, YIN_YANG_EYE_TYPE_BOX, seq, true);
		return;
	}

	const YinYangEyeRewardCfg* reward_cfg = LOGIC_CONFIG->GetYinYangEyeConfig()->GetRewardCfg(box_cfg->reward_group);
	if (NULL == reward_cfg)
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (0 < reward_cfg->reward_list.size())
	{
		if (!m_mgr->GetKnapsack()->CheckForPutList((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0]))
		{
			m_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
			return;
		}

		//给予
		if (!m_mgr->GetKnapsack()->PutList((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0], PUT_REASON_YIN_YANG_EYE))
		{
			m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}

	m_param.box_flag.SetBit(seq);
	EventHandler::Instance().OnYinYangEyeReward(m_mgr->GetRole(), 0 == box_cfg->is_always_appear);
	this->SendInfo();
}

void YinYangEye::OnNpcOp(int seq)
{
	if (0 > seq || seq >= YIN_YANG_EYE_NPC_MAXNUM)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_param.npc_flag.IsBitSet(seq)) return;

	if (1 != m_param.open_mark)
	{
		//m_mgr->NoticeNum(errornum::EN_YIN_YANG_EYE_PLEASE_OPEN);
		//return;
	}

	const YinYangEyeNpcCfg* npc_cfg = LOGIC_CONFIG->GetYinYangEyeConfig()->GetNpcCfg(seq);
	if (NULL == npc_cfg)
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	//检测是否在所符合地图位置
	Scene* m_scene = m_mgr->GetRole()->GetScene();
	if (m_scene == NULL) return;

	if (m_scene->GetSceneID() != npc_cfg->scene_id) return;

	if ((m_mgr->GetRole()->GetPos() - npc_cfg->pos).IsLongerThan(LOGIC_CONFIG->GetYinYangEyeConfig()->GetOtherCfg().box_radius))
	{
		m_mgr->NoticeNum(errornum::EN_NPC_ROLE_IS_FAR_FROM_NPC);
		return;
	}

	//已完成的seq直接进领取逻辑（如今了战斗逻辑）
	if (seq == m_param.cur_npc_finish_seq)
	{
		this->OnFetchReward(YIN_YANG_EYE_TYPE_NPC, seq);
		return;
	}

	//战斗逻辑进战斗
	if (YIN_YANG_EYE_NPC_TYPE_BAD == npc_cfg->npc_type && 0 < npc_cfg->monster_group_id)
	{
		BattleManagerLocal::GetInstance().StartBattleModeFightReq(m_mgr->GetRole(), BATTLE_MODE_YIN_YANG_EYE, npc_cfg->monster_group_id, YIN_YANG_EYE_TYPE_NPC, seq, true);
		return;
	}

	const YinYangEyeRewardCfg* reward_cfg = LOGIC_CONFIG->GetYinYangEyeConfig()->GetRewardCfg(npc_cfg->reward_group);
	if (NULL == reward_cfg)
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (0 < reward_cfg->reward_list.size())
	{
		if (!m_mgr->GetKnapsack()->CheckForPutList((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0]))
		{
			m_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
			return;
		}

		//给予
		if (!m_mgr->GetKnapsack()->PutList((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0], PUT_REASON_YIN_YANG_EYE))
		{
			m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}
	m_param.npc_flag.SetBit(seq);
	EventHandler::Instance().OnYinYangEyeReward(m_mgr->GetRole(), 0 == npc_cfg->is_always_appear);
	this->SendInfo();
}

void YinYangEye::OnBattleFinish(int type, int seq, bool is_win)
{
	if (!is_win) return;

	Team * m_team = m_mgr->GetRole()->GetMyTeam();
	if (NULL != m_team)
	{
		if (!m_team->IsLeader(m_mgr->GetRole()))
		{
			return;
		}
	}

	if (YIN_YANG_EYE_TYPE_BOX > type || type >= YIN_YANG_EYE_TYPE_END)
	{
		return;
	}

	if (YIN_YANG_EYE_TYPE_BOX == type)
	{
		if (0 > seq || seq >= YIN_YANG_EYE_BOX_MAXNUM) return;

		const YinYangEyeBoxCfg* box_cfg = LOGIC_CONFIG->GetYinYangEyeConfig()->GetBoxCfg(seq);
		if (NULL == box_cfg) return;

		const YinYangEyeRewardCfg* reward_cfg = LOGIC_CONFIG->GetYinYangEyeConfig()->GetRewardCfg(box_cfg->reward_group);
		if (NULL == reward_cfg) return;

		m_param.cur_box_finish_seq = seq;
	}
	else if (YIN_YANG_EYE_TYPE_NPC == type)
	{
		if (0 > seq || seq >= YIN_YANG_EYE_NPC_MAXNUM) return;

		const YinYangEyeNpcCfg* npc_cfg = LOGIC_CONFIG->GetYinYangEyeConfig()->GetNpcCfg(seq);
		if (NULL == npc_cfg) return;

		const YinYangEyeRewardCfg* reward_cfg = LOGIC_CONFIG->GetYinYangEyeConfig()->GetRewardCfg(npc_cfg->reward_group);
		if (NULL == reward_cfg) return;

		m_param.cur_npc_finish_seq = seq;
	}

	this->SendInfo();
}

void YinYangEye::OnFetchReward(int type, int seq)
{
	if (YIN_YANG_EYE_TYPE_BOX > type || type >= YIN_YANG_EYE_TYPE_END)
	{
		return;
	}

	if (YIN_YANG_EYE_TYPE_BOX == type)
	{
		if (0 > seq || seq >= YIN_YANG_EYE_BOX_MAXNUM) return;
		if (m_param.box_flag.IsBitSet(seq)) return;

		const YinYangEyeBoxCfg* box_cfg = LOGIC_CONFIG->GetYinYangEyeConfig()->GetBoxCfg(seq);
		if (NULL == box_cfg) return;

		const YinYangEyeRewardCfg* reward_cfg = LOGIC_CONFIG->GetYinYangEyeConfig()->GetRewardCfg(box_cfg->reward_group);
		if (NULL == reward_cfg) return;

		if (0 < reward_cfg->reward_list.size())
		{
			if (!m_mgr->GetKnapsack()->CheckForPutList((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0]))
			{
				m_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
				return;
			}

			if (!m_mgr->GetKnapsack()->PutList((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0], PUT_REASON_YIN_YANG_EYE))
			{
				m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return;
			}
		}

		m_param.box_flag.SetBit(seq);
		m_param.cur_box_finish_seq = -1;
		EventHandler::Instance().OnYinYangEyeReward(m_mgr->GetRole(), 0 == box_cfg->is_always_appear);
		this->SendInfo();
	}
	else if (YIN_YANG_EYE_TYPE_NPC == type)
	{
		if (0 > seq || seq >= YIN_YANG_EYE_NPC_MAXNUM) return;
		if (m_param.npc_flag.IsBitSet(seq)) return;

		const YinYangEyeNpcCfg* npc_cfg = LOGIC_CONFIG->GetYinYangEyeConfig()->GetNpcCfg(seq);
		if (NULL == npc_cfg) return;

		const YinYangEyeRewardCfg* reward_cfg = LOGIC_CONFIG->GetYinYangEyeConfig()->GetRewardCfg(npc_cfg->reward_group);
		if (NULL == reward_cfg) return;

		if (0 < reward_cfg->reward_list.size())
		{
			if (!m_mgr->GetKnapsack()->CheckForPutList((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0]))
			{
				m_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
				return;
			}

			if (!m_mgr->GetKnapsack()->PutList((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0], PUT_REASON_YIN_YANG_EYE))
			{
				m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return;
			}
		}

		m_param.npc_flag.SetBit(seq);
		m_param.cur_npc_finish_seq = -1;
		EventHandler::Instance().OnYinYangEyeReward(m_mgr->GetRole(), 0 == npc_cfg->is_always_appear);
		this->SendInfo();
	}
}

int YinYangEye::GetFinishRewardCount()
{
	int count = 0;
	const std::vector<YinYangEyeBoxCfg> & box_vec = LOGIC_CONFIG->GetYinYangEyeConfig()->GetBoxVecCfg();
	for (int i = 0; i < (int)box_vec.size() && i < YIN_YANG_EYE_BOX_MAXNUM; i++)
	{
		if (0 == box_vec[i].is_always_appear && m_param.box_flag.IsBitSet(i))
		{
			count++;
		}
	}

	const std::vector<YinYangEyeNpcCfg> & npc_vec = LOGIC_CONFIG->GetYinYangEyeConfig()->GetNpcVecCfg();
	for (int i = 0; i < (int)npc_vec.size() && i < YIN_YANG_EYE_NPC_MAXNUM; i++)
	{
		if (0 == npc_vec[i].is_always_appear && m_param.npc_flag.IsBitSet(i))
		{
			count++;
		}
	}

	return count;
}

void YinYangEye::SendInfo()
{
	static Protocol::SCYinYangEyeInfo info;
	info.open_mark = m_param.open_mark;
	info.ch_reserve = 0;
	info.sh_reserve = 0;
	info.cur_box_finish_seq = m_param.cur_box_finish_seq;
	info.cur_npc_finish_seq = m_param.cur_npc_finish_seq;
	info.box_flag = m_param.box_flag;
	info.npc_flag = m_param.npc_flag;

	m_mgr->NetSend(&info, sizeof(info));
}
