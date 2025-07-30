#ifndef __MSG_SHITU_HPP__
#define __MSG_SHITU_HPP__

#include "servercommon/shitudef.h"
#include "servercommon/userprotocal/msgheader.h"

#ifdef _MSC_VER
#include <type_traits>
#endif

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSShiTuMakeRelationReq					//!<2110			��ʦ����ͽ ����
	{
	public:
		CSShiTuMakeRelationReq();
		MessageHeader header;

		int type;			
		int uid;
	};

	class CSShiTuMakeRelationAck				//!<2111   �ظ���ʦ����ͽ
	{
	public:
		CSShiTuMakeRelationAck();
		MessageHeader header;

		unsigned int type;

		int uid;
		int is_agree;
	};

	enum SHI_TU_BREAK_RELATION_REQ_TYPE
	{
		SHI_TU_BREAK_RELATION_REQ_TYPE_BREAK,			// �����ϵ
		SHI_TU_BREAK_RELATION_REQ_TYPE_BREAK_FORCE,		// ǿ�ƽ����ϵ
		SHI_TU_BREAK_RELATION_REQ_TYPE_BREAK_CANCEL,		// �侲����ȡ�������ϵ
	};

	class CSShiTuBreakRelationReq		//!<2112  ��ҵ���ʦ ����
	{
	public:
		CSShiTuBreakRelationReq();
		MessageHeader header;

		int req_type;
		int relation_type;
		int uid;
		int param1;	// ǿ�ƽ���Ƿ�ʹ��Ԫ��
	};

	class SCShiTuMakeRelationRoute	//!<2113   ��ʦ����ͽ ����ת��
	{
	public:
		SCShiTuMakeRelationRoute();
		MessageHeader header;

		int type;						
	
		GameName name;
		int uid;
		int level;
		short sex;
		char prof;
		char reserve_ch;
		short avatar_type;
		short headshot_id;
	};

	class SCShiTuRelationInfo		//!<2114  ʦͽ��Ϣ
	{
	public:
		SCShiTuRelationInfo();
		MessageHeader header;

		struct ShiTuItemInfo
		{
			ShiTuItemInfo()
			{				
#ifdef _MSC_VER
				using c = std::remove_reference<decltype(*this)>::type;
				static_assert(std::is_standard_layout<c>::value, "can't memset");
#endif

				memset(this, 0, sizeof(*this));
			}

			GuildName guild_name;
			GameName name;
			int uid;			
			int level;
			short sex;
			char hold_ch;
			char is_online;
			int prof;
			short avatar_type;
			short headshot_id;
			unsigned int shi_tu_time;	

			unsigned int login_time;	//�ϴε�¼ʱ��

			int apprentice_old_cap;			//��ʦʱ ͽ�ܵ�ս��
			int apprentice_cap;				//��ǰͽ��ս��
			int mile_reward_flag;			//��̽�����־[���]		//û���ˣ��ĳ�ֻ����ȡһ�Σ�����ÿ��ʦͽ������ȡ
			int relation_break_time_type;			// 1��ǿ�ƽ����0������������Է������߻�ܾ�
			unsigned int relation_break_time;		// �����ϵ����ʱ
		};

		ShiTuItemInfo master_info_list[SHITU_MAX_MASTER_COUNT];		//��С2
		int apprentice_count;
		ShiTuItemInfo apprentice_info_list[SHITU_MAX_APPRENTICE_COUNT]; //��С3
	};

	class SCShiTuBreakRelationRoute		//!< 2115   ��ҵ���ʦ ����ת��
	{
	public:
		SCShiTuBreakRelationRoute();
		MessageHeader header;

		unsigned int type;

		GameName name;
		int uid;
		int level;
		short sex;
		char prof;
		char reserve_ch;
		long long	avatar_timestamp;
	};

	class CSShiTuBreakRelationAck			//!< 2116  �ظ� ��ҵ���ʦ 
	{
	public:
		CSShiTuBreakRelationAck();
		MessageHeader header;

		unsigned int type;

		int uid;
		int is_agree;
	};

	enum SHI_TU_REQ_TYPE
	{
		SHI_TU_REQ_TYPE_RELATION_INFO,  //��ȡʦͽ��Ϣ����ȡ2114��
		SHI_TU_REQ_TYPE_INVITATION,	//����ʦͽ����
		SHI_TU_REQ_TYPE_FINISH_FB,		//��������	
	};

	class CSShiTuReq			//!< 2117		������Ϣ
	{
	public:
		CSShiTuReq();
		MessageHeader header;
		
		int req_type;
	};

	class SCShiTuFbSceneInfo				 //!< 2118		 ����������Ϣ
	{
	public:
		SCShiTuFbSceneInfo();
		MessageHeader header;

		unsigned int end_time;
		int count;
		ShiTuFbMonsterInfoItem monster_info[100];  //����
	};

	class SCShiTuFbCommonInfo				 //!< 2119  ʦͽ������Ϣ
	{
	public:
		SCShiTuFbCommonInfo();
		MessageHeader header;

		int is_get_shi_tu_reward;		//�Ƿ��Ѿ���ȡ��ʦͽ��������

		int day_mile_cap_fetch_num;			//����ս����� ��ȡ����
		int day_mile_pass_day_fetch_num;	//���հ�ʦ������� ��ȡ����

		int week_chuan_gong_flag;		//���ܴ����������
		int week_chuan_gong_exp_flag;	//���ܴ���������
		int week_chuan_gong_score_max;	//���ܴ�����ߵ÷�
		long long mile_reward_flag;				//��̽�����־[���]
	};

	class SCShiTuFbInvitationNotice			//!< 2120 ʦͽ�����������
	{
	public:
		SCShiTuFbInvitationNotice();
		MessageHeader		header;

		GameName name;
		int team_index;
		int uid;
		int is_cross;
	};

	class SCShiTuFbFinishNotice			//!< 2121 ����֪ͨ
	{
	public:
		SCShiTuFbFinishNotice();
		MessageHeader		header;

		int finish_group;
	};

	class SCShiTuFbBattleResult	//!< 2122 ս��֪ͨ
	{
	public:
		SCShiTuFbBattleResult();
		MessageHeader		header;

		int result;
		int group;
		int index;
	};

	class SCShiTuPreferencesInfo				 //!< 2124		
	{
	public:
		SCShiTuPreferencesInfo();
		MessageHeader header;

		char preferences[SHI_TU_PREFERENCES_NUM];		//4
	};

	class CSShiTuSeekingReq				//!< 2125			
	{
	public:
		enum SHI_TU_SEEKING_REQ_TYPE
		{
			SHI_TU_SEEKING_REQ_TYPE_INFO,			//������Ϣ�� seeking_type;	//1 Ѱ��ͽ�ܣ� 0 Ѱ��ʦ��
			SHI_TU_SEEKING_REQ_TYPE_INSERT,			//�������� �� seeking_type;	//1 Ѱ��ͽ�ܣ� 0 Ѱ��ʦ��
			SHI_TU_SEEKING_REQ_TYPE_REMOVE,		// ɾ������
			SHI_TU_SEEKING_REQ_TYPE_PREFERENCES,		// ʦͽƫ������ preferences
		};

		CSShiTuSeekingReq();
		MessageHeader header;

		int req_type;		
		int seeking_type;	
		char preferences[SHI_TU_PREFERENCES_NUM];  //4

		ShiTuSeekingNotice seeking_notice;
	};

	struct ShiTuSeekingInfo
	{
		GuildName		guild_name;
		GameName		gamename;
		int				user_id;
		char			reserve_ch;
		signed char		sex;
		char			prof;
		char			is_online;
		short			level;
		short			headshot_id;
		unsigned int	timestamp;
		int				avatar_type;
		ShiTuSeekingNotice	seeking_notice;
	};

	class SCShiTuSeekingInfo				 //!< 2126		
	{
	public:
		SCShiTuSeekingInfo();
		MessageHeader header;

		ShiTuSeekingNotice	self_seeking;		//�Լ�������
		int seeking_type;
		int count;
		ShiTuSeekingInfo seeking_info[SHI_TU_SEEKING_MAX_COUNT];
	};


	//ʦͽ����
	//��������		4470
	class CSShiTuChuanGongFBReq
	{
	public:
		CSShiTuChuanGongFBReq();
		MessageHeader header;

		enum ReqType
		{
			REQ_TYPE_GAME_READY = 0,//����
			REQ_TYPE_GAME_INFO,		//��Ϸ����	p_1:�����÷�  p_2: �ӳɺ�÷�
			REQ_TYPE_GAME_END,		//(ʧЧ)������Ϸ	p:�������
			REQ_TYPE_FETCH_MILE,	//��ȡ��̽���	p_1:Ŀ��uid p_2:��ȡ���
		};

		int req_type;
		int p_1;
		int p_2;
	};

	//��Ϸ��Ϣ֪ͨ	4471
	class SCShiTuChuanGongGameInfo
	{
	public:
		SCShiTuChuanGongGameInfo();
		MessageHeader header;

		enum InfoType
		{
			INFO_TYPE_GAME_BEGIN = 0,//��Ϸ��ʼ׼��	p:��ʼʱ��
			INFO_TYPE_GAME_SCORE,//�Է���Ϸ�÷�	p:�Է������÷�
		};

		int type;
		unsigned int param;
	};

	//������Ϣ	4472
	class SCShiTuChuanGongSceneInfo
	{
	public:
		SCShiTuChuanGongSceneInfo();
		MessageHeader header;

		unsigned int fb_begin_time;//��������ʱ��

		//��ɫ��Ϣ
		struct RoleShowInfo
		{
			RoleShowInfo()
			{
				uid = 0;
				memset(role_name, 0, sizeof(role_name));
				appearance.Reset();
			}

			int uid;
			GameName role_name;		//��ɫ��
			Appearance appearance;	//���
		};
		//ʦͽ��Ϣ
		int count;
		RoleShowInfo host_list[SHI_TU_SEEKING_TYPE_MAX];
	};

	//ʦͽ֪ͨ  4473
	class SCShiTuSyncInfo
	{
	public:
		SCShiTuSyncInfo();
		MessageHeader header;

		enum SyncType
		{
			ROLE_ONLINE_DEF,	//ʦ������֪ͨ ��ʼ��	p_1:ʦ��uid p_2:0ȫ��δ��ɣ�1ʦͽ����δ��ɣ�2����δ���
			ROLE_ONLINE_UPDATA,	//ʦ������֪ͨ ����					||||��λд�� 0:���� 1:ʦͽ����
		};

		int type;
		int param_1;
		int param_2;
	};

	enum SHI_TU_NOTICE_TYPE
	{
		SHI_TU_OP_NOTICE_TYPE_FORCE_BREAK,  //!<ǿ�ƽ����ȴ�� p1 uid
		SHI_TU_OP_NOTICE_TYPE_FORCE_BREAK_CANCEL,  //!<ȡ��ǿ�ƽ����ȴ�� p1 uid
	};

	class SCShiTuOpNotice			//!< 5190
	{
	public:
		SCShiTuOpNotice();
		MessageHeader		header;
		
		int notice_type;
		int p1;
	};
}

#pragma pack(pop)

#endif