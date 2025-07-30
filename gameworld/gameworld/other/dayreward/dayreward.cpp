#include "item/itempool.h"
#include "world.h"
#include "servercommon/errornum.h"
#include "dayreward.hpp"
#include "dayrewardconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "item/money.h"
#include "item/knapsack.h"
#include "protocol/msgother.h"
#include "engineadapter.h"
#include "other/trevifountionconfig.hpp"
#include "other/wst_question/wst_question.hpp"
#include "other/schooltask/schooltask.h"
#include "other/funopen/funopen.hpp"
#include "other/researchtask/researchtask.h"
#include "other/treasure_map/treasuremap.h"
#include "other/bountytask/bountytask.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "other/activesystem/activesystem.hpp"
#include "other/transactiontask/transactiontask.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "config/activityconfig/guildanswer/activityguildanswerconfig.hpp"
#include "config/activityconfig/guildfight/activityguildfightconfig.hpp"
#include "config/activityconfig/onlyfight/activityonlyfightconfig.hpp"
#include "config/activityconfig/teamfight/activityteamfightconfig.hpp"
#include "config/activityconfig/silkroads/activitysilkroadsconfig.hpp"
#include "servercommon/logdef.h"
#include "gameworld/gamelog.h"
#include "global/activity/activityimpl/activitysilkroads.hpp"
#include "global/activity/activitymanager.hpp"

DayReward::DayReward() : month_zero_open_day(0)
{
}

DayReward::~DayReward()
{
}

void DayReward::Init(RoleModuleManager * role_module_manager, const RoleDayRewardParam & param)
{
	m_role_module_mgr = role_module_manager;
	m_param = param;

	if (0 == m_role_module_mgr->GetCommonData().sign_reward_next_month_reset_time)
	{
		unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_role_module_mgr->GetCommonData().sign_reward_next_month_reset_time = now + EngineAdapter::Instance().NextMouthInterval(1, 0, 0, 0);
	}
	const tm * local_time = EngineAdapter::Instance().LocalTime();
	if (m_role_module_mgr->GetCommonData().sign_count > local_time->tm_mday)
	{
		m_role_module_mgr->GetCommonData().sign_count = local_time->tm_mday - 1;
		m_role_module_mgr->GetCommonData().sign_reward_fetch_flag = 0;
	}
	month_zero_open_day = World::GetInstWorld()->GetOpenDayByTimestamp(m_role_module_mgr->GetRole()->GetUniqueServerID(), GetMonthZeroTime(EngineAdapter::Instance().Time()));
	if (m_param.online_gift_join_begin_timestamp > 0 && 0 != m_param.is_can_join_online_gift_flag)
	{
		m_param.online_gift_record_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	}

}

void DayReward::GetInitParam(RoleDayRewardParam * param)
{
	if (m_param.online_gift_join_begin_timestamp > 0 && 0 != m_param.is_can_join_online_gift_flag)
	{
		unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		int delta_time = static_cast<unsigned int>(now > m_param.online_gift_record_begin_timestamp ? now - m_param.online_gift_record_begin_timestamp : 0);
		m_param.online_gift_times += delta_time;
		m_param.online_gift_record_begin_timestamp = now;
	}

	*param = m_param;
}

void DayReward::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{
	
}

void DayReward::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	common_data.online_reward_fetch_flag = 0;
	common_data.day_food_flag.Reset();
	this->SendOnlineRewardInfo();
	this->SendDayFoodInfo();
	month_zero_open_day = World::GetInstWorld()->GetOpenDayByTimestamp(m_role_module_mgr->GetRole()->GetUniqueServerID(), GetMonthZeroTime(EngineAdapter::Instance().Time()));

	if (0 != common_data.sign_reward_fetch_flag)
	{
		common_data.sign_count++;
		common_data.sign_reward_fetch_flag = 0;
	}
	this->SendSignRewardInfo();
	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	for (int i = 0; i < DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_MAX; i++)
	{	  
		//找回时间内
		bool is_in_time = false;
		unsigned int can_find_end_timestamp = WorldStatus::Instance().GetFindEndTimestamp(i);
		if (0 != can_find_end_timestamp && now_second < can_find_end_timestamp && now_second >= can_find_end_timestamp - SECOND_PER_DAY)
		{
				is_in_time = true;
		}

		switch (i)
		{
			case DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_GUILD_ANSWER:
			{
				this->OnGuildAnswerFindDataReset(is_in_time, can_find_end_timestamp);
			}
			break;
			case DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_GUILD_FIGHT:
			{
				this->OnGuildFightFindDataReset(is_in_time, can_find_end_timestamp);
			}
			break;
			case DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_ONLY_FIGHT:
			{
				this->OnOnlyFightFindDataReset(is_in_time, can_find_end_timestamp);
			}
			break;
			case DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_TEAM_FIGHT:
			{
				this->OnTeamFightFindDataReset(is_in_time, can_find_end_timestamp);
			}
			break;
			case DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_SILK_ROADS:
			{
				this->OnSilkRoadsFindDataReset(can_find_end_timestamp);
			}
			break;
		default:
			break;
		}
	}
}

void DayReward::OnWeekChange()
{
}

void DayReward::OnMonthChange()
{
	
}

void DayReward::Update(unsigned int now_second)
{
	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	if (now_second >=  m_role_module_mgr->GetCommonData().sign_reward_next_month_reset_time)
	{
		common_data.sign_count = 0;
		common_data.sign_reward_fetch_flag = 0;
		this->SendSignRewardInfo();

		common_data.sign_reward_next_month_reset_time = now_second + EngineAdapter::Instance().NextMouthInterval(1, 0, 0, 0);
	}
}

