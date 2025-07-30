#ifndef __ITEMGRIDDATA_H__
#define __ITEMGRIDDATA_H__

#include "servercommon/basedef.h"
#include "servercommon/serverdef.h"
#include "servercommon/struct/itemlistparam.h"
#include "itembase.h"

class ItemParam;

enum ITEM_MERGE_RESULT_TYPE
{
	ITEM_MERGE_CANNOT = 0,						// �����Ժϲ� des������������Ϊ99 ��������ԭ��
	ITEM_MERGE_COMPLETE,						// ��ȫ�ϲ� src����Ϊ��
	ITEM_MERGE_NOT_COMPLETE,					// ����ȫ�ϲ� des������Ϊ99 ��src����ʣ
};

// �����ⲿ��ȡItemGridData�����ݣ����ֲ���Ҫ�޸�������ʱ
struct ItemGridDataGetter
{
	ItemGridDataGetter() : item_id(0), num(0), is_bind(true), invalid_time(0), 
		gold_price(0), item_unique_id(0LL), is_market_buy_flag(0) {}
	
	~ItemGridDataGetter() {}

	ItemID		item_id;
	short		num;
	mutable bool	is_bind;
	UInt32		invalid_time;
	int			gold_price;
	NetValueItemParam param;
	long long	item_unique_id;		// ��ϵͳ�������ƷΨһID
	char		is_market_buy_flag;	//!<  �Ƿ���г������ʶ 0:�� 1:���г�����
};

struct ItemGridData									// ����ṹ�����paramָ�벻Ҫͨ���ṹ��������ͷ�
{
private:
	ItemGridData & operator = (const ItemGridData &rh)	// ���ṹ������ʹ�õȺ������
	{
		item_id = rh.item_id;
		num = rh.num;
		is_bind = rh.is_bind;
		invalid_time = rh.invalid_time;
		gold_price = rh.gold_price;
		param = rh.param;
		item_unique_id = rh.item_unique_id;
		is_market_buy_flag = rh.is_market_buy_flag;
		return *this; 
	}

public:

	ItemGridData() : item_id(0), num(0), is_bind(true), invalid_time(0), gold_price(0), param(0), item_unique_id(0LL), is_market_buy_flag(0) {}

	bool Set(const ItemDataWrapper &item_data_wrapper);
	void Set(const ItemGridData &data) { *this = data; }

	bool WrapTo(ItemDataWrapper *item_data_wrapper) const;
	bool WrapTo(ARG_OUT ItemGridDataGetter* getter) const;

	bool Invalid() const { return 0 == item_id; }

	void Clear();
	void Reset() { item_id = 0; num = 0; is_bind = false; invalid_time = 0; gold_price = 0; param = 0;  item_unique_id = 0;}

	bool CanBeMerge(bool ignore_invalid_time = false) const;
	bool CanMergeWith(const ItemDataWrapper &item_data_wrapper) const;
	int Merge(const ItemDataWrapper &iteminfo, int *rest_num, int *merged_gold_price, bool ignore_bind = false);
	int Merge(const ItemGridData &itemdata, int *rest_num, int *merged_gold_price, bool ignore_bind = false);

	ItemID		item_id;
	short		num;
	mutable bool	is_bind;
	UInt32		invalid_time;
	int			gold_price;
	NetValueItemParam* param;
	long long	item_unique_id;		// ��ϵͳ�������ƷΨһID
	char		is_market_buy_flag;	//!<  �Ƿ���г������ʶ 0:�� 1:���г�����
};

#endif
