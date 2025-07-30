#include "activesystem.hpp"
#include "activesystemconfig.hpp"

#include "servercommon/errornum.h"
#include "config/logicconfigmanager.hpp"
#include "obj/character/role.h"
#include "item/knapsack.h"
#include "other/rolemodulemanager.hpp"
#include "other/funopen/funopen.hpp"
#include "other/rolegatherfb/rolegatherfb.hpp"
#include "other/schooltask/schooltask.h"
#include "other/researchtask/researchtask.h"
#include "other/treasure_map/treasuremap.h"
#include "other/monster_wave/monsterwave.h"
#include "other/dayreward/dayreward.hpp"
#include "other/dayreward/dayrewardconfig.hpp"
#include "other/bountytask/bountytask.hpp"
#include "other/transactiontask/transactiontask.hpp"
#include "servercommon/logdef.h"
#include "gamelog.h"
#include "global/randactivity/randactivityadapter.hpp"
#include "other/event/eventhandler.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "config/sharedconfig/sharedconfig.h"
#include "other/cycleactivity/concentricknot/concentricknot.hpp"
#include "servercommon/serverconfig/bigcrossconfig/utaconfig.hpp"
#include "global/universe_team_arena/UTAShadow.hpp"

ActiveSystem::ActiveSystem():m_module_mgr(NULL)
{
	m_find_reward_map[DAY_ACTIVITY_TYPE_WAN_LING_FB_ACTIVE] = ACTIVE_SYSTEM_TYPE_WAN_LING_FB;
	m_find_reward_map[DAY_ACTIVITY_TYPE_CRYSTAL_XI_YOU_ACTIVE] = ACTIVE_SYSTEM_TYPE_CRYSTAL_XIYOU;
	m_find_reward_map[DAY_ACTIVITY_TYPE_CLOUD_ARENA_ACTIVE] = ACTIVE_SYSTEM_TYPE_CLOUD_ARENA;
	m_find_reward_map[DAY_ACTIVITY_TYPE_ZHEN_ZHONG_ACTIVE] = ACTIVE_SYSTEM_TYPE_ZHEN_ZHONG;
	m_find_reward_map[DAY_ACTIVITY_TYPE_BOUNTY_TASK_ACTIVE] = ACTIVE_SYSTEM_TYPE_BOUNTY_TASK;
}

ActiveSystem::~ActiveSystem()
{
}

void ActiveSystem::Init(RoleModuleManager * module_mgr, const ActiveSystemParam & param,
	const ActiveDegreeBaseParam & week_base_param,
	const ActiveDegreeTaskParam & week_task_param,
	const ActiveTurntableParam & turntable_param)
{
	m_module_mgr = module_mgr;
	m_param = param;
	m_week_base_param = week_base_param;
	m_week_task_param = week_task_param;
	m_turntable_param = turntable_param;

	if (0 == m_param.level)
	{
		m_param.level = module_mgr->GetRole()->GetLevel();
	}
}

void ActiveSystem::GetInitParam(ActiveSystemParam * param,
	ActiveDegreeBaseParam * week_base_param,
	ActiveDegreeTaskParam * week_task_param,
	ActiveTurntableParam * turntable_param)
{
	*param = m_param;
	*week_base_param = m_week_base_param;
	*week_task_param = m_week_task_param;
	*turntable_param = m_turntable_param;
}

void ActiveSystem::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid || !FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_ACTIVE_SYSTEM)) return;

	this->OnFindDataReset();
	m_param.DayReset();
	m_param.level = m_module_mgr->GetRole()->GetLevel();
	this->SendActiveInfo();

	//周常
	this->WeekDegreeDailyReset();
	this->SendWeekInfo();

	//活跃转盘 每日奖励
	m_turntable_param.turntable_day_task_flag.Reset();
	this->SendTurntableInfo();
}

