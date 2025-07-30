#ifndef __MSG_JEWELRY_H__
#define __MSG_JEWELRY_H__

#include "gamedef.h"
#include "servercommon/workshopdef.h"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/itemlistparam.h"
#include "item/itembase.h"
#include "equipment/jewelry.h"

#pragma pack(push, 4) 

namespace Protocol
{
	class CSJewelryHandleReq
	{
	public:
		CSJewelryHandleReq();		//!< 1740�� ���� ϵͳ��������
		MessageHeader header;

		enum OPERA_TYPE
		{
			OPERA_TYPE_ALL_INFO_REQ = 0,		//!< �������� ���� ��Ϣ
			OPERA_TYPE_WEAR_JEWELRY = 1,		//!< ����װ������
			OPERA_TYPE_TAKE_OFF_ONE = 2,		//!< ������������
		};

		short op_type;		//!< ��������

		short param;		//!< ���������0 - �ޣ� 1 - ��������ID�� 2 - ����װ����ID��
	};

	class SCJewelryAllInform
	{
	public:
		SCJewelryAllInform();		//!< 1741�� װ��������Ϣ����
		MessageHeader header;

		int jewelry_count;		//!< ���� ����
		JewelryPro jewelry_list[Jewelry::J_TYPE_MAX];
	};

	class SCJewelryInfoAlter
	{
	public:
		SCJewelryInfoAlter();		//!< 1742�� ����������Ϣ����
		MessageHeader header;

		JewelryPro jewelry_info;
	};
}

#pragma pack(pop)

#endif


