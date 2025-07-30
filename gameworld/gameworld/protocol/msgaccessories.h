#ifndef MSGACCESSORIESMENT_H
#define MSGACCESSORIESMENT_H

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "item/itembase.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSAccessoriesMentOperaReq					//!< 4150 ���β�������
	{
	public:
		CSAccessoriesMentOperaReq();
		MessageHeader header;

		enum OPERA_TYPE
		{
			OPERA_TYPE_INFO_REQ = 0,			//!< 0 ������Ϣ
			OPERA_TYPE_PUTON = 1,				//!< 1 ����װ�� \param param1 ������id  \param param2 �����������  \param param3 �����Ĳ�λ \see EQUIPMENT_INDEX
			OPERA_TYPE_TAKEOFF = 2,				//!< 2 ����װ�� \param param1 Ҫ���µĲ�λ \see EQUIPMENT_INDEX
			OPERA_TYPE_DISCOMPOSE = 3,			//!< 3 �ֽ�װ�� param1 ����coloumn param2 ����index param3 ����Ĭ�� = 1
			OPERA_TYPE_ACCESSORIES = 4,			//!< 4 ǿ��װ�� param1 �Ƿ񴩴� param2 ����index
			OPERA_TYPE_CHANGE = 5,				//!< 5 �û�װ�� param1 �Ƿ񴩴� param2 ����coloumn param3 ����index(����index) param4 �µ���id
			OPERA_TYPE_SPECIAL = 6,				//!< 6 �������� param1 seq[1-2]
		};

		int msg_type;							//!< ��Ϣ����  \see OPERA_TYPE
		int param1;
		int param2;
		int param3;
		int param4;
	};

	class CSAccessoriesMentVocComposeReq// 4153 ���δ����̳�
	{
	public:
		CSAccessoriesMentVocComposeReq();
		struct ComposeVoc
		{
			ComposeVoc() :voc_index(-1), replace_voc_index(-1) {}
			int voc_index;
			int replace_voc_index;
		};
		MessageHeader header;

		int is_wear;
		int index;
		int replace_index;
		ComposeVoc compose_voc_list[MAX_ACCESSORIES_VOC_NUM];
	};

	class SCAccessoriesMentInfo					//!< 4151 ������Ϣ
	{
	public:
		SCAccessoriesMentInfo();
		MessageHeader header;
		int capability;
		AccessoriesPro equipment_list[ItemNamespace::MAX_ACCESSORIES_INDEX_NUM];
	};

	class SCAccessoriesMentChange					//!< 4152 �������θı�֪ͨ
	{
	public:
		SCAccessoriesMentChange();
		MessageHeader header;
		int capability;
		AccessoriesPro equip_info;
	};

	class SCAccessoriesSpecialEquip					//!< 4154 ���� �·�
	{
	public:
		SCAccessoriesSpecialEquip();
		MessageHeader header;
		int accessories_special_equip_list[ACCESSORIES_SPECIAL_EQUIP_LEVEL];
	};
}

#pragma pack(pop)

#endif