void ActiveSystem::OnFindDataReset()
{
	for(std::map<int, int>::const_iterator it = m_find_reward_map.begin(); it != m_find_reward_map.end(); ++it)
	{
		const DayRewardFindCfg *cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(it->first);
		if (NULL == cfg || m_module_mgr->GetRole()->GetLevel() < cfg->find_level || it->second < 0 || it->second >= ARRAY_ITEM_COUNT(m_param.can_find_count_list)) continue;
	
		int need_level_open_fun = 0;
		switch (it->second)
		{
			case ACTIVE_SYSTEM_TYPE_WAN_LING_FB:
			{
				need_level_open_fun = FunOpen::GetFunLimitLevel(FUN_TYPE_WAN_LING_FB);
			}
			break;
			case DAY_ACTIVITY_TYPE_CRYSTAL_XI_YOU_ACTIVE:
			{
				need_level_open_fun = FunOpen::GetFunLimitLevel(FUN_TYPE_CRYSTAL_FB);
			}
			break;
			case DAY_ACTIVITY_TYPE_CLOUD_ARENA_ACTIVE:
			{
				need_level_open_fun = FunOpen::GetFunLimitLevel(FUN_TYPE_CLOUD_ARENA);
			}
			break;
			case DAY_ACTIVITY_TYPE_ZHEN_ZHONG_ACTIVE:
			{
				need_level_open_fun = FunOpen::GetFunLimitLevel(FUN_TYPE_ZHENZHONG_RESERCH);
			}
			break;
			case DAY_ACTIVITY_TYPE_BOUNTY_TASK_ACTIVE:
			{
				need_level_open_fun = FunOpen::GetFunLimitLevel(FUN_TYPE_BOUNTY_TASK);
			}
			break;
			default:
			break;
		}
		if (m_module_mgr->GetRole()->GetLevel() < need_level_open_fun)
		{
			continue;
		}

		short & can_find_count = m_param.can_find_count_list[it->second];
		CommonDataParam& m_commondata = m_module_mgr->GetCommonData();
		if (m_commondata.offine_day < 2)
		{
			if (!m_param.type_reward_fetch_flag.IsBitSet(it->second))
			{
				can_find_count = 1;
			}
			else
			{
				can_find_count = 0;
			}
		}
		else
		{
			can_find_count = 1;
		}
	}
}

void ActiveSystem::OnWeekChange()
{
	this->WeekDegreeWeekReset();

	//活跃转盘 周奖励
	m_turntable_param.turntable_week_task_flag.Reset();

	//先OnWeekChange 再OnDayChange 可以不用下发通知
	m_param.special_active_reward_flag.UnSetBit(ACTIVITY_SYSTEM_SPECIAL_TYPE_WILD_BOSS);
	this->OnSendSpecialActiveInfo();
}

void ActiveSystem::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{

}

void ActiveSystem::ActiveSystemOp(Protocol::CSActiveSystemReq * req)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_ACTIVE_SYSTEM)) return;

	switch (req->op_type)
	{
	case Protocol::CSActiveSystemReq::ACTIVE_SYSTEM_INFO_TYPE:
		m_module_mgr->GetActiveSystem()->SendActiveInfo();
		break;
	case Protocol::CSActiveSystemReq::ACTIVE_SYSTEM_FETCH_REWARD:
		m_module_mgr->GetActiveSystem()->OnFetchActiveReward(req->index);
		break;
	case Protocol::CSActiveSystemReq::ACTIVE_SYSTEM_WEEK_FETCH_REWARD:
		{
			this->FetchWeekDegreeReward(req->index);
		}
		break;
	case Protocol::CSActiveSystemReq::ACTIVE_SYSTEM_FETCH_SPECIAL_REWARD:
		{
			this->OnFetchSpecialActiveReward(req->index);
		}
		break;
	case Protocol::CSActiveSystemReq::ACTIVE_SYSTEM_SPECIAL_INFO:
		{
			this->OnSendSpecialActiveInfo();
		}
		break;
	default:
		return;
	}
}

bool ActiveSystem::OnFindReward(int find_type, int count)
{
	std::map<int, int>::const_iterator it = m_find_reward_map.find(find_type);
	if (it == m_find_reward_map.end() || it->second < 0 || it->second >= ARRAY_ITEM_COUNT(m_param.can_find_count_list))
	{
		return false;
	}

	if (count > m_param.can_find_count_list[it->second])
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_FIND_COUNT_NOT_ENOUGH);
		return false;
	}

	const ActiveSystemCfg* cfg = LOGIC_CONFIG->GetActiveSystemConfig()->GetActiveSystemCfg(it->second, m_module_mgr->GetRole()->GetLevel());
	if (NULL == cfg)
	{
		return false;
	}
	
	//目前只给经验奖励
	/*if (cfg->reward_cfg.reward_num > 0 && !m_module_mgr->GetKnapsack()->CheckForPutList(cfg->reward_cfg.reward_num, cfg->reward_cfg.reward_list))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (cfg->reward_cfg.reward_num > 0 && !m_module_mgr->GetKnapsack()->PutList(cfg->reward_cfg.reward_num, cfg->reward_cfg.reward_list, ADD_EXP_TYPE_FIND_REWARD))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}*/

	//m_module_mgr->GetMoney()->AddCoinBind(cfg->reward_cfg.reward_coin, __FUNCTION__);
	//m_module_mgr->GetRoleCrystalFb()->AddCrystalFbEnergy(cfg->reward_cfg.reward_energy, __FUNCTION__, ENTER_ENERGY_CHANGE_TYPE_OTHER);
	m_module_mgr->GetRole()->AddExp(cfg->reward_cfg.reward_exp, ADD_EXP_TYPE_ACTIVE_SYSTEM, __FUNCTION__);

	m_param.can_find_count_list[it->second] -= count;
	m_module_mgr->GetCommonData().today_activity_type_find_count[find_type] += count;
	m_module_mgr->GetCommonData().today_find_reward_flag.SetBit(find_type);

	return true;
}

