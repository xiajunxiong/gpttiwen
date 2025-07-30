#include <iostream>
#include <stdlib.h>


#include "common/syspath.h"
#include "gameserver/game.h"
#include "log/logmain.h"
#include "network/networkmain.h"
#include "config/configmain.h"
#include "rmi/rmimain.h"
#include "gameserver/modulemain.h"

#include "serverlogic.h"
#include "gameworld/other/itemtxt/itemtxt.hpp"

#include "servercommon/servercommon.h"
#include "servercommon/version.hpp"

#ifdef _WIN32
#include "servercommon/exceptdump.h"
#endif

#ifdef __unix
#include <assert.h>
#include <sys/resource.h>

#define MAX_FILE_NO		(1024 * 32)

bool CheckResource()
{
	struct rlimit rl;
	if (getrlimit(RLIMIT_NOFILE, &rl) == -1)
	{
		return false;
	}
	rl.rlim_cur = (rl.rlim_cur > MAX_FILE_NO ? rl.rlim_cur : MAX_FILE_NO);
	rl.rlim_max = (rl.rlim_max > MAX_FILE_NO ? rl.rlim_max : MAX_FILE_NO);
	if (setrlimit(RLIMIT_NOFILE, &rl) == -1)
	{
		return false;
	}
	return true;
}
#endif

#ifdef __unix
#include "hotfix/hotfix.h"
#endif

bool g_is_check_res_succ = false;
bool g_is_unittest_succ = false;

int main(int argc, char *argv[])
{
	#ifdef _WIN32
	start_except_catch();
	#endif

	#ifdef __unix
	assert(CheckResource());
	#endif
	
	GameConfig config;
	config.loop_interval = 8;
	config.work_path = argv[0];
	SysPath::Up(&config.work_path);

	const char *config_file = "serverconfig.xml";
	if (argc > 1)
	{
		config_file = argv[1];
	}

	char* workbench_item_txt_path = 0;
	bool check_res_mode = false;
	bool unittest_mode = false;
	int total_server = 0, server_index = 0;
	if (argc > 2)
	{
		if(!strcmp(argv[2], "-checkres"))
		{
			total_server = 1, server_index = 0; // 检查模式下启动一个gameworld检查配置
			check_res_mode = true;
			printf("------------       check resource mode start       --------------- \n");
			printf("------------       please wait............         --------------- \n\n");

			if (argc == 4)
			{
				workbench_item_txt_path = argv[3];
			}
		}
		else if (!strcmp(argv[2], "-unittest"))
		{
			total_server = 1, server_index = 0;
			unittest_mode = true;
			printf("-------------      unittest mode start     -------------\n");
			printf("------------       please wait............ ----------- \n\n");
		}
		else 
		{	
			if (argc < 4) 
			{
				printf("EXEgameworld serverconfig.xml total_game game_index");
				return -1;
			}

			total_server = atoi(argv[2]); server_index = atoi(argv[3]);
			if (total_server <= 0 || server_index < 0 || server_index > total_server)	// server_index == total_server 时为特殊GS
			{
				printf("EXEgameworld serverconfig.xml total_game game_index param error");
				return -2;
			}
		}
	}

	if (!check_res_mode && !unittest_mode)
	{
		if (total_server <= 0 || server_index < 0 || server_index > total_server)
		{
			printf("EXEgameworld serverconfig.xml total_game game_index param error");
			return -3;
		}
	}

	if (!ReadHotfixFile(HOTFIX_GAMEWORLD_PATH))
	{
		return -4;
	}

	Game game;

	// 静态链接模块
	gsclock::Register(&game);
	gsgame::Register(&game, &game);
	gstimer::Register(&game);
	gsfilewrite::Register(&game);

	game.RegisterModule(SERVER_MODULE, new ServerLogic(check_res_mode, unittest_mode, total_server, server_index, workbench_item_txt_path));


	// 动态链接模块
	ConfigDLL::Register(&game, config_file, true);
	LogDLL::Register(&game);
	NetworkDLL::Register(&game);
	NetworkDLL::Register(&game, INTERNAL_NETWORK);
	RMIDLL::Register(&game, INTERNAL_NETWORK);

	game.Run(config);

	if (check_res_mode)
	{
		if (g_is_check_res_succ)
			printf("Check Res SUCC SUCC SUCC!!!\n");
		else
			printf("Check Res FAIL FAIL FAIL!!!\n");
	}

	if (unittest_mode)
	{
		if (g_is_unittest_succ)
		{
			printf("Unit Test SUCC SUCC SUCC!!!\n");
		}
		else
		{
			printf("Unit Test FAIL FAIL FAIL\n");
		}
	}
	

	system("pause");

	#ifdef _WIN32
	stop_except_catch();
	#endif

	return 0;
}