void DayReward::OnRoleLogin()
{
//	m_role_module_mgr->GetCommonData().overflow_find_count = m_role_module_mgr->GetRoleCrystalFb()->GetOverflowEnergy();

	if (0 == m_param.is_old_user_flag)
	{
		m_param.is_old_user_flag = 1;
		if (m_role_module_mgr->GetCreateDays() <= 1)
		{
			m_param.is_can_join_online_gift_flag = 1;
		}
	}
	this->OnCheckOnlineGiftOpen();

	this->SendPetStrengthenInfo();

}

void DayReward::OnRoleLevelUp(int cur_level)
{
	this->OnCheckOnlineGiftOpen();
}

int DayReward::OnFetchOnlineReward(int index)
{
	const OnlineReward *cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetOnlineReward(index);
	if (NULL == cfg) return -1;

	// 判断是否领取
	if (0 != (m_role_module_mgr->GetCommonData().online_reward_fetch_flag & (1 << cfg->seq)))
	{
		return -2;
	}
	int open_day = World::GetInstWorld()->GetCurRealOpenDay(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if(cfg->section_start > open_day || cfg->section_end < open_day)  return -5;

	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	m_role_module_mgr->GetRole()->TodayOnlineStatiTime(now);
	unsigned int curr_online_time = m_role_module_mgr->GetRole()->GetTodayOnlineTime();
	int need_time = cfg->time;

	// 判断条件是否达成
	if (curr_online_time < (unsigned int)need_time)
	{
		return -3;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList(cfg->reward_count, cfg->reward))
	{
		return -4;
	}

	// 下发奖励
	m_role_module_mgr->GetKnapsack()->PutList(cfg->reward_count, cfg->reward, PUT_REASON_DAY_REWARD);
	m_role_module_mgr->GetMoney()->AddCoinBind(cfg->bind_coin, __FUNCTION__);
	m_role_module_mgr->GetCommonData().online_reward_fetch_flag |= (1 << cfg->seq);

	this->SendOnlineRewardInfo();
	return 0;
}

int DayReward::OnFetchSignReward(int day)
{
	if (day <= 0 || day > 31) return -1;
	int now_day = day - 1;
	const tm * l_tm = EngineAdapter::Instance().LocalTime();
	// 判断能否领取
	if (m_role_module_mgr->GetCommonData().sign_count < now_day)
	{
		return -1;
	}
	// 判断是否已领取
	if (0 != m_role_module_mgr->GetCommonData().sign_reward_fetch_flag)
	{
		return -2;
	}
	//每月签到用该月1号0点来获取对应时间段配置
	const SignReward * cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetSignReward(l_tm->tm_mon + 1, day, month_zero_open_day);
	if (NULL == cfg) return -3;

	// 下发奖励
	m_role_module_mgr->GetKnapsack()->Put(cfg->reward, PUT_REASON_DAY_REWARD);
	m_role_module_mgr->GetMoney()->AddCoinBind(cfg->bind_coin, __FUNCTION__);
	m_role_module_mgr->GetCommonData().sign_reward_fetch_flag = 1;

	this->SendSignRewardInfo();
	return 0;
}

int DayReward::OnFetchLevelReward(int seq)
{
	// 判断是否已领取
	if (0 !=(m_role_module_mgr->GetCommonData().level_reward_fetch_flag & (1 << seq)))
	{
		return -1;
	}
	const LevelReward * cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetLevelReward(seq);
	if (NULL == cfg) return -2;

	// 判断能否领取
	if (m_role_module_mgr->GetRole()->GetLevel() < cfg->level)
	{
		return -3;
	}

	std::vector<const ItemConfigData*> reward_list;
	if (0 == m_role_module_mgr->GetRole()->GetProfession())
	{
		reward_list.reserve(cfg->reward_count);
		for (int i = 0; i < cfg->reward_count; ++i)
		{
			// 当没有职业时，不下发转换物品
			const ItemBase* item_base = ITEMPOOL->GetItem(cfg->reward[i].item_id);
			if (NULL == item_base || I_TYPE_CONVERT_ITEM == item_base->GetItemType()) continue;
		
			reward_list.push_back(&cfg->reward[i]);
		}
	}
	else
	{
		reward_list.reserve(cfg->reward_count);
		for (int i = 0; i < cfg->reward_count; ++i)
		{
			reward_list.push_back(&cfg->reward[i]);
		}
	}

	if ((int)reward_list.size() > 0)
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)reward_list.size(), reward_list[0]))
		{
			return -4;
		}

		// 下发奖励
		if (!m_role_module_mgr->GetKnapsack()->PutList((short)reward_list.size(), reward_list[0], PUT_REASON_DAY_REWARD))
		{
			return -4;
		}
	}
	
	m_role_module_mgr->GetMoney()->AddCoinBind(cfg->bind_coin, __FUNCTION__);
	m_role_module_mgr->GetCommonData().level_reward_fetch_flag |= (1 << seq);
	this->SendLevelRewardInfo();
	return 0;
}

