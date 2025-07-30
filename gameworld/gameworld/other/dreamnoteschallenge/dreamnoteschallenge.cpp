#include "dreamnoteschallenge.hpp"
#include "engineadapter.h"
#include "gamelog.h"

#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/dreamnoteschallengeconfig.hpp"

#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "protocol/msgdreamnoteschallenge.hpp"

#include "item/money.h"
#include "item/knapsack.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "other/route/mailroute.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"

#include "global/rank/rankmanager.hpp"


DreamNotesChallenge::DreamNotesChallenge() :m_role_module_mgr(NULL)
{
}

DreamNotesChallenge::~DreamNotesChallenge()
{
}

void DreamNotesChallenge::Init(RoleModuleManager * module_mgr, const DreamNotesChallengeParam & param)
{
	m_role_module_mgr = module_mgr;
	m_param = param;
	if (1 != m_param.is_set_times)
	{
		m_param.remaining_times = LOGIC_CONFIG->GetDreamNotesChallengeConfig()->GetOtherCfg().pass_time_limit;
		m_param.is_set_times = 1;
	}
}

void DreamNotesChallenge::GetInitParam(DreamNotesChallengeParam * param)
{
	*param = m_param;
}

void DreamNotesChallenge::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if(old_dayid == now_dayid) return;

	const DreamNotesChallengeOtherCfg other_cfg = LOGIC_CONFIG->GetDreamNotesChallengeConfig()->GetOtherCfg();
	//this->OnReissueReward(m_param.remaining_times);
	//const CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	//if (common_data.offine_day > 1)
	//{
	//	int max_reissue_mail_num = ((common_data.offine_day <= MAX_DREAM_NOTES_REISSUE_MAIL_NUM) ? common_data.offine_day : MAX_DREAM_NOTES_REISSUE_MAIL_NUM) - 1;
	//	for (int i = 0; i < max_reissue_mail_num; i++)
	//	{
	//		this->OnReissueReward(other_cfg.pass_time_limit);
	//	}
	//}

	m_param.remaining_times = other_cfg.pass_time_limit;
	m_param.buy_times = 0;
	m_param.use_item_add_times = 0;
	this->SendInfo();
}

void DreamNotesChallenge::OnRoleLogin()
{
	//合服的时候排行榜数据只保留母游戏服
	RankManager::Instance().SyncPersonRankInfo(m_role_module_mgr->GetRole(), PERSON_RANK_TYPE_DREAM_NOTES_CHALLENGE);
}

bool DreamNotesChallenge::IsHasTimes()
{
	//if(m_param.remaining_times <= 0)
	//{
	//	return false;
	//}

	return true;
}

bool DreamNotesChallenge::IsCanChallenge(int level)
{
	if (m_param.pass_level + 1 < level)
	{
		return false;
	}
	return true;
}

