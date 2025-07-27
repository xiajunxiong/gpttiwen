#ifndef __COURAGE_CHALLENGE_DEF_H__
#define __COURAGE_CHALLENGE_DEF_H__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"

#pragma pack(push, 4)

static const int COURAGE_CHALLENGE_MAX_LEVEL = 150;
static const int COURAGE_CHALLENGE_SEQ_MAX_NUM = 400;
static const int COURAGE_CHALLENGE_FIRST_PASS_BIT_NUM = 7;				// Ĭ���״�ͨ��λ����λ��		
static const int COURAGE_CHALLENGE_EXTER_REWARD_BIT_NUM = 6;		// ���⽱��Ĭ��λ����λ��
static const int COURAGE_CHALLENGE_CONDITION_MAX_NUM = 3;
static const int COURAGE_CHALLENGE_LAYER_LEVEL_MAX_NUM = 20;			// �ȼ���������
static const int COURAGE_CHALLENGE_LAYER_LEVEL_NEW_ADD_MAX_NUM = 30;	// �����ȼ�������
static const int COURAGE_CHALLENGE_SCORE_LAYER_LEVEL_MAX_NUM = 64;
static const int COURAGE_CHALLENGE_MAX_SCORE_FLAG_BIT = COURAGE_CHALLENGE_SCORE_LAYER_LEVEL_MAX_NUM - 1;		// ���ǽ���

static const int MAX_COURAGE_CHALLENGE_SCORE_NUM = 120;					//�ֲ�ȼ���������
static const int MAX_COURAGE_CHALLENGE_LEVEL_SEQ = 15;					//ÿ���ֲ�ȼ�����С�ؿ�����
static const int MAX_COURAGE_CHALLENGE_LEVEL_NUM = MAX_COURAGE_CHALLENGE_SCORE_NUM * MAX_COURAGE_CHALLENGE_LEVEL_SEQ;		//���зֲ��С�ؿ��ܺ�����
UNSTD_STATIC_CHECK(MAX_COURAGE_CHALLENGE_SCORE_NUM == MAX_ROLE_LEVEL);

struct LayerLevelNode
{
	LayerLevelNode() { this->Reset(); }

	void Reset()
	{
		layer_level = -1;
		level_seq = -1;
		flag = 0;
	}
	short layer_level;			// �ֲ�ȼ���
	char level_seq;				// �÷ֲ�ȼ����е�seq
	unsigned char flag;			// �ò��������  ������λ���㣬���ݵ�ǰ���ö��ٸ������ֶ� COURAGE_CHALLENGE_CONDITION_MAX_NUM 
};

struct ScoreLayerLevelNode
{
	ScoreLayerLevelNode() { this->Reset(); }

	void Reset()
	{
		layer_level = -1;
		star = 0;
		flag.Reset();
	}

	short layer_level;			// �ֲ�ȼ���
	short star;					// �ķֲ�ȼ��˵��Ѵ�ɵ�������
	BitMap<COURAGE_CHALLENGE_SCORE_LAYER_LEVEL_MAX_NUM> flag;   // ��ʱ���ֽ�����ȡ��ʶ
};

struct CourageChallengeParam
{
	CourageChallengeParam()
	{
		this->Reset();
	}