int DayReward::OnFindReward(int type, int count, int consume_type)
{
	if (type <= DAY_ACTIVITY_TYPE_MIN || type >= DAY_ACTIVITY_TYPE_MAX) return -99;
	if (count <= 0) return -98;
	//if (consume_type < 0 || consume_type >= RB_TYPE_MAX) return -97;
	if (consume_type != RB_TYPE_COIN) return -993;	//目前元宝配置移除

	const DayRewardFindCfg *cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(type);
	if (NULL == cfg) return -999;

	const DayRewardFindPriceCfg * price_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindRewardPriceCfg(type, m_role_module_mgr->GetRole()->GetLevel());
	if(NULL == price_cfg) return -992;

	int need_consume = 0;
	if(type != DAY_ACTIVITY_TYPE_OVERFLOW_ENERGY)		// 体力找回特殊处理，根据溢出体力值计算铜币
	{
		switch (consume_type)
		{
		case RB_TYPE_COIN:
			need_consume = (price_cfg->coin_price * count * price_cfg->coin_scale) / 100;
			if (!m_role_module_mgr->GetMoney()->AllCoinMoreThan(need_consume))
			{
				m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return -998;
			}
			break;
			/*case 1:
			need_consume = cfg->gold_price * count;
			if (!m_role_module_mgr->GetMoney()->GoldMoreThan(need_consume))
			{
				m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return -997;
			}
			break;*/
		}
	}

	UNSTD_STATIC_CHECK(20 == DAY_ACTIVITY_TYPE_MAX);
	int ret = 0;
	switch (type)
	{
	case DAY_ACTIVITY_TYPE_ZHEN_ZHONG:
		if (!m_role_module_mgr->GetResearchTask()->OnResearchTaskFindReward(consume_type, count))
		{
			ret = -type;
		}
		break;
	case DAY_ACTIVITY_TYPE_TREASURE_MAP_TASK:
		if (!m_role_module_mgr->GetTreasureMap()->RewardFindBack(consume_type, count))
		{
			ret = -type;
		}
		break;
	case DAY_ACTIVITY_TYPE_SCHOOL_TASK:
		{
			if (!m_role_module_mgr->GetSchoolTask()->OnSchoolTaskFindReward(consume_type, count))
			{
				ret = -type;
			}
		}
		break;
	case DAY_ACTIVITY_TYPE_WST_QUESTION:
		{
			if (!m_role_module_mgr->GetWstQuestion()->OnWstQuestionFindReward(consume_type, count))
			{
				ret = -type;
			}
		}
		break;
	case DAY_ACTIVITY_TYPE_TREVI_FOUNTAIN:
		{
			if (!m_role_module_mgr->OnTreviFountainFindReward(consume_type, count))
			{
				ret = -type;
			}
		}
		break;
	case DAY_ACTIVITY_TYPE_BOUNTY_TASK:
		{
			if (!m_role_module_mgr->GetBountyTask()->OnBountyTaskFindReward(consume_type, count))
			{
				ret = -type;
			}
		}
		break;
	case DAY_ACTIVITY_TYPE_FANTASY_FB:
		{
			if (!m_role_module_mgr->OnFantasyFindReward(consume_type, count))
			{
				ret = -type;
			}
		}
		break;
	case DAY_ACTIVITY_TYPE_GUILD_ANSWER:
		{
			if (!this->OnGuildAnswerFindReward(consume_type, count))
			{
				ret = -type;
			}
		}
		break;
	case DAY_ACTIVITY_TYPE_GUILD_FIGHT:
		{
			if (!this->OnGuildFightFindReward(consume_type, count))
			{
				ret = -type;
			}
		}
		break;	
	case DAY_ACTIVITY_TYPE_ONLY_FIGHT:
		{
			if (!this->OnOnlyFightFindReward(consume_type, count))
			{
				ret = -type;
			}
		}
		break;
	case DAY_ACTIVITY_TYPE_TEAM_FIGHT:
		{		
			if (!this->OnTeamFightFindReward(consume_type, count))
			{
				ret = -type;
			}
		}
		break;
	case DAY_ACTIVITY_TYPE_TRANSACTION_TASK:
		{
			if (!m_role_module_mgr->GetTransactionTask()->OnTransactionTaskFindReward(consume_type, count))
			{
				ret = -type;
			}
		}
		break;
	case DAY_ACTIVITY_TYPE_OVERFLOW_ENERGY:
		{
			need_consume = this->OnOverFolowEnergyFindReward();
			if (need_consume <= 0)
			{
				ret = -type;
			}
		}
		break;
	case DAY_ACTIVITY_TYPE_WAN_LING_FB_ACTIVE:
	case DAY_ACTIVITY_TYPE_CRYSTAL_XI_YOU_ACTIVE:
	case DAY_ACTIVITY_TYPE_CLOUD_ARENA_ACTIVE:
	case DAY_ACTIVITY_TYPE_ZHEN_ZHONG_ACTIVE:
	case DAY_ACTIVITY_TYPE_BOUNTY_TASK_ACTIVE:
		{
			if (!m_role_module_mgr->GetActiveSystem()->OnFindReward(type, count))
			{
				ret = -type;
			}
		}
		break;
	case DAY_ACTIVITY_TYPE_SILK_ROADS_ACTIVE:
		{
			if (!this->OnSilkRoadsFindReward(consume_type, count))
			{
				ret = -type;
			}
		}
		break;
	}
	if (ret < 0) return ret;

	switch (consume_type)
	{
	case RB_TYPE_COIN:
		m_role_module_mgr->GetMoney()->UseAllCoin(need_consume, __FUNCTION__);
		break;
	/*case 1:
		m_role_module_mgr->GetMoney()->UseGold(need_consume, __FUNCTION__);
		break;*/
	}

	this->SendFindRewardInfo();
	return 0;
}

