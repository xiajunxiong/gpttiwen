#include "realmbreak.hpp"
#include "engineadapter.h"
#include "gamelog.h"

#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/userprotocal/msgsystem.h"

#include "item/itempool.h"
#include "obj/character/role.h"
#include "battle/battle_manager_local.hpp"

#include "scene/scenemanager.hpp"

#include "protocol/msgother.h"

#include "other/funopen/funopen.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "other/funopen/funopenconfig.hpp"
#include "other/roleattribute/roleexpconfig.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/realmbreakconfig.hpp"
#include "other/route/mailroute.hpp"

RealmBreak::RealmBreak() : m_module_mgr(NULL), last_tuna_timestamp(0), last_add_break_exp_timestamp(0), last_add_player_exp_timestamp(0)
{
}

RealmBreak::~RealmBreak()
{
}

void RealmBreak::Init(RoleModuleManager * module_mgr, const RealmBreakParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;

	if (0 == m_param.is_change_ver_reset_flag)
	{
		m_param.is_change_ver_reset_flag = 1;
		m_param.cur_seq = 0;
		m_param.tuna_num = 0;
		m_param.break_exp = 0;
		m_param.cur_fight_seq = 0;
	}

	/*if (m_param.cur_seq <= 0)		//目前默认是没有境界的,境界跟心灵之境挑战挂钩
	{
		gamelog::g_Log_realm_break.printf(LL_INFO, "%s user[%d, %s] old_seq[%d] cur_seq[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), m_param.cur_seq, 1);
		m_param.cur_seq = 1;
	}*/

	const RealmBreakOtherCfg & other_cfg = LOGIC_CONFIG->GetRealmBreakConfig()->GetOtherCfg();
	if (m_param.cur_seq > other_cfg.break_max_num)
	{
		gamelog::g_Log_realm_break.printf(LL_INFO, "%s user[%d, %s] old_seq[%d] cur_seq[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), m_param.cur_seq,
			 other_cfg.break_max_num);
		m_param.cur_seq = other_cfg.break_max_num;
		m_param.cur_fight_seq = other_cfg.break_max_num;
		/*const RealmBreakCfg * break_cfg = LOGIC_CONFIG->GetRealmBreakConfig()->GetBreakCfg(m_param.cur_seq);  灵力值砍掉
		if (NULL != break_cfg && m_param.break_exp > (unsigned int)break_cfg->break_need_exp)
		{
			m_param.break_exp = break_cfg->break_need_exp;
		}*/
	}
}

void RealmBreak::GetInitParam(RealmBreakParam * param)
{
	*param = m_param;
}

