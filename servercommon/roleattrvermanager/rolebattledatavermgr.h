
#ifndef __ROLE_BATTLE_ATTR_VER_MGR__
#define __ROLE_BATTLE_ATTR_VER_MGR__

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

class RoleBattleDataVerMgr
{
public:

	enum
	{
		ROLE_BATTLE_DATA_VER_INVALID = 0,
		ROLE_BATTLE_DATA_VER_1,

		ROLE_BATTLE_DATA_VER_MAX,
	};

	static const int ROLE_BATTLE_DATA_CURRENT_VERSION = ROLE_BATTLE_DATA_VER_MAX - 1; // 当前内存版本
	static const int ROLE_BATTLE_DATA_MAX_BUFFER_LENGTH = 4096;							

	static RoleBattleDataVerMgr & Instance()
	{
		static RoleBattleDataVerMgr inst;
		return inst;
	}

	bool HandlerVerUnSerializer(short ver, RoleBattleData& role_battle_data, TLVUnserializer& instream);

private:

	typedef bool (RoleBattleData::*VerHandlerFunc) (TLVUnserializer &instream);

	struct VerHandlerItem 
	{
		short ver;
		VerHandlerFunc func;
	};

	VerHandlerItem* GetVerHandlerItemList()
	{
		static VerHandlerItem ver_hander_item_list[] = 
		{
			{ROLE_BATTLE_DATA_VER_INVALID, NULL},
			{ ROLE_BATTLE_DATA_VER_1, NULL},
		};

		UNSTD_STATIC_CHECK(sizeof(ver_hander_item_list) / sizeof(ver_hander_item_list[0]) == ROLE_BATTLE_DATA_VER_MAX);

		return &ver_hander_item_list[0];
	}

private:
	
	// 版本递增后 需要调用的初始化函数
	typedef void (RoleBattleDataVerMgr::*VerFunc_Ver_Change_Init) (RoleBattleData & data);

	struct VerFuncVerChangeItem 
	{
		short ver_cur;			// 当前版本
		short ver_next;			// 下个版本
		VerFunc_Ver_Change_Init func;
	};
	
	// 更新到最新的版本
	void Update_TO_Max_Version(short ver_cur, RoleBattleData & data);

	// 第一个版本到第二个版本 需调用的初始化函数 相当是Diff信息 
	

	VerFuncVerChangeItem* GetVerFuncChangeInitList()
	{
		static VerFuncVerChangeItem ver_func_vec_change_item_list[] = 
		{
			{ROLE_BATTLE_DATA_VER_INVALID, ROLE_BATTLE_DATA_VER_INVALID, NULL},
		};

		UNSTD_STATIC_CHECK(sizeof(ver_func_vec_change_item_list) / sizeof(ver_func_vec_change_item_list[0]) == (ROLE_BATTLE_DATA_VER_MAX - 1));

		return &ver_func_vec_change_item_list[0];
	}

private:
	RoleBattleDataVerMgr() {}
	RoleBattleDataVerMgr(const RoleBattleDataVerMgr &rhs);
};


#endif  // RMI_VER_MANAGER

