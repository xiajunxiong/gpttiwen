#ifndef __MARRIAGE_DEF_HPP__
#define __MARRIAGE_DEF_HPP__
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

enum MARRIAGE_TASK_TYPE
{
	MARRIAGE_TASK_1 = 1,	//快乐拼图
	MARRIAGE_TASK_2,		//心有灵犀
	MARRIAGE_TASK_3,		//共渡难关

	MARRIAGE_TASK_4,		//快乐拼图
	MARRIAGE_TASK_5,		//育儿心得
};

const static int MARRIAGE_SEEKING_MAX_COUNT = 50;			// 征婚宣言最大个数

struct MarriageParam
{
	MarriageParam() { this->Reset(); }
	void Reset()
	{
		lover_uid = 0;
		memset(lover_name, 0, sizeof(lover_name));
		last_marry_time = 0;
		last_divorce_time = 0;
		apply_divorce_time = 0;

		puzzle_play_times = 0;
		question_play_times = 0;

		max_pass_fb_wave = 0;
		curr_wave = 0;
		is_auto_fb = 0;
		reserved_ch = 0;
	}

	int lover_uid;								// 伴侣UID
	GameName lover_name;						// 伴侣名字
	unsigned int last_marry_time;					// 结婚时间
	unsigned int last_divorce_time;					// 离婚时间
	unsigned int apply_divorce_time;				// 申请离婚时间

	short puzzle_play_times;			//快乐拼图玩的次数
	short question_play_times;			//答题玩的次数

	int max_pass_fb_wave;		//最大波数
	short curr_wave;			//当前波数
	char is_auto_fb;				//是否扫荡
	char reserved_ch;
};

typedef char MarryParamHex[sizeof(MarriageParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MarryParamHex) < 1024);

#pragma pack(pop)

#endif // !__MARRIAGE_DEF_HPP__
