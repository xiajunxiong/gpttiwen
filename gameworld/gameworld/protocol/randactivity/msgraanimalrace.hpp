#ifndef __MSG_RA_ANIMAL_RACE_HPP__
#define __MSG_RA_ANIMAL_RACE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rahappyupparam.hpp"

//////////////////////////////////////////// 2061  ����  ���޾���  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_ANIMAL_RACE_OPERA_TYPE
	{
		RA_ANIMAL_RACE_OPERA_TYPE_ROLE_INFO = 0,		//���޾��������Ϣ�·�	
		RA_ANIMAL_RACE_OPERA_TYPE_BET = 1,				//�����ע param_1 - �������[1,4] param_2 - ��ע������
		RA_ANIMAL_RACE_OPERA_TYPE_ACTIVITY_INFO = 2,	//���޾����������Ϣ�·�
		RA_ANIMAL_RACE_OPERA_TYPE_ENTER_SCENE = 3,		//��ҽ��볡��
		RA_ANIMAL_RACE_OPERA_TYPE_EXIT_SCENE = 4,		//����˳�����
		RA_ANIMAL_RACE_OPERA_TYPE_ANIMATION_INFO = 5,	//��עʱ�����,׼����ʼ���Ŷ���
	};

	struct SCRAAnimalRaceInfo								// 3018 ���޾���������Ϣ�·�
	{
	public:
		SCRAAnimalRaceInfo();
		MessageHeader header;

		enum ANIMAL_RACE_INFO_TYPE
		{
			ANIMAL_RACE_INFO_TYPE_ROLE_READY = 0,			//�����Ϣ�·�    timestamp - ׼��ʱ��: Ѻעʱ�������ʱ��� |  ����Ҫ��ʤ�������
			ANIMAL_RACE_INFO_TYPE_ROLE_DOING = 1,			//�����Ϣ�·�    timestamp - ������:  ������һ��Ѻעʱ���ʱ��� | ��Ҫ��ʤ�������
			ANIMAL_RACE_INFO_TYPE_NOT_TIME = 2,				//�����Ϣ�·�    timestamp - ����״̬ �´�Ѻעʱ�俪ʼ��ʱ�� | ����Ҫ��ʤ�������
		};

		int op_type;
		long long  timestamp;
		char rank[MAX_ANIMAL_RACE_NUM];								//���ֻ�ʤ��������� [1,4]
		int count;													//���������������
		role_info scene_appearance[MAX_ANIMAL_RACE_SC_NUM];			//����������� MAX_ANIMAL_RACE_SC_NUM - 20
	};

	struct SCRAAnimalRaceRoleInfo								// 3019 ���޾��������Ϣ�·�
	{
	public:
		SCRAAnimalRaceRoleInfo();
		MessageHeader header;

		short bet_type_count[MAX_ANIMAL_RACE_NUM];					//��Ҹ��˶���ÿ�����޵�Ѻע���
		int everything_bet_type_count[MAX_ANIMAL_RACE_NUM];			//ȫ����ʱÿ�����޵�Ѻע���
		int type_response_rate[MAX_ANIMAL_RACE_NUM];				//��ʱÿ�����޵Ļر���
	};

	struct SCRAAnimalRaceAnimationInfo		//!<��3037 ���޾������������·�
	{
	public:
		SCRAAnimalRaceAnimationInfo();
		MessageHeader header;

		AnimalRaceData animation_data;				//��������
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_ANIMAL_RACE_HPP__