int DayReward::OnFetchDayFood(int seq)
{
	if(!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_DAY_FOOD, true)) return -99;

	const DayRewardDayFoodCfg * food_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetDayFoodCfg(seq);
	if (NULL == food_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return -1;
	}
	CommonDataParam & common_data  = m_role_module_mgr->GetCommonData();
	if (common_data.day_food_flag.IsBitSet(seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return -2;
	}
	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	unsigned int zero_timestamp = GetZeroTime(now_second);
	unsigned int start_timestamp = zero_timestamp + (food_cfg->start_time / 100) * SECOND_PER_HOUR + (food_cfg->start_time % 100) * SECOND_PER_MIN;
	unsigned int end_timestamp = zero_timestamp + (food_cfg->end_time / 100) * SECOND_PER_HOUR + (food_cfg->end_time % 100) * SECOND_PER_MIN;
	unsigned int make_fetch_end_timestamp = zero_timestamp + (food_cfg->make_time_end / 100) * SECOND_PER_HOUR + (food_cfg->make_time_end % 100) * SECOND_PER_MIN;

	bool is_can_fetch = false;
	bool is_make_fetch = false;
	if (now_second < start_timestamp)		//还未到开放领取时间
	{
		m_role_module_mgr->NoticeNum(errornum::EN_DAY_REWARD_DAY_FOOD_FETCH_NOT_TIME);
		return -3;
	}
	else if (now_second <= end_timestamp)	//处在免费领取时间内
	{
		is_can_fetch = true;
	}
	else if(now_second <= make_fetch_end_timestamp) //处在补领时间内
	{
		if (!food_cfg->is_make)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_DAY_REWARD_DAY_FOOD_NOT_MAKE_FETCH);
			return -4;
		}
		is_can_fetch = true;
		is_make_fetch = true;
	}
	else   //超过了最后的补领时间
	{
		m_role_module_mgr->NoticeNum(errornum::EN_DAY_REWARD_DAY_FOOD_FETCH_TIME_OUT);
		return -5;
	}
	if (!is_can_fetch)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_NOT_ELIGIBLE);
		return -6;
	}
	if (m_role_module_mgr->GetRoleCrystalFb()->GetCrystalFbEnergy() >= LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg().extra_power_limit)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_USE_CRYSTAL_FB_ENERGY_ITEM_UPPER_LIMIT);
		return -7;
	}
	if (is_make_fetch && food_cfg->currency_num > 0)
	{
		switch (food_cfg->currency_type)
		{
			case DAY_REWARD_DAY_FOOD_CURRENCY_TYPE_COIN:
			{
				if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(food_cfg->currency_num))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
					return -8;
				}
				m_role_module_mgr->GetMoney()->UseCoinBind(food_cfg->currency_num, __FUNCTION__);
			}
			break;
			case DAY_REWARD_DAY_FOOD_CURRENCY_TYPE_GOLD:
			{
				if (!m_role_module_mgr->GetMoney()->GoldMoreThan(food_cfg->currency_num))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
					return -9;
				}
				m_role_module_mgr->GetMoney()->UseGold(food_cfg->currency_num, __FUNCTION__);
			}
			break;
			case DAY_REWARD_DAY_FOOD_CURRENCY_TYPE_SILVER_COIN:
			{
				if (!m_role_module_mgr->GetMoney()->OtherCoinMoreThan(food_cfg->currency_num, MONEY_TYPE_SILVER_COIN))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_SILVER_COIN_NOT_ENOUGH);
					return -10;
				}
				m_role_module_mgr->GetMoney()->UseOtherCoin(food_cfg->currency_num, MONEY_TYPE_SILVER_COIN, __FUNCTION__);
			}
			break;
			default:
			return -11;
		}
	}

	m_role_module_mgr->GetRoleCrystalFb()->AddCrystalFbEnergy(food_cfg->food_num, __FUNCTION__, ENTER_ENERGY_CHANGE_TYPE_OTHER);
	common_data.day_food_flag.SetBit(seq);
	this->SendDayFoodInfo();

	m_role_module_mgr->GetRole()->LogActive(LOG_ACTIVE_TYPE_LUNCH_DINNER);
	return 0;
}

int DayReward::OnFetchOnlineGift(int seq)
{
	if (0 == m_param.is_can_join_online_gift_flag)
	{
		return -1;
	}
	const DayRewardOnlineGiftCfg * gift_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetOnlineGiftCfg(seq);
	if (NULL == gift_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return -2;
	}

	if (m_param.online_gift_flag.IsBitSet(seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return -3;
	}

	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	unsigned int online_second = now_second - m_param.online_gift_record_begin_timestamp + m_param.online_gift_times;
	if (online_second <= 0)
	{
		return -4;
	}

	if (online_second < gift_cfg->need_time)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_NOT_ELIGIBLE);
		return -5;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList2(gift_cfg->reward_list, false))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_NO_SPACE);
		return -6;
	}
	if (!m_role_module_mgr->GetKnapsack()->PutVec(gift_cfg->reward_list, PUT_REASON_DAY_REWARD_ONLINE_GIFT))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_NO_SPACE);
		return -7;
	}

	m_param.online_gift_flag.SetBit(seq);
	this->SendOnlineGiftInfo();
	return 0;
}

void DayReward::OnFindRewardCancelNotify()
{	
	m_role_module_mgr->GetCommonData().today_open_find_fun = 1;
	this->SendFindNotifysInfo();
}

void DayReward::OnUnlockPetStrengthenReward(int quality, int str_level)
{
	do
	{
		if (str_level <= 0) break;

		const DayRewardOtherCfg& reward_find_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetOtherCfg();

		if (str_level > reward_find_cfg.max_level)
		{
			str_level = reward_find_cfg.max_level;
		}

		std::set<int>::iterator it = reward_find_cfg.pet_strengthen.find(quality);
		if (it == reward_find_cfg.pet_strengthen.end())
			break;

		// 首次获取到目标品质神兽的时候, 记录并发一条协议
		if(m_role_module_mgr->GetCommonData().pet_strengthen_reawrd_flag == 0)
		{
			m_role_module_mgr->GetCommonData().pet_strengthen_reawrd_flag = 1;
			this->SendPetStrengthenInfo();
		}

		const PetStrengthenWelfareCfg * pet_str_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetPetStrengthenWelfareCfg(str_level);
		if (NULL == pet_str_cfg) break;

		if (m_role_module_mgr->GetCommonData().pet_strengthen_reawrd_flag < str_level)
		{
			m_role_module_mgr->GetCommonData().pet_strengthen_reawrd_flag = str_level;
			this->SendPetStrengthenInfo();
		}

		break;
		
	} while (0);

	return;
}



