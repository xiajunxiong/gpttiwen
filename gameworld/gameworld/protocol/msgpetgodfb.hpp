#ifndef __MSG_PET_GOD_FB_H__
#define __MSG_PET_GOD_FB_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/commondata.hpp"
#include "servercommon/servercommon.h"
#include "gamedef.h"

#include "servercommon/petgodfbdef.hpp"


#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	//��ӡ֮�� �û�����		3930
	class CSPetGodFBReq
	{
	public:
		CSPetGodFBReq();
		MessageHeader		header;
		enum REQ_TYPE
		{
			REQ_TYPE_BUY_DAY_TMES = 0,	//����ÿ�մ���
			REQ_TYPE_BUY_AUTO_FB =1,	//ɨ��		p_1:fb_seq
		};
		int req_type;		//��������
		int param_1;
	};

	//��ɫ��Ϣ			3931
	class SCPetGodFBRoleInfo
	{
	public:
		SCPetGodFBRoleInfo();
		MessageHeader		header;

		//��ʷ���ͨ�ز��� [ ��������(fb_type) - 1 ]
		int max_history_pass_level[PET_GOD_FB_MAX_TYPE_NUM];
	};

	//����������Ϣ		3932
	class SCPetGodFBSceneInfo
	{
	public:
		SCPetGodFBSceneInfo();
		MessageHeader		header;

		int m_fb_seq;		//����seq
		int m_is_finish;	//������ɱ��
		int m_boss_npc_id;	//boss
	};


};

#pragma pack(pop)

#endif