int ActiveSystem::GetCanFindCountByType(int find_type) const
{
	std::map<int, int>::const_iterator it = m_find_reward_map.find(find_type);
	if (it == m_find_reward_map.end() || it->second < 0 || it->second >= ARRAY_ITEM_COUNT(m_param.can_find_count_list))
	{
		return 0;
	}

	return m_param.can_find_count_list[it->second];
}

void ActiveSystem::RoleLevelUp(int cur_level)
{
	if(!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_ACTIVE_SYSTEM)) return;
	if(m_param.level != 0) return;
	
	m_param.DayReset();
	m_param.level = cur_level;
	this->SendActiveInfo();
}

void ActiveSystem::AddActiveDegree(int type, int count, const char* reason_str /* = "NULL" */)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_ACTIVE_SYSTEM) || count < 0) return;

	UNSTD_STATIC_CHECK(26 == ACTIVE_SYSTEM_TYPE_MAX);
	bool is_add = false;
	switch (type)
	{
		case ACTIVE_SYSTEM_TYPE_SCHOOL_TASK: 
		{
			count = m_module_mgr->GetSchoolTask()->GetFinishCount();
		}
		break;
		case ACTIVE_SYSTEM_TYPE_ZHEN_ZHONG:			// 2  真中的研究(三界除魔)
		{
			count = m_module_mgr->GetResearchTask()->GetTodayFinishCount();
		}
		break;
		case ACTIVE_SYSTEM_TYPE_ARRESTING_THIEVES:	// 3  追缉盗贼
		{
			count = m_module_mgr->GetTreasureMap()->GetTodayFinish();
		}
		break;
		case ACTIVE_SYSTEM_TYPE_WST_QUESTION:		// 4  万事通的考验 / 神算子的考验
		{
			count = m_module_mgr->GetCommonData().wst_answer_num;
		}
		break;
		case ACTIVE_SYSTEM_TYPE_TREVI_FOUNTAIN:		// 5  法兰城许愿池 / 许愿池
		{
			count = m_module_mgr->GetCommonData().trevi_fountain_today_use_count;
		}
		break;
		case ACTIVE_SYSTEM_TYPE_DEMON_CAVE:			// 6  魔窟（万妖谷)
		{
			count = m_module_mgr->GetMonsterWave()->GetPlayTimes();
		}
		break;
		case ACTIVE_SYSTEM_TYPE_ESCORT:				// 7  运镖
		{
			count = m_module_mgr->GetCommonData().escort_today_count;
		}
		break;
		case ACTIVE_SYSTEM_TYPE_BOUNTY_TASK:		// 8  赏金任务
		{
			count = m_module_mgr->GetBountyTask()->GetTodayFinish();
		}
		break;
		case ACTIVE_SYSTEM_TYPE_TRANSACTION_TASK:	// 9  购买任务 - 丝绸之路
		{
			count = m_module_mgr->GetTransactionTask()->GetTodayFinish();
		}
		break;
		case ACTIVE_SYSTEM_TYPE_FANTASY_FB:			// 10 四象幻境  
		{
			count = m_module_mgr->GetFantasyTodayFinishNum();
		}
		break;
		case ACTIVE_SYSTEM_TYPE_CRYSTAL_XIYOU:				// 19 幻境试炼
		{
			count = m_module_mgr->GetRoleCrystalFb()->GetXiYouUseTimes();
		}
		break;
		case ACTIVE_SYSTEM_TYPE_JINGJICHANG:				// 11 参与竞技场
		case ACTIVE_SYSTEM_TYPE_COURAGE_CHALLENGE:			// 12 参与锢魔之塔
		case ACTIVE_SYSTEM_TYPE_MIYI:						// 13 远征魔冢(试炼之地)
		case ACTIVE_SYSTEM_TYPE_ENERGY:						// 14 消耗体力
		case ACTIVE_SYSTEM_TYPE_CLOUD_ARENA:				// 15 云端竞技
		case ACTIVE_SYSTEM_TYPE_LUNHUIWANGCHUAN:			// 16 轮回忘川
		case ACTIVE_SYSTEM_TYPE_DREAM_NOTES_CHALLENGE:		// 17 梦渊笔录
		case ACTIVE_SYSTEM_TYPE_SHAN_HAI_BOSS:				// 18 山海降妖
		case ACTIVE_SYSTEM_TYPE_NETHER_WORLD_PURGATORY:		// 20 幽冥炼狱
		case ACTIVE_SYSTEM_TYPE_PRESTIGE_TASK:				// 21 声望任务
		case ACTIVE_SYSTEM_TYPE_BIG_DIPPER:					// 22 北斗七星
		case ACTIVE_SYSTEM_TYPE_WAN_LING_FB:				// 23 万灵话本
		case ACTIVE_SYSTEM_TYPE_TREASURE_TASK:				// 24 九州秘宝
		case ACTIVE_SYSTEM_TYPE_FLY_UP_TASK:				// 25 飞升任务
		{
			is_add = true;
		}
		break;
		default:
		return;
	}

	const ActiveSystemCfg * cfg = LOGIC_CONFIG->GetActiveSystemConfig()->GetActiveSystemCfg(type, m_param.level);
	if (NULL == cfg)
	{
		/*m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);*/
		return;
	}
	
	if (cfg->reward_cfg.open_day > m_module_mgr->GetCreateDays())
	{
		//m_module_mgr->NoticeNum(errornum::EN_FUNOPEN_SERVER_OPEN_ERR);
		return;
	}

	if (m_param.type_flag.IsBitSet(type)) return;
	
	if (is_add)
	{
		m_param.progress[type] += count;
	}
	else
	{
		m_param.progress[type] = count;
	}
	

	if(m_param.progress[type] >= cfg->need_count)
	{ 
		m_param.type_flag.SetBit(type);

		m_param.progress[type] = GetMax(m_param.progress[type], cfg->need_count);
	}

	this->SendSigleActiveInfo(type);

	gamelog::g_log_active_system.printf(LL_INFO, "ActiveSystem::AddActiveDegree user[%d,%s] type[%d] progress[%d]",
		m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), type, m_param.progress[type]);
}

