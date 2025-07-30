#ifndef __MSG_BABY_HPP__
#define __MSG_BABY_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/babydef.hpp"
#include "servercommon/struct/global/babydataparam.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCBabyInfo				//!< 2060
	{
	public:
		SCBabyInfo();
		MessageHeader		header;

		unsigned short single_ji_fen;		//!<���������Ļ���
		unsigned short duet_ji_fen;		//!<˫�������Ļ���
		unsigned char draw_times;		//!< �������� ÿ���ǩ����
		unsigned char worship_times;		 //!< �������� ÿ�칩�����		
		unsigned char duet_create_stage;	//!< ˫������ �׶�
		unsigned char add_jifen_times;		//!<С��Ϸ�Ƿ��ܼӻ��֣�����0��˵���������

		short create_type;				//!<���ڽ����������� 1�ǵ��ˣ�2��˫��
		short reserved_sh;			

		BabyDataParam baby[MAX_BABY_NUM];		// baby ��Ϣ,��С2
	};

	class CSBabyReq				//!< 2061
	{
	public:
		enum BABY_REQ_TYPE
		{
			BABY_REQ_TYPE_INFO,					//!<��Ϣ
			BABY_REQ_TYPE_DRAW_LOTS,				//!< ��ǩ 
			BABY_REQ_TYPE_CONTRIBUTION,			//!< ���� p1 �Ϲ����� [0~2 ����Ϲ���ħ���Ϲ�����ʯ�Ϲ�]
			BABY_REQ_TYPE_REMOVE,				//!< ���� p1 baby��index
			BABY_REQ_TYPE_UP_LEVEL,				//!< ���� p1 baby��index
			BABY_REQ_TYPE_PLAY_REQ,				//!< ѯ�ʶԷ���ʼС��Ϸ��0��2�׶Σ�
			BABY_REQ_TYPE_PLAY_RET,				//!< �ش��Ƿ�ʼС��Ϸ p1 �Ƿ���ܣ�0�Ƿ�
			BABY_REQ_TYPE_ADD_PLAY_JI_FEN,			//!< ���С��Ϸ���� p1 Ҫ��ӵĻ���
			BABY_REQ_TYPE_DUET_CREATE_REQ,		//!< �������˫������
			BABY_REQ_TYPE_DUET_CREATE_RET,			//!< �ش��Ƿ����˫������,p1 �Ƿ����								
			BABY_REQ_TYPE_SELECT_SINGLE_CREATE,		//!<  ѡ��������ģʽ
			BABY_REQ_TYPE_DUET_CREATE_STAGE_1,		//!< ѡ��˫������  1�׶β���
			BABY_REQ_TYPE_GIVE_UP_CREATE,			//!< ������ǰ����
		};

		CSBabyReq();
		MessageHeader		header;	

		int req_type;
		int param1;
	};

	class CSBabyRename						//!< ���� 2062
	{
	public:
		CSBabyRename();
		MessageHeader		header;

		int index;
		GameName name;
	};

	class SCBabyReqRoute				//!<  2063 ѯ������ת��
	{
	public:
		SCBabyReqRoute();
		MessageHeader		header;

		int req_type;				// ��������
	};

	class SCBabyStartPlay			//!<  2064 ��ʼС��Ϸ
	{
	public:
		SCBabyStartPlay();
		MessageHeader		header;

		unsigned int start_time;
	};

	class CSBabyActivte					//!< ������Ů 2065
	{
	public:
		enum REQ_TYPE
		{
			BABY_REQ_TYPE_DUET_ACTIVE,				//!< ����˫��������3�׶Σ�
			BABY_REQ_TYPE_SINGLE_ACTIVE,			//!< ��������������ֹ���
		};

		CSBabyActivte();
		MessageHeader		header;
		
		int req_type;
		GameName name;
	};
}

#pragma pack(pop)

#endif // __MSG_BABY_HPP__

