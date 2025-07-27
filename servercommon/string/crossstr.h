#ifndef __CROSSSTR_H__
#define __CROSSSTR_H__

#include <string>
namespace gamestring
{
	extern char GAMESTRING_BUF[2048];

	bool StringInit(const char *filename, std::string *error);
}

#endif // __CROSSSTR_H__

