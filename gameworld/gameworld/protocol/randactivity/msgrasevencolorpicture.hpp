#ifndef __MSG_RA_SEVEN_COLOR_PICTURE_HPP__
#define __MSG_RA_SEVEN_COLOR_PICTURE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rasevencolorpictureparam.hpp"

//////////////////////////////////////////// 2058 ���� -- �߲�ͼ��  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRASevenColorPictureInfo			// 3014 �߲�ͼ������Ϣ�·�
	{
	public:
		SCRASevenColorPictureInfo();
		MessageHeader header;

		short seven_color_picture_pigment[SEVEN_COLOR_PIGMENT_COLOR_TYPE_MAX][SEVEN_COLOR_PIGMENT_TYPE_MAX];  //����Ļ���
		unsigned char role_level;												//�����ʱ��ɫ�ĵȼ�
		char is_fetch_make_up_flag;												//��ȡ���������ʶ  0:δ���� 1:�Ѳ���
		char ra_seven_color_area_data[SEVEN_COLOR_PICTURE_AREA_MAX_NUM];        //�ͻ���ʹ��,����˱���,�ͻ�����Ҫʱ�ṩ

	};

	enum SEVEN_COLOR_PICTURE_REQ_TYPE
	{
		SEVEN_COLOR_PICTURE_REQ_TYPE_INFO = 0,				//��������Ϣ�·�		
		SEVEN_COLOR_PICTURE_REQ_TYPE_USE_PIGMENT,			//ʹ�û���(����)	param1 param2 param3 param4
	};

	struct CSRASevenColorReq			// 3021 �߲�ͼ������  �߲ʲ�ʹ��ͨ������Э������(3000) 
	{
	public:
		CSRASevenColorReq();
		MessageHeader header;

		char op_type;
		char index;							//����ͼ 0 - 6 
		char seven_color_picture_pigment[SEVEN_COLOR_PIGMENT_COLOR_TYPE_MAX][SEVEN_COLOR_PIGMENT_TYPE_MAX];  //����������ʹ�õĻ���
		char reserve;
		char ra_seven_color_area_data[SEVEN_COLOR_PICTURE_AREA_MAX_NUM];        //�ͻ���ʹ��,����˱���,�ͻ�����Ҫʱ�ṩ
	};

	struct SCRASevenColorTypeInfo        // 3022 �߲�ͼ������仯ʱʹ��
	{
	public:
		SCRASevenColorTypeInfo();
		MessageHeader header;

		char seq;					//������ɫ
		char quality;				//�ĸ�Ʒ��
		short num;					//�����ϵ�����
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_SEVEN_COLOR_PICTURE_HPP__
