#ifndef __ITEM_POOL_H__
#define __ITEM_POOL_H__

#include <limits.h>
#include <string>
#include <map>

#include "servercommon/basedef.h"
#include "servercommon/serverdef.h"
#include "servercommon/struct/itemlistparam.h"

class ItemBase;

#define ITEMPOOL ItemPool::Instance()

class ItemPool 
{
public:
	static const int MAX_ITEM_ID = 65535;
	static const int MAX_SUIT_ID = 1024;

	enum RELOAD_ITEM_TYPE
	{
		RELOAD_I_TYPE_EXPENSE = 0,
		RELOAD_I_TYPE_OTHER,
		RELOAD_I_TYPE_DEFGIFT,
		RELOAD_I_TYPE_RANDGIFT,
	};

	static ItemPool * Instance();

	bool Init(const std::string &dir, const std::string &filename, std::string *err);	
	static bool Reload(const std::string &dir, const std::string &filename, std::string *err);
	bool ReloadOneItem(const std::string &dir, int item_type, ItemID item_id, std::string *err);

	const ItemBase * GetItem(ItemID item_id);
	bool CheckItemLegal(ItemID item_id, int price, const char * reason_str);
	short GetItemColumn(ItemID item_id);
	static const char* ItemDataWrapperLog(const ItemDataWrapper *item_wrapper, int index);
		
	void AddBaseWeaponID(int weapon_type, ItemID weapon_id);
	ItemID GetBaseWeaponID(int weapon_type);
private:
	ItemPool();
	~ItemPool();

	void Release();

	long long ConvertToLongLong(short param1, short param2, short param3, char param4, char param5);

	ItemBase *m_item_pool[MAX_ITEM_ID];						// ŒÔ∆∑≥ÿ

	ItemID m_base_weapon_id_list[WEAPON_TYPE_MAX];
};

#endif // __ITEM_POOL_H__

