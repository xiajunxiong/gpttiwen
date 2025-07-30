#include "global/mail/mailmanager.hpp"
#include "common/platform/system.h"
#include "common/platform/socket/pisocket.h"
#include "world.h"
#include "ilogmodule.h"
#include "inetworkmodule.h"
#include "utility/configpath.h"
#include "internalcomm.h"
#include "gamelog.h"
#include "global/colosseum/colosseummanager.hpp"
#include "servercommon/internalprotocal/lgprotocal.h"
#include "servercommon/internalprotocal/wgprotocal.h"
#include "servercommon/internalprotocal/gglobalprotocal.h"
#include "servercommon/serverconfig/serverconfigpool.h"
#include "common/memory/memmonitor.h"
#include "servercommon/performancecatch.hpp"
#include "internalcomm.h"
#include <assert.h>
#include "scene/asynreqregister/asynreqregister.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "global/rmibackobjdef.h"
#include "global/worldstatus/worldstatus.hpp"
#include "globalconfig/globalconfig.h"
#include "config/scenemanagerconfig.h"
#include "config/logicconfigmanager.hpp"
#include "global/rank/rankmanager.hpp"
#include "item/itempool.h"
#include "task/taskpool.h"
#include "npc/npcpool.h"
#include "monster/monsterpool.hpp"
#include "scene/delayloginmanager.hpp"
#include "battle/battle_manager_local.hpp"
#include "skill/skillpool.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "other/baby/babymanager.hpp"
#include "other/robot/robotconfig.hpp"
#include "global/guild/guildmanager.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/system/system.hpp"
#include "battle/battle_manager_local.hpp"
#include "global/wildbossmanager/wildbossmanager.h"
#include "drop/droppool.hpp"
#include "monster/monster_group_pool.hpp"
#include "global/trademarket/trademarketmanager.hpp"
#include "global/trademarket/trademarketconfig.hpp"
#include "config/otherconfig/sceneminemonsterconfig.hpp"
#include "obj/character/role.h"
#include "global/team/teammanager.hpp"
#include "other/marriage/marriagetask.hpp"
#include "global/cross/crossmanager.h"
#include "global/team/teammatch.hpp"
#include "other/event/eventhandler.hpp"
#include "config/otherconfig/noticeconfig.hpp"
#include "global/platformbattle/platformbattle.h"
#include "global/cross/crossusershadow.hpp"
#include "global/cross/crossuserregister.hpp"
#include "global/mail/mailmanager.hpp"
#include "global/team/teammatch.hpp"
#include "global/trademarket/trademarketrouter.hpp"
#include "global/server_first_kill/serverfirstkill.hpp"
#include "global/colosseum/colosseummanager.hpp"
#include "config/sharedconfig/sharedconfig.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/rolegatherfb/dropgather.hpp"
#include "other/monitor/systemmonitor.hpp"
#include "chat/chatmanager.h"
#include "battle/battle_video_manager.hpp"
#include "other/animalrace/animalracemanager.hpp"
#include "global/randactivity/randactivityimpl/raquanmindiscountmanager.hpp"
#include "global/shituseeking/shituseekingmanager.h"
#include "global/couragechallengerank/couragechallengerank.hpp"
#include "gameworld/other/itemtxt/itemtxt.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/hiddenrandactivityshadow.hpp"
#include "global/commonsave/commonsavemanager.hpp"
#include "global/gatherdreammanager/gatherdreamrecordmanager.hpp"
#include "chat/crosschatgroupmanager.h"
#include "global/shanhaibossmanager/shanhaibossmanager.hpp"
#include "global/cross/originbusinessregister.hpp"
#include "global/rmibackmanager/rmibackmanager.hpp"
#include "global/drawmanager/drawmanager.hpp"
#include "global/trademarket/ordermanager.hpp"
#include "chat/muteplayerrecorder.hpp"
#include "global/cloudarena/cloudarena.hpp"
#include "global/marketmanager/marketmanager.hpp"
#include "global/guildhonorbattlemanager/guildhonorbattlemanager.hpp"
#include "scene/teamkeyregister/teamkeyregister.hpp"
#include "other/robot/robotmanager.hpp"
#include "dbsessionreconnector.hpp"
#include "dbdisconnectrolesavecache.hpp"
#include <iterator>
#include "servercommon/internalcommmonitor.hpp"
#include "other/hongbao/hongbaomanager.hpp"
#include "serverlogic.h"
#include "other/bigdipper/bigdippermanager.hpp"
#include "global/tiandidaohen/tiandidaohen.h"
#include "global/givegift/givegift.h"
#include "global/tiandidaohen/tiandidaohen.h"
#include "global/rapaimaihangmanager/rapaimaihangmanager.hpp"
#include "global/platformbattle/platformbattle.h"
#include "other/gm/gm.hpp"
#include "friend/friendintimacy.hpp"
#include "global/jieyi/jieyizumanager.h"
#include "global/qingyuan/qingyuanmanager.h"
#include "global/cowreportgoodnews/cowreportgoodnews.hpp"
#include "friend/friendmanager.h"
#include "global/tigerbringsblessing/tigerbringsblessing.hpp"
#include "servercommon/redis/redismanager.hpp"
#include "servercommon/asynctask/asynctaskmanager.hpp"
#include "asynctaskimpl/asyncredistest.hpp"
#include "servercommon/asynctask/asynctaskfactorymanager.hpp"

#include "qingyuan/weddingmanger.hpp"
#include "global/worldarena/worldarena.hpp"
#include "global/worldteamarenamanager/worldteamarenarankmanager.hpp"
#include "global/worldteamarenamanager/worldteamarenasignupmanager.hpp"
#include "global/telnet/telnet.h"
#include "global/datasynccheck/datasynccheck.hpp"
#include "global/incomemanager/incomemanager.hpp"
#include "other/workshop/godequipupconfig.hpp"
#include "global/cycleactivity/whereisthefairymanager.hpp"
#include "global/bigcross/bigcrossmanager.hpp"
#include "servercommon/serverconfig/bigcrossconfig/utaconfig.hpp"

#include "other/msgcachemanager/msgcachemanager.hpp"
#include "global/zhouqimanager/zhouqimanager.h"
#include "other/cycleactivity/qingyuanduiduipeng/qingyuanduiduipengmanager.hpp"
#include "global/universe_team_arena/UTAShadow.hpp"
#include "global/bigcross/utaobshadow.hpp"
#include "timer/delayasyncconnectbigcrosstimer.hpp"
#include "global/team/teamrecruit.hpp"
#include "global/kuafuyingxionglumanager/kuafuyingxionglumanager.hpp"
#include "servercommon/serverconfig/battleconfig/battle_hallow_gift_pool.hpp"
#include "asynctaskimpl/asyncreloadmonster.hpp"
#include "servercommon/serverconfig/hotfixconfig.hpp"
#include "global/offlinebattledata/offlinebattledata.hpp"

#define CATCH_CONFIG_RUNNER
#include "unittest/catch.hpp"

static World *world_instance = NULL;

World::World(ILogModule *log,
	IConfigModule *config,
	IGameModule *game,
	INetworkModule *network,
	INetworkModule *internal_network,
	IClockModule *clock,
	IRMIModule *rmi_module,
	ITimerModule *timer,
	IFileWriteModule *filewrite,
	int total_server,
	int server_index)
	: m_network(NULL), m_game_time(0), m_listen_port(0), m_telnet_netid(-1), m_telnet_listen_port(0), m_total_server(total_server), m_server_index(server_index),
	m_is_on_server_start(false), m_last_update_dayid(0), m_last_reset_role_module_data_dayid(0),
	m_last_reset_role_module_timestamp(0), m_next_reset_role_module_timestamp(0),
	m_next_dataaccess_heartbeat_timestamp(0), m_dataaccess_heartbeat_try_times(0),
	m_hidden_next_send_heartbeat_to_game_timestamp(0)
{
	m_network = network;

	world_instance = this;

	m_buffer = new char[MAX_MSG_SIZE];	// 4M, 不可能有比4M还大的一个网络包，一定够用了
	memset(m_buffer, 0, MAX_MSG_SIZE);

	m_cross_comm_monitor = new InternalCommMonitor();
	m_login_comm_monitor = new InternalCommMonitor();
	m_battle_comm_monitor = new InternalCommMonitor();

	m_cross_comm_monitor->SetMsgTypeRange(crossgameprotocal::MT_CROSS_GAME_BEGIN, crossgameprotocal::MT_GC_PROTOCOL_MAX);
	m_login_comm_monitor->SetMsgTypeRange(lgprotocal::MT_LG_PROTOCOL_BEGIN, lgprotocal::MT_LG_PROTOCOL_END);
	m_battle_comm_monitor->SetMsgTypeRange(battlegameprotocol::MT_BATTLE_GAME_PROTOCOL_BEGIN, battlegameprotocol::MT_BATTLE_GAME_MAX);
}

World::~World()
{
	delete[]m_buffer;

	delete m_cross_comm_monitor;
	delete m_login_comm_monitor;
	delete m_battle_comm_monitor;
}

World * World::GetInstWorld()
{
	return world_instance;
}

bool World::Init()
{
	gamelog::g_log_world.print(LL_MAINTANCE, "Game Global Init.");

	gamelog::g_log_world.print(LL_MAINTANCE, "World Init.");

	return true;
}

class MsgMoniterTimer : public ITimerCallback
{
	int interval_s;

public:
	MsgMoniterTimer(int i) :interval_s(i) {}

	virtual void OnTimer()
	{
		static char barrier1[1024 * 10];
		static char buff[MAX_UINT16];
		static char barrier2[1024 * 10];
		memset(buff, 0, sizeof(buff));

		// 这里传个buff进去，但又不传length，容易出现buff长度不够，所以要监测好
		int t = memmonitor::PrintDyMemInfo(buff);

		assert(0 == buff[sizeof(buff) - 1]);
		assert(0 == buff[sizeof(buff) / 2]); // 检查位置为一半，比较稳妥，当内网出现一半就挂掉，外网可能已经出现溢出的问题

		if (0 != buff[sizeof(buff) - 1])
		{
			gamelog::g_log_world.printf(LL_CRITICAL, "MsgMoniterTimer buff overflow!!!");
			throw std::overflow_error("MsgMoniterTimer buff overflow");
		}

		gamelog::g_log_msgmoniter.print(LL_INFO, buff);

		EngineAdapter::Instance().CreateTimerSecond(interval_s, new MsgMoniterTimer(interval_s));
	}

	virtual void Free() { delete this; }
};

