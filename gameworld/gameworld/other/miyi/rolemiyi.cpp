#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/msgmiyi.h"
#include "engineadapter.h"
#include "servercommon/errornum.h"
#include "npc/npcmanager.h"
#include "other/capability/capability.hpp"

#include "rolemiyi.hpp"
#include "other/miyi/miyiconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "gameworld/gamelog.h"
#include "servercommon/logdef.h"
#include "other/route/mailroute.hpp"
#include "item/itembase.h"
#include "servercommon/string/gameworldstr.h"
#include "other/poker/pokermodulemanager.hpp"

MiYi::MiYi() :m_role_module_mgr(NULL)
{
}

MiYi::~MiYi()
{
}

void MiYi::Init(RoleModuleManager * mgr, const MiYiParam & param)
{
	m_role_module_mgr = mgr;
	m_param = param;
}

void MiYi::GetInitParam(MiYiParam * param)
{
	*param = m_param;
}

void MiYi::OnWeekChange()
{
	const std::map<int, MiYiLevelCfg> & all_level_cfg = LOGIC_CONFIG->GetMiYiConfig()->GetAllLevelCfg();
	for (std::map<int, MiYiLevelCfg> ::const_iterator iter = all_level_cfg.begin(); iter != all_level_cfg.end(); ++iter)
	{
		const MiYiLevelCfg * cfg = &iter->second;
		int seq = cfg->seq;
		if (seq < 0 || seq >= MI_YI_MAX_LEVEL)
		{
			continue;;
		}

		for (int i = 0; i < (int)cfg->monsters.size(); ++i)
		{
			const MiYiMonsterCfg * monster_cfg = &cfg->monsters[i];
			int monster_index = monster_cfg->monster_seq;
			if (monster_index < 0 || monster_index >= MI_YI_MAX_MONSTER_NUM)
			{
				continue;
			}

			if (!this->IsWeekKilled(seq, monster_index))
			{
				continue;
			}

			if (m_param.fetch_flag[seq] & (1 << monster_index))
			{
				continue;
			}

			// 发邮件
			static MailContentParam contentparam; contentparam.Reset();
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = monster_cfg->add_exp;
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = monster_cfg->add_coin;
			FillMailContentParam(contentparam, monster_cfg->week_first_kill_reward);

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_mi_yi_first_kill_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_mi_yi_first_kill_content, NpcManager::Instance().GetMonsterIdByNpcId(monster_cfg->npc_monster_id));
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}


	for (int i = 0; i < MI_YI_MAX_LEVEL; ++i)
	{
		for (int z = 0; z < MI_YI_MAX_MONSTER_NUM; ++z)
		{
			m_param.pass_info[i][z] &= (1<< MI_YI_PASS_BIT_INFO_FIRST_PASS);
		}

		m_param.fetch_flag[i] = 0;
	}
	m_param.weekly_pass_count = 0;
	this->SendMiyiInfo();
	this->SendInfo();
}

void MiYi::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.day_pass_count = 0;
	this->SendMiyiInfo();
}

bool MiYi::IsFirstKill(int seq, int which_monster)
{
	if (seq < 0 || seq >= MI_YI_MAX_LEVEL)
	{
		return false;
	}

	if (which_monster < 0 || which_monster >= MI_YI_MAX_MONSTER_NUM)
	{
		return false;
	}

	if (m_param.pass_info[seq][which_monster] & (1 << MI_YI_PASS_BIT_INFO_FIRST_PASS))
	{
		return false;
	}

	return true;
}

bool MiYi::IsWeekKilled(int seq, int which_monster)
{
	if (seq < 0 || seq >= MI_YI_MAX_LEVEL)
	{
		return false;
	}

	if (which_monster < 0 || which_monster >= MI_YI_MAX_MONSTER_NUM)
	{
		return false;
	}

	return m_param.pass_info[seq][which_monster] & (1 << MI_YI_PASS_BIT_INFO_PASS);
}

bool MiYi::IsWeekKilled(int monster_group_id)
{
	const MiYiSimpleCfg * cfg = LOGIC_CONFIG->GetMiYiConfig()->GetMiYiLevelSimpleCfg(monster_group_id);
	if (NULL == cfg)
	{
		return false;
	}

	return this->IsWeekKilled(cfg->seq, cfg->monster_seq);
}