void DayReward::OnFetchPetStrengthenReward(int seq)
{
	if (seq <= 0)  return;

	const PetStrengthenWelfareCfg * psw_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetPetStrengthenWelfareCfg(seq, true);
	if (NULL == psw_cfg) return;

	if (m_role_module_mgr->GetCommonData().pet_strengthen_reawrd_flag < psw_cfg->pet_strengthen_lv)	return;
	if (m_role_module_mgr->GetCommonData().has_pet_strengthen_flag.IsBitSet(seq))  return;

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(psw_cfg->reward_list))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_role_module_mgr->GetKnapsack()->PutVec(psw_cfg->reward_list, PUT_REASON_PET_STRENGTHEN_REWARD);
	m_role_module_mgr->GetCommonData().has_pet_strengthen_flag.SetBit(seq);

	std::string gamelog_str;
	for (int i = 0; i < (int)psw_cfg->reward_list.size(); ++i)
	{
		gamelog_str = STRING_SPRINTF("{item[%d, %d]} ", psw_cfg->reward_list[i].item_id, psw_cfg->reward_list[i].num);
	}
	gamelog::g_log_other.printf(LL_INFO, "%s role[%d, %s] get_reward[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), gamelog_str.c_str());

	this->SendPetStrengthenInfo();
}

int DayReward::OnOverFolowEnergyFindReward()
{
	if (m_role_module_mgr->GetCommonData().today_find_reward_flag.IsBitSet(DAY_ACTIVITY_TYPE_OVERFLOW_ENERGY))
	{
		return 0;
	}

	const DayRewardOtherCfg& reward_find_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetOtherCfg();

	int overflow_energy = m_role_module_mgr->GetCommonData().overflow_find_count;

	int need_consume = reward_find_cfg.copper_coin_price;
	if (!m_role_module_mgr->GetMoney()->AllCoinMoreThan(need_consume))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return -1;
	}

	int exp = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetFindExpCfg(m_role_module_mgr->GetRole()->GetLevel());
	if (exp == 0) return -2;

	m_role_module_mgr->GetRole()->AddExp(exp * overflow_energy, ADD_EXP_TYPE_FIND_REWARD, __FUNCTION__);

	m_role_module_mgr->GetCommonData().today_activity_type_find_count[DAY_ACTIVITY_TYPE_OVERFLOW_ENERGY] += overflow_energy;
	m_role_module_mgr->GetCommonData().today_find_reward_flag.SetBit(DAY_ACTIVITY_TYPE_OVERFLOW_ENERGY);

	m_role_module_mgr->GetCommonData().overflow_find_count = 0;

	gamelog::g_log_role_exp.printf(LL_INFO, "%s role[%d, %s] find_tili[%d]",__FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), overflow_energy);

	return need_consume;
}


void DayReward::SendAllInfo()
{
	SendOnlineRewardInfo();
	SendSignRewardInfo();
	SendLevelRewardInfo();
	SendFindRewardInfo();
	SendFindNotifysInfo();
	SendDayFoodInfo();
	SendOnlineGiftInfo();
}

void DayReward::SendFindRewardAndNotifysInfo()
{
	this->SendFindRewardInfo();
	this->SendFindNotifysInfo();
}

