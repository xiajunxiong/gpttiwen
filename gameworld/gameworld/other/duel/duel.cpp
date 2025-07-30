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

#include "duel.hpp"
#include "duelconfig.hpp"
#include "protocol/msgduel.h"
#include "config/otherconfig/scoresystemconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/event/eventhandler.hpp"
#include "other/roleattribute/roleexpconfig.hpp"
#include "other/title/title.hpp"
#include "other/rolecross/rolecross.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/escort/escort.hpp"
#include "other/escort/escortconfig.hpp"
#include "other/prestige/prestige.hpp"
#include "global/team/team.hpp"

Duel::Duel()
{
}

Duel::~Duel()
{
}

void Duel::Init(RoleModuleManager * mgr, const DuelParam & param)
{
	m_mgr = mgr;
	m_param = param;
}

void Duel::GetInitParam(DuelParam * param)
{
	*param = m_param;
}

void Duel::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.use_reduce_item_num = 0;
}

void Duel::Update(unsigned long interval, time_t now_second, unsigned int now_dayid)
{
	const DuelOtherCfg& other_cfg = LOGIC_CONFIG->GetDuelConfig()->GetOtherCfg();

	if ((unsigned int)now_second >= m_param.last_reduce_time + 60 * other_cfg.reduce_kill_value_time && 0 != m_param.last_reduce_time)
	{
		int reduce_times = ((unsigned int)now_second - m_param.last_reduce_time) / (60 * other_cfg.reduce_kill_value_time);

		if (0 < reduce_times)
		{
			m_param.last_reduce_time += 60 * other_cfg.reduce_kill_value_time * reduce_times;
			this->ChangeKillValue(-reduce_times, __FUNCTION__);
		}
	}
}

void Duel::OnRoleLogin()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (0 < m_param.kill_value)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::duel_red_name_login_cross,
				m_mgr->GetRole()->GetName(), m_mgr->GetRole()->GetScene()->GetSceneID(), m_mgr->GetRole()->GetPos().x, m_mgr->GetRole()->GetPos().y);
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgAllGame(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT),
					0, 0, 0, 0);
			}
		}
	}
}

bool Duel::ChangeKillValue(int value, const char * reason)
{
	if (NULL == reason) return false;
	if (0 == value) return false;
	if (0 == m_param.kill_value && 0 >= value) return false;

	int old_kill_value = m_param.kill_value;

	m_param.kill_value += value;
	if (0 > m_param.kill_value)
	{
		m_param.kill_value = 0;
	}

	if (0 == old_kill_value)
	{
		m_param.last_reduce_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	}

	if (0 == m_param.kill_value)
	{
		m_param.last_reduce_time = 0;
	}

	this->CheckTitle();
	this->SendInfo();
	this->OnAoiNotice();
	this->CheckTeam();

	gamelog::g_log_duel.printf(LL_INFO, "%s role[%d, %s] value[%d] change_value[%d] reason[%s]", __FUNCTION__,
		m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), m_param.kill_value, value, reason);
	return true;
}

void Duel::CheckTitle()
{
	if (0 == m_param.kill_value)
	{
		if (m_mgr->GetTitle()->HasTitle(m_param.kill_title_id))
		{
			m_mgr->GetTitle()->RemoveTitle(m_param.kill_title_id, "Duel::CheckTitle");
		}

		m_param.kill_title_id = 0;
	}
	else
	{
		const DuelKillCfg* cfg = LOGIC_CONFIG->GetDuelConfig()->GetKillCfg(m_param.kill_value);
		if (NULL == cfg) return;

		if (0 >= cfg->title_id) return;
		if (m_param.kill_titel_stage == cfg->kill_level && m_param.kill_title_id == cfg->title_id) return;

		if (m_param.kill_title_id != cfg->title_id)
		{
			int index = m_mgr->GetTitle()->AddTitle(cfg->title_id, "Duel::CheckTitle");
			if (0 <= index)
			{
				if (0 <= m_mgr->GetTitle()->OnWearTitle(index, false, true))
				{
					m_mgr->NoticeNum(errornum::EN_TITLE_KILL_VALUE_TITLE_CHANGE);
				}
			}

			if (m_mgr->GetTitle()->HasTitle(m_param.kill_title_id))
			{
				m_mgr->GetTitle()->RemoveTitle(m_param.kill_title_id, "Duel::CheckTitle");
			}

			m_param.kill_title_id = cfg->title_id;
		}

		if (m_param.kill_titel_stage != cfg->kill_level)
		{
			m_param.kill_titel_stage = cfg->kill_level;
		}
	}

	m_mgr->ReCalcAttr(AttributeList::RECALC_REASON_INVALID, __FUNCTION__);
}