bool World::Start(bool check_res_mode, bool unittest_mode, int total_server, int server_index)
{
	m_local_ip = "127.0.0.1";
	m_listen_port = 7777;
	m_telnet_listen_port = 7778;

	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "World" / "GameServer" / "Index", &m_server_index))
	{
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, GameServer Index read failed!");
		return false;
	}

	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "World" / "GameServer" / "LocalIP", &m_local_ip))
	{
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, GameServer LocalIP read failed!");
		return false;
	}

	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "World" / "GameServer" / "ListenPort", &m_listen_port))
	{
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, GameServer ListenPort read failed!");
		return false;
	}

	//if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "World" / "GameServer" / "TelnetListenPort", &m_telnet_listen_port))
	//{
	//	gamelog::g_log_world.printf(LL_ERROR, "World On INIT, GameServer TelNetListenPort read failed!");
	//	return false;
	//}

	m_total_server = total_server; m_server_index = server_index;
	m_listen_port += server_index; // 监听端口递增
	EngineAdapter::Instance().m_server_index = m_server_index; // 使用命令行参数的

	// 获取配置目录
	std::string config_dir = "../config";
	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "ConfigDir", &config_dir))
	{
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, GameServer ConfigDir read failed!");
		return false;
	}
	if (config_dir[config_dir.length() - 1] != '/')
	{
		config_dir += '/';
	}

	// 加载所有服务器配置
	char err_info[1024] = { 0 };
	if (!ServerConfigPool::Instance().Init(config_dir, err_info))
	{
		gamelog::g_log_world.printf(LL_ERROR, err_info);
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, ServerConfigPool::Init return FALSE.");
		return false;
	}
	gamelog::g_log_world.printf(LL_INFO, "World On INIT, ServerConfigPool::Init Success.");

	InternalComm::Instance().SetSesstionKeyBase(ServerConfigPool::Instance().common_config.sessionkey);

	// 读配置 初始化场景
	if (!InitGameWorld(config_dir, check_res_mode))
	{
		return false;
	}

	// 资源检查模式  
	if (check_res_mode)
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "[Resource Check Success.]");
		return true;
	}

	// 单元测试模式
	if (unittest_mode)
	{
		return 0 == this->StartUnitTest();
	}

	// 角色缓存和好友关系缓存
	if (!LoadFromDBDirectly())
	{
		return false;
	}

	// 连接数据存取服务器
	if (!ConnectToDB())
	{
		return false;
	}

	// 连接登陆服务器
	if (!ConnectToLoginServer())
	{
		return false;
	}

	// 为gateway监听
	if (!ListenForGateway())
	{
		return false;
	}

	// 为Telnet监听
	/*if (!ListenForTelnet())
	{
		return false;
	}*/

	UniqueIDGenerator::Instance()->Init(GAMEWORLD, LocalConfig::Instance().GetMergeServerId(), m_server_index, EngineAdapter::Instance().GetClockModule());

	gamelog::g_log_world.printf(LL_MAINTANCE, "World[total_server:%d server_index:%d] Start.", m_total_server, m_server_index);

	static const int MSG_MONITER_INTERVAL_S = 60;
	EngineAdapter::Instance().CreateTimerSecond(MSG_MONITER_INTERVAL_S, new MsgMoniterTimer(MSG_MONITER_INTERVAL_S));

	// 开启消息缓存
	if (GlobalConfig::Instance()->IsOpenMsgCache())
	{
		MsgCaCheManager::Instance().StartThread();
	}

	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "World" / "GameServer" / "LocalIP", &m_local_ip))
	{
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, GameServer LocalIP read failed!");
		return false;
	}

	if (!this->InitAsyncTaskModule())
	{
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, InitAsyncTaskModule failed!");
		return false;
	}

	std::string redis_ip;
	int redis_port = 0;
	std::string redis_auth;
	int redis_connect_timeout_ms = 0;
	bool is_open = true;
	if (!this->InitRedisConfig(is_open, redis_ip, redis_port, redis_auth, redis_connect_timeout_ms))
	{
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, Redis Config Init Failed!");
		is_open = false;
	}

	std::string redis_key_prefix = STRING_SPRINTF("%s_s%d#", LocalConfig::Instance().GetPlatName(), LocalConfig::Instance().GetMergeServerId());
	RedisManager::Instance().Init(is_open, redis_key_prefix, redis_ip, redis_port, redis_auth, redis_connect_timeout_ms);
	if (is_open)
	{
		bool connect_succ = RedisManager::Instance().TryConnectOnServerStart();
		if (!connect_succ)
		{
			gamelog::g_log_world.printf(LL_ERROR, "World On INIT, Redis Connect Failed! ip[%s] port[%d]", redis_ip.c_str(), redis_port);
			return false;
		}
		else
		{
			gamelog::g_log_world.printf(LL_INFO, "World On INIT, Redis Connect Success! ip[%s] port[%d]", redis_ip.c_str(), redis_port);
		}
	}
	else
	{
		gamelog::g_log_world.printf(LL_INFO, "World On INIT, Redis Not Open");
	}

	return true;
}

void World::Update()
{
	//unsigned long beg_time = PITime();

	if (!m_is_on_server_start)
	{
		CommonSaveManager::Instance().RegisterLoadSuccCallback(new JieYiZuManagerLoadSuccCallback());
		CommonSaveManager::Instance().RegisterLoadSuccCallback(new QingYuanManagerLoadSuccCallback());
		m_is_on_server_start = true;
		this->OnServerStart();
		return;
	}

	unsigned long now = EngineAdapter::Instance().GetGameTime();
	unsigned long interval = now - m_game_time;
	time_t now_time = EngineAdapter::Instance().Time();

	RMIBackManager::Instance().Update(interval, now_time);
	AsyncTaskManager::Instance().LogicThreadUpdate();

	if (!this->IsAllDataLoadFinish()) // 所有数据加载完了才准update
	{
		return;
	}

	if (0 == m_last_reset_role_module_data_dayid)
	{
		m_last_reset_role_module_data_dayid = WorldStatus::Instance().GetLastResetRoleModuleDataDayId();
		m_last_reset_role_module_timestamp = WorldStatus::Instance().GetLastResetRoleModuleDataTimestamp();
		this->RecalNextResetDataTimeStamp();
	}

	if (0 == m_last_update_dayid)
	{
		m_last_update_dayid = WorldStatus::Instance().GetLastUpdateDayId();

		this->OnAllDataLoadFinish();

		return;
	}



	this->UpdateGlobal(interval);
	m_scene_manager.Update(interval);

	DelayLoginManager::GetInstance().Update();

	PerformanceCatch::Instance().Update(interval);

	BattleManagerLocal::GetInstance().Update();

	WorldStatus::Instance().Update(interval, now_time);
	TradeMarketManager::Instance().Update(interval, now_time);
	RankManager::Instance().Update(interval, now_time);
	ColosseumManager::Instance().Update(interval, now_time);
	MarriageTask::Instance().Update(interval, now_time);
	DropGather::Instance().Update(now_time);
	ServerFirstKill::Instance().Update(now_time);
	ChatManager::Instance().Update(now_time);
	CrossChatGroupManager::Instance().Update(now_time);
	BattleVideoMgr::Instance().Update(interval, now_time);
	AnimalRaceManager::Instance().Update(now_time);
	CourageChallengeRankManager::Instance().Update((unsigned int)now_time);
	CommonSaveManager::Instance().Update(interval, now_time);
	GatherDreamRecordManager::Instance().Update(now_time);
	OriginBusinessRegister::Instance().Update(interval, static_cast<unsigned int>(now_time));
	OrderManager::Instance().Update(static_cast<unsigned int>(now_time));
	MutePlayerRecorder::Instance().Update(static_cast<unsigned int>(now_time));
	BigDipperManager::Instance().Update(interval, now_time);
	RAPaiMaiHangManager::Instance().Update(now_time);
	FriendIntimacyManager::Instance().Update(interval, now_time);
	WeddingManger::Instance().Update(interval, now_time);
	BigCrossManager::Instance().Update(now_time);

	DrawManager::Instance().Update(interval, now_time);
	RobotManager::Instance().Update(now_time);
	DBSessionReconnector::Instance().Update(static_cast<unsigned int>(now_time));
	DBDisconnectRoleSaveCache::Instance().Update(static_cast<unsigned int>(now_time));
	DataSyncCheck::Instance().Update(interval, now_time);
	Gm::Instance().Update(interval, now_time);
	WorldArena::Instance().Update(static_cast<unsigned int>(now_time));
	WhereIsTheFairyManager::Instance().Update(now_time);
	UTAShadow::Instance().Update(static_cast<unsigned int>(now_time));
	UTAOBShadow::Instance().Update(static_cast<unsigned int>(now_time));
	TeamRecruit::Instance().Update(static_cast<unsigned int>(now_time));

	m_login_comm_monitor->Update(static_cast<unsigned int>(now_time), &gamelog::g_log_login_comm_monitor);
	m_battle_comm_monitor->Update(static_cast<unsigned int>(now_time), &gamelog::g_log_battle_comm_monitor);
	m_cross_comm_monitor->Update(static_cast<unsigned int>(now_time), &gamelog::g_log_cross_comm_monitor);

	m_game_time = now;

	if (0 == m_next_dataaccess_heartbeat_timestamp || now_time > m_next_dataaccess_heartbeat_timestamp) // 10秒发一次心跳
	{
		++m_dataaccess_heartbeat_try_times;
		m_next_dataaccess_heartbeat_timestamp = static_cast<unsigned int>(now_time) + 15;

		RMIHeartbeatCheckBackImpl *callback_obj = new RMIHeartbeatCheckBackImpl();

		RMIGlobalClient sc;
		sc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
		sc.HeartbeatCheckAsyn(callback_obj);
	}

	if (m_dataaccess_heartbeat_try_times > 60) // 15分钟没有响应 应该断掉 (服务器备份可能会出现dataaccess很久不响应心跳或心跳超时）
	{
		assert(0);
	}

	if (CrossConfig::Instance().IsHiddenServer() && static_cast<unsigned int>(now_time) > m_hidden_next_send_heartbeat_to_game_timestamp)
	{
		m_hidden_next_send_heartbeat_to_game_timestamp = static_cast<unsigned int>(now_time + 3); // 每3秒发一次
		this->HiddenSendHeartBeatToGame();
	}

	LogicConfigManager::CheckLogicConfigAsyncReload();

	/*if (m_game_time -  m_begin_time> 1000)
	{
		gamelog::g_log_world.printf(LL_DEBUG, "total:%d pc:%d(%d) ec:%d(%d) lc:%d(%d)", gamestat::g_game_stat.user_packet_count,
			gamestat::g_game_stat.sync_pos_count, gamestat::g_game_stat.sync_pos_packege_count,
			gamestat::g_game_stat.enter_count, gamestat::g_game_stat.enter_packege_count,
			gamestat::g_game_stat.leave_count, gamestat::g_game_stat.leave_packege_count);
		m_begin_time = m_game_time;
	}*/

	//gamelog::g_log_world.printf(LL_DEBUG, "World Update cast %d ms.", PITime() - beg_time);
}

