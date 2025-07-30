#ifndef __RAND_ACTIVITY_CONSUME_CAROUSEL_CONFIG_HPP__
#define __RAND_ACTIVITY_CONSUME_CAROUSEL_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/raconsumecarouselparam.hpp"
#include "item/itembase.h"

struct RAConsumerCarouselCfg
{
	RAConsumerCarouselCfg() :seq(0), score(0)
	{}

	int seq;
	int score;				//权重
	int show;				//珍稀度 1.珍稀显示 0.不显示
	ItemConfigData item;
};

struct RAConsumerCarouselSectionCfg
{
	RAConsumerCarouselSectionCfg() :section_start(0), section_end(0), rate_count(0)
	{}

	int section_start;
	int section_end;

	int rate_count;
	RAConsumerCarouselCfg node_cfg[CONSUME_CAROUSEL_MAX_NUM];				//CONSUME_CALOUSEL_MAX_NUM -12
};


class RandActivityManager;

class RandActivityConsumeCarouselConfig : public RandActivityConfig
{
public:
	RandActivityConsumeCarouselConfig();
	virtual ~RandActivityConsumeCarouselConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAConsumerCarouselCfg * GetConsumerCarouselReward(RandActivityManager* ramgr)const;
	const RAConsumerCarouselCfg * GetConsumerCarouselbyseq(RandActivityManager* ramgr, int seq) const;

private:
	int InitCfg(TiXmlElement * RootElement);

	std::vector<RAConsumerCarouselSectionCfg> m_consume_carousel_sections;				// 消费转盘
};

#endif	//__RAND_ACTIVITY_CONSUME_CAROUSEL_CONFIG_HPP__