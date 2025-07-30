#ifndef __MSG_GUILD_HONOR_BATTLE_HPP__
#define __MSG_GUILD_HONOR_BATTLE_HPP__

#include "servercommon/activitydef.hpp"
#include "servercommon/guilddef.hpp"
#include "servercommon/guildhonorbattledef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum GUILD_HONOR_REQ_TYPE
	{
		GUILD_HONOR_REQ_TYPE_ASSIGN,			//p1 ���䵽����0,1,2,3);  P2 role_id
		GUILD_HONOR_REQ_TYPE_JOIN,			// �������
		GUILD_HONOR_REQ_TYPE_AUTO_ASSIGN,	// �Զ�����
		GUILD_HONOR_REQ_TYPE_OB,				// ��ս p1 zond_id
	};

	class CSGuildHonorReq			//!< 2750
	{
	public:
		CSGuildHonorReq();
		MessageHeader header;

		int req_type;
		int p1;
		int p2;
	};

	struct MsgGuildHonorRoleItemInfo
	{
		int role_id;
		GameName role_name;
		int level;
		short prof;
		short zone_id;
	};

	class SCGuildHonorRoleInfo		//!< 		2751		
	{
	public:
		SCGuildHonorRoleInfo();
		MessageHeader header;

		int count;
		MsgGuildHonorRoleItemInfo role_info[GUILD_MAX_MEMBER_COUNT];	//256
	};

	class SCGuildHonorRoleChange		//!< 		2752		
	{
	public:
		SCGuildHonorRoleChange();
		MessageHeader header;

		short change_reson;		
		short zone_id;		
		int role_id;
	};

	class SCGuildHonorAddRoleInfo		//!< 		2754		
	{
	public:
		SCGuildHonorAddRoleInfo();
		MessageHeader header;

		MsgGuildHonorRoleItemInfo role_info;	
	};

	class SCGuildHonorZoneBattleResultInfo		//!< 		2755	  ÿ��ս���������ʱ��
	{
	public:
		SCGuildHonorZoneBattleResultInfo();		
		MessageHeader header;

		int zone_id;
		int win_guild_id;  //0��δ�ֳ�ʤ����-1��ƽ�֣���Ȼ����ʤ����guild_id
	};



	class SCGuildHonorAllZoneBattleResultInfo		//!< 2756	 ���볡����
	{
	public:
		SCGuildHonorAllZoneBattleResultInfo();
		MessageHeader header;

		struct GuildInfoItem
		{
			GuildInfoItem()
			{
				guild_id = 0;
				memset(guild_name, 0, sizeof(guild_name));
			}
			int guild_id;
			GuildName guild_name;
		};

		GuildInfoItem guild_info[2];
		int zone_battle_result[GUILD_HONOR_BATTLE_MAX_ZONE];  //0��δ�ֳ�ʤ����-1��ƽ�֣���Ȼ����ʤ����guild_id
	};


	class SCGuildHonorAllZoneRoleNumInfo		//!< 		2757	 ս���͹�ս���棬����һ��UI����ʾ��ʣ��δ��ս�������������ı�ͽ��볡����
	{
	public:
		SCGuildHonorAllZoneRoleNumInfo();
		MessageHeader header;

		int zone_role_num[GUILD_HONOR_BATTLE_MAX_ZONE]; 
	};

	enum GUILD_HONOR_REQ_TYPE_2
	{
		GUILD_HONOR_REQ_TYPE_2_SCORE_INFO,		// ����ĳ������ļ�����Ҷ�һ���ڵķ���  p1 guild_id
		GUILD_HONOR_REQ_TYPE_2_SCORE_RANK,		// ���Ҷ����ڵķ������а�
		GUILD_HONOR_REQ_TYPE_2_GUESSING,			// ���� p1 guild_id p2 win_rank
		GUILD_HONOR_REQ_TYPE_2_CANCEL_GUESSING,			// ȡ������ p1 guild_id p2 win_rank
		GUILD_HONOR_REQ_TYPE_2_MATCH_RECORD_INFO,			// �ϴα�����Ϣ
		GUILD_HONOR_REQ_TYPE_2_GUESSING_INFO,			// ���󾺲�����
		GUILD_HONOR_REQ_TYPE_2_GUESSING_CHAMPION,			// �ھ���������p1 guild_id 
		GUILD_HONOR_REQ_TYPE_2_CANCEL_GUESSING_CHAMPION,			// ȡ���ھ����� p1 guild_id 
	};

	class CSGuildHonorReq2			//!< 2758
	{
	public:
		CSGuildHonorReq2();
		MessageHeader header;

		int req_type;
		int p1;
		int p2;
	};

	class SCGuildHonorBattleProcessInfo		//!< 		2759  ����� �����û���Ӳ����·�
	{
	public:
		SCGuildHonorBattleProcessInfo();
		MessageHeader header;

		int process;			//GUILD_HONOR_BATTLE_PROCESS
		unsigned int settlement_list_time;		//����ʱ��
	};

	class SCGuildHonorBattleScoreInfo		//!< 		2760  ĳ������ļ�����Ҷ�һ���ڵķ���
	{
	public:
		SCGuildHonorBattleScoreInfo();
		MessageHeader header;

		int guild_id;

		unsigned short join_guild_fight_day_flag;	//�μ��˴��Ҷ����ڼ��ı�־
		unsigned short score[7];				//��������ÿ��ķ�����index 0��������	
	};

	class SCGuildHonorScoreRankInfo		//!< 		2761 ���Ҷ�ÿ���ڵķ������а�
	{
	public:
		SCGuildHonorScoreRankInfo();
		MessageHeader header;

		struct ItemInfo
		{
			int server_id;
			int guild_id;
			GuildName name;
			GuildBanner banner;

			unsigned short join_guild_fight_day_flag;	//�μ��˴��Ҷ����ڼ��ı�־
			unsigned short score[7];				//��������ÿ��ķ�����index 0��������
		};


		int count;
		ItemInfo item[50];
	};

	struct GuildHonorBattleMatchInfoItem 
	{
		struct FigtherInfoItem
		{
			int number;
			int server_id;
			int guild_id;
			GuildName guild_name;
			GuildBanner banner;
			GameName role_name;	//�峤����
			int guild_fight_rank;				//������Ҷ�����

			int match_round;		//����Ļغϣ���curr_round ��ͬ
			char is_finish;			//match_round �غ��Ƿ����ս��
			char is_lose;			//match_round �غ��ǲ�������
			unsigned short last_match_rank;				//0 û�У��ϴα�������

			int champion_guess_num;		//ѡ�ھ�֧�ֵ�����
			int guess_num[GUILD_HONOR_BATTLE_MAX_ROUND];		//ÿ��֧�ֵ�����
			int battle_id[GUILD_HONOR_BATTLE_MAX_ZONE];		//ÿ�������սid
			int battle_result[GUILD_HONOR_BATTLE_MAX_ZONE];  //ÿ�������ս�������������ʾĳ�������Ƿ��ܹ�ս
		};

		int start_first_round;			//���鿪ʼʱ����һ��غ�
		int win_rank;				//ʤ������
		int curr_round;				//��ǰ�غ�	
		int round_state;				//0�ǻغ�׼��״̬��1�ǿ�ʼ
		unsigned int next_round_state_time;		//�غϽ���ʱ��
		int is_finish;				//�Ƿ����
		int  fighter_num;
		FigtherInfoItem figther_info[GUILD_HONOR_BATTLE_FIGHTER_MAX_NUM];
	};

	class SCGuildHonorBattleMatchInfo		//!< 		2762  ������Ϣ
	{
	public:
		SCGuildHonorBattleMatchInfo();
		MessageHeader header;

		int match_info_count;			//������Ϣ����
		GuildHonorBattleMatchInfoItem match_info[16];		//������Ϣ
	};

	class SCGuildHonorBattleGuessInfo		//!< 		2763  ������Ϣ
	{
	public:
		SCGuildHonorBattleGuessInfo();
		MessageHeader header;

		struct Item
		{
			short win_rank;
			short round;
			int guild_id;
		};

		int champion_guild_id;
		int guess_item_count;
		Item guess_item[32];
	};

	class SCGuildHonorBattleMatchRecord		//!< 		2764  �ϸ���������
	{
	public:
		SCGuildHonorBattleMatchRecord();
		MessageHeader header;

		struct InfoItem
		{
			short start_first_round;;
			short win_rank;
			int number;
			int server_id;
			int guild_id;
			GuildName guild_name;
			GuildBanner banner;
			GameName role_name;	//�峤����
			int guild_fight_rank;				//������Ҷ�����
			int match_round;		//����Ļغϣ���curr_round ��ͬ
			int champion_guess_num;		//ѡ�ھ�֧�ֵ�����
			int guess_num[GUILD_HONOR_BATTLE_MAX_ROUND];		//ÿ��֧�ֵ�����
		};


		int  fighter_num;
		InfoItem figther_info[128];
	};

	class SCGuildHonorBattleTopInfo		//!< 		2765  buff
	{
	public:
		SCGuildHonorBattleTopInfo();
		MessageHeader header;
		const int static ITEM_NUM = 3;

		int guild_id[ITEM_NUM];
		GameName guild_name[ITEM_NUM];
	};

	class SCGuildHonorRoleBattleState			//2766  ս��״̬
	{
	public:
		SCGuildHonorRoleBattleState();
		MessageHeader header;


		int state;
	};
}

#pragma pack(pop)

#endif