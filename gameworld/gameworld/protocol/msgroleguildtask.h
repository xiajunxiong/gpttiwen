#ifndef __MSG_ROLE_GUILD_TASK_HPP__
#define __MSG_ROLE_GUILD_TASK_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/roleguildtaskdef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum ROLE_GUILD_TASK_REQ_TYPE
	{
		ROLE_GUILD_TASK_REQ_TYPE_ACCEPT,		//接受 p1 5401协议index
		ROLE_GUILD_TASK_REQ_TYPE_GIVE_UP,		//放弃 p1 5401协议index		
		ROLE_GUILD_TASK_REQ_TYPE_SOLICIT,		//请求帮助 
		ROLE_GUILD_TASK_REQ_TYPE_GIVE,		//帮助 p1 role_id
		ROLE_GUILD_TASK_REQ_TYPE_REFRESH,		//刷新
		ROLE_GUILD_TASK_REQ_TYPE_BATTLE,		//打怪任务的打怪，发这个请求
		ROLE_GUILD_TASK_REQ_TYPE_SEND_MAIL,		//发送提醒邮件 p1 是否一键 p2 role_id
		ROLE_GUILD_TASK_REQ_TYPE_FETCH,		//领取赠予奖励
	};

	class CSRoleGuildTaskReq					//!< 5400
	{
	public:
		CSRoleGuildTaskReq();
		MessageHeader		header;

		int req_type;
		int param1;
		int param2;
	};

	class SCRoleGuildTaskInfo				//!< 5401	
	{
	public:
		SCRoleGuildTaskInfo();
		MessageHeader		header;

		int finish_times;	//完成次数
		int task_index;	//当前进行任务index
		RoleGuildTaskItem task_list[ROLE_GUILD_TASK_MAX_NUM];		//!< 6 任务列表
	};

	enum ROLE_GUILD_TASK_CHANGE_REASON
	{
		ROLE_GUILD_TASK_CHANGE_REASON_START,	//任务开始
		ROLE_GUILD_TASK_CHANGE_REASON_FINISH,  //任务完成
		ROLE_GUILD_TASK_CHANGE_REASON_GIVE_UP,	//任务放弃
		ROLE_GUILD_TASK_CHANGE_REASON_SOLICIT,	//征集
	};

	class SCRoleGuildTaskChange			//!< 5402	列表变化A
	{
	public:
		SCRoleGuildTaskChange();
		MessageHeader		header;

		int reason;
		int task_index;
		RoleGuildTaskItem task;		
	};
}

#pragma pack(pop)

#endif 

