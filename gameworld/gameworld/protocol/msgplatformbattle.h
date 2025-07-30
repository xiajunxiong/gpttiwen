#ifndef __MSG_PLATFORM_BATTLE_HPP__
#define __MSG_PLATFORM_BATTLE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/platformbattledef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum PLATFORM_BATTLE_REQ_TYPE
	{
		PLATFORM_BATTLE_REQ_TYPE_ROLE_INFO,		//��ɫ��Ϣ
		PLATFORM_BATTLE_REQ_TYPE_RANK_INFO,		//��̨��Ϣ p1 ��̨���
		PLATFORM_BATTLE_REQ_TYPE_CHALLENGE,			//ս��	 p1 ��̨���	,p2 rank ,  pet_indexҪ�ϳ��ĳ��partner_idҪ�ϳ��Ļ��
		PLATFORM_BATTLE_REQ_TYPE_AJUST,			//����	 p1 ��̨���,			pet_indexҪ�ϳ��ĳ��partner_idҪ�ϳ��Ļ��
		PLATFORM_BATTLE_REQ_TYPE_REMOVE,			//����ĳ����̨�ϵĻ��ͳ���  p1 ��̨���,
		PLATFORM_BATTLE_REQ_TYPE_RECORD,			//����ս��
		PLATFORM_BATTLE_REQ_TYPE_SET_NOTICE,		// ���ÿͻ��˴�ź����Ϣ p1 1��isfirst
		PLATFORM_BATTLE_REQ_TYPE_SET_NOTICE_2,		// ���ÿͻ��˴�ź����Ϣ p1��0��ʼ����lose_record_flag�ڼ�����0
		PLATFORM_BATTLE_REQ_TYPE_LAST_RANK_INFO,		// �ϴ�������Ϣ ���·���2791��
	};

	class CSPlatformBattleReq			//!< 2780
	{
	public:
		CSPlatformBattleReq();
		MessageHeader header;

		int req_type;
		short p1;
		short p2;

		MsgPlatformBattlePlayItem pet_index[PLATFORM_BATTLE_PET_PARTNER_MAX];		//5		p1 index p2 pos
		MsgPlatformBattlePlayItem partner_id[PLATFORM_BATTLE_PET_PARTNER_MAX];		//5		p1 partner_id p2 pos
	};

	class SCPlatformBattleRoleInfo		//!< 	2781 ����Լ�������Ϣ
	{
	public:
		SCPlatformBattleRoleInfo();		
		MessageHeader header;

		int count;
		MsgPlatformBattleRoleInfoItem role_info[PLATFORM_BATTLE_MAX_JOIN_NUM];			//10
	};

	struct MsgPlatformBattleRankItemPetInfo
	{
		MsgPlatformBattleRankItemPetInfo()
		{
			pos = 0;
			pet_id = 0;
			pet_capability = 0;
			pet_level = 0;
			pet_strengthen_level = 0;
			pet_skin_id = 0;
			memset(pet_name, 0, sizeof(pet_name));
		}

		int pos;
		int pet_id;				//0��Ч
		int pet_capability;		
		int pet_level;
		int pet_strengthen_level;
		int pet_skin_id;
		GameName pet_name;
	};

	struct MsgPlatformBattleRankItemPartnerInfo
	{
		MsgPlatformBattleRankItemPartnerInfo()
		{
			pos = 0;
			partner_id = 0;				//0��Ч			
			partner_capability = 0;
			partner_skin_id = 0;
			level = 0;
			str_level = 0;
		}

		int pos;
		int partner_id;
		int partner_capability;
		int partner_skin_id;	
		int level;
		int str_level;
	};


	class SCPlatformBattleRankInfo		//!< 		2782  һ����̨ ���������Ϣ
	{
	public:
		SCPlatformBattleRankInfo();		
		MessageHeader header;

		struct PlatformBattleRankItem
		{
			int role_id;
			int role_level;
			int top_level;
			short avatar_type;
			short headshot_id;
			GameName role_name;

			MsgPlatformBattleRankItemPetInfo battling_pet[PLATFORM_BATTLE_PET_PARTNER_MAX];		//��С5
			MsgPlatformBattleRankItemPartnerInfo battling_partner[PLATFORM_BATTLE_PET_PARTNER_MAX];	//��С5
		};

		short platform_number;
		short count;
		PlatformBattleRankItem rank_item[PLATFORM_BATTLE_RANK_ITEM_MAX];
	};


	class CSPlatformBattleRankFirstInfoReq		//!< 		2783  
	{
	public:
		CSPlatformBattleRankFirstInfoReq();
		MessageHeader header;

		unsigned char platform_number_list[PLATFORM_BATTLE_PLATFORM_NUM];
	};

	class SCPlatformBattleRankFirstInfo		//!< 		2784  ��̨��һ�������Ϣ
	{
	public:
		SCPlatformBattleRankFirstInfo();
		MessageHeader header;

		struct PlatformBattleRankItem
		{
			int platform_number;
			int role_id;
			int role_level;
			int top_level;
			short avatar_type;
			short headshot_id;
			GameName role_name;

			MsgPlatformBattleRankItemPetInfo battling_pet[PLATFORM_BATTLE_PET_PARTNER_MAX];		//��С5
			MsgPlatformBattleRankItemPartnerInfo battling_partner[PLATFORM_BATTLE_PET_PARTNER_MAX];	//��С5
		};

		int count;
		PlatformBattleRankItem first_item[PLATFORM_BATTLE_PLATFORM_NUM];
	};

	class CSPlatformBattlePlaformSwitch	//!< 2785 ����̨�������е�������̨����
	{
	public:
		CSPlatformBattlePlaformSwitch();
		MessageHeader header;

		int platform_seq;  //!< 1~5
	};

	class SCPlatformBattleRecord	//!< 2786 ��̨սս��
	{
	public:
		SCPlatformBattleRecord();
		MessageHeader header;

		int record_num;
		CommonSavePlatformBattleRoleRecord records[PLATFORM_BATTLE_RECORD_ITEM_MAX_NUM];
	};

	class SCPlatformBattleRecordNotice	//!< 2787   ֪ͨ��ʾ��� 
	{
	public:
		SCPlatformBattleRecordNotice();
		MessageHeader header;

		short is_first_times;			//��0��û���
		short lose_record_flag;		//ÿ����̨�������ͣ���Ҫ�����ʾ
	};

	enum PLATFORM_BATTLE_REQ2_TYPE
	{
		PLATFORM_BATTLE_REQ2_TYPE_ACHIEVEMENT,   //��ȡ�ɾͽ���,p1 type , p2 seq
	};

	class CSPlatformBattleReq2	//!< 2788   
	{
	public:
		CSPlatformBattleReq2();
		MessageHeader header;

		int req_type;
		int param1;
		int param2;
	};

	class SCPlatformBattleAchievementInfo	//!< 2789   �ɾ���Ϣ
	{
	public:
		SCPlatformBattleAchievementInfo();
		MessageHeader header;

		CommonSavePlatformBattleAchievementItem achievement_info[PLATFORM_BATTLE_ACHIEVEMENT_MAX];
	};

	class SCPlatformBattleSettleAccountNotice	//!< 2790   ��̨ս����
	{
	public:
		SCPlatformBattleSettleAccountNotice();
		MessageHeader header;
	};

	class SCPlatformBattleLastRankInfo 	//!< 2791   �ϴ�������Ϣ
	{
	public:
		SCPlatformBattleLastRankInfo();
		MessageHeader header;

		CommonSavePlatformBattleLastRankItem info[PLATFORM_BATTLE_MAX_RANK_REWARD];
	};
}

#pragma pack(pop)

#endif