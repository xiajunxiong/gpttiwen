#ifndef __BIGCROSSSTR_H__
#define __BIGCROSSSTR_H__
#include <string>

namespace gamestring
{
	extern char GAMESTRING_BUF[2048];

	extern char* g_uta_rank_reward_subject;
	extern char* g_uta_qualification_rank_reward_content;
	extern char* g_uta_main_rank_reward_content;
	extern char* g_uta_advance_reward_subject;
	extern char* g_uta_advance_reward_content;
	extern char* g_battle_server_disconnect;
	extern char* g_bigcross_battle_server_disconnect;

	bool StringInit(const char *filename, std::string *error);
}

#endif // __GLOBALSTR_H__