void World::UpdateGlobal(unsigned long intervals)
{
	time_t now_second = EngineAdapter::Instance().Time();

	CrossManager::Instance().Update(intervals, now_second);
	OfflineRegisterManager::Instance().Update(intervals, now_second);
	ZhouQiManager::Instance().Update(intervals, now_second);
	BabyManager::Instance().Update(intervals, now_second);
	GuildManager::Instance().Update(intervals, now_second);
	UserMailManager::Instance().Update(intervals, now_second);
	ActivityManager::Instance().Update(intervals, now_second);
	GuildHonorBattleManager::Instance().Update(intervals, now_second);
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL != ramgr)
	{
		ramgr->Update(intervals, now_second);
	}
	HiddenRandActivityShadow::Instance()->Update(intervals, now_second);
	System::Instance().Update(intervals, now_second);
	WildBossManager::Instance().Update(intervals, now_second);
	TeamManager::Instance().Update(intervals, now_second);
	TeamMatch::Instance().Update(intervals, (unsigned int)now_second);
	PlatformBattle::Instance().Update(intervals, now_second);
	SystemMonitor::Instance().Update(intervals, static_cast<unsigned int>(now_second));
	ShiTuSeekingManager::Instance().Update(now_second);
	RAQuanMinDiscountManager::Instance().Update(intervals, now_second);
	ShanHaiBossManager::Instance().Update(intervals, now_second);
	TeamKeyRegister::Instance().Update(intervals, now_second);
	CloudArena::Instance().Update(intervals, now_second);
	MarketManager::Instance().Update(intervals, now_second);
	HongBaoManager::Instance().Update(intervals, now_second);
	TianDiDaoHen::Instance().Update(intervals, now_second);
	CowReportGoodNewsManager::Instance().Update(intervals, now_second);
	JieYiZuManager::Instance().Update(intervals, now_second);
	FriendManager::Instance().Update(intervals, now_second);
	TigerBringsBlessingManager::Instance().Update(intervals, now_second);
	QingYuanManager::Instance().Update(intervals, now_second);
	WorldTeamArenaSignUpManager::Instance().Update((unsigned int)now_second);
	WorldTeamArenaRankManager::Instance().Update((unsigned int)now_second);
	OfflineBattleDataMgr::Instance().Update((unsigned int)now_second);
	KuaFuYingXiongLuManager::Instance().Update(intervals, (unsigned int)now_second);

	unsigned int now = static_cast<unsigned int>(now_second);
	if (now >= WorldStatus::Instance().GetCommonData().next_month_timestamp)
	{
		WorldStatus::Instance().GetCommonData().next_month_timestamp = now + EngineAdapter::Instance().NextMouthInterval(1, 0, 0, 0);
		this->OnMonthChange();
	}

	if (WorldStatus::Instance().GetCommonData().two_week_refresh_flag == 0)
	{
		WorldStatus::Instance().GetCommonData().two_week_refresh_flag = 1;
		this->OnTwoWeekChange();
	}

	if (now >= WorldStatus::Instance().GetCommonData().next_week_timestamp)
	{
		WorldStatus::Instance().GetCommonData().next_week_timestamp = now + EngineAdapter::Instance().NextWeekInterval(1, 0, 0, 0);
		this->OnWeekChange();
		WorldStatus::Instance().GetCommonData().week_refresh_flag += 1;
		if (WorldStatus::Instance().GetCommonData().week_refresh_flag > 1)
		{
			this->OnTwoWeekChange();
			WorldStatus::Instance().GetCommonData().week_refresh_flag = 0;
		}
	}

	unsigned int today_id = EngineAdapter::Instance().DayID();
	if (today_id != m_last_update_dayid)
	{
		this->OnDayChange(m_last_update_dayid, today_id);

		m_last_update_dayid = today_id;
		WorldStatus::Instance().SetLastUpdateDayId(m_last_update_dayid);
	}

	if (now >= WorldStatus::Instance().GetCommonData().next_month_6_OClock)
	{
		WorldStatus::Instance().GetCommonData().next_month_6_OClock = now + EngineAdapter::Instance().NextMouthInterval(1, 6, 0, 0);
		this->OnMonthResetGlobalModuleData();
	}
	if (now >= WorldStatus::Instance().GetCommonData().next_week_6_OClock)
	{
		WorldStatus::Instance().GetCommonData().next_week_6_OClock = now + EngineAdapter::Instance().NextWeekInterval(1, 6, 0, 0);
		this->OnWeekResetGlobalModuleData();
	}

	if (m_next_reset_role_module_timestamp > 0 && now_second >= m_next_reset_role_module_timestamp)
	{
		this->OnResetGlobalModuleData(m_last_reset_role_module_data_dayid, today_id);

		m_last_reset_role_module_data_dayid = today_id;
		m_last_reset_role_module_timestamp = now_second;

		WorldStatus::Instance().SetLastResetRoleModuleDataDayId(m_last_reset_role_module_data_dayid);
		WorldStatus::Instance().SetLastResetRoleModuleDataTimestamp(static_cast<unsigned int>(m_last_reset_role_module_timestamp));

		this->RecalNextResetDataTimeStamp();
	}


}

bool World::InitGlobalConfig(const std::string &config_dir)
{
	// 加载全局配置
	std::string err;
	std::string config_name = "globalconfigmanager.xml";

	if (!GLOBALCONFIG->Init(config_dir, config_name, &err))
	{
		gamelog::g_log_world.printf(LL_ERROR, err.c_str());
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, GlobalConfig::Init return FALSE.");
		return false;
	}
	gamelog::g_log_world.printf(LL_INFO, "World On INIT, GlobalConfig::Init Success.");

	return true;
}

bool World::InitScene(const std::string &config_dir)
{
	std::string err;
	if (!SceneMineMonsterConfig::Instance().Init(config_dir + "hide_monster.xml", &err))
	{
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, SceneMineMonsterConfig Init Config Error:\n%s", err.c_str());
		return false;
	}

	// 加载场景和地图配置
	std::string scenemanager_config_name = "scenemanager.xml";

	SceneManagerConfig scenemanagerconfig;
	if (!scenemanagerconfig.Load(config_dir, scenemanager_config_name, m_total_server, m_server_index))
	{
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, Load Scene and Map Config Error:\n%s", scenemanagerconfig.GetError().c_str());
		return false;
	}

	gamelog::g_log_world.printf(LL_INFO, "World On INIT, Scene and Map Config Load Success.");

	bool ret = m_scene_manager.Init(scenemanagerconfig);

	return ret;
}

bool World::InitLogicConfig(const std::string &config_dir, bool check_res_mode)
{
	std::string err_info;

	if (!LogicConfigManager::Instance()->Init(config_dir, &err_info, check_res_mode))
	{
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, LogicConfig::Init return FALSE.");
		return false;
	}
	gamelog::g_log_world.printf(LL_INFO, "World On INIT, LogicConfig::Init Success.");

	return true;
}

bool World::InitItem(const std::string &config_dir)
{
	std::string err_info;
	std::string config_name;

	// 加载物品配置
	config_name = "itemmanager.xml";

	if (!ITEMPOOL->Init(config_dir, config_name, &err_info))
	{
		gamelog::g_log_world.printf(LL_ERROR, err_info.c_str());
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, ItemPool::Init return FALSE.");
		return false;
	}

	gamelog::g_log_world.printf(LL_INFO, "World On INIT, ItemPool::Init Success.");
	return true;
}

bool World::InitNPC(const std::string & config_dir)
{
	std::string err_info;

	// 加载 Npc 配置
	if (!NPCPOOL->Init(config_dir, &err_info))
	{
		gamelog::g_log_world.printf(LL_ERROR, err_info.c_str());
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, NpcPool::Init return FALSE.");
		return false;
	}

	gamelog::g_log_world.printf(LL_INFO, "World On INIT, NpcPool::Init Success.");
	return true;
}

bool World::InitTask(const std::string & config_dir)
{
	std::string err_info;

	// 加载任务配置
	if (!TASKPOOL->Init(config_dir, &err_info))
	{
		gamelog::g_log_world.printf(LL_ERROR, err_info.c_str());
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, TaskPool::Init return FALSE.");
		return false;
	}

	gamelog::g_log_world.printf(LL_INFO, "World On INIT, TaskPool::Init Success.");
	return true;
}

bool World::InitSkill(const std::string &config_dir)
{
	std::string err_info;
	if (!SkillPool::Instance().Init(config_dir + "skill/", &err_info))
	{
		gamelog::g_log_world.printf(LL_INFO, "World::InitSkill ERROR with error[%s]", err_info.c_str());
		return false;
	}

	return true;
}

bool World::InitMonster(const std::string &config_dir)
{
	std::string err_info;
	if (!MonsterPool::GetInstance().Init(config_dir, "battlemonstermanager.xml", &err_info))
	{
		gamelog::g_log_world.printf(LL_INFO, "World::InitMonster ERROR with error[%s]", err_info.c_str());
		return false;
	}

	if (!MonsterGroupPool::GetInstance().Init(config_dir + "monster_group.xml", &err_info))
	{
		gamelog::g_log_world.printf(LL_INFO, "World::InitMonster ERROR with error[%s]", err_info.c_str());
		return false;
	}

	return true;
}

bool World::InitBattleCommonConfig(const std::string &config_dir)
{
	std::string err_info;
	if (!BattleSpecialEffectPool::GetInstance().Init(config_dir + "logicconfig/special_effect.xml", &err_info))
	{
		gamelog::g_log_world.printf(LL_INFO, "World::InitBattleCommonConfig ERROR with error[%s]", err_info.c_str());
		return false;
	}

	if (!BattleHallowGiftPool::Instance().Init(config_dir + "logicconfig/", &err_info))
	{
		gamelog::g_log_world.printf(LL_INFO, "World::InitBattleCommonConfig ERROR with error[%s]", err_info.c_str());
		return false;
	}
	return true;
}

bool World::InitDrop(const std::string &config_dir)
{
	std::string err_info;
	std::string config_name;

	// 加载掉落配置
	config_name = "dropmanager.xml";

	if (!DROPPOOL->Init(config_dir, config_name, &err_info))
	{
		gamelog::g_log_world.printf(LL_ERROR, err_info.c_str());
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, DropPool::Init return FALSE.");
		return false;
	}
	gamelog::g_log_world.printf(LL_INFO, "World On INIT, DropPool::Init Success.");

	return true;
}

bool World::InitTradeMarketConfig(const std::string& config_dir)
{
	std::string err_info;
	std::string config_name;

	// 加载交易行配置
	config_name = "trade_market.xml";

	if (!TRADEMARKET_CFG.Init(config_dir + config_name, &err_info))
	{
		gamelog::g_log_world.printf(LL_ERROR, err_info.c_str());
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, TradeMarketConfig::Init return FALSE.");
		return false;
	}
	gamelog::g_log_world.printf(LL_INFO, "World On INIT, TradeMarketConfig::Init Success.");

	return true;
}

bool World::InitNoticeConfig(const std::string& config_dir)
{
	std::string err_info;
	std::string config_name;

	config_name = "logicconfig/notice_cfg.xml";

	if (!NoticeConfig::Instance()->Init(config_dir + config_name, &err_info))
	{
		gamelog::g_log_world.printf(LL_ERROR, err_info.c_str());
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, NoticeConfig::Init return FALSE.");
		return false;
	}
	gamelog::g_log_world.printf(LL_INFO, "World On INIT, NoticeConfig::Init Success.");

	return true;
}

bool World::InitRobotConfig(const std::string & config_dir)
{
	std::string err_info;
	std::string config_name;

	config_name = "logicconfig/robot.xml";

	if (!ROBOTCONFIG->Init(config_dir + config_name, &err_info))
	{
		gamelog::g_log_world.printf(LL_ERROR, err_info.c_str());
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, ROBOTCONFIG->Init return FALSE.");
		return false;
	}
	gamelog::g_log_world.printf(LL_INFO, "World On INIT, ROBOTCONFIG->Init Success.");

	return true;
}

bool World::InitGodEquipConfig(const std::string & config_dir)
{
	std::string err_info;
	std::string config_name;

	config_name = "logicconfig/godequip_up.xml";

	if (!GODEQUIP_CONFIG->Init(config_dir + config_name, &err_info))
	{
		gamelog::g_log_world.printf(LL_ERROR, err_info.c_str());
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, GODEQUIP_CONFIG->Init return FALSE.");
		return false;
	}
	gamelog::g_log_world.printf(LL_INFO, "World On INIT, GODEQUIP_CONFIG->Init Success.");

	return true;
}

