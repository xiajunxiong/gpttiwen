#ifndef __RA_XIAN_SHI_LEI_CHONG_CONFIG_HPP__
#define __RA_XIAN_SHI_LEI_CHONG_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/raxianshileichongparam.hpp"
#include "item/itembase.h"
#include "servercommon/maildef.hpp"

struct RAXianShiLeiChongCfg
{
	RAXianShiLeiChongCfg():seq(0), acc_price(0), count(0){}

	int seq;
	int acc_price;

	int count;
	ItemConfigData reward_arr[MAX_ATTACHMENT_ITEM_NUM];
};

class RandActivityXianShiLeiChongConfig : public RandActivityConfig
{
public:
	RandActivityXianShiLeiChongConfig();
	virtual ~RandActivityXianShiLeiChongConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAXianShiLeiChongCfg * GetRewardCfgBySeq(int _open_day , int _seq) const;
	const std::vector<RAXianShiLeiChongCfg> *GetRewardCfg(int _open_day) const;
private:
	int InitRewradCfg(TiXmlElement * RootElement);

	std::map<int, std::vector<RAXianShiLeiChongCfg> > m_reawrd_cfg_map;
};

#endif	//__RA_XIAN_SHI_LEI_CHONG_CONFIG_HPP__
