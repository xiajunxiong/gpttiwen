#ifndef __MSG_TRADE_MARKET_H__
#define __MSG_TRADE_MARKET_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/global/trademarketparam.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	struct TradeItemDetail
	{
		ItemID item_id;				//!< ��ƷID 
		short item_num;				//!< ��Ʒ����
		unsigned int invalid_time;	//!< ��Ʒ����ʱ���
		short param_length;			//!< param����
		NetValueItemParamData data;	//!< param����
	};

	class CSAddItemOnShelve			//!< 2301 �ϼܻ���
	{
	public:
		CSAddItemOnShelve();
		MessageHeader header;

		char is_from_pet;			//!< �Ƿ���� [0:��  1:��]
		char reserve_ch;			//!< ������
		short index;				//!< ��� ����Ϊ����ʱ���ǻ����ڳ��ﱳ������ţ������ǻ����ڱ�������ţ�
		short column;				//!< ��Ʒ���� �����������ǵ���ʱ��Ч����ʾ�����ڱ����е��ĸ���Ʒ����
		short item_num;				//!< �ϼܻ��������
		int price;					//!< ����
	};

	class CSRemoveItemOnShelve		//!< 2302 �¼ܻ���
	{
	public:
		CSRemoveItemOnShelve();
		MessageHeader header;

		int sale_index;				//!< �����ڻ����ϵ���� [0,8)
	};

	class CSTradeMarketCashOut		//!< 2303 ����
	{
	public:
		CSTradeMarketCashOut();
		MessageHeader header;

		int sale_index;				//!< �����ڻ����ϵ���� [0,8)
	};

	class SCTradeMarketUserShelveData : public IMessageSerializer	//!< 2351 ��ҽ�������������
	{
	public:
		SCTradeMarketUserShelveData();
		MessageHeader header;

		enum REASON
		{
			REASON_NORMAL = 0,
			REASON_NOTIFY_CHANGE = 1,
			REASON_NOTIFY_ADD = 2,
			REASON_NOTIFY_REMOVE = 3,
		};

		struct SaleItemInfo
		{
			short sale_index;				//!< ������� [0,8)
			short cur_status;				//!< ��ǰ״̬ \see SALE_ITEM_STATUS
			unsigned int next_status_time;	//!< �л��¸�״̬��ʱ���

			short sold_num;					//!< �������� 
			short price_type;				//!< �Ժ��ֻ��ҽ��� \see ITEM_TRADE_MONEY_TYPE
			int price;						//!< ����

			TradeItemDetail item_detail;	//!< ��Ʒ��ϸ����
		};

		virtual int GetMessageType() const { return header.msg_type; }

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteShort(reason, buffer, buffer_size, out_length);
			result = result && WriteShort(item_count, buffer, buffer_size, out_length);
			for (int i = 0; i < item_count; ++i)
			{
				result = result && WriteShort(item_list[i].sale_index, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].cur_status, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].next_status_time, buffer, buffer_size, out_length);

				result = result && WriteShort(item_list[i].sold_num, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].price_type, buffer, buffer_size, out_length);
				result = result && WriteInt(item_list[i].price, buffer, buffer_size, out_length);

				result = result && WriteUShort(item_list[i].item_detail.item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].item_detail.item_num, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].item_detail.invalid_time, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].item_detail.param_length, buffer, buffer_size, out_length);
				result = result && WriteStrN(item_list[i].item_detail.data, item_list[i].item_detail.param_length, buffer, buffer_size, out_length);
			}

			return result;
		}

		short reason;									//!< ԭ��,  0:����������Ϣ�·�  1:�ı�֪ͨ  2:����  3��ɾ��
		short item_count;								//!< �����ϻ��������
		SaleItemInfo item_list[USER_SALE_ITEM_MAX_NUM];	//!< �����ϻ������Ϣ�����鳤����item_countΪ׼  \see SaleItemInfo
	};

	class CSTradeMarketUserShelveData		//!< 2304 ��Ҹ��˽�������������
	{
	public:
		CSTradeMarketUserShelveData();
		MessageHeader header;
	};

	class CSTradeMarketSearchByItemID   //!< 2305 ����ƷID����
	{
	public:
		CSTradeMarketSearchByItemID();
		MessageHeader header;

		ItemID item_id;					//!< ��ƷID
		short req_page;					//!< �����ҳ��

		short is_announce;				//!< �Ƿ�������ڹ�ʾ�ڵ� 0����   1����
		short order_type;				//!< ��������  0:���۸�͵���   1:���۸�ߵ���
	};

	class CSTradeMarketSearchByItemSearchType	//!< 2306 ����Ʒ�������ͼ���
	{
	public:
		CSTradeMarketSearchByItemSearchType();
		MessageHeader header;

		short item_search_type;			//!< ��Ʒ�������� \note �� J-������.xls -> ɸѡsheet
		short req_page;					//!< �����ҳ��

		short is_announce;				//!< �Ƿ�������ڹ�ʾ�ڵ� 0����   1����
		short order_type;				//!< ��������  0:���۸�͵���   1:���۸�ߵ���

		short item_big_search_type;		//!< ֵΪ-1ʱ����С��(item_search_type)�����������򰴴�������
		short param;					//!< ���������������װ��ʱparam����ȼ�������������ʱparam������Чְҵ��(Ϊ-1ʱ��"ȫ��"����)
	};

	class SCTradeMarketSearchResult : public IMessageSerializer	//!< 2352 �����������
	{
	public:
		SCTradeMarketSearchResult();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		struct SaleItemInfo
		{
			unsigned int next_status_time;	//!< �¸�״̬ʱ��
			short cur_item_status;			//!< ��ǰ��Ʒ״̬	\see SALE_ITEM_STATUS
			short price_type;				//!< �Ժ��ֻ��ҽ��� \see ITEM_TRADE_MONEY_TYPE
			int price;						//!< ����
			TradeItemDetail item_detail;	//!< ��Ʒ��ϸ����
			int seller_uid;					//!< ����uid					\note ���ڹ�������
			short sale_index;				//!< ��Ʒ�����һ����ϵ����		\note ���ڹ�������
			unsigned int upshelve_time;		//!< ��Ʒ�ϼ�ʱ��				\note ���ڹ�������
			UniqueKeyStr unique_key_str;	//!< Ψһ��ʶ�ַ���				\note ���ڹ���ϵͳ���ܵ���Ʒ
		};

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteShort(cursor_index, buffer, buffer_size, out_length);
			result = result && WriteShort(is_announce, buffer, buffer_size, out_length);
			result = result && WriteShort(cur_page, buffer, buffer_size, out_length);
			result = result && WriteShort(total_page, buffer, buffer_size, out_length);
			result = result && WriteShort(order_type, buffer, buffer_size, out_length);
			result = result && WriteShort(item_count, buffer, buffer_size, out_length);

			for (int i = 0; i < item_count; ++i)
			{
				result = result && WriteUInt(item_list[i].next_status_time, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].cur_item_status, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].price_type, buffer, buffer_size, out_length);
				result = result && WriteInt(item_list[i].price, buffer, buffer_size, out_length);

				result = result && WriteUShort(item_list[i].item_detail.item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].item_detail.item_num, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].item_detail.invalid_time, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].item_detail.param_length, buffer, buffer_size, out_length);
				result = result && WriteStrN(item_list[i].item_detail.data, item_list[i].item_detail.param_length, buffer, buffer_size, out_length);

				result = result && WriteInt(item_list[i].seller_uid, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].sale_index, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].upshelve_time, buffer, buffer_size, out_length);
				result = result && WriteStrN(item_list[i].unique_key_str, sizeof(item_list[i].unique_key_str), buffer, buffer_size, out_length);
			}

			return result;
		}

		short cursor_index;					//!< ����׼�Ļ�Ʒ���
		short is_announce;					//!< �Ƿ�������ڹ�ʾ�ڵ� 0����   1����
		short cur_page;						//!< ��ǰҳ
		short total_page;					//!< ��ҳ��
		short order_type;					//!< ��������  0:���۸�͵���   1:���۸�ߵ���
		short item_count;					//!< ��ҳ�ж��ٻ�Ʒ
		SaleItemInfo item_list[SEARCH_SALE_ITEM_PER_PAGE];	//!< ���鳤����item_countΪ׼  \see SaleItemInfo

	};

	class CSTradeMarketBuyItem				//!< 2307 ������Ʒ
	{
	public:
		CSTradeMarketBuyItem();
		MessageHeader header;

		int seller_uid;						//!< ����uid
		short sale_index;					//!< �����һ����ϵ����
		short buy_num;						//!< ��������
		unsigned int upshelve_time;			//!< ��Ʒ�ϼ�ʱ�� �����ں˶���Ʒ��
		ItemID buy_item_id;					//!< �������ƷID �����ں˶���Ʒ�Լ��Զ���������
		short reserve_sh;					//!< ������
		int expect_price;					//!< Ԥ�ڵļ۸������Զ�����ʱ���˶��Ƿ񳬹�Ԥ�ڼ۸�
		UniqueKeyStr unique_key_str;		//!< Ψһ��ʶ�ַ���
	};

	class CSRecentTransactionRecords		//!< 2308 ��ѯ������׼�¼
	{
	public:
		CSRecentTransactionRecords();
		MessageHeader header;

		ItemID item_id;						//!< ��ƷID
		short reserve_sh;					//!< ������
	};

	struct ProtocolTransactionItemInfo
	{
		ProtocolTransactionItemInfo() : item_id(0), item_num(0), price_type(0), data_len(0), price(0) {}

		void Reset()
		{
			item_id = 0;
			item_num = 0;
			price_type = 0;
			data_len = 0;
			price = 0;
			memset(data, 0, sizeof(data));
		}

		ItemID item_id;			//!< ��ƷID
		short item_num;			//!< ��������
		short price_type;		//!< �۸����� 
		short data_len;			//!< ��ֵ���ݳ���
		int price;				//!< ����
		NetValueItemParamData data;	//!< ��ֵ����
	};

	class SCRecentTransactionRecords : public IMessageSerializer	//!< 2353 ����������׼�¼
	{
	public:
		SCRecentTransactionRecords();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;
			result = WRITE_BEGIN(header.msg_type);

			result = result && WRITE_INT(count);
			for (int i = 0; i < count; ++i)
			{
				result = result && WRITE_USHORT(record_list[i].item_id);
				result = result && WRITE_SHORT(record_list[i].item_num);
				result = result && WRITE_SHORT(record_list[i].price_type);
				result = result && WRITE_SHORT(record_list[i].data_len);
				result = result && WRITE_INT(record_list[i].price);
				result = result && WRITE_STRN(record_list[i].data, record_list[i].data_len);
			}

			return result;
		}

		int count;							//!< ��¼����
		ProtocolTransactionItemInfo record_list[TRANSACTION_RECORD_MAX_NUM];	//!< ��¼�б�
	};

	class CSTradeMarketGetFocusList			//!< 2309 ��ѯ��ע�б�
	{
	public:
		CSTradeMarketGetFocusList();
		MessageHeader header;
	};

	struct FocusItemInfo
	{
		unsigned int next_status_time;	//!< �¸�״̬ʱ��
		short cur_item_status;			//!< ��ǰ��Ʒ״̬	\see SALE_ITEM_STATUS
		short price_type;				//!< �Ժ��ֻ��ҽ��� \see ITEM_TRADE_MONEY_TYPE
		int price;						//!< ����
		TradeItemDetail item_detail;	//!< ��Ʒ��ϸ����
		int seller_uid;					//!< ����uid					\note ���ڹ�������
		short sale_index;				//!< ��Ʒ�����һ����ϵ����		\note ���ڹ�������
		short focus_index;				//!< ��Ʒ���ҵĹ�ע�б��е���� \note ����ȡ����עʱ�������
		unsigned int upshelve_time;		//!< ��Ʒ�ϼ�ʱ��				\note ���ڹ�������
		UniqueKeyStr unique_key_str;	//!< Ψһ��ʶkey
	};

	class SCTradeMarketGetFocusList : public IMessageSerializer	//!< 2354 ��ȡ��ע�б�
	{
	public:
		SCTradeMarketGetFocusList();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteShort(announce_count, buffer, buffer_size, out_length);
			result = result && WriteShort(sale_count, buffer, buffer_size, out_length);

			for (int i = 0; i < announce_count && i < SEARCH_SALE_ITEM_PER_PAGE; ++i)
			{
				result = result && WriteUInt(announce_focus_list[i].next_status_time, buffer, buffer_size, out_length);
				result = result && WriteShort(announce_focus_list[i].cur_item_status, buffer, buffer_size, out_length);
				result = result && WriteShort(announce_focus_list[i].price_type, buffer, buffer_size, out_length);
				result = result && WriteInt(announce_focus_list[i].price, buffer, buffer_size, out_length);

				result = result && WriteUShort(announce_focus_list[i].item_detail.item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(announce_focus_list[i].item_detail.item_num, buffer, buffer_size, out_length);
				result = result && WriteUInt(announce_focus_list[i].item_detail.invalid_time, buffer, buffer_size, out_length);
				result = result && WriteShort(announce_focus_list[i].item_detail.param_length, buffer, buffer_size, out_length);
				result = result && WriteStrN(announce_focus_list[i].item_detail.data, announce_focus_list[i].item_detail.param_length, buffer, buffer_size, out_length);

				result = result && WriteInt(announce_focus_list[i].seller_uid, buffer, buffer_size, out_length);
				result = result && WriteShort(announce_focus_list[i].sale_index, buffer, buffer_size, out_length);
				result = result && WriteShort(announce_focus_list[i].focus_index, buffer, buffer_size, out_length);
				result = result && WriteUInt(announce_focus_list[i].upshelve_time, buffer, buffer_size, out_length);
				result = result && WriteStrN(announce_focus_list[i].unique_key_str, sizeof(announce_focus_list[i].unique_key_str), buffer, buffer_size, out_length);
			}

			for (int i = 0; i < sale_count && i < SEARCH_SALE_ITEM_PER_PAGE; ++i)
			{
				result = result && WriteUInt(sale_focus_list[i].next_status_time, buffer, buffer_size, out_length);
				result = result && WriteShort(sale_focus_list[i].cur_item_status, buffer, buffer_size, out_length);
				result = result && WriteShort(sale_focus_list[i].price_type, buffer, buffer_size, out_length);
				result = result && WriteInt(sale_focus_list[i].price, buffer, buffer_size, out_length);

				result = result && WriteUShort(sale_focus_list[i].item_detail.item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(sale_focus_list[i].item_detail.item_num, buffer, buffer_size, out_length);
				result = result && WriteUInt(sale_focus_list[i].item_detail.invalid_time, buffer, buffer_size, out_length);
				result = result && WriteShort(sale_focus_list[i].item_detail.param_length, buffer, buffer_size, out_length);
				result = result && WriteStrN(sale_focus_list[i].item_detail.data, sale_focus_list[i].item_detail.param_length, buffer, buffer_size, out_length);

				result = result && WriteInt(sale_focus_list[i].seller_uid, buffer, buffer_size, out_length);
				result = result && WriteShort(sale_focus_list[i].sale_index, buffer, buffer_size, out_length);
				result = result && WriteShort(sale_focus_list[i].focus_index, buffer, buffer_size, out_length);
				result = result && WriteUInt(sale_focus_list[i].upshelve_time, buffer, buffer_size, out_length);
				result = result && WriteStrN(sale_focus_list[i].unique_key_str, sizeof(sale_focus_list[i].unique_key_str), buffer, buffer_size, out_length);
			}

			return true;
		}

		short announce_count;											//!< ��ʾ������
		short sale_count;												//!< ����������
		FocusItemInfo announce_focus_list[SEARCH_SALE_ITEM_PER_PAGE];	//!< ��ʾ�ڹ�ע�б�
		FocusItemInfo sale_focus_list[SEARCH_SALE_ITEM_PER_PAGE];		//!< ��ע�б�
	};

	class CSTradeMarketAddFocus				//!< 2310 ��ӹ�ע
	{
	public:
		CSTradeMarketAddFocus();
		MessageHeader header;

		int seller_uid;						//!< ����uid
		short sale_index;					//!< �����һ����ϵ����
		short reserve_sh;					//!< ������
		unsigned int upshelve_time;			//!< ��Ʒ�ϼ�ʱ�� �����ں˶���Ʒ��
	};

	class CSTradeMarketRemoveFocus			//!< 2311 ȡ����ע
	{
	public:
		CSTradeMarketRemoveFocus();
		MessageHeader header;

		int focus_index;					//!< ��Ʒ���ҵĹ�ע�б��е����  \note ���SCTradeMarketGetFocusList�е�focus_index
		short cur_page;						//!< ��ǰ���������һҳ
		short is_announce;					//!< ��ǰ����������ǹ�ʾ�ڻ��Ƿ����ڵĹ�ע�б�
	};

	class CSTradeMarketGetFocusItemInfo		//!< 2312 ��ȡ��ע��Ʒ����������
	{
	public:
		CSTradeMarketGetFocusItemInfo();
		MessageHeader header;

		int focus_index;					//!< ��Ʒ���ҵĹ�ע�б��е����  \note ���SCTradeMarketGetFocusList�е�focus_index
		short cur_page;						//!< ��ǰ���������һҳ
		short is_announce;					//!< ��ǰ����������ǹ�ʾ�ڻ��Ƿ����ڵĹ�ע�б�
	};

	class SCTradeMarketGetFocusItemInfo : public IMessageSerializer	//!< 2355 ���ع�ע��Ʒ����������
	{
	public:
		SCTradeMarketGetFocusItemInfo();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteUInt(info.next_status_time, buffer, buffer_size, out_length);
			result = result && WriteShort(info.cur_item_status, buffer, buffer_size, out_length);
			result = result && WriteShort(info.price_type, buffer, buffer_size, out_length);
			result = result && WriteInt(info.price, buffer, buffer_size, out_length);

			result = result && WriteUShort(info.item_detail.item_id, buffer, buffer_size, out_length);
			result = result && WriteShort(info.item_detail.item_num, buffer, buffer_size, out_length);
			result = result && WriteUInt(info.item_detail.invalid_time, buffer, buffer_size, out_length);
			result = result && WriteShort(info.item_detail.param_length, buffer, buffer_size, out_length);
			result = result && WriteStrN(info.item_detail.data, info.item_detail.param_length, buffer, buffer_size, out_length);

			result = result && WriteInt(info.seller_uid, buffer, buffer_size, out_length);
			result = result && WriteShort(info.sale_index, buffer, buffer_size, out_length);
			result = result && WriteShort(info.focus_index, buffer, buffer_size, out_length);
			result = result && WriteUInt(info.upshelve_time, buffer, buffer_size, out_length);
			result = result && WriteStrN(info.unique_key_str, sizeof(info.unique_key_str), buffer, buffer_size, out_length);

			return result;
		}

		FocusItemInfo info;					//!< ��ע��Ʒ����������
	};

	class CSTradeMarketAdvertise	//!< 2313 ����
	{
	public:
		CSTradeMarketAdvertise();
		MessageHeader header;

		short sale_index;			//!< ��Ҫ����Ļ�Ʒ�����
		short reserve_sh;			//!< ������
	};

	class CSTradeMarketSearchAdvertise		//!< 2314 ���ҹ���ϵ���Ʒ \note ����������ڹ�������·�
	{
	public:
		CSTradeMarketSearchAdvertise();
		MessageHeader header;

		int seller_uid;						//!< ����uid
		unsigned int put_on_shelve_time;	//!< �ϼ�ʱ��
		int price;							//!< �۸�
		short sale_index;					//!< �ڻ����ϵ����
		ItemID item_id;						//!< ��ƷID
		UniqueKeyStr unique_key_str;		//!< Ψһ��ʶ�ַ���
	};

	class CSTradeMarketGetSellingList		//!< 2315 ��ѯ����������ĳ��Ʒ
	{
	public:
		CSTradeMarketGetSellingList();
		MessageHeader header;

		ItemID item_id;						//!< ��ƷID
		short special_effect_type;			//!< ��Ч���ͣ�������ʱʹ�ã�
	};

	struct MsgTransactionInfo
	{
		MsgTransactionInfo() : item_id(0), item_num(0), price_type(0), data_len(0), price(0)
		{
			memset(data, 0, sizeof(data));
		}

		void Reset()
		{
			item_id = 0;
			item_num = 0;
			price_type = 0;
			data_len = 0;
			price = 0;
		}

		ItemID item_id;			//!< ��ƷID
		short item_num;			//!< ��������
		short price_type;		//!< �۸����� 
		short data_len;			//!< ��ֵ���ݳ���
		int price;				//!< ����
		NetValueItemParamData data;	//!< ��ֵ����
	};

	class SCTradeMarketGetSellingList : public IMessageSerializer	//!< 2356 ��������������ĳ��Ʒ�б�
	{
	public:
		SCTradeMarketGetSellingList();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;
			result = WRITE_BEGIN(header.msg_type);

			result = result && WRITE_INT(count);
			for (int i = 0; i < count; ++i)
			{
				result = result && WRITE_USHORT(selling_list[i].item_id);
				result = result && WRITE_SHORT(selling_list[i].item_num);
				result = result && WRITE_SHORT(selling_list[i].price_type);
				result = result && WRITE_SHORT(selling_list[i].data_len);
				result = result && WRITE_INT(selling_list[i].price);
				result = result && WRITE_STRN(selling_list[i].data, selling_list[i].data_len);
			}

			return result;
		}

		int count;							//!< ����
		ProtocolTransactionItemInfo selling_list[TRANSACTION_RECORD_MAX_NUM];	//!< �б�
	};

	class CSTradeMarketGetRoleTransactionRecord	//!< 2316 �����ȡ��ҽ��׼�¼
	{
	public:
		CSTradeMarketGetRoleTransactionRecord();
		MessageHeader header;
	};

	class SCTradeMarketGetRoleTransactionRecord : public IMessageSerializer //!< 2357 ��ȡ��ҽ��׼�¼����  \note decode��ο������Serialize����
	{
	public:
		SCTradeMarketGetRoleTransactionRecord();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;
			result = WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteShort(sold_record_num, buffer, buffer_size, out_length);
			result = result && WriteShort(buy_record_num, buffer, buffer_size, out_length);

			for (int i = 0; i < sold_record_num; ++i)
			{
				result = result && WriteUShort(sold_list[i].item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(sold_list[i].item_num, buffer, buffer_size, out_length);
				result = result && WriteShort(sold_list[i].price_type, buffer, buffer_size, out_length);
				result = result && WriteShort(sold_list[i].data_len, buffer, buffer_size, out_length);
				result = result && WriteInt(sold_list[i].price, buffer, buffer_size, out_length);
				result = result && WRITE_STRN(sold_list[i].data, sold_list[i].data_len);
			}

			for (int i = 0; i < buy_record_num; ++i)
			{
				result = result && WriteUShort(buy_list[i].item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(buy_list[i].item_num, buffer, buffer_size, out_length);
				result = result && WriteShort(buy_list[i].price_type, buffer, buffer_size, out_length);
				result = result && WriteShort(buy_list[i].data_len, buffer, buffer_size, out_length);
				result = result && WriteInt(buy_list[i].price, buffer, buffer_size, out_length);
				result = result && WRITE_STRN(buy_list[i].data, buy_list[i].data_len);
			}

			result = WRITE_LL(total_sold_gold);
			result = WRITE_LL(total_buy_gold);

			return result;
		}

		short sold_record_num;
		short buy_record_num;
		ProtocolTransactionItemInfo sold_list[ROLE_TRANSACTION_RECORD_MAX_NUM];
		ProtocolTransactionItemInfo buy_list[ROLE_TRANSACTION_RECORD_MAX_NUM];

		long long total_sold_gold;
		long long total_buy_gold;
	};

	class CSTradeMarketRedoAddItemOnShelve		//!< 2317 �����ϼ�
	{
	public:
		CSTradeMarketRedoAddItemOnShelve();
		MessageHeader header;

		short sale_index;						//!< �ļ���Ʒ��Ҫ�����ϼ� [0,8)	
		short reserve_sh;						//!< ������
		int price;								//!< ����
	};

	class CSTradeMarketSearchPetForQuickBuy		//!< 2318 �����ݹ�����������
	{
	public:
		CSTradeMarketSearchPetForQuickBuy();
		MessageHeader header;

		short order_type;						//!< ɸѡ˳�� \see TRADE_PET_INDEX_ORDER
		ItemID item_id;							//!< Ҫ�ѵĳ�����ƷID

		int last_seller_uid;					//!< �ϴ��ѵ������һ����Ʒ������UID ����һ�����������ɸѡ˳��ʱ��0��
		int last_price;							//!< �ϴ��ѵ������һ����Ʒ�ļ۸�	 ����һ�����������ɸѡ˳��ʱ��0��
		unsigned int last_put_on_shelve_time;	//!< �ϴ��ѵ������һ����Ʒ���ϼ�ʱ�䣨��һ�����������ɸѡ˳��ʱ��0��
		short last_str_level;					//!< �ϴ��ѵ������һ����Ʒ��ǿ���ȼ�����һ�����������ɸѡ˳��ʱ��0��
		short quality;							//!< ����IDΪ0ʱʹ�������������Ʒ�ʲ��ҳ���
	};

	class SCTradeMarketSearchPetForQuickBuy : public IMessageSerializer	//!< 2358 �����ݹ�����������
	{
	public:
		SCTradeMarketSearchPetForQuickBuy();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		struct SaleItemInfo
		{
			unsigned int next_status_time;	//!< �¸�״̬ʱ��
			short cur_item_status;			//!< ��ǰ��Ʒ״̬	\see SALE_ITEM_STATUS
			short price_type;				//!< �Ժ��ֻ��ҽ��� \see ITEM_TRADE_MONEY_TYPE
			int price;						//!< ����
			TradeItemDetail item_detail;	//!< ��Ʒ��ϸ����
			int seller_uid;					//!< ����uid					\note ���ڹ�������
			short sale_index;				//!< ��Ʒ�����һ����ϵ����		\note ���ڹ�������
			unsigned int upshelve_time;		//!< ��Ʒ�ϼ�ʱ��				\note ���ڹ�������
			UniqueKeyStr unique_key_str;	//!< Ψһ��ʶ�ַ���				\note ���ڹ���ϵͳ���ܵ���Ʒ	
		};

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteShort(order_type, buffer, buffer_size, out_length);
			result = result && WriteShort(item_count, buffer, buffer_size, out_length);
			for (int i = 0; i < item_count; ++i)
			{
				result = result && WriteUInt(item_list[i].next_status_time, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].cur_item_status, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].price_type, buffer, buffer_size, out_length);
				result = result && WriteInt(item_list[i].price, buffer, buffer_size, out_length);

				result = result && WriteUShort(item_list[i].item_detail.item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].item_detail.item_num, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].item_detail.invalid_time, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].item_detail.param_length, buffer, buffer_size, out_length);
				result = result && WriteStrN(item_list[i].item_detail.data, item_list[i].item_detail.param_length, buffer, buffer_size, out_length);

				result = result && WriteInt(item_list[i].seller_uid, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].sale_index, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].upshelve_time, buffer, buffer_size, out_length);
				result = result && WriteStrN(item_list[i].unique_key_str, sizeof(item_list[i].unique_key_str), buffer, buffer_size, out_length);
			}

			return result;
		}

		short order_type;					//!< ɸѡ˳�� \see TRADE_PET_INDEX_ORDER
		short item_count;					//!< �ж���������
		SaleItemInfo item_list[SEARCH_PET_QUICK_BUY_ITEM_NUM];	//!< ���鳤����item_countΪ׼  \see SaleItemInfo
	};

	class SCTradeMarketRefreshNotice	// 2359 ֪ͨ�ͻ��˸��������б�
	{
	public:
		SCTradeMarketRefreshNotice();
		MessageHeader header;

	};

	class CSTradeMarketGetMerchandiseNum	//!< 2319 ����ÿ������������С��Ŀǰ�ж�����Ʒ
	{
	public:
		CSTradeMarketGetMerchandiseNum();
		MessageHeader header;

		short is_announce;		//!< 0:������   1:��ʾ��
		short big_search_type;	//!< ����  1������	2������  3������  10������
	};

	class SCTradeMarketGetMerchandiseNum	//!< 2360 ����ÿ������������С��Ŀǰ�ж�����Ʒ
	{
	public:
		SCTradeMarketGetMerchandiseNum();
		MessageHeader header;

		struct MerchandiseInfo
		{
			int small_search_type;	// С��
			int merchandise_num;	// ��Ʒ����
		};

		short is_announce;			//!< 0:������  1:��ʾ��
		short big_search_type;		//!< ����  ԭ������
		int num;					//!< MerchandiseInfo����
		MerchandiseInfo info_list[TRADE_ITEM_SEARCH_TYPE_MAX];
	};

	class SCTradeMarketCreditInfo	//!< 2361 ����������Ϣ�·�
	{
	public:
		SCTradeMarketCreditInfo();
		MessageHeader header;

		long long total_credit;		//!< ��ǰ����ӵ�е�����ֵ
	};

	class CSTradeMarketSearchMedalByEffectTypeAndColor   //!< 2320 ����Ч���ͺ���ɫ������
	{
	public:
		CSTradeMarketSearchMedalByEffectTypeAndColor();
		MessageHeader header;

		short sp_type;					//!< ��Ч����
		short color;					//!< ��ɫ
		short reserve_sh;				//!< ������

		short req_page;					//!< �����ҳ��
		short is_announce;				//!< �Ƿ�������ڹ�ʾ�ڵ� 0����   1����
		short order_type;				//!< ��������  0:���۸�͵���   1:���۸�ߵ���
	};

	class CSQueryItemRecommendPrice		//!< 2321 ��ѯ��Ʒ�Ƽ��۸�
	{
	public:
		CSQueryItemRecommendPrice();
		MessageHeader header;

		char is_from_pet;			//!< �Ƿ���� [0:��  1:��]
		char reserve_ch;			//!< ������
		short reserve_sh;			//!< ������
		short index;				//!< ��� ����Ϊ����ʱ���ǻ����ڳ��ﱳ������ţ������ǻ����ڱ�������ţ�
		short column;				//!< ��Ʒ���� �����������ǵ���ʱ��Ч����ʾ�����ڱ����е��ĸ���Ʒ����
	};

	class SCQueryItemRecommendPriceResp	//!< 2362 ��ѯ��Ʒ�Ƽ��۸񷵻�
	{
	public:
		SCQueryItemRecommendPriceResp();
		MessageHeader header;

		int price;					//!< �Ƽ��۸�
		int min_price;				//!< ��ͼ۸�
		int max_price;				//!< ��߼۸�

		// �����ֶ�ԭ������
		char is_from_pet;			//!< �Ƿ���� [0:��  1:��]
		char reserve_ch;			//!< ������
		short reserve_sh;			//!< ������
		short index;				//!< ��� ����Ϊ����ʱ���ǻ����ڳ��ﱳ������ţ������ǻ����ڱ�������ţ�
		short column;				//!< ��Ʒ���� �����������ǵ���ʱ��Ч����ʾ�����ڱ����е��ĸ���Ʒ����
	};
}

#pragma pack(pop)

#endif