bool World::InitUTAConfig(const std::string& config_dir)
{
	std::string err_info;
	std::string config_name;

	config_name = "logicconfig/universe_arena.xml";

	if (!UTAConfig::Instance().Init(config_dir + config_name, &err_info))
	{
		gamelog::g_log_world.printf(LL_ERROR, err_info.c_str());
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, UTAConfig::Init return FALSE.");
		return false;
	}
	gamelog::g_log_world.printf(LL_INFO, "World On INIT, UTAConfig::Init Success.");

	return true;
}

bool World::InitHotfixConfig(const std::string& config_dir)
{
	std::string err_info;
	std::string config_name;

	config_name = "globalconfig/hotfixfile.xml";

	if (!HotfixConfig::Instance().Init(config_dir + config_name, &err_info))
	{
		gamelog::g_log_world.printf(LL_ERROR, err_info.c_str());
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, HotfixConfig::Init return FALSE.");
		return false;
	}
	gamelog::g_log_world.printf(LL_INFO, "World On INIT, HotfixConfig::Init Success.");

	return true;
}

int World::StartUnitTest()
{
	Catch::Session session;
	Catch::ConfigData config_data;
	//config_data.listTests = true;
	config_data.useColour = Catch::UseColour::Yes;
	config_data.showDurations = Catch::ShowDurations::Always;
	config_data.filenamesAsTags = true;
	config_data.reporterNames.push_back("junit");
	config_data.outputFilename = "unittest_game.xml";
	//config_data.waitForKeypress = Catch::WaitForKeypress::BeforeExit;

	session.useConfigData(config_data);

	int result = session.run();

	return result;
}

void World::Stop(bool check_res_mode)
{
	if (check_res_mode)							// 资源检查模式  
	{
		return;
	}

	m_scene_manager.LogoutAllRole();

	this->OnServerStop();

	gamelog::g_log_world.print(LL_MAINTANCE, "World Stop.");
}

void World::Release()
{
	m_scene_manager.Release();
	gamelog::g_log_world.print(LL_MAINTANCE, "World Release.");
}

void World::OnAccept(Port listen_port, NetID netid, IP ip, Port port)
{
	GateWayInfo gw_info;
	gw_info.netid = netid;
	gw_info.gateway_index = -1;

	m_gateway_map.insert(GateWayMap::value_type(netid, gw_info));
}

void World::OnRecv(NetID netid, const char *data, int length)
{
	wgprotocal::MessageHeader *header = (wgprotocal::MessageHeader*)data;

	switch (header->msg_type)
	{
	case wgprotocal::MT_NET_RECV_MSG:
		OnRecvMsg(netid, data, length);
		break;
	case wgprotocal::MT_NET_DISCONNECT_NOTICE:
		OnNetDisconnect(netid, data);
		break;
	case wgprotocal::MT_GATE_REGISTER_TO_GAME:
		OnGateRegisterToGame(netid, data, length);
		break;

	default:
		break;
	}
}

void World::OnRecvMsg(NetID netid, const char *data, int length)
{
	wgprotocal::WGNetRecvMsg *nrm = (wgprotocal::WGNetRecvMsg*)data;
	GSNetID idtmp((unsigned short)(netid), (unsigned short)(nrm->header.netid));
	m_scene_manager.OnRecv(idtmp, nrm->header.user_ip, data + sizeof(wgprotocal::WGNetRecvMsg), length - sizeof(wgprotocal::WGNetRecvMsg));
}

void World::OnNetDisconnect(NetID netid, const char *data)
{
	wgprotocal::WGNetDisconnect *nd = (wgprotocal::WGNetDisconnect*)data;

	GSNetID idtmp((unsigned short)(netid), (unsigned short)(nd->header.netid));
	m_scene_manager.OnDisconnect(idtmp);

	DelayLoginManager::GetInstance().OnNetidDisconnect(idtmp);
	AsynReqRegister::Instance().OnNetidDisconnect(idtmp);
}

void World::OnGateRegisterToGame(NetID netid, const char *data, int length)
{
	wgprotocal::WGRegisterToGame *msg = (wgprotocal::WGRegisterToGame*)data;

	GateWayMap::iterator it = m_gateway_map.find(netid);
	if (it != m_gateway_map.end())
	{
		it->second.gateway_index = msg->gate_index;
		it->second.hostname_for_user = msg->hostname_for_user;
		it->second.port_for_user = msg->port_for_user;
	}
}

void World::OnDisconnect(NetID netid)
{
	m_scene_manager.OnGateWayDisconnect(netid);
	m_gateway_map.erase(netid);

	if (m_gateway_map.empty())	// 所有的gateway断开时候保存所有世界相关的信息 
	{
	}
}

void World::OnInternalAccept(Port listen_port, NetID netid, IP ip, Port port)
{
	if (m_telnet_listen_port == listen_port)
	{
		this->OnInternalAcceptByTelnet(listen_port, netid, ip, port);
	}
}

void World::OnInternalAcceptByTelnet(Port listen_port, NetID netid, IP ip, Port port)
{
	// 检查IP前缀是否合法
	sockaddr_in host_adr;
	std::string allow_ip_prefix = "127.0.0.1";
	memset(&host_adr, 0, sizeof(sockaddr_in));
	host_adr.sin_family = AF_INET;
	host_adr.sin_addr.s_addr = htonl(ip);
	std::string ip_str = inet_ntoa(host_adr.sin_addr);
	std::string ip_record = ip_str;
	ip_str.resize(allow_ip_prefix.size());
	if (ip_str != allow_ip_prefix)
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "Internal Network Accept NOT allow ip[%s].", ip_record.c_str());
		InternalComm::Instance().NetDisconnect(netid);
		return;
	}

	if (m_telnet_netid == (NetID)-1)
	{
		/*m_telnet_netid = netid;
		Telnet::Instance().OnTelnetAccept();
		gamelog::g_log_world.printf(LL_MAINTANCE, "World::OnAcceptByTelnet Telnet at netid[%d]", netid);*/

	}
	else
	{
		InternalComm::Instance().NetDisconnect(netid); //已连接telnet，则不接受新的telnet连接
		gamelog::g_log_world.printf(LL_MAINTANCE, "World::OnAcceptByTelnet Telnet , BUT..... Telnet Already Accecpted at[%d], so disconnect", m_telnet_netid);
	}
}

void World::OnInternalRecv(NetID netid, const char *data, int length)
{
	/*if (netid == m_telnet_netid)
	{
		// 来源于TelNet的消息
		Telnet::Instance().OnRecvFromTelnet(netid, data, length);
	}*/
	if (netid == InternalComm::Instance().m_login_server_netid)
	{
		// 登陆服务器来消息
		lgprotocal::MessageHeader *header = (lgprotocal::MessageHeader*)data;
		switch (header->msg_type)
		{
		case lgprotocal::MT_KICK_ROLE:
			{
				OnLGKickRole(data);
			}
			break;

		case lgprotocal::MT_LG_REGISTER_BATTLE:
			{
				OnLGRegisterBattle(data);
			}
			break;

		default:
			break;
		}

		m_login_comm_monitor->OnRecvMsg(header->msg_type);
		return;
	}
	else if (netid == InternalComm::Instance().m_cross_server_netid)
	{
		m_scene_manager.OnRecvMsgFromCross(data, length);

		crossgameprotocal::MessageHeader *header = (crossgameprotocal::MessageHeader*)data;
		m_cross_comm_monitor->OnRecvMsg(header->msg_type);
	}
	else if (netid == InternalComm::Instance().m_bigc_server_netid)
	{
		BigCrossManager::Instance().OnRecvMsg(netid, data, length);
	}
	else
	{
		BattleInfoNetidHash::iterator it = m_battle_info_netid_hash.find(netid);
		if (it != m_battle_info_netid_hash.end())
		{
			m_scene_manager.OnRecvMsgFromBattle(netid, data, length);

			battlegameprotocol::MessageHeader *header = (battlegameprotocol::MessageHeader*)data;
			m_battle_comm_monitor->OnRecvMsg(header->msg_type);
		}
	}
}

void World::OnInternalDisconnect(NetID netid)
{
	gamelog::g_log_world.printf(LL_MAINTANCE, "OnInternalDisconnect: netid[%d].", netid);

	BattleInfoNetidHash::iterator binh_it;
	DBSession* db_session = NULL;

	if (netid == InternalComm::Instance().m_bigc_server_netid)
	{
		InternalComm::Instance().m_bigc_server_netid = -1;
		gamelog::g_log_world.printf(LL_MAINTANCE, "OnInternalDisconnect big_cross netid[%d].", (int)netid);

		BigCrossManager::Instance().OnDisconnectFromBigCrossServer(true);

		//断线后立刻重连一次，后续交给BigCrossManager的心跳去检查
		BigCrossManager::Instance().ConnectToBigCrossServer();
		//DelayAsyncConnectBigcrossTimer* timer = new DelayAsyncConnectBigcrossTimer();
		//EngineAdapter::Instance().CreateTimer(RandomNum(3000, 5000), timer);
	}
	else if (netid == m_telnet_netid)
	{
		m_telnet_netid = -1;
		gamelog::g_log_world.printf(LL_MAINTANCE, "OnInternalDisconnect telnet netid[%d].", (int)netid);

	}
	else if (netid == InternalComm::Instance().m_login_server_netid)
	{
		InternalComm::Instance().m_login_server_netid = -1;

		// 启动重连机制
		ReConnectToLogin();
	}
	else if (netid == InternalComm::Instance().m_cross_server_netid)
	{
		InternalComm::Instance().m_cross_server_netid = -1;

		CrossManager::Instance().OnDisconnectFromCrossServer(true);
		CrossManager::Instance().CheckCrossConnection();

		lgprotocal::GLOnCrossDisconnect glcd;
		InternalComm::Instance().NetSend(InternalComm::Instance().m_login_server_netid, (const char*)&glcd, sizeof(glcd));

		gamelog::g_log_debug.printf(LL_MAINTANCE, "OnInternalDisconnect crossserver netid[%d].", (int)netid);
	}
	else if ((binh_it = m_battle_info_netid_hash.find(netid)) != m_battle_info_netid_hash.end())
	{
		BattleInfo* info = binh_it->second;

		gamelog::g_log_world.printf(LL_INFO, "World::OnInternalDisconnect battle index[%d] addr[%s:%d] disconnect",
			info->battle_server_index, info->battle_server_ip.c_str(), info->battle_server_port);

		info->battle_server_netid = -1;
		info->has_start_connect_to_battle = false;
		info->has_connected_to_battle = false;
		info->handle_battle_num = 0;

		m_battle_info_netid_hash.erase(binh_it);

		//先复原所有状态
		int battle_server_index = info->battle_server_index;
		BattleManagerLocal::GetInstance().OnBattleServerDisconnect(battle_server_index);

		EventHandler::Instance().OnBattleServerDisconnect(battle_server_index);

		// 重连BattleServer
		this->ReConnectToBattle(battle_server_index);
	}
	else if (NULL != (db_session = InternalComm::Instance().GetDBSession(netid)))
	{
		EngineAdapter::Instance().RMICloseSession(db_session->session);
		db_session->session.handle = 0;
		db_session->session.netid = -1;

		DBSessionReconnector::Instance().AddReconnectInfo(db_session->ip, db_session->port, &db_session->reconnect_handle);
	}
}

