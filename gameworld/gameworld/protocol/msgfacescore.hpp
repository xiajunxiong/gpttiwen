#ifndef __MSG_FACE_SCORE_HPP__
#define __MSG_FACE_SCORE_HPP__

#include "gamedef.h"
#include "servercommon/facescoredef.h"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{
	class CSFaceScoreRequest
	{
	public:
		CSFaceScoreRequest();				//!< 1790 - 颜值系统 - 操作请求
		MessageHeader header;

		enum FACE_SCORE_REQ_TYPE
		{
			FACE_REQ_SEND_INFO = 0,			//!< 下发信息
			FACE_REQ_EQUIPS_RES = 1,		//!< 装备：索引seq
			FACE_REQ_REMOVE_RES = 2,		//!< 拆卸：格子ID
		};

		int op_type;
		int param1;
		int param2;
	};

	class SCFaceScoreAllInfo
	{
	public:
		SCFaceScoreAllInfo();			//!< 1791 - 颜值系统 - 所有信息
		MessageHeader header;

		int face_level;
		int cur_exp;
		int equip_list[FACE_ADDITION_TYPE_MAX];
		BitMap<FACE_RESOURCE_NUM_MAX> active_flag;
	};

	class SCFaceListGridInfo
	{
	public:
		SCFaceListGridInfo();			//!< 1792 - 颜值系统 - 单个格子信息
		MessageHeader header;

		int grid_id;
		int seq;
	};

	class SCFaceResStateInfo
	{
	public:
		SCFaceResStateInfo();			//!< 1793 - 颜值系统 - 单个资源信息
		MessageHeader header;

		int seq;			// 索引seq
		int status;			//!< 当前状态：0 - 未激活；1 - 已激活
	};

	class SCFaceLevelAndExps
	{
	public:
		SCFaceLevelAndExps();			//!< 1794 - 颜值系统 - 等级与经验值信息
		MessageHeader header;

		int face_level;
		int cur_exp;
	};

	struct SCFaceTimeSingleInfo
	{
		SCFaceTimeSingleInfo();			//!< 1795 - 时间戳信息
		MessageHeader header;

		int seq;
		unsigned int time;
	};

	struct SCFaceTimeAllInfo
	{
		SCFaceTimeAllInfo();			//!< 1796 - 激活所有时间戳信息
		MessageHeader header;
		
		int count;
		unsigned int active_seq[FACE_RESOURCE_NUM_MAX][2];  // 激活的道具[count] = {seq , 时间戳}

	};
}

#pragma pack(pop)
#endif