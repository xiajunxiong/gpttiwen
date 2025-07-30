#ifndef __MSG_RA_JING_JI_YUE_KA_HPP__
#define __MSG_RA_JING_JI_YUE_KA_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rajingjiyuekaparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	//竞技月卡
	enum RA_JINGJI_YUEKA_REQ_TYPE		//由协议3000
	{
		RA_JINGJI_YUEKA_REQ_TYPE_INFO = 0,							// 下发信息		
		RA_JINGJI_YUEKA_REQ_TYPE_FETCH_COMMON = 1,					// 领取普通奖励 P1:card_level
		RA_JINGJI_YUEKA_REQ_TYPE_FETCH_SENIOR = 2,					// 领取高级奖励 P1:card_level
		RA_JINGJI_YUEKA_REQ_TYPE_BUY_LEVEL = 3,						// 购买等级 P1:buy_level
		RA_JINGJI_YUEKA_REQ_TYPE_REFRESH_TASK = 4,					// 刷新每日任务 P1:task_index
		RA_JINGJI_YUEKA_REQ_TYPE_BUY_CMD_ITEM_CONFIG_CHECK = 5,		// 购买直购前请求这个进行配置检测 
	};
	
	class SCRAJingJiYueKaAllInfo : public IMessageSerializer	//!< 3140 随机活动-竞技月卡所有信息下发
	{
	public:
		SCRAJingJiYueKaAllInfo();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(active_role_level, buffer, buffer_size, out_length);
			result = result && WriteUInt(next_switch_timestamp, buffer, buffer_size, out_length);
			result = result && WriteUInt(last_switch_timestamp, buffer, buffer_size, out_length);

			result = result && WriteChar(is_active_buy_flag, buffer, buffer_size, out_length);
			result = result && WriteShort(role_card_level, buffer, buffer_size, out_length);
			result = result && WriteInt(role_card_exp, buffer, buffer_size, out_length);

			UNSTD_STATIC_CHECK(MAX_RA_JING_JI_YUE_KA_LEVEL_NUM == 64);
			UNSTD_STATIC_CHECK(sizeof(fetch_common_flag) == sizeof(fetch_adv_flag));
			result = result && WriteShort((short)sizeof(fetch_common_flag), buffer, buffer_size, out_length);
			for (int k = 0; k < ARRAY_LENGTH(fetch_common_flag.byte_list); k++)
			{
				result = result && WriteUChar(fetch_common_flag.byte_list[k], buffer, buffer_size, out_length);
			}
			for (int k = 0; k < ARRAY_LENGTH(fetch_adv_flag.byte_list); k++)
			{
				result = result && WriteUChar(fetch_adv_flag.byte_list[k], buffer, buffer_size, out_length);
			}

			result = result && WriteShort(day_task_count, buffer, buffer_size, out_length);
			for (int i = 0; i < day_task_count && i < ARRAY_LENGTH(day_task_list); ++i)
			{
				result = result && WriteShort(day_task_list[i].task_id, buffer, buffer_size, out_length);
				result = result && WriteChar(day_task_list[i].finish_flag, buffer, buffer_size, out_length);
				result = result && WriteUChar(day_task_list[i].refresh_num, buffer, buffer_size, out_length);
			}
			result = result && WriteShort(week_task_count, buffer, buffer_size, out_length);
			for (int i = 0; i < week_task_count && i < ARRAY_LENGTH(week_task_list); ++i)
			{
				result = result && WriteShort(week_task_list[i].task_id, buffer, buffer_size, out_length);
				result = result && WriteChar(week_task_list[i].finish_flag, buffer, buffer_size, out_length);
				result = result && WriteUChar(week_task_list[i].refresh_num, buffer, buffer_size, out_length);
			}
			result = result && WriteShort(season_task_count, buffer, buffer_size, out_length);
			for (int i = 0; i < season_task_count && i < ARRAY_LENGTH(season_task_list); ++i)
			{
				result = result && WriteShort(season_task_list[i].task_id, buffer, buffer_size, out_length);
				result = result && WriteChar(season_task_list[i].finish_flag, buffer, buffer_size, out_length);
				result = result && WriteUChar(season_task_list[i].refresh_num, buffer, buffer_size, out_length);
			}

			UNSTD_STATIC_CHECK(RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_MAX == MAX_RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_NUM);
			UNSTD_STATIC_CHECK(MAX_RA_JING_JI_YUE_KA_TASK_TYPE_NUM >= RA_JING_JI_YUE_KA_TASK_TYPE_MAX);

			result = result && WriteShort(RA_JING_JI_YUE_KA_TASK_TYPE_MAX, buffer, buffer_size, out_length);
			for (int i = 0; i < MAX_RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_NUM && i < ARRAY_LENGTH(progress_list); ++i)
			{
				for (int k = 0; k < RA_JING_JI_YUE_KA_TASK_TYPE_MAX && k < ARRAY_LENGTH(progress_list[i]); ++k)
				{
					result = result && WriteUInt(progress_list[i][k], buffer, buffer_size, out_length);
				}
			}

			return result;
		}


		int active_role_level;					//激活该赛季时角色等级
		unsigned int next_switch_timestamp;		//下次切换状态时间戳	
		unsigned int last_switch_timestamp;		//上次切换状态时间戳

		char is_active_buy_flag;				//是否直购激活了额外奖励(江湖引) 0:未激活 1:已激活
		char reserve_ch;
		short role_card_level;					//月卡等级
		int role_card_exp;						//月卡经验
		BitMap<MAX_RA_JING_JI_YUE_KA_LEVEL_NUM> fetch_common_flag;				//普通等级奖励领取标志位
		BitMap<MAX_RA_JING_JI_YUE_KA_LEVEL_NUM> fetch_adv_flag;					//高级等级奖励领取标志位

		short day_task_count;
		short week_task_count;
		short season_task_count;
		short reserve_sh;
		RAJingJiYueKaTaskInfo day_task_list[MAX_RA_JING_JI_YUE_KA_DAY_TASK_NUM];
		RAJingJiYueKaTaskInfo week_task_list[MAX_RA_JING_JI_YUE_KA_WEEK_TASK_NUM];
		RAJingJiYueKaTaskInfo season_task_list[MAX_RA_JING_JI_YUE_KA_SEASON_TASK_NUM];

		unsigned int progress_list[MAX_RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_NUM][MAX_RA_JING_JI_YUE_KA_TASK_TYPE_NUM];
	};

	class SCRAJingJiYueKaNotice		//!< 3141 随机活动-竞技月卡相关通知    
	{
	public:
		SCRAJingJiYueKaNotice();
		MessageHeader header;

		enum NOTICE_TYPE
		{
			NOTICE_TYPE_FETCH_COMMON_SUCC = 0,		//领取普通奖励成功 P1:card_level
			NOTICE_TYPE_FETCH_SENIOR_SUCC = 1,		//领取高级奖励成功 P1:card_level
			NOTICE_TYPE_BUY_LEVEL = 2,				//购买等级 P1:cur_role_card_level
			NOTICE_TYPE_BUY_CMD_SUCC = 3,			//直购购买成功 P1:is_active_buy_flag
			NOTICE_TYPE_DAY_TASK_FINISH = 4,		//每日任务完成 P1:task_index
			NOTICE_TYPE_WEEK_TASK_FINISH = 5,		//每周任务完成 P1:task_index
			NOTICE_TYPE_SEASON_TASK_FINISH = 6,		//赛季任务完成 P1:task_index
			NOTICE_TYPE_LEVEL_UP = 7,				//等级提升 P1:cur_role_card_level
			NOTICE_TYPE_EXP_CHANGE = 8,				//经验变化 P1:role_card_exp

			NOTICE_TYPE_MAX
		};

		int notice_type;
		int param1;
	};

	class SCRAJingJiYueKaRefreshNotice		//!< 3142 随机活动-竞技月卡任务刷新下发
	{
	public:
		SCRAJingJiYueKaRefreshNotice();
		MessageHeader header;

		int task_index;						//接取任务数组序号
		RAJingJiYueKaTaskInfo task_info;	//任务进度
	};

	class SCRAJingJiYueKaSingleTypeInfo		//!< 3143 随机活动-竞技月卡单条条件信息下发  
	{
	public:
		SCRAJingJiYueKaSingleTypeInfo();
		MessageHeader header;

		int task_type;
		unsigned int progress_list[MAX_RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_NUM];		//条件进度
	};

	class SCRAJingJiYueKaTaskListInfo		//!< 3144 随机活动-竞技月卡任务列表信息下发
	{
	public:
		SCRAJingJiYueKaTaskListInfo();
		MessageHeader header;

		short task_group_type;				//RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_MAX
		short task_count;					
		RAJingJiYueKaTaskInfo task_list[MAX_RA_JING_JI_YUE_KA_TASK_COUNT];		//任务列表	MAX_RA_JING_JI_YUE_KA_TASK_COUNT -- 20
	};

}


#pragma pack(pop)

#endif // __MSG_RA_JING_JI_YUE_KA_HPP__