void World::OnDisconnectTelnet()
{	
	gamelog::g_log_world.printf(LL_MAINTANCE, "World::OnDisconnectTelnet netid:%d", m_telnet_netid);
	InternalComm::Instance().NetDisconnect(m_telnet_netid);
	m_telnet_netid = (NetID)-1;
}

void World::ReConnectToLogin()
{
	std::string login_server_ip = ServerConfigPool::Instance().common_config.ipconfigs.login_server_addr.inner_connect_addr.server_ip;
	Port login_server_port = ServerConfigPool::Instance().common_config.ipconfigs.login_server_addr.inner_connect_addr.for_gameserver.port;

	int handle = -1;
	bool ret = InternalComm::Instance().NetConnectAsyn(login_server_ip.c_str(), login_server_port, &handle, 1000);
	if (ret)
	{
		InternalComm::Instance().m_reconnet_login_handle = handle;
	}
	else
	{
		// log
	}
}

bool World::RegisterToLogin()
{
	lgprotocal::GLRegisterGS rgs;
	STRNCPY(rgs.ip, m_local_ip.c_str(), sizeof(rgs.ip));
	rgs.index = m_server_index;
	rgs.port = m_listen_port;
	rgs.scene_count = m_scene_manager.GetSceneList(rgs.scene_list, MAX_SCENE_LIST_NUM);

	int send_len = sizeof(rgs) - sizeof(rgs.scene_list) + rgs.scene_count * sizeof(rgs.scene_list[0]);
	return InternalComm::Instance().NetSend(InternalComm::Instance().m_login_server_netid, (const char *)&rgs, send_len);
}

void World::ReConnectToBattle(int index)
{
	this->ConnectToBattleServerAsyn(index);
}

void World::OnLGKickRole(const char *msg)
{
	lgprotocal::LGKickRole *lkr = (lgprotocal::LGKickRole*)msg;

	Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(lkr->role_id));
	if (NULL != role)
	{
		EngineAdapter::Instance().NetDisconnect(role->GetNetId(), role->GetUserId(), role->GetName(), 0, "Be Frobit form Login KickOut");
	}
	else if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameCrossKickCrossUser gckcu;
		gckcu.origin_plat_type = LocalConfig::Instance().GetPlatType();
		gckcu.origin_server_id = LocalConfig::Instance().GetMergeServerId();
		gckcu.origin_uid = lkr->role_id;
		InternalComm::Instance().SendToCross((const char *)&gckcu, sizeof(gckcu));
	}

	EventHandler::Instance().OnForbidPlayer(lkr->role_id, lkr->forbid_time_s);
}

void World::GetCrossDataSync(UniqueServerID _unique_sid)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::HiddenGameSyncCrossData hgscd;
	InternalComm::Instance().SendToGameThroughCross(_unique_sid.plat_type, _unique_sid.server_id, (const char *)&hgscd, sizeof(hgscd));
}

bool World::InitRedisConfig(ARG_OUT bool& is_open, ARG_OUT std::string& ip, ARG_OUT int& port, ARG_OUT std::string auth, ARG_OUT int connect_timeout_ms)
{
	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "Redis" / "IsOpen", &is_open))
	{
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, Redis Config [IsOpen] Read Failed!!");
		return false;
	}

	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "Redis" / "IP", &ip))
	{
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, Redis Config [Ip] Read Failed!!");
		return false;
	}

	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "Redis" / "PORT", &port))
	{
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, Redis Config [Port] Read Failed!!");
		return false;
	}

	EngineAdapter::Instance().ConfigGetValue(ROOT / "Redis" / "AUTH", &auth);

#ifndef _DEBUG
	if (auth.empty())
	{
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, Redis Auth Cannot Be Empty In Official Environment");
		return false;
	}
#endif

	if (!EngineAdapter::Instance().ConfigGetValue(ROOT / "Redis" / "CONNECT_TIMEOUT_MS", &connect_timeout_ms))
	{
		gamelog::g_log_world.printf(LL_ERROR, "World On INIT, Redis Config [Timeout_MS] Read Failed!!");
		return false;
	}

	return true;
}

bool World::InitAsyncTaskModule()
{
	AsyncTaskInitConfig atic;
	atic.max_task_num_in_queue = 100000;
	AsyncTaskManager::Instance().Init(atic);
	AsyncTaskManager::Instance().StartAsyncThread();

	return this->RegisterAsyncTaskFactory();
}

#define REGISTER_ASYNC_TASK_FACTORY(CLASS_NAME, FACTORY_NAME) \
	AsyncTaskFactoryManager::Instance().RegisterFactory((FACTORY_NAME), new (CLASS_NAME))

bool World::RegisterAsyncTaskFactory()
{
	return REGISTER_ASYNC_TASK_FACTORY(AsyncRedisTestFactory, "AsyncRedisTestFactory")
		&& REGISTER_ASYNC_TASK_FACTORY(AsyncReloadMonsterFactory, "AsyncReloadMonsterFactory");
}

void World::CheckHotFixConfig()
{
	std::string path = GetCurrentWorkingDirectory();
	std::string version_str;
	if (GetVersionStrFromPathStr(&version_str, path))
	{
		const HotfixConfig::HotfixCfgVec& hotfix_cfgs = HotfixConfig::Instance().GetGameWorldHotfixCfgs();
		HotfixConfig::Instance().DoHotfix(HOTFIX_GAMEWORLD_PATH, version_str, hotfix_cfgs);
	}
}

void World::ReleaseConfig()
{
#ifdef _DEBUG
	LogicConfigManager::ReleaseConfig();
#endif
}

World::BattleInfo* World::GetBattleServerInfo(int index)
{
	BattleInfoMap::iterator it = m_battle_info_map.find(index);
	if (it != m_battle_info_map.end())
	{
		return &it->second;
	}
	return NULL;
}

void World::OnLGRegisterBattle(const char* msg)
{
	lgprotocal::LGRegisterBattle* lgrb = (lgprotocal::LGRegisterBattle*)msg;

	lgrb->ip[sizeof(lgrb->ip) - 1] = '\0';

	bool need_connect = false;

	//###################################################################################
	BattleInfoMap::iterator it = m_battle_info_map.find(lgrb->index);
	if (it == m_battle_info_map.end())
	{
		static BattleInfo info;
		it = m_battle_info_map.insert(BattleInfoMap::value_type(lgrb->index, info)).first;
		need_connect = true;
	}
	else if (it->second.has_connected_to_battle)
	{
		if (it->second.battle_server_ip != lgrb->ip || it->second.battle_server_port != lgrb->port)
		{
			InternalComm::Instance().NetDisconnect(it->second.battle_server_netid);
			m_battle_info_netid_hash.erase(it->second.battle_server_netid);
			it->second.battle_server_netid = (NetID)-1;
			need_connect = true;
		}
	}
	else if (it->second.has_start_connect_to_battle)
	{
		if (it->second.battle_server_ip != lgrb->ip || it->second.battle_server_port != lgrb->port)
		{
			it->second.has_start_connect_to_battle = false;
			it->second.reconnect_handle = -1;
			need_connect = true;
		}
	}

	if (need_connect)
	{
		it->second.battle_server_index = lgrb->index;
		it->second.battle_server_ip = lgrb->ip;
		it->second.battle_server_port = lgrb->port;
		it->second.has_connected_to_battle = false;
		it->second.has_start_connect_to_battle = false;
		it->second.reconnect_handle = -1;
		it->second.battle_server_netid = (NetID)-1;

		this->ConnectToBattleServerAsyn(lgrb->index);
	}
}

bool World::HiddenConnectToGameDB(UniqueServerID server_id, const char* ip, Port port) // 这里假定本服用的都是同一个IP和端口
{
	rmi::Session accounter_session = EngineAdapter::Instance().RMICreateSession(ip, port);
	if (accounter_session.handle == 0)
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "HiddenConnectToGameDB Connect Accounter DBLayer[%s:%d] Fail!", ip, port);
		return false;
	}
	DBSession db_accounter_session(accounter_session, ip, port);

	unsigned int role_db_num = 1;
	if (role_db_num == 0 || role_db_num > (unsigned int)DBHandler::MAX_ROLE_DB_INDEX)
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "Role DBLayer NUM:%d is NOT valid!", role_db_num);
		return false;
	}

	DBSession role_db_session_list[DBHandler::MAX_ROLE_DB_INDEX];
	for (unsigned int i = 0; i < role_db_num; ++i)
	{
		rmi::Session role_db = EngineAdapter::Instance().RMICreateSession(ip, port);
		if (role_db.handle == 0)
		{
			gamelog::g_log_world.printf(LL_MAINTANCE, "Connect Role DBLayer[%s:%d] index[%d] Fail!", ip, port, i);
			return false;
		}

		role_db_session_list[i] = DBSession(role_db, ip, port);
	}

	// 只需要角色相关的session
	DBSession global_session;
	DBSession system_session;
	DBSession name_session;

	InternalComm::Instance().SetOriginGameworldDBSession(server_id, db_accounter_session, role_db_session_list, role_db_num, global_session, system_session, name_session);

	gamelog::g_log_world.printf(LL_MAINTANCE, "Hidden Connect OriginGameworld DataAccess suc. ServerId[%s,%d] addr[%s:%d]", LocalConfig::GetPlatNameBySpid(server_id.plat_type), server_id.server_id, ip, port);
	return true;
}

bool World::ConnectToBattleServerAsyn(int index)
{
	BattleInfo* info = this->GetBattleServerInfo(index);
	if (NULL == info) return false;

	if (-1 != info->reconnect_handle)
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "Already Connecting To BattleServer[%s:%d] index[%d], Please Wait", info->battle_server_ip.c_str(), (int)info->battle_server_port, index);
		return true; //already connecting
	}

	bool ret = InternalComm::Instance().NetConnectAsyn(info->battle_server_ip.c_str(), info->battle_server_port, &info->reconnect_handle);
	if (!ret)
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "Asyn Connect To BattleServer[%s:%d] index[%d] Fail!", info->battle_server_ip.c_str(), (int)info->battle_server_port, index);
		return false;
	}

	if (!info->has_start_connect_to_battle)
	{
		info->has_start_connect_to_battle = true;
		gamelog::g_log_world.printf(LL_MAINTANCE, "[DEBUG] START ConnectToBattleServerAsyn addr[%s:%d] index[%d] ...", info->battle_server_ip.c_str(), (int)info->battle_server_port, index);
	}

	return true;
}

bool World::SendToAllGateway(const char *msg, int length)
{
	bool ret = true;

	for (GateWayMap::iterator i = m_gateway_map.begin(); i != m_gateway_map.end(); ++i)
	{
		ret &= EngineAdapter::Instance().NetSendAll(i->second.netid, msg, length);
	}

	return ret;
}

bool World::SendSerializeToAllGateway(IMessageSerializer * msg)
{
	if (NULL == msg) return false;

	int max_length = MAX_MSG_SIZE;
	int serialized_length = 0;

	char * tmp_buffer = m_buffer;
	if (!msg->Serialize(tmp_buffer, max_length, &serialized_length))
	{
		delete[] tmp_buffer;
		return false;
	}

	return this->SendToAllGateway(tmp_buffer, serialized_length);
}

bool World::SendToALLGameAllGateway(const void * msg, int length)
{
	//跨服没开则发到本地
	if (!CrossManager::Instance().IsCrossServerConnected())
	{
		return this->SendToAllGateway((const char *)msg, length);
	}
	else //跨服开了则发到所有连接到跨服的游戏服
	{
		return InternalComm::Instance().SendToAllGameAllUserThroughCross((const char *)msg, length);
	}
}

