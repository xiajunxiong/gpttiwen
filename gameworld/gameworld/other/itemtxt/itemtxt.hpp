#ifndef __ITEM_TXT_MANAGER_HPP__
#define __ITEM_TXT_MANAGER_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include <vector>
#include <map>

static const int TXT_MAX_ITEM_ID = 65535;

class ItemTxt
{
public:
	static ItemTxt& Instance();

	void OnWriet(char* path);

private:
	ItemTxt();
	~ItemTxt();

	void SetItemName();
	void OutPutFile();

	const char* m_item_name[TXT_MAX_ITEM_ID];
	char* m_path;
};

#endif

