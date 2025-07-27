#ifndef __GLOBALSTR_H__
#define __GLOBALSTR_H__
#include <string>

namespace gamestring
{
	extern char GAMESTRING_BUF[2048];

	bool StringInit(const char *filename, std::string *error);

	
}

#endif // __GLOBALSTR_H__