void MiYi::SetKill(int seq, int which_monster)
{
	if (seq < 0 || seq >= MI_YI_MAX_LEVEL)
	{
		return;
	}

	if (which_monster < 0 || which_monster >= MI_YI_MAX_MONSTER_NUM)
	{
		return;
	}

	if (this->IsFirstKill(seq,which_monster))
	{
		ROLE_LOG_QUICK6(LOG_TYPE_MIYI, m_role_module_mgr->GetRole(), seq, which_monster, __FUNCTION__, NULL);
	}

	m_param.pass_info[seq][which_monster] |= (1 << MI_YI_PASS_BIT_INFO_PASS);
	m_param.pass_info[seq][which_monster] |= (1 << MI_YI_PASS_BIT_INFO_FIRST_PASS);

	this->SendInfo();

	m_role_module_mgr->GetRole()->LogActive(LOG_ACTIVE_TYPE_MIYI);
}

bool MiYi::CheckLayerFinish(int seq)
{
	if(seq < 0 || seq >= MI_YI_MAX_LEVEL) return false;

	bool is_true = true;
	const MiyiOtherCfg & other_cfg = LOGIC_CONFIG->GetMiYiConfig()->GetMiYiOtherCfg();
	for (int i = 0; i < MI_YI_MAX_MONSTER_NUM && i < other_cfg.monster_num; i++)
	{
		if (m_param.pass_info[seq][i] & (1 << MI_YI_PASS_BIT_INFO_FIRST_PASS))
		{
			continue;
		}
		is_true = false;
	}

	return is_true;
}

int MiYi::GetCurLayerFinishNum(int seq)
{
	if (seq < 0 || seq >= MI_YI_MAX_LEVEL) return 0;

	int finish_num = 0;
	const MiyiOtherCfg & other_cfg = LOGIC_CONFIG->GetMiYiConfig()->GetMiYiOtherCfg();
	for (int i = 0; i < MI_YI_MAX_MONSTER_NUM && i < other_cfg.monster_num; i++)
	{
		if (m_param.pass_info[seq][i] & (1 << MI_YI_PASS_BIT_INFO_FIRST_PASS))
		{
			finish_num++;;
		}
	}
	return finish_num;
}

void MiYi::OnLogin()
{
// 	// 重连补发奖励
// 	MiYiPoker *cur_role_poker = POKER_MODULE.Get<MiYiPoker,TeamPokerRewardInfoTemp>(m_role_module_mgr->GetRole()->GetUID(),-1);
// 	if (cur_role_poker == NULL) return;
// 	if (cur_role_poker->is_can_turn == 1 && cur_role_poker->is_turned == 0)
// 	{
// 		static MailContentParam contentparam;
// 		contentparam.Reset();
// 		const ItemBase *item = ITEMPOOL->GetItem(cur_role_poker->reward_id);
// 		if (!item) return;
// 
// 		const ItemBase *f_item = ITEMPOOL->GetItem(cur_role_poker->fixed_reward.reward_id);
// 		if (!f_item) return;
// 
// 		int length_1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_miyi_poker_title);
// 		int length_2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_miyi_poker_reward_content);
// 
// 		contentparam.item_list[0] = ItemDataWrapper(item->GetItemId(), cur_role_poker->reward_count, item->IsBind());
// 		contentparam.item_list[1] = ItemDataWrapper(f_item->GetItemId(), cur_role_poker->fixed_reward.num, f_item->IsBind());
// 
// 		if (length_1 > 0 && length_2 > 0)
// 		{
// 			cur_role_poker->is_can_turn = 0;
// 			cur_role_poker->is_turned = 1;
// 			MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
// 		}
// 	}
}

void MiYi::AddPassCount()
{
	m_param.weekly_pass_count += 1;
	m_param.day_pass_count += 1;
	this->SendMiyiInfo();
}

void MiYi::GmSetWeeklyPassCount(int pass_count)
{
	m_param.weekly_pass_count = pass_count;
	this->SendMiyiInfo();
}

