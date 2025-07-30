#include "lunhuiwangchuan.hpp"
#include "other/funopen/funopen.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "config/sharedconfig/sharedconfig.h"
#include "global/cross/crossmanager.h"
#include "gameworld/gameworld/engineadapter.h"
#include "global/worldstatus/worldstatus.hpp"
#include "gameworld/config/logicconfigmanager.hpp"
#include "other/lunhuiwangchuan/lunhuiwangchuanconfig.hpp"
#include "battle/battle_business_local.hpp"
#include "gameworld/battle/battle_manager_local.hpp"
#include "protocol/msgother.h"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "global/rank/rankmanager.hpp"
#include "scene/scene.h"
#include "scene/speciallogic.hpp"
#include "global/team/team.hpp"
#include "other/fabao/fabao.hpp"
#include "task/taskmanager.h"
#include "gamelog.h"

LunHuiWangChuan::LunHuiWangChuan()
{
}

LunHuiWangChuan::~LunHuiWangChuan()
{
}

void LunHuiWangChuan::Init(RoleModuleManager * module_mgr, const LunHuiWangChuanParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;
}

void LunHuiWangChuan::GetInitParam(LunHuiWangChuanParam * param)
{
	*param = m_param;
}

void LunHuiWangChuan::OnRoleLogin()
{
	this->OnPassSeqChange();		//登录时检测一下关卡数能不能激活法宝抽奖炉子
}

void LunHuiWangChuan::Update(time_t now_second)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_LUN_HUI_WANG_CHUAN))return;

	//检查刷新
	unsigned int  refresh_tamp = WorldStatus::Instance().GetLunHuiWangChuanRefreshTamp();
	
	if (refresh_tamp > 0)
	{
		//检查  玩家刷新时间点 !=  服务器刷新时间点
		const LunHuiWangChuanOtherCfg & other_cfg = LOGIC_CONFIG->GetLunHuiWangChuanConfig()->GetLunHuiWangChuanOhterCfg();
		if (other_cfg.refresh_day > 0)
		{
			if (m_param.previous_refresh_tamp < refresh_tamp)
			{
				m_param.previous_refresh_tamp = now_second;//是乱的玩家立刻刷新轮回忘川
				m_param.fight_refresh_tamp = m_param.previous_refresh_tamp;
				gamelog::g_lun_hui_wang_chuan.printf(LL_INFO, "LunHuiWangChuan::Refresh m_param.previous_refresh_tamp != refresh_tamp Data user[%d,%s] refresh_tamp[%u] m_param.previous_refresh_tamp[%u] m_param.fight_refresh_tamp[%u]",
					m_module_mgr->GetUid(), m_module_mgr->GetRole()->GetName(), refresh_tamp, m_param.previous_refresh_tamp, m_param.fight_refresh_tamp);
			}
		}
	}
	if (refresh_tamp > 0 && m_param.previous_refresh_tamp < refresh_tamp)
	{
		//把没有领取的奖励发放
		for (int i = 0; i < MAX_LUN_HUI_WANG_CHUAN_INDEX;i++)
		{
			const LunHuiWangChuanRewardCfg * reward_cfg = LOGIC_CONFIG->GetLunHuiWangChuanConfig()->GetLunHuiWangChuanRewardCfg(m_param.start_level, i);
			if (NULL != reward_cfg)
			{
				if (!m_param.pass_list.IsBitSet(reward_cfg->layer))continue;
				if (m_param.reward_list.IsBitSet(reward_cfg->layer))continue;
				if (!reward_cfg->is_value)continue;
				static MailContentParam contentparam; contentparam.Reset();
				for (int i = 0; i < (short)reward_cfg->reward_list.size() && i < MAX_ATTACHMENT_ITEM_NUM; i++)
				{
					const ItemBase * item_base = ITEMPOOL->GetItem(reward_cfg->reward_list[i].item_id);
					if (item_base == NULL)continue;
					contentparam.item_list[i].item_id = reward_cfg->reward_list[i].item_id;
					contentparam.item_list[i].num = reward_cfg->reward_list[i].num;
					contentparam.item_list[i].is_bind = reward_cfg->reward_list[i].is_bind ? 1 : 0;
					contentparam.item_list[i].invalid_time = item_base->CalInvalidTime();
				}
				SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_lun_hui_wang_chuan_subject);
				SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_lun_hui_wang_chuan_content);
				MailRoute::MailToUser(m_module_mgr->GetRole()->GetUID(), MAIL_REASON_DEFAULT, contentparam);

				m_param.reward_list.SetBit(reward_cfg->layer);
			}
		}
		gamelog::g_lun_hui_wang_chuan.printf(LL_INFO, "LunHuiWangChuan::Refresh Data user[%d,%s] refresh_tamp[%d] m_param.previous_refresh_tamp[%d]", m_module_mgr->GetUid(), m_module_mgr->GetRole()->GetName(), refresh_tamp, m_param.previous_refresh_tamp);
		m_param.Refresh();
		m_param.previous_refresh_tamp = refresh_tamp;
		m_param.start_level = m_module_mgr->GetRole()->GetLevel();
		m_param.set_seq = 1;
		m_module_mgr->GetTaskManager()->OnPassLunHuiWangChuan();
		const LunHuiWangChuanMonsterGroupCfg * monster_cfg = LOGIC_CONFIG->GetLunHuiWangChuanConfig()->GetLunHuiWangChuanMonsterDetailCfg(m_param.set_seq);
		if (NULL != monster_cfg)
		{
			bool monster_idx = (rand() % 2) == 1 ? true : false;
			m_param.battle_monster_id = monster_idx ? monster_cfg->monster_group_id_1 : monster_cfg->monster_group_id_2;
		}
		this->SendInfo();
	}
}

