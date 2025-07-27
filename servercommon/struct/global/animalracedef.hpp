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

	char rank[MAX_ANIMAL_RACE_NUM];			//Ĭ���±�0Ϊ1������,��˳������,����Ϊ[1,4]�����ظ�
	unsigned char moving_distance_second[MAX_ANIMAL_RACE_NUM][MAX_ANIMAL_RACE_SECOND_NUM];		//����ÿһ����ƶ�����,����ͬ��
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
	int date_id;									//��һ��
	int win_numner;									//��ʤ������
	int pirze_pool;									//�ܽ���
	int animal_race_num[MAX_ANIMAL_RACE_NUM];		//���޶�Ӧ����ע����
	int response_rate_num[MAX_ANIMAL_RACE_NUM];		//���޶�Ӧ�Ļر���
	long long time_stamp;							//���ڽ�����ʱ��
	AnimalRaceData animation_data;					//��������
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

		char change_state;			//�ı��״̬
		int id;
		
		AnimalRaceEntry animal_race_info;
	};

	int count;						//��ǰvector����
	std::vector<AnimalRaceSaveEntry> data_list;
};


#endif