#ifndef __DAY_REWARD_CONFIG_HPP__
#define __DAY_REWARD_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/commondata.hpp"
#include "servercommon/dayrewarddef.hpp"
#include <map>
#include <set>

struct OnlineReward
{
	OnlineReward()
	{
		index = 0;
		seq = 0;
		time = 0;
		bind_coin = 0;
		section_start = 0;
		section_end = 0;
		reward_count = 0;
	}

	int index;					
	int seq;					//���
	int time;					//����ʱ��
	int bind_coin;				//ͭ�ҽ���
	int section_start;			//��ʼ����
	int section_end;			//�������� (���ڿ�������)
	int reward_count;
	ItemConfigData reward[10];	//���߽���
};

struct SignReward
{
	SignReward()
	{
		month=0;
		day=0;
		section_start = 0;
		section_end = 0;
		bind_coin=0;
	}

	int month;						//�·�	
	int day;						//����
	int section_start;
	int section_end;	
	int bind_coin;					//ͭ�ҽ���
	ItemConfigData reward;			//���߽���
};

struct LevelReward
{
	LevelReward()
	{
		seq = 0;
		level = 0;
		bind_coin = 0;
		reward_count = 0;
	}

	int seq;								  //���
	int level;								  //�ȼ�
	int bind_coin;							  //ͭ�ҽ���
	int reward_count;
	ItemConfigData reward[10];				  //���߽���
};

enum REWARD_FIND_TYPE
{
	REWARD_FIND_TYPE_COIN = 0,
	REWARD_FIND_TYPE_GOLD = 1,
};

struct DayRewardFindCfg
{
	DayRewardFindCfg()
	{
		find_type=0;
		find_level=0;
		//coin_scale = 0;
		//coin_price=0;
		//gold_scale = 0;
		//gold_price = 0;
	}
	int find_type;
	int find_level;		//����Ҵﵽ����ȼ�ʱ,�Ϳ��Լ����һش���������Ҫ�ﵽ��Ӧģ��Ŀ����ȼ�
	//int coin_scale;
	//int coin_price;
	//int gold_scale;
	//int gold_price;
};

struct DayRewardFindPriceCfg
{
	DayRewardFindPriceCfg() : min_level(0), max_level(0), coin_scale(0), coin_price(0)
	{}

	int min_level;
	int max_level;
	int coin_scale;			//ͭ�Ұٷֱ�
	int coin_price;
};

enum DAY_REWARD_DAY_FOOD_CURRENCY_TYPE
{
	DAY_REWARD_DAY_FOOD_CURRENCY_TYPE_COIN = 0,				//ͭ��
	DAY_REWARD_DAY_FOOD_CURRENCY_TYPE_GOLD = 1,				//Ԫ��
	DAY_REWARD_DAY_FOOD_CURRENCY_TYPE_SILVER_COIN = 2,		//����

	DAY_REWARD_DAY_FOOD_CURRENCY_TYPE_MAX
};

struct DayRewardDayFoodCfg
{
	DayRewardDayFoodCfg() :seq(0), start_time(0), end_time(0), food_num(0), is_make(false), make_time_end(0), currency_type(0), currency_num(0)
	{}

	int seq;
	int start_time;
	int end_time;
	int food_num;
	bool is_make;		//�Ƿ�ɲ���
	int make_time_end;
	int currency_type;	//�����������ĵĻ�������
	int currency_num;
};

struct DayRewardOtherCfg
{
	DayRewardOtherCfg():copper_coin_price(0), max_level(0), online_gift_open_level(0)
	{}

	int copper_coin_price;
	int max_level;			// ����ǿ��������߽����ȼ�
	std::set<int> pet_strengthen;
	int online_gift_open_level;		//������������ȼ�
};

struct PetStrengthenWelfareCfg
{
	PetStrengthenWelfareCfg():seq(0),pet_strengthen_lv(0)
	{}

	int seq;
	int pet_strengthen_lv;
	std::set<int> quality;
	std::vector<ItemConfigData> reward_list;
};

struct DayRewardOnlineGiftCfg
{
	DayRewardOnlineGiftCfg() : need_time(0)
	{}

	int need_time;			//����ʱ��(����)
	std::vector<ItemConfigData> reward_list;	//�����б�
};

class DayRewardConfig : public ILogicConfig
{
public:
	DayRewardConfig();
	virtual ~DayRewardConfig();
	virtual bool Init(const std::string &configname, std::string *err);

	const OnlineReward* GetOnlineReward(int index);
	const SignReward* GetSignReward(int month, int day, int server_open_day);
	const LevelReward* GetLevelReward(int seq);
	const DayRewardFindCfg* GetFindReward(int type);
	const DayRewardFindPriceCfg * GetFindRewardPriceCfg(int type, int role_level);
	const DayRewardDayFoodCfg * GetDayFoodCfg(int seq);
	const DayRewardOtherCfg& GetOtherCfg()const { return m_other_cfg; }
	const PetStrengthenWelfareCfg * GetPetStrengthenWelfareCfg(int level_or_seq, bool is_seq = false);
		
	const DayRewardOnlineGiftCfg * GetOnlineGiftCfg(int seq) const;

private:
	int InitOnlineRewardCfg(TiXmlElement *RootElement);
	int InitSignRewardCfg(TiXmlElement *RootElement);
	int InitLevelRewardCfg(TiXmlElement *RootElement);
	int InitFindRewardPriceCfg(TiXmlElement *RootElement);
	int InitFindRewardCfg(TiXmlElement *RootElement);
	int InitDayFoodCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitPetStrengthenCfg(TiXmlElement *RootElement);
	int InitOnlineGiftCfg(TiXmlElement *RootElement);

	std::map<int, OnlineReward> m_online_reward_map;
	//std::map<int, std::map<int, SignReward> > m_sign_reward_map;
	std::map<int, std::vector<SignReward> > m_sign_reward_map;		//key:month * 1000 + day
	std::map<int, LevelReward> m_level_reward_map;
	std::map<int, DayRewardFindCfg> m_find_reward_map;
	std::map<int, std::vector<DayRewardFindPriceCfg> > m_find_reward_price_map;		// �һؼ۸� key:find_type
	std::vector<DayRewardDayFoodCfg> m_day_food_vec;		//������
	DayRewardOtherCfg m_other_cfg;
	std::vector<PetStrengthenWelfareCfg> m_pet_strengthen_welfare_cfg;

	int m_max_online_gift_count;		//��ǰ���õ���������������
	DayRewardOnlineGiftCfg m_online_gift_list[DAY_REWARD_ONLINE_GIFT_NUM];	//������� �����±�:����seq
};

#endif // !__DAY_REWARD_CONFIG_HPP__