void LunHuiWangChuan::OnDayChange(int old_day, int now_day)
{
	//把没有领取的奖励发放
	for (int i = 0; i < MAX_LUN_HUI_WANG_CHUAN_INDEX; i++)
	{
		const LunHuiWangChuanRewardCfg * reward_cfg = LOGIC_CONFIG->GetLunHuiWangChuanConfig()->GetLunHuiWangChuanRewardCfg(m_param.start_level, i);
		if (NULL != reward_cfg)
		{
			if (!m_param.pass_list.IsBitSet(reward_cfg->layer))continue;
			if (m_param.reward_list.IsBitSet(reward_cfg->layer))continue;
			if (!reward_cfg->is_value)continue;
			static MailContentParam contentparam; contentparam.Reset();
			for (int i = 0; i < (short)reward_cfg->reward_list.size() && i < MAX_ATTACHMENT_ITEM_NUM; i++)
			{
				const ItemBase * item_base = ITEMPOOL->GetItem(reward_cfg->reward_list[i].item_id);
				if(item_base == NULL)continue;
				contentparam.item_list[i].item_id = reward_cfg->reward_list[i].item_id;
				contentparam.item_list[i].num = reward_cfg->reward_list[i].num;
				contentparam.item_list[i].is_bind = reward_cfg->reward_list[i].is_bind ? 1 : 0;
				contentparam.item_list[i].invalid_time = item_base->CalInvalidTime();
			}
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_lun_hui_wang_chuan_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_lun_hui_wang_chuan_content);
			MailRoute::MailToUser(m_module_mgr->GetRole()->GetUID(), MAIL_REASON_DEFAULT, contentparam);

			m_param.reward_list.SetBit(reward_cfg->layer);
		}
	}
}

