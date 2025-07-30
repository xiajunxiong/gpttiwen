#include "common/typetransform.h"
#include "world.h"
#include "utility/configpath.h"
#include "internalcomm.h"
#include "gamelog.h"
#include "servercommon/internalprotocal/lgprotocal.h"
#include "servercommon/internalprotocal/gglobalprotocal.h"
#include "servercommon/serverconfig/serverconfigpool.h"
#include "servercommon/namefilter.hpp"
#include "servercommon/serverconfig/localconfig.hpp"
#include "servercommon/serverconfig/nameconfig.h"
#include "servercommon/serverconfig/appearanceconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "config/sharedconfig/sharedconfig.h"
#include "global/usercache/usercache.hpp"
#include "global/courseofgrowthmanager/courseofgrowthmanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "checkresourcecenter.hpp"
#include "servercommon/serverconfig/bigcrossconfig/bigcrossserverconfig.hpp"

void World::RecalNextResetDataTimeStamp()
{
	time_t last_reset_module_data_timestamp = m_last_reset_role_module_timestamp;

	const tm *last_reset_tm = ::localtime(&last_reset_module_data_timestamp);
	if (NULL != last_reset_tm)
	{
		time_t zero_timestamp = GetZeroTime(last_reset_module_data_timestamp);
		if (last_reset_tm->tm_hour >= ROLE_RESET_MODULE_DATA_TM_HOUR)
		{
			m_next_reset_role_module_timestamp = zero_timestamp + SECOND_PER_DAY + ROLE_RESET_MODULE_DATA_TM_HOUR * SECOND_PER_HOUR;
		}
		else
		{
			m_next_reset_role_module_timestamp = zero_timestamp + ROLE_RESET_MODULE_DATA_TM_HOUR * SECOND_PER_HOUR;
		}
	}
}

bool World::ListenForGateway()
{
	int backlog = 64;
	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "World" / "GameServer" / "Backlog", &backlog))
	{
		gamelog::g_log_world.print(LL_MAINTANCE, "World::ListenForGateway Backlog read failed!!");
		return false;
	}

	bool ret = m_network->Listen(m_listen_port, backlog);
	if (!ret)
	{
		gamelog::g_log_world.print(LL_MAINTANCE, "NetListen return FAIL!");
		return false;
	}
	gamelog::g_log_world.printf(LL_MAINTANCE, "Server Listen[%d] suc.", m_listen_port);
	return true;
}

bool World::ListenForTelnet()
{
	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "World" / "GameServer" / "TelnetListenPort", &m_telnet_listen_port))
	{
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, GameServer TelNetListenPort read failed!");
		return false;
	}

	int backlog = 64;
	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "World" / "GameServer" / "TelnetBacklog", &backlog))
	{
		gamelog::g_log_world.print(LL_MAINTANCE, "World::ListenForTelnet TelnetBacklog read failed!!");
		return false;
	}

	bool ret = InternalComm::Instance().NetListen(m_telnet_listen_port, backlog);
	if (!ret)
	{
		gamelog::g_log_world.print(LL_MAINTANCE, "NetTelnetListen return FAIL!");
		return false;
	}
	gamelog::g_log_world.printf(LL_MAINTANCE, "Server TelnetListen[%d] suc.", m_telnet_listen_port);
	return true;
}