void MiYi::GmSetDayPassCount(int pass_count)
{
	m_param.day_pass_count = pass_count;
	this->SendMiyiInfo();
}

bool MiYi::CanGetXiaYiZhi()
{
	const MiyiOtherCfg & other_cfg = LOGIC_CONFIG->GetMiYiConfig()->GetMiYiOtherCfg();
	return m_param.day_pass_count >= other_cfg.get_xia_yi_zhi_times;  //超过次数本应>，但因为给侠义值的在添加次数前面 >=
}

void MiYi::SendInfo()
{
	Protocol::SCMiYiInfo protocol;
	for (int i = 0; i < MI_YI_MAX_LEVEL; ++i)
	{
		for (int z = 0; z < MI_YI_MAX_MONSTER_NUM; ++z)
		{
			protocol.pass_info[i][z] = m_param.pass_info[i][z];
		}

		protocol.fetch_flag[i] = m_param.fetch_flag[i];
	}

	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), &protocol, sizeof(protocol));
}

void MiYi::SendMiyiInfo()
{
	Protocol::SCMiYiPokerInfo info;
	info.weekly_pass_count = m_param.weekly_pass_count;
	info.day_pass_count = m_param.day_pass_count;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void MiYi::FetchWeekFirstKillReward(int scene_id, int monster_npc_id)
{
	const MiYiLevelCfg * cfg = LOGIC_CONFIG->GetMiYiConfig()->GetMiYiLevelCfgBySceneId(scene_id);
	if (cfg == NULL)
	{
		return;
	}

	const MiYiMonsterCfg * monster_cfg = cfg->GetMiYiMonsterCfg(monster_npc_id);
	if (monster_cfg == NULL)
	{
		return;
	}

	int seq = cfg->seq;
	int monster_index = monster_cfg->monster_seq;
	if (seq < 0 || seq >= MI_YI_MAX_LEVEL)
	{
		return;
	}


	if (monster_index < 0 || monster_index >= MI_YI_MAX_MONSTER_NUM)
	{
		return;
	}

	if (m_param.fetch_flag[seq] & (1 << monster_index))
	{
		return;
	}

	if (!this->IsWeekKilled(seq, monster_index))
	{
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->PutVec(monster_cfg->week_first_kill_reward, PUT_REASON_MI_YI_FETCH_WEEK_KILL_REWARD))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_role_module_mgr->GetRole()->AddExp(monster_cfg->add_exp);
	m_role_module_mgr->GetMoney()->AddCoinBind(monster_cfg->add_coin, __FUNCTION__);
	m_param.fetch_flag[seq] |= (1 << monster_index);

	this->SendInfo();
}

void MiYi::SaoDang(int scene_id)
{
	const MiYiLevelCfg * cfg = LOGIC_CONFIG->GetMiYiConfig()->GetMiYiLevelCfgBySceneId(scene_id);
	if (cfg == NULL)
	{
		return;
	}

	//先判断评分有无达到要求，限制等级,再判断本层有无全通关，
	//最后一关不让扫荡
	int role_capability = m_role_module_mgr->GetRole()->GetCapabilityWithFightPetAndPartner();
	int role_level = m_role_module_mgr->GetRole()->GetLevel();
	if (role_capability < cfg->sao_dang_capability)
	{
		return;
	}

	if (role_level < cfg->sao_dang_need_role_level)
	{
		return;
	}

	for (int i = 0; i < cfg->monsters.size(); ++i)
	{
		const MiYiMonsterCfg & monster_cfg = cfg->monsters[i];
		int seq = cfg->seq;
		int monster_index = monster_cfg.monster_seq;

		if (seq < 0 || seq >= MI_YI_MAX_LEVEL)
		{
			return;
		}

		if (monster_index < 0 || monster_index >= MI_YI_MAX_MONSTER_NUM)
		{
			return;
		}

		if (this->IsFirstKill(seq, monster_index))
		{
			return;
		}
	}

	for (int i = 0; i < cfg->monsters.size(); ++i)
	{
		const MiYiMonsterCfg & monster_cfg = cfg->monsters[i];
		int seq = cfg->seq;
		int monster_index = monster_cfg.monster_seq;
		this->SetKill(seq, monster_index);
	}

	this->SendInfo();
}