void LunHuiWangChuan::OnBattleFinishWin(bool is_win)
{
	//发放通关奖励
	if (is_win)
	{
		bool special_battle = false;
		m_param.pass_times++;
		if (m_param.fight_refresh_tamp < m_param.previous_refresh_tamp)//刷新前打的，发奖励
		{
			const LunHuiWangChuanMonsterGroupCfg * monster_cfg = LOGIC_CONFIG->GetLunHuiWangChuanConfig()->GetLunHuiWangChuanMonsterDetailCfg(m_param.fight_seq);
			if (NULL != monster_cfg)
			{
				const LunHuiWangChuanPassRewardCfg * cfg = LOGIC_CONFIG->GetLunHuiWangChuanConfig()->GetLunHuiWangChuanPassRewardCfg(monster_cfg->challenge_id, m_param.fight_level);
				if (NULL != cfg)
				{
					if ((short)cfg->reward_list.size() > 0 && !m_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->reward_list.size(), &cfg->reward_list[0]))
					{
						m_module_mgr->GetKnapsack()->SendMail(&cfg->reward_list[0], (short)cfg->reward_list.size(), SEND_MAIL_TYPE_LUN_HUI_WANG_CHUAN);
					}
					else
					{
						m_module_mgr->GetKnapsack()->PutList((short)cfg->reward_list.size(), &cfg->reward_list[0], PUT_REASON_LUN_HUI_WANG_CHUAN);
					}
					
					special_battle = true;
				}
			}
			m_param.OnFightFinish();
			return;
		}

		if (!special_battle && m_param.fight_seq == m_param.set_seq)
		{
			const LunHuiWangChuanMonsterGroupCfg * monster_cfg = LOGIC_CONFIG->GetLunHuiWangChuanConfig()->GetLunHuiWangChuanMonsterDetailCfg(m_param.fight_seq);
			if (NULL != monster_cfg)
			{
				const LunHuiWangChuanPassRewardCfg * cfg = LOGIC_CONFIG->GetLunHuiWangChuanConfig()->GetLunHuiWangChuanPassRewardCfg(monster_cfg->challenge_id, m_param.fight_level);
				if (NULL != cfg)
				{
					if (m_param.pass_list.IsBitSet(cfg->challenge_id))
					{
						m_module_mgr->NoticeNum(errornum::EN_LUNHUIWANGCHUAN_REWARD_HAS_GET);
						return;
					}

					if ((short)cfg->reward_list.size() > 0 && !m_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->reward_list.size(), &cfg->reward_list[0]))
					{
						m_module_mgr->GetKnapsack()->SendMail(&cfg->reward_list[0], (short)cfg->reward_list.size(), SEND_MAIL_TYPE_LUN_HUI_WANG_CHUAN);
					}
					else
					{
						m_module_mgr->GetKnapsack()->PutList((short)cfg->reward_list.size(), &cfg->reward_list[0], PUT_REASON_LUN_HUI_WANG_CHUAN);
					}

					m_param.pass_list.SetBit(m_param.set_seq);
				}
			}
		}
		RankManager::Instance().SyncPersonRankInfo(m_module_mgr->GetRole(), PERSON_RANK_TYPE_LUN_HUI_WANG_CHUAN_RANK);
		m_param.OnFightFinish();
		m_param.set_seq += 1;
		this->OnPassSeqChange();
		//预先记录下一次的怪物信息
		this->OnPassSeqChange();
		const LunHuiWangChuanMonsterGroupCfg * monster_cfg = LOGIC_CONFIG->GetLunHuiWangChuanConfig()->GetLunHuiWangChuanMonsterDetailCfg(m_param.set_seq);
		if (NULL != monster_cfg)
		{
			bool monster_idx = (rand() % 2) == 1 ? true : false;
			m_param.battle_monster_id = monster_idx ? monster_cfg->monster_group_id_1 : monster_cfg->monster_group_id_2;
		}
		this->SendInfo();

		m_module_mgr->GetRole()->LogActive(LOG_ACTIVE_TYPE_LUN_HUI_WANG_CHUAN);
	}
}

void LunHuiWangChuan::OnPassSeqChange()
{
	m_module_mgr->GetFaBao()->OnLunHuiWangChuangChange(m_param.set_seq - 1);
}

