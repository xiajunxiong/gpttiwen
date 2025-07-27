#ifndef __RA_SEVEN_COLOR_PICTURE_PARAM_HPP__
#define __RA_SEVEN_COLOR_PICTURE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2058 ���� - �߲�ͼ��  //////////////////////////////////////////

enum SEVEN_COLOR_PIGMENT_TYPE				//�߲�ͼ����������Ʒ��
{
	SEVEN_COLOR_PIGMENT_TOP_TYPE = 0,		//��Ʒ
	SEVEN_COLOR_PIGMENT_MIDDLE_TYPE,		//��Ʒ
	SEVEN_COLOR_PIGMENT_UNDER_TYPE,			//��Ʒ

	SEVEN_COLOR_PIGMENT_TYPE_MAX
};

enum SEVEN_COLOR_PIGMENT_COLOR_TYPE			//�߲�ͼ������������ɫ
{
	SEVEN_COLOR_PIGMENT_COLOR_SCARLET = 0,	//��ɫ
	SEVEN_COLOR_PIGMENT_COLOR_ORANGE,		//��ɫ
	SEVEN_COLOR_PIGMENT_COLOR_YELLOW,		//��ɫ
	SEVEN_COLOR_PIGMENT_COLOR_GREEN,		//��ɫ
	SEVEN_COLOR_PIGMENT_COLOR_CYAN_BLUE,	//��ɫ
	SEVEN_COLOR_PIGMENT_COLOR_BLUE,			//��ɫ
	SEVEN_COLOR_PIGMENT_COLOR_PURPLE,		//��ɫ

	SEVEN_COLOR_PIGMENT_COLOR_TYPE_MAX
};

//-----------------------------�߲�ͼ��------------------------------------
static const int SEVEN_COLOR_PICTURE_AWARD__LEVEL_MAX_NUM = 12;
static const int SEVEN_COLOR_PICTURE_SCORE_MAX_NUM = 7;
static const int SEVEN_COLOR_PICTURE_APPLY_MAX_NUM = 9;
static const int SEVEN_COLOR_PICTURE_AREA_MAX_NUM = 64;		//ֻ��7*9���ֽ���Ҫʹ��	ʣ��1���ֽ���0,��ֹ�ͻ��˲���������

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RASevenColorPictureParam
{
	RASevenColorPictureParam() { this->Reset(); }
	void Reset()
	{
		ra_seven_color_picture_begin_timestamp = 0;
		memset(ra_seven_color_picture_pigment, 0, sizeof(ra_seven_color_picture_pigment));
		memset(ra_seven_color_picture_score, 0, sizeof(ra_seven_color_picture_score));
		memset(ra_seven_color_picture_apply, 0, sizeof(ra_seven_color_picture_apply));

		seven_color_picture_flag.Reset();

		ra_seven_color_picutre_role_level = 0;
		memset(ra_seven_color_area_data, 0, sizeof(ra_seven_color_area_data));
		memset(reserve_ch, 0, sizeof(reserve_ch));
	}

	unsigned int ra_seven_color_picture_begin_timestamp;
	short ra_seven_color_picture_pigment[SEVEN_COLOR_PIGMENT_COLOR_TYPE_MAX][SEVEN_COLOR_PIGMENT_TYPE_MAX];  //����Ļ���
	short ra_seven_color_picture_score[SEVEN_COLOR_PICTURE_SCORE_MAX_NUM];		//����ÿ��ͼ��������
	int ra_seven_color_picture_apply[SEVEN_COLOR_PICTURE_SCORE_MAX_NUM];		//����ÿ��ͼ����ǰ��Ϳ�˶��ٿ�

	BitMap<SEVEN_COLOR_PICTURE_SCORE_MAX_NUM> seven_color_picture_flag;			//ͼ��������ȡ��ʶ (7��ͼ��0-6) ���һ��ȫ���뽱��(7)

	int ra_seven_color_picutre_role_level;										//�����ʱ��ɫ�ĵȼ�
	char ra_seven_color_area_data[SEVEN_COLOR_PICTURE_AREA_MAX_NUM];			//�����ĸ��ͻ�����,�ͻ��˽���
	char reserve_ch[80];														//����

};

UNSTD_STATIC_CHECK(sizeof(RASevenColorPictureParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_SEVEN_COLOR_PICTURE_PARAM_HPP__