void RealmBreak::Update(time_t now_second)
{
	if(!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_REALM_BREAK) || m_param.cur_seq <= 0 || m_param.cur_fight_seq <= 0) return;

	bool is_send = false;
	const RealmBreakOtherCfg & other_cfg = LOGIC_CONFIG->GetRealmBreakConfig()->GetOtherCfg();
	if (m_param.cur_seq > other_cfg.break_max_num)
	{
		gamelog::g_Log_realm_break.printf(LL_INFO, "%s old_seq[%d] cur_seq[%d]", __FUNCTION__, m_param.cur_seq, other_cfg.break_max_num);
		m_param.cur_seq = other_cfg.break_max_num;
		m_param.cur_fight_seq = other_cfg.break_max_num;
		is_send = true;
	}
	const RealmBreakCfg * break_cfg = LOGIC_CONFIG->GetRealmBreakConfig()->GetBreakCfg(m_param.cur_seq);
	if (NULL == break_cfg)
	{
		return;
	}

	unsigned int now = (unsigned int)now_second;
	if (m_param.last_rand_item_timestamp + other_cfg.item_time_s <= now)
	{
		unsigned int item_time_interval = now - m_param.last_rand_item_timestamp;
		if (m_param.user_cumulation_pratice_time < break_cfg->time_max_s)
		{
			int rand_count = item_time_interval / other_cfg.item_time_s;
			item_time_interval = rand_count * other_cfg.item_time_s;
			if (rand_count > 0)
			{
				RealmBreakItem item_list[MAX_REALM_BREAK_ITEM_NUM];
				if (LOGIC_CONFIG->GetRealmBreakConfig()->GetRandItem(break_cfg->item_group_id, rand_count, item_list))
				{
					if (this->DepositItem(rand_count, item_list))
					{
						this->SendItemInfo();
					}
				}
			}
		}
		m_param.last_rand_item_timestamp += item_time_interval;
	}
	if (m_param.last_get_fix_item_timestamp + other_cfg.gaimingdan_item_s <= now)
	{
		unsigned int item_time_interval = now - m_param.last_get_fix_item_timestamp;
		if (m_param.user_cumulation_pratice_time < break_cfg->time_max_s)
		{
			int rand_count = item_time_interval / other_cfg.gaimingdan_item_s;
			item_time_interval = rand_count * other_cfg.gaimingdan_item_s;
			if (rand_count > 0)
			{
				RealmBreakItem break_item;
				break_item.item_id = break_cfg->time_interval_item_id;
				break_item.num = break_cfg->time_interval_item_num * rand_count;
				break_item.is_bind = true;
				if (this->DepositItem(1, &break_item))
				{
					this->SendItemInfo();
				}
			}
		}
		m_param.last_get_fix_item_timestamp += item_time_interval;
	}

	//1.正常情况下达到时间间隔就会进来,所以不需要去做什么计算 2.在线经过2次热更,1->达到等级热更到未达到,2->未达到等级热更到达到等级,此时之前记录的时间戳不太正确,所以默认给一倍间隔增加
	/*int add_break_exp = 0;
	if (last_add_break_exp_timestamp + other_cfg.break_exp_s <= now)
	{
		add_break_exp = break_cfg->break_exp_time_s;
		last_add_break_exp_timestamp = now;			//赋值放在这里是为了防止满阶满灵力后update每次都进入这里
		if (m_param.cur_seq >= other_cfg.break_max_num && m_param.break_exp + (unsigned int)add_break_exp > (unsigned int)break_cfg->break_need_exp)
		{
			add_break_exp = break_cfg->break_need_exp - m_param.break_exp;
		}
	}*/
	int add_coin = 0;
	int add_exp = 0;
	if (last_add_player_exp_timestamp + other_cfg.player_exp_s <= now)
	{
		last_add_player_exp_timestamp = now;		//赋值放在这里是为了防止满级满经验后update每次都进入这里
		if (m_param.user_cumulation_pratice_time < break_cfg->time_max_s)
		{
			add_exp = break_cfg->player_exp_time_s;
			add_coin = break_cfg->player_coin_time_s;
			if (add_exp < 0)
			{
				add_exp = 0;
			}
		}
	}
	/*if (add_break_exp > 0)
	{
		m_param.break_exp += add_break_exp;
		is_send = true;
	}*/
	if (add_exp > 0 || add_coin > 0)
	{
		m_param.cumulation_user_coin_count += add_coin;
		//m_param.cumulation_user_exp_count += add_exp;
		m_param.user_cumulation_pratice_time += other_cfg.player_exp_s;
		if (m_param.user_cumulation_pratice_time > break_cfg->time_max_s)
		{
			m_param.user_cumulation_pratice_time = break_cfg->time_max_s;
		}
		is_send = true;
	}

	if (is_send)
	{
		this->SendAllInfo();
	}
}

