#ifndef __RAND_ACTIVITY_HORCRUX_TIANCHENG_CONFIG_HPP__
#define __RAND_ACTIVITY_HORCRUX_TIANCHENG_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"
#include "servercommon/roleactivity/rahorcruxtianchengparam.hpp"

struct RAHorcruxTianChangGiftCfg
{
	RAHorcruxTianChangGiftCfg() : seq(0), reward_type(0), gold_num(0), reward_per(0), hearsay(0), guarantee_times(0) {}

	short seq;
	short reward_type;					// 奖励类型
	int gold_num;
	ItemConfigData reward_item;
	short reward_per;
	short hearsay;
	int guarantee_times;
};

struct RAHorcruxTianChengJackpotCfg
{
	RAHorcruxTianChengJackpotCfg() : jackpot_type(0), item_id(0), start_num(0), all_draw_times(0), add_item_num(0) {}

	short jackpot_type;
	unsigned short item_id;
	int start_num;						// 奖池初始道具数量
	short all_draw_times;				// 全服抽奖次数
	short add_item_num;					// 增加道具数量
};

struct RAHorcruxTianChengShopCfg
{
	RAHorcruxTianChengShopCfg() : limit_type(0), limit_times(0), price_type(0), buy_money(0) {}

	short limit_type;					// 限购类型
	short limit_times;					// 限购次数
	short price_type;					// 货币类型
	int buy_money;						// 购买金额

	std::vector<ItemConfigData> reward_item;
};

struct RAHorcruxTianChengExchangeCfg
{
	RAHorcruxTianChengExchangeCfg() : gold(0), jade(0), buy_type(0), limit_times(0) {}

	ItemConfigData exchange_item;
	std::vector<ItemConfigData> consume_item;			// 消耗物品
	int gold;											// 消耗元宝
	int jade;											// 消耗灵玉

	short buy_type;										// 限购类型
	short limit_times;									// 限购数量
};

struct RAHorcruxTianChengCfgMap
{
	RAHorcruxTianChengCfgMap () : rete_max(0) {}

	int rete_max;
	std::map<int, RAHorcruxTianChangGiftCfg> gift_cfg;
};

struct RAHorcruxTianChengOtherCfg
{
	RAHorcruxTianChengOtherCfg() : item_id(0), item_num(0), is_open_exchange(false), cfg_ver(0) {}

	short item_id;
	short item_num;
	bool is_open_exchange;
	short cfg_ver;
};

struct RAHorcruxTianChengConsumeCfg
{
	RAHorcruxTianChengConsumeCfg() : consume_num(0) {}

	int consume_num;
	std::vector<ItemConfigData> item;
};

class RandActivityManager;

class RandActivityHorcruxTianChengConfig : public RandActivityConfig
{
public:
	RandActivityHorcruxTianChengConfig();
	virtual ~RandActivityHorcruxTianChengConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAHorcruxTianChangGiftCfg * GetGiftCfg(RandActivityManager * ramgr, int check_floor, const BitMap<MAX_HORCRUX_TIANCHENG_REWARD_GROUP_NUM>* bit_map, bool *floor_back) const;
	const RAHorcruxTianChengJackpotCfg * GetJackpotCfg(RandActivityManager * ramgr) const;
	const RAHorcruxTianChengShopCfg * GetShopCfg(RandActivityManager * ramgr, int shop_index) const;
	const RAHorcruxTianChengExchangeCfg * GetExchangeCfg(RandActivityManager * ramgr, int exchange_index) const;
	const RAHorcruxTianChengOtherCfg & GetOtherCfg() const { return m_other_cfg; }

	const std::vector<RAHorcruxTianChengShopCfg> * GetShopMapCfg(RandActivityManager * ramgr) const;
	const std::vector<RAHorcruxTianChengExchangeCfg> * GetExchangeMapCfg(RandActivityManager * ramgr) const;

	const RAHorcruxTianChengConsumeCfg * GetConsumeCfg(RandActivityManager * ramgr, int seq) const;
	int GetConsumeSize(RandActivityManager * ramgr) const;

private:
	int InitGiftCfg(TiXmlElement *RootElement);
	int InitJackpotCfg(TiXmlElement *RootElement);
	int InitShopCfg(TiXmlElement *RootElement);
	int InitExchangeCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitConsumeCfg(TiXmlElement *RootElement);

	std::map<int, std::map<int, RAHorcruxTianChangGiftCfg*> > m_cfg_floor_map;
	std::map<int, RAHorcruxTianChengCfgMap> m_cfg_map;
	std::map<int, RAHorcruxTianChengJackpotCfg> m_jackpot_cfg;
	std::map<int, std::vector<RAHorcruxTianChengShopCfg> > m_shop_cfg;
	std::map<int, std::vector<RAHorcruxTianChengExchangeCfg> > m_exchang_cfg;
	RAHorcruxTianChengOtherCfg m_other_cfg;
	std::map<int, std::vector<RAHorcruxTianChengConsumeCfg> > m_consume_cfg;
};

#endif	//__RAND_ACTIVITY_HORCRUX_TIANCHENG_CONFIG_HPP__
