#ifndef __MSGGUILD_HPP__
#define __MSGGUILD_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/serverdef.h"
#include "servercommon/guilddef.hpp"
#include "servercommon/commondata.hpp"
#include "servercommon/activitydef.hpp"
#include "servercommon/roleguildinborndef.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	class SCCreateRet	//!< �������� 9819
	{
	public:
		SCCreateRet();
		MessageHeader header;
		enum TYPE
		{
			CREATE = 1,
		};

		int param;
	};

	class CSCreateGuild		//!< �������� 9800
	{
	public:
		CSCreateGuild();
		MessageHeader header;

		GuildName name;
		GuildNotice notice;
		GuildBanner banner;
	};

	class CSQuitGuild		//!< �˳����� 9801
	{
	public:
		CSQuitGuild();
		MessageHeader header;

		GuildID guild_id;
	};

	class CSApplyJoinGuildReq		//!< ���������� 9805
	{
	public:
		CSApplyJoinGuildReq();
		MessageHeader header;

		GuildID guild_id;
		int one_click;			//!< �Ƿ�һ������
	};

	class CSApplyJoinGuildAck		//!< �������� 9806
	{
	public:
		CSApplyJoinGuildAck();
		MessageHeader header;

		GuildID guild_id;
		int uid;
		int result;				//!< 0Ϊ�ܾ�
	};

	class SCQuitGuild		//!< 9802 ֪ͨû�м�����
	{
	public:
		SCQuitGuild();
		MessageHeader header;
		enum REASON_TYPE
		{
			QUIT = 1, //!< �����˳�
			EXPEL_OUT, //!< ������
		};

		int reason;
	};

	class SCGuildBaseInfo : public IMessageSerializer	//!<9803 ���������Ϣ
	{
	public:
		SCGuildBaseInfo();
		virtual ~SCGuildBaseInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteStrN(patriarch_name, sizeof(GameName), buffer, buffer_size, out_length);
			result = result && WriteUInt(patriarch_last_login_time, buffer, buffer_size, out_length);
			result = result && WriteUInt(patriarch_tan_he_start_time, buffer, buffer_size, out_length);
			result = result && WriteShort(can_tan_he_patriarch, buffer, buffer_size, out_length);
			result = result && WriteShort(is_join_tan_he_patriarch, buffer, buffer_size, out_length);
			result = result && WriteStrN(guild_name, sizeof(GuildName), buffer, buffer_size, out_length);
			result = result && WriteStrN(guild_notice, sizeof(GuildNotice), buffer, buffer_size, out_length);

			result = result && WriteInt(guild_id, buffer, buffer_size, out_length);
			result = result && WriteInt(level, buffer, buffer_size, out_length);
			result = result && WriteLL(exp, buffer, buffer_size, out_length);

			result = result && WriteUInt(create_time, buffer, buffer_size, out_length);
			result = result && WriteUInt(join_time, buffer, buffer_size, out_length);
			result = result && WriteInt(is_need_approval, buffer, buffer_size, out_length);
			result = result && WriteInt(guild_buff_rank, buffer, buffer_size, out_length);

			result = result && WRITE_STRN(guild_banner, sizeof(guild_banner));

			return result;
		}

		GameName patriarch_name;		//!<�峤��
		unsigned int patriarch_last_login_time; //!<�峤��¼ʱ��
		unsigned int patriarch_tan_he_start_time; //!<����ʱ��,��Ϊ0�������˿�ʼ�˵�����
		short can_tan_he_patriarch;		//�Ƿ��ܵ���
		short is_join_tan_he_patriarch;		//�Ƿ�μ����峤����
		GuildName guild_name;			//!<������
		GuildNotice guild_notice;		//!<���幫��
		int guild_id;					//!<����id
		int level;						//!<�ȼ�
		long long exp;					//!<����

		unsigned int create_time;		//!<���崴��ʱ��
		unsigned int join_time;			//!<�������ʱ��
		int is_need_approval;					//�Ƿ���Ҫ����������빤�����
		int guild_buff_rank;			//����buff

		GuildBanner guild_banner;
	};

	struct MemberInfo
	{
		GameName name;			//!< ��Ա��
		int uid;				//!< 
		char online;			//!< 1������
		char post;				//!< ְ��
		short level;			//!< �ȼ�
		int top_level;			//!< �۷�
		int m_profession;		//!< ְҵ
		int gongxian;			//!< ����
		int total_gongxian;		//!< �ܹ���
		long long join_time;	//!< ��������ʱ��
		long long last_online_timestamp;		//!< �������ʱ���
		Appearance appearance;	//!< ��ɫ����
		int seven_active;		//!< ��������ܻ�Ծ��
		unsigned int seven_active_times;	//!< ��һ�μ����Ծ�ȱ��ʱ���
		short role_guild_task_finish_times;		//��ɴ���
		char role_guild_task_is_mail_notice;		//�ʼ�֪ͨ
		char role_guild_task_is_solicit;		//�Ƿ�����
		int role_guild_task_solicit_task_id;		//��������id
		short role_guild_task_solicit_num;		//��������
		short role_guild_task_solicit_fetch_num;	//���������Ƿ���ȡ
	};

	class SCGuildMemberInfo		//!<9804 �����Ա��Ϣ
	{
	public:
		SCGuildMemberInfo();
		MessageHeader header;
		
		int member_count;		//!< ��Ա��Ϣ�������С
		MemberInfo member_list[GUILD_MAX_MEMBER_COUNT];
	};

	class CSGuildExpelOutReq		//!<9807 �߳�����
	{
	public:
		CSGuildExpelOutReq();
		MessageHeader header;

		GuildID guild_id;
		int expel_uid;
	};

	class CSSetGuildNoticeReq		//!< 9808 �޸����˹���
	{
	public:
		CSSetGuildNoticeReq();
		MessageHeader header;

		GuildID guild_id;
		GuildNotice notice;
	};

	class CSGetGuildList			//!< ���������Ϣ 9809
	{
	public:
		CSGetGuildList();
		MessageHeader header;

		enum REQ_TYPE
		{
			GET_GUILD_LIST =1,						//!< ��ȡ�����б�
			GET_GUILD_NOTICE,						//!< ��ȡ����֪ͨ param ����id
			GET_GUILD_BASE_INFO,					//!< ��ȡ���������Ϣ param ����id
			GET_GUILD_MEMBER_LIST,					//!< ��ȡ�����Ա�б�
			GET_GUILD_APPLY_LIST,					//!< ��ȡ�����б�
			GET_GUILD_BUILD_INFO,					//!< ��ȡ������Ϣ param ����id
			GET_GUILD_TASK_INFO,					//!< ��ȡ����������Ϣ
			GET_GUILD_TASK_RANK_INFO,				//!< ��ȡ������������
			GET_GUILD_GATHER_DREAM_LIST,			//!< ��ȡ���弯Ը��Ϣ�б�
			GET_GUILD_PUBLISH_GATHER_DREAM,			//!< �������弯Ը��Ϣ  param item_id param1 gather_dream_type
			GET_GUILD_GIFT_GATHER_DREAM,			//!< ���輯Ը��Ƭ   param role_uid
			GET_GUILD_FETCH_GATHER_DREAM,			//!< ��ȡ��Ը��Ƭ
			GET_GUILD_GIFT_GATHER_DREAM_LIST,		//!< ��ȡ��ҵ������б�
			GET_GUILD_GET_GATHER_DREAM_LIST,		//!< �����ҵ������б�
			GET_GUILD_ROLE_GIFT_GATHER_DREAM_LIST,	//!< �����ҽ���������Ϣ
			GET_GUILD_ROLE_CAN_PULISH_LIST,			//!< �����ҿɷ����ļ�Ը��Ϣ�б�
			GET_GUILD_ROLE_SET_PUBLISH_SEND,		//!< ������ҷ�����Ը�Ƿ�����Ϣ������Ƶ��  param:0 ���� 1:������
			TAN_HE_ZU_ZHANG,						//!< �����峤
			REFUSE_GUILD_RECOMMEND,					//!< �ܾ������Ƽ�
			REQ_GUILD_RECOMMEND,					//!< ���󹤻��Ƽ�
		};

		int req_type;
		int param;
		int param1;
	};

	static const int MAX_GUILD_COUNT = 100;
	struct GuildNodeInfo
	{
		int guild_id;				//!< ����id
		int patriarch_id;			//!< �峤id
		int level;					//!< �ȼ�
		int member_count;			//!< ��Ա����
		GuildName guild_name;		//!< ������
		GameName patriarch_name;	//!< �峤��
		GuildBanner guild_banner;	//!< ���
		int seven_day_active;		//!< �����7���ܻ�Ծ
		unsigned int seven_day_active_last_change_times;		//!< �����7���ܻ�Ծ�������ʱ��
		int extra_member_limit;		//!< �᷿��������
	};
	class SCSendGuildList			//!< �·������б� 9810
	{
	public:
		SCSendGuildList();
		MessageHeader header;

		int count;
		GuildNodeInfo guild_list[MAX_GUILD_COUNT];
	};
	
	class SCSendGuildNotic			//!< �·�����֪ͨ 9811
	{
	public:
		SCSendGuildNotic();
		MessageHeader header;

		int guild_id;
		GuildNotice notice;
	};

	class CSGuildPetTrainingUp			//!< ǿ������ѵ�� 9812
	{
	public:
		CSGuildPetTrainingUp();
		MessageHeader header;

		int training_type;			//!< ��������
	};

	class CSGuildPetTraining			//!< ����ѵ�� 9813
	{
	public:
		CSGuildPetTraining();
		MessageHeader header;

		int pet_index;				//!< �����index
		int places_index;			//!< ѵ�����ӵ�index[0,2]
	};

	class CSGetGuildTrainingInfo			//!< ����ѵ����Ϣ 9814
	{
	public:
		CSGetGuildTrainingInfo();
		MessageHeader header;

	};

	class SCSendGuildTrainingInfo			//!< ����ѵ����Ϣ 9815
	{
	public:
		SCSendGuildTrainingInfo();
		MessageHeader header;

		int guild_training_level[PET_TRAINING_TYPE_MAX];				//!< ǿ��ѵ���ȼ� PET_TRAINING_TYPE_MAX = 4
		GuildTrainingPetParam training_pet_index[GUILD_PET_TRAINING_PET_MAX_COUNT];		//!< ��ѵ���ĳ����index  GUILD_PET_TRAINING_PET_MAX_COUNT = 3
	};

	
	static const int APPLY_COUNT_MAX = 100;
	class SCSendGuildApplyList			//!< �·������б� 9816
	{
	public:
		SCSendGuildApplyList();
		MessageHeader header;

		int count;
		GuildApplyUserInfo apply_list[APPLY_COUNT_MAX];		//!<�����б�
	};

	class CSGuildAbdicationOp			//!< ת���峤 9820
	{
	public:
		CSGuildAbdicationOp();
		MessageHeader header;

		GuildID guild_id;
		int uid;
	};

	class CSGuildImpeachmentOp			//!< ���� 9821
	{
	public:
		CSGuildImpeachmentOp();
		MessageHeader header;

		GuildID guild_id;
	};

	class CSGuildAppointmentOp			//!< ���� 9822
	{
	public:
		CSGuildAppointmentOp();
		MessageHeader header;

		GuildID guild_id;
		int uid;
		int post;
	};

	class CSGuildShopBuy			//!< �̵깺�� 9823
	{
	public:
		CSGuildShopBuy();
		MessageHeader header;

		GuildID guild_id;
		int shop_seq;
		int count;
	};


	class SCGuildShopInfo			//!< �̵���Ϣ 9824
	{
	public:
		SCGuildShopInfo();
		MessageHeader header;
		struct ShopNode
		{
			short seq;
			short buy_count;
		};

		int count;
		ShopNode shop_buy_list[GUILD_SHOP_COUNT_MAX];		//!< ���칺���˶��ٴ�
	};

	class SCGuildPetTrainingExp			//!< ����ѵ�����㾭����ʾ 9825
	{
	public:
		SCGuildPetTrainingExp();
		MessageHeader header;

		int pet_index;
		int time;
		int exp;
	};


	class CSGuildRenameReq		//!< 9826 �������
	{
	public:
		CSGuildRenameReq();
		MessageHeader header;

		GuildName new_name;
	};

	class CSGuildInvitationReq			//!< ��������9829
	{
	public:
		CSGuildInvitationReq();
		MessageHeader header;

		int uid;				//!< ����˭����
	};

	class SCGuildInvitationNotice			//!< �յ��������� 9830
	{
	public:
		SCGuildInvitationNotice();
		MessageHeader header;

		GameName inviter_name;
		GuildName guild_name;
	};

	class SCGuildBossInfo				//!< ����boss��Ϣ���� 9831
	{
	public:
		SCGuildBossInfo();
		MessageHeader header;

		int guild_boss_chapter_id;		//!< �������½�

		int boss_participate_reward_fetch_flag;	//!< ���뽱����ȡ��� 1:����ȡ  0:δ��ȡ
	};


	enum GUILD_BOSS_REQ_TYPE
	{
		GUILD_BOSS_REQ_TYPE_INFO,						//��Ϣ
		GUILD_BOSS_REQ_TYPE_RANK,						//�˺����а�
		GUILD_BOSS_REQ_TYPE_SET_CHAPTER,				//�����������½� ,p1 �½�
		GUILD_BOSS_REQ_TYPE_FETCH_CHAPTER_REWARD,		//��ȡ�½ڽ���
		GUILD_BOSS_REQ_TYPE_GUILD_RANK,					//��������֮��ļ���������������
	};

	class CSGuildBossReq				//!< ����boss��Ϣ���� 9832
	{
	public:
		CSGuildBossReq();
		MessageHeader header;

		int req_type;
		int p1;
	};

	class CSGuildLevelUpReq				//!< ������������ 9833
	{
	public:
		CSGuildLevelUpReq();
		MessageHeader header;

		GuildID guild_id;
	};

	class SCPlayerApplyList
	{
	public:
		SCPlayerApplyList();			//!< �ҵ������б� 9834
		MessageHeader header;

		int apply_count;
		int guild_id_list[PLAYER_APPLY_COUNT_MAX];
	};

	class CSInvitationReply
	{
	public:
		CSInvitationReply();			//!< 9835 ��������ظ�
		MessageHeader header;

		int result;		// �ظ������0 - �ܾ���1 - ͬ�⣻
	};

	class SCGuildNeedApproveInfo   //!< 9837 �����Ƿ���Ҫ��׼������Ϣ
	{
	public:
		SCGuildNeedApproveInfo();
		MessageHeader header;

		int is_need_approve;
	};

	class CSSetGuildNeedApproveReq
	{
	public:
		CSSetGuildNeedApproveReq();			//!< 9838 ���ù����Ƿ���Ҫ��׼������Ϣ
		MessageHeader header;

		int is_need_approve;
	};

	//////////////////////////////////// �������� ////////////////////////////////////
	class SCGuildWeekTaskAllInfor
	{
	public:
		SCGuildWeekTaskAllInfor();		//!< 1930 �������� - ������Ϣ
		MessageHeader header;

		int cur_finish_times;	//!< �����屾�ܵ�ǰ�������Ĵ���
		int cur_task_id;		//!< ��ǰ�ļ�������ID
		int cur_task_prog;		//!< ��ǰ���������
	};

	class SCGuildWeekTaskRankList
	{
	public:
		SCGuildWeekTaskRankList();		//!< 1931 �������� - ������Ϣ
		MessageHeader header;

		struct GWTRankItem
		{
			GWTRankItem()
			{
				memset(name, 0, sizeof(GameName));
				avatar_type = 0;
				headshot_id = 0;
				profession = 0;
				contribution = 0;
			}

			GameName name;
			short avatar_type;
			short headshot_id;
			int profession;
			int contribution;
		};

		int list_count;
		GWTRankItem rank_list[GUILD_WEEK_TASK_RANK_LIST_NUM_MAX];
	};

	//////////////////////////////////// ������� ////////////////////////////////////
	class SCActivityGuildAnswerCurQuestion
	{
	public:
		SCActivityGuildAnswerCurQuestion();		//!< 1223 - ��ǰ��Ŀ��Ϣ
		MessageHeader header;

		int cur_seq;		//!< ��ǰ�ǵڼ���, -1����׼������ʱ��
		int question_id;	//!< ����ID
		unsigned int next_time;
	};

	class CSActivityGuildAnswerAnswerQuest
	{
	public:
		CSActivityGuildAnswerAnswerQuest();		//!< 1224 - ���ѡ��Ĵ�
		MessageHeader header;

		int answer;			//!< �ҵĴ� - [1, 4] - 0 ��ʾ����뿪�˴�����
	};

	class SCActivityGuildAnswerResultInfor
	{
	public:
		SCActivityGuildAnswerResultInfor();		//!< 1226 - ���������
		MessageHeader header;

		int answer_result;		//!< �����Ƿ��ԣ�0 - ��� 1 - ��ԣ�
	};

	class SCActivityGuildAnswerAwardSettle
	{
	public:
		SCActivityGuildAnswerAwardSettle();		//!< 1227 - ���⽱���ܱ�
		MessageHeader header;

		int coin;			//!< ͭ��
		int exp;			//!< ����
		int contribution;	//!< ����

		int item_num;		//!< ���߱���
		PtcItemCon item_list[ACTIVITY_AWARD_NUM_MAX];
	};

	//
	class SCGuildBossRankInfo		//!< 		9836 �������� �˺����а�
	{
	public:
		SCGuildBossRankInfo();
		MessageHeader header;

		struct GuildBossRankItem
		{
			int role_id;
			int role_level;
			int top_level;
			int avatar_type;
			int headshot_id;
			int prof_base;			//!< ְҵ[0,9)
			GameName role_name;
			long long total_damage;
		};

		int count;
		GuildBossRankItem rank_item[GUILD_MAX_MEMBER_COUNT];
	};

	class SCGuildEventInfo : public IMessageSerializer		//!< 		9839 �����¼��·�
	{
	public:
		SCGuildEventInfo();
		virtual ~SCGuildEventInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(count, buffer, buffer_size, out_length);
			
			for (int i = 0; i < count && i < GUILD_EVENTHANDLER_COUNT; i++)
			{
				result = result && WriteInt(eventhandler_list[i].eventhandler_type, buffer, buffer_size, out_length);
				result = result && WriteUInt(eventhandler_list[i].eventhandler_time, buffer, buffer_size, out_length);
				result = result && WriteStrN(eventhandler_list[i].eventhandler_name, sizeof(GameName), buffer, buffer_size, out_length);

				int eventhandler_type = eventhandler_list[i].eventhandler_type / GUILD_EVENTHANDLER_PARAM;
				switch (eventhandler_type)
				{
					case GUILD_EVENTHANDLER_TYPE_XUNBAO:
					case GUILD_EVENTHANDLER_TYPE_ADD_PET:
					case GUILD_EVENTHANDLER_TYPE_WORK_SHOP_COMPOUND:
					case GUILD_EVENTHANDLER_TYPE_WORK_SHOP_CRAFT_JEWELRY:
					{
						result = result && WriteUShort(eventhandler_list[i].item_info.item_id, buffer, buffer_size, out_length);
						/*result = result && WriteShort(eventhandler_list[i].item_info.column, buffer, buffer_size, out_length);
						result = result && WriteShort(eventhandler_list[i].item_info.index, buffer, buffer_size, out_length);
						result = result && WriteShort(eventhandler_list[i].item_info.num, buffer, buffer_size, out_length);
						result = result && WriteChar(eventhandler_list[i].item_info.is_bind, buffer, buffer_size, out_length);
						result = result && WriteChar(eventhandler_list[i].item_info.has_param, buffer, buffer_size, out_length);
						result = result && WriteUInt(eventhandler_list[i].item_info.invalid_time, buffer, buffer_size, out_length);*/
						if (1 == eventhandler_list[i].item_info.has_param)
						{
							result = result && WriteShort(eventhandler_list[i].item_info.param_length, buffer, buffer_size, out_length);
							result = result && WriteStrN(eventhandler_list[i].item_info.param_data, eventhandler_list[i].item_info.param_length, buffer, buffer_size, out_length);
						}
						else
						{
							result = result && WriteShort(0, buffer, buffer_size, out_length);
						}
					}
					break;
					default:
					{
						//��ͻ��˶Ժõ�,�ͻ����Դ�Ϊ�ж�
						result = result && WriteUShort(0, buffer, buffer_size, out_length);
						result = result && WriteShort(0, buffer, buffer_size, out_length);
					}
					break;
				}
			}
			return result;
		}

		int count;
		GuildEventhandlerParam eventhandler_list[GUILD_EVENTHANDLER_COUNT];
	};

	class CSGuildSetBanner		//!< 9840 �����޸����
	{
	public:
		CSGuildSetBanner();
		MessageHeader header;

		GuildBanner banner;		//!< ����Ϊ8���ַ��� 
	};

	class SCGuildBossChapterGuildRankInfo : public IMessageSerializer		//!< 9841 ��������֮�����������������
	{
	public:
		SCGuildBossChapterGuildRankInfo();
		virtual ~SCGuildBossChapterGuildRankInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteInt(count, buffer, buffer_size, out_length);

			for (int i = 0; i < count && i < ARRAY_LENGTH(rank_list); i++)
			{
				result = result && WriteShort(rank_list[i].boss_index, buffer, buffer_size, out_length);
				result = result && WriteShort(rank_list[i].kill_num, buffer, buffer_size, out_length);
				result = result && WriteStrN(rank_list[i].guild_name, sizeof(GameName), buffer, buffer_size, out_length);
				result = result && WriteStrN(rank_list[i].guild_banner, sizeof(GuildBanner), buffer, buffer_size, out_length);
			}
			return result;
		}

		int count;
		SynGuildBossChapterRankInfo rank_list[50];
	};

	class SCGuildGatherDreamInfoList	// 9842 ���弯Ը��Ϣ�б��·�
	{
	public:
		SCGuildGatherDreamInfoList();
		MessageHeader header;

		int count;
		GuildGatherDreamData gather_dream_list[GUILD_MAX_MEMBER_COUNT];
	};

	class SCGuildGatherDreamSignleInfo			// 9843 ���弯Ը������Ϣ�·�
	{
	public:
		SCGuildGatherDreamSignleInfo();
		MessageHeader header;

		GuildGatherDreamData gather_dream_info;
	};

	class CSGuildActivePersonRank			// 3910  ��ȡ ���ɻ�Ծ�� ����������Ϣ 
	{
	public:
		CSGuildActivePersonRank();
		MessageHeader header;

	};

	class SCGuildActiveInfo						// 3911  ���ɻ�Ծ����Ϣ[���ɻ�Ծֵ]
	{
	public:
		SCGuildActiveInfo();
		MessageHeader header;

		int day_active_val;						// �����ջ�Ծ��ֵ
		int week_active_val;					// �����ܻ�Ծ��ֵ
	};

	class SCGuildActiveInfoMember				// 3912  ���ɻ�Ծ����Ϣ[������ȡ��������Ϣ]
	{
	public:
		SCGuildActiveInfoMember();
		MessageHeader header;

		int flag;								// ��ȡ�������[��λ]
		int seq;								// -1ʱ��ʹ�� , ��-1 ���ʾ�ղ���ȡ�ɹ��Ľ������
	};

	class SCGuildActivePersonRank				// 3913  ���ɻ�Ծ�� ����������Ϣ 
	{
	public:
		SCGuildActivePersonRank();
		MessageHeader header;

		struct PersonRankItem
		{
			GameName game_name;					// �������
			int prof;							// ְҵ
			char reserve_ch;					// ����
			char post;							// ����ְλ
			short level;						// ��ҵȼ�
			int week_active_val;				// �ܻ�Ծ��
		};

		int index;		// -1��ʾ���ڰ���
		int count;
		PersonRankItem  rank_info[GUILD_MAX_MEMBER_COUNT];		// ������
	};

	class CSGuildActiveGetGift					// 3914  ��ȡ ���ɻ�Ծ�� ���
	{
	public:
		CSGuildActiveGetGift();
		MessageHeader header;

		int seq;								// ��������
	};

	struct GuildInbornSkillInfo
	{
		GuildInbornSkillInfo() : guild_inborn_type(0), guild_type_seq(0), skill_level(0) {}
		GuildInbornSkillInfo(char _type, char _seq, short _level) : guild_inborn_type(_type), guild_type_seq(_seq), skill_level(_level) {}

		char guild_inborn_type;			//������������
		char guild_type_seq;			//�����������
		short skill_level;
	};

	class CSGuildInbornReq					//!< 3360 ����������������		
	{
	public:
		CSGuildInbornReq();
		MessageHeader header;

		enum GUILD_INBORN_REQ_TYPE
		{
			GUILD_INBORN_REQ_TYPE_ALL_INFO = 0,			//!< ��ȡ��������������Ϣ
			GUILD_INBORN_REQ_TYPE_LEARN_SKILL = 1,		//!< ��������ѧϰ���� param:inborn_type param1:type_seq
			GUILD_INBORN_REQ_TYPE_RESET_SKILL = 2,		//!< ���������������� param:inborn_type param1:type_seq
		};

		int req_type;
		int param1;
		int param2;
	};

	class SCGuildInbornAllInfo				//!< 3361 ��������������Ϣ�·�
	{
	public:
		SCGuildInbornAllInfo();
		MessageHeader header;

		short reserve_sh;
		short learn_count;
		GuildInbornSkillInfo learn_list[MAX_GUILD_INBORN_SKILL_CUR_NUM];	//��ѧϰ�б�
	};

	class SCGuildInbornSingleInfo		//!< 3362 ��������������Ϣ�·�
	{
	public:
		SCGuildInbornSingleInfo();
		MessageHeader header;

		GuildInbornSkillInfo learn_info;			//��ѧϰ�ļ�����Ϣ
	};

	class SCGuildInbornSingleTypeInfo	//!< 3363 ��������������Ϣ�·�
	{
	public:
		SCGuildInbornSingleTypeInfo();
		MessageHeader header;

		short reserve_sh;
		short guild_inborn_type;													//������������
		short learn_level_list[MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_CUR_NUM];	//ѧϰ�ȼ��б�  
	};

	class SCGuildQuitInfo						// 9844 �˳�������Ϣ
	{
	public:
		SCGuildQuitInfo();
		MessageHeader header;

		int quit_count;
		unsigned int next_join_time;
	};

	class SCGuildRecommend						// 9845 �����Ƽ�
	{
	public:
		SCGuildRecommend();
		MessageHeader header;

		int guild_id;
		GuildName guild_name;
		GameName role_name;
	};

	class CSGuildHallOfFameReq						// 9846 �����ò���
	{
	public:
		CSGuildHallOfFameReq();
		MessageHeader header;
		enum REQ_TYPE
		{
			REQ_TYPE_INFO = 0,//���������·� param enum HALLOFFAME_TYPE
			REQ_TYPE_QIFU = 1,//��
			REQ_TYPE_REWARD = 2,//��ȡ������
			REQ_TYPE_QIFU_INFO = 3,//����Ϣ�·�
		};
		int type;
		int param;
	};

	class SCGuildHallOfFameInfo						// 9847 ���������������·�
	{
	public:
		SCGuildHallOfFameInfo();
		MessageHeader header;
		struct HallOfFameInfo
		{
			int value;
			int role_id;
			GameName name;
			int prof;
		};
		int rank_type;//��������
		Appearance appearance_info;//��һ���
		int rank_count;//��������
		HallOfFameInfo rank_list[GUILD_MAX_MEMBER_COUNT];
	};

	class SCGuildQiFuInfo						// 9848 ����Ϣ�·�
	{
	public:
		SCGuildQiFuInfo();
		MessageHeader header;
		int is_open;
		int qifu_value;//������
		int today_qifu_times;//����������
		int is_get_qifu_attr;//�Ƿ���ȡ������
	};

	struct ConstructInfo		//����������Ϣ
	{
		ConstructInfo():build_type(0),build_level(0)
		{}

		short build_type;
		short build_level;
	};

	class SCGuildConstructInfo					//!< 9849 �����������赥����Ϣ�·�
	{
	public:
		SCGuildConstructInfo();
		MessageHeader header;

		ConstructInfo build_info;
	};

	class CSGuildConstructReq					//!< 9850 ����������������
	{
	public:
		CSGuildConstructReq();
		MessageHeader header;

		enum OP_TYPE
		{
			OP_TYPE_BUILDING = 1,		// ����
			OP_TYPE_DESTROY = 2,		// ���
			OP_TYPE_MEMBER_SPEED_UP = 3,// �����ԱΪ��������
		};

		short build_type;
		short op_type;
	};

	class SCGuildBuildingFinishInfo			//!< 9851 ���彨������,����ͻ����жϺ��
	{
	public:
		SCGuildBuildingFinishInfo();
		MessageHeader header;

		ConstructInfo build_finish_info;
	};

	class SCGuildUseBuildMaterialInfo		//!< 9852 ���彨��ͨ��Ư�ֵ���
	{
	public:
		SCGuildUseBuildMaterialInfo();
		MessageHeader header;

		void Reset()
		{
			op_type = 0;
			uid = 0;
			item_id = 0;
			num = 0;
			build_type = 0;
			memset(name, 0, sizeof(name));
		}

		enum OP_TYPE
		{
			OP_TYPE_USE_ITEM = 1,		//ʹ����Ʒ
			OP_TYPE_OPEN_SUP = 2,		//��������
			OP_TYPE_ADD_GONGXIAN = 3,	//���ӹ���
			OP_TYPE_MEMBER_SUP = 4,		//��Ա����
			OP_TYPE_SUP_FINISH = 5,		//���ٽ������
		};

		int op_type;
		int uid;				// type=2ʱ,��ʾְλ
		GameName name;
		int item_id;
		int num;
		int build_type;
	};

	class SCGuildConstructAllInfo					//!< 9853 ������������ȫ����Ϣ�·�
	{
	public:
		SCGuildConstructAllInfo();
		MessageHeader header;

		ConstructInfo build_list[GUILD_CONSTRUCT_BUILDING-1]; // 5������
		int building_build;									// ���ڽ���Ľ�����Ϣ,0-�޽���
		unsigned int building_finish_timestamp;				// �������ʱ���
		int building_count;									// �����ѽ������
	};

	class SCGuildSpeedUpInfo			//!<9854 ���彨�������Ϣ�·�
	{
	public:
		SCGuildSpeedUpInfo();
		MessageHeader header;

		short quicken_building_count;		// �����ѿ������ٴ���
		short can_speed_up;					// ��ǰ�Ƿ�ɼ���
		int is_cur_building_help;			// ��ҽ��ղ�����ٴ���
	};

	class SCGuildConstructTimeStampInfo			//!<9855 ���彨�����ʱ���	
	{
	public:
		SCGuildConstructTimeStampInfo();
		MessageHeader header;

		int building_build;					// ���ڽ���Ľ���,0-δ����
		unsigned int building_finish_timestamp;	// �������ʱ���,0-δ����
		int building_count;							// �����ѽ������
	};

	class SCGuildConstructOpenSUPInfo		//!<9856 ������ٿ�������Ϣ
	{
	public:
		SCGuildConstructOpenSUPInfo();
		MessageHeader header;

		int open_post;
		GameName name;
	};

	class SCGuildMemberInfoChange		//!<9857 �����Ա��Ϣ
	{
	public:
		SCGuildMemberInfoChange();
		MessageHeader header;

		MemberInfo member_info;
	};
}

#pragma pack(pop)
#endif