void RealmBreak::OnLogin(long long last_save_time)
{
	// 补发改命丹, 只调用一次
	this->GaiMingReissue();

	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	if (1 != m_param.is_old_user_flag)
	{
		m_param.is_old_user_flag = 1;	
		if (FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_REALM_BREAK) && m_param.cur_seq > 0 && m_param.cur_fight_seq > 0)
		{
			//last_add_break_exp_timestamp = now_second;
			last_add_player_exp_timestamp = now_second;
			m_param.last_rand_item_timestamp = now_second;
			m_param.last_get_fix_item_timestamp = now_second;
		}
		return;
	}
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_REALM_BREAK) || m_param.cur_fight_seq <= 0 || m_param.cur_seq <= 0) 
	{
		m_param.last_rand_item_timestamp = now_second;
		m_param.last_get_fix_item_timestamp = now_second;
		return;
	}
	unsigned int last_logout_timestamp = static_cast<unsigned int>(last_save_time);
	//last_add_break_exp_timestamp = now_second;
	last_add_player_exp_timestamp = now_second;

	if(now_second <= last_logout_timestamp) return;		//防止调时间

	const RealmBreakOtherCfg & other_cfg = LOGIC_CONFIG->GetRealmBreakConfig()->GetOtherCfg();
	const RealmBreakCfg * break_cfg = LOGIC_CONFIG->GetRealmBreakConfig()->GetBreakCfg(m_param.cur_seq);
	if(NULL == break_cfg) return;
	
	int add_exp = 0, add_break_exp = 0, add_coin = 0;
	unsigned int time_interval = now_second - last_logout_timestamp;
	/*if (time_interval >= (unsigned int)other_cfg.break_exp_s)
	{
		if (time_interval > break_cfg->time_max_s)
		{
			add_break_exp = break_cfg->time_max_s / other_cfg.break_exp_s * break_cfg->break_exp_time_s;
		}
		else
		{
			add_break_exp = time_interval / other_cfg.break_exp_s * break_cfg->break_exp_time_s;
		}
	//	if (add_break_exp > break_cfg->break_exp_lixian_max)
	//	{
	//		add_break_exp = break_cfg->break_exp_lixian_max;
	//	}
	}*/
	bool is_send = false;
	//经验计算需要放在灵力值之后,否则当前时间距离上次离线时间间隔可能会被修改
	if (m_param.user_cumulation_pratice_time + (unsigned int)other_cfg.player_exp_s <= (unsigned int)break_cfg->time_max_s && time_interval >= (unsigned int)other_cfg.player_exp_s)
	{
		if (m_param.user_cumulation_pratice_time + time_interval > (unsigned int)break_cfg->time_max_s)
		{
			time_interval = break_cfg->time_max_s - m_param.user_cumulation_pratice_time;
		}

		add_exp = time_interval / other_cfg.player_exp_s * break_cfg->player_exp_time_s;
		add_coin = time_interval / other_cfg.player_exp_s * break_cfg->player_coin_time_s;
		//道具
		if (0 != m_param.last_rand_item_timestamp)
		{
			unsigned int item_time_interval = now_second - m_param.last_rand_item_timestamp;
			if (item_time_interval >= other_cfg.item_time_s)
			{
				int rand_count = 0;
				if (m_param.user_cumulation_pratice_time + item_time_interval > (unsigned int)break_cfg->time_max_s)
				{
					item_time_interval = break_cfg->time_max_s - m_param.user_cumulation_pratice_time;
					rand_count = item_time_interval / other_cfg.item_time_s;
				}
				else
				{
					rand_count = item_time_interval / other_cfg.item_time_s;
					item_time_interval = rand_count * other_cfg.item_time_s;
				}

				RealmBreakItem item_list[MAX_REALM_BREAK_ITEM_NUM];
				if (LOGIC_CONFIG->GetRealmBreakConfig()->GetRandItem(break_cfg->item_group_id, rand_count, item_list))
				{
					is_send |= this->DepositItem(rand_count, item_list);
				}
			}
			m_param.last_rand_item_timestamp += item_time_interval;
		}
		else
		{
			m_param.last_rand_item_timestamp = now_second;
		}

		if (0 != m_param.last_get_fix_item_timestamp)
		{
			unsigned int item_time_interval = now_second - m_param.last_get_fix_item_timestamp;
			if (item_time_interval >= other_cfg.gaimingdan_item_s)
			{
				int rand_count = 0;
				if (m_param.user_cumulation_pratice_time + item_time_interval > (unsigned int)break_cfg->time_max_s)
				{
					item_time_interval = break_cfg->time_max_s - m_param.user_cumulation_pratice_time;
					rand_count = item_time_interval / other_cfg.gaimingdan_item_s;
				}
				else
				{
					rand_count = item_time_interval / other_cfg.gaimingdan_item_s;
					item_time_interval = rand_count * other_cfg.gaimingdan_item_s;
				}

				if (rand_count > 0)
				{
					break_cfg->time_interval_item_id;
					RealmBreakItem break_item;
					break_item.item_id = break_cfg->time_interval_item_id;
					break_item.num = break_cfg->time_interval_item_num * rand_count;
					break_item.is_bind = true;
					is_send |= this->DepositItem(1, &break_item);
				}
				m_param.last_get_fix_item_timestamp += item_time_interval;
			}
		}
		else
		{
			m_param.last_get_fix_item_timestamp = now_second;
		}
		/*if (add_exp > break_cfg->player_exp_lixian_max)
		{
			add_exp = break_cfg->player_exp_lixian_max;
		}*/
	}

	/*if (m_param.cur_seq >= other_cfg.break_max_num && m_param.break_exp + (unsigned int)add_break_exp > (unsigned int)break_cfg->break_need_exp)
	{
		add_break_exp = break_cfg->break_need_exp - m_param.break_exp;
	}

	
	if (m_param.break_exp > 0)
	{
		m_param.break_exp += add_break_exp;
	}*/
	if (is_send)
	{
		this->SendItemInfo();
	}
	if (add_exp > 0 || add_coin > 0)
	{
		m_param.cumulation_user_coin_count += add_coin;
		//m_param.cumulation_user_exp_count += add_exp;
		m_param.user_cumulation_pratice_time += time_interval;
	}
	if (add_exp > 0 || add_break_exp > 0 || add_coin > 0)
	{
		this->SendAllInfo();
		gamelog::g_Log_realm_break.printf(LL_INFO, "%s user[%d, %s] cur_seq[%d] cur_break_exp[%u] add_exp[%d] add_break_exp[%d] add_coin[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
			m_param.cur_seq, m_param.break_exp, add_exp, add_break_exp, add_coin);
	}
}

