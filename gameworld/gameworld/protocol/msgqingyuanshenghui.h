#ifndef __MSG_QING_YUAN_SHENG_HUI_HPP__
#define __MSG_QING_YUAN_SHENG_HUI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/commonsavedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum QING_YUAN_SHENG_HUI_REQ_TYPE
	{
		QING_YUAN_SHENG_HUI_REQ_TYPE_YU_YUE,  //预约
		QING_YUAN_SHENG_HUI_REQ_TYPE_ENTER,  //进入场景
		QING_YUAN_SHENG_HUI_REQ_TYPE_GATHER,	//采集
		QING_YUAN_SHENG_HUI_REQ_TYPE_QUESTION_OPEN,	//打开答题界面	其中一人请求双方收到4654，4655
		QING_YUAN_SHENG_HUI_REQ_TYPE_QUESTION_ANSWER,	//答题 p1 选项
		QING_YUAN_SHENG_HUI_REQ_TYPE_QUESTION_CLOSE,	//关闭答题界面	其中一人请求双方收到4654
		QING_YUAN_SHENG_HUI_REQ_TYPE_PI_PEI_OPEN,	//打开匹配界面
		QING_YUAN_SHENG_HUI_REQ_TYPE_PI_PEI_CLOSE,	//关闭匹配界面
		QING_YUAN_SHENG_HUI_REQ_TYPE_PI_PEI,		//开始匹配 P1 是否匹配
		QING_YUAN_SHENG_HUI_REQ_TYPE_QI_YUAN,	//祈愿
		QING_YUAN_SHENG_HUI_REQ_TYPE_BATTLE_REWARD,	//领取战斗奖励 p1 次数
		QING_YUAN_SHENG_HUI_REQ_TYPE_INFO,	// 请求信息 ,4658下发		
	};

	class CSQingYuanShengHuiReq			//!< 4650
	{
	public:
		CSQingYuanShengHuiReq();
		MessageHeader header;

		int req_type;
		int p1;
		int p2;
	};

	class SCQingYuanShengHuiSceneInfo	//!< 4651
	{
	public:
		SCQingYuanShengHuiSceneInfo();
		MessageHeader header;

		int phase;		//当前阶段
		int phase_start;
		int phase_end;
		int role_num;
	};

	class SCQingYuanShengHuiRoleInfo			//!< 4652
	{
	public:
		SCQingYuanShengHuiRoleInfo();
		MessageHeader header;

		unsigned int can_gather_next_time;		//下次能采集时间
		short battle_times;		//战斗次数
		short is_get_qi_yuan_rewards;		//是否已经祈愿了
		int battle_reward_flag;		//获取战斗奖励标志
	};

	class SCQingYuanShengHuiQuestionResult			//!< 4653 答题结果
	{
	public:
		SCQingYuanShengHuiQuestionResult();
		MessageHeader header;

		int is_same;				//双方选择是否相同
		int question_id;
	};

	class SCQingYuanShengHuiQuestionRoute			//!< 4654
	{
	public:
		SCQingYuanShengHuiQuestionRoute();
		MessageHeader header;

		int is_open;
	};

	class SCQingYuanShengHuiQuestionInfo		//!< 4655	题目信息	
	{
	public:
		SCQingYuanShengHuiQuestionInfo();
		MessageHeader header;
			
		int question_id;			//题目id	
		unsigned int end_time;		//当前题目时间
		int quest_num;
	};


	class SCQingYuanShengHuiOpponentInfo 		//!< 4656 匹配到对手信息		
	{
	public:
		SCQingYuanShengHuiOpponentInfo();
		MessageHeader header;

		unsigned int start_battle_time;		//开始战斗时间,匹配到人后多久开打，时间戳
		MsgRoleBaseInfo role_info[2];
	};


	class SCQingYuanShengHuiMatchOpRoute			//!< 4657
	{
	public:
		SCQingYuanShengHuiMatchOpRoute();
		MessageHeader header;

		short is_open;
		short is_match;
		unsigned int begin_time;
	};

	class SCQingYuanShengHuiInfo			//!< 4658
	{
	public:
		SCQingYuanShengHuiInfo();
		MessageHeader header;
		
		int is_yu_yue;		//是否已经预约了
		int role_num;
	};

	class SCQingYuanShengHuiQiYuanNotice 		//!< 4659
	{
	public:
		SCQingYuanShengHuiQiYuanNotice();
		MessageHeader header;

		int x;
		int y;
		unsigned int begin_time;
	};



	class CSQingYuanShengHuiReq2			//!< 2520
	{
	public:
		CSQingYuanShengHuiReq2();
		MessageHeader header;

		int length;
		char text[2048];		//许愿文字
	};
}

#pragma pack(pop)

#endif