void Duel::LossDeduce()
{
	const DuelOtherCfg& other_cfg = LOGIC_CONFIG->GetDuelConfig()->GetOtherCfg();

	long long reduce_exp = 0;
	const RoleTopLevelCfg * top_cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetTopLevelCfg(m_mgr->GetRole()->GetTopLevel());
	if (NULL != top_cfg)
	{
		reduce_exp = top_cfg->up_exp * (other_cfg.reduce_exp / 100.0);
	}
	else
	{
		const RoleLevelCfg* cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetLevelCfg(m_mgr->GetRole()->GetLevel());
		if (NULL != cfg)
		{
			reduce_exp = cfg->exp * (other_cfg.reduce_exp / 100.0);
		}
	}

	if (reduce_exp > m_mgr->GetRole()->GetExp())
	{
		reduce_exp = m_mgr->GetRole()->GetExp();
	}

	long long reduce_coin = other_cfg.reduce_coin;
	if (reduce_coin > m_mgr->GetMoney()->GetCoinBind())
	{
		reduce_coin = m_mgr->GetMoney()->GetCoinBind();
	}

	m_mgr->GetRole()->ReduceExp(reduce_exp, REDUCE_EXP_TYPE_DUEL, "Duel::LossDeduct");
	m_mgr->GetMoney()->UseCoinBind(reduce_coin, "Duel::LossDeduct");
}

void Duel::CheckTeam()
{
	Team* team = m_mgr->GetRole()->GetMyTeam();
	if (NULL == team) return;

	int index = team->GetMemberIndex(m_mgr->GetRole());
	if (0 > index || index >= MAX_TEAM_MEMBER_NUM) return;

	team->SetMemberKillValue(index, m_param.kill_value);
}

void Duel::SendInfo()
{
	m_mgr->GetRole()->SendRoleInfo();
}

void Duel::OnAoiNotice()
{
	Protocol::SCRoleDuelKillValueAOINotice aoi_notice;
	aoi_notice.obj_id = m_mgr->GetRole()->GetId();
	aoi_notice.reserve_sh = 0;
	aoi_notice.kill_value = m_param.kill_value;
	m_mgr->GetRole()->GetScene()->GetZoneMatrix()->NotifyAreaMsg(m_mgr->GetRole()->GetObserHandle(), &aoi_notice, sizeof(aoi_notice));
}

