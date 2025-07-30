#include "reloadconfigmanager.hpp"
#include "servercommon/performancecatch.hpp"
#include "servercommon/servercommon.h"
#include "config/logicconfigmanager.hpp"
#include "gamelog.h"
#include "engineadapter.h"
#include "utility/configpath.h"
#include "globalconfig/globalconfig.h"
#include "skill/skillpool.hpp"
#include "item/itempool.h"
#include "task/taskpool.h"
#include "monster/monsterpool.hpp"
#include "world.h"
#include "config/sharedconfig/sharedconfig.h"
#include "other/event/eventhandler.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "monster/monster_group_pool.hpp"
#include "global/trademarket/trademarketconfig.hpp"
#include "npc/npcpool.h"
#include "drop/droppool.hpp"
#include "config/otherconfig/sceneminemonsterconfig.hpp"
#include "config/otherconfig/noticeconfig.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "other/robot/robotconfig.hpp"
#include "global/xunbao/xunbao.hpp"
#include "servercommon/serverconfig/appearanceconfig.hpp"
#include "other/workshop/godequipupconfig.hpp"
#include "servercommon/serverconfig/battleconfig/battle_hallow_gift_pool.hpp"
#include "servercommon/asynctask/asynctaskfactorymanager.hpp"
#include "servercommon/asynctask/asynctaskmanager.hpp"
#include "asynctaskimpl/asyncreloadmonster.hpp"

ReloadConfigManager::ReloadConfigManager()
{

}

ReloadConfigManager::~ReloadConfigManager()
{

}

ReloadConfigManager & ReloadConfigManager::Instance()
{
	static ReloadConfigManager inst;
	return inst;
}

bool ReloadConfigManager::ReloadConfig(int reload_cfg_type, int param1, int param2)
{
	std::string config_dir = "../config";
	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "ConfigDir", &config_dir))
	{
		gamelog::g_log_world.printf(LL_ERROR, "ReloadConfigManager::ReloadConfig ConfigDir Read Failed!");
		return false;
	}

	if (config_dir[config_dir.length() - 1] != '/')
	{
		config_dir += '/';
	}

	if (RELOAD_CONFIG_TYPE_SHARED != reload_cfg_type)
	{
		config_dir += "gameworld/";
	}

