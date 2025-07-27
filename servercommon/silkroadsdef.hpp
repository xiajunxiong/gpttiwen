#ifndef __SILK_ROADS_DEF_HPP__
#define __SILK_ROADS_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int MAX_SILK_ROADS_LINE_NUM = 20;					//·��ͼ�����������ֵ
static const int MAX_SILK_ROADS_CITY_COMMODITY_NUM = 20;		//�������ز����ֵ
static const int MAX_SILK_ROADS_GRID_NUM = 350;					//�ֿ�������ֵ
static const int MAX_SILK_ROADS_RANK_NUM = 100;					//����������
static const int MAX_SILK_ROADS_REFRESH_NUM = 60;				//��ˢ��ʱ����
static const int MAX_SILK_ROADS_TRADING_LOG_NUM	= 100;			//��ɫó����־���ֵ

static const int MAX_SILK_ROADS_REWAD_RATE_COUNT = 10000;		

//��ɫ�����е���Ϣ
struct ActivitySilkRoadsRoleCityData
{
	ActivitySilkRoadsRoleCityData() { this->Reset(); }
	
	struct CommodityItem
	{
		CommodityItem() { this->Reset(); }

		void Reset()
		{
			commodity_seq = 0;
			price = 0;
		}
		
		short commodity_seq;			//�������
		unsigned short price;			//����۸�
	};

	void Reset()
	{
		reserve_sh = 0;
		cur_city_buy_news_count = 0;
		city_map_seq = 0;
		commondity_count = 0;
		for (int i = 0; i < ARRAY_LENGTH(commodity_list); i++)
		{
			commodity_list[i].Reset(); 
		}
	}

	void GetCommodity(int commodity_seq, int & price)
	{
		for (int i = 0; i < commondity_count && i < ARRAY_LENGTH(commodity_list); i++)
		{
			if (commodity_seq == commodity_list[i].commodity_seq)
			{
				price = (int)commodity_list[i].price;
				break;
			}
		}
	}

	short reserve_sh;
	short cur_city_buy_news_count;										//�ó�������Ϣ����
	short city_map_seq;													//�ó����Ӧ��ͼ���
	short commondity_count;												//��������
	CommodityItem commodity_list[MAX_SILK_ROADS_CITY_COMMODITY_NUM];	//�ó�����ز��б�
};

enum SILK_ROADS_LOG_TYPE
{
	SILK_ROADS_LOG_TYPE_BEGIN = 0,
	SILK_ROADS_LOG_TYPE_PRICE_UP = 1,			//��ͨ�Ƿ�
	SILK_ROADS_LOG_TYPE_DOUBLE_PRICE_UP = 2,	//����

	SILK_ROADS_LOG_TYPE_MAX
};
UNSTD_STATIC_CHECK(SILK_ROADS_LOG_TYPE_MAX < MAX_INT16);

//��ɫó����־
struct ActivitySilkRoadsRoleTradingLogData
{
	ActivitySilkRoadsRoleTradingLogData() { this->Reset(); }

	void Reset()
	{
		log_type = SILK_ROADS_LOG_TYPE_BEGIN;
		freight_id = 0;
		from_city_map_seq = 0;
		target_city_map_seq = 0;
		commodity_seq = 0;
		reserve_sh = 0;
		price = 0;
	}
	short log_type;
	short freight_id;			//�չ�Ŀ��ID
	short from_city_map_seq;	//��Ϣ��Դ�����ͼ���
	short target_city_map_seq;	//Ŀ������ͼ���
	short commodity_seq;		//�������
	short reserve_sh;
	int price;					//�۸�
};

struct ActivitySilkRoadsRoleWarehouseData
{
	ActivitySilkRoadsRoleWarehouseData() { this->Reset(); }

	void Reset()
	{
		cur_max_grid_count = 0;
		cur_has_commodity_count = 0;
		for (int i = 0; i < ARRAY_LENGTH(grid_list); ++i)
		{
			grid_list[i].Reset();
		}
	}

	struct GridInfo
	{
		GridInfo() { this->Reset(); }
		void Reset()
		{
			commodity_seq = 0;
			commodity_count = 0;
			commodity_cost = 0;
		}
		
		short commodity_seq;		//�������
		short commodity_count;		//��������
		float commodity_cost;		//����ɱ�
	};

	short cur_max_grid_count;		//��ǰ����ʹ�õ�����
	short cur_has_commodity_count;	//��ǰ���еĻ�������
	GridInfo grid_list[MAX_SILK_ROADS_GRID_NUM];		//��¼��ʱ�������Ȼ�ѵ���,����ʵ��ռ��һ������ռһ������
};

struct ActivitySilkRoadsRoleData
{
	ActivitySilkRoadsRoleData() { this->Reset(); }

