#ifndef __RAND_ACTIVITY_CINIU_YINGXINHU_CONFIG_HPP__
#define __RAND_ACTIVITY_CINIU_YINGXINHU_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"


struct RACiNiuYingXinHuBasisCfg
{
	RACiNiuYingXinHuBasisCfg() :activity_days(0), awaken_reward_group(0) {}

	short activity_days;
	short awaken_reward_group;
};

struct RACiNiuYingXinHuGiftCfg
{
	RACiNiuYingXinHuGiftCfg() : param(0) {}

	int param;
	std::vector<ItemConfigData> reward;
};

struct RACiNiuYingXinHuGroup
{
	RACiNiuYingXinHuGroup() : is_hearsay(false) {}

	bool is_hearsay;
	ItemConfigData item;
};

struct RACiNiuYingXinHuGroupMap
{
	RACiNiuYingXinHuGroupMap() : max_rate(0) {}

	int max_rate;
	std::map<int, RACiNiuYingXinHuGroup> item_map;
};

struct RACiNiuYingXinHuOtherCfg
{
	RACiNiuYingXinHuOtherCfg() : item_id(0), item_num(0), add_tiger_air(0) {}

	unsigned short item_id;
	short item_num;
	int add_tiger_air;
};

class RandActivityManager;
class RandActivityCiNiuYingXinHuConfig : public RandActivityConfig
{
public:
	RandActivityCiNiuYingXinHuConfig();
	virtual ~RandActivityCiNiuYingXinHuConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RACiNiuYingXinHuBasisCfg * GetBasisCfg(RandActivityManager * ramgr, int seq) const;
	const RACiNiuYingXinHuGiftCfg * GetGiftCfg(RandActivityManager * ramgr, int module_type, int seq) const;
	int GetGiftMaxCount(RandActivityManager * ramgr, int module_type) const;			// 虎气最大值
	int GetGiftMax(RandActivityManager * ramgr, int module_type) const;					// 可领取礼包档位
	const RACiNiuYingXinHuGroup * GetGroupCfg(int group) const;
	const RACiNiuYingXinHuOtherCfg * GetOtherCfg() const { return &m_other_cfg; }

private:
	int InitBasisCfg(TiXmlElement *RootElement);
	int InitGiftCfg(TiXmlElement *RootElement);
	int InitGroupCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::map<short, std::vector<RACiNiuYingXinHuBasisCfg> > m_basis_cfg;
	std::map<short, std::map<short, std::vector<RACiNiuYingXinHuGiftCfg> > > m_gift_cfg;
	std::map<short, RACiNiuYingXinHuGroupMap> m_group_cfg;
	RACiNiuYingXinHuOtherCfg m_other_cfg;
};


#endif	// __RAND_ACTIVITY_CINIU_YINGXINHU_CONFIG_HPP__