void ActiveSystem::OnFetchActiveReward(int index)
{
	const ActiveSystemRewardCfg * cfg = LOGIC_CONFIG->GetActiveSystemConfig()->GetActiveSystemRewardCfg(m_param.level, index);
	if (NULL == cfg)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (m_param.active_flag.IsBitSet(index))
	{
		m_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}
	if (m_param.integral_active < cfg->integral_active)
	{
		m_module_mgr->NoticeNum(errornum::EN_ACTIVE_SYSTEM_NOT_ENOUGH);
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_module_mgr->GetRole()->GetUniqueServerID());

	std::vector<ItemConfigData> reward_list;
	LOGIC_CONFIG->GetActiveSystemConfig()->GetActiveSystemRelatedCfgNum(ramgr, cfg->seq, cfg->count, cfg->reward, &reward_list);
	if (!reward_list.empty())
	{
		if (!m_module_mgr->GetKnapsack()->CheckForPutList((short)reward_list.size(), &reward_list[0]))
		{
			m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
		if (!m_module_mgr->GetKnapsack()->PutList((short)reward_list.size(), &reward_list[0], PUT_REASON_ACTIVE_SYSTEM_REWARD))
		{
			m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}
	m_module_mgr->GetMoney()->AddCoinBind(cfg->coin, __FUNCTION__);
	m_module_mgr->GetMoney()->AddGold(cfg->gold, __FUNCTION__);
	//m_module_mgr->GetRoleGatherFb()->AddGatherPower(cfg->power);     
	
	m_module_mgr->GetRole()->AddExp(cfg->exp, ADD_EXP_TYPE_ACTIVE_SYSTEM, __FUNCTION__);

	m_param.active_flag.SetBit(index);

	this->SendActiveInfo();

	{
		m_module_mgr->GetConcentricKnot()->OnAddActiveReward(cfg->integral_active);
	}
}

void ActiveSystem::OnFetchActiveExtraReward(int type)
{
	if (type <= ACTIVE_SYSTEM_TYPE_BEGIN || type >= ACTIVE_SYSTEM_TYPE_MAX)
	{
		return;
	}

	if (!m_param.type_flag.IsBitSet(type))
	{
		return; // 尚未完成指定项目的全部活跃度
	}

	if (m_param.type_reward_fetch_flag.IsBitSet(type))
	{
		return; // 已领取奖励
	}

	const ActiveSystemCfg* cfg = LOGIC_CONFIG->GetActiveSystemConfig()->GetActiveSystemCfg(type, m_param.level);
	if (NULL == cfg)
	{
		return;
	}

	if (cfg->reward_cfg.reward_num > 0 && !m_module_mgr->GetKnapsack()->CheckForPutList(cfg->reward_cfg.reward_num, cfg->reward_cfg.reward_list))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}
	
	if (cfg->reward_cfg.reward_num > 0 && !m_module_mgr->GetKnapsack()->PutList(cfg->reward_cfg.reward_num, cfg->reward_cfg.reward_list, PUT_REASON_ACTIVE_SYSTEM_REWARD))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_module_mgr->GetMoney()->AddCoinBind(cfg->reward_cfg.reward_coin, __FUNCTION__);
	m_module_mgr->GetRoleCrystalFb()->AddCrystalFbEnergy(cfg->reward_cfg.reward_energy, __FUNCTION__, ENTER_ENERGY_CHANGE_TYPE_OTHER);
	m_module_mgr->GetRole()->AddExp(cfg->reward_cfg.reward_exp, ADD_EXP_TYPE_ACTIVE_SYSTEM, __FUNCTION__);

	m_param.type_reward_fetch_flag.SetBit(type);

	m_param.integral_active += cfg->integral_active;
	this->SendSigleActiveInfo(type);

	ROLE_LOG_QUICK6(LOG_TYPE_ACTIVE_POINT, m_module_mgr->GetRole(), cfg->integral_active, m_param.integral_active, __FUNCTION__, NULL);
	EventHandler::Instance().OnAddActive(m_module_mgr->GetRole(), cfg->integral_active);

	gamelog::g_log_active_system.printf(LL_INFO, "OnFetchActiveExtraReward role[%d,%s] type[%d]", m_module_mgr->GetRoleUID(), m_module_mgr->GetRoleName(), type);
}

void ActiveSystem::SendActiveInfo()
{
	Protocol::SCActiveSystemInfo info;
	info.level = m_param.level;
	info.integral_active = m_param.integral_active;
	info.active_flag = m_param.active_flag;
	memcpy(info.progress, m_param.progress, sizeof(info.progress));

	info.extra_reward_flag_list[ACTIVE_SYSTEM_TYPE_BEGIN] = 0;
	info.task_count = 1;
	for (int type = ACTIVE_SYSTEM_TYPE_BEGIN + 1; type < ACTIVE_SYSTEM_TYPE_MAX; ++type)
	{
		if (!m_param.type_flag.IsBitSet(type))
		{
			info.extra_reward_flag_list[type] = Protocol::SCActiveSystemInfo::EXTRA_REWARD_FLAG_CANNOT_FETCH;
		}
		else if (!m_param.type_reward_fetch_flag.IsBitSet(type))
		{
			info.extra_reward_flag_list[type] = Protocol::SCActiveSystemInfo::EXTRA_REWARD_FLAG_FETCHABLE;
		}
		else
		{
			info.extra_reward_flag_list[type] = Protocol::SCActiveSystemInfo::EXTRA_REWARD_FLAG_ALREADY_FETCH;
		}
		info.task_count += 1;
	}

	m_module_mgr->NetSend(&info, sizeof(info));
}

void ActiveSystem::SendSigleActiveInfo(int active_id)
{
	if (active_id < 0 || active_id >= ACTIVE_SYSTEM_TYPE_MAX_IN_DB)return;
	SCActiveSystemSingleInfo info;
	info.active_id = active_id;
	info.active_flag = m_param.active_flag;
	info.level = m_param.level;
	info.integral_active = m_param.integral_active;
	info.progress = m_param.progress[active_id];
	if (!m_param.type_flag.IsBitSet(active_id))
	{
		info.extra_reward_flag = Protocol::SCActiveSystemInfo::EXTRA_REWARD_FLAG_CANNOT_FETCH;
	}
	else if (!m_param.type_reward_fetch_flag.IsBitSet(active_id))
	{
		info.extra_reward_flag = Protocol::SCActiveSystemInfo::EXTRA_REWARD_FLAG_FETCHABLE;
	}
	else
	{
		info.extra_reward_flag = Protocol::SCActiveSystemInfo::EXTRA_REWARD_FLAG_ALREADY_FETCH;
	}
	m_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void ActiveSystem::GmAddActiveDegree(int add_num)
{
	if (add_num > 0)
	{
		m_param.integral_active += add_num;
		ROLE_LOG_QUICK6(LOG_TYPE_ACTIVE_POINT, m_module_mgr->GetRole(), add_num, m_param.integral_active, __FUNCTION__, NULL);
		this->SendActiveInfo();
	}
}

bool ActiveSystem::IsStartedActive(int type)
{
	if (type < 0 || type >= ACTIVE_SYSTEM_TYPE_MAX_IN_DB) return false;
	return m_param.progress[type] != 0;
}

void ActiveSystem::OnFetchSpecialActiveReward(int special_active_type)
{
	const ActiveSystemSpecialCfg * cfg = LOGIC_CONFIG->GetActiveSystemConfig()->GetSpecialActiveCfg(special_active_type, m_module_mgr->GetRole()->GetLevel());
	if (NULL == cfg)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_param.special_active_reward_flag.IsBitSet(cfg->special_type))
	{
		return; // 已领取奖励
	}

	switch (cfg->special_type)
	{
		case ACTIVITY_SYSTEM_SPECIAL_TYPE_WILD_BOSS:
		{
			if (m_module_mgr->GetRole()->GetWildBossRewardTimes() < cfg->need_count)
			{
				return;
			}
		}
		break;
		default:
		return;
	}

	if (cfg->reward_cfg.reward_num > 0 && !m_module_mgr->GetKnapsack()->CheckForPutList(cfg->reward_cfg.reward_num, cfg->reward_cfg.reward_list))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (cfg->reward_cfg.reward_num > 0 && !m_module_mgr->GetKnapsack()->PutList(cfg->reward_cfg.reward_num, cfg->reward_cfg.reward_list, PUT_REASON_SPECIAL_ACTIVE_REWARD))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_module_mgr->GetMoney()->AddCoinBind(cfg->reward_cfg.reward_coin, __FUNCTION__);
	m_module_mgr->GetRoleCrystalFb()->AddCrystalFbEnergy(cfg->reward_cfg.reward_energy, __FUNCTION__, ENTER_ENERGY_CHANGE_TYPE_OTHER);
	m_module_mgr->GetRole()->AddExp(cfg->reward_cfg.reward_exp, ADD_EXP_TYPE_ACTIVE_SYSTEM, __FUNCTION__);

	m_param.special_active_reward_flag.SetBit(cfg->special_type);
	this->OnSendSpecialActiveInfo();

	gamelog::g_log_active_system.printf(LL_INFO, "%s role[%d,%s] special_type[%d]", __FUNCTION__, m_module_mgr->GetRoleUID(), m_module_mgr->GetRoleName(), cfg->special_type);
}

void ActiveSystem::OnSendSpecialActiveInfo()
{
	Protocol::SCSpecialActiveInfo info;
	info.special_active_reward_flag = m_param.special_active_reward_flag;

	m_module_mgr->NetSend(&info, sizeof(info));
}

void ActiveSystem::AddWeekDegreeParam(int active_degree_type, int param)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_ACTIVE_SYSTEM))
	{//系统未开启 不记录
		return;
	}

	if (0 >= param)
	{
		return;
	}

	UNSTD_STATIC_CHECK(ACTIVE_DEGREE_TYPE_MAX == 16);
	switch (active_degree_type)
	{
		//直接增加
	case ACTIVE_DEGREE_TYPE_ZHEN_ZHONG:
	case ACTIVE_DEGREE_TYPE_BOUNTY_TASK:
	case ACTIVE_DEGREE_TYPE_ENERGY:
	case ACTIVE_DEGREE_TYPE_WORLD_BOSS:
	case ACTIVE_DEGREE_TYPE_WORLD_BOSS_2:
	case ACTIVE_DEGREE_TYPE_GUILD_ANSWER:
	case ACTIVE_DEGREE_TYPE_GUILD_FIGHT:
	case ACTIVE_DEGREE_TYPE_TEAM_FIGHT:
	case ACTIVE_DEGREE_TYPE_MAZE_RACER:
	case ACTIVE_DEGREE_TYPE_ONLY_FIGHT:
	case ACTIVE_DEGREE_TYPE_GUILD_HONOR_BATTLE:
	case ACTIVE_DEGREE_TYPE_CHIEF_ELECTION:
	case ACTIVE_DEGREE_TYPE_HUNDRED_GHOST:
	case ACTIVE_DEGREE_TYPE_BRAVE_GROUND:
	case ACTIVE_DEGREE_TYPE_SILK_ROADS:
	case ACTIVE_DEGREE_TYPE_RELIC_LOOTING:
		{
			m_week_task_param.task_info_list[active_degree_type].task_param += param;
		}
		break;
	default:
		return;
	}

	//需要在完成时增加活跃
	if (m_week_base_param.week_task_today_finish_flag.IsBitSet(active_degree_type))
	{
		return;
	}
	
	const WeekActiveDegreeTaskCfg* cfg = LOGIC_CONFIG->GetActiveSystemConfig()->GetWeekActiveDegreeTaskCfg(active_degree_type);
	if (NULL == cfg)
	{
		return;
	}

	if (cfg->task_param > m_week_task_param.task_info_list[active_degree_type].task_param)
	{
		return;
	}

	m_week_base_param.week_task_today_finish_flag.SetBit(active_degree_type);
	m_week_base_param.week_active_degree += cfg->week_active_degree;

	gamelog::g_log_active_system.printf(LL_INFO, "ActiveSystem::AddWeekDegreeParam role[%d,%s] type[%d] week_active_degree[%d]",
		m_module_mgr->GetRoleUID(), m_module_mgr->GetRoleName(), 
		active_degree_type, m_week_base_param.week_active_degree);

	this->SendWeekInfo();
	this->OnCompleteTurntableTask(ACTIVE_TURNTABLE_TYPE_WEEK_ACTIVE, m_week_base_param.week_active_degree);
}

