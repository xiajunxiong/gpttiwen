#ifndef __MSG_RA_SEVEN_COLOR_PICTURE_HPP__
#define __MSG_RA_SEVEN_COLOR_PICTURE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rasevencolorpictureparam.hpp"

//////////////////////////////////////////// 2058 随机活动 -- 七彩图鉴  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	struct SCRASevenColorPictureInfo			// 3014 七彩图鉴总信息下发
	{
	public:
		SCRASevenColorPictureInfo();
		MessageHeader header;

		short seven_color_picture_pigment[SEVEN_COLOR_PIGMENT_COLOR_TYPE_MAX][SEVEN_COLOR_PIGMENT_TYPE_MAX];  //保存的花灵
		unsigned char role_level;												//活动开启时角色的等级
		char is_fetch_make_up_flag;												//领取奖励补齐标识  0:未补齐 1:已补齐
		char ra_seven_color_area_data[SEVEN_COLOR_PICTURE_AREA_MAX_NUM];        //客户端使用,服务端保存,客户端需要时提供

	};

	enum SEVEN_COLOR_PICTURE_REQ_TYPE
	{
		SEVEN_COLOR_PICTURE_REQ_TYPE_INFO = 0,				//请求总信息下发		
		SEVEN_COLOR_PICTURE_REQ_TYPE_USE_PIGMENT,			//使用花灵(颜料)	param1 param2 param3 param4
	};

	struct CSRASevenColorReq			// 3021 七彩图鉴请求  七彩不使用通用随机活动协议请求(3000) 
	{
	public:
		CSRASevenColorReq();
		MessageHeader header;

		char op_type;
		char index;							//哪张图 0 - 6 
		char seven_color_picture_pigment[SEVEN_COLOR_PIGMENT_COLOR_TYPE_MAX][SEVEN_COLOR_PIGMENT_TYPE_MAX];  //本次填入所使用的花灵
		char reserve;
		char ra_seven_color_area_data[SEVEN_COLOR_PICTURE_AREA_MAX_NUM];        //客户端使用,服务端保存,客户端需要时提供
	};

	struct SCRASevenColorTypeInfo        // 3022 七彩图鉴花灵变化时使用
	{
	public:
		SCRASevenColorTypeInfo();
		MessageHeader header;

		char seq;					//哪种颜色
		char quality;				//哪个品质
		short num;					//该颜料的数量
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_SEVEN_COLOR_PICTURE_HPP__