void RealmBreak::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid ||(!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_REALM_BREAK)) || m_param.cur_seq <= 0 || m_param.cur_fight_seq <= 0) return;

	//m_param.tuna_num = 0;
	//this->SendAllInfo();
}

void RealmBreak::OnRoleLevelUp()
{
	const FunOpenCfg* fun_open_cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_REALM_BREAK);
	if(NULL == fun_open_cfg) return;

	if (fun_open_cfg->limit_level == m_module_mgr->GetRole()->GetLevel() && m_param.cur_fight_seq > 0 && m_param.cur_seq > 0)
	{
		unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
		//last_add_break_exp_timestamp = now_second;
		last_add_player_exp_timestamp = now_second;
		m_param.last_rand_item_timestamp = now_second;
		m_param.last_get_fix_item_timestamp = now_second;
		//this->SendAllInfo();
	}
}

void RealmBreak::OnUseItem(ItemID item_id, short num)
{	
	return;	//灵力值砍掉,灵力丹砍掉
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_REALM_BREAK, true)) return;

	if (num <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
	}
	//策划说每次只能使用一颗
	num = 1;
	if (NULL == ITEMPOOL->GetItem(item_id))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return;
	}
	int add_break_exp = LOGIC_CONFIG->GetRealmBreakConfig()->GetBreakExpByItemId(item_id) * num;
	if(add_break_exp <= 0) 
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return;
	}
	const RealmBreakCfg * break_cfg = LOGIC_CONFIG->GetRealmBreakConfig()->GetBreakCfg(m_param.cur_seq);
	if(NULL == break_cfg) return;

	const RealmBreakOtherCfg & other_cfg = LOGIC_CONFIG->GetRealmBreakConfig()->GetOtherCfg();
	if (m_param.cur_seq >= other_cfg.break_max_num)
	{
		if (m_param.break_exp >= (unsigned int)break_cfg->break_need_exp)
		{
			m_param.break_exp = break_cfg->break_need_exp;
			m_module_mgr->NoticeNum(errornum::EN_REALM_BREAK_PERFECT_SPIRITUAL_PRACTIVCE);
			return;
		}
		else if (m_param.break_exp + (unsigned int)add_break_exp > (unsigned int)break_cfg->break_need_exp)
		{
			add_break_exp = break_cfg->break_need_exp - m_param.break_exp;
		}
	}//不满阶满灵力是可以继续吃灵力丹
	/*else if (m_param.break_exp >= (unsigned int)break_cfg->break_need_exp)
	{
		m_module_mgr->NoticeNum(errornum::EN_REALM_BREAK_BREAK_EXP_FULL_USE_ITEM);
		return;
	}*/
	if (!m_module_mgr->GetKnapsack()->ConsumeItem(item_id, num, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	m_param.break_exp += add_break_exp;
	this->SendAllInfo();
	gamelog::g_Log_realm_break.printf(LL_INFO, "%s user[%d, %s] cur_seq[%d] cur_break_exp[%u] add_break_exp[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		m_param.cur_seq, m_param.break_exp, add_break_exp);
}

void RealmBreak::OnBreak()
{
	return;		//奖励移动到心灵挑战,境界提升也移到心灵挑战
	const RealmBreakOtherCfg & other_cfg = LOGIC_CONFIG->GetRealmBreakConfig()->GetOtherCfg();
	if (m_param.cur_seq >= other_cfg.break_max_num)
	{
		m_module_mgr->NoticeNum(errornum::EN_REALM_BREAK_PERFECT_SPIRITUAL_PRACTIVCE);
		return;
	}
	
	const RealmBreakCfg * break_cfg = LOGIC_CONFIG->GetRealmBreakConfig()->GetBreakCfg(m_param.cur_seq);
	if (NULL == break_cfg) return;

	if (m_param.break_exp < (unsigned int)break_cfg->break_need_exp)
	{
		m_module_mgr->NoticeNum(errornum::EN_REALM_BREAK_BREAK_EXP_NOT_ENOUGH);
		return;
	}
	/*if (0 != break_cfg->monster_group_id)
	{
		BattleManagerLocal::GetInstance().StartBattleModeFightReq(m_module_mgr->GetRole(), BATTLE_MODE_REALM_BREAK, break_cfg->monster_group_id, 0, 0, true);
		return;
	}*/
	m_param.break_exp -= break_cfg->break_need_exp;
	m_param.cur_seq += 1;
	//m_param.tuna_num = 0;	//升阶不重置
	if (break_cfg->break_player_exp_award > 0)
	{
		m_module_mgr->GetRole()->AddExp(break_cfg->break_player_exp_award, ADD_EXP_TYPE_REALM_BREAK_BREAK, __FUNCTION__);
	}
	if (!break_cfg->reward_list.empty())
	{
		if (!m_module_mgr->GetKnapsack()->PutList((short)break_cfg->reward_list.size(), &break_cfg->reward_list[0], PUT_REASON_REALM_BREAK))
		{
			m_module_mgr->GetKnapsack()->SendMail(&break_cfg->reward_list[0], (int)break_cfg->reward_list.size(), SNED_MAIL_TYPE_DEFAULT, true);
		}
	}
	if (m_param.cur_seq >= other_cfg.break_max_num)
	{
		const RealmBreakCfg * new_break_cfg = LOGIC_CONFIG->GetRealmBreakConfig()->GetBreakCfg(m_param.cur_seq);
		if (NULL != new_break_cfg && m_param.break_exp > (unsigned int)new_break_cfg->break_need_exp)
		{
			m_param.break_exp = new_break_cfg->break_need_exp;
		}
	}
	this->SendAllInfo();
	EventHandler::Instance().OnRoleRealmBreak(m_module_mgr->GetRole(), m_param.cur_seq);
	gamelog::g_Log_realm_break.printf(LL_INFO, "%s user[%d, %s] cur_seq[%d] cur_break_exp[%u]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		m_param.cur_seq, m_param.break_exp);
}

void RealmBreak::OnStartTuna()
{
	return;
	const RealmBreakOtherCfg & other_cfg = LOGIC_CONFIG->GetRealmBreakConfig()->GetOtherCfg();
	const RealmBreakCfg * break_cfg = LOGIC_CONFIG->GetRealmBreakConfig()->GetBreakCfg(m_param.cur_seq);
	if (NULL == break_cfg) return;

	//判断时间
	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	if (now_second < last_tuna_timestamp + other_cfg.tuna_time_s)
	{
		m_module_mgr->NoticeNum(errornum::EN_YOUR_CLICK_IS_TOO_OFTEN);
		return;
	}

	if (m_param.tuna_num >= other_cfg.tuna_num)
	{
		m_module_mgr->NoticeNum(errornum::EN_REALM_BREAK_TUNA_NUM_NOT_ENOUGH);
		return;
	}

	if (m_param.cur_seq >= other_cfg.break_max_num && m_param.break_exp >= (unsigned int)break_cfg->break_need_exp)
	{
		m_param.cur_seq = other_cfg.break_max_num;
		m_param.break_exp = break_cfg->break_need_exp;
		m_module_mgr->NoticeNum(errornum::EN_REALM_BREAK_PERFECT_SPIRITUAL_PRACTIVCE);
		this->SendAllInfo();
	}

	//不满阶满灵力时可以继续吐纳
	/*if (m_param.break_exp >= (unsigned int)break_cfg->break_need_exp)
	{
		if (m_param.cur_seq >= other_cfg.break_max_num)
		{
			m_param.cur_seq = other_cfg.break_max_num;
			m_param.break_exp = break_cfg->break_need_exp;
			m_module_mgr->NoticeNum(errornum::EN_REALM_BREAK_PERFECT_SPIRITUAL_PRACTIVCE);
			this->SendAllInfo();
		}
		else
		{
			m_module_mgr->NoticeNum(errornum::EN_REALM_BREAK_BREAK_EXP_FULL_TUNA);
		}
		return;
	}*/

	unsigned int add_break_exp = break_cfg->tuna_exp;
	int rate_num = rand() % RealmBreakCfg::MAX_RATE_COUNT_NUM;
	if (rate_num < break_cfg->exp_double)
	{
		add_break_exp *= 2;
	}
	else if (rate_num - break_cfg->exp_double < break_cfg->exp_treble)
	{
		add_break_exp *= 3;
	}
	else if (rate_num - break_cfg->exp_double - break_cfg->exp_treble < break_cfg->exp_quintuple)
	{
		add_break_exp *= 5;
	}
	if (m_param.cur_seq >= other_cfg.break_max_num && m_param.break_exp + add_break_exp > (unsigned int)break_cfg->break_need_exp)
	{
		add_break_exp = break_cfg->break_need_exp - m_param.break_exp;
	}

	m_param.break_exp += add_break_exp;
	m_param.tuna_num++;
	last_tuna_timestamp = now_second;
	this->SendAllInfo();
	gamelog::g_Log_realm_break.printf(LL_INFO, "%s user[%d, %s] cur_seq[%d] cur_break_exp[%u] add_break_exp[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		m_param.cur_seq, m_param.break_exp, add_break_exp);
}

void RealmBreak::OnBattleFinish(bool is_win, int seq)
{
	if (!is_win) return;
	if (m_param.cur_fight_seq >= seq) return;
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_REALM_BREAK)) return;

	const RealmBreakFightCfg * fight_cfg = LOGIC_CONFIG->GetRealmBreakConfig()->GetFightCfg(m_param.cur_fight_seq + 1);
	if (NULL == fight_cfg) return;

	m_param.cur_fight_seq++;
	m_param.cur_seq = m_param.cur_fight_seq;
	if (1 == m_param.cur_seq)	//第一个境界
	{
		unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
		//last_add_break_exp_timestamp = now_second;
		last_add_player_exp_timestamp = now_second;
		m_param.last_rand_item_timestamp = now_second;
		m_param.last_get_fix_item_timestamp = now_second;
	}

	if (0 < fight_cfg->coin)
	{
		m_module_mgr->GetMoney()->AddCoinBind(fight_cfg->coin, __FUNCTION__);
	}
	if (0 < fight_cfg->player_exp)
	{
		m_module_mgr->GetRole()->AddExp(fight_cfg->player_exp, ADD_EXP_TYPE_REALM_BREAK_FIGHT, __FUNCTION__);
	}
	if (!fight_cfg->item.empty())
	{
		if (!m_module_mgr->GetKnapsack()->PutList((short)fight_cfg->item.size(), &fight_cfg->item[0], PUT_REASON_REALM_BREAK_FIGHT))
		{
			m_module_mgr->GetKnapsack()->SendMail(&fight_cfg->item[0], (int)fight_cfg->item.size(), SNED_MAIL_TYPE_DEFAULT, true);
		}
	}

	//灵气奖励
	/*int old_seq = m_param.cur_seq;
	const RealmBreakOtherCfg & other_cfg = LOGIC_CONFIG->GetRealmBreakConfig()->GetOtherCfg();

	const RealmBreakCfg * break_cfg = LOGIC_CONFIG->GetRealmBreakConfig()->GetBreakCfg(m_param.cur_seq);
	if (NULL != break_cfg && 0 < fight_cfg->break_exp)
		{
		if (m_param.cur_seq >= other_cfg.break_max_num)
		{
			if (m_param.break_exp >= (unsigned int)break_cfg->break_need_exp)
			{
				m_param.break_exp = break_cfg->break_need_exp;
			}
			else if (m_param.break_exp + fight_cfg->break_exp > (unsigned int)break_cfg->break_need_exp)
			{
				m_param.break_exp = break_cfg->break_need_exp;
			}
		}
		
		m_param.break_exp += fight_cfg->break_exp;
	}*/

	m_module_mgr->GetKnapsack()->NotifyGetItemList3(0, fight_cfg->coin, fight_cfg->player_exp, (short)fight_cfg->item.size(), &fight_cfg->item[0], GET_REASON_REALM_BREAK_FIGHT, fight_cfg->break_exp);

	this->SendAllInfo();
	EventHandler::Instance().OnRoleRealmBreak(m_module_mgr->GetRole(), m_param.cur_seq);
	EventHandler::Instance().OnRoleRealmBreakFight(m_module_mgr->GetRole(), m_param.cur_fight_seq);
	gamelog::g_Log_realm_break.printf(LL_INFO, "%s user[%d, %s] cur_seq[%d] cur_fight_seq[%d] cur_break_exp[%u]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		m_param.cur_seq, m_param.cur_fight_seq, m_param.break_exp);
}

void RealmBreak::OnFetchExp()
{
	/*if (m_param.cumulation_user_coin_count <= 0 && m_param.cumulation_user_exp_count <= 0 && m_param.item_count <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_REALM_BREAK_FETCH_EROOR);
		return;
	}*/
	if (m_param.item_count <= 0 && m_param.cumulation_user_coin_count <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_REALM_BREAK_FETCH_EROOR);
		return;
	}

	std::vector<ItemConfigData> item_list;
	this->FillItemVec(item_list);
	if (!item_list.empty())
	{
		if (!m_module_mgr->GetKnapsack()->PutList(item_list.size(), &item_list[0], PUT_REASON_REALM_BREAK))
		{
			m_module_mgr->GetKnapsack()->SendMail(&item_list[0], (int)item_list.size(), SNED_MAIL_TYPE_DEFAULT, true);
		}
	}

	/*Protocol::SCSystemMsg sm;
	int sendlen = 0;
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_realm_break_offline_info_pool_notice, m_param.cumulation_user_exp_count);
	if (length > 0)
	{
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
		{
			m_module_mgr->NetSend((const void *)&sm, sendlen);
		}
	}*/
	/*if (m_param.cumulation_user_exp_count > 0)
	{
		m_module_mgr->GetRole()->AddExp(m_param.cumulation_user_exp_count, ADD_EXP_TYPE_REALM_BREAK, __FUNCTION__);
	}*/
	if (m_param.cumulation_user_coin_count > 0)
	{
		m_module_mgr->GetMoney()->AddCoinBind(m_param.cumulation_user_coin_count, __FUNCTION__);
	}

	m_param.FetchReset();
	this->SendAllInfo();
	this->SendItemInfo();
}

void RealmBreak::GmSetInfo(int op_type, int param1)
{
	enum GM_SET_INFO_TYPE
	{
		GM_SET_INFO_TYPE_CUR_SEQ = 0,			//设置阶数 param1:seq
		GM_SET_INFO_TYPE_CLEAR_EXP = 1,			//清除灵气  

		GM_SET_INFO_TYPE_MAX
	};

	switch (op_type)
	{
		case GM_SET_INFO_TYPE_CUR_SEQ:
		{
			const RealmBreakOtherCfg & other_cfg = LOGIC_CONFIG->GetRealmBreakConfig()->GetOtherCfg();
			if (param1 > other_cfg.break_max_num)
			{
				m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return;
			}

			m_param.cur_fight_seq = param1;
			m_param.cur_seq = param1;
			//m_param.tuna_num = 0;
			EventHandler::Instance().OnRoleRealmBreak(m_module_mgr->GetRole(), m_param.cur_seq);
			EventHandler::Instance().OnRoleRealmBreakFight(m_module_mgr->GetRole(), m_param.cur_fight_seq);
		}
		break;
		case GM_SET_INFO_TYPE_CLEAR_EXP:
		{
			m_param.break_exp = 0;
		}
		break;
		default:
		return;
	}

	this->SendAllInfo();
}

bool RealmBreak::IsPassFightLevel(int fight_seq)
{
	if (m_param.cur_fight_seq >= fight_seq)
	{
		return true;
	}
	return false;
}

void RealmBreak::OnSoulFight(int seq)
{
	if (seq > m_param.cur_fight_seq + 1)
	{
		m_module_mgr->NoticeNum(errornum::EN_BREAK_SOUL_FIGHT_OLD_NOT_PASS);
		return;
	}

	const RealmBreakFightCfg * cfg = LOGIC_CONFIG->GetRealmBreakConfig()->GetFightCfg(seq);
	if (NULL == cfg)
	{
		return;
	}

	if (m_module_mgr->GetRole()->GetLevel() < cfg->level)
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return;
	}

	Team * m_team = m_module_mgr->GetRole()->GetMyTeam();
	if (NULL != m_team)
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return;
	}

	if (0 >= cfg->monster_group_id)
	{
		return;
	}

	BattleManagerLocal::GetInstance().StartBattleModeFightReq(m_module_mgr->GetRole(), BATTLE_MODE_REALM_BREAK_FIGHT, cfg->monster_group_id, 0, seq, true);

	gamelog::g_Log_realm_break.printf(LL_INFO, "%s user[%d, %s] cur_seq[%d] fight_seq[%u]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		m_param.cur_fight_seq, seq);
}

void RealmBreak::SendAllInfo()
{
	Protocol::SCRealmBreakAllInfo info;
	info.cur_seq = m_param.cur_seq;
	info.tuna_num = m_param.tuna_num;
	info.cur_break_exp = m_param.break_exp;
	info.cumulation_user_exp_count = m_param.cumulation_user_exp_count;
	info.user_cumulation_pratice_time = m_param.user_cumulation_pratice_time;
	info.cumulation_user_coin_count = m_param.cumulation_user_coin_count;
	info.cur_fight_seq = m_param.cur_fight_seq;
	info.reserver_sh = 0;
	
	m_module_mgr->NetSend(&info, sizeof(info));	
}

void RealmBreak::SendItemInfo()
{
	Protocol::SCRealmBreakItemInfo info;
	info.item_count = m_param.item_count;
	for (int i = 0; i < m_param.item_count && i < MAX_REALM_BREAK_ITEM_NUM; i++)
	{
		info.cumulation_item_list[i] = m_param.cumulation_item_list[i];
	}

	int length = sizeof(info) - (MAX_REALM_BREAK_ITEM_NUM - info.item_count) * sizeof(info.cumulation_item_list[0]);
	m_module_mgr->NetSend(&info, length);
}

void RealmBreak::GaiMingReissue()
{
	if (!m_param.is_reissue)
	{
		m_param.is_reissue = 1;

		const std::vector<ItemConfigData>* reward_list = LOGIC_CONFIG->GetRealmBreakConfig()->GetGaiMingReissueCfg(m_param.cur_fight_seq);
		if (NULL == reward_list) return;

		{
			static MailContentParam contentparam;
			contentparam.Reset();

			std::string logstr;
			for (size_t i = 0; i < reward_list->size() && i < ARRAY_LENGTH(contentparam.item_list); ++i)
			{
				contentparam.item_list[i].item_id = (*reward_list)[i].item_id;
				contentparam.item_list[i].is_bind = (*reward_list)[i].is_bind ? 1 : 0;
				contentparam.item_list[i].num = (*reward_list)[i].num;

				logstr += STRING_SPRINTF("[%d, %d]", (*reward_list)[i].item_id, (*reward_list)[i].num);
			}

			int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_return_item_subject15);
			int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_return_item_content15);
			if (length1 > 0 && length2 > 0)
			{
				MailRoute::MailToUser(IntToUid(m_module_mgr->GetUid()), MAIL_REASON_DEFAULT, contentparam);

				gamelog::g_Log_realm_break.printf(LL_INFO, "%s role[%d, %s] is_reissue[%d] reward[%s]", __FUNCTION__,
					m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), (int)m_param.is_reissue, logstr.c_str());
			}
		}
	}
}

