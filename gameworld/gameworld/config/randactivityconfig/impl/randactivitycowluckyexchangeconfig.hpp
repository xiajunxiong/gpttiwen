#ifndef __RAND_ACTIVITY_COW_LUCKY_EXCHANGE_CONFIG_HPP__
#define __RAND_ACTIVITY_COW_LUCKY_EXCHANGE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itemextern.hpp"
#include "item/itembase.h"
#include <map>
#include <vector>
#include "servercommon/roleactivity/racowparam.hpp"


struct RACowLuckyExchangeCfg
{
	RACowLuckyExchangeCfg() :type(0), seq(0), accumulated_down(0), accumulated_up(0), rate(0), price(0) {}

	int type;
	int	seq;
	int	accumulated_down;
	int	accumulated_up;
	int	rate;
	int	price;

	std::vector<ItemConfigData>	reward_item_vec;

};

class RandActivityManager;

class RandActivityCowLuckyExchangeConfig : public RandActivityConfig
{
public:
	RandActivityCowLuckyExchangeConfig();
	virtual ~RandActivityCowLuckyExchangeConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);
	const RACowLuckyExchangeCfg * GetCowLuckyExchangeCfgByIndex(int id) const;
	const RACowLuckyExchangeCfg * GetCowLuckyExchangeCfgByActivityChongzhi(long long chongzhi_num, int id) const;
	const RACowLuckyExchangeCfg * RandCowLuckyExchangeCfgByChongzhi(long long chongzhi_num) const;
private:
	int InitCowLuckyExchangeCfg(TiXmlElement *RootElement);

	const static int KEY_RAND_DATA = 10000;
	long long GetKeyBySection(int section_start, int section_end) const;
	void GetSectionByKey(long long key, int* section_start, int* section_end) const;

	std::map<long long, int> m_section_weight_map;
	std::map<long long,std::vector<RACowLuckyExchangeCfg> > m_section_map;
	std::map<int, RACowLuckyExchangeCfg> m_index_map;
};

#endif	//__RAND_ACTIVITY_COW_LUCKY_EXCHANGE_CONFIG_HPP__