void LunHuiWangChuan::OnStartBattle()
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_LUN_HUI_WANG_CHUAN))return;
	if (!m_module_mgr->GetRole()->GetScene()->GetSpecialLogic()->CanJoinOtherActivity())
	{
		m_module_mgr->NoticeNum(errornum::EN_PLAY_ACTIVITY_NOW_ERR);
		return;
	}

	if (m_module_mgr->GetRole()->InTeam() && m_module_mgr->GetRole()->GetMyTeam()->IsRoleMemberFollow(m_module_mgr->GetUid()))
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return;
	}

	if (m_param.pass_list.IsBitSet(m_param.set_seq))
	{
		m_module_mgr->NoticeNum(errornum::EN_LUNHUIWANGCHUAN_HAS_PASS_GET);
		return;
	}

	//if (m_param.fight_seq != 0)
	//{
	//	gamelog::g_log_debug.printf(LL_WARNING, "LunHuiWangChuan::OnStartBattle IsInBattle role[%d,%s] fight_seq[%d] fight_level[%d] battle_tamp[%u]",
	//		m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), m_param.fight_seq, m_param.start_level, m_param.battle_tamp);
	//	m_module_mgr->NoticeNum(errornum::EN_ROLE_IN_BATTLE);
	//	return;
	//}

	//检查这一关卡是否存在
	const LunHuiWangChuanMonsterGroupCfg * monster_cfg = LOGIC_CONFIG->GetLunHuiWangChuanConfig()->GetLunHuiWangChuanMonsterDetailCfg(m_param.set_seq);
	if (NULL == monster_cfg)return;

	if (BattleManagerLocal::GetInstance().StartBattleModeFightReq(m_module_mgr->GetRole(), BATTLE_MODE_LUN_HUI_WANG_CHUAN, m_param.battle_monster_id, 0, 0, true))
	{
		m_param.battle_tamp = (unsigned int)EngineAdapter::Instance().Time();
		m_param.fight_level = m_param.start_level;
		m_param.fight_seq = m_param.set_seq;
		m_param.fight_refresh_tamp = m_param.previous_refresh_tamp;
	}
}

void LunHuiWangChuan::GetReward(int reward_seq)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_LUN_HUI_WANG_CHUAN))return;
	const LunHuiWangChuanRewardCfg * cfg = LOGIC_CONFIG->GetLunHuiWangChuanConfig()->GetLunHuiWangChuanRewardCfg(m_param.start_level, reward_seq);
	if (cfg != NULL)
	{
		if (!m_param.pass_list.IsBitSet(cfg->layer))
		{
			m_module_mgr->NoticeNum(errornum::EN_LUNHUIWANGCHUAN_NOT_PASS);
			return;
		}

		if (m_param.reward_list.IsBitSet(cfg->layer))
		{
			m_module_mgr->NoticeNum(errornum::EN_LUNHUIWANGCHUAN_REWARD_HAS_GET);
			return;
		}

		if (!m_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->reward_list.size(), &cfg->reward_list[0]))
		{
			m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
			return;
		}

		m_module_mgr->GetKnapsack()->PutList((short)cfg->reward_list.size(), &cfg->reward_list[0], PUT_REASON_LUN_HUI_WANG_CHUAN);

		m_param.reward_list.SetBit(cfg->layer);
		this->SendInfo();
	}
}

void LunHuiWangChuan::RefreshNow()
{
	m_param.previous_refresh_tamp = EngineAdapter::Instance().Time();
	gamelog::g_lun_hui_wang_chuan.printf(LL_INFO, "LunHuiWangChuan::RefreshNow Data user[%d,%s] m_param.previous_refresh_tamp[%d]", m_module_mgr->GetUid(), m_module_mgr->GetRole()->GetName(), m_param.previous_refresh_tamp);
}

void LunHuiWangChuan::SendInfo()
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_LUN_HUI_WANG_CHUAN))return;
	Protocol::SCLunHuiWangChuanInfo protocol;
	
	protocol.start_level = m_param.start_level;
	protocol.battle_monster_id = m_param.battle_monster_id;
	protocol.previous_refresh_tamp = WorldStatus::Instance().GetLunHuiWangChuanRefreshTamp();
	protocol.pass_list = m_param.pass_list;
	protocol.reward_list = m_param.reward_list;

	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), &protocol, sizeof(protocol));
}
