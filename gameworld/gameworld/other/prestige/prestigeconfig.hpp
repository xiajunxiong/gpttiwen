#ifndef __PRESTIGE_CONFIG_HPP__
#define __PRESTIGE_CONFIG_HPP__
#include "servercommon/serverdef.h"
#include "servercommon/configcommon.h"
#include "gamedef.h"
#include <map>
#include <vector>

struct PrestigeOtherCfg
{
	PrestigeOtherCfg()
	{
		donate_max_num = 0;
	}

	//��ɾ��׶��⽱��
	int donate_max_num;
};

struct PrestigeUpGradeCfg
{
	PrestigeUpGradeCfg()
	{
		village_id = 0;
		relation = 0;
		need_prestige = 0;
		title_id = 0;
	}

	//����ID
	int village_id;
	//��ϵ����
	int relation;
	//��������
	int need_prestige;
	int title_id;
};

enum PRESTIGE_SHOP_LIMIT_TYPE
{
	PRESTIGE_SHOP_LIMIT_TYPE_BEGIN = 0,

	PRESTIGE_SHOP_LIMIT_TYPE_FOREVER,
	PRESTIGE_SHOP_LIMIT_TYPE_WEEK,
	PRESTIGE_SHOP_LIMIT_TYPE_DAY,

	PRESTIGE_SHOP_LIMIT_TYPE_END,
};

enum PRESTIGE_SHOP_REQUIRE_TYPE
{
	PRESTIGE_SHOP_REQUIRE_TYPE_NO = 0,	//0 - �κ����й����ʸ�
	PRESTIGE_SHOP_REQUIRE_TYPE_1,		//1 - ������ȼ���ȡ�����ʸ�
	PRESTIGE_SHOP_REQUIRE_TYPE_2,		//2 - �������ȼ����������ʸ�
	PRESTIGE_SHOP_REQUIRE_TYPE_3,		//3 - ���������������ʸ�
	PRESTIGE_SHOP_REQUIRE_TYPE_4,		//4 - ��񷶥֮����λ�ʸ��ʸ�����ֵ���ն�λID��д��
	PRESTIGE_SHOP_REQUIRE_TYPE_5,		//5 - ��񷶥֮�������ʸ�
	PRESTIGE_SHOP_REQUIRE_TYPE_6,		//6 - ���ý�ս����λ�ʸ��ʸ�����ֵ���ն�λ������д��
	
	PRESTIGE_SHOP_REQUIRE_TYPE_END,
};

struct PrestigeShopCfg
{
	PrestigeShopCfg()
	{
		index = 0;
		seq = 0;
		village_id = 0;
		relation = 0;
		item_id = 0;
		price = 0;
		is_bind = 0;

		require_type = PRESTIGE_SHOP_REQUIRE_TYPE_NO;
		require_value = 0;

		limit_type = 0;
		limit_times = 0;
		money_type = 0;
	}

	int index;
	int seq;
	int village_id;
	// ��ϵ����
	int relation;
	int item_id;
	int price;
	int is_bind;

	int require_type;
	int require_value;

	// �޹�����
	int limit_type;
	int limit_times;
	int money_type;
};

struct PrestigeDonateCfg
{
	PrestigeDonateCfg()
	{
		color= 0;
		level= 0;
		reward_prestige = 0;
	}

	int color;
	int level;
	int reward_prestige;
};

struct PrestigeVillageCfg
{
	PrestigeVillageCfg()
	{
		village_id = 0;
		donate_min_level = 0;
		donate_max_level = 0;
	}
	int village_id;
	int donate_min_level;
	int donate_max_level;
};

struct PrestigeBuyCountVersionCfg
{
	PrestigeBuyCountVersionCfg() : version(0), index(0) {}

	int version;
	int index;
};

class PrestigeConfig : public ILogicConfig
{
public:
	typedef std::vector<PrestigeBuyCountVersionCfg> BuyCountVersionVec;
	typedef std::map<int, std::map<int, PrestigeShopCfg> > ShopCfgMap;

public:
	PrestigeConfig();
	virtual ~PrestigeConfig();
	virtual bool Init(const std::string &configname, std::string *err);

	const PrestigeUpGradeCfg* GetUpGradeCfg(int village_id, int level);
	const PrestigeUpGradeCfg* GetMaxLevelCfg(int village_id);
	const PrestigeShopCfg* GetShopCfg(int village_id, int seq);
	const PrestigeShopCfg* GetShopByIndexCfg(int index);
	const PrestigeOtherCfg* GetOtherCfg() { return &m_other_cfg; }
	int RandTaskID(int rand_num);
	int GetTotalRate(int level);
	const PrestigeDonateCfg* GetDonateCfg(int village_id, int color, int level);
	const PrestigeVillageCfg* GetVillageCfg(int village_id);
	const BuyCountVersionVec& GetBuyCountVersionVec() { return m_buy_count_version_vec; }
	const ShopCfgMap& GetLimitBuyShopCfgMap() { return m_shop_limit_buy_count_cfg; }

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitUpGradeCfg(TiXmlElement *RootElement);
	int InitShopCfg(TiXmlElement *RootElement);
	int InitDonateCfg(TiXmlElement *RootElement);
	int InitVillageListCfg(TiXmlElement *RootElement);
	int InitBuyCountVersionCfg(TiXmlElement *RootElement);

	PrestigeOtherCfg m_other_cfg;

	// key1 ���� key2 �ȼ�
	std::map<int, std::map<int, PrestigeUpGradeCfg> > m_up_grade_cfg;
	
	// key1 ���� key2 seq
	ShopCfgMap m_shop_cfg;
	// key1�޹����� key2 seq
	ShopCfgMap m_shop_limit_buy_count_cfg;
	std::map<int, PrestigeShopCfg> m_shop_index_cfg;
	std::map<int, std::vector<PrestigeDonateCfg> > m_donate_map;
	std::map<int, PrestigeVillageCfg> m_village_map;

	BuyCountVersionVec m_buy_count_version_vec;
};

#endif // !__PRESTIGE_CONFIG_HPP__
