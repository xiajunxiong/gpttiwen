#ifndef __MSG_WORLD_TEAM_ARENA_HPP__
#define __MSG_WORLD_TEAM_ARENA_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/global/worldteamarenaparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	class CSWorldTeamArenaReq		//!< 4600 Ӣ�ۻ�������
	{
	public:
		CSWorldTeamArenaReq();
		MessageHeader header;

		enum OP_TYPE
		{	
			OP_TYPE_SIGN_UP_ALL_INFO = 0,		//��������Ϣ�б�
			OP_TYPE_MEMBER_SIGN_UP = 1,			//��Աȷ�ϱ���
			OP_TYPE_CANCEL_SIGN_UP = 2,			//ȡ������
			OP_TYPE_SIGN_UP_DETAILED_INFO = 3,	//�����ѯĳ��ս����ϸ��Ϣ(�ӱ���) P1:team_unique_id P2:type(0:ս�ӱ��� 1:����)
			OP_TYPE_RANK_DETAILED_INFO = 4,		//�����ѯĳ��ս����ϸ��Ϣ(����������) P1:team_unique_id
			OP_TYPE_TEAM_SPECIAL_INFO = 5,		//�������������Ϣ
			OP_TYPE_OTHER_INFO = 6,				//����������Ϣ
		};

		int op_type;
		long long param1;
		int param2;
	};

	class  SCWorldTeamArenaAllSimpleInfo : public IMessageSerializer	//!< 4601 ���б���ս����Ϣ�·�
	{
	public:
		SCWorldTeamArenaAllSimpleInfo();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(my_team_index, buffer, buffer_size, out_length);
			result = result && WriteShort(count, buffer, buffer_size, out_length);
			for (int i = 0; i < count && i < ARRAY_LENGTH(team_list); i++)
			{
				const WorldTeamArenaTeamSignUpSimpleInfo & info = team_list[i];
				result = result && WriteLL(info.team_unique_id, buffer, buffer_size, out_length);
				result = result && WriteStrN(info.team_name, sizeof(info.team_name), buffer, buffer_size, out_length);
				result = result && WriteLL(info.total_capablity, buffer, buffer_size, out_length);

				result = result && WriteShort(info.level, buffer, buffer_size, out_length);
				result = result && WriteShort(info.top_level, buffer, buffer_size, out_length);
				result = result && WriteShort(info.avatar_type, buffer, buffer_size, out_length);
				result = result && WriteShort(info.headshot_id, buffer, buffer_size, out_length);
				result = result && WriteShort(info.fly_flag, buffer, buffer_size, out_length);

				for (int k = 0; k < ARRAY_LENGTH(info.member_list); ++k)
				{
					result = result && WriteInt(info.member_list[k].role_id, buffer, buffer_size, out_length);
					result = result && WriteInt(info.member_list[k].profession, buffer, buffer_size, out_length);
				}
			}
			result = result && WriteShort(know_count, buffer, buffer_size, out_length);
			for (int i = 0; i < know_count && i < ARRAY_LENGTH(know_team_index); i++)
			{
				result = result && WriteShort(know_team_index[i], buffer, buffer_size, out_length);
			}

			return result;
		}

		int my_team_index;
		short count;
		short know_count;
		WorldTeamArenaTeamSignUpSimpleInfo team_list[MAX_WORLD_TEAM_ARENA_SIGN_UP_TEAM_INFO_NUM];
		short know_team_index[MAX_WORLD_TEAM_ARENA_SIGN_UP_TEAM_INFO_NUM];				//��ʶս��  ����ֵΪ���������±�
	};

	//�޸�Ϊ����/����
	class CSWorldTeamArenaSignUpReq		//!< 4602 Ӣ�ۻ��䱨������
	{
	public:
		CSWorldTeamArenaSignUpReq();
		MessageHeader header;

		enum OP_TYPE
		{
			OP_TYPE_SIGN_UP = 0,		//����
			OP_TYPE_QUERY_TEAM = 1,		//��ѯս��		//ս����/��ɫ��(��֧��ģ������)
		};

		int op_type;
		GameName name;
	};

	class SCWorldTeamArenaPreTeamSignUpInfo		//!< 4603 Ӣ�ۻ���Ԥ��������Ϣ�·�
	{
	public:
		SCWorldTeamArenaPreTeamSignUpInfo();
		MessageHeader header;

		PreTeamSignUpInfo pre_team_sign_up_info;
	};

	class SCWorldTeamArenaNoticeInfo		//!< 4604 Ӣ�ۻ���֪ͨ��Ϣ�·�
	{
	public:
		SCWorldTeamArenaNoticeInfo();
		MessageHeader header;

		enum NOTICE_TYPE
		{
			NOTICE_TYPE_MEMBER_SIGN_UP = 0,				//��Աȷ�ϱ��� param1:member_index param2:determine_flag
			NOTICE_TYPE_SIGN_UP_FAILED_CLOSE = 1,		//����ʧ�ܹرձ�������

			NOTICE_TYPE_MAX
		};

		int notice_type;
		int param1;
		int param2;
	};

	class SCWorldTeamArenaTeamSignUpDetailedInfo	//!< 4605 Ӣ�ۻ���ս����ϸ��Ϣ�·�(��ӵ��ս��)
	{
	public:
		SCWorldTeamArenaTeamSignUpDetailedInfo();
		MessageHeader header;

		enum NOTICE_TYPE
		{
			NOTICE_TYPE_SIGN_UP = 0,		//��������
			NOTICE_TYPE_GAME = 1,			//��������
			NOTICE_TYPE_RANK_END = 2,		//�����������
		};

		int notice_type;
		WorldTeamArenaTeamSignUpDetailedInfo team_detailed_info;	
	};
	
	class SCActivityWorldTeamArenaInfo		//!< 4606 Ӣ�ۻ�����Ϣ�·�
	{
	public:
		SCActivityWorldTeamArenaInfo();
		MessageHeader header;

		int season_num;								//����
		unsigned int start_sign_up_timestamp;		//��ʼ����ʱ���
		unsigned int end_sign_up_timestamp;			//ֹͣ����ʱ���
		unsigned int activity_open_next_zero_timestamp;	//���ʼ����0��ʱ���
	};

	class CSActivityWorldTeamArenaSceneReq	//!< 4607 Ӣ�ۻ��䳡����������
	{
	public:
		CSActivityWorldTeamArenaSceneReq();
		MessageHeader header;

		enum WORLD_TEAM_ARENA_SCENE_OP_TYPE
		{
			WTA_OP_TYPE_REQ_ALL_RANK_INFO = 0,		//!< ��������������Ϣ(�����а�)
			WTA_OP_TYPE_REQ_CLOSE_RANK = 1,			//!< ����ر����а� 
			WTA_OP_TYPE_REQ_FIGHT_OTHER = 2,		//!< ����ս�� P1:����ս�����ID
		};

		int op_type;
		int param1;
	};

	class SCActivityWorldTeamArenaRankAllInfo			//!< 4608 Ӣ�ۻ������а���Ϣ
	{
	public:
		SCActivityWorldTeamArenaRankAllInfo();
		MessageHeader header;

		int resi_team_num;			//!< ʣ��ս������
		int list_num;
		WorldTeamArenaRankInfo rank_list[ACTIVITY_RANK_DEFAULT_NUM_MAX];
	};

	class SCActivityWorldTeamArenaMyInfo					//!< 4609 Ӣ�ۻ�������ս�����а���Ϣ
	{
	public:
		SCActivityWorldTeamArenaMyInfo();
		MessageHeader header;

		enum WORLD_TEAM_ARENA_MY_INFO_TYPE
		{
			WORLD_TEAM_ARENA_MY_INFO_TYPE_SEND = 0,			//�����ҵ�������Ϣ
			WORLD_TEAM_ARENA_MY_INFO_TYPE_NOTICE = 1,		//֪ͨ��ʾ���а������·��ҵ�������Ϣ		
		};

		int type;
		int rank_posi;							//!< ����λ�� - 0 ��ʾδ�ϰ�
		WorldTeamArenaRankInfo m_info;			//!< �ҵ�������Ϣ
	};

	class SCActivityWorldTeamArenaSpecialInfo		//!< 4610 Ӣ�ۻ���������Ϣ�·�
	{
	public:
		SCActivityWorldTeamArenaSpecialInfo();
		MessageHeader header;

		int resi_team_num;			//!< ʣ��ս������
	};

	class SCActivityWorldTeamArenaTeamSpecialInfo		//!< 4611 Ӣ�ۻ������������Ϣ�·�
	{
	public:
		SCActivityWorldTeamArenaTeamSpecialInfo();
		MessageHeader header;

		WorldTeamArenaTeamSpecialInfo member_list[ACTIVITY_WORLD_TEAM_ARENA_TEAM_MEMBER_NUM];
	};

	class SCActivityWorldTeamArenaFightInfo : public IMessageSerializer 	//!< 4612 Ӣ�ۻ��䷢��ս����Ϣ�·�
	{
	public:
		SCActivityWorldTeamArenaFightInfo();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteUInt(start_fight_timestamp, buffer, buffer_size, out_length);
			for (int team_index = 0; team_index < 2; team_index++)
			{
				ActivityWorldTeamArenaTeamFightInfo team_info;
				if (0 == team_index)
				{
					team_info = fight_info.attacker_team;
				}
				else
				{
					team_info = fight_info.defender_team;
				}
				result = result && WriteInt(team_info.rank_posi, buffer, buffer_size, out_length);
				result = result && WriteStrN(team_info.team_name, sizeof(team_info.team_name), buffer, buffer_size, out_length);
				result = result && WriteInt(team_info.member_count, buffer, buffer_size, out_length);
				for (int i = 0; i < team_info.member_count && i < ARRAY_LENGTH(team_info.member_list); ++i)
				{
					result = result && WriteStrN((const char *)&team_info.member_list[i], sizeof(team_info.member_list[i]), buffer, buffer_size, out_length);
				}
			}

			return result;
		}

		unsigned int start_fight_timestamp;				//��ʼս��ʱ���
		ActivityWorldTeamArenaFightInfo fight_info;
	};

	class CSActivityWorldTeamArenaRankReq			//!< 4613 Ӣ�ۻ������հ�����
	{
	public:
		CSActivityWorldTeamArenaRankReq();
		MessageHeader header;

	};

	class SCActivityWorldTeamArenaFinallyRankInfo		//!< 4614 Ӣ�ۻ������հ���Ϣ�·�
	{
	public:
		SCActivityWorldTeamArenaFinallyRankInfo();
		MessageHeader header;

		int rank_count;
		int my_rank_posi;						//!< ����λ�� - 0 ��ʾδ�ϰ�
		WorldTeamArenaRankInfo my_rank_info;
		WorldTeamArenaRankInfo rank_list[ACTIVITY_RANK_DEFAULT_NUM_MAX];
	};

	class SCActivityWorldTeamArenaOtherInfo			//!< 4615 Ӣ�ۻ���������Ϣ�·�
	{
	public:
		SCActivityWorldTeamArenaOtherInfo();
		MessageHeader header;

		int is_sign_up;				//!< 0:û���� 1:�ѱ���
	};

}

#pragma pack(pop)
#endif	// __MSG_WORLD_TEAM_ARENA_HPP__