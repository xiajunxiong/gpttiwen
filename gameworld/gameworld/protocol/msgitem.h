#ifndef MSGITEM_H
#define MSGITEM_H

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/serverdef.h"
#include "servercommon/struct/itemlistparam.h"
#include "item/itembase.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{

	/*
	��Ʒ��ز���������ʹ����Ʒ��װ����
	*/

	class CSKnapsackOperaReq			//!< 1550 ������������
	{
	public:
		enum OPERA_TYPE					//!< ������������ö��
		{
			OPERA_TYPE_REQUEST_INFO,	//!< ���󱳰���Ϣ 0  
			OPERA_TYPE_EXTEND,			//!< ��չ���� 1  \param param1 ������ID  \param param2 ��չ������
			OPERA_TYPE_DEPOSIT,			//!< ����ֿ� 2  \param param1 ������ID  \param param2 ��Ʒ������� \param param3 �ֿ�ҳ��
			OPERA_TYPE_WITHDRAW,		//!< ȡ���ֿ����ʱ���� 3  \param param1 ������ID(�ֿ����ʱ����) \param param2 ���ڵ����
			OPERA_TYPE_WITHDRAW_ALL,	//!< ȡ�����вֿ����ʱ�����е���Ʒ 4  \param param1 ������ID(�ֿ����ʱ����)
			OPERA_TYPE_INORDER,			//!< ������ 5  \param param1 ������ID
			OPERA_TYPE_DISCARD,			//!< ����(����)��Ʒ 6 \param param1 ������ID \param param2 ��Ʒ�������  \param param3 ������Ʒ����
		};

	public:
		CSKnapsackOperaReq();		
		MessageHeader		header;

		int					opera_type;	//!< �������� \see OPERA_TYPE
		int					param1;
		int					param2;
		int					param3;
		int					param4;
	};

	class SCKnapsackInfoAck : public IMessageSerializer  //!< 1500 ������Ϣ����
	{
	public:
		enum SEND_REASON
		{
			SEND_REASON_NORMAL = 0,				//!< 0 �����·������߻�ͻ�������			
			SEND_REASON_IN_ORDER = 1,			//!< 1 ����
		};

	public:
		SCKnapsackInfoAck();
		virtual ~SCKnapsackInfoAck() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			for (int i = ItemNamespace::ITEM_COLUMN_TYPE_MIN; i < ItemNamespace::ITEM_COLUMN_TYPE_MAX; ++i)
			{
				result = result && WriteShort(valid_grid_num_list[i], buffer, buffer_size, out_length);
			}

			for (int i = ItemNamespace::ITEM_COLUMN_TYPE_MIN; i < ItemNamespace::ITEM_COLUMN_TYPE_MAX; ++i)
			{
				result = result && WriteShort(extend_times_list[i], buffer, buffer_size, out_length);
			}

			result = result && WriteShort(reason, buffer, buffer_size, out_length);
			result = result && WriteShort(item_count, buffer, buffer_size, out_length);

			for (int i = 0; i < item_count; ++i)
			{
				result = result && WriteUShort(item_list[i].item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].column, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].index, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].is_market_buy_flag, buffer, buffer_size, out_length);
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
		
		short				valid_grid_num_list[ItemNamespace::ITEM_COLUMN_TYPE_MAX];	//!< ��λ��Ч�������б� \see ITEM_COLUMN_TYPE_MAX
		short				extend_times_list[ItemNamespace::ITEM_COLUMN_TYPE_MAX];		//!< ��λ��չ�����б� \see ITEM_COLUMN_TYPE_MAX
		
		short				reason;														//!< �·�����
		short				item_count;													//!< ��Ʒ����

		InfoType			item_list[ItemNamespace::KNAPSACK_MAX_GRID_NUM];			//!< ��Ʒ�б�  \see InfoType \see KNAPSACK_MAX_GRID_NUM
	};
	UNSTD_STATIC_CHECK(9 == ItemNamespace::ITEM_COLUMN_TYPE_MAX);

	class SCKnapsackItemChange : public IMessageSerializer //!< 1501 ������Ʒ�ı�֪ͨ
	{
	public:
		SCKnapsackItemChange();	
		virtual ~SCKnapsackItemChange() {}
		MessageHeader		header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteShort(reason, buffer, buffer_size, out_length);
			result = result && WriteShort(put_reason, buffer, buffer_size, out_length);
			result = result && WriteUShort(item_id, buffer, buffer_size, out_length);
			result = result && WriteShort(column, buffer, buffer_size, out_length);
			result = result && WriteShort(index, buffer, buffer_size, out_length);
			result = result && WriteChar(is_market_buy_flag, buffer, buffer_size, out_length);
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
		short				column;					//!< ��Ʒ���ĸ��������� [0,9)
		short				index;					//!< ��Ʒ�ڱ����е���� 
		char				is_market_buy_flag;		//!< �Ƿ���г������ʶ 0:�� 1:���г�����
		char				reserve_ch;
		short				num;					//!< ����
		char				is_bind;				//!< �Ƿ��
		char				has_param;				//!< �Ƿ��о�ֵ����
		UInt32				invalid_time;			//!< ����ʱ��
		long long           item_unique_id;			//!< ����ΨһID
		short				param_length;			//!< ��ֵ���������ݳ��� ����has_paramΪ1ʱ�ɶ���
		NetValueItemParamData param_data;			//!< ��ֵ���������ݣ�����param_length��ֵ����ȡ ����has_paramΪ1ʱ�ɶ���
	};

	class SCUseItemSuc						//!< 1504 ֪ͨʹ��ĳ��Ʒ�ɹ�
	{
	public:
		SCUseItemSuc();
		MessageHeader		header;

		UInt16				item_id;		//!< ��ƷID
		short num;
	};

	class SCKnapsackExtend					//!< 1505 �������䷵��
	{
	public:
		SCKnapsackExtend();
		MessageHeader		header;

		short				column;			//!< ������ \see ITEM_COLUMN_TYPE_MAX
		short				grid_num;		//!< �µĸ�����
		short				extend_times;	//!< ��ǰ���������
		short				reserve_sh;		//!< ������
	};

	class SCLackItem						//!< 1506ȱ����Ʒ  \note ������Ʒ����ʱ���·�
	{
	public:
		SCLackItem();
		MessageHeader		header;

		int					item_id;		//!< ��Ʒid
		int					item_count;		//!< ����
	};

	class CSUseItem							//!< 1551 ʹ����Ʒ
	{
	public:
		CSUseItem();				
		MessageHeader		header;

		short				column;
		short				index;
		short				num;
		short				param1;
		short				param2;
		short				param3;

	};

	class SCItemIntoKnpackSpecialProtocal :public IMessageSerializer2		//��<1507 �����Ʒ����֪ͨ
	{
	public:
		SCItemIntoKnpackSpecialProtocal();
		MessageHeader		header;

		virtual bool Serialize(TLVSerializer2 * outstream) const;

		static const int MAX_FETCH_ITEM_NUM = 120;

		struct ItemInfo
		{
			void Reset()
			{
				item_id = 0;
				is_bind = 0;
				count = 0;
				has_param = 0;
				param_length = 0;
				memset(param_data, 0, sizeof(param_data));
			}

			ItemID			item_id;
			short			is_bind;
			int				count;
			short			reserve_sh;
			short			has_param;
			unsigned int	param_length;		//!< ��ֵ���������ݳ��� ����has_param��Ϊ0ʱ�ɶ���
			NetValueItemParamData param_data;	//!< ��ֵ���������ݣ�����param_length��ֵ����ȡ ����has_paramΪ1ʱ�ɶ���
		};
		
		short get_reson;
		short count;
		int param1;				//get_reason = 8, param1Ϊ����ID
		int param2;				//get_reason = 8, param2Ϊʹ������
		ItemInfo item_list[MAX_FETCH_ITEM_NUM];
	};

	class CSUseOptionalGift		//1552
	{

	public:
		CSUseOptionalGift();
		MessageHeader		header;

		static const int MAX_SELECT_ITEM = 64;

		struct SelectInfo
		{
			short index;
			short num;
		};

		short				column;
		short				index;
		int					num;
		SelectInfo select_info[MAX_SELECT_ITEM];
	};

	class SCTaskItemCommitNotice	//!< 1554 �ύ������Ʒ֪ͨ
	{
	public:
		SCTaskItemCommitNotice();
		MessageHeader header;

		struct ItemInfo
		{
			ItemID item_id;
			short num;
		};

		int count;		//!< ��Ʒ���� ������������
		ItemInfo item_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	};

	class SCTaskItemLackForCommitNotice		//!< 1553 ȱ�������ύ������Ʒ��֪ͨ
	{
	public:
		SCTaskItemLackForCommitNotice();
		MessageHeader header;

		struct ItemInfo
		{
			ItemID item_id;
			short num;
		};

		int count;		//!< ��Ʒ���� ������������
		ItemInfo item_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	};

	class CSUseOptionalGift2		//1555
	{

	public:
		CSUseOptionalGift2();
		MessageHeader		header;

		short				column;
		short				index;
		long long      select_index_flag;
	};

	class CSUseGoldRecovery		//!<1556 ʹ��Ԫ���ָ�
	{

	public:
		CSUseGoldRecovery();
		MessageHeader		header;

		int param1;
		int param2;
		int param3;
		int param4;
	};

	class SCUseRecoveryNotice		//!<1557 ʹ�ûָ�֪ͨ
	{

	public:
		SCUseRecoveryNotice();
		MessageHeader		header;

		unsigned short item_id;					
		unsigned short use_gold_type;
		int param1;
		int param2;
		int param3;
	};

	class SCKnapsackGetItemNotice		//!< 1558 �����õ���Ʈ��֪ͨ	
	{
	public:
		SCKnapsackGetItemNotice();
		MessageHeader header;

		enum NOTICE_TYPE
		{
			NOTICE_TYPE_BATCH_COMPOUND = 0,		//������������
			NOTICE_TYPE_LIFE_SKILL = 1,			//�����˫�� param1:NEW_LIFE_SKILL_TYPE
		};

		int notice_type;
		ItemID item_id;
		short num;	
		int param1;
	};
		
	class CSUseSpecialLoigcItem			//!<  1559  ʹ����Ҫ�����߼�����
	{
	public:
		CSUseSpecialLoigcItem();
		MessageHeader header;

		enum USE_SPECIAL_LOGIC_ITEM_TYPE
		{
			USE_SPECIAL_LOGIC_ITEM_TYPE_CRYSTAL_FB = 0,				// ʹ���ؾ�ҩ�� 
			USE_SPECIAL_LOGIC_ITEM_TYPE_BREAK_ITEM = 1,				// ʹ��������
		};

		int notice_type;
		ItemID item_id;
		short num;
	};

	class CSActiveSpecialLogicItem			//!< 1560 ���������߼�����
	{
	public:
		CSActiveSpecialLogicItem();
		MessageHeader header;
		
		enum ACTIVE_SPECIAL_LOGIC_ITEM_TYPE
		{
			ACTIVE_SPECIAL_LOGIC_ITEM_TYPE_STAR_TREASURE_MAP = 0,		//�����ǳ��ر�ͼ	param1:column param2:index
			ACTIVE_SPECIAL_LOGIC_ITEM_RESET_TREASURE_MAP_POS = 1,		//�ͻ��˼��ر�ͼ���겻��,�����������  param1:column param2:index
		};

		int notice_type;
		short param1;
		short param2;
	};

	class CSHearsayInfoReq					//!< 1561 �����·�����
	{
	public:
		CSHearsayInfoReq();
		MessageHeader header;

		enum HEARSAY_INFO_TYPE
		{
			HEARSAY_INFO_TYPE_TREASURE_MAP = 0,				//��ͼ���� param1:monster_id/reward_id	param2:use_item_id	param3:��������(is_notice)
		};

		int notice_type;
		int param1;
		int param2;
		int param3;
	};

	class CSBuyCmdGiftReq				//!< 1562 ������ֱ������(���)
	{
	public:
		CSBuyCmdGiftReq();
		MessageHeader header;

		int param1;			// BUY_GIFT_ITEM_TYPE_MAX
		int param2;			
		int param3;
	};

	class SCHongBaoUseRet 		//��<1563 ���ʹ�ý�������
	{
	public:
		SCHongBaoUseRet();
		MessageHeader		header;

		struct Item
		{
			int money_type;
			int num;
		};

		int count;
		Item item_list[100];
	};
}

#pragma pack(pop)

#endif





