#ifndef __MSG_HOLY_EQUIP_H__
#define __MSG_HOLY_EQUIP_H__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "item/itembase.h"
#include "servercommon/commondata.hpp"

#pragma pack(push) 
#pragma pack(4)

static const int MAX_HOLY_LEVEL_UP_ITEM_TYPE = 4;		//!< ʥ��������������
static const int MAX_HOLY_LEVEL_UP_INDEX_NUM = 30;		//!< ���������������ɷ����ʥ������

namespace Protocol
{

	class CSHolyEquipOpReq				//!<4830 ʥ���ͻ��˲�������
	{
	public:
		CSHolyEquipOpReq();
		MessageHeader header;

		enum OP_TYPE
		{
			OP_TYPE_INFO_REQ = 0,		//!< ������Ϣ
			OP_TYPE_PUT_ON = 1,			//!< ����ʥ��״̬ param1 ����������� param2 �޸ĵ���״̬ HOLY_EQUIP_STATUS
			OP_TYPE_UNLOCK_HOLE = 2,	//!< �����������ܿ�λ param1 Ҫ������ʥ��index, param2 Ҫ�����Ŀ�λ
			OP_TYPE_STUDY_NEW_SKILL = 3,//!< �����µ��������� param1 Ҫ�����ʥ���������, param2 ʹ�õ��������id
			OP_TYPE_SAVE_NEW_SKILL = 4,	//!< ����ϴ���������¼��� param1 Ҫ�����ʥ���������, param2 ��������(1--����, 0--����)

			OP_TYPE_INLAY_FU_SHI = 5,	//!< ��Ƕ��ʯ param1 Ҫ��Ƕ��ʥ���������, param2 Ҫ��Ƕ��ʥ���Ŀ�λ, param3 Ҫ��Ƕ�ķ�ʯ�ĸ������
			OP_TYPE_TAKE_OFF_FU_SHI = 6,//!< ȡ�·�ʯ param1 Ҫȡ�·�ʯ��ʥ���������, param2 Ҫȡ�·�ʯ��ʥ���ķ�ʯ��λ
			OP_TYPE_FU_SHI_FUSION = 7,	//!< �ϳɷ�ʯ param1 Ŀ��ȼ� param2 ��ʯindex1, param3 ��ʯindex2, param4 ��ʯindex3, param5 ��ʯindex4, param6 ��ʯindex5
			OP_TYPE_FU_SHI_RECAST = 8,	//!< ������ʯ param1 Ҫ�����ķ�ʯindex, param2 �������ĵķ�ʯindex
			OP_TYPE_UNLOCK_FUSHI_HOLE = 9,//!< ������ʯ�� param1 ʥ��index param2 ��ʯ��λ0-4
			OP_TYPE_FU_SHI_RECAST_ON_HOLY_EQUIP = 10,	//!< ����ʥ���ϵķ�ʯ, param1 ʥ��index, param2 Ҫ�����ķ�ʯ��λ, param3 �������ĵķ�ʯindex
			OP_TYPE_HOLY_BLESS_PET = 11,//!< ����ʥ�� param1 ʥ��index param2 ����index(-1��ʾȡ��ʥ��)
			OP_TYPE_HOLY_FOLLOW	= 12,	//!< ʥ������ param1 ʥ��index
			OP_TYPE_HOLY_SKILL_CONVERT = 13,	//!< ʥ���������û� param1 ��������Ʒid_1, param2 ��������Ʒid_2
		};

		int op_type;
		int param1;
		int param2;
		int param3;
		int param4;
		int param5;
		int param6;
	};


	class CSHolyEquipLevelUpReq				//!<4831 ʥ����������
	{
	public:
		CSHolyEquipLevelUpReq();
		MessageHeader header;

		int holy_equip;		// Ҫ������ʥ��idx

		short material_type_num;				// ������������
		short holy_index_num;					// ����ʥ������
		unsigned short material_type[MAX_HOLY_LEVEL_UP_ITEM_TYPE];		// ����id, MAX_HOLY_LEVEL_UP_ITEM_TYPE = 4
		short material_num[MAX_HOLY_LEVEL_UP_ITEM_TYPE];		// ��������
		short holy_index[MAX_HOLY_LEVEL_UP_INDEX_NUM];		// ��ʥ����Ϊ����,���ɷ���30��
	};


