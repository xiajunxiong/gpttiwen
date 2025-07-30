#ifndef __MSG_RA_YUE_XI_DENG_MI_HPP__
#define __MSG_RA_YUE_XI_DENG_MI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rayuexidengmiparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{

	//////////////////////////////////////// 月夕灯谜 ///////////////////////////////////////////////////
	static const int RA_YXDM_MAX_USER_ANSWER_LENGTH = 16;	// 月夕灯谜答案最长长度

	class CSRAYueXiDengMiRequest	//!<5320 随机活动-月夕灯谜 请求
	{
	public:
		enum RA_YUE_XI_DENG_MI_REQ_TYPE
		{
			RA_YUE_XI_DENG_MI_REQ_TYPE_INFO = 0,		// 请求信息
			RA_YUE_XI_DENG_MI_REQ_TYPE_START = 1,		// 请求开始答题
			RA_YUE_XI_DENG_MI_REQ_TYPE_ANSWER = 2,		// 答题 user_answer:玩家答案
		};
	public:
		CSRAYueXiDengMiRequest();
		MessageHeader header;

		int op_type;
		char user_answer[RA_YXDM_MAX_USER_ANSWER_LENGTH];
	};

	class SCRAYueXiDengMiInfo		//!<5321 随机活动-月夕灯谜 活动信息下发
	{
	public:
		SCRAYueXiDengMiInfo();
		MessageHeader header;

		int cur_question_id;	// 当前题目id
		int today_answer_count;	// 今日已答题数量
	};

	class SCRAYueXiDengMiAnswerResult		//!<5323 随机活动-月夕灯谜 答题结果下发
	{
	public:
		SCRAYueXiDengMiAnswerResult();
		MessageHeader header;

		int is_true;		// 回答是否正确, 0-错误, 1-正确
	};


	//////////////////////////////////////// 月夕累充 ///////////////////////////////////////////////////
	//!<3000 随机活动请求操作
	enum RA_YUE_XI_LEI_CHONG_REQ_TYPE					//对应 3000协议的 opera_type
	{
		RA_YUE_XI_LEI_CHONG_REQ_TYPE_INFO = 0,				// 请求信息
		RA_YUE_XI_LEI_CHONG_REQ_TYPE_RECEIVE_REWARD = 1,	// 请求领取奖励, param1-seq
	};

	class SCRAYueXiLeiChongInfo							//<! 5322 随机活动-月夕累充 活动信息下发
	{
	public:
		SCRAYueXiLeiChongInfo();
		MessageHeader header;

		int m_total_chongzhi;		// 活动期间总充值金额
		BitMap<MAX_RA_YUE_XI_LEI_CHONG_BITMAP> m_receive_reward_flag;		// 奖励领取flag
	};



}

#pragma pack(pop)

#endif  
