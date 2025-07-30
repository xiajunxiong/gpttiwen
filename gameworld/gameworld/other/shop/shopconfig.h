#ifndef __SHOP_CONFIG_H__
#define __SHOP_CONFIG_H__

#include "servercommon/configcommon.h"
#include "servercommon/basedef.h"
#include "servercommon/shopdef.h"
#include "servercommon/roleconvertshopparam.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/struct/global/marketmanagerparamhpp.h"
#include <map>
#include <vector>

struct ShopItemCfg
{
	ShopItemCfg()
	{
		index = 0;
		shop_id = 0;
		item_id = 0;
		price = 0;
		money_type = 0;
		is_bind = 0;
		limit_type = 0;
		limit_times = 0;
		npc_seq = 0;
	}

	int index;
	int shop_id;
	int item_id;
	int price;
	int money_type;
	int is_bind;
	int limit_type;
	int limit_times;
	int npc_seq;
};

struct FastBuyContainer
{
	FastBuyContainer() : unit_price(0), currency_type(0), is_bind(0) {}

	int unit_price;
	int currency_type;
	int is_bind;
};

struct GoldShopItemCfg
{
	GoldShopItemCfg() :index(0), item_id(0), item_num(0), is_bind(0), price(0), money_type(0), limit_type(0), limit_times(0)
	{}

	int index;

	int item_id;
	int item_num;
	int is_bind;
	int price;
	int money_type;
	int limit_type;
	int limit_times;
};

struct ConvertShopItemCfg
{
	ConvertShopItemCfg() : price_type(0), price(0), conver_item_id(0), 
		require_type(0), require_value(0), limit_buy_times(0), 
		limit_type(0), version(0), open_day(0), last_seq(-1),
		is_cloud_arena_new_shop(0), feisheng_count(0) {}

	bool IsValid() { return item.item_id > 0; }

	ItemConfigData item;
	int price_type;
	int price;
	int conver_item_id;
	short require_type;
	short require_value;
	short limit_buy_times;
	short limit_type;
	short version;			// �������ù�����������
	short open_day;			// ��������һ��
	int last_seq;			// -1:û��ǰ��  >= 0 ������Ҫ�ȹ���ø�����Ʒ���ܹ���˵���(ȫ������)

	int is_cloud_arena_new_shop; // 0:��񷶥��Ʒ  1:��񷶥��Ʒ
	int feisheng_count;		// ��������:��ɫ��������
};

struct NewShopItemCfg
{
	NewShopItemCfg() : price_type(0), price(0), require_type(0), require_value(0), is_show(false) {}

	ItemConfigData item;
	int price_type;
	int price;
	short require_type;
	short require_value;
	bool is_show;
};

struct MarketItemCfg
{
	static const int BASE_NUM = 10000;

	MarketItemCfg() : conver_type(0), price_type(0), price(0), min_price(0), max_price(0), business_amount(0), float_range(0), require_type(0), 
		require_value(0), feisheng_count(0), version(0), is_show(false)
	{}

	int conver_type;
	ItemConfigData item;
	int price_type;
	int price;						//ԭ��
	int min_price;					//��ͼ�
	int max_price;					//��߼�
	int business_amount;			//��������
	int float_range;				//ʵ����������/�������� *���ֶ�,�ó�ʵ�ʲ������� ��ֱ�	
	short require_type;
	short require_value;
	int feisheng_count;				// ��������:��ɫ��������
	int version;					// ���
	bool is_show;					// false:��ʾֻ����������ture:�ȿ������ֿ�����
};

struct MarketOtherCfg
{
	MarketOtherCfg() : sell_price_scale(0), float_limit_max(0), float_limit_min(0), float_time_reset(0)
	{}

	int sell_price_scale;
	int float_limit_max;
	int float_limit_min;
	int float_time_reset;
};

struct ShopOtherCfg
{
	ShopOtherCfg() : cloud_arena_new_shop_timestamp(0u) {}

	unsigned int cloud_arena_new_shop_timestamp;
	int reserve;
};

// ------------------------------------------------------------

class ShopConfig : public ILogicConfig
{
public:
	static const int MAX_NEW_SHOP_ITEM_NUM = 200;

public:
	ShopConfig();
	virtual ~ShopConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ShopItemCfg * GetShopItemCfg(int index);
	const GoldShopItemCfg * GetGoldShopItemCfg(int index);
	const FastBuyContainer * GetFastBuyCfg(int item_id);
	const ConvertShopItemCfg* GetConvertShopItemCfg(int convert_type, int seq);
	const ConvertShopItemCfg* GetConvertShopItemCfgByItemID(int convert_type, ItemID item_id);
	const NewShopItemCfg* GetNewShopItemCfg(int index);
	int GetConvertTypeItemNum(int convert_type);
	void ResetBuyCount(int type, ShopParam * param);
	void ResetGoldBuyCount(int type, ShopParam * param);

	inline const ShopOtherCfg& GetShopOtherCfg() const { return m_shop_other_cfg; }
	const MarketItemCfg * GetMarketItemCfg(ItemID item_id);
	const MarketOtherCfg & GetMarketOtherCfg() { return m_market_other_cfg; }
private:
	int InitShopItemInfoCfg(TiXmlElement *RootElement);
	int InitShopNpcInfoCfg(TiXmlElement *RootElement);
	int InitShopFastBuyCfg(TiXmlElement *RootElement);
	int InitGoldShopBuyCfg(TiXmlElement *RootElement);
	int InitConvertShopCfg(TiXmlElement *RootElement);
	int InitNewShopItemInfoCfg(TiXmlElement *RootElement);
	int InitMarketItemCfg(TiXmlElement *RootElement);
	int InitMarketOtherCfg(TiXmlElement *RootElement);
	int InitShopOtherCfg(TiXmlElement *RootElement);

	std::map<int, ShopItemCfg> m_shop_item_map;
	std::map<int, std::vector<int> > m_shop_limit_map;

	std::map<int, int> m_shop_npc_cfg;
	std::map<int, FastBuyContainer> m_fast_buy_cfg;

	std::map<int, GoldShopItemCfg> m_gold_shop_item_map;
	std::map<int, std::vector<int> > m_gold_shop_limit_map;

	int m_convert_type_item_num_list[CONVERT_TYPE_MAX];
	ConvertShopItemCfg m_convert_shop_cfg_list[CONVERT_TYPE_MAX][CONVERT_TYPE_MAX_ITEM_COUNT];

	int m_new_shop_item_num;
	NewShopItemCfg m_new_shop_item_list[MAX_NEW_SHOP_ITEM_NUM];

	std::map<ItemID, MarketItemCfg> m_market_item_map;			//�г��������� key:item_id
	MarketOtherCfg m_market_other_cfg;

	ShopOtherCfg m_shop_other_cfg;
};

#endif	//__SHOP_CONFIG_H__