void DreamNotesChallenge::SendPassReward(int level)
{
	const DreamNotesChallengeCfg * scene_cfg = LOGIC_CONFIG->GetDreamNotesChallengeConfig()->GetChallengeFbCfg(level);
	if (NULL == scene_cfg)
	{
		return;
	}
	if (!this->IsCanChallenge(level))
	{
		return;
	}

	std::vector<ItemConfigData> reward_list;

	//随机检查是否有伙伴碎片
	if (scene_cfg->partner_fragment_rate > 0)
	{
		int rate_weight = rand() % 10000;
		if (rate_weight < scene_cfg->partner_fragment_rate)
		{
			reward_list.insert(reward_list.end(), scene_cfg->partner_fragment_reward.begin(), scene_cfg->partner_fragment_reward.end());
		}
	}

	//首通关卡不消耗次数(2022-6-8 取消该逻辑)
	bool is_first = false;
	bool is_first_pass = false;
	int add_yuanling = 0;
	int add_lucky = 0;
	int add_exp = 0;
	if (level == m_param.pass_level + 1)
	{
		reward_list.insert(reward_list.end(), scene_cfg->first_pass_reward.begin(), scene_cfg->first_pass_reward.end());
		m_param.pass_level = level;
		m_param.pass_new_level_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		add_yuanling = scene_cfg->first_yuanling;
		add_lucky = scene_cfg->first_lucky;
		is_first = true;
// 		is_first_pass = true;
	}
	else
	{
		m_param.remaining_times--;
		reward_list.insert(reward_list.end(), scene_cfg->common_reward.begin(), scene_cfg->common_reward.end());
		add_yuanling = scene_cfg->common_yuanling;
		add_lucky = scene_cfg->lucky;
		m_role_module_mgr->GetMoney()->AddCoinBind(scene_cfg->coin, __FUNCTION__, false, is_first_pass);
		add_exp = LOGIC_CONFIG->GetDreamNotesChallengeConfig()->GetChallengeExpCfg(m_role_module_mgr->GetRole()->GetLevel()) * scene_cfg->power_expend;
	}
	m_role_module_mgr->GetMoney()->AddOtherCoin(add_yuanling, MONEY_TYPE_YUAN_SUI_ZHI_LING, __FUNCTION__,false, is_first_pass);
	m_role_module_mgr->GetMoney()->AddOtherCoin(add_lucky, MONEY_TYPE_LUCK_COIN, __FUNCTION__, false, is_first_pass);
	if (add_exp > 0)
	{
		m_role_module_mgr->GetRole()->AddExp(add_exp, ADD_EXP_TYPE_DREAM_NOTES_CHALLENGE, __FUNCTION__);
		EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_role_module_mgr->GetRole(), add_exp, __FUNCTION__);
	}

	bool is_put_succ = m_role_module_mgr->GetKnapsack()->PutList((short)reward_list.size(), &reward_list[0], PUT_REASON_DREAM_NOTES_CHALLENGE, !is_first_pass);
	if (!is_put_succ)
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward_list[0], (short)reward_list.size(), SEND_MAIL_TYPE_DREAM_NOTES_CHALLENGE, !is_first_pass);
	}
	m_role_module_mgr->GetKnapsack()->NotifyGetItemList((short)reward_list.size(), &reward_list[0], GET_REASON_DREAM_NOTES_CHALLENGE_FINISH, level, is_put_succ ? 1 : 0);

	if (!is_first_pass)	//首次通关不消耗体力
	{
		this->OnRoleGetFBBossReward();
	}
	if (is_first)
	{
		RankManager::Instance().SyncPersonRankInfo(m_role_module_mgr->GetRole(), PERSON_RANK_TYPE_DREAM_NOTES_CHALLENGE);
	}
	this->SendInfo();
	EventHandler::Instance().OnPassDreamNotesChallenge(m_role_module_mgr->GetRole(), level, is_first);
	gamelog::g_log_dream_notes_challenge.printf(LL_INFO, "%s user[%d, %s] pass_level[%d] remaining_times[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), 
		level, m_param.remaining_times);
}

void DreamNotesChallenge::OnFastPassFb(int level)
{
	if (!this->IsHasTimes())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_DREAM_NOTES_CHALLENGE_NOT_TIMES);
		return;
	}
	if (level > m_param.pass_level)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_DREAM_NOTES_CHALLENGE_NOT_FAST_PASS);
		return;
	}
	if (!Scene::IsStaticScene(m_role_module_mgr->GetRole()->GetScene()->GetSceneType()))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_SCENE_NOT_CAN_FAST_PASS_FB);
		return;
	}

	const DreamNotesChallengeCfg * scene_cfg = LOGIC_CONFIG->GetDreamNotesChallengeConfig()->GetChallengeFbCfg(level);
	if (NULL == scene_cfg)
	{
		return;
	}
	if (m_role_module_mgr->GetRole()->GetCapabilityWithFightPetAndPartner() < scene_cfg->recommend_score)
	{
		m_role_module_mgr->OnSendScoreHintNotice(scene_cfg->recommend_score);
		return;
	}

	if (!m_role_module_mgr->GetRoleCrystalFb()->UseCrystalFbEnergy(scene_cfg->power_expend, __FUNCTION__, ENTER_ENERGY_CHANGE_TYPE_DREAM_NOTES_FAST))
	{
		return;
	}

	std::vector<ItemConfigData> reward_list;

	//随机检查是否有伙伴碎片
	if (scene_cfg->partner_fragment_rate > 0)
	{
		int rate_weight = rand() % 10000;
		if (rate_weight < scene_cfg->partner_fragment_rate)
		{
			reward_list.insert(reward_list.end(), scene_cfg->partner_fragment_reward.begin(), scene_cfg->partner_fragment_reward.end());
		}
	}

	reward_list.insert(reward_list.end(), scene_cfg->common_reward.begin(), scene_cfg->common_reward.end());
	m_role_module_mgr->GetMoney()->AddCoinBind(scene_cfg->coin, __FUNCTION__);
	m_role_module_mgr->GetMoney()->AddOtherCoin(scene_cfg->lucky, MONEY_TYPE_LUCK_COIN, __FUNCTION__);
	m_role_module_mgr->GetMoney()->AddOtherCoin(scene_cfg->common_yuanling, MONEY_TYPE_YUAN_SUI_ZHI_LING, __FUNCTION__);
	int add_exp = LOGIC_CONFIG->GetDreamNotesChallengeConfig()->GetChallengeExpCfg(m_role_module_mgr->GetRole()->GetLevel()) * scene_cfg->power_expend;
	if (add_exp > 0)
	{
		m_role_module_mgr->GetRole()->AddExp(add_exp, ADD_EXP_TYPE_DREAM_NOTES_CHALLENGE, __FUNCTION__);
		EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_role_module_mgr->GetRole(), add_exp, __FUNCTION__);
	}

	bool b = m_role_module_mgr->GetKnapsack()->PutList((short)reward_list.size(), &reward_list[0], PUT_REASON_DREAM_NOTES_CHALLENGE);
	if (!b)
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward_list[0], (short)reward_list.size(), SEND_MAIL_TYPE_DREAM_NOTES_CHALLENGE);
	}
	m_role_module_mgr->GetKnapsack()->NotifyGetItemList((short)reward_list.size(), &reward_list[0], GET_REASON_DREAM_NOTES_CHALLENGE_FAST_PASS, level, b ? 1 : 0);

	m_param.remaining_times--;
	this->SendInfo();
	EventHandler::Instance().OnPassDreamNotesChallenge(m_role_module_mgr->GetRole(), level, false, true);
	gamelog::g_log_dream_notes_challenge.printf(LL_INFO, "%s user[%d, %s] pass_level[%d] remaining_times[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		level, m_param.remaining_times);
}

