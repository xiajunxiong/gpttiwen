
#ifndef __BATTLE_CHARACTER_DATA_VER_MGR__
#define __BATTLE_CHARACTER_DATA_VER_MGR__

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

class BattleCharacterDataVerMgr
{
public:

	enum
	{
		BATTLE_CHARACTER_DATA_VER_INVALID = 0,
		BATTLE_CHARACTER_DATA_VER_1,

		BATTLE_CHARACTER_DATA_VER_MAX,
	};

	static const int BATTLE_CHARACTER_DATA_CURRENT_VERSION = BATTLE_CHARACTER_DATA_VER_MAX - 1; // ��ǰ�ڴ�汾
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
	
	// �汾������ ��Ҫ���õĳ�ʼ������
	typedef void (BattleCharacterDataVerMgr::*VerFunc_Ver_Change_Init) (BattleCharacterData & data);

	struct VerFuncVerChangeItem 
	{
		short ver_cur;			// ��ǰ�汾
		short ver_next;			// �¸��汾
		VerFunc_Ver_Change_Init func;
	};
	
	// ���µ����µİ汾
	void Update_TO_Max_Version(short ver_cur, BattleCharacterData & data);

	// ��һ���汾���ڶ����汾 ����õĳ�ʼ������ �൱��Diff��Ϣ 
	

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

