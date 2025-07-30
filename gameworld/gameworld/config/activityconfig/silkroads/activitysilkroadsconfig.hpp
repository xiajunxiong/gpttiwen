#ifndef __ACTIVITY_SILK_ROADS_CONFIG_HPP__
#define __ACTIVITY_SILK_ROADS_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"

#include <set>

struct ActivitySilkRoadsOtherCfg
{
	ActivitySilkRoadsOtherCfg() : buy_bag(0), buy_supply(0), act_num(0), start_bag(0), start_coin(0), start_supply(0), city_reward_group_rate(0), 
	start_city_seq(0), start_supply_num(0), city_max_news(0), find_coin(0)
	{}

	int buy_bag;					//���α�����������
	int buy_supply;					//���β�����������
	int act_num;					//ÿ�λ����
	int start_bag;					//Ĭ�ϳ�ʼ��������
	int start_coin;					//Ĭ�ϳ�ʼ��������
	int start_supply;				//Ĭ�ϳ�ʼ��������
	int city_reward_group_rate;		//�������������ĸ��� ��ֱ�
	int start_city_seq;				//������
	int start_supply_num;			//��ʼ��������
	int city_max_news;				//���������ܲ�������Ϣ����
	int find_coin;					//�һ�ͭ�ҽ���
	ItemConfigData find_reward;		//�һ���Ʒ����
};

struct ActivitySilkRoadsCityCfg
{
	ActivitySilkRoadsCityCfg() : city_map_seq(0), city_reward_group_id(0)
	{}

	int city_map_seq;					//��Ӧ��ͼ���
	int city_reward_group_id;			//�������Ľ�����ID
	std::set<int> city_specialty_list;	//������ز��б�
	std::vector<int> double_price_list;	//��Ʒ���Ǹ���	
};

struct ActivitySilkRoadsCommodityCfg
{
	static const int MAX_COMMODITY_BASE_NUM = 100;		
	ActivitySilkRoadsCommodityCfg() :city_specialty_seq(0), freight_market_price(0), min_price(0), max_price(0),min_price_up(0), max_price_up(0), lowest_price(0)
	{}

	int city_specialty_seq;					//�������
	int freight_market_price;				//����ԭ��(�г���)
	int min_price;							//��ͼ۸���(�ٷֱ�)
	int max_price;							//��߼۸���(�ٷֱ�)
	int min_price_up;						//����������Ͱٷֱ�
	int max_price_up;						//����������߰ٷֱ�
	int lowest_price;						//��ͼ�
};

struct ActivitySilkRoadsLineCfg
{
	ActivitySilkRoadsLineCfg(): line_group_id(0)
	{}

	int line_group_id;
	std::vector<int> line_vec;		//��·˳��
};

struct ActivitySilkRoadsSettlementBoxCfg
{
	ActivitySilkRoadsSettlementBoxCfg() : levels(0), num_end(0), coin(0)
	{}

	int levels;	
	int num_end;						//����ʱ�Ļ�������
	int coin;							//ͭ�ҽ���
	ItemConfigData reward_item;			//������Ʒ
};

enum  ACTIVITY_SILK_ROADS_SKILL_TYPE
{
	ACTIVITY_SILK_ROADS_SKILL_TYPE_BEGIN = 0,	
	ACTIVITY_SILK_ROADS_SKILL_TYPE_WAREHOUSE = 1,	//�������� �������Ͳ�λX
	ACTIVITY_SILK_ROADS_SKILL_TYPE_FUNDING = 2,		//�����ӵ� ��ʼ�ʽ�����X
	ACTIVITY_SILK_ROADS_SKILL_TYPE_TRADING = 3,		//ó����� ��;�������ļ���X%
	ACTIVITY_SILK_ROADS_SKILL_TYPE_NEWS = 4,		//���ݺ��� �������ļ���10%

	ACTIVITY_SILK_ROADS_SKILL_TYPE_MAX
};

