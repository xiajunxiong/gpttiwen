#ifndef __MSG_KUA_FU_YING_XIONG_LU_HPP__
#define __MSG_KUA_FU_YING_XIONG_LU_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/global/kuafuyingxiongludef.hpp"

#pragma pack(push)
#pragma pack(4)

namespace Protocol
{
	class CSKuaFuYingXiongLuReq		//!< 5310 跨服英雄录请求
	{
	public:
		CSKuaFuYingXiongLuReq();
		MessageHeader header;

		enum REQ_TYPE
		{
			REQ_TYPE_ALL_INFO = 0,				//!< 请求跨服英雄录所有信息
			REQ_TYPE_ACTIVITY_INFO = 1,			//!< 请求跨服英雄录活动信息
			REQ_TYPE_ROLE_ALL_INFO = 2,			//!< 请求跨服英雄录角色信息
			REQ_TYPE_FETCH_TASK_REWARD = 3,		//!< 请求领取任务奖励 param1:rank_type param2:seq
			REQ_TYPE_MY_RANK_VALUE = 4,			//!< 请求玩家自身榜单数值
			REQ_TYPE_CHECK_ROLE_INIT = 5,		//!< 请求检测(客户端收到5311会请求这个)
		};

		int req_type;
		int param1;
		int param2;
	};

	class SCKuaFuYingXiongLuActivityInfo				//!< 5311 跨服英雄录活动信息下发
	{
	public:
		SCKuaFuYingXiongLuActivityInfo();
		MessageHeader header;

		int cross_open_day;			//跨服开服天数
		unsigned int activity_begin_timestamp;
		unsigned int activty_end_timestamp;
		int count;
		unsigned int rank_end_timestamp_list[MAX_KUA_FU_YING_XIONG_LU_RANK_NUM];
	};


	struct KFYXLRoleRankTypeInfo
	{
		KFYXLRoleRankTypeInfo() { this->Reset(); }
		bool Serialize(char* buffer, int buffer_size, int* out_length) const
		{
			bool result = true;

			result = result && WriteInt(count, buffer, buffer_size, out_length);
			for (int task_index = 0; task_index < count && task_index < ARRAY_LENGTH(task_list); ++task_index)
			{
				result = result && WriteChar(task_list[task_index].is_fetch_flag, buffer, buffer_size, out_length);
				result = result && WriteInt(task_list[task_index].schedule, buffer, buffer_size, out_length);
			}

			return result;
		}
		void Reset()
		{
			count = 0;
			for (int i = 0; i < ARRAY_LENGTH(task_list); ++i)
			{
				task_list[i].Reset();
			}
		}

		int count;
		KuaFuYingXiongLuTaskInfo task_list[MAX_KUA_FU_YING_XIONG_LU_RANK_TASK_SEQ_NUM];// 任务列表
 	};

	class SCKuaFuYingXiongLuRoleAllInfo : public IMessageSerializer		//!< 5312 跨服英雄录角色所有信息下发
	{
	public:
		SCKuaFuYingXiongLuRoleAllInfo();
		virtual ~SCKuaFuYingXiongLuRoleAllInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(count, buffer, buffer_size, out_length);
			for (int i = 0; i < count && i < ARRAY_LENGTH(rank_info_list); i++)
			{
				const KFYXLRoleRankTypeInfo & rank_info = rank_info_list[i];
				result = result && rank_info.Serialize(buffer, buffer_size, out_length);
			}

			return result;
		}

		int count;
		KFYXLRoleRankTypeInfo rank_info_list[MAX_KUA_FU_YING_XIONG_LU_RANK_NUM];	//角色榜单信息列表
	};

	class SCKuaFuYingXiongLuRoleSingleRankInfo : public IMessageSerializer		//!< 5313 跨服英雄录角色单个榜单信息下发
	{
	public:
		SCKuaFuYingXiongLuRoleSingleRankInfo();
		virtual ~SCKuaFuYingXiongLuRoleSingleRankInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteInt(rank_type, buffer, buffer_size, out_length);
			result = result && rank_info.Serialize(buffer, buffer_size, out_length);

			return result;
		}

		int rank_type;
		KFYXLRoleRankTypeInfo rank_info;
	};

	class SCKuaFuYingXiongLuRoleSingleTaskInfo		//!< 5314 跨服英雄录角色单个任务信息下发
	{
	public:
		SCKuaFuYingXiongLuRoleSingleTaskInfo();
		MessageHeader header;

		int rank_type;
		int seq;
		KuaFuYingXiongLuTaskInfo task_info;
	};

	class SCKuaFuYingXiongLuRoleRankValue			//!< 5315 跨服英雄录角色榜单数值列表信息下发
	{
	public:
		SCKuaFuYingXiongLuRoleRankValue();
		MessageHeader header;

		int count;
		int rank_value_list[MAX_KUA_FU_YING_XIONG_LU_RANK_NUM];	//角色榜单数值列表
	};

}

#pragma pack(pop)

#endif	// __MSG_KUA_FU_YING_XIONG_LU_HPP__