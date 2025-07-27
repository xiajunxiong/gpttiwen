#ifndef __FACE_SCORE_DEF_H__
#define __FACE_SCORE_DEF_H__

#include "servercommon/bitmapfunction.hpp"
#include "servercommon/struct/battle/battle_def.hpp"

#pragma pack(push, 4)

static const int FACE_ADDITION_TYPE_MAX = 10;		// ��ֵ�ӳ����
static const int FACE_RESOURCE_NUM_MAX = 1023;		// ��ֵϵͳ ��Դ���� ��� 1023 ��
static const int MAX_FACE_RECORD_LEN = sizeof(short) + (sizeof(int) + sizeof(int)) * FACE_RESOURCE_NUM_MAX;	// ��¼����

enum FACE_SCORE_TYPE
{
	FACE_CHATFRAME = 0,			// ����� | ð�ݿ�
	FACE_SIGNATURE = 1,			// ǩ���� | β��
	FACE_TITLENAME = 2,			// ͷ��	  ��ʱ�˴���ͷ�θ��ƺ��Ǻϲ���

	FACE_TYPE_MAX
};
UNSTD_STATIC_CHECK(FACE_TYPE_MAX <= FACE_ADDITION_TYPE_MAX)		// �����չ�� 10 ��

struct FaceScoreParam
{
	FaceScoreParam() { this->Reset(); }

	void Reset()
	{
		memset(face_equip_list, -1, sizeof(face_equip_list));		//seq ����ʼֵΪ0,����������Ҫ�޸�Ϊ-1	
		active_flag.Reset();
		is_first_change_equip_list_reset = 0;
		reserve_char = 0;
		reserve_short = 0;
		face_level = -1;
	}

	int face_equip_list[FACE_ADDITION_TYPE_MAX];	// ��װ���ģ�����seq������鿴��ֵϵͳ���ñ�
	BitMap<FACE_RESOURCE_NUM_MAX> active_flag;		// ������
	char is_first_change_equip_list_reset;			// �Ƿ��ǵ�һ�θ�������Ϊ-1, 0:δ���� 1:��ǰ������ 2:������(����װ����Ϣ) 
	char reserve_char;
	short reserve_short;	
	int face_level;									// ��ǰ�ȼ�
	
	
};
typedef char FaceScoreParamHex[sizeof(FaceScoreParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FaceScoreParamHex) < 512);

/****************** ʱ������ ***************************/
struct FaceScoreTimeParam 
{
	FaceScoreTimeParam() { this->Reset(); }

	void Reset()
	{
		memset(times, 0, sizeof(times));
	}

	unsigned int times[FACE_RESOURCE_NUM_MAX];		// [seq] = ����ʱ���
};
typedef char FaceScoreTimeParamHex[sizeof(FaceScoreTimeParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FaceScoreTimeParamHex) < 9216);

#pragma pack(pop)
#endif