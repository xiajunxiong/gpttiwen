
#ifndef MSGEQUIPMENT_H
#define MSGEQUIPMENT_H

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "item/itembase.h"
#include "servercommon/commondata.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSEquipmentOperaReq					//!< 1650 װ����������
	{
	public:
		CSEquipmentOperaReq();
		MessageHeader header;

		enum OPERA_TYPE
		{
			OPERA_TYPE_INFO_REQ = 0,			//!< 0 ������Ϣ
			OPERA_TYPE_PUTON = 1,				//!< 1 ����װ�� \param param1 ������id  \param param2 �����������  \param param3 �����Ĳ�λ \see EQUIPMENT_INDEX
			OPERA_TYPE_TAKEOFF = 2,				//!< 2 ����װ�� \param param1 Ҫ���µĲ�λ \see EQUIPMENT_INDEX
			OPERA_TYPE_TAKEOFF_CRYSTAL = 3,		//!< 3 ����Ԫ��ˮ��
			OPERA_TYPE_VOC_LOCK = 4,			//!< 4 ������ param1 װ�����ͣ�1��װ��/2���Σ� param2 �±꣨װ��[0-4]/����[0-2]�� param3 �����±꣨[0-5]�� param4 �Ƿ����� 0/1
			OPERA_TYPE_UPGREADE = 5,			//!< 5 װ��ϵͳǿ�� param1 ��������[0,�·���1ǿ��]		param2 װ���±�
			OPERA_TYPE_RESONACE = 6,			//!< 6 ��װ��������
		};

		int msg_type;							//!< ��Ϣ����  \see OPERA_TYPE
		int param1;
		int param2;
		int param3;
		int param4;
	};

	class SCEquipmentInfoAck					//!< 1600 װ����Ϣ
	{
	public:
		SCEquipmentInfoAck();
		MessageHeader header;

		ItemID element_crystal_id;				//!< Ԫ��ˮ������ƷID
		short used_times;						//!< Ԫ��ˮ����ʹ�ô���

		short reserve_sh;
		short count;
		EquipmentPro equipment_list[ItemNamespace::MAX_EQUIPMENT_GRID_NUM];
	};

	class SCEquipmentGridChange					//!< 1601 װ���ı�֪ͨ
	{
	public:
		SCEquipmentGridChange();
		MessageHeader header;

		EquipmentPro equip_info;
	};

	class SCElementCrystalChangeNotice			//!< 1602 Ԫ��ˮ�����֪ͨ
	{
	public:
		SCElementCrystalChangeNotice();
		MessageHeader header;

		ItemID element_crystal_id;				//!< Ԫ��ˮ������ƷID
		short used_times;						//!< Ԫ��ˮ����ʹ�ô���
	};

	class SCEquipmentLockInfo					//!< 1603 ��������Ϣ
	{
	public:
		SCEquipmentLockInfo();
		MessageHeader header;

		EquipVocLock m_equipment_list[EQUIPMENT_MAX_INDEX];
		EquipVocLock m_jewelry_list[JEWELRY_MAX_INDEX];
	};

	class SCEquipmentJewerlyUpGreadeInfo					//!< 1604 װ��ǿ��ϵͳ
	{
	public:
		SCEquipmentJewerlyUpGreadeInfo();
		MessageHeader header;

		int equipment_jewelry_upgreade[EQUIP_JEWELRY_TYPE_MAX];//ǿ���ȼ�
		int equipment_jewelry_strengthen[EQUIP_JEWELRY_TYPE_MAX];//ͻ�Ƶȼ�
	};

	class SCEquipmentJewerlyResonaceInfo					//!< 1605 ��װ����
	{
	public:
		SCEquipmentJewerlyResonaceInfo();
		MessageHeader header;

		int resonace_flag;
	};

	class CSZaoHuaEquipmentOperaReq					//!< 1640 �컯װ����������
	{
	public:
		CSZaoHuaEquipmentOperaReq();
		MessageHeader header;

		enum OPERA_TYPE
		{
			OPERA_TYPE_INFO_REQ = 0,			//!< 0 ������Ϣ
			OPERA_TYPE_PUTON = 1,				//!< 1 ����װ�� param1 �����������
			OPERA_TYPE_TAKEOFF = 2,				//!< 2 ����װ�� param1 Ҫ���µĲ�λ
			OPERA_TYPE_ORDER = 3,				//!< 3 ������
			OPERA_TYPE_COMPOSE = 4,				//!< 4 �ϳ�		param1 �ϳ�seq	param2 �Ƿ񴩴���0/1�� param3 index
			OPERA_TYPE_DISCOMPOSE = 5,			//!< 5 �ֽ�		param1 ����idx
			OPERA_TYPE_ACTIVE_APP = 6,			//!< 6 ������� param1 ���id
			OPERA_TYPE_APP_WEAR = 7,			//!< 7 ������� param1 ���id
			OPERA_TYPE_APP_WEAROFF = 8			//!< 8 �������
		};

		int msg_type;							//!< ��Ϣ����  \see OPERA_TYPE
		int param1;
		int param2;
		int param3;
	};

	class SCZaoHuaBagListInfo : public IMessageSerializer			//!< 1641 �컯װ�������·�
	{
	public:
		SCZaoHuaBagListInfo();
		virtual ~SCZaoHuaBagListInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteShort(reason, buffer, buffer_size, out_length);
			result = result && WriteShort(item_count, buffer, buffer_size, out_length);

			for (int i = 0; i < item_count; ++i)
			{
				result = result && WriteUShort(item_list[i].item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].index, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].is_market_buy_flag, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].reserve_ch, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].reserve_sh, buffer, buffer_size, out_length);
				result = result && WriteInt(item_list[i].num, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].is_bind, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].has_param, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].invalid_time, buffer, buffer_size, out_length);
				result = result && WriteLL(item_list[i].item_unique_id, buffer, buffer_size, out_length);

				if (1 == item_list[i].has_param)
				{
					result = result && WriteShort(item_list[i].param_length, buffer, buffer_size, out_length);
					result = result && WriteStrN(item_list[i].param_data, item_list[i].param_length, buffer, buffer_size, out_length);
				}
			}

			return result;
		}
		short				reason;														//!< �·�����
		short				item_count;													//!< ��Ʒ����

		InfoType2			item_list[ItemNamespace::MAX_ZAO_HUA_BAG_NUM];				//!< ��Ʒ�б�
	};

	class SCZaoHuaBagItemChange : public IMessageSerializer //!< 1642 �컯���������޸��·�
	{
	public:
		SCZaoHuaBagItemChange();
		virtual ~SCZaoHuaBagItemChange() {}
		MessageHeader		header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteShort(reason, buffer, buffer_size, out_length);
			result = result && WriteShort(put_reason, buffer, buffer_size, out_length);
			result = result && WriteUShort(item_id, buffer, buffer_size, out_length);
			result = result && WriteShort(index, buffer, buffer_size, out_length);
			result = result && WriteChar(is_market_buy_flag, buffer, buffer_size, out_length);
			result = result && WriteChar(reserve_ch, buffer, buffer_size, out_length);
			result = result && WriteShort(reserve_sh, buffer, buffer_size, out_length);
			result = result && WriteInt(num, buffer, buffer_size, out_length);
			result = result && WriteChar(is_bind, buffer, buffer_size, out_length);
			result = result && WriteChar(has_param, buffer, buffer_size, out_length);
			result = result && WriteUInt(invalid_time, buffer, buffer_size, out_length);
			result = result && WriteLL(item_unique_id, buffer, buffer_size, out_length);

			if (1 == has_param)
			{
				result = result && WriteShort(param_length, buffer, buffer_size, out_length);
				result = result && WriteStrN(param_data, param_length, buffer, buffer_size, out_length);
			}

			return result;
		}

		short				reason;					//!< �ı��ԭ��
		short				put_reason;				//!< �����Ʒ����Դ  \see PUT_REASON_TYPE
		ItemID				item_id;				//!< ��Ʒid
		short				index;					//!< ��Ʒ�ڱ����е���� 
		char				is_market_buy_flag;		//!< �Ƿ���г������ʶ 0:�� 1:���г�����
		char				reserve_ch;
		short				reserve_sh;
		int					num;					//!< ����
		char				is_bind;				//!< �Ƿ��
		char				has_param;				//!< �Ƿ��о�ֵ����
		short				param_length;			//!< ��ֵ���������ݳ��� ����has_paramΪ1ʱ�ɶ���
		UInt32				invalid_time;			//!< ����ʱ��
		long long           item_unique_id;			//!< ����ΨһID
		NetValueItemParamData param_data;			//!< ��ֵ���������ݣ�����param_length��ֵ����ȡ ����has_paramΪ1ʱ�ɶ���
	};

	class SCZaoHuaEquipmentInfoAck					//!< 1643 �컯װ����Ϣ
	{
	public:
		SCZaoHuaEquipmentInfoAck();
		MessageHeader header;

		int count;
		ZaoHuaPro equipment_list[ItemNamespace::MAX_ZAO_HUA_EQUIPMENT_GRID_NUM];
	};

	class SCZaoHuaEquipmentGridChange					//!< 1644 �컯װ���ı�֪ͨ
	{
	public:
		SCZaoHuaEquipmentGridChange();
		MessageHeader header;

		ZaoHuaPro equip_info;
	};

	class SCZaoHuaEquipmentComposeResults : public IMessageSerializer
	{
	public:
		SCZaoHuaEquipmentComposeResults();		//!< 1645 - �컯װ���������ظ�
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteChar(com_result, buffer, buffer_size, out_length);
			result = result && WriteChar(has_param, buffer, buffer_size, out_length);
			result = result && WriteUShort(item_id, buffer, buffer_size, out_length);
			result = result && WriteUShort(number, buffer, buffer_size, out_length);

			if (0 != has_param)
			{
				result = result && WriteUShort(param_len, buffer, buffer_size, out_length);
				result = result && WriteStrN(param_data, param_len, buffer, buffer_size, out_length);
			}

			return result;
		}

		char com_result;		//!< �ϳɽ��: 0 - ʧ�ܣ�1 - �ɹ���
		char has_param;			//!< �Ƿ���о�ֵ
		ItemID item_id;			//!< ����ID
		UInt16 number;			//!< ����
		UInt16 param_len;		//!< ���ݳ���
		NetValueItemParamData param_data;	//!< ������Ϣ
	};

	class SCZaoHuaAppearance					//!< 1646 �컯װ�����
	{
	public:
		SCZaoHuaAppearance();
		MessageHeader header;

		int wear_app_id;
		BitMap<ZAO_HUA_SUIT_APPEARANCE_MAX> flag;
	};

	class CSZaoHuaEquipmentDiscomposeReq					//!< 1647 �컯װ���ֽ��б�����
	{
	public:
		CSZaoHuaEquipmentDiscomposeReq();
		MessageHeader header;

		BitMap<512> discompose_list;
	};
	UNSTD_STATIC_CHECK(512 >= ItemNamespace::MAX_ZAO_HUA_BAG_NUM);

	class SCZaoHuaEquipmentCommonDataInfo					//!< 1648 �컯װ��ͨ����Ϣ�·�
	{
	public:
		SCZaoHuaEquipmentCommonDataInfo();
		MessageHeader header;

		int show_mo_zhuang;
	};
}

#pragma pack(pop)

#endif


