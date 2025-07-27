#ifndef __BATTLESTR_H__
#define __BATTLESTR_H__
#include <string>

namespace gamestring
{
	extern char GAMESTRING_BUF[2048];

	extern char* g_fastforward_battle_continue;

	bool StringInit(const char *filename, std::string *error);
}

#endif // __GLOBALSTR_H__