void DreamNotesChallenge::OnBuyTimes(int buy_times)
{
	if(buy_times <= 0) return; 

	int max_buy_times = LOGIC_CONFIG->GetDreamNotesChallengeConfig()->GetMaxBuyTimes();
	if (m_param.buy_times >= max_buy_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_DREAM_NOTES_CHALLENGE_BUY_TIMES_LIMIT);
		return;
	}
	if (m_param.buy_times + buy_times > max_buy_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
		return;
	}
	std::map<int, int> consume_price_list;		//key:货币类型
	for (int i = 1; i <= buy_times; i++)
	{
		const DreamNotesChallengeBuyCfg * price_cfg = LOGIC_CONFIG->GetDreamNotesChallengeConfig()->GetPriceByBuyTimes(m_param.buy_times + i);
		if (NULL == price_cfg)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
		consume_price_list[price_cfg->price_type] += price_cfg->price;
	}	

	//因为是多个,所以需要先循环检测是否满足,然后再消耗
	for (std::map<int, int>::iterator it = consume_price_list.begin(); it != consume_price_list.end(); it++)
	{
		switch (it->first)
		{
		case MONEY_TYPE_GOLD:
		{
			if (!m_role_module_mgr->GetMoney()->GoldMoreThan(it->second))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
		}
		break;
		case MONEY_TYPE_IMMORTAL_COIN:
		{
			if (!m_role_module_mgr->GetMoney()->ImmortalCoinMoreThan(it->second))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_IMMORTAL_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
		case MONEY_TYPE_COIN_BIND:
		{
			if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(it->second))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
		default:
		{
			if (!m_role_module_mgr->GetMoney()->OtherCoinMoreThan(it->second, it->first))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_DREAM_NOTES_CHALLENGE_BUY_ERROR);
				return;
			}
		}
		break;
	}
	}

	for (std::map<int, int>::iterator it = consume_price_list.begin(); it != consume_price_list.end(); it++)
	{
		switch (it->first)
		{
		case MONEY_TYPE_GOLD:
		{
			if (!m_role_module_mgr->GetMoney()->UseGold(it->second, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
		}
		break;
		case MONEY_TYPE_IMMORTAL_COIN:
		{
			if (!m_role_module_mgr->GetMoney()->UseImmortalCoin(it->second, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_IMMORTAL_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
		case MONEY_TYPE_COIN_BIND:
		{
			if (!m_role_module_mgr->GetMoney()->UseCoinBind(it->second, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
		default:
		{
			if (!m_role_module_mgr->GetMoney()->UseOtherCoin(it->second, it->first, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_DREAM_NOTES_CHALLENGE_BUY_ERROR);
				return;
			}
		}
		break;
		}
	}

	m_param.buy_times += buy_times;
	m_param.remaining_times += buy_times;
	this->SendInfo();
	gamelog::g_log_dream_notes_challenge.printf(LL_INFO, "%s user[%d, %s] buy_times[%d] remaining_times[%d] buy_count[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), 
		m_role_module_mgr->GetRole()->GetName(), buy_times, m_param.remaining_times, m_param.buy_times);
}

void DreamNotesChallenge::OnUseItemAddTimes(int add_times)
{
	if(add_times <= 0) return;

	m_param.remaining_times += add_times;
	m_param.use_item_add_times += add_times;
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_dream_notes_challenge_use_item_add_times_notice, add_times);
	if (length > 0)
	{
		World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(m_role_module_mgr->GetRole(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
	}
	this->SendInfo();
	gamelog::g_log_dream_notes_challenge.printf(LL_INFO, "%s user[%d, %s] add_times[%d] remaining_times[%d] use_item_add_times[%d]", __FUNCTION__, 
		m_role_module_mgr->GetRole()->GetUID(),m_role_module_mgr->GetRole()->GetName(), add_times, m_param.remaining_times, m_param.use_item_add_times);
}

void DreamNotesChallenge::OnReissueReward(int count)
{
	if(count <= 0) return;

	const DreamNotesChallengeCfg * scene_cfg = LOGIC_CONFIG->GetDreamNotesChallengeConfig()->GetChallengeFbCfg(m_param.pass_level);
	if (NULL == scene_cfg)
	{
		return;
	}
	static MailContentParam contentparam; contentparam.Reset();
	int index = 0;
	for (int i = 0; i < (int)scene_cfg->common_reward.size(); i++)
	{
		const ItemConfigData & reward = scene_cfg->common_reward[i];
		const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);
		if (item_base == NULL)
		{
			continue;
		}
		if (index >= MAX_ATTACHMENT_ITEM_NUM)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
			index = 0;
			memset(contentparam.item_list, 0, sizeof(contentparam.item_list));
		}
		contentparam.item_list[index].item_id = reward.item_id;
		contentparam.item_list[index].num = reward.num * count;
		contentparam.item_list[index].is_bind = (reward.is_bind) ? 1 : 0;
		index++;
	}

	contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] += scene_cfg->coin * count;
	contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_LUCK_COIN] += scene_cfg->lucky * count;
	contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_YUAN_SUI_ZHI_LING] += scene_cfg->common_yuanling * count;
	int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_dream_notes_challenge_subject);
	int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_dream_notes_challenge_reissue);
	if (lensub > 0 && length > 0) MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);

}

