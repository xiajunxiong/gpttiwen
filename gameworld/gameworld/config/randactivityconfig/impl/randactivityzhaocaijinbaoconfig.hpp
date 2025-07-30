#ifndef __RAND_ACTIVITY_ZHAOCAIJINBAO_CONFIG_HPP__
#define __RAND_ACTIVITY_ZHAOCAIJINBAO_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"
#include "servercommon/roleactivity/razhaocaijinbaoparam.hpp"


struct RAZhaoCaiJinBaoGroupCfg
{
	RAZhaoCaiJinBaoGroupCfg() : seq(0), reward_type(0), gold_num(0), select_rate(0), extract_rate(0), is_hearsay(false) {}

	short seq;
	short reward_type;
	ItemConfigData reward_item;
	int gold_num;

	int select_rate;
	int extract_rate;
	bool is_hearsay;
	int bao_di_id;
	int bao_di_times;
};

struct RAZhaoCaiJinBaoGroupMap
{
	int MAX_RATE;
	std::vector<RAZhaoCaiJinBaoGroupCfg> group_cfg;
};

struct RAZhaoCaiJinBaoBuyCfg
{
	RAZhaoCaiJinBaoBuyCfg() : limit_type(0), buy_times(0), price_type(0), buy_money(0) {}

	std::vector<ItemConfigData> reward_item;
	short limit_type;
	short buy_times;
	short price_type;
	int buy_money;
};

struct RAZhaoCaiJinBaoChongZhiCfg
{
	RAZhaoCaiJinBaoChongZhiCfg() : top_up_type(0), top_up_num(0), get_type(0), get_times(0) {}

	short top_up_type;
	int top_up_num;
	std::vector<ItemConfigData> reward_item;
	short get_type;
	short get_times;
};

struct RAZhaoCaiJinBaoChongZhiMap
{
	int top_up_type;
	std::vector<RAZhaoCaiJinBaoChongZhiCfg> chongzhi_cfg;
};

struct RAZhaoCaiJinBaoOtherCfg
{
	RAZhaoCaiJinBaoOtherCfg() : item_id(0), item_num(0), refresh_gold(0) {}

	unsigned short item_id;
	short item_num;
	int refresh_gold;
};

class RandActivityManager;

class RandActivityZhaoCaiJinBaoConfig : public RandActivityConfig
{
public:
	RandActivityZhaoCaiJinBaoConfig();
	virtual ~RandActivityZhaoCaiJinBaoConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAZhaoCaiJinBaoGroupMap * GetGroupCfg(RandActivityManager * ramgr) const;

	const std::vector<RAZhaoCaiJinBaoBuyCfg> * GetBuyMapCfg(RandActivityManager * ramgr) const;
	const RAZhaoCaiJinBaoBuyCfg * GetBuyCfg(RandActivityManager * ramgr, int seq) const;

	const RAZhaoCaiJinBaoChongZhiMap * GetTopUpMapCfg(RandActivityManager * ramgr) const;
	const RAZhaoCaiJinBaoChongZhiCfg * GetTopUpCfg(RandActivityManager * ramgr, int seq) const;

	const RAZhaoCaiJinBaoOtherCfg & GetOtherCfg() const { return m_other_cfg; }

private:
	int InitBasisCfg(TiXmlElement *RootElement);
	int InitGroupCfg(TiXmlElement *RootElement);
	int InitBuyCfg(TiXmlElement *RootElement);
	int InitTopUpCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::map<int, int> m_basis_cfg;
	std::map<int, RAZhaoCaiJinBaoGroupMap> m_group_cfg;
	std::map<int, std::vector<RAZhaoCaiJinBaoBuyCfg> > m_buy_cfg;
	std::map<int, RAZhaoCaiJinBaoChongZhiMap > m_top_up_cfg;
	RAZhaoCaiJinBaoOtherCfg m_other_cfg;
};

#endif	//__RAND_ACTIVITY_ZHAOCAIJINBAO_CONFIG_HPP__