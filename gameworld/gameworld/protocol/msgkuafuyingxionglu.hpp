#ifndef __MSG_KUA_FU_YING_XIONG_LU_HPP__
#define __MSG_KUA_FU_YING_XIONG_LU_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/global/kuafuyingxiongludef.hpp"

#pragma pack(push)
#pragma pack(4)

namespace Protocol
{
	class CSKuaFuYingXiongLuReq		//!< 5310 ���Ӣ��¼����
	{
	public:
		CSKuaFuYingXiongLuReq();
		MessageHeader header;

		enum REQ_TYPE
		{
			REQ_TYPE_ALL_INFO = 0,				//!< ������Ӣ��¼������Ϣ
			REQ_TYPE_ACTIVITY_INFO = 1,			//!< ������Ӣ��¼���Ϣ
			REQ_TYPE_ROLE_ALL_INFO = 2,			//!< ������Ӣ��¼��ɫ��Ϣ
			REQ_TYPE_FETCH_TASK_REWARD = 3,		//!< ������ȡ������ param1:rank_type param2:seq
			REQ_TYPE_MY_RANK_VALUE = 4,			//!< ��������������ֵ
			REQ_TYPE_CHECK_ROLE_INIT = 5,		//!< ������(�ͻ����յ�5311���������)
		};

		int req_type;
		int param1;
		int param2;
	};

	class SCKuaFuYingXiongLuActivityInfo				//!< 5311 ���Ӣ��¼���Ϣ�·�
	{
	public:
		SCKuaFuYingXiongLuActivityInfo();
		MessageHeader header;

		int cross_open_day;			//�����������
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
		KuaFuYingXiongLuTaskInfo task_list[MAX_KUA_FU_YING_XIONG_LU_RANK_TASK_SEQ_NUM];// �����б�
 	};

	class SCKuaFuYingXiongLuRoleAllInfo : public IMessageSerializer		//!< 5312 ���Ӣ��¼��ɫ������Ϣ�·�
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
		KFYXLRoleRankTypeInfo rank_info_list[MAX_KUA_FU_YING_XIONG_LU_RANK_NUM];	//��ɫ����Ϣ�б�
	};

	class SCKuaFuYingXiongLuRoleSingleRankInfo : public IMessageSerializer		//!< 5313 ���Ӣ��¼��ɫ��������Ϣ�·�
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

	class SCKuaFuYingXiongLuRoleSingleTaskInfo		//!< 5314 ���Ӣ��¼��ɫ����������Ϣ�·�
	{
	public:
		SCKuaFuYingXiongLuRoleSingleTaskInfo();
		MessageHeader header;

		int rank_type;
		int seq;
		KuaFuYingXiongLuTaskInfo task_info;
	};

	class SCKuaFuYingXiongLuRoleRankValue			//!< 5315 ���Ӣ��¼��ɫ����ֵ�б���Ϣ�·�
	{
	public:
		SCKuaFuYingXiongLuRoleRankValue();
		MessageHeader header;

		int count;
		int rank_value_list[MAX_KUA_FU_YING_XIONG_LU_RANK_NUM];	//��ɫ����ֵ�б�
	};

}

#pragma pack(pop)

#endif	// __MSG_KUA_FU_YING_XIONG_LU_HPP__