#ifndef __RA_SEVEN_COLOR_PICTURE_PARAM_HPP__
#define __RA_SEVEN_COLOR_PICTURE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2058 随机活动 - 七彩图鉴  //////////////////////////////////////////

enum SEVEN_COLOR_PIGMENT_TYPE				//七彩图鉴花灵颜料品级
{
	SEVEN_COLOR_PIGMENT_TOP_TYPE = 0,		//上品
	SEVEN_COLOR_PIGMENT_MIDDLE_TYPE,		//中品
	SEVEN_COLOR_PIGMENT_UNDER_TYPE,			//下品

	SEVEN_COLOR_PIGMENT_TYPE_MAX
};

enum SEVEN_COLOR_PIGMENT_COLOR_TYPE			//七彩图鉴花灵颜料颜色
{
	SEVEN_COLOR_PIGMENT_COLOR_SCARLET = 0,	//赤色
	SEVEN_COLOR_PIGMENT_COLOR_ORANGE,		//橙色
	SEVEN_COLOR_PIGMENT_COLOR_YELLOW,		//黄色
	SEVEN_COLOR_PIGMENT_COLOR_GREEN,		//绿色
	SEVEN_COLOR_PIGMENT_COLOR_CYAN_BLUE,	//青色
	SEVEN_COLOR_PIGMENT_COLOR_BLUE,			//蓝色
	SEVEN_COLOR_PIGMENT_COLOR_PURPLE,		//紫色

	SEVEN_COLOR_PIGMENT_COLOR_TYPE_MAX
};

//-----------------------------七彩图鉴------------------------------------
static const int SEVEN_COLOR_PICTURE_AWARD__LEVEL_MAX_NUM = 12;
static const int SEVEN_COLOR_PICTURE_SCORE_MAX_NUM = 7;
static const int SEVEN_COLOR_PICTURE_APPLY_MAX_NUM = 9;
static const int SEVEN_COLOR_PICTURE_AREA_MAX_NUM = 64;		//只有7*9个字节需要使用	剩下1个字节置0,防止客户端插入恶意代码

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

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
	short ra_seven_color_picture_pigment[SEVEN_COLOR_PIGMENT_COLOR_TYPE_MAX][SEVEN_COLOR_PIGMENT_TYPE_MAX];  //保存的花灵
	short ra_seven_color_picture_score[SEVEN_COLOR_PICTURE_SCORE_MAX_NUM];		//保存每个图鉴的评分
	int ra_seven_color_picture_apply[SEVEN_COLOR_PICTURE_SCORE_MAX_NUM];		//保存每个图鉴当前填涂了多少块

	BitMap<SEVEN_COLOR_PICTURE_SCORE_MAX_NUM> seven_color_picture_flag;			//图鉴奖励领取标识 (7个图鉴0-6) 外加一个全集齐奖励(7)

	int ra_seven_color_picutre_role_level;										//活动开启时角色的等级
	char ra_seven_color_area_data[SEVEN_COLOR_PICTURE_AREA_MAX_NUM];			//单纯的给客户端用,客户端解析
	char reserve_ch[80];														//保留

};

UNSTD_STATIC_CHECK(sizeof(RASevenColorPictureParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_SEVEN_COLOR_PICTURE_PARAM_HPP__