bool DreamNotesChallenge::IsPassLevel(int level)
{
	if (m_param.pass_level < level)
	{
		return false;
	}
	return true;
}

void DreamNotesChallenge::GmSetPassLevel(int level)
{
	m_param.pass_level = level;
	this->SendInfo();
}

void DreamNotesChallenge::OnRoleEnterScene()
{
	Scene* scene = m_role_module_mgr->GetRole()->GetScene();
	if (NULL == scene || !scene->IsInStaticScene()) return;

	if (m_param.ticket_data.has_give_ticket > 0)
	{
		if (0 == m_param.ticket_data.has_get_boss_reward)
		{
			m_role_module_mgr->GetRoleCrystalFb()->AddCrystalFbEnergy(m_param.ticket_data.consume_energy, __FUNCTION__, ENTER_ENERGY_CHANGE_TYPE_ADVANCE_DREAM_NOTES, false);
		}
		m_param.ticket_data.ResetTicketInfo();
	}
}

void DreamNotesChallenge::OnRoleEnterFBUseEnergy(int use_energy_num)
{
	m_param.ticket_data.ResetTicketInfo();
	m_param.ticket_data.has_give_ticket = 1;
	m_param.ticket_data.consume_energy = use_energy_num;
}

void DreamNotesChallenge::OnRoleGetFBBossReward()
{
	m_param.ticket_data.has_get_boss_reward = 1;
	EventHandler::Instance().OnSpecialUseEnergy(m_role_module_mgr->GetRole(), m_param.ticket_data.consume_energy, ENTER_ENERGY_CHANGE_TYPE_DREAM_NOTES_PASS);
}

void DreamNotesChallenge::SendInfo()
{
	Protocol::SCDreamNotesChallengeInfo info;
	info.pass_level = m_param.pass_level;
	info.remaining_times = m_param.remaining_times;
	info.buy_times = m_param.buy_times;
	info.use_item_add_times = m_param.use_item_add_times;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}
