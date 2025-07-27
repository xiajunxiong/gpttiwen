#ifndef __RA_ANIMAL_RACE_PARAM_HPP__
#define __RA_ANIMAL_RACE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2061 ���� - ���޾���  //////////////////////////////////////////

const static int MAX_ANIMAL_RACE_NUM = 4;						//�����������
const static int MAX_ANIMAL_RACE_SCENE_NUM = 19;				//��������������ֱ������Ϊ19,������Լ�Ϊ20
const static int MAX_ANIMAL_RACE_SC_NUM = 20;					//Э�鷢��
const static int MAX_ANIMAL_RACE_SECOND_NUM = 50;				//�����ƶ����������
const static int MAX_ANIMAL_RACE_DISTANCE_NUM = 10000;			//�����趨�ܵ������
const static int ANIMAL_RACE_AVERAGE_SPEED_NUM = MAX_ANIMAL_RACE_DISTANCE_NUM / MAX_ANIMAL_RACE_SECOND_NUM;	//��������������ܵ�����,���ƽ���ٶ�
const static int MAX_ANIMAL_RACE_EVENTHAMDLER_SECOND_NUM = 5;	//һ���¼����ĳ���ʱ��

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RAAnimalRaceParam
{
	RAAnimalRaceParam() { this->Reset(); }
	void Reset()
	{
		ra_animal_race_begin_timestamp = 0;
		next_refresh_game_info_stamp = 0;
		server_id = 0;
		date_id = 0;
		memset(ra_animal_race_bet_type_count, 0, sizeof(ra_animal_race_bet_type_count));
	}

	unsigned int ra_animal_race_begin_timestamp;
	time_t next_refresh_game_info_stamp;										//�´�ˢ����Ϸ��ʱ���,Ҳ�Ƿ��Ž�����ʱ��	
	int date_id;																//����һ��Ѻע
	int	server_id;																//Ѻעʱ������ڵķ�����
	short ra_animal_race_bet_type_count[MAX_ANIMAL_RACE_NUM];					//��Ҹ���ÿ�����޵�Ѻע���
};

UNSTD_STATIC_CHECK(sizeof(RAAnimalRaceParam) <= sizeof(RoleActivityRoleData))


#pragma pack(pop)


#endif	//__RA_ANIMAL_RACE_PARAM_HPP__