//#ifdef _DEBUG
//	PerformanceCatch::Instance().GetElapseUsBegin(PERFORMANCE_CATCH_TYPE_RELOAD_CONFIG);
//#endif

	std::string err;

	switch (reload_cfg_type)
	{
	case RELOAD_CONFIG_TYPE_SPID_CONFIG:
	case RELOAD_CONFIG_TYPE_GLOBAL_CONFIG: // 重读全局配置 globalconfig.xml 
		{
			std::string config_name = "globalconfigmanager.xml"; 

			if (!GlobalConfig::Reload(config_dir, config_name, &err))
			{
				gamelog::g_log_world.printf(LL_ERROR, err.c_str());
				gamelog::g_log_world.printf(LL_ERROR, "GlobalConfig::Reload return FALSE.");
				return false;
			}

			gamelog::g_log_world.printf(LL_INFO, "GlobalConfig::Reload Success.");
		}
		break;

	case RELOAD_CONFIG_TYPE_SKILL: // 重读技能配置
		{
			std::string skill_dir = (config_dir + "skill/");
			if (!SkillPool::Reload(skill_dir.c_str(), &err))	
			{
				gamelog::g_log_world.printf(LL_ERROR, err.c_str());
				gamelog::g_log_world.printf(LL_ERROR, "SkillPoll::Reload return FALSE.");
				return false;
			}
			gamelog::g_log_world.printf(LL_INFO, "SkillPoll::Reload Success.");
		}
		break;

	case RELOAD_CONFIG_TYPE_TASK:	// 重读任务配置
		{
			if (!TaskPool::Reload(config_dir.c_str(), &err))	
			{
				gamelog::g_log_world.printf(LL_ERROR, err.c_str());
				gamelog::g_log_world.printf(LL_ERROR, "TaskPool::Reload return FALSE.");
				return false;
			}
			gamelog::g_log_world.printf(LL_INFO, "TaskPool::Reload Success.");
		}
		break;

	case RELOAD_CONFIG_TYPE_MONSTER: // 重读怪物配置
		{
			std::string config_name = "battlemonstermanager.xml";
			if (!MonsterPool::Reload(config_dir.c_str(), config_name, &err))	
			{
				gamelog::g_log_world.printf(LL_ERROR, err.c_str());
				gamelog::g_log_world.printf(LL_ERROR, "MonsterPool::Reload return FALSE.");
				return false;
			}

			config_name = "monster_group.xml";
			if (!MonsterGroupPool::Reload(config_dir.c_str() + config_name, &err))
			{
				gamelog::g_log_world.printf(LL_ERROR, err.c_str());
				gamelog::g_log_world.printf(LL_ERROR, "MonsterGroupPool::Reload return FALSE.");
				return false;
			}

			gamelog::g_log_world.printf(LL_INFO, "Monster Reload Success.");
		}
		break;

	case RELOAD_CONFIG_TYPE_ONE_MONSTER:
		{
			if (!MonsterPool::GetInstance().ReloadOneMonster(config_dir, param1, &err))
			{
				gamelog::g_log_world.printf(LL_ERROR, err.c_str());
				gamelog::g_log_world.printf(LL_ERROR, "MonsterPool::ReloadOneMonster[monster=%d] return FALSE.", param1);
				return false;
			}
			gamelog::g_log_world.printf(LL_INFO, "Reload One Monster[monster=%d] Success.", param1);
		}
		break;

	case RELOAD_CONFIG_TYPE_ITEM: // 重读物品配置
		{
			std::string config_name = "itemmanager.xml";
			if (!ItemPool::Reload(config_dir, config_name, &err))
			{
				gamelog::g_log_world.printf(LL_ERROR, err.c_str());
				gamelog::g_log_world.printf(LL_ERROR, "ItemPool::Reload return FALSE.");
				return false;
			}
			gamelog::g_log_world.printf(LL_INFO, "ItemPool::Reload Success.");
		}
		break;

	case RELOAD_CONFIG_TYPE_LOGIC_CONFIG: // 重读逻辑配置
		{
			if (!LogicConfigManager::Reload(config_dir, &err))
			{
				gamelog::g_log_world.printf(LL_ERROR, err.c_str());
				gamelog::g_log_world.printf(LL_ERROR, "LogicConfig::Reload return FALSE.");
				return false;
			}
			gamelog::g_log_world.printf(LL_INFO, "LogicConfig::Reload Success.");
		}
		break;

	case RELOAD_CONFIG_TYPE_LOGIC_CONFIG_ASYNC: // 异步重读逻辑
		{
			if (!LogicConfigManager::AsyncReload(config_dir, &err))
			{
				gamelog::g_log_world.printf(LL_ERROR, err.c_str());
				gamelog::g_log_world.printf(LL_ERROR, "LogicConfig::AsyncReload return FALSE.");
				return false;
			}
			gamelog::g_log_world.printf(LL_INFO, "LogicConfig::AsyncReload Success.");
		}
		break;

	case RELOAD_CONFIG_TYPE_DROP: // 重读掉落配置
		{
			std::string config_name = "dropmanager.xml";
			if (!DropPool::Reload(config_dir, config_name, &err))
			{
				gamelog::g_log_world.printf(LL_ERROR, err.c_str());
				gamelog::g_log_world.printf(LL_ERROR, "Droppool::Reload return FALSE.");
				return false;
			}
			gamelog::g_log_world.printf(LL_INFO, "Droppool::Reload Success.");
		}
		break;

	case RELOAD_CONFIG_TYPE_SCENE: // 重读场景配置
		{
			if (!SceneMineMonsterConfig::Reload(config_dir + "hide_monster.xml", &err))
			{
				gamelog::g_log_world.printf(LL_ERROR, err.c_str());
				gamelog::g_log_world.printf(LL_ERROR, "SceneMineMonsterConfig::Reload return FALSE.");
				return false;
			}

			if (!World::GetInstWorld()->ReloadScene(config_dir, &err))
			{
				gamelog::g_log_world.printf(LL_ERROR, err.c_str());
				gamelog::g_log_world.printf(LL_ERROR, "SceneConfig::Reload return FALSE.");
				return false;
			}
			gamelog::g_log_world.printf(LL_INFO, "SceneConfig::Reload Success.");
		}
		break;

	case RELOAD_CONFIG_TYPE_SHARED:
		{
			//if (!SharedConfig::Reload(config_dir, &err))
			//{
			//	gamelog::g_log_world.printf(LL_ERROR, err.c_str());
			//	gamelog::g_log_world.printf(LL_ERROR, "SharedConfig::Reload return FALSE.");
			//	return false;
			//}
			//gamelog::g_log_world.printf(LL_INFO, "SharedConfig::Reload Success.");
		}
		break;

	case RELOAD_CONFIG_TYPE_ONE_ITEM:
		{
			if (!ITEMPOOL->ReloadOneItem(config_dir, param1, param2, &err))
			{
				gamelog::g_log_world.printf(LL_ERROR, err.c_str());
				gamelog::g_log_world.printf(LL_ERROR, "ItemPool::ReloadOneItem return FALSE.");
				return false;
			}
			gamelog::g_log_world.printf(LL_INFO, "ItemPool::ReloadOneItem Success.");
		}
		break;

	case RELOAD_CONFIG_TYPE_ACTIVITY: 
		{
			
		}
		break;

	case RELOAD_CONFIG_TYPE_BATTLE:
		{
			// 技能、怪物、BUFF、特效等
			bool is_check_reload_times = (0 != LOGIC_CONFIG->GetSundryConfig()->GetOtherCfg().server_check) ? true : false;
			long long elapse_time = 0;
			std::string err_info;

			//
			this->OnStartCalcTime(is_check_reload_times, PERFORMANCE_TEST_SKILL_CONFIG_RELOAD);
			if (!SkillPool::Reload(config_dir + "skill/", &err_info))
			{
				gamelog::g_log_world.printf(LL_INFO, "ReloadSkill ERROR with error[%s]", err_info.c_str());
				return false;
			}
			elapse_time += ReloadConfigManager::Instance().OnEndCalcTime(is_check_reload_times, PERFORMANCE_TEST_SKILL_CONFIG_RELOAD, "SkillPool");

			//
			this->OnStartCalcTime(is_check_reload_times, PERFORMANCE_TEST_MONSTER_CONFIG_RELOAD);
			if (!MonsterPool::Reload(config_dir, "battlemonstermanager.xml", &err_info))
			{
				gamelog::g_log_world.printf(LL_INFO, "ReloadMonster ERROR with error[%s]", err_info.c_str());
				return false;
			}
			elapse_time += ReloadConfigManager::Instance().OnEndCalcTime(is_check_reload_times, PERFORMANCE_TEST_MONSTER_CONFIG_RELOAD, "MonsterPool");

			//
			this->OnStartCalcTime(is_check_reload_times, PERFORMANCE_TEST_EFFECT_CONFIG_RELOAD);
			if (!BattleSpecialEffectPool::Reload(config_dir + "logicconfig/special_effect.xml", &err_info))
			{
				gamelog::g_log_world.printf(LL_INFO, "ReloadBattleSpecialEffect ERROR with error[%s]", err_info.c_str());
				return false;
			}
			elapse_time += ReloadConfigManager::Instance().OnEndCalcTime(is_check_reload_times, PERFORMANCE_TEST_EFFECT_CONFIG_RELOAD, "BattleSpecialEffectPool");

			if (is_check_reload_times)
			{
				printf("Reload Battle Config elapse[%lld]us=[%lld]ms\n", elapse_time, elapse_time / 1000);
				gamelog::g_log_debug.printf(LL_INFO, "Reload Battle Config elapse[%lld]us=[%lld]ms", elapse_time, elapse_time / 1000);
			}
		}
		break;

	case RELOAD_CONFIG_TYPE_SPECIAL_EFFECT:
		{
			std::string err_info;
			if (!BattleSpecialEffectPool::Reload(config_dir + "logicconfig/special_effect.xml", &err_info))
			{
				gamelog::g_log_world.printf(LL_INFO, "ReloadBattleSpecialEffect ERROR with error[%s]", err_info.c_str());
				return false;
			}
			gamelog::g_log_world.printf(LL_INFO, "BattleSpecialEffectPool::Reload Success.");
		}
		break;

	case RELOAD_CONFIG_TYPE_BATTLE_HALLOW:
		{
			std::string err_info;
			if (!BattleHallowGiftPool::Reload(config_dir + "logicconfig/", &err_info))
			{
				gamelog::g_log_world.printf(LL_INFO, "ReloadBattleHallowGiftPool ERROR with error[%s]", err_info.c_str());
				return false;
			}
			gamelog::g_log_world.printf(LL_INFO, "BattleHallowGiftPool::Reload Success.");
		}
		break;

	case RELOAD_CONFIG_TYPE_TRADE_MARKET:
		{
			if (!TradeMarketConfig::Reload(config_dir + "trade_market.xml", &err))
			{
				gamelog::g_log_world.printf(LL_ERROR, err.c_str());
				gamelog::g_log_world.printf(LL_ERROR, "TradeMarketConfig::Reload return FALSE.");
				return false;
			}
			gamelog::g_log_world.printf(LL_INFO, "TradeMarketConfig::Reload Success.");
		}
		break;

	case RELOAD_CONFIG_TYPE_NPC:
		{
			if (!NpcPool::Reload(config_dir, &err))
			{
				gamelog::g_log_world.printf(LL_ERROR, err.c_str());
				gamelog::g_log_world.printf(LL_ERROR, "NpcPool::Reload return FALSE.");
				return false;
			}
			gamelog::g_log_world.printf(LL_INFO, "NpcPool::Reload Success.");
		}
		break;

	case RELOAD_CONFIG_TYPE_NOTICE_CONFIG:
		{
			if (!NoticeConfig::Reload(config_dir + "logicconfig/notice_cfg.xml", &err))
			{
				gamelog::g_log_world.printf(LL_ERROR, err.c_str());
				gamelog::g_log_world.printf(LL_ERROR, "NoticeConfig::Reload return FALSE.");
				return false;
			}
			gamelog::g_log_world.printf(LL_INFO, "NoticeConfig::Reload Success.");
		}
		break;
	case RELOAD_CONFIG_TYPE_APPEARANCE_CONFIG:
		{
			std::string appearance_cfg_dir = config_dir + "/gameworld/logicconfig/";

			if (!AppearanceConfig::Instance().Reload(appearance_cfg_dir, "image_change.xml", &err))
			{
				gamelog::g_log_world.printf(LL_ERROR, err.c_str());
				gamelog::g_log_world.printf(LL_ERROR, "AppearanceConfig::Reload return FALSE.");
				return false;
			}
			gamelog::g_log_world.printf(LL_INFO, "AppearanceConfig::Reload Success.");
		}
		break;
	case RELOAD_CONFIG_TYPE_ROBOT:	
		{
			if (!ROBOTCONFIG->Reload(config_dir + "logicconfig/robot.xml", &err))
			{
				gamelog::g_log_world.printf(LL_ERROR, err.c_str());
				gamelog::g_log_world.printf(LL_ERROR, "ROBOTCONFIG->Reload return FALSE.");
				return false;
			}
			gamelog::g_log_world.printf(LL_INFO, "ROBOTCONFIG->Reload Success.");
		}
		break;
	case RELOAD_CONFIG_TYPE_GOD_EQUIP:
		{
			if (!GODEQUIP_CONFIG->Reload(config_dir + "logicconfig/godequip_up.xml", &err))
			{
				gamelog::g_log_world.printf(LL_ERROR, err.c_str());
				gamelog::g_log_world.printf(LL_ERROR, "GODEQUIP_CONFIG->Reload return FALSE.");
				return false;
			}
			gamelog::g_log_world.printf(LL_INFO, "GODEQUIP_CONFIG->Reload Success.");
		}
		break;

	case RELOAD_CONFIG_TYPE_ASYNC_BATTLE_MONSTER:
		{
			AsyncReloadMonster* arm = new AsyncReloadMonster();
			if (NULL == arm)
			{
				gamelog::g_log_world.printf(LL_ERROR, "AsyncReloadMonster IS NULL");
				return false;
			}
			arm->SetConfigDir(config_dir);
			if (!AsyncTaskManager::Instance().AddTask(arm))
			{
				gamelog::g_log_world.printf(LL_ERROR, "AsyncReloadMonster Add Async Task Failed.");
				return false;
			}
			gamelog::g_log_world.printf(LL_INFO, "AsyncReloadMonster Add Async Task Success.");
		}
		break;
	}

	EventHandler::Instance().OnReloadConfig(reload_cfg_type, param1, param2);

