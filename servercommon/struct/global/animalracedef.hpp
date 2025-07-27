#ifndef __ANIMAL_RACE_DEF_HPP__
#define __ANIMAL_RACE_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/roleactivity/raanimalraceparam.hpp"

#pragma pack(push, 4)

static const int ANIMAL_RACE_SAVE_TO_DB_TIME = 60;

struct AnimalRaceData
{
	AnimalRaceData() { this->Reset(); }

	void Reset()
	{
		memset(rank, 0, sizeof(rank));
		memset(moving_distance_second, 0, sizeof(moving_distance_second));
	}

	char rank[MAX_ANIMAL_RACE_NUM];			//默认下标0为1号神兽,按顺序往下,排名为[1,4]不可重复
	unsigned char moving_distance_second[MAX_ANIMAL_RACE_NUM][MAX_ANIMAL_RACE_SECOND_NUM];		//神兽每一秒的移动距离,排序同上
};


struct AnimalRaceEntry
{
	AnimalRaceEntry() { this->Reset(); }

	void Reset()
	{
		server_id = 0;
		date_id = 0;
		win_numner = 0;
		pirze_pool = 0;
		memset(animal_race_num, 0, sizeof(animal_race_num));
		memset(response_rate_num, 0, sizeof(response_rate_num));
		time_stamp = 0;
		animation_data.Reset();
	}
	
	bool InValid()
	{
		return time_stamp <= 0;
	}

	int server_id;		
	int date_id;									//哪一期
	int win_numner;									//获胜的神兽
	int pirze_pool;									//总奖池
	int animal_race_num[MAX_ANIMAL_RACE_NUM];		//神兽对应的下注数量
	int response_rate_num[MAX_ANIMAL_RACE_NUM];		//神兽对应的回报率
	long long time_stamp;							//本期结束的时间
	AnimalRaceData animation_data;					//动画数据
};

typedef char AnimalRaceDataHex[sizeof(AnimalRaceData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(AnimalRaceDataHex) < 2048);

#pragma pack(pop)

class AnimalRaceParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	AnimalRaceParam():count(0)
	{}

	struct AnimalRaceSaveEntry
	{
		AnimalRaceSaveEntry():change_state(structcommon::CS_NONE), id(0)
		{ 
			animal_race_info.Reset(); 
		}

		char change_state;			//改变的状态
		int id;
		
		AnimalRaceEntry animal_race_info;
	};

	int count;						//当前vector数量
	std::vector<AnimalRaceSaveEntry> data_list;
};


#endif