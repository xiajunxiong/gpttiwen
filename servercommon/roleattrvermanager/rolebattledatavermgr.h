
#ifndef __ROLE_BATTLE_ATTR_VER_MGR__
#define __ROLE_BATTLE_ATTR_VER_MGR__

#include <stdio.h>

#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"

class TLVUnserializer;

/**********************************************************************************************
								
								��ɫս�����԰汾����
		note:
		(1) �汾����+1����
		(2) �벻Ҫɾ��֮ǰ�汾����
		(3) �벻Ҫ����֮ǰ�汾���ݸ�ʽ ���� char -> int 
		(4) ���û���漰չʾ�ֶ� ��ô�������Ľ�ɫ����ʱ ������¸�server
		(5) ���ں���׷���ֶ� ע�����л�ʱ����˳��

**********************************************************************************************/


// RoleBattleData�汾����

class RoleBattleDataVerMgr
{
public:

	enum
	{
		ROLE_BATTLE_DATA_VER_INVALID = 0,
		ROLE_BATTLE_DATA_VER_1,

		ROLE_BATTLE_DATA_VER_MAX,
	};

	static const int ROLE_BATTLE_DATA_CURRENT_VERSION = ROLE_BATTLE_DATA_VER_MAX - 1; // ��ǰ�ڴ�汾
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
	
	// �汾������ ��Ҫ���õĳ�ʼ������
	typedef void (RoleBattleDataVerMgr::*VerFunc_Ver_Change_Init) (RoleBattleData & data);

	struct VerFuncVerChangeItem 
	{
		short ver_cur;			// ��ǰ�汾
		short ver_next;			// �¸��汾
		VerFunc_Ver_Change_Init func;
	};
	
	// ���µ����µİ汾
	void Update_TO_Max_Version(short ver_cur, RoleBattleData & data);

	// ��һ���汾���ڶ����汾 ����õĳ�ʼ������ �൱��Diff��Ϣ 
	

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