void ActiveSystem::FetchWeekDegreeReward(int seq)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_ACTIVE_SYSTEM))
	{
		return;
	}

	const WeekActiveDegreeRewardCfg* cfg = LOGIC_CONFIG->GetActiveSystemConfig()->GetWeekActiveDegreeRewardCfg(m_module_mgr->GetRole()->GetLevel(), seq);
	if (NULL == cfg)
	{
		return;
	}
	int reward_index = seq - 1;
	if (m_week_base_param.week_fetch_flag.IsBitSet(reward_index))
	{
		return;
	}

	if (cfg->need_degree > m_week_base_param.week_active_degree)
	{
		return;
	}

	m_week_base_param.week_fetch_flag.SetBit(reward_index);

	if (!m_module_mgr->GetKnapsack()->CheckForPutList2(cfg->reward_item_vec))
	{
		return;
	}

	short reward_num = (short)cfg->reward_item_vec.size();
	if (0 < reward_num)
	{
		m_module_mgr->GetKnapsack()->PutList(reward_num, &cfg->reward_item_vec[0], PUT_REASON_ACTIVE_SYSTEM_REWARD);
	}
	gamelog::g_log_active_system.printf(LL_INFO, "ActiveSystem::FetchWeekDegreeReward role[%d,%s] seq[%d] week_active_degree[%d]",
		m_module_mgr->GetRoleUID(), m_module_mgr->GetRoleName(),
		seq, m_week_base_param.week_active_degree);

	this->SendWeekInfo();
}