int Duel::OnBattleFinish(int duel_type, bool is_attack, bool is_win, int level_diff, int scene_id, UInt64 coin)
{
	if (DUEL_BATTLE_TYPE_BEGIN >= duel_type || duel_type >= DUEL_BATTLE_TYPE_END)
	{
		return 0;
	}

	const DuelOtherCfg& cfg = LOGIC_CONFIG->GetDuelConfig()->GetOtherCfg();

	int change_value = 0;
	if (0 >= level_diff)
	{
		level_diff = 0;
	}

	switch (duel_type)
	{
	case DUEL_BATTLE_TYPE_DUEL:
		{
			if (is_attack)
			{
				if (is_win)
				{
					change_value = cfg.kill_value_base + cfg.kill_value_multiple * level_diff;
					m_mgr->GetMoney()->AddCoinBind(coin, __FUNCTION__);
				}
				else
				{
					change_value = -cfg.loss_reduce_kill_value;
					this->LossDeduce();
				}
			}
			else
			{
				if (is_win)
				{
					m_mgr->GetMoney()->AddCoinBind(coin, __FUNCTION__);
				}
				else
				{
					this->LossDeduce();
				}
			}
		}
		break;
	case DUEL_BATTLE_TYPE_ADJUDICATION:
		{
			if (is_win)
			{
				m_mgr->GetMoney()->AddCoinBind(coin, __FUNCTION__);
			}
			else
			{
				change_value = -cfg.loss_reduce_kill_value;
				this->LossDeduce();
			}
		}
		break;
	case DUEL_BATTLE_TYPE_ESCORT:
		{
			if (is_attack)
			{
				if (is_win)
				{
					change_value = cfg.kill_value_base + cfg.kill_value_multiple * level_diff;

					const EscortTaskCfg * e_cfg = LOGIC_CONFIG->GetEscortConfig()->GetTaskCfg(m_param.escort_task_seq);
					if (NULL != e_cfg)
					{
						if (ESCORT_TYPE_NORMAL == e_cfg->progress_desc || ESCORT_TYPE_SENIOR == e_cfg->progress_desc)
						{
							const EscortNpcCfg* npc_cfg = LOGIC_CONFIG->GetEscortConfig()->GetEscortNpcCfg(e_cfg->NPC_group, m_param.escort_npc_id);
							if (NULL != npc_cfg)
							{
								//增加声望
								m_mgr->GetPrestige()->AddPrestige(npc_cfg->village_id, npc_cfg->reward_prestige, __FUNCTION__);
							}
							if (e_cfg->reward_count > 0) m_mgr->GetKnapsack()->PutList(e_cfg->reward_count, e_cfg->reward_list, PUT_REASON_DUEL);
							if (e_cfg->reward_coin > 0) m_mgr->GetMoney()->AddCoinBind(e_cfg->reward_coin, __FUNCTION__);
						}

						//返回接镖奖励
						if (e_cfg->deposit > 0)
						{
							m_mgr->GetMoney()->AddCoinBind(e_cfg->deposit, "Duel::OnBattleFinish");
						}
					}
				}
				else
				{
					change_value = -cfg.loss_reduce_kill_value;
					this->LossDeduce();
				}

				m_param.escort_task_seq = 0;
				m_param.escort_npc_id = 0;
			}
			else
			{
				if (is_win)
				{
					m_mgr->GetMoney()->AddCoinBind(coin, __FUNCTION__);
				}
				else
				{
					this->LossDeduce();
					m_mgr->GetEscort()->GiveupTask();
				}
			}
		}
		break;
	default:
		break;
	}

	this->ChangeKillValue(change_value, __FUNCTION__);

	if (!is_win)
	{
		//送回附近无法强制决斗村庄
		const DuelMapCfg* map_cfg = LOGIC_CONFIG->GetDuelConfig()->GetMapCfg(scene_id);
		if (NULL != map_cfg)
		{
			World::GetInstWorld()->GetSceneManager()->FlyByShoe(m_mgr->GetRole(), map_cfg->revive_map, map_cfg->pos.x, map_cfg->pos.y);
		}
	}

	return change_value;
}

