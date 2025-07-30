
#ifndef MSG_ZAO_HUA_EQUIPMENT_H
#define MSG_ZAO_HUA_EQUIPMENT_H

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/userprotocal/msgheader.h"
#include "item/itembase.h"
#include "servercommon/zaohuaequipdef.hpp"


#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum ZAO_HUA_EQUIPMENT_REQ_TYPE
	{
		ZAO_HUA_EQUIPMENT_REQ_TYPE_REFINE = 0,			//!< �컯װ�������� p1:1600Э������index
		ZAO_HUA_EQUIPMENT_REQ_TYPE_HUN_LI_UP_LEVEL = 1,		//!< ��������  p1 item_src,��YUAN_HUN_ZHU_ITEM_SRC  p2 index,  p3 column   
		ZAO_HUA_LONG_ZHU_DRAW = 2,		//!< �����齱 p1 equip_index p2 slot_index p3 ����
		ZAO_HUA_LONG_ZHU_PUT_ON = 3,		//!< �������� p1 equip_index p2 slot_index p3 ����index
		ZAO_HUA_LONG_ZHU_CLEAR_1 = 4,		//!< �������1�� p1 equip_index p2 slot_index p3 ����index
		ZAO_HUA_LONG_ZHU_CLEAR_ALL = 5,	//!< �������ȫ�� p1 equip_index p2 slot_index
		ZAO_HUA_LONG_ZHU_UNLOCK = 6,	//!< �������� p1 equip_index p2 slot_index
		ZAO_HUA_RU_MO = 7,	//!< ��ħ  p1 item_src,��YUAN_HUN_ZHU_ITEM_SRC  p2 index,
		ZAO_HUA_DECOMPOSE = 8,	//!< �ֽ�Ԫ���� p1 index,  p2 column   
		ZAO_HUA_COMPOSE = 9,	//!< �ϳ�Ԫ���� p1 item_id
	};

	class CSZaoHuaEquipmentReq					//!< 4800 װ����������
	{
	public:
		CSZaoHuaEquipmentReq();
		MessageHeader header;

		int req_type;							
		int param1;
		int param2;
		int param3;
	};

	class SCZaoHuaReFineInfo					//!< 4801 װ����Ϣ
	{
	public:
		SCZaoHuaReFineInfo();
		MessageHeader header;

		int refine_info[ItemNamespace::MAX_ZAO_HUA_EQUIPMENT_GRID_NUM];		//8 �����ȼ�
	};

	enum ZAO_HUA_YUAN_HUN_ZHU_REQ_TYPE
	{
		ZAO_HUA_YUAN_HUN_ZHU_REQ_TYPE_PUT_ON,		//���� p1 item_src p2 index,  p3 column,  p4 slot_index  |  ���� p5  item_src  p6 index, p7  column   p8 slot_index
		ZAO_HUA_YUAN_HUN_ZHU_REQ_TYPE_TAKE_OFF,	//���� p1 item_src p2 index,  p3 column,  p4 slot_index  |  ���� p5  item_src  p6 index, p7  column   p8 slot_index
	};

	class CSZaoHuaYuanHunZhuReq					//!< 4802
	{
	public:
		CSZaoHuaYuanHunZhuReq();
		MessageHeader		header;

		int req_type;
		short param1;
		short param2;
		short param3;
		short param4;
		short param5;
		short param6;
		short param7;
		short param8;
	};

	class CSYuanHunZhuUpLevelReq				//!< 4803			Ԫ������������
	{
	public:
		CSYuanHunZhuUpLevelReq();
		MessageHeader		header;

		short item_src;
		short index;
		short  column;
		short slot_index;
	};

	class SCZaoHuaLongZhuInfo					//!< 4804 ������Ϣ
	{
	public:
		SCZaoHuaLongZhuInfo();
		MessageHeader header;

		ZaoHuaLongZhuPartParam long_zhu_info[ItemNamespace::MAX_ZAO_HUA_EQUIPMENT_GRID_NUM];  //8����λ	
	};

	class SCZaoHuaLongZhuSlotInfo //!< 4805 ����������λ��Ϣ
	{
	public:
		SCZaoHuaLongZhuSlotInfo();
		MessageHeader header;

		int equip_index;
		int slot_index;
		ZaoHuaLongZhuSlotParam long_zhu_slot_info;  
	};
}

#pragma pack(pop)

#endif