bool World::ConnectToDB()
{
	// 角色相关部分
	std::string db_server_ip = ServerConfigPool::Instance().common_config.ipconfigs.db_server_addr.DBAccounter.module_addr.ip;
	Port db_server_port = ServerConfigPool::Instance().common_config.ipconfigs.db_server_addr.DBAccounter.module_addr.port;

	rmi::Session accounter_session = EngineAdapter::Instance().RMICreateSession(db_server_ip.c_str(), db_server_port);
	if (accounter_session.handle == 0)
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "Connect Accounter DBLayer[%s:%d] Fail!", db_server_ip.c_str(), (int)db_server_port);
		return false;
	}
	DBSession db_accounter_session(accounter_session, db_server_ip, db_server_port);

	unsigned int role_db_num = ServerConfigPool::Instance().common_config.ipconfigs.db_server_addr.DBRoleCount;
	if (role_db_num == 0 || role_db_num > (unsigned int)DBHandler::MAX_ROLE_DB_INDEX)
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "Role DBLayer NUM:%d is NOT valid!", role_db_num);
		return false;
	}
	DBSession role_db_session_list[DBHandler::MAX_ROLE_DB_INDEX];

	for (unsigned int i = 0; i < role_db_num; ++i)
	{
		std::string role_db_server_ip = ServerConfigPool::Instance().common_config.ipconfigs.db_server_addr.DBRoleList[i].module_addr.ip;
		Port role_db_server_port = ServerConfigPool::Instance().common_config.ipconfigs.db_server_addr.DBRoleList[i].module_addr.port;

		rmi::Session role_db = EngineAdapter::Instance().RMICreateSession(role_db_server_ip.c_str(), role_db_server_port);
		if (role_db.handle == 0)
		{
			gamelog::g_log_world.printf(LL_MAINTANCE, "Connect Role DBLayer[%s:%d] Fail!", role_db_server_ip.c_str(), (int)role_db_server_port);
			return false;
		}
		role_db_session_list[i] = DBSession(role_db, role_db_server_ip, role_db_server_port);
	}

	// 全局相关部分
	std::string global_db_server_ip = ServerConfigPool::Instance().common_config.ipconfigs.db_server_addr.DBGlobal.module_addr.ip;
	Port global_db_server_port = ServerConfigPool::Instance().common_config.ipconfigs.db_server_addr.DBGlobal.module_addr.port;

	rmi::Session global_session = EngineAdapter::Instance().RMICreateSession(global_db_server_ip.c_str(), global_db_server_port);
	if (global_session.handle == 0)
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "Connect Center DBLayer[%s:%d] Fail!", global_db_server_ip.c_str(), (int)global_db_server_port);
		return false;
	}
	gamelog::g_log_world.printf(LL_MAINTANCE, "Connect Center DBLayer[%s:%d] Suc!", global_db_server_ip.c_str(), (int)global_db_server_port);

	DBSession db_global_session(global_session, global_db_server_ip, global_db_server_port);

	std::string system_db_server_ip = ServerConfigPool::Instance().common_config.ipconfigs.db_server_addr.DBSystem.module_addr.ip;
	Port system_db_server_port = ServerConfigPool::Instance().common_config.ipconfigs.db_server_addr.DBSystem.module_addr.port;

	rmi::Session system_session = EngineAdapter::Instance().RMICreateSession(system_db_server_ip.c_str(), system_db_server_port);
	if (system_session.handle == 0)
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "Connect System DBLayer[%s:%d] Fail!", system_db_server_ip.c_str(), (int)system_db_server_port);
		return false;
	}
	gamelog::g_log_world.printf(LL_MAINTANCE, "Connect System DBLayer[%s:%d] Suc!", system_db_server_ip.c_str(), (int)system_db_server_port);

	DBSession db_system_session(system_session, system_db_server_ip, system_db_server_port);

	InternalComm::Instance().SetDBSession(db_accounter_session, role_db_session_list, role_db_num, db_global_session, db_system_session, DBSession());

	gamelog::g_log_world.printf(LL_MAINTANCE, "Connect to DataAccessServer suc. Accounter[%s:%d], Role_db_num:%d", db_server_ip.c_str(), (int)db_server_port, role_db_num);
	return true;
}

bool World::LoadFromDBDirectly()
{
	unsigned int role_db_num = ServerConfigPool::Instance().common_config.ipconfigs.db_server_addr.DBRoleCount;
	if (role_db_num != 1)
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "LoadFromDBDirectly Role DBLayer NUM:%d is NOT valid!", role_db_num);
		return false;
	}
	
	DBModuleAddr &db_cfg = ServerConfigPool::Instance().common_config.ipconfigs.db_server_addr.DBRoleList[0];

	

	TMysqlStatement stMysqlStatement;
	CMysqlUtility::Init(&stMysqlStatement);


	char szErrMsg[512];
	int iRet = CMysqlUtility::ConnectDB(&stMysqlStatement,
				db_cfg.db_config.mysql_addr.ip.c_str(), db_cfg.db_config.mysql_db_username.c_str(),
				db_cfg.db_config.mysql_db_password.c_str(), db_cfg.db_config.mysql_db_name.c_str(),
				db_cfg.db_config.mysql_addr.port, szErrMsg);

	
	if (0 != iRet)
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "UserCacheManager::LoadAllUser failed :%d---------------- \n",iRet);
		CMysqlUtility::CloseDatabase(&stMysqlStatement);
		return false;
	}

	bool ret = true;
	
	{
		// 加载角色缓存
		ret = UserCacheManager::Instance().LoadAllUser(stMysqlStatement);
		if (!ret)
		{
			CMysqlUtility::CloseDatabase(&stMysqlStatement);

			gamelog::g_log_world.printf(LL_MAINTANCE, "UserCacheManager::LoadAllUser Fail!");
			return false;
		}
	}


	{
		ret = CourseOfGrowthManager::Instance().LoadAllUser(stMysqlStatement);
		if (!ret)
		{
			CMysqlUtility::CloseDatabase(&stMysqlStatement);

			gamelog::g_log_world.printf(LL_MAINTANCE, "CourseOfGrowthManager::LoadAllUser Fail!");
			return false;
		}
	}

	CMysqlUtility::CloseDatabase(&stMysqlStatement);
	
	return true;
}

bool World::LoadOriginDBGlobalCache(UniqueServerID unique_sid)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return false;
	}

	if (!UserCacheManager::Instance().InitOriginAllUserCache(unique_sid))
	{
		gamelog::g_log_world.printf(LL_CRITICAL, "World::InitOriginAllUserCache InitOriginAllUserCache failed !!! plat_type: %d, server_id: %d",
			unique_sid.plat_type, unique_sid.server_id);
		return false;

	}

	return true;
}