void ActiveSystem::SendWeekInfo()
{
	Protocol::SCWeekActiveDegreeInfo info;
	info.week_task_today_finish_flag = m_week_base_param.week_task_today_finish_flag;
	info.week_fetch_flag = m_week_base_param.week_fetch_flag;
	info.week_active_degree = m_week_base_param.week_active_degree;

	m_module_mgr->NetSend(&info, sizeof(info));
}

void ActiveSystem::WeekDegreeDailyReset()
{
	m_week_base_param.week_task_today_finish_flag.Reset();
	for (int i = 0; i < ACTIVE_DEGREE_TYPE_MAX; i++)
	{
		m_week_task_param.task_info_list[i].task_param = 0;
	}
}

void ActiveSystem::WeekDegreeWeekReset()
{
	m_week_base_param.week_active_degree = 0;
	m_week_base_param.week_fetch_flag.Reset();
}

void ActiveSystem::GmAddWeekActiveDegree(int add_num)
{
	m_week_base_param.week_active_degree += add_num;
	this->OnCompleteTurntableTask(ACTIVE_TURNTABLE_TYPE_WEEK_ACTIVE, m_week_base_param.week_active_degree);

	this->SendWeekInfo();
}

void ActiveSystem::GmAddTurntableTime(int add_num)
{
	m_turntable_param.turntable_time += add_num;
	this->SendTurntableInfo();
}

