#ifndef __MSG_FABAO_HPP__
#define __MSG_FABAO_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/fabaodef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCFabaoInfo				//!< 2450 ����������Ϣ
	{
	public:
		SCFabaoInfo();
		MessageHeader		header;
		int week_use_coin_times;									//!< ���ܽ��ʹ�ô���
		int m_fabao_equipment_bar[MAX_FABAO_EQUIPMENT_BAR];			//!< ����װ����-��Ӧװ���ķ���index
		FaBaoEquip m_fabao_list[MAX_FABAO_COUNT];					//!< ������Ϣ
	};

	class CSFabaoReq				//!< 2451 ���󷨱�����
	{
	public:
		enum FABAO_REQ_TYPE
		{
			FABAO_REQ_TYPE_INFO = 0,			//!< 0 ��Ϣ
			FABAO_REQ_TYPE_ACTIVE = 1,			//!< 1 ����� p1 fabao_index
			FABAO_REQ_TYPE_UPLEVEL = 2,			//!< 2 ���� p1 fabao_index
			FABAO_REQ_TYPE_ADVANCED = 3,		//!< 3 ���� p1 fabao_index
			FABAO_REQ_TYPE_GUIDE_MONSTER = 9,		//!< 9 �򷨱�ָ����
			FABAO_REQ_TYPE_DRAW_MATERIAL = 10,		//!< 10 ���ò��� p1 index p2~p6 ��Ʒid
			FABAO_REQ_TYPE_DRAW_SPEED_UP = 11,		//!< 11  ����  p1 index  p2 ����
			FABAO_REQ_TYPE_DRAW = 12,				//!< 12 ��ȡ�齱���� p1 index
		};

		CSFabaoReq();
		MessageHeader		header;	

		int req_type;
		unsigned short param1;
		unsigned short param2;
		unsigned short param3;
		unsigned short param4;
		unsigned short param5;
		unsigned short param6;
	};

	class SCFabaoSignInfo			//!< 2452 ����������Ϣ�޸�
	{
	public:
		enum SEND_INFO_TYPE								// !< �·�ԭ��
		{
			SEND_INFO_TYPE_ACTIVE = 0,					// !< ��������
			SEND_INFO_TYPE_UPLEVEL = 1,					// !< ��������
			SEND_INFO_TYPE_ADVANCE = 2,					// !< ��������
			SEND_INFO_TYPE_DECOMPOSE = 3,				// !< �����ֽ�
			SEND_INFO_TYPE_RANDATTR = 4,				// !< ����ϴ��
			SEND_INFO_TYPE_TAKEUP_OR_OFF = 5,			// !< ��������
		};
		SCFabaoSignInfo();
		MessageHeader		header;
		int reason;
		int week_use_coin_times;							//!< ���ܽ��ʹ�ô���
		int m_fabao_equipment_bar[MAX_FABAO_EQUIPMENT_BAR];	//!< ����װ����-��Ӧװ���ķ���index
		FaBaoEquip m_fabao_info;							//!< ������Ϣ
	};

	class SCFabaoEquipBarInfo			//!< 2453 ����װ�������
	{
	public:
		SCFabaoEquipBarInfo();
		MessageHeader		header;

		int m_fabao_equipment_bar[MAX_FABAO_EQUIPMENT_BAR];	//!< ����װ����-��Ӧװ���ķ���index
	};

	class SCFabaoDrawInfo				//!< 2454,		// �����齱��Ϣ
	{
	public:
		SCFabaoDrawInfo();
		MessageHeader		header;

		FaBaoDrawItemInfo item_info[FA_BAO_DRAW_ITEM_MAX_NUM];
	};

	class SCFabaoDrawRewardInfo			//!<  2455,		// �����齱������Ϣ
	{
	public:
		SCFabaoDrawRewardInfo();
		MessageHeader		header;


		short fa_bao_id;
		short fa_bao_sui_pian_index;
		int item_id[FA_BAO_DRAW_MATERIAL_MAX_NUM];
		int item_num[FA_BAO_DRAW_MATERIAL_MAX_NUM];
	};
}

#pragma pack(pop)

#endif // __MSG_BABY_HPP__