void DayReward::SendOnlineRewardInfo()
{
	Protocol::SCOnlineRewardInfo info;
	info.today_online_time = m_role_module_mgr->GetRole()->GetTodayOnlineTime();
	info.reward_fetch_flag = m_role_module_mgr->GetCommonData().online_reward_fetch_flag;
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void DayReward::SendSignRewardInfo()
{
	Protocol::SCSignRewardInfo info;
	info.sign_count = m_role_module_mgr->GetCommonData().sign_count + 1;
	info.reward_fetch_flag = m_role_module_mgr->GetCommonData().sign_reward_fetch_flag;
	info.month_zero_open_day = month_zero_open_day;
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void DayReward::SendLevelRewardInfo()
{
	Protocol::SCLevelRewardInfo info;
	info.reward_fetch_flag = m_role_module_mgr->GetCommonData().level_reward_fetch_flag;
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void DayReward::SendFindRewardInfo()
{
	Protocol::SCFindRewardInfo info;

	UNSTD_STATIC_CHECK(20 == DAY_ACTIVITY_TYPE_MAX)
	UNSTD_STATIC_CHECK(LAYER_MAX_NUM <= 32)
	for (int i = 0; i < DAY_ACTIVITY_TYPE_MAX; ++i)
	{
		int count = 0;
		switch (i)
		{
		case DAY_ACTIVITY_TYPE_ZHEN_ZHONG:
			count = m_role_module_mgr->GetResearchTask()->GetYesterdayUnfinishTimes();
			break;
		case DAY_ACTIVITY_TYPE_TREASURE_MAP_TASK:
			count = m_role_module_mgr->GetTreasureMap()->GetYesterdayUndoneTimes();
			break;
		case DAY_ACTIVITY_TYPE_SCHOOL_TASK:
			count = m_role_module_mgr->GetSchoolTask()->GetFindRewardCount();
			break;
		case DAY_ACTIVITY_TYPE_WST_QUESTION:
			count = m_role_module_mgr->GetCommonData().wst_answer_yesterday_num;
			break;
		case DAY_ACTIVITY_TYPE_TREVI_FOUNTAIN:
			count = m_role_module_mgr->GetCommonData().trevi_fountain_can_find_count;
			break;
		case DAY_ACTIVITY_TYPE_BOUNTY_TASK:
			count = m_role_module_mgr->GetCommonData().bounty_task_find_num;
			break;
		case DAY_ACTIVITY_TYPE_FANTASY_FB:
			memcpy(&count, m_role_module_mgr->GetCommonData().find_fantasy_flag.byte_list, sizeof(count));
			break;
		case DAY_ACTIVITY_TYPE_TRANSACTION_TASK:
			count = m_role_module_mgr->GetTransactionTask()->GetFindRewardCount();
			break;
		case DAY_ACTIVITY_TYPE_GUILD_ANSWER:
			count = m_role_module_mgr->GetCommonData().guild_answer_find_count;
			break;
		case DAY_ACTIVITY_TYPE_GUILD_FIGHT:	
			count = m_role_module_mgr->GetCommonData().guild_fight_find_count;
			break;
		case DAY_ACTIVITY_TYPE_ONLY_FIGHT:
			count = m_role_module_mgr->GetCommonData().only_fight_find_count;
			break;
		case DAY_ACTIVITY_TYPE_TEAM_FIGHT:
			count = m_role_module_mgr->GetCommonData().team_fight_find_count;
			break;
		case DAY_ACTIVITY_TYPE_OVERFLOW_ENERGY:
			{
				m_role_module_mgr->GetRoleCrystalFb()->OverFlowEnergyCal();
				count = m_role_module_mgr->GetCommonData().overflow_find_count;
			}
			break;
		case DAY_ACTIVITY_TYPE_WAN_LING_FB_ACTIVE:
		case DAY_ACTIVITY_TYPE_CRYSTAL_XI_YOU_ACTIVE:
		case DAY_ACTIVITY_TYPE_CLOUD_ARENA_ACTIVE:
		case DAY_ACTIVITY_TYPE_ZHEN_ZHONG_ACTIVE:
		case DAY_ACTIVITY_TYPE_BOUNTY_TASK_ACTIVE:
			{
				count = m_role_module_mgr->GetActiveSystem()->GetCanFindCountByType(i);
			}
			break;
		case DAY_ACTIVITY_TYPE_SILK_ROADS_ACTIVE:
			count = m_role_module_mgr->GetCommonData().silk_roads_find_count;
			break;
		}
		info.can_find_count[i] = count;
	}
	info.find_reward_flag = m_role_module_mgr->GetCommonData().today_find_reward_flag;
	for (int i = 0; i < DAY_ACTIVITY_TYPE_MAX && i < ARRAY_LENGTH(m_role_module_mgr->GetCommonData().today_activity_type_find_count); i++)
	{
		info.today_find_count[i] = m_role_module_mgr->GetCommonData().today_activity_type_find_count[i];
	}

	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void DayReward::SendFindNotifysInfo()
{
	Protocol::SCRewardFindNotifys info;
	info.notifys = m_role_module_mgr->GetCommonData().today_open_find_fun;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void DayReward::SendDayFoodInfo()
{	
	Protocol::SCDayRewardDayFoodInfo info;
	info.day_food_flag = m_role_module_mgr->GetCommonData().day_food_flag;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void DayReward::SendPetStrengthenInfo()
{
	Protocol::SCPetStrengthenRewardInfo info;
	info.pet_strengthen_lv = m_role_module_mgr->GetCommonData().pet_strengthen_reawrd_flag;
	info.has_pet_strengthen_flag = m_role_module_mgr->GetCommonData().has_pet_strengthen_flag;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void DayReward::SendOnlineGiftInfo()
{
	Protocol::SCDayRewardOnlineGiftInfo info;
	info.online_gift_record_begin_timestamp = m_param.online_gift_record_begin_timestamp;
	info.online_gift_times = m_param.online_gift_times;
	info.online_gift_flag = m_param.online_gift_flag;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void DayReward::OnGuildAnswerFindDataReset(bool is_in_time, unsigned int can_find_end_timestamp)
{
	const DayRewardFindCfg *cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(DAY_ACTIVITY_TYPE_GUILD_ANSWER);
	if (NULL == cfg || m_role_module_mgr->GetRole()->GetLevel() < cfg->find_level) return;

	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	const GuildAnswerOthers & other_cfg = LOGIC_CONFIG->GetActivityGuildAnswerConfig()->GetGuildAnswerOtherConfig();
	if (is_in_time)
	{
		if (common_data.fetch_special_logic_activity_reward_zero_timestamp[DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_GUILD_ANSWER] + SECOND_PER_DAY == can_find_end_timestamp)
		{
			if (other_cfg.question_numb >= common_data.guild_answer_reply_count)
			{
				common_data.guild_answer_find_count = other_cfg.question_numb - common_data.guild_answer_reply_count;
			}
			else
			{
				common_data.guild_answer_find_count = 0;
			}
		}
		else
		{
			common_data.guild_answer_find_count = other_cfg.question_numb;
		}
	}
	else
	{
		common_data.guild_answer_find_count = 0;
	}
	common_data.guild_answer_reply_count = 0;

}

bool DayReward::OnGuildAnswerFindReward(int consume_type, int count)
{
	if (count > m_role_module_mgr->GetCommonData().guild_answer_find_count)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FIND_COUNT_NOT_ENOUGH);
		return false;
	}
	const GuildAnswerOthers & other_cfg = LOGIC_CONFIG->GetActivityGuildAnswerConfig()->GetGuildAnswerOtherConfig();
	const GuildAnswerFindCfg * find_cfg = LOGIC_CONFIG->GetActivityGuildAnswerConfig()->GetFindCfg(consume_type, m_role_module_mgr->GetRole()->GetLevel());
	if(NULL == find_cfg) return false;
	
	if (-1 != find_cfg->group_id)
	{
		std::vector<ItemConfigData> item_list;
		for (int i = 0; i < count && i < other_cfg.question_numb; i++)
		{
			const GuildAnswerFindRewardGroupCfg * reward_group_cfg = LOGIC_CONFIG->GetActivityGuildAnswerConfig()->GetFindRewardGroupCfg(find_cfg->group_id);
			if (NULL != reward_group_cfg)
			{
				item_list.push_back(reward_group_cfg->reward);
			}
		}
		if (!item_list.empty())
		{
			if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)item_list.size(), &item_list[0]))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return false;
			}
			if (!m_role_module_mgr->GetKnapsack()->PutList((short)item_list.size(), &item_list[0], PUT_REASON_FIND_REWARD))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return false;
			}
		}
	}

	if (find_cfg->exp > 0)
	{
		m_role_module_mgr->GetRole()->AddExp(find_cfg->exp * count, ADD_EXP_TYPE_FIND_REWARD);
	}
	m_role_module_mgr->GetMoney()->AddCoinBind(find_cfg->coin * count, __FUNCTION__);
	if (NULL != m_role_module_mgr->GetRole()->GetGuild())
	{
		m_role_module_mgr->GetMoney()->AddOtherCoin(find_cfg->family_gongxian * count, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__);
	}
	m_role_module_mgr->GetCommonData().guild_answer_find_count -= count;
	m_role_module_mgr->GetCommonData().today_activity_type_find_count[DAY_ACTIVITY_TYPE_GUILD_ANSWER] += count;
	m_role_module_mgr->GetCommonData().today_find_reward_flag.SetBit(DAY_ACTIVITY_TYPE_GUILD_ANSWER);
	return true;
}

void DayReward::OnGuildFightFindDataReset(bool is_in_time, unsigned int can_find_end_timestamp)
{
	const DayRewardFindCfg *cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(DAY_ACTIVITY_TYPE_GUILD_FIGHT);
	if (NULL == cfg || m_role_module_mgr->GetRole()->GetLevel() < cfg->find_level) return;
	
	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	if (is_in_time)
	{
		if (common_data.fetch_special_logic_activity_reward_zero_timestamp[DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_GUILD_FIGHT] + SECOND_PER_DAY == can_find_end_timestamp)
		{
			common_data.guild_fight_find_count = (0 != common_data.fetch_guild_fight_join_reward_flag) ? 0 : 1;
		}
		else
		{
			common_data.guild_fight_find_count = 1;
		}
	}
	else
	{
		common_data.guild_fight_find_count = 0;
	}
	common_data.fetch_guild_fight_join_reward_flag = 0;
}

bool DayReward::OnGuildFightFindReward(int consume_type, int count)
{
	if (count > m_role_module_mgr->GetCommonData().guild_fight_find_count)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FIND_COUNT_NOT_ENOUGH);
		return false;
	}

	const GuildFightFindCfg * find_cfg = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetFindCfg(consume_type, m_role_module_mgr->GetRole()->GetLevel());
	if(NULL == find_cfg) return false;

	if (-1 != find_cfg->group_id)
	{
		std::vector<ItemConfigData> item_list;
		for (int i = 0; i < count; i++)
		{
			const GuildFightFindRewardGroupCfg * reward_group_cfg = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetFindRewardGroupCfg(find_cfg->group_id);
			if (NULL != reward_group_cfg)
			{
				item_list.push_back(reward_group_cfg->reward);
			}
		}
		if (!item_list.empty())
		{
			if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)item_list.size(), &item_list[0]))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return false;
			}
			if (!m_role_module_mgr->GetKnapsack()->PutList((short)item_list.size(), &item_list[0], PUT_REASON_FIND_REWARD))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return false;
			}
		}
	}
	if (find_cfg->exp > 0)
	{
		m_role_module_mgr->GetRole()->AddExp(find_cfg->exp * count, ADD_EXP_TYPE_FIND_REWARD);
	}
	m_role_module_mgr->GetMoney()->AddCoinBind(find_cfg->coin * count, __FUNCTION__);
	if (NULL != m_role_module_mgr->GetRole()->GetGuild())
	{
		m_role_module_mgr->GetMoney()->AddOtherCoin(find_cfg->family_gongxian * count, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__);
	}
	m_role_module_mgr->GetCommonData().guild_fight_find_count -= count;
	m_role_module_mgr->GetCommonData().today_activity_type_find_count[DAY_ACTIVITY_TYPE_GUILD_FIGHT] += count;
	m_role_module_mgr->GetCommonData().today_find_reward_flag.SetBit(DAY_ACTIVITY_TYPE_GUILD_FIGHT);
	return true;
}

