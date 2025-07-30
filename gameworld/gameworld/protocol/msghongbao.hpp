#ifndef __MSG_HONG_BAO_H__
#define __MSG_HONG_BAO_H__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/hongbaodef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSHongBaoClientReq					//!< 4000 红包 - 客户端请求
	{
	public:
		CSHongBaoClientReq();
		MessageHeader header;

		enum OPERA_TYPE
		{
			OPERA_TYPE_SEND_LIST_REQ = 0,		//!< 0 请求可发送列表信息		[]
			OPERA_TYPE_GET_LIST_REQ = 1,		//!< 1 请求可领取列表信息		[param_1:红包系统类型]
			OPERA_TYPE_BASE_INFO_REQ = 2,		//!< 2 请求单个红包的详细信息	[param_1:红包系统类型 , param_2:红包ID]
			OPERA_TYPE_SEND_HONG_BAO = 3,		//!< 3 发红包 [param_1:红包系统类型 , param_2:红包类型 , greeting: 红包祝福语]
			OPERA_TYPE_GET_HONG_BAO = 4,		//!< 4 领红包 [param_1:红包系统类型 , param_2:红白ID]		
		};

		int req_type;							//!< 消息类型  \see OPERA_TYPE
		int param1;
		int param2;
		HongBaoGreeting greeting;				// 红包祝福语
	};


	class SCHongBaoSendList						//!< 4001 红包 - 可发送列表信息
	{
	public:
		SCHongBaoSendList();
		MessageHeader header;

		struct SendInfo
		{
			SendInfo():type(0), count(0), day_count(0), total_count(0){}

			int type;							// 可发送的红包类型
			int count;
			int day_count;						// 每日已获得的可发送红包数量
			int total_count;					// 累计已获得的可发送红包数量
		};

		int count;								// 可发送列表表长
		SendInfo send_list[MAX_HONG_BAO_TYPE_COUNT];
	};

	class SCHongBaoGetList						//!< 4002 红包 - 可领取列表信息
	{
	public:
		SCHongBaoGetList();
		MessageHeader header;

		struct GetInfo
		{
			GetInfo() :hongbao_id(0), send_avatar_type(0), send_headshot_id(0)
			{
				memset(send_name, 0, sizeof(send_name));
			}

			int hongbao_id;								// 红包ID
			int send_avatar_type;						// 发送者人物模型
			int send_headshot_id;						// 发送者头像ID
			GameName send_name;							// 发送者名称
		};

		int hongbao_sys_type;							// 红包所属的系统类型
		int count;										// 表长
		// 后面接可领取的红包表 由 count 份 GetInfo 组成
	};

	class SCHongBaoBaseInfo								//!< 4003 红包 - 详细信息
	{
	public:
		SCHongBaoBaseInfo();
		MessageHeader header;

		int hong_bao_id;								// 红包ID
		unsigned int invalid_timestamp;					// 红包失效时间

		int hongbao_type;								// 红包类型	[策划配置的类型]
		int hongbao_sys_type;							// 红包所属于的系统类型	[例如:世界,家族等等..]

		int send_uid;									// 发红包人的uid
		GameName send_name;								// 发红包人的名字
		HongBaoGreeting greeting;						// 红包祝福语

		int used_val;									// 已使用的金额

		int get_count;									// 领取红包的人员数量
		HongBaoItem::RewardInfo get_info[MAX_HONG_BAO_GET_MEMBER_COUNT];	// 领取红包的人员信息
	};

	class SCHongBaoSendListChange						//!< 4004 红包 - 可发送列表信息变更
	{
	public:
		SCHongBaoSendListChange();
		MessageHeader header;

		int hongbao_type;								// 红包类型
		int num;										// 红包数量
		int day_count;									// 每日已获得的可发送红包数量
		int total_count;								// 累计已获得的可发送红包数量
	};

	class SCHongBaoGetListChange						//!< 4005 红包 - 可领取列表信息变更
	{
	public:
		SCHongBaoGetListChange();
		MessageHeader header;

		enum CHANGE_TYPE
		{
			CHANGE_TYPE_ADD = 0,						//!< 0 有新的可领取的		[change_type]
			CHANGE_TYPE_DEL = 1,						//!< 1 有红包消失时间到了	[change_type]
		};

		struct GetInfo
		{
			GetInfo() :hongbao_id(0), send_avatar_type(0), send_headshot_id(0)
			{
				memset(send_name, 0, sizeof(send_name));
			}

			int hongbao_id;								// 红包ID
			int send_avatar_type;						// 发送者人物模型
			int send_headshot_id;						// 发送者头像ID
			GameName send_name;							// 发送者名称
		};
	
		int change_type;								// 变更类型
		int hongbao_sys_type;							// 红包系统类型
		GetInfo info;									// 红包信息
	};

	class SCHongBaoBaseInfoChange						//!< 4006 红包 - 详细信息变更
	{
	public:
		SCHongBaoBaseInfoChange();
		MessageHeader header;

		enum CHANGE_TYPE
		{
			CHANGE_TYPE_ADD = 0,						//!< 0 有新的领取人			[change_type]
			CHANGE_TYPE_DEL = 1,						//!< 1 红包时间到了			[change_type]
		};

		int change_type;								// 变更类型

		int hong_bao_id;								// 红包ID
		unsigned int invalid_timestamp;					// 红包失效时间

		int hongbao_type;								// 红包类型	[策划配置的类型]
		int hongbao_sys_type;							// 红包所属于的系统类型	[例如:世界,家族等等..]

		int used_val;									// 已使用的金额

		HongBaoItem::RewardInfo get_info;				// 领取人信息	
	};
}

#pragma pack(pop)

#endif	//__MSG_HONG_BAO_H__