struct ActivitySilkRoadsSkillCfg
{
	ActivitySilkRoadsSkillCfg() : skill_id(0), skill_type(0), skill_roads_skill_num(0)
	{}

	int skill_id;						//����ID
	int skill_type;						//��������
	int skill_roads_skill_num;			//��Ӧ������ֵ
};

struct ActivitySilkRoadsCityRewardCfg
{
	ActivitySilkRoadsCityRewardCfg() : rate(0), commodity_count(0)
	{}

	int rate;
	std::set<int> reward_commodity_list;		//�����Ļ����б�
	int commodity_count;						//��������
};

class  ActivitySilkRoadsConfig : public ILogicConfig
{
public:
	ActivitySilkRoadsConfig();
	virtual ~ActivitySilkRoadsConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ActivitySilkRoadsOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const ActivitySilkRoadsCityCfg * GetCityCfg(int city_map_seq);
	const ActivitySilkRoadsCommodityCfg* GetCommodityCfg(int commodity_seq);
	const std::vector<ActivitySilkRoadsCommodityCfg> & GetCommodityList() { return m_commodity_list; }
	const ActivitySilkRoadsLineCfg * GetLineCfg(int group);
	const ActivitySilkRoadsLineCfg * GetRandLineCfg();
	int GetRandFreightId(int type);
	int GetBuyWarehousePrice(int buy_times);
	int GetMaxBuyWarehouseTimes();
	int GetWarehouseExpend(int warehouse_grid_count);
	int GetBuySupplyPrice(int buy_times);
	int GetMaxBuySupplyTimes();
	int GetBuyNewsPrice(int buy_times);
	int GetMaxBuyNewsTimes();

	const ActivitySilkRoadsSettlementBoxCfg * GetSettlementBoxCfg(int coin_count);
	const ItemConfigData * GetRankReward(int rank);
	const ActivitySilkRoadsSkillCfg * GetSkillCfg(int skill_id);
	const ActivitySilkRoadsSkillCfg * GetRandSkillCfg();
	const ActivitySilkRoadsCityRewardCfg * GetRandCityRewardByCity(int city_map_seq);
	const ActivitySilkRoadsCityRewardCfg * GetRandCityReward(int reward_group_id);

private:
	int InitOtherCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitCityCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitCommodityCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitShoppingCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitLineCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitBuyWarehouseCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitWarehouseExpendCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitBuySupplyPeiceCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitBuyNewsCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitSettlementBoxCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitRankListCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitRoadsSkillCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitCityRewardCfg(TiXmlElement *RootElement, std::string& errormsg);

	ActivitySilkRoadsOtherCfg m_other_cfg;
	std::map<int, ActivitySilkRoadsCityCfg> m_city_map;		//�������� key:city_map_seq
	std::vector<ActivitySilkRoadsCommodityCfg> m_commodity_list;	//�������� key:seq
	std::map<int, ActivitySilkRoadsLineCfg> m_line_map;		//��·ͼ���� key:group
	std::set<int> m_shopping_list[SILK_ROADS_LOG_TYPE_MAX];		//�չ���Ϣ���� 

	std::map<int, int> m_buy_warehouse_pirce_map;			//�ֿ�����۸����� key:������� second:�۸�
	std::map<int, int> m_warehouse_expend_map;				//�ֿ���Ӷ�Ӧ������������ key:�������� second:�����۸�
	std::map<int, int> m_buy_supply_peice_map;				//��������۸����� key:������� second:�����۸�
	std::map<int, int> m_buy_news_map;						//������Ϣ�۸����� key:������� second:�۸�

	std::map<int, ActivitySilkRoadsSettlementBoxCfg> m_settlement_box_map;		// ���㽱�� key:��������
	std::map<ParamSection, ItemConfigData> m_rank_reward_map;		// ���а��� key:��������
	std::map<int, ActivitySilkRoadsSkillCfg> m_skill_map;			// ����Ч�� key:����ID
	std::map<int, std::vector<ActivitySilkRoadsCityRewardCfg> > m_city_reward_cfg;	//����������� key:������ID

};


#endif