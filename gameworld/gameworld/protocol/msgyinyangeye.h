#ifndef __MSG_YIN_YANG_EYE_H__
#define __MSG_YIN_YANG_EYE_H__

#include "servercommon/yinyangeyedef.h"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSYinYangEyeOpReq							//!<4110 �����۲�������
	{
	public:
		CSYinYangEyeOpReq();
		MessageHeader header;

		enum YIN_YANG_EYE_OP_TYPE
		{
			YIN_YANG_EYE_OP_TYPE_OPEN = 0,			// ����
			YIN_YANG_EYE_OP_TYPE_BOX = 1,			// ���� param-seq
			YIN_YANG_EYE_OP_TYPE_NPC = 2,			// Npc  param-seq

			YIN_YANG_EYE_OP_TYPE_MAX
		};

		short op_type;
		short param;
	};


	class SCYinYangEyeInfo							//!<4111 ��������Ϣ
	{
	public:
		SCYinYangEyeInfo();
		MessageHeader header;

		char open_mark;								//�����ۿ���״̬
		char sh_reserve;
		short ch_reserve;

		short cur_box_finish_seq;					//��ǰ����ɵı�������
		short cur_npc_finish_seq;					//��ǰ����ɵ�npc����

		BitMap<YIN_YANG_EYE_BOX_MAXNUM> box_flag;	//��ɱ�־λ YIN_YANG_EYE_BOX_MAXNUM-512
		BitMap<YIN_YANG_EYE_NPC_MAXNUM> npc_flag;	//��ɱ�־λ YIN_YANG_EYE_NPC_MAXNUM-512
	};
}

#pragma pack(pop)
#endif




