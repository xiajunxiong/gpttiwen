#ifndef __MSG_PEAK_TOKEN_HPP__
#define __MSG_PEAK_TOKEN_HPP__


#include "servercommon/userprotocal/msgheader.h"


#pragma pack(push) 
#pragma pack(4)


namespace Protocol
{
	//////天下至尊令
	//玩家操作  4455
	class CSPeaktokenRoleReq
	{
	public:
		CSPeaktokenRoleReq();
		MessageHeader		header;

		enum ReqType
		{
			REQ_TYPE_ROLE_INFO = 0,		//角色数据
			REQ_TYPE_ROLE_JOIN,			//参与活动 p_1:活动雕像obj_id
			REQ_TYPE_ROLE_FETCH,		//领取奖励 p_1:活动雕像obj_id
		};

		int req_type;		//操作类型
		int param_1;
	};
	//角色参与通知 4457
	class SCPeaktokenRoleStatusInfo
	{
	public:
		SCPeaktokenRoleStatusInfo();
		MessageHeader		header;

		long long peak_token_act_id;	//活动id
		unsigned int role_join_time;	//参与时间
		int reward_times;				//奖励次数

	};
	//角色奖励通知 4458
	class SCPeaktokenRoleJoinInfo
	{
	public:
		SCPeaktokenRoleJoinInfo();
		MessageHeader		header;

		long long peak_token_act_id;	//活动id
		unsigned int role_join_time;	//参与时间
		int reward_times;		//奖励次数

		ItemID item_id;			//奖励id
		short num;				//奖励数量
	};

	//玩家信息  4459
	class SCPeaktokenRoleInfo
	{
	public:
		SCPeaktokenRoleInfo();
		MessageHeader		header;

		int today_use_times;	//今日使用次数
	};
};

#pragma pack(pop)

#endif