//#ifdef _DEBUG
//	long long elapse_us = PerformanceCatch::Instance().GetElapseUsEnd(PERFORMANCE_CATCH_TYPE_RELOAD_CONFIG);
//	printf("Reload Config type[%d] elapse[%lld]us=[%lld]ms\n", reload_cfg_type, elapse_us, elapse_us / 1000);
//	gamelog::g_log_debug.printf(LL_INFO, "Reload Config type[%d] elapse[%lld]us=[%lld]ms", reload_cfg_type, elapse_us, elapse_us / 1000);
//#endif

	return true;
}

void ReloadConfigManager::OnStartCalcTime(bool is_check_reload_times, int catch_type)
{
	if (is_check_reload_times)
	{
		PerformanceCatch::Instance().GetElapseUsBegin(catch_type);
	}
}

long long ReloadConfigManager::OnEndCalcTime(bool is_check_reload_times, int catch_type, const char * reason, int param)
{
	if (!is_check_reload_times || NULL == reason) return 0;

	long long elapse_us = PerformanceCatch::Instance().GetElapseUsEnd(catch_type);
	if (param < 0)
	{
		printf("Reload %s elapse[%lld]us=[%lld]ms\n", reason, elapse_us, elapse_us / 1000);
		gamelog::g_log_debug.printf(LL_INFO, "Reload %s elapse[%lld]us=[%lld]ms", reason, elapse_us, elapse_us / 1000);
	}
	else
	{
		printf("Reload %s[%d] elapse[%lld]us=[%lld]ms\n", reason, param, elapse_us, elapse_us / 1000);
		gamelog::g_log_debug.printf(LL_INFO, "Reload %s[%d] elapse[%lld]us=[%lld]ms", reason, param, elapse_us, elapse_us / 1000);
	}

	return elapse_us;
}