void DayReward::OnOnlyFightFindDataReset(bool is_in_time, unsigned int can_find_end_timestamp)
{
	const DayRewardFindCfg *cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(DAY_ACTIVITY_TYPE_ONLY_FIGHT);
	if (NULL == cfg || m_role_module_mgr->GetRole()->GetLevel() < cfg->find_level) return;

	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	if (is_in_time)
	{
		if (common_data.fetch_special_logic_activity_reward_zero_timestamp[DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_ONLY_FIGHT] + SECOND_PER_DAY == can_find_end_timestamp)
		{
			common_data.only_fight_find_count = (0 != common_data.fetch_only_fight_join_reward_flag) ? 0 : 1;
		}
		else
		{
			common_data.only_fight_find_count = 1;
		}
	}
	else
	{
		common_data.only_fight_find_count = 0;
	}
	common_data.fetch_only_fight_join_reward_flag = 0;
}

bool DayReward::OnOnlyFightFindReward(int consume_type, int count)
{
	if (count > m_role_module_mgr->GetCommonData().only_fight_find_count)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FIND_COUNT_NOT_ENOUGH);
		return false;
	}

	const OnlyFightFindCfg * find_cfg = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetFindCfg(consume_type, m_role_module_mgr->GetRole()->GetLevel());
	if (NULL == find_cfg) return false;

	if (-1 != find_cfg->group_id)
	{
		std::vector<ItemConfigData> item_list;
		for (int i = 0; i < count; i++)
		{
			const OnlyFightFindRewardGroupCfg * reward_group_cfg = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetFindRewardGroupCfg(find_cfg->group_id);
			if (NULL != reward_group_cfg)
			{
				item_list.push_back(reward_group_cfg->reward);
			}
		}
		if (!item_list.empty())
		{
			if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)item_list.size(), &item_list[0]))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return false;
			}
			if (!m_role_module_mgr->GetKnapsack()->PutList((short)item_list.size(), &item_list[0], PUT_REASON_FIND_REWARD))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return false;
			}
		}
	}
	if (find_cfg->exp > 0)
	{
		m_role_module_mgr->GetRole()->AddExp(find_cfg->exp * count, ADD_EXP_TYPE_FIND_REWARD);
	}
	m_role_module_mgr->GetMoney()->AddCoinBind(find_cfg->coin * count, __FUNCTION__);
	if (NULL != m_role_module_mgr->GetRole()->GetGuild())
	{
		m_role_module_mgr->GetMoney()->AddOtherCoin(find_cfg->family_gongxian * count, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__);
	}
	m_role_module_mgr->GetCommonData().only_fight_find_count -= count;
	m_role_module_mgr->GetCommonData().today_activity_type_find_count[DAY_ACTIVITY_TYPE_ONLY_FIGHT] += count;
	m_role_module_mgr->GetCommonData().today_find_reward_flag.SetBit(DAY_ACTIVITY_TYPE_ONLY_FIGHT);
	return true;
}

