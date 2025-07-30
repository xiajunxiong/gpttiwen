#ifndef __MSG_BOSS_CHAPTER_H__
#define __MSG_BOSS_CHAPTER_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"
#include "servercommon/bosschapterdef.hpp"
#include "gamedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCBossChapterInfo			//!< 1860 ��Ϣ�·�
	{
	public:
		SCBossChapterInfo();
		MessageHeader header;

		struct Info
		{
			int monster_group_id;	//!< ������ID
			char reward_status;		//!< ������ȡ״̬  1������ȡ  2������ȡ
			char reserve_ch;		//!< ������
			short reserve_sh;		//!< ������
		};

		enum REASON
		{
			REASON_INIT = 0,		//!< 0 ��ʼ��
			REASON_ADD = 1,			//!< 1 ����
			REASON_INFO_CHANGE = 2,	//!< 2 ������Ϣ
			REASON_OTHER_INFO_CHANGE = 3, //!< 3 ����������Ϣ
		};

		short num;		//!< ����
		short reason;	//!< �·�Э��ԭ�� \see REASON
		long long chapter_reward_flag;				//!< �½���ȡ��ǣ���������ʽ��ʾ�½ڣ�
		Info info_list[BOSS_CHAPTER_MAX_BOSS_NUM];	//!< ��Ϣ�б� \see Info
	};


	class CSBossChapterReq		//!< 1861 boss�½�����
	{
	public:
		CSBossChapterReq();
		MessageHeader header;
		
		enum REQ_TYPE
		{
			REQ_TYPE_CLAIM_BOSS_REWARD = 0,		//!< ��ȡboss�Ľ���  param: ������ID
			REQ_TYPE_CLAIM_CHAPTER_REWARD = 1,	//!< ��ȡ�½ڽ���	 param: �½�ID
			REQ_TYPE_START_BATTLE = 2,			//!< ��ʼս��		 param: ������ID
		};

		int req_type;
		int param;
	};
}

#pragma pack(pop)

#endif