bool World::SendToRoleAny(int role_id, const void * msg, int length)
{
	Role * role = this->GetSceneManager()->GetRole(role_id);
	if (role)
	{
		return role->GetRoleModuleManager()->NetSend(msg, length);
	}
	else
	{
		UniqueServerID origin_usid(LocalConfig::Instance().GetPlatType(), IntToUid(role_id).db_index);
		const UniqueServerID * usid = this->GetMotherServerId(origin_usid);
		if (usid == NULL)
		{
			return false;
		}

		if (LocalConfig::Instance().GetUniqueServerID() == *usid)
		{
			return true;
		}

		return InternalComm::Instance().SendToAllGameUserThroughCross(role_id, (const char *)msg, length);
	}

	return false;
}

void World::NoticeNumAny(int role_id, int error_num)
{
	Protocol::SCNoticeNum notice;
	notice.notice_num = error_num;

	this->SendToRoleAny(role_id, &notice, sizeof(notice));
}

bool World::SendToRole(int role_id, const void * msg, int length)
{
	Role * role = this->GetSceneManager()->GetRole(role_id);
	if (role)
	{
		return role->GetRoleModuleManager()->NetSend(msg, length);
	}
	else if (CrossConfig::Instance().IsHiddenServer())
	{
		return InternalComm::Instance().SendToGameUserThroughCross(role_id, msg, length);
	}
	else
	{
		return InternalComm::Instance().SendToHiddenUserThroughCross(role_id, msg, length);
	}

	return true;
}

void World::NoticeNum(int role_id, int error_num)
{
	Protocol::SCNoticeNum notice;
	notice.notice_num = error_num;

	this->SendToRole(role_id, &notice, sizeof(notice));
}

void World::OnServerStart()
{
	gamelog::g_log_world.printf(LL_INFO, "World::OnServerStart begin load data at %u %lu", (unsigned int)EngineAdapter::Instance().Time(), EngineAdapter::Instance().GetGameTime());
	
	this->CheckHotFixConfig();
	
	WorldStatus::Instance().OnServerStart();
	System::Instance().Init();
	OfflineRegisterManager::Instance().OnServerStart();
	BabyManager::Instance().OnServerStart();
	GuildManager::Instance().OnServerStart();
	UserMailManager::Instance().OnServerStart();
	TradeMarketManager::Instance().OnServerStart();
	RankManager::Instance().OnServerStart();
	ColosseumManager::Instance().OnServerStart();
	PlatformBattle::Instance().OnServerStart();
	ChatManager::Instance().OnServerStart();
	BattleVideoMgr::Instance().OnServerStart();
	ServerFirstKill::Instance().OnServerStart();	//需要放在录像初始化后面,否则在删除录像时找不到对应数据
	AnimalRaceManager::Instance().OnServerStart();
	ShiTuSeekingManager::Instance().OnServerStart();
	RAQuanMinDiscountManager::Instance().OnServerStart();
	CourageChallengeRankManager::Instance().OnServerStart();
	CommonSaveManager::Instance().OnServerStart();
	GatherDreamRecordManager::Instance().OnServerStart();
	CrossChatGroupManager::Instance().OnServerStart();
	CloudArena::Instance().OnServerStart();
	HongBaoManager::Instance().OnServerStart();
	RAPaiMaiHangManager::Instance().OnServerStart();
	FriendIntimacyManager::Instance().OnServerStart();
	CowReportGoodNewsManager::Instance().OnServerStart();
	FriendManager::Instance().OnServerStart();
	TigerBringsBlessingManager::Instance().OnServerStart();
	WeddingManger::Instance().OnServerStart();
	WorldArena::Instance().OnServerStart();
	MarketManager::Instance().OnServerStart();
	WorldTeamArenaSignUpManager::Instance().OnServerStart();
	WorldTeamArenaRankManager::Instance().OnServerStart();
	BigCrossManager::Instance().OnServerStart();
	ZhouQiManager::Instance().OnServerStart();
	OfflineBattleDataMgr::Instance().OnServerStart();
	
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL != ramgr)
	{
		ramgr->SetUniqueServerID(LocalConfig::Instance().GetUniqueServerID());
	}
}

void World::OnServerStop()
{
	if (!this->IsAllDataLoadFinish())
	{
		return;
	}

	WorldStatus::Instance().OnServerStop();
	OfflineRegisterManager::Instance().OnServerStop();
	BabyManager::Instance().OnServerStop();
	GuildManager::Instance().OnServerStop();
	UserMailManager::Instance().OnServerStop();
	TradeMarketManager::Instance().OnServerStop();
	ColosseumManager::Instance().OnServerStop();
	PlatformBattle::Instance().OnServerStop();
	ServerFirstKill::Instance().OnServerStop();
	ChatManager::Instance().OnServerStop();
	BattleVideoMgr::Instance().OnServerStop();
	AnimalRaceManager::Instance().OnServerStop();
	ShiTuSeekingManager::Instance().OnServerStop();
	RAQuanMinDiscountManager::Instance().OnServerStop();
	CourageChallengeRankManager::Instance().OnServerStop();
	CommonSaveManager::Instance().OnServerStop();
	GatherDreamRecordManager::Instance().OnServerStop();
	CrossChatGroupManager::Instance().OnServerStop();
	DrawManager::Instance().OnServerStop();
	CloudArena::Instance().OnServerStop();
	HongBaoManager::Instance().OnServerStop();
	RankManager::Instance().OnServerStop();
	RAPaiMaiHangManager::Instance().OnServerStop();
	FriendIntimacyManager::Instance().OnServerStop();
	CowReportGoodNewsManager::Instance().OnServerStop();
	FriendManager::Instance().OnServerStop();
	TigerBringsBlessingManager::Instance().OnServerStop();
	WeddingManger::Instance().OnServerStop();
	WorldArena::Instance().OnServerStop();
	MarketManager::Instance().OnServerStop();
	WorldTeamArenaSignUpManager::Instance().OnServerStop();
	WorldTeamArenaRankManager::Instance().OnServerStop();
	MsgCaCheManager::Instance().StopThread();
	BigCrossManager::Instance().OnServerStop();
	OfflineBattleDataMgr::Instance().OnServerStop();



	// 放最后
	this->ReleaseConfig();
}

bool World::IsAllDataLoadFinish()
{
	return WorldStatus::Instance().IsLoadFinish() &&
		OfflineRegisterManager::Instance().IsLoadFinish() &&
		BabyManager::Instance().IsLoadFinish() &&
		GuildManager::Instance().IsGuildLoadFinish() &&
		TradeMarketManager::Instance().IsLoadFinish() &&
		ColosseumManager::Instance().IsLoadFinish() &&
		PlatformBattle::Instance().IsLoadFinish() &&
		ServerFirstKill::Instance().IsLoadFinish() &&
		ChatManager::Instance().IsLoadFinish() &&
		BattleVideoMgr::Instance().IsLoadFinish() &&
		AnimalRaceManager::Instance().ISLoadFinish() &&
		ShiTuSeekingManager::Instance().IsLoadFinish() &&
		RAQuanMinDiscountManager::Instance().IsLoadFinish() &&
		CourageChallengeRankManager::Instance().IsLoadFinish() &&
		CommonSaveManager::Instance().IsLoadFinish() &&
		GatherDreamRecordManager::Instance().IsLoadFinish() &&
		HongBaoManager::Instance().IsLoadFinish() &&
		RankManager::Instance().IsLoadFinish() &&
		RAPaiMaiHangManager::Instance().IsLoadFinish() &&
		FriendIntimacyManager::Instance().IsLoadFinish() &&
		CowReportGoodNewsManager::Instance().IsLoadFinish() &&		// 需要排行榜加载完毕才能连接跨服
		FriendManager::Instance().IsLoadFinish() &&
		WorldArena::Instance().IsLoadAllFinish() &&
		MarketManager::Instance().IsLoadFinish() &&
		WorldTeamArenaSignUpManager::Instance().IsLoadFinish() &&
		WorldTeamArenaRankManager::Instance().IsLoadFinish() &&
		BigCrossManager::Instance().IsLoadFinish() &&
		OfflineBattleDataMgr::Instance().IsLoadFinish();
}

void World::OnAllDataLoadFinish()
{
	gamelog::g_log_world.printf(LL_INFO, "World::OnAllDataLoadFinish finish load data at %u %lu", (unsigned int)EngineAdapter::Instance().Time(), EngineAdapter::Instance().GetGameTime());

	CrossManager::Instance().CheckCrossConnection();
	WorldStatus::Instance().InitLoadFinish();
	ServerFirstKill::Instance().OnServerLoadDataFinish();
	MarketManager::Instance().OnServerLoadDataFinish();

	m_game_time = EngineAdapter::Instance().GetGameTime();
}

void World::OnMonthResetGlobalModuleData()
{

}

void World::OnWeekResetGlobalModuleData()
{

}

void World::OnResetGlobalModuleData(unsigned int old_dayid, unsigned int now_dayid)
{
	ColosseumManager::Instance().OnResetGlobalModuleData(old_dayid, now_dayid);
	WorldStatus::Instance().OnResetData(old_dayid, now_dayid);
	GuildManager::Instance().OnResetData(old_dayid, now_dayid);
}

void World::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	WorldStatus::Instance().OnDayChange(old_dayid, now_dayid);
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL != ramgr)
	{
		ramgr->OnDayChange(old_dayid, now_dayid);
	}
	HiddenRandActivityShadow::Instance()->OnDayChange(old_dayid, now_dayid);
	RankManager::Instance().OnDayChange(old_dayid, now_dayid);
	GuildManager::Instance().OnDayChange(old_dayid, now_dayid);
	ColosseumManager::Instance().OnDayChange(old_dayid, now_dayid);
	GatherDreamRecordManager::Instance().OnDayChange(old_dayid, now_dayid);
	ShanHaiBossManager::Instance().OnDayChange(old_dayid, now_dayid);
	ActivityManager::Instance().OnDayChange();
	ZhouQiManager::Instance().OnDayChange();
	CloudArena::Instance().OnDayChange();
	PlatformBattle::Instance().OnDayChange();
	GiveGift::Instance().OnDayChange();
	WorldArena::Instance().OnDayChange();
	MarketManager::Instance().OnDayChange(old_dayid, now_dayid);
	QingYuanManager::Instance().OnDayChange();
	IncomeManager::Instance().OnDayChange();
	QingYuanDuiDuiPengManager::Instance().OnDayChange();
	OfflineBattleDataMgr::Instance().OnDayChange();
}

void World::OnWeekChange()
{
	WorldStatus::Instance().OnWeekChange();
	RankManager::Instance().OnWeekChange();
	GuildManager::Instance().OnWeekChange();
	GuildHonorBattleManager::Instance().OnWeekChange();
	BigDipperManager::Instance().OnWeekChange();
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL != ramgr)
	{
		ramgr->OnWeekChange();
	}
	HiddenRandActivityShadow::Instance()->OnWeekChange();
	ActivityManager::Instance().OnWeekChange();
}

void World::OnTwoWeekChange()
{
	WorldStatus::Instance().OnTwoWeekChange();
}

void World::OnMonthChange()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL != ramgr)
	{
		ramgr->OnMonthChange();
	}
	ActivityManager::Instance().OnMonthChange();
	HiddenRandActivityShadow::Instance()->OnMonthChange();
}