void DayReward::OnTeamFightFindDataReset(bool is_in_time, unsigned int can_find_end_timestamp)
{
	const DayRewardFindCfg *cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(DAY_ACTIVITY_TYPE_TEAM_FIGHT);
	if (NULL == cfg || m_role_module_mgr->GetRole()->GetLevel() < cfg->find_level) return;

	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	if (is_in_time)
	{
		if (common_data.fetch_special_logic_activity_reward_zero_timestamp[DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_TEAM_FIGHT] + SECOND_PER_DAY == can_find_end_timestamp)
		{
			common_data.team_fight_find_count = (0 != common_data.fetch_team_fight_join_reward_flag) ? 0 : 1;
		}
		else
		{
			common_data.team_fight_find_count = 1;
		}
	}
	else
	{
		common_data.team_fight_find_count = 0;
	}
	common_data.fetch_team_fight_join_reward_flag = 0;
}

bool DayReward::OnTeamFightFindReward(int consume_type, int count)
{
	if (count > m_role_module_mgr->GetCommonData().team_fight_find_count)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FIND_COUNT_NOT_ENOUGH);
		return false;
	}

	const TeamFightFindCfg * find_cfg = LOGIC_CONFIG->GetActivityTeamFightConfig()->GetFindCfg(consume_type, m_role_module_mgr->GetRole()->GetLevel());
	if (NULL == find_cfg) return false;

	if (-1 != find_cfg->group_id)
	{
		std::vector<ItemConfigData> item_list;
		for (int i = 0; i < count; i++)
		{
			const TeamFightFindRewardGroupCfg * reward_group_cfg = LOGIC_CONFIG->GetActivityTeamFightConfig()->GetFindRewardGroupCfg(find_cfg->group_id);
			if (NULL != reward_group_cfg)
			{
				item_list.push_back(reward_group_cfg->reward);
			}
		}
		if (!item_list.empty())
		{
			if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)item_list.size(), &item_list[0]))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return false;
			}
			if (!m_role_module_mgr->GetKnapsack()->PutList((short)item_list.size(), &item_list[0], PUT_REASON_FIND_REWARD))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return false;
			}
		}
	}
	if (find_cfg->exp > 0)
	{
		m_role_module_mgr->GetRole()->AddExp(find_cfg->exp * count, ADD_EXP_TYPE_FIND_REWARD);
	}
	m_role_module_mgr->GetMoney()->AddCoinBind(find_cfg->coin * count, __FUNCTION__);
	if (NULL != m_role_module_mgr->GetRole()->GetGuild())
	{
		m_role_module_mgr->GetMoney()->AddOtherCoin(find_cfg->family_gongxian * count, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__);
	}
	m_role_module_mgr->GetCommonData().team_fight_find_count -= count;
	m_role_module_mgr->GetCommonData().today_activity_type_find_count[DAY_ACTIVITY_TYPE_TEAM_FIGHT] += count;
	m_role_module_mgr->GetCommonData().today_find_reward_flag.SetBit(DAY_ACTIVITY_TYPE_TEAM_FIGHT);
	return true;
}

void DayReward::OnSilkRoadsFindDataReset(unsigned int can_find_end_timestamp)
{
	const DayRewardFindCfg *cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(DAY_ACTIVITY_TYPE_SILK_ROADS_ACTIVE);
	if (NULL == cfg || m_role_module_mgr->GetRole()->GetLevel() < cfg->find_level) return;

	unsigned int next_day_timestamp = (unsigned int)EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextDayInterval(0, 0, 0);
	if (next_day_timestamp != (EngineAdapter::Instance().WeekID() + (SECOND_PER_DAY * 5)) &&
		next_day_timestamp != (EngineAdapter::Instance().WeekID() + (SECOND_PER_DAY * 6)))
	{
		return;
	}	

	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	if (LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetOtherCfg().act_num >= common_data.silk_roads_count)
	{
		common_data.silk_roads_find_count = LOGIC_CONFIG->GetActivitySilkRoadsConfig()->GetOtherCfg().act_num - common_data.silk_roads_count;
	}
	else
	{
		common_data.silk_roads_find_count = 0;
	}
	common_data.silk_roads_count = 0;
}

bool DayReward::OnSilkRoadsFindReward(int consume_type, int count)
{
	ActivitySilkRoads * activity = (ActivitySilkRoads *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_SILK_ROADS);
	if (NULL != activity)
	{
		if (!activity->FindReward(m_role_module_mgr->GetRole(), consume_type, count))
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

void DayReward::OnCheckOnlineGiftOpen()
{
	//1.已开启 2.没有资格参加
	if (m_param.online_gift_join_begin_timestamp > 0 || 0 == m_param.is_can_join_online_gift_flag)
	{
		return;
	}

	//3.等级不足,不予开启
	if (m_role_module_mgr->GetRole()->GetLevel() < LOGIC_CONFIG->GetDayRewardConfig()->GetOtherCfg().online_gift_open_level)
	{
		return;
	}

	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	m_param.online_gift_join_begin_timestamp = now_second;
	m_param.online_gift_record_begin_timestamp = now_second;
	m_param.online_gift_times = 0;

	this->SendOnlineGiftInfo();
}