	void Reset()
	{
		cumulation_coin_count = 0;
		cumulation_trading_count = 0;
		finish_num = 0;
		line_group_id = 0;
		buy_warehouse_count = 0;
		buy_supply_count = 0;
		reserve_sh = 0;
		have_coin_count = 0;
		have_supply_count = 0;
		init_skill = 0;
		cur_city_index = -1;
		trading_num = 0;
		city_count = 0;
		warehouse_data.Reset();
		for (int i = 0; i < ARRAY_LENGTH(city_list); i++)
		{
			city_list[i].Reset();
		}
	}
	void FinishReset()
	{
		line_group_id = 0;
		buy_warehouse_count = 0;
		buy_supply_count = 0;
		reserve_sh = 0;
		have_coin_count = 0;
		have_supply_count = 0;
		init_skill = 0;
		cur_city_index = -1;
		trading_num = 0;
		city_count = 0;
		warehouse_data.Reset();
		for (int i = 0; i < ARRAY_LENGTH(city_list); i++)
		{
			city_list[i].Reset();
		}
	}

	int cumulation_coin_count;		//���λ�ۻ��Ļ������� (����ʱ��:��ɴ���)
	int cumulation_trading_count;	//���λ�ۻ���ó�״���
	int finish_num;					//���λ��ɴ���

	short line_group_id;			//��ұ���ѡ���·��ͼID	
	short buy_warehouse_count;		//�ֿ��������
	short buy_supply_count;			//�����������
	short reserve_sh;	
	int have_coin_count;			//��ǰӵ�еĻ���
	int have_supply_count;			//��ǰӵ�еĲ���
	short init_skill;				//��ʼ����
	short cur_city_index;			//��ǰ���ڵĳ���(indexΪ��������)
	short trading_num;				//ó�״���
	short city_count;				//��·�߳�������
	ActivitySilkRoadsRoleWarehouseData warehouse_data;					//�ֿ���Ϣ
	ActivitySilkRoadsRoleCityData city_list[MAX_SILK_ROADS_LINE_NUM];	//���ѡ���·��ͼ��ÿ���������Ϣ
};
typedef char ActivitySilkRoadsRoleDataHex[sizeof(ActivitySilkRoadsRoleData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ActivitySilkRoadsRoleDataHex) <= 10240);

struct ActivitySilkRoadsRoleRankData
{
	ActivitySilkRoadsRoleRankData(int _role_id, int _cumulation_coin_count, int _cumulation_trading_count, int _finish_num) 
		: role_id(_role_id), cumulation_coin_count(_cumulation_coin_count), cumulation_trading_count(_cumulation_trading_count), finish_num(_finish_num) {}
	ActivitySilkRoadsRoleRankData() { this->Reset(); }
	
	void Reset()
	{
		role_id = 0;
		cumulation_coin_count = 0;
		cumulation_trading_count = 0;
		finish_num = 0;
	}

	int role_id;
	int cumulation_coin_count;		//���λ�ۻ��Ļ������� (����ʱ��:��ɴ���)
	int cumulation_trading_count;	//���λ�ۻ���ó�״���
	int finish_num;					//���λ��ɴ���
};

struct ActivitySilkRoadsRoleRankInfo
{
	ActivitySilkRoadsRoleRankInfo() { this->Reset(); }
	ActivitySilkRoadsRoleRankInfo(const ActivitySilkRoadsRoleRankData & rank_info)
	{
		role_id = rank_info.role_id;
		memset(role_name, 0, sizeof(role_name));
		avatar_type = 0;
		headshot_id = 0;
		cumulation_coin_count = rank_info.cumulation_coin_count;
		cumulation_trading_count = rank_info.cumulation_trading_count;
	}

	void Reset()
	{
		role_id = 0;
		memset(role_name, 0, sizeof(role_name));
		avatar_type = 0;
		headshot_id = 0;
		cumulation_coin_count = 0;
		cumulation_trading_count = 0;	
		
	}

	int role_id;
	GameName role_name;
	short avatar_type;				// ������������
	short headshot_id;				// ͷ��ID 
	int cumulation_coin_count;		// ���λ�ۻ��Ļ�������
	int cumulation_trading_count;	// ���λ�ۻ���ó�״���
};

#pragma pack(pop)

struct ActivitySilkRoadsTradingLogData
{
	ActivitySilkRoadsTradingLogData() { this->Reset(); }

	void Reset()
	{
		city_log_list.clear();
		buy_count = 0;
		for (int i = 0; i < ARRAY_LENGTH(buy_city_log_list); ++i)
		{
			buy_city_log_list[i].Reset();
		}
	}

	std::vector<ActivitySilkRoadsRoleTradingLogData> city_log_list;		//һ��ʼ���ɵ�������Ϣ�б�
	int buy_count;
	ActivitySilkRoadsRoleTradingLogData buy_city_log_list[MAX_SILK_ROADS_TRADING_LOG_NUM]; 	//��ҹ������Ϣ�б�
};


#endif