void Duel::NoticeResult(int duel_type, bool is_attack, bool is_win, int change_value, bool target_is_team, const char* name, bool target_is_red, UInt64 coin)
{
	const DuelOtherCfg& cfg = LOGIC_CONFIG->GetDuelConfig()->GetOtherCfg();

	Protocol::SCSystemMsg sm, sm1, sm2, sm3, winsm;
	int sendlen = 0, sendlen1 = 0, sendlen2 = 0, sendlen3 = 0, winsendlen = 0;
	int length = 0, length1 = 0, length2 = 0, length3 = 0, winsendlength = 0;

	char text[2048] = { 0 };
	char text1[2048] = { 0 };
	char text2[2048] = { 0 };
	char text3[2048] = { 0 };
	char wintext[2048] = { 0 };

	if (!is_win)
	{
		/*char string[128] = { 0 };
		int len = 0;
		len += SNPRINTF(string + len, sizeof(cfg.reduce_exp), "%d", cfg.reduce_exp);
		len += SNPRINTF(string + len, sizeof("%"), "%");
		string[len + 1 - sizeof("%")] = '\0';*/

		if (target_is_team)
		{
			length = SNPRINTF(text, sizeof(text), gamestring::duel_lose_reduce_exp_team, name, cfg.reduce_exp);
			length1 = SNPRINTF(text1, sizeof(text1), gamestring::duel_lose_reduce_coin_team, name, coin);
		}
		else
		{
			length = SNPRINTF(text, sizeof(text), gamestring::duel_lose_reduce_exp, name, cfg.reduce_exp);
			length1 = SNPRINTF(text1, sizeof(text1), gamestring::duel_lose_reduce_coin, name, coin);
		}
	}
	else
	{
		if (target_is_team)
		{
			winsendlength = SNPRINTF(wintext, sizeof(wintext), gamestring::duel_win_add_coin_team, name, coin);
		}
		else
		{
			winsendlength = SNPRINTF(wintext, sizeof(wintext), gamestring::duel_win_add_coin, name, coin);
		}
	}

	switch (duel_type)
	{
	case DUEL_BATTLE_TYPE_DUEL:
		{
			if (is_attack)
			{
				if (is_win)
				{
					if (target_is_team)
					{
						length = SNPRINTF(text, sizeof(text), gamestring::duel_type_one_attack_win_team, name, change_value);
					}
					else
					{
						length = SNPRINTF(text, sizeof(text), gamestring::duel_type_one_attack_win, name, change_value);
					}
				}
				else
				{
					if (target_is_team)
					{
						length2 = SNPRINTF(text2, sizeof(text2), gamestring::duel_lose_reduce_kill_value_team, name, change_value);
					}
					else
					{
						length2 = SNPRINTF(text2, sizeof(text2), gamestring::duel_lose_reduce_kill_value, name, change_value);
					}
				}
			}
			else
			{
				if (is_win)
				{
					if (target_is_team)
					{
						if (target_is_red)
						{
							length = SNPRINTF(text, sizeof(text), gamestring::duel_type_one_defend_win_red_team, name);
						}
						else
						{
							length = SNPRINTF(text, sizeof(text), gamestring::duel_type_one_defend_win, name);
						}
					}
					else
					{
						if (target_is_red)
						{
							length = SNPRINTF(text, sizeof(text), gamestring::duel_type_one_defend_win_red_team, name);
						}
						else
						{
							length = SNPRINTF(text, sizeof(text), gamestring::duel_type_one_defend_win, name);
						}
					}
				}
			}
		}
		break;
	case DUEL_BATTLE_TYPE_ADJUDICATION:
		{
			if (is_attack)
			{
				if (is_win)
				{
					if (target_is_team)
					{
						length = SNPRINTF(text, sizeof(text), gamestring::duel_type_two_attack_win_team, name, change_value);
					}
					else
					{
						length = SNPRINTF(text, sizeof(text), gamestring::duel_type_two_attack_win, name, change_value);
					}
				}
				else
				{
					if (target_is_team)
					{
						if (this->IsRedName())
						{
							length2 = SNPRINTF(text2, sizeof(text2), gamestring::duel_lose_reduce_kill_value, name, change_value);
						}
					}
					else
					{
						if (this->IsRedName())
						{
							length2 = SNPRINTF(text2, sizeof(text2), gamestring::duel_lose_reduce_kill_value, name, change_value);
						}
					}
				}
			}
			else
			{
				if (is_win)
				{
					length = SNPRINTF(text, sizeof(text), gamestring::duel_type_two_defend_win, name);
				}
				else
				{
					if (target_is_team)
					{
						length2 = SNPRINTF(text2, sizeof(text2), gamestring::duel_lose_reduce_kill_value_team, name, change_value);
					}
					else
					{
						length2 = SNPRINTF(text2, sizeof(text2), gamestring::duel_lose_reduce_kill_value, name, change_value);
					}
				}
			}
		}
		break;
	case DUEL_BATTLE_TYPE_ESCORT:
		{
			if (is_attack)
			{
				if (is_win)
				{
					length = SNPRINTF(text, sizeof(text), gamestring::duel_type_three_attack_win, change_value);
				}
				else
				{
					if (target_is_team)
					{
						if (this->IsRedName())
						{
							length2 = SNPRINTF(text2, sizeof(text2), gamestring::duel_lose_reduce_kill_value, name, change_value);
						}
					}
					else
					{
						if (this->IsRedName())
						{
							length2 = SNPRINTF(text2, sizeof(text2), gamestring::duel_lose_reduce_kill_value, name, change_value);
						}
					}
				}
			}
			else
			{
				if (is_win)
				{
					if (target_is_team)
					{
						length2 = SNPRINTF(text2, sizeof(text2), gamestring::duel_type_three_defend_win_team, name);
					}
					else
					{
						length2 = SNPRINTF(text2, sizeof(text2), gamestring::duel_type_three_defend_win, name);
					}
				}
				else
				{
					if (target_is_team)
					{
						if (this->IsRedName())
						{
							length2 = SNPRINTF(text2, sizeof(text2), gamestring::duel_lose_reduce_kill_value_team, name, change_value);
						}
						length3 = SNPRINTF(text3, sizeof(text3), gamestring::duel_type_three_defend_lose_team, name);
					}
					else
					{
						if (this->IsRedName())
						{
							length2 = SNPRINTF(text2, sizeof(text2), gamestring::duel_lose_reduce_kill_value, name, change_value);
						}
						length3 = SNPRINTF(text3, sizeof(text3), gamestring::duel_type_three_defend_lose, name);
					}
				}
			}
		}
		break;
	default:
		break;
	}

	if (winsendlength > 0)
	{
		if (SceneManager::CreateSystemMsg(&winsm, &winsendlen, wintext, winsendlength, GetMsgType(SYS_MSG_FLOAT)))
		{
			EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&winsm, winsendlen);
		}
	}
	if (length > 0)
	{
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, text, length, GetMsgType(SYS_MSG_FLOAT)))
		{
			EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&sm, sendlen);
		}
	}
	if (length1 > 0)
	{
		if (SceneManager::CreateSystemMsg(&sm1, &sendlen1, text1, length1, GetMsgType(SYS_MSG_FLOAT)))
		{
			EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&sm1, sendlen1);
		}
	}
	if (length2 > 0)
	{
		if (SceneManager::CreateSystemMsg(&sm2, &sendlen2, text2, length2, GetMsgType(SYS_MSG_FLOAT)))
		{
			EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&sm2, sendlen2);
		}
	}
	if (length3 > 0)
	{
		if (SceneManager::CreateSystemMsg(&sm3, &sendlen3, text3, length3, GetMsgType(SYS_MSG_FLOAT)))
		{
			EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&sm3, sendlen3);
		}
	}
}

void Duel::OnDuelEscort(int escort_task_seq, int escort_npc_id)
{
	m_param.escort_task_seq = escort_task_seq;
	m_param.escort_npc_id = escort_npc_id;
}

bool Duel::CanUseRedeceItem()
{
	if (m_param.use_reduce_item_num >= LOGIC_CONFIG->GetDuelConfig()->GetOtherCfg().item_reduce_daily_times)
	{
		return false;
	}

	return true;
}