	class SCHolyBagListInfo : public IMessageSerializer			//!<4832 ʥ������ȫ����Ϣ�·�
	{
	public:
		SCHolyBagListInfo();
		virtual ~SCHolyBagListInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(item_count, buffer, buffer_size, out_length);

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
				result = result && WriteShort(item_list[i].param_length, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].invalid_time, buffer, buffer_size, out_length);
				result = result && WriteLL(item_list[i].item_unique_id, buffer, buffer_size, out_length);
			
				result = result && WriteStrN(item_list[i].param_data, item_list[i].param_length, buffer, buffer_size, out_length);			
			}

			return result;
		}

		int	item_count;		//!< ��Ʒ����
		InfoType2 item_list[ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM];	//!< ��Ʒ�б�
	};



	class SCFuShiBagListInfo : public IMessageSerializer			//!<4833 ��ʯ����ȫ����Ϣ�·�
	{
	public:
		SCFuShiBagListInfo();
		virtual ~SCFuShiBagListInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(item_count, buffer, buffer_size, out_length);

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
				result = result && WriteShort(item_list[i].param_length, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].invalid_time, buffer, buffer_size, out_length);
				result = result && WriteLL(item_list[i].item_unique_id, buffer, buffer_size, out_length);
				
				result = result && WriteStrN(item_list[i].param_data, item_list[i].param_length, buffer, buffer_size, out_length);			
			}

			return result;
		}

		int	item_count;		//!< ��Ʒ����
		InfoType2 item_list[ItemNamespace::MAX_RUNE_STONE_BAG_NUM];	//!< ��Ʒ�б�
	};



	class SCHolyBagItemChange : public IMessageSerializer		//!<4834 ʥ��/��ʯ����������Ϣ�·�
	{
	public:
		enum BAG_TYPE
		{
			BAG_TYPE_HOLY = 1,
			BAG_TYPE_FUSHI = 2,
		};

	public:
		SCHolyBagItemChange();
		virtual ~SCHolyBagItemChange() {}
		MessageHeader header;

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
			result = result && WriteChar(bag_type, buffer, buffer_size, out_length);
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

		short				reason;					//!< �ı��ԭ�� \see HOLY_EQUIP_SEND_REASON
		short				put_reason;				//!< �����Ʒ����Դ  \see PUT_REASON_TYPE
		ItemID				item_id;				//!< ��Ʒid
		short				index;					//!< ��Ʒ�ڱ����е���� 
		char				is_market_buy_flag;		//!< �Ƿ���г������ʶ 0:�� 1:���г�����
		char				bag_type;				//!< ��������,1-ʥ������,2-��ʯ���� BAG_TYPE
		short				reserve_sh;
		int					num;					//!< ����
		char				is_bind;				//!< �Ƿ��
		char				has_param;				//!< �Ƿ��о�ֵ����
		short				param_length;			//!< ��ֵ���������ݳ��� ����has_paramΪ1ʱ�ɶ���
		UInt32				invalid_time;			//!< ����ʱ��
		long long           item_unique_id;			//!< ����ΨһID
		NetValueItemParamData param_data;			//!< ��ֵ���������ݣ�����param_length��ֵ����ȡ ����has_paramΪ1ʱ�ɶ���
	};

	class SCSCHolyEquipLevelUpConsumeInfo		//!<4835 ʥ�� ʥ��ʥ��/��ʯ�ϳ������б��·�
	{
	public:
		SCSCHolyEquipLevelUpConsumeInfo();
		MessageHeader header;

		enum BAG_TYPE
		{
			BAG_TYPE_HOLY = 1,
			BAG_TYPE_FUSHI = 2,
		};

		int bag_type;
		int count;
		int index[MAX_HOLY_LEVEL_UP_INDEX_NUM];
	};

	class SCHolyEquipFirstGainInfo		//4836 ʥ�� ��һ�λ��ʱ����ָ��
	{
	public:
		SCHolyEquipFirstGainInfo();
		MessageHeader header;

		int first_gain;		// 0-δ����,1-�Ѵ�����
	};

	class SCHolyEquipFollowInfo		//4837 ʥ�� ������Ϣ
	{
	public:
		SCHolyEquipFollowInfo();
		MessageHeader header;

		int follow_index;
	};

};

#pragma pack(pop)

#endif