void ActiveSystem::OnCompleteTurntableTask(int task_type, int param)
{
	bool is_send = false;

	switch (task_type)
	{
	case ACTIVE_TURNTABLE_TYPE_JOIN_ACTIVITY:
		{
			const TurntableTaskCfg * cfg = LOGIC_CONFIG->GetActiveSystemConfig()->GetTurntableActivityCfg(param);
			if (NULL == cfg)
				return;

			if (param != cfg->param)
				return;

			if (m_turntable_param.turntable_day_task_flag.IsBitSet(cfg->seq))
				return;
			
			if (m_turntable_param.turntable_time + cfg->turntable_num > MAX_INT) m_turntable_param.turntable_time = MAX_INT;
			else m_turntable_param.turntable_time += cfg->turntable_num;
			
			m_turntable_param.turntable_day_task_flag.SetBit(cfg->seq);
			is_send = true;
			//log
			gamelog::g_log_active_system.printf(LL_INFO, "OnCompleteTurntableTask role[%d,%s] active_type[%d]", 
				m_module_mgr->GetRoleUID(), m_module_mgr->GetRoleName(), param);
		}
		break;
	case ACTIVE_TURNTABLE_TYPE_WEEK_ACTIVE:
		{
			const std::vector<TurntableTaskCfg> & cfg_list = LOGIC_CONFIG->GetActiveSystemConfig()->GetTurntableWeekCfg();
			for (int i = (int)cfg_list.size() - 1; i >= 0; i--)
			{
				if(param < cfg_list[i].param)
					continue;

				if(m_turntable_param.turntable_week_task_flag.IsBitSet(cfg_list[i].seq))
					break;

				if (m_turntable_param.turntable_time + cfg_list[i].turntable_num > MAX_INT) m_turntable_param.turntable_time = MAX_INT;
				else m_turntable_param.turntable_time += cfg_list[i].turntable_num;

				m_turntable_param.turntable_week_task_flag.SetBit(cfg_list[i].seq);
				is_send = true;
				//log
				gamelog::g_log_active_system.printf(LL_INFO, "OnCompleteTurntableTask role[%d,%s] week_active_degree[%d]",
					m_module_mgr->GetRoleUID(), m_module_mgr->GetRoleName(), cfg_list[i].turntable_num
				);
			}
		}
		break;
	default:
		return;
	}

	if(is_send) this->SendTurntableInfo();
}