void World::OnUserLogin(const UserID& user_id)
{
	/******************************************************************************
	* 注意：请传入user_id而非role实例，因为可能role会在某个调用中跳转了场景而失效 *
	******************************************************************************/
	TeamKeyRegister::Instance().UserLogin(user_id);
	TeamManager::Instance().UserLogin(user_id);
	UserMailManager::Instance().OnUserLogin(user_id);
	CrossUserShadow::Instance().OnUserLogin(user_id);
	CrossUserRegister::Instance().OnUserLogin(user_id);
	TeamMatch::Instance().OnUserLogin(user_id);
	TradeMarketRouter::Instance().OnUserLogin(user_id);
	TradeMarketManager::Instance().OnUserLogin(user_id);
	SystemMonitor::Instance().OnUserLogin(user_id);
	ShiTuSeekingManager::Instance().OnUserLogin(user_id);
	BattleManagerLocal::GetInstance().OnUserLogin(user_id);
	OriginBusinessRegister::Instance().OnUserLogin(user_id);
	CloudArena::Instance().OnUserLogin(user_id);
	WorldArena::Instance().OnUserLogin(user_id);
	UTAShadow::Instance().OnUserLogin(user_id);
	BigCrossManager::Instance().OnUserLogin(user_id);
	OfflineBattleDataMgr::Instance().OnLogin(user_id);
}

void World::OnDataaccessHeartbeat()
{
	m_dataaccess_heartbeat_try_times = 0;
	m_next_dataaccess_heartbeat_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 10;
}

bool World::ReloadScene(const std::string &config_dir, std::string *err)
{
	std::string scenemanager_config_name = "scenemanager.xml";

	SceneManagerConfig scenemanagerconfig;
	if (!scenemanagerconfig.Load(config_dir, scenemanager_config_name, m_total_server, m_server_index))
	{
		*err = scenemanagerconfig.GetError();
		return false;
	}

	bool ret = m_scene_manager.ReInit(scenemanagerconfig);

	return ret;
}

int World::GetGateWayIndexByNetId(NetID netid)
{
	GateWayMap::iterator it = m_gateway_map.find(netid);
	if (it == m_gateway_map.end())
	{
		return -1;
	}

	return it->second.gateway_index;
}

NetID World::GetGateWayNetIdByIndex(int index)
{
	GateWayMap::iterator it = m_gateway_map.begin();
	for (; it != m_gateway_map.end(); ++it)
	{
		if (index == it->second.gateway_index)
		{
			return it->second.netid;
		}
	}

	return (NetID)-1;
}

const World::BattleInfo* World::GetBattleServerInfoConst(int index)
{
	return this->GetBattleServerInfo(index);
}

World::BattleInfo* World::GetBattleServerInfoByReconnectHandle(int handle)
{
	BattleInfoMap::iterator it = m_battle_info_map.begin();
	for (; it != m_battle_info_map.end(); ++it)
	{
		if (handle == it->second.reconnect_handle)
		{
			return &it->second;
		}
	}

	return NULL;
}

void World::OnBattleServerConnect(NetID netid, BattleInfo* info)
{
	if (NULL == info) return;

	m_battle_info_netid_hash[netid] = info;
}

bool World::SendMsgToAllBattle(const char* data, int data_len)
{
	BattleInfoMap::iterator it = m_battle_info_map.begin();
	for (; it != m_battle_info_map.end(); ++it)
	{
		if (it->second.battle_server_netid != (NetID)(-1))
		{
			InternalComm::Instance().NetSend(it->second.battle_server_netid, data, data_len);
		}
	}

	return true;
}

bool World::SendMsgToOneBattle(int index, const char* data, int data_len)
{
	BattleInfoMap::iterator it = m_battle_info_map.find(index);
	if (it != m_battle_info_map.end() && it->second.battle_server_netid != (NetID)(-1))
	{
		return InternalComm::Instance().NetSend(it->second.battle_server_netid, data, data_len);
	}

	return false;
}

bool World::SendSerializeMsgToOneBattle(int index, IMessageSerializer* serializer)
{
	BattleInfoMap::iterator it = m_battle_info_map.find(index);
	if (it != m_battle_info_map.end() && it->second.battle_server_netid != (NetID)(-1))
	{
		return InternalComm::Instance().NetSendSerializeMsg(it->second.battle_server_netid, serializer);
	}

	return false;
}

bool World::ChooseBattleIndex(int* out_index)
{
	// 根据所有战斗服实例现时负责的战斗数，选一个最小的来用
	int min_battle_num_index = -1;
	int min_battle_num = MAX_INT;
	BattleInfoMap::iterator it = m_battle_info_map.begin();
	for (; it != m_battle_info_map.end(); ++it)
	{
		if (!it->second.has_connected_to_battle) continue;

		if (it->second.handle_battle_num < min_battle_num)
		{
			min_battle_num = it->second.handle_battle_num;
			min_battle_num_index = it->first;
		}
	}

	if (-1 != min_battle_num_index)
	{
		if (NULL != out_index) *out_index = min_battle_num_index;

		gamelog::g_log_battle.printf(LL_INFO, "ChooseBattleIndex [%d]", min_battle_num_index);

		return true;
	}

	return false;
}

void World::ModifyBattleIndexHandleNum(int index, int modify_num)
{
	BattleInfoMap::iterator it = m_battle_info_map.find(index);
	if (it != m_battle_info_map.end())
	{
		it->second.handle_battle_num += modify_num;
		if (it->second.handle_battle_num < 0)
		{
			it->second.handle_battle_num = 0;
		}

		gamelog::g_log_battle.printf(LL_INFO, "ModifyBattleIndexHandleNum index[%d] modify_num[%d] cur_num[%d]", index, modify_num, it->second.handle_battle_num);
	}
}

bool World::HasBattleServerConnected()
{
	BattleInfoMap::iterator it = m_battle_info_map.begin();
	for (; it != m_battle_info_map.end(); ++it)
	{
		if (it->second.has_connected_to_battle)
		{
			return true;
		}
	}

	return false;
}

bool World::SendToTelnet(const char * data, int length)
{
	if (m_telnet_netid != (NetID)-1)
	{
		return InternalComm::Instance().NetSend(m_telnet_netid, data, length);
	}

	return false;
}

void World::OnCrossRegisterGameServer(UniqueServerID usid, const char* ip, Port port, int plat_type, int server_num, unsigned int open_server_timestamp, int* server_list, const char* gw_hostname, unsigned short gw_port)
{
	if (server_num <= 0) return;

	// 貌似不需要对已连接的session进行close
	//const DBHandler* db_handler = InternalComm::Instance().GetHiddenDBHandler(usid);
	//if (NULL != db_handler)
	//{
	//	
	//}

	this->HiddenConnectToGameDB(usid, ip, port);

	UniqueServerID mother_server_id(plat_type, server_list[0]);

	std::set<int> server_id_list;
	for (int i = 0; i < server_num; ++i)
	{
		server_id_list.insert(server_list[i]);

		UniqueServerID usid(plat_type, server_list[i]);
		MotherServerInfo info;
		info.mother_server_usid = mother_server_id;
		info.gw_hostname = gw_hostname;
		info.gw_port = gw_port;
		m_mother_server_hash[usid] = info;

		static ServerOpenTimeInfo soti;
		soti.server_real_start_time = open_server_timestamp;
		soti.server_start_zero_time = (unsigned int)(SHAREDCONFIG->GetOpenServerConfig().GetOpenServerZeroTime(&open_server_timestamp));
		soti.server_start_six_time = (unsigned int)(SHAREDCONFIG->GetOpenServerConfig().GetOpenServerSixTime(&open_server_timestamp));
		m_server_open_time_map[usid] = soti;
	}

	//Global System
	GuildManager::Instance().RequestSyncAllGuild(server_list[0], false);
	CrossChatGroupManager::Instance().OnGameServerInLineReturnCrossData(server_num, server_list);
	this->LoadOriginDBGlobalCache(usid);
	WorldStatus::Instance().OnGameServerConnect(server_id_list);
	ActivityManager::Instance().OnGameServerConnect(server_id_list);
	CloudArena::Instance().OnGameRegisterCross(mother_server_id);
	TianDiDaoHen::Instance().OnGameServerConnect(server_id_list);
	PlatformBattle::Instance().OnGameServerConnect(server_id_list);
	JieYiZuManager::Instance().OnGameServerConnect(server_id_list);
	QingYuanManager::Instance().OnGameServerConnect(server_id_list);
	WeddingManger::Instance().OnGameServerConnect(server_id_list);
	OfflineBattleDataMgr::Instance().OnGameServerConnect(mother_server_id, server_id_list);

	this->GetCrossDataSync(usid);
}

const UniqueServerID *World::GetMotherServerId(const UniqueServerID &kid_server_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		static UniqueServerID origin_usid(LocalConfig::Instance().GetPlatType(), LocalConfig::Instance().GetMergeServerId());
		return &origin_usid;
	}

	MotherServerHash::const_iterator c_it = m_mother_server_hash.find(kid_server_id);
	if (c_it == m_mother_server_hash.end())
	{
		return NULL;
	}

	return &c_it->second.mother_server_usid;
}

const World::MotherServerInfo* World::GetMotherServerInfo(const UniqueServerID& kid_server_id)
{
	MotherServerHash::const_iterator c_it = m_mother_server_hash.find(kid_server_id);
	if (c_it == m_mother_server_hash.end())
	{
		return NULL;
	}

	return &c_it->second;
}

bool World::RandOneMotherServerId(ARG_OUT int* mother_server_id)
{
	if (m_mother_server_hash.empty())
	{
		return false;
	}

	int r = rand() % m_mother_server_hash.size();
	MotherServerHash::iterator it = m_mother_server_hash.begin();
	std::advance(it, r);

	if (NULL != mother_server_id) *mother_server_id = it->second.mother_server_usid.server_id;

	return true;
}

unsigned int World::GetRealServerOpenTime(const UniqueServerID &usid)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return (unsigned int)SHAREDCONFIG->GetOpenServerConfig().GetServerRealStartTime();
	}

	ServerOpenTimeMap::const_iterator c_it = m_server_open_time_map.find(usid);
	if (c_it == m_server_open_time_map.end())
	{
		// 怕碰到计算开服至今时间的，0的话依然还是有效值
		return (unsigned int)-1;
	}

	return c_it->second.server_real_start_time;
}

unsigned int World::GetServerOpenZeroTime(const UniqueServerID &usid)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return (unsigned int)SHAREDCONFIG->GetOpenServerConfig().GetOpenServerZeroTime();
	}

	ServerOpenTimeMap::const_iterator c_it = m_server_open_time_map.find(usid);
	if (c_it == m_server_open_time_map.end())
	{
		// 怕碰到计算开服至今时间的，0的话依然还是有效值
		return (unsigned int)-1;
	}

	return c_it->second.server_start_zero_time;
}

unsigned int World::GetServerOpenSixTime(const UniqueServerID& usid)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return (unsigned int)SHAREDCONFIG->GetOpenServerConfig().GetOpenServerSixTime();
	}

	ServerOpenTimeMap::const_iterator c_it = m_server_open_time_map.find(usid);
	if (c_it == m_server_open_time_map.end())
	{
		// 怕碰到计算开服至今时间的，0的话依然还是有效值
		return (unsigned int)-1;
	}

	return c_it->second.server_start_six_time;
}