bool World::ConnectToLoginServer()
{
	int game_thread = 0;
	std::string login_server_ip = ServerConfigPool::Instance().common_config.ipconfigs.login_server_addr.inner_connect_addr.server_ip;
	Port login_server_port = ServerConfigPool::Instance().common_config.ipconfigs.login_server_addr.inner_connect_addr.for_gameserver.port;

	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "GameThread", &game_thread))
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "ConnectToLoginServer GameThread read failed!!");
		return false;
	}
	
	bool ret = InternalComm::Instance().NetConnect(login_server_ip.c_str(), login_server_port, &InternalComm::Instance().m_login_server_netid);
	if (!ret)
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "Connect To LoginServer[%s:%d] Fail!", login_server_ip.c_str(), (int)login_server_port);
		return false;
	}

	EngineAdapter::Instance().m_game_thread = game_thread;

	gamelog::g_log_world.printf(LL_MAINTANCE, "Connect to LoginServer[%s:%d] suc.", login_server_ip.c_str(), (int)login_server_port);

	// 将自己注册到登陆服务器
	this->RegisterToLogin();

	return true;
}

bool World::InitGameWorld(std::string config_dir, bool check_res_mode)
{
	std::string error;

	// 加载cross配置
	if (!CrossConfig::Instance().Init(config_dir + "cross.xml", &error))
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "CrossConfig::Init Return Fail! Error At:%s", error.c_str());
		return false;
	}

	// 加载 bigcross 配置
	if (!BigCrossServerConfig::Instance().Init(config_dir, &error))
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "BigCrossServerConfig::Init Return Fail! Error At:%s", error.c_str());
		return false;
	}

	// 初始化字符串资源
	if (!gamestring::StringInit((config_dir + "/serverconfig/string.xml").c_str(), &error))
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "gamestring::StringInit Return Fail! Error At:%s", error.c_str());
		return false;
	}

	// 初始化敏感词
	if (!NameFilter::Instance().Init((config_dir + "/serverconfig/namefilter.txt").c_str(), &error))
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "NameFilter Return Fail! Error At:%s", error.c_str());
		return false;
	}

	// 初始化名字库
	std::string configname = "role_name.xml";
	if (!NameConfig::Instance()->Init((config_dir + "/serverconfig/"), configname, &error))
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "NameConfig Return Fail! Error At:%s", error.c_str());
		return false;
	}

	//初始化AppearanceConfig
	configname = "image_change.xml";
	if (!AppearanceConfig::Instance().Init((config_dir + "/gameworld/logicconfig/"), configname, &error))
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "AppearanceConfig Return Fail! Error At:%s", error.c_str());
		return false;
	}

	if (!SHAREDCONFIG->Init(config_dir, &error))
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "SharedConfig::Init Return Fail! Error At:%s", error.c_str());
		return false;
	}

	if (!LocalConfig::Instance().Init(config_dir + "local.xml", &error))
	{
		gamelog::g_log_world.printf(LL_ERROR, error.c_str());
		gamelog::g_log_world.printf(LL_ERROR, "World::InitGameWorld, LocalConfig::Init return FALSE.");

		return false;
	}

	config_dir += "gameworld/";

	if (!InitItem(config_dir))	
	{
		return false;
	}

	if (!InitNPC(config_dir))		// 
	{
		return false;
	}

	if (!InitSkill(config_dir))
	{
		return false;
	}

	if (!InitDrop(config_dir))
	{
		return false;
	}

	if (!InitMonster(config_dir))
	{
		return false;
	}

	//读关键的配置
	if (!InitGlobalConfig(config_dir))			
	{
		return false;
	}
	
	if (!InitScene(config_dir))			// 需要检查【技能id、掉落物品id】 所以必须保证【技能池、物品池】已经初始完
	{
		return false;
	}

	if (!InitTask(config_dir))			// 需要检查【怪物、物品、NPC】 所以必须保证【怪物池、物品池、NPC池】已经初始完
	{
		return false;
	}

	if (!InitBattleCommonConfig(config_dir))
	{
		return false;
	}

	if (!InitTradeMarketConfig(config_dir))
	{
		return false;
	}

	if (!InitNoticeConfig(config_dir))
	{
		return false;
	}

	if (!InitLogicConfig(config_dir, check_res_mode))
	{
		return false;
	}
	//机器人配置读取用到了逻辑配置
	if (!InitRobotConfig(config_dir))
	{
		return false;
	}

	if (!InitGodEquipConfig(config_dir))
	{
		return false;
	}

	if (!InitUTAConfig(config_dir))
	{
		return false;
	}

	if (!InitHotfixConfig(config_dir))
	{
		return false;
	}

	std::string err;
	if (!CheckResourceCenter::Instance().Check(&err))
	{
		gamelog::g_log_world.printf(LL_ERROR, err.c_str());
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, CheckResourceCenter::Check return FALSE.");
		return false;
	}

	return true;
}
