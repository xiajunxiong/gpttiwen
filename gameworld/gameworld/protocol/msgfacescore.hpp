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
		CSFaceScoreRequest();				//!< 1790 - ��ֵϵͳ - ��������
		MessageHeader header;

		enum FACE_SCORE_REQ_TYPE
		{
			FACE_REQ_SEND_INFO = 0,			//!< �·���Ϣ
			FACE_REQ_EQUIPS_RES = 1,		//!< װ��������seq
			FACE_REQ_REMOVE_RES = 2,		//!< ��ж������ID
		};

		int op_type;
		int param1;
		int param2;
	};

	class SCFaceScoreAllInfo
	{
	public:
		SCFaceScoreAllInfo();			//!< 1791 - ��ֵϵͳ - ������Ϣ
		MessageHeader header;

		int face_level;
		int cur_exp;
		int equip_list[FACE_ADDITION_TYPE_MAX];
		BitMap<FACE_RESOURCE_NUM_MAX> active_flag;
	};

	class SCFaceListGridInfo
	{
	public:
		SCFaceListGridInfo();			//!< 1792 - ��ֵϵͳ - ����������Ϣ
		MessageHeader header;

		int grid_id;
		int seq;
	};

	class SCFaceResStateInfo
	{
	public:
		SCFaceResStateInfo();			//!< 1793 - ��ֵϵͳ - ������Դ��Ϣ
		MessageHeader header;

		int seq;			// ����seq
		int status;			//!< ��ǰ״̬��0 - δ���1 - �Ѽ���
	};

	class SCFaceLevelAndExps
	{
	public:
		SCFaceLevelAndExps();			//!< 1794 - ��ֵϵͳ - �ȼ��뾭��ֵ��Ϣ
		MessageHeader header;

		int face_level;
		int cur_exp;
	};

	struct SCFaceTimeSingleInfo
	{
		SCFaceTimeSingleInfo();			//!< 1795 - ʱ�����Ϣ
		MessageHeader header;

		int seq;
		unsigned int time;
	};

	struct SCFaceTimeAllInfo
	{
		SCFaceTimeAllInfo();			//!< 1796 - ��������ʱ�����Ϣ
		MessageHeader header;
		
		int count;
		unsigned int active_seq[FACE_RESOURCE_NUM_MAX][2];  // ����ĵ���[count] = {seq , ʱ���}

	};
}

#pragma pack(pop)
#endif