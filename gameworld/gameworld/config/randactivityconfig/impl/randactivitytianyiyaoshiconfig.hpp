#ifndef __RAND_ACTIVITY_TIAN_YI_YAO_SHI_CONFIG_HPP__
#define __RAND_ACTIVITY_TIAN_YI_YAO_SHI_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itempool.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/roleactivity/rahunqifanliparam.hpp"
#include <memory>
struct RATianYiYaoShiBasicCfg
{
	RATianYiYaoShiBasicCfg():rate(0), is_hearsay(false)
	{}

	int rate;
	bool is_hearsay;		// �Ƿ���
	ItemConfigData reward;
};

struct RATianYiYaoShiBasicSectionCfg
{
	RATianYiYaoShiBasicSectionCfg() :section_start(0), section_end(0), total_rate(0)
	{}

	bool IsInclude(int real_open_server_day) const
	{
		return (section_start <= real_open_server_day && real_open_server_day <= section_end);
	}

	const RATianYiYaoShiBasicCfg* RandBasicCfg()const;

	int section_start;
	int section_end;
	int total_rate;
	std::vector<RATianYiYaoShiBasicCfg> reward_list;
};

struct RATianYiYaoShiShopCfg
{
	RATianYiYaoShiShopCfg():cfg_or(false), limit_type(0), buy_times(0), price_type(0), price(0)
	{}

	bool cfg_or;	// �Ƿ�ֱ��
	int limit_type;	// �޹�����
	int buy_times;	// �޹�����
	int price_type;	// ��������(�����ֱ��������Ч)
	int price;		// �۸�
	ItemConfigData reward;
};

struct RATianYiYaoShiShopSectionCfg
{
	RATianYiYaoShiShopSectionCfg() :section_start(0), section_end(0)
	{}

	bool IsInclude(int real_open_server_day) const
	{
		return (section_start <= real_open_server_day && real_open_server_day <= section_end);
	}

	int section_start;
	int section_end;
	std::vector<RATianYiYaoShiShopCfg> shop_list;
};


struct RATianYiYaoShiOtherCfg
{
	RATianYiYaoShiOtherCfg():consume_item(0)
	{}

	ItemID consume_item;
};

class RandActivityTianYiYaoShiConfig : public RandActivityConfig
{
public:
	RandActivityTianYiYaoShiConfig();
	virtual ~RandActivityTianYiYaoShiConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RATianYiYaoShiBasicCfg* RandRewardCfg(RandActivityManager* ramgr)const;
	const RATianYiYaoShiShopCfg* GetShopCfg(RandActivityManager* ramgr, int seq)const;

	const RATianYiYaoShiOtherCfg& GetOtherCfg() const { return m_other_cfg; }

private:
	int InitBasisCfg(TiXmlElement *RootElement, std::string& err);
	int InitShopCfg(TiXmlElement *RootElement, std::string& err);
	int InitOtherCfg(TiXmlElement *RootElement, std::string& err);


	std::vector<RATianYiYaoShiBasicSectionCfg> m_basic_section_cfg;	// ��������
	std::vector<RATianYiYaoShiShopSectionCfg> m_shop_section_cfg;	// �̳�����
	RATianYiYaoShiOtherCfg m_other_cfg;
};

#endif