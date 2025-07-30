#ifndef __MSG_WAN_LING_FN_H__
#define __MSG_WAN_LING_FN_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/commondata.hpp"
#include "servercommon/servercommon.h"
#include "gamedef.h"

#include "servercommon/wanlingfbdef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{//万灵话本
	//用户操作
	class CSWanLingFBReq
	{
	public:
		CSWanLingFBReq();
		MessageHeader		header;

		enum ReqType
		{
			REQ_TYPE_TASK_FINISH = 0,	//FB-任务完成(客户端伪任务)
			REQ_TYPE_FB_FETCH_SET,//设置领奖标记 p_1:副本_index p_2:0可领 1不可领

			REQ_TYPE_TASK_FIGHT,		//FB-进入当前任务的战斗
		};

		int req_type;		//操作类型
		int param_1;
		int param_2;
	};

	//角色信息
	class SCWanLingFBRoleInfo
	{
	public:
		SCWanLingFBRoleInfo();
		MessageHeader		header;

		int day_fetch_battle_reward;	//今日领取战斗奖励次数
		int day_fetch_pass_reward;		//今日领取通关奖励次数
		int fb_fetch_flag;				//副本领奖标记[副本_index]
	};

	//副本信息
	class SCWanLingFBInfo
	{
	public:
		SCWanLingFBInfo();
		MessageHeader		header;

		int m_fb_index;		//副本index
		int m_is_finish;	//副本完成标记

		int m_task_seq;		//当前任务
	};
};

#pragma pack(pop)

#endif