int World::GetOpenServerDays_6(const UniqueServerID& usid)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		unsigned int open_server_six_time = (unsigned int)SHAREDCONFIG->GetOpenServerConfig().GetOpenServerSixTime();
		unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		if (now >= open_server_six_time)
		{
			return static_cast<int>((now - open_server_six_time)) / SECOND_PER_DAY + 1;
		}
		else
		{
			return 1;
		}
	}

	ServerOpenTimeMap::const_iterator c_it = m_server_open_time_map.find(usid);
	if (c_it == m_server_open_time_map.end())
	{
		// 怕碰到计算开服至今时间的，0的话依然还是有效值
		return 1;
	}

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (now >= c_it->second.server_start_six_time)
	{
		return static_cast<int>((now - c_it->second.server_start_six_time)) / SECOND_PER_DAY + 1;
	}
	else
	{
		return 1;
	}
}

int World::GetOpenServerDays_0(const UniqueServerID& usid)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		unsigned int open_server_zero_time = (unsigned int)SHAREDCONFIG->GetOpenServerConfig().GetOpenServerZeroTime();
		unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		if (now >= open_server_zero_time)
		{
			return static_cast<int>((now - open_server_zero_time)) / SECOND_PER_DAY + 1;
		}
		else
		{
			return 1;
		}
	}

	ServerOpenTimeMap::const_iterator c_it = m_server_open_time_map.find(usid);
	if (c_it == m_server_open_time_map.end())
	{
		// 怕碰到计算开服至今时间的，0的话依然还是有效值
		return 1;
	}

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (now >= c_it->second.server_start_zero_time)
	{
		return static_cast<int>((now - c_it->second.server_start_zero_time)) / SECOND_PER_DAY + 1;
	}
	else
	{
		return 1;
	}
}

int World::GetCurRealOpenDay(const UniqueServerID & usid)
{
	time_t cur_time = EngineAdapter::Instance().Time(); //取当前时间做计算
	time_t open_server_timestamp = this->GetServerOpenZeroTime(usid);
	int cur_real_open_day = GetDayIndex(open_server_timestamp, cur_time) + 1; // 开服天数，1~N

	if (cur_real_open_day <= 0)
	{
		cur_real_open_day = 1;
	}

	return cur_real_open_day;
}

int World::GetOpenDayByTimestamp(const UniqueServerID & usid, unsigned int timestamp)
{
	time_t open_server_timestamp = World::GetInstWorld()->GetServerOpenZeroTime(usid);
	int cur_real_open_day = GetDayIndex(open_server_timestamp, timestamp) + 1; // 开服天数，1~N

	if (cur_real_open_day <= 0)
	{
		cur_real_open_day = 1;
	}

	return cur_real_open_day;
}

int World::GetCurRealOpenDayByOpenTimestamp(unsigned int open_timestamp)
{
	time_t cur_time = EngineAdapter::Instance().Time(); //取当前时间做计算
	int cur_real_open_day = GetDayIndex(open_timestamp, cur_time) + 1; // 开服天数，1~N

	if (cur_real_open_day <= 0)
	{
		cur_real_open_day = 1;
	}

	return cur_real_open_day;
}

int World::GetDayByTimestamp(unsigned int begin_timestamp, unsigned int end_timestamp)
{
	int day = GetDayIndex(begin_timestamp, end_timestamp) + 1; // 天数，1~N
	if (day <= 0)
	{
		day = 1;
	}

	return day;
}

int World::GetCurRealOpenDay()
{
	time_t time = SHAREDCONFIG->GetOpenServerConfig().GetServerRealStartTime();
	return this->GetCurRealOpenDayByOpenTimestamp(time);
}

time_t World::GetMaxServerOpenTime()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return SHAREDCONFIG->GetOpenServerConfig().GetServerRealStartTime();
	}
	unsigned int temp_time = 0;
	ServerOpenTimeMap::const_iterator c_it = m_server_open_time_map.begin();
	for (; c_it != m_server_open_time_map.end(); c_it++)
	{
		if (temp_time < c_it->second.server_real_start_time)
		{
			temp_time = c_it->second.server_real_start_time;
		}
	}

	// 可能无跨服连接
	if (0 == temp_time)
	{
		temp_time = SHAREDCONFIG->GetOpenServerConfig().GetServerRealStartTime();
	}

	return temp_time;
}

void World::SetCreatAutoRobotServerID(UniqueServerID usid, int can_creat_static_robot)
{
	if (1 != can_creat_static_robot)
	{
		m_can_creat_auto_robot_map.erase(usid);
		return;
	}

	if (m_can_creat_auto_robot_map.find(usid) == m_can_creat_auto_robot_map.end())
	{
		m_can_creat_auto_robot_map[usid] = 1;
	}
}

bool World::RandCanCreatAutoRobotServerID(ARG_OUT int * mother_server_id)
{
	if (m_can_creat_auto_robot_map.empty())
	{
		return false;
	}

	int r = rand() % (int)m_can_creat_auto_robot_map.size();
	CanCreatAutoRobotMap::iterator it = m_can_creat_auto_robot_map.begin();
	std::advance(it, r);

	if (NULL != mother_server_id) *mother_server_id = it->first.server_id;

	return true;
}

void RMIHeartbeatCheckBackImpl::HeartbeatCheckRet()
{
	World::GetInstWorld()->OnDataaccessHeartbeat();
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
}

void World::WriteItemToWorkbench(char * path)
{
	ItemTxt::Instance().OnWriet(path);
}

void World::GetOneGatewayInfo(ARG_OUT std::string* hostname, ARG_OUT unsigned short* port)
{
	if (m_gateway_map.empty())
	{
		*hostname = "";
		*port = 0u;
	}
	else
	{
		GateWayInfo& info = m_gateway_map.begin()->second;
		*hostname = info.hostname_for_user;
		*port = info.port_for_user;
	}
}

bool World::RandGatewayInfo(ARG_OUT std::string* hostname, ARG_OUT unsigned short* port)
{
	if (m_gateway_map.empty())
	{
		*hostname = "";
		*port = 0u;

		return false;
	}
	else
	{
		int idx = rand() % m_gateway_map.size();
		GateWayMap::iterator it = m_gateway_map.begin();
		std::advance(it, idx);

		*hostname = it->second.hostname_for_user;
		*port = it->second.port_for_user;

		return true;
	}
}

void World::LOG_QUICK(int server_id, LogAgent &log_agent, long long n1, long long n2, const char *z1, const char *z2, long long n3, long long n4, 
	const char *z3, const char *z4, long long n5, long long n6, long long n7, long long n8, long long n9)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		LOG_QUICK12(log_agent, n1, n2, z1, z2, n3, n4, z3, z4, n5, n6, n7, n8, n9);
	}
	else
	{
		LOG_QUICK12(log_agent, n1, n2, z1, z2, n3, n4, z3, z4, n5, n6, n7, n8, n9);

		static crossgameprotocal::HiddenGameSyncLogQuick logquick;
		logquick.n1 = n1;
		logquick.n2 = n2;
		logquick.z1.assign((NULL == z1) ? "-" : z1);
		logquick.z2.assign((NULL == z2) ? "-" : z2);
		logquick.n3 = n3;
		logquick.n4 = n4;
		logquick.z3.assign((NULL == z3) ? "-" : z3);
		logquick.z4.assign((NULL == z4) ? "-" : z4);
		logquick.n5 = n5;
		logquick.n6 = n6;
		logquick.n7 = n7;
		logquick.n8 = n8;
		logquick.n9 = n9;

		InternalComm::Instance().SendSerializeMsgToGameThroughCross(&logquick, server_id);
	}
}

bool World::SyncLogToOriginServer(int server_id, int log_type, const char *format, ...)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return false;
	}

	if (NULL == format) return false;

	crossgameprotocal::HiddenGameSyncLogToOriginServer msg;
	msg.length = 0;
	msg.log_type = log_type;
	memset(msg.buffer, 0, sizeof(msg.buffer));

	va_list va;
	va_start(va, format);

	int ret = vsnprintf(msg.buffer, crossgameprotocal::SYNC_LOG_BUFFER_SIZE, format, va);
	if (ret >= 0 && ret < crossgameprotocal::SYNC_LOG_BUFFER_SIZE)
	{
		msg.length = ret;
	}
	else
	{
#ifdef _DEBUG
		assert(0);
#endif

		ret = -1;
	}

	va_end(va);

	msg.buffer[crossgameprotocal::SYNC_LOG_BUFFER_SIZE - 1] = '\0';

	if (ret < 0)
	{
		return false;
	}

	int plat_type = LocalConfig::Instance().GetPlatType();
	int length = sizeof(msg) - sizeof(msg.buffer) + msg.length;
	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, (const char*)&msg, length);

	return true;
}

bool World::IsOriginServerConnected(int plat_type, int server_id)
{
	return NULL != this->GetMotherServerId(UniqueServerID(plat_type, server_id));
}

void World::OnOriginServerDisconnectCross(int plat_type, int server_id)
{
	const UniqueServerID* usid = this->GetMotherServerId(UniqueServerID(plat_type, server_id));
	if (NULL == usid) return;

	m_mother_server_hash.erase(*usid);

	BigCrossManager::Instance().SyncGameServerInfoToBigCross(
		bigchgameprotocol::SYNC_GAME_SERVER_INFO_REQ_TYPE_UNSUBSCRIBE,
		1, 
		&server_id);
}

void World::SendNoticeNumToUser(const UserID& user_id, int notice_num)
{
	Protocol::SCNoticeNum msg;
	msg.notice_num = notice_num;

	this->TrySendMsgToUser(user_id, &msg, sizeof(msg));
}

bool World::TrySendMsgToUser(const UserID& user_id, const void* msg, unsigned int length)
{
	Role* role = this->GetSceneManager()->GetRole(user_id);
	if (NULL != role)
	{
		return EngineAdapter::Instance().NetSend(role->GetNetId(), msg, length);
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		return InternalComm::Instance().SendToGameUserThroughCross(UidToInt(user_id), msg, static_cast<int>(length));
	}
	else
	{
		return InternalComm::Instance().SendToHiddenUserThroughCross(UidToInt(user_id), msg, static_cast<int>(length));
	}
}

bool World::TrySendSerializeMsgToUser(const UserID & user_id, IMessageSerializer * msg)
{
	Role* role = this->GetSceneManager()->GetRole(user_id);
	if (NULL != role)
	{
		return EngineAdapter::Instance().NetSendSerializeMsg(role->GetNetId(), msg);
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		// 若当前为跨服，就发去本服
		return InternalComm::Instance().SendSerializeMsgToGameUserThroughCross(UidToInt(user_id), msg);
	}
	else
	{
		// 若当前为本服，就发去跨服
		return InternalComm::Instance().SendSerializeMsgToHiddenUserThroughCross(UidToInt(user_id), msg);
	}
}

void World::GetServerIDSet(std::set<int> & _out_server_set)
{
	MotherServerHash::iterator it_begin = m_mother_server_hash.begin();
	MotherServerHash::iterator it_end = m_mother_server_hash.end();

	for (; it_begin != it_end; ++it_begin)
	{
		_out_server_set.insert(it_begin->first.server_id);
	}
}

void World::HiddenSendHeartBeatToGame()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGameHeartBeat msg;
	InternalComm::Instance().SendToAllGameThroughCross(&msg, sizeof(msg));
}