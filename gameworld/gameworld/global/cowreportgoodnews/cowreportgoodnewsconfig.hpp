#ifndef  __COW_REPORT_GOOD_NEWS_CONFIG_HPP__
#define  __COW_REPORT_GOOD_NEWS_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/maildef.hpp"

struct CowReportGoodNewsOtherCfg
{
	CowReportGoodNewsOtherCfg():current_price(0), reward_count(0), return_count(0),
		start_time(0), until_time(0), known_time(0), show_time(0), timestamp(0){}

	int current_price;												// 购买价格
	
	int reward_count;												// 表长
	ItemConfigData reward_item[MAX_ATTACHMENT_ITEM_NUM];			// 申购奖励

	int return_count;												// 申购失败返还的灵玉数量

	int start_time;													// 开启时间 [表示开服第一天的0点后多少个小时开启活动] [单位:小时]
	int until_time;													// 购买持续时间,从开启时间计算 [单位:小时]
	int known_time;													// 展示期开启时间,从开启时间计算 [单位:小时]
	int show_time;													// 展示期时长 , 从展示期开启后计算 [单位:小时]

	unsigned int timestamp;											// 时间戳 开服时间在这个时间之后的才会开启活动
};


// 福牛报喜
class CowReportGoodNewsConfig : public ILogicConfig
{
public:
	CowReportGoodNewsConfig();
	virtual ~CowReportGoodNewsConfig();

	virtual bool Init(const std::string & configname, std::string * err);
	const CowReportGoodNewsOtherCfg & GetOtherCfg() { return m_other_cfg; }

private:
	int InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg);

	CowReportGoodNewsOtherCfg	m_other_cfg;
};

#endif	// __COW_REPORT_GOOD_NEWS_CONFIG_HPP__