	void Reset()
	{
		change_reset_flag = 0;
		is_add_rank = 0;
		is_set_new_layer_level_data = 0;
		is_set_old_data_version_flag = 0;
		old_get_timestamp = 0;
		is_change_data_storage_flag = 0;
		reserve_ch = 0;
		pet_helper_num = 0;
		new_get_timestamp = 0;
		reserve_ll = 0;
		memset(reserve_long, 0, sizeof(reserve_long));
		count = 0;
		score_layer_count = 0;
		for (int i = 0; i < COURAGE_CHALLENGE_LAYER_LEVEL_MAX_NUM; i++)
		{
			score_data_node[i].Reset();
		}
		for (int i = 0; i < COURAGE_CHALLENGE_SEQ_MAX_NUM; i++)
		{
			data_node[i].Reset();
		}
		for (int i = 0; i < COURAGE_CHALLENGE_LAYER_LEVEL_NEW_ADD_MAX_NUM; i++)
		{
			score_1_data_node[i].Reset();
		}
		memset(layer_data_node_version, 0, sizeof(layer_data_node_version));
		for (int i = 0; i < MAX_COURAGE_CHALLENGE_SCORE_NUM; i++)
		{
			score_flag[i].Reset();
		}
	}
	void DataReset()
	{
		count = 0;
		score_layer_count = 0;
		for (int i = 0; i < COURAGE_CHALLENGE_SEQ_MAX_NUM; i++)
		{
			data_node[i].Reset();
		}
		for (int i = 0; i < COURAGE_CHALLENGE_LAYER_LEVEL_MAX_NUM; i++)
		{
			score_data_node[i].Reset();
		}
		for (int i = 0; i < COURAGE_CHALLENGE_LAYER_LEVEL_NEW_ADD_MAX_NUM; i++)
		{
			score_1_data_node[i].Reset();
		}
		memset(layer_data_node_version, 0, sizeof(layer_data_node_version));
	}
	
	char change_reset_flag;				// ��ħ֮����ĺ�����֮ǰ����  6:������ ��6δ����
	char is_add_rank;					// ��һ�ε�½��Ҫ����ħ֮���б���������Ϣ 4�������
	char is_set_new_layer_level_data;	// �Ƿ����������ȼ������� 0:δ���� 1:������
	char is_set_old_data_version_flag;	// �Ƿ����þ����ݰ汾�� 1������
	unsigned int old_get_timestamp;		// ���������ʱ��ʱ��� (�ѷ���)
	char is_change_data_storage_flag;	// �ı�洢����λ�ñ�ʶ 1:�Ѹı�
	char reserve_ch;
	short pet_helper_num;				// ������ս���� ȡֵ��Χ[0,4]
	long long new_get_timestamp;		// ���������ʱ��ʱ���			
	long long reserve_ll;		
	long long reserve_long[3];		
	short count;					// ��ǰ�����ӵ�е���������
	short score_layer_count;		// ��ǰ��ҷֲ�ȼ���������������
	ScoreLayerLevelNode score_data_node[COURAGE_CHALLENGE_LAYER_LEVEL_MAX_NUM];	// �������ħ֮����ÿһ���ֲ�ȼ�����������
	LayerLevelNode data_node[COURAGE_CHALLENGE_SEQ_MAX_NUM];					// �������ħ֮����ÿһ�������
	ScoreLayerLevelNode score_1_data_node[COURAGE_CHALLENGE_LAYER_LEVEL_NEW_ADD_MAX_NUM];	// �������ħ֮����ÿһ���ֲ�ȼ�����������(��������)
	short layer_data_node_version[COURAGE_CHALLENGE_LAYER_LEVEL_MAX_NUM + COURAGE_CHALLENGE_LAYER_LEVEL_NEW_ADD_MAX_NUM];		// �Եȼ��ֲ�Ϊ��λ,��¼���,�޸��������ø÷ֲ�ͨ������������
	BitMap<COURAGE_CHALLENGE_SCORE_LAYER_LEVEL_MAX_NUM> score_flag[MAX_COURAGE_CHALLENGE_SCORE_NUM];			//���ֽ�����ȡ��ʶ
};

typedef char CourageChallengeParamHex[sizeof(CourageChallengeParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(CourageChallengeParamHex) < 7168);
#pragma pack(pop)


struct LayerLevelInfo
{
	LayerLevelInfo() { this->Reset(); }

	void Reset()
	{
		change_state = 0;
		info.Reset();
	}

	char change_state;
	LayerLevelNode info;
};

class TLVSerializer;
class TLVUnserializer;
class RoleCourageChallengeLevelParamList
{
public:
	RoleCourageChallengeLevelParamList() : count(0)
	{}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);
	
	int count;
	LayerLevelInfo data_list[MAX_COURAGE_CHALLENGE_LEVEL_NUM];
};

#endif