void ActiveSystem::OnLuckTurntable()
{
	if (m_turntable_param.turntable_time <= 0)
	{
		m_module_mgr->NoticeNum(errornum::EN_CAN_NOT_START_TASK);
		return;
	}

	const TurntableRewardCfg * cfg = LOGIC_CONFIG->GetActiveSystemConfig()->GetTurntableRewardCfg(m_module_mgr->GetRole()->GetLevel());
	if (NULL == cfg)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	std::vector<ItemConfigData> convert_item_list;
	if (m_module_mgr->GetKnapsack()->OnRoleAttrPelletConvert(cfg->item, convert_item_list))
	{
		if (!m_module_mgr->GetKnapsack()->CheckForPutList2(convert_item_list))
		{
			m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
		if (!m_module_mgr->GetKnapsack()->PutVec(convert_item_list, PUT_REASON_ACTIVE_TURNTABLE))
		{
			m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}
	else
	{
		convert_item_list.push_back(cfg->item);
		if (!m_module_mgr->GetKnapsack()->CheckForPut(cfg->item.item_id, cfg->item.num))
		{
			m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}

		if (!m_module_mgr->GetKnapsack()->Put(cfg->item, PUT_REASON_ACTIVE_TURNTABLE))
		{
			m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}

	m_module_mgr->GetKnapsack()->NotifyGetItemListVec(convert_item_list, GET_RAASON_ACTIVE_TURNTABLE);
	m_turntable_param.turntable_time -= 1;

	Protocol::SCActiveTurntableRewardInfo info;
	info.seq = cfg->seq;
	info.turntable_time = m_turntable_param.turntable_time;
	info.count = 0;
	for (int i = 0; i < (int)convert_item_list.size(); i++)
	{
		info.item_list[info.count].item_id = convert_item_list[i].item_id;
		info.item_list[info.count].is_bind = convert_item_list[i].is_bind ? 1 : 0;
		info.item_list[info.count].num = convert_item_list[i].num;
		info.count++;
	}
	int length = sizeof(info) - (Protocol::SCActiveTurntableRewardInfo::MAX_NOTICE_ITEM_LIST_NUM - info.count) * sizeof(info.item_list[0]);
	m_module_mgr->NetSend(&info, length);
}

void ActiveSystem::SendTurntableInfo()
{
	Protocol::SCActiveTurntableInfo info;
	info.turntable_time = m_turntable_param.turntable_time;
	info.turntable_day_task_flag = m_turntable_param.turntable_day_task_flag;
	info.turntable_week_task_flag = m_turntable_param.turntable_week_task_flag;

	m_module_mgr->NetSend(&info, sizeof(info));
}

void ActiveSystem::OnAddActiveCheckGiveGuessCoin(int add_num)
{
	if (!UTAShadow::Instance().CanGiveGuessCoinForActive()) return;
	
	if (m_module_mgr->GetRole()->GetLevel() < UTAConfig::Instance().GetOtherCfg().level_limit)
	{
		return;
	}

	m_param.add_guess_coin_active_num += add_num;
	gamelog::g_log_active_system.printf(LL_INFO, "OnAddActiveCheckGiveGuessCoin role[%d,%s] add_num[%d] total_num[%d]",
		m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), add_num, m_param.add_guess_coin_active_num);

	this->CheckGiveGuessCoin();
}

void ActiveSystem::CheckGiveGuessCoin()
{
	if (!UTAShadow::Instance().CanGiveGuessCoinForActive()) return;

	int need_active = UTAConfig::Instance().GetGiveGuessCoinNeedActive();
	if (need_active <= 0) return;

	while (m_param.add_guess_coin_active_num >= need_active)
	{
		m_param.add_guess_coin_active_num -= need_active;
		gamelog::g_log_active_system.printf(LL_INFO, "CheckGiveGuessCoin role[%d,%s] minus_num[%d] total_num[%d]",
			m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), need_active, m_param.add_guess_coin_active_num);

		const UTAOtherCfg& cfg = UTAConfig::Instance().GetOtherCfg();
		if (m_module_mgr->GetKnapsack()->CheckForPutList2(cfg.active_rewards))
		{
			m_module_mgr->GetKnapsack()->PutVec(cfg.active_rewards, PUT_REASON_UTA_ACTIVE_REWARD);
		}	
		else
		{
			m_module_mgr->GetKnapsack()->SendMail(cfg.active_rewards, SNED_MAIL_TYPE_DEFAULT, true);
		}
	}
}
