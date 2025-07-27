
#ifndef __BATTLE_CHARACTER_DATA_VER_MGR__
#define __BATTLE_CHARACTER_DATA_VER_MGR__

#include <stdio.h>

#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"

class TLVUnserializer;

/**********************************************************************************************
								
								角色战斗属性版本管理
		note:
		(1) 版本号请+1递增
		(2) 请不要删除之前版本数据
		(3) 请不要更改之前版本数据格式 比如 char -> int 
		(4) 如果没有涉及展示字段 那么仅仅更改角色属性时 无需更新该server
		(5) 请在后面追加字段 注意序列化时参数顺序

**********************************************************************************************/


// RoleBattleData版本管理

class BattleCharacterDataVerMgr
{
public:

	enum
	{
		BATTLE_CHARACTER_DATA_VER_INVALID = 0,
		BATTLE_CHARACTER_DATA_VER_1,

		BATTLE_CHARACTER_DATA_VER_MAX,
	};

	static const int BATTLE_CHARACTER_DATA_CURRENT_VERSION = BATTLE_CHARACTER_DATA_VER_MAX - 1; // 当前内存版本
	static const int BATTLE_CHARACTER_DATA_MAX_BUFFER_LENGTH = 1024;

	static BattleCharacterDataVerMgr & Instance()
	{
		static BattleCharacterDataVerMgr inst;
		return inst;
	}

	bool HandlerVerUnSerializer(short ver, BattleCharacterData& data, TLVUnserializer& instream);

private:

	typedef bool (BattleCharacterData::*VerHandlerFunc) (TLVUnserializer &instream);

	struct VerHandlerItem 
	{
		short ver;
		VerHandlerFunc func;
	};

	VerHandlerItem* GetVerHandlerItemList()
	{
		static VerHandlerItem ver_hander_item_list[] = 
		{
			{BATTLE_CHARACTER_DATA_VER_INVALID, NULL},
			{ BATTLE_CHARACTER_DATA_VER_1, NULL},
		};

		UNSTD_STATIC_CHECK(sizeof(ver_hander_item_list) / sizeof(ver_hander_item_list[0]) == BATTLE_CHARACTER_DATA_VER_MAX);

		return &ver_hander_item_list[0];
	}

private:
	
	// 版本递增后 需要调用的初始化函数
	typedef void (BattleCharacterDataVerMgr::*VerFunc_Ver_Change_Init) (BattleCharacterData & data);

	struct VerFuncVerChangeItem 
	{
		short ver_cur;			// 当前版本
		short ver_next;			// 下个版本
		VerFunc_Ver_Change_Init func;
	};
	
	// 更新到最新的版本
	void Update_TO_Max_Version(short ver_cur, BattleCharacterData & data);

	// 第一个版本到第二个版本 需调用的初始化函数 相当是Diff信息 
	

	VerFuncVerChangeItem* GetVerFuncChangeInitList()
	{
		static VerFuncVerChangeItem ver_func_vec_change_item_list[] = 
		{
			{BATTLE_CHARACTER_DATA_VER_INVALID, BATTLE_CHARACTER_DATA_VER_INVALID, NULL},
		};

		UNSTD_STATIC_CHECK(sizeof(ver_func_vec_change_item_list) / sizeof(ver_func_vec_change_item_list[0]) == (BATTLE_CHARACTER_DATA_VER_MAX - 1));

		return &ver_func_vec_change_item_list[0];
	}

private:
	BattleCharacterDataVerMgr() {}
	BattleCharacterDataVerMgr(const BattleCharacterDataVerMgr &rhs);
};


#endif  // __BATTLE_CHARACTER_DATA_VER_MGR__