void RealmBreak::FillItemVec(std::vector<ItemConfigData>& item_list)
{
	if(m_param.item_count <= 0) return ;

	for (int i = 0; i < MAX_REALM_BREAK_ITEM_NUM && i < m_param.item_count; ++i)
	{
		const ItemBase * item_base = ITEMPOOL->GetItem(m_param.cumulation_item_list[i].item_id);
		if(NULL == item_base) continue;

		ItemConfigData item_data(m_param.cumulation_item_list[i].item_id, m_param.cumulation_item_list[i].is_bind, m_param.cumulation_item_list[i].num);
		item_list.push_back(item_data);
	}
}

bool RealmBreak::DepositItem(int item_num, RealmBreakItem * item_list)
{
	if(item_num <= 0 || NULL == item_list || m_param.item_count < 0 || m_param.item_count >= MAX_REALM_BREAK_ITEM_NUM) return false;

	bool is_change = false;
	for (int i = 0; i < item_num && i < MAX_REALM_BREAK_ITEM_NUM; i++)
	{
		bool is_merge = false;
		for (int k = 0; k < m_param.item_count && k < MAX_REALM_BREAK_ITEM_NUM; ++k)
		{
			if (m_param.cumulation_item_list[k].item_id == item_list[i].item_id && m_param.cumulation_item_list[k].is_bind == item_list[i].is_bind)
			{
				is_merge = true;
				m_param.cumulation_item_list[k].num += item_list[i].num;
				is_change = true;
				break;
			}
		}
		if (!is_merge && m_param.item_count < MAX_REALM_BREAK_ITEM_NUM)
		{
			m_param.cumulation_item_list[m_param.item_count++] = item_list[i];
			is_change = true;
		}
	}
	return is_change;
}
