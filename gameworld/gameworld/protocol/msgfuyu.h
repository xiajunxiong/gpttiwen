#ifndef __MSG_FU_YU_H__
#define __MSG_FU_YU_H__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/fuyudef.h"

#pragma pack(push, 4) 

namespace Protocol
{
	enum FU_YU_REQ_TYPE
	{
		FU_YU_REQ_TYPE_INFO = 0,		//!< ������Ϣ
		FU_YU_REQ_TYPE_PUT_ON = 1,		//!< ��������			p1-���id, p2-������װ��index(0-2) p3-����column, p4-����index
		FU_YU_REQ_TYPE_TAKE_OFF = 2,	//!< ���·���			p1-���id, p2-������װ��index(0-2)
		FU_YU_REQ_TYPE_DECOMPOSE = 3,	//!< �ֽ����			p1-����column, p2-����index
		FU_YU_REQ_TYPE_UPGRADE = 4,		//!< ǿ������(����)		p1-���id, p2-������װ��index(0-2), p3-���Ĳ���id, p4- �Ƿ�ʹ�ñ�������(1-ʹ��, 0-��ʹ��)
		FU_YU_REQ_TYPE_UPGRADE_2 = 5,	//!< ǿ������(����)		p1-����column, p2-����index, p3-���Ĳ���id, p4- �Ƿ�ʹ�ñ�������(1-ʹ��, 0-��ʹ��)
		FU_YU_REQ_TYPE_CONVERT_WEAR = 6,//!< ����ת��(����)		p1-���id, p2-������װ��index(0-2), p3-Ҫת������Чid1, p4-Ҫת������Чid2(��Ԫ/���������Ҫ, ��ͨ������0)
		FU_YU_REQ_TYPE_CONVERT_BAG = 7,	//!< ����ת��(����)		p1-����column, p2-����index, p3-Ҫת������Чid1, p4-Ҫת������Чid2(��Ԫ/���������Ҫ, ��ͨ������0)
		FU_YU_REQ_TYPE_SUBLIME_WEAR = 8,//!< ��������(����)		p1-���id, p2-�����ͷ���װ��index(0-2), p3&p4-������ͷ���column & index, p5&p6-������ͨ����column & index
		FU_YU_REQ_TYPE_SUBLIME_BAG = 9,	//!< ��������(����)		p1&p2-Ŀ����ͷ���column & index, p3&p4-������ͷ���column & index, p5&p6-������ͨ����column & index
	};

	class CSFuYuReq		//!<5188 ��������
	{
	public:
		CSFuYuReq();
		MessageHeader header;

		int req_type;
		int param1;
		int param2;
		int param3;
		int param4;
		int param5;
		int param6;
	};

	class SCFuYuInfo : public IMessageSerializer		//!<5186 �Ѵ�����������Ϣ�·�
	{
	public:
		SCFuYuInfo();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length);

		int reason;			// FuYuManager::FU_YU_SEND_REASON
		FuYuItemInfo info;
	};

	class SCAllFuYuInfo : public IMessageSerializer		//!<5187 �Ѵ�������ȫ����Ϣ�·�
	{
	public:
		SCAllFuYuInfo();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length);

		int count;
		FuYuItemInfo info_list[ItemNamespace::MAX_FU_YU_GRID_NUM];
	};
}

#pragma pack(pop) 
#endif
