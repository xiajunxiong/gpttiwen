#ifndef __RAND_ACTIVITY_PAI_MAI_HANG_CONFIG_HPP__
#define __RAND_ACTIVITY_PAI_MAI_HANG_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/activitydef.hpp"
#include "config/randactivityconfig/randactivityconfig.hpp"

struct RAPaiMaiHangOtherCfg
{
	RAPaiMaiHangOtherCfg()
	{}

	int auction_time;			//拍卖开始时间
	int end_time;				//拍卖正常结束时间
	int last_end_time;			//拍卖最晚结束时间
	int delay_time;				//延迟时间(分钟)
	int pre_price;				//加价金额
};

struct RAPaiMaiHangShopCfg
{
	RAPaiMaiHangShopCfg() : seq(0), activity_day(0), base_price(0)
	{}

	int seq;
	int activity_day;
	int base_price;
	ItemConfigData item;
};

union RAPaiMaiHangKey
{
	int param[2];
	long long unique_key;
};
static const int PAI_MAI_HANG_DAY_KEY_BASE_NUM = 10000;

class RandActivityManager;
class RandActivityPaiMaiHangConfig : public RandActivityConfig
{
public:
	RandActivityPaiMaiHangConfig();
	virtual ~RandActivityPaiMaiHangConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAPaiMaiHangOtherCfg & GetOtherCfg()const { return m_other_cfg; }
	const RAPaiMaiHangShopCfg * GetShopCfg(RandActivityManager* ramgr, int seq, int activity_open_day)const;


private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitShopCfg(TiXmlElement *RootElement);

	long long GetSectionKey(int section_start, int section_end);
	void GetSectionByKey(long long section_key, int * section_start, int * section_end)const;
	int GetDaySeqKey(int activity_day, int seq)const;
	
	RAPaiMaiHangOtherCfg m_other_cfg;
	std::map<long long, std::map<int, RAPaiMaiHangShopCfg> > m_shop_cfg;	//key1:GetSectionKey  key2:GetDaySeqKey
};

#endif