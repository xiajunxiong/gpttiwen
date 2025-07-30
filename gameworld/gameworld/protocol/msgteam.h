#ifndef __MSGTEAM_H__
#define __MSGTEAM_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"
#include "servercommon/teamdef.hpp"
#include "gamedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{	
	class CSCreateTeam					//!< �������� 9150
	{
	public:
		CSCreateTeam();
		MessageHeader		header;

		short		limit_level_low;	//!< ��͵ȼ�
		short		limit_level_high;	//!< ��ߵȼ�
		short		team_type;			//!< ���Ŀ��
		char		is_cross;			//!< �Ƿ��� (1:��  0:��)
		char		is_match;			//!< �Ƿ���ƥ�� (1:��  0:��)
		char		allow_red_join;		//!< �Ƿ����������Ҽ��� (1:��  0:��)
		char		ch_reserve;
		short		sh_reserve;
	};

	class CSChangeTeamLimit				//!< �޸Ķ�����Ϣ 9163
	{
	public:
		CSChangeTeamLimit();
		MessageHeader		header;

		short		limit_level_low;	//!< ��͵ȼ�
		short		limit_level_high;	//!< ��ߵȼ�
		int			team_type;			//!< ���Ŀ��
		char		allow_red_join;		//!< �����������Ҽ���
		char		ch_reserve;
		short		sh_reserve;
	};

	class CSDismissTeam					//!< ��ɢ���� 9155
	{
	public:
		CSDismissTeam();
		MessageHeader		header;

	};

	class SCTeamListAck			//!< 9104  ��������б�ظ�
	{
	public:
		SCTeamListAck();
		MessageHeader	header;

		int				count;	//!< �����б�����
		TeamItem		team_list[MAX_TEAM_LIST_NUM]; //!< �����б�
	};

	class SCChivalrousTeamList	//!< 9112  ����ֵ�����б���
	{
	public:
		SCChivalrousTeamList();
		MessageHeader header;

		struct ChivalrousTeam
		{
			int add_chivalrous_value;	// ����ֵ
			TeamItem team_info;			// ������Ϣ
		};

		int				count;	//!< �����б�����
		ChivalrousTeam  team_list[MAX_TEAM_LIST_NUM]; //!< �����б�
	};

	class CSChivalrousTeamListReq	//!< 9201 ��������ֵ�����б�
	{
	public:
		CSChivalrousTeamListReq();
		MessageHeader header;
	};

	class CSUnfullTeamListByTypeReq			//!< 9165 ��ȡ����б� 
	{
	public:
		CSUnfullTeamListByTypeReq();
		MessageHeader		header;

		int				team_type;
	};

	struct TeamMemberInfo
	{
		TeamMemberInfo() : real_member_idx(0), show_member_idx(0),
		is_partner(0), level(0), uid(0), profession(0), is_follow(0), 
		is_online(0), pos_index(0), curr_hp(0), max_hp(0), curr_mp(0), 
		max_mp(0), partner_skin_id(0), pet_id(0), pet_level(0),
		pet_str_level(0), pet_curr_hp(0), pet_max_hp(0), pet_max_mp(0),
		sub_partner_id(0), sub_partner_cur_hp(0), sub_partner_max_hp(0),
		sub_partner_cur_mp(0), sub_partner_max_mp(0)
		{
			memset(member_name, 0, sizeof(member_name));
			appearance.Reset(); 
		}

		int real_member_idx;//!< ��ʵ��index
		int show_member_idx;//!< ��ʾ��index
		int is_partner;		//!< �Ƿ�Ϊ���
		int level;			//!< �ȼ�
		int uid;			//!< ��ɫuid ���� ���id
		int profession;		//!< ְҵ
		char is_follow;		//!< ����
		char is_online;		//!< �Ƿ�����
		short pos_index;	//!< ���վλ[0,5)
		int curr_hp;		//!< Ѫ��
		int max_hp;			//!< Ѫ��
		int curr_mp;		//!< ħ��
		int max_mp;			//!< ħ��
		int partner_skin_id;//!< ���Ƥ��

		int pet_id;
		int pet_level;
		int pet_str_level;		
		int pet_curr_hp;	//!< ����Ѫ��
		int pet_max_hp;		//!< ����Ѫ��
		int pet_curr_mp;	//!< ����ħ��
		int pet_max_mp;		//!< ����ħ��	
		int pet_skin_id;	//!< �������

		int sub_partner_id;
		int sub_partner_cur_hp;
		int sub_partner_max_hp;
		int sub_partner_cur_mp;
		int sub_partner_max_mp;

		GameName member_name;
		Appearance appearance; //!< ��������Ϣ
	};

	class SCTeamInfo					//!< ���Ͷ�����Ϣ����� 9100
	{
	public:
		SCTeamInfo();
		MessageHeader		header;

		int team_id;
		short			limit_level_low;
		short			limit_level_high;
		int				team_type;
		short			check_type;					//!< 0Ϊ�ܾ��������� 1Ϊ��Ҫ���� 2Ϊ�Զ�����
		char			is_cross;					//!< 0���ǿ��  1�����
		char			allow_red_join;				//!< 0���������������  1�������������

		int				leader_index;
		int				member_count;
		TeamMemberInfo	member_list[MAX_TEAM_MEMBER_NUM];
	};
	UNSTD_STATIC_CHECK(MAX_TEAM_MEMBER_NUM == 5);

	class SCTeamEnterRoute			//!<9108 ���븱������ת��
	{
	public:
		SCTeamEnterRoute();
		MessageHeader		header;

		int enter_type;
		int enter_type_param;
		unsigned int agree_enter_end_time;
		int param1;
		int param2;
		int param3;
	};

	class SCTeamEnterFbInfo			//!< 9109 ���븱����Աȷ����Ϣ
	{
	public:
		SCTeamEnterFbInfo();
		MessageHeader		header;

		short member_index;
		short is_determine;
	};

	class CSTeamEnterFbRet		//!< 9176 �ظ����븱������
	{
	public:
		CSTeamEnterFbRet();
		MessageHeader		header;

		int is_determine;
	};


	class SCOutOfTeam				//!< 9101  ֪ͨ���Ѿ�û�ж����� �����˻����Լ�����
	{
	public:
		SCOutOfTeam();
		MessageHeader		header;
	};

	class SCInviteNotice			//!< 9102 ���յ����������
	{
	public:
		SCInviteNotice();
		MessageHeader		header;

		int xia_yi_zhi;
		int team_index;
		int team_type;
		MsgRoleBaseInfo inviter_info;
	};
	class SCJoinReq				//!< 9103 ���յ����������
	{
	public:
		SCJoinReq();
		MessageHeader		header;

		int req_index;
		int uid;
		int level;
		int profession;
		long long avatar_timestamp;
		unsigned int req_tmestamp;
		GameName name;
		short avatar_type;
		short headshot_id;
	};

	class CSInviteJoinReq		//!< 9151 ����������
	{
	public:
		CSInviteJoinReq();
		MessageHeader		header;

		int uid;			//!< 
	};

	class CSExitTeamReq		//!< 9158 �˶�
	{
	public:
		CSExitTeamReq();
		MessageHeader		header;
	};

	class CSKickOutOfTeamReq		//!< 9156 ����Ա�߳�����
	{
	public:
		CSKickOutOfTeamReq();
		MessageHeader		header;

		int kick_index;
	};

	class CSJoinTeamReq		//!< 9154 �������
	{
	public:
		CSJoinTeamReq();
		MessageHeader		header;

		int team_index;
		int is_apply_cross_team; //!< �Ƿ����������� [0:�� 1:��]
	};

	class CSJoinTeamReqRet		//!< 9153 �����������
	{
	public:
		CSJoinTeamReqRet();
		MessageHeader		header;

		int uid;				//!< ������uid
		int is_agree;			//!< 0 Ϊ�ܾ�������� ����Ϊͬ��
	};

	class CSInviteJoinRet		//!< 9152 ���������
	{
	public:
		CSInviteJoinRet();
		MessageHeader		header;

		int uid;				//!< ������uid
		int is_agree;			//!< 0 Ϊ�ܾ�������� ����Ϊͬ��
	};

	class SCTeamMatchInfo		//!< 9169
	{
	public:
		SCTeamMatchInfo();
		MessageHeader		header;

		int is_match;		//!< 0Ϊδƥ��
	};

	class CSTeamMatchOp			//!< 9170 ƥ������
	{
	public:
		CSTeamMatchOp();
		MessageHeader		header;

		int match_type;			//!< Ҫƥ����������
		short op_type;			//!< ��������   0��ȡ��ƥ��  1����ʼƥ��
		char is_cross;			//!< �Ƿ���ƥ��  [0:�� 1:��] 
		char is_auto_create;	//!< һ��ʱ��ƥ�䲻�����Ƿ��Զ��������� [0:�� 1:��] 
	};

	class CSChangeTeamLeader			//!< 9157 ���ӳ�
	{
	public:
		CSChangeTeamLeader();
		MessageHeader		header;

		int member_index;
	};

	class CSChangeMustCheck			//!< 9160 �ı��������Ƿ���Ҫ�ӳ�ͨ��
	{
	public:
		CSChangeMustCheck();
		MessageHeader		header;

		int check_type;				//!< 0Ϊ�ܾ��������� 1Ϊ��Ҫ���� 2Ϊ�����Զ�����
	};

	class CSTemporarilyPartReq			//!< 9171 ����
	{
	public:
		CSTemporarilyPartReq();
		MessageHeader		header;

	};

	class CSTeamInvitationReq			//!< 9172 �������
	{
	public:
		CSTeamInvitationReq();
		MessageHeader		header;

		int uid;
		int invite_type;
	};

	enum TEAM_INVITATION_NOTICE_TYPE
	{
		TEAM_INVITATION_NOTICE_TYPE_DEFAULT,
		TEAM_INVITATION_NOTICE_TYPE_UPDATE,
		TEAM_INVITATION_NOTICE_TYPE_NOT_EXIST,
	};

	class SCTeamInvitationNotice			//!< 9173 �������֪ͨ
	{
	public:
		SCTeamInvitationNotice();
		MessageHeader		header;
	
		int notice_type;
		short is_from_cross;
		short invite_type;
		int xia_yi_zhi;
		int team_index;
		int team_type;		
		int team_member_num;
		MsgRoleBaseInfo inviter_info; 
	};

	class CSTeamInvitationRet			//!< 9174 �������ظ�
	{
	public:
		CSTeamInvitationRet();
		MessageHeader		header;

		int uid;
		int is_accept;					//!< 0�ܾ� 1 ����
		int is_from_cross;				//!< ��9173Э���·���ԭ������������
		int team_index;					//!< ��9173Э���·���ԭ������������
	};

	class CSTeamRecruit		//!< 9175 ��ļ
	{
	public:
		CSTeamRecruit();
		MessageHeader header;
	};

	class CSTeamSwitchToCross	//!< 9177 �����л������
	{
	public:
		CSTeamSwitchToCross();
		MessageHeader header;
	};

	class SCTeamChannelInfo		//!< 9178 ���Ƶ����Ϣ
	{
	public:
		SCTeamChannelInfo();
		MessageHeader header;

		enum REASON_TYPE					//!< �·�ԭ��
		{
			REASON_TYPE_NORMAL = 0,			//!< 0 ��������������б��·���
			REASON_TYPE_INFO_CHANGE = 1,	//!< 1 ��Ϣ�ı�
			REASON_TYPE_ADD_TEAM = 2,		//!< 2 ��������
			REASON_TYPE_REMOVE_TEAM = 3,	//!< 3 ���б���ȥ������
		};

		short reason;
		short count;

		TeamItem team_list[MAX_TEAM_CHANNEL_ITEM_NUM];
	};

	class CSTeamSwitchPosition	//!< ����վλ 9179
	{
	public:
		CSTeamSwitchPosition();
		MessageHeader header;

		short pos1;			//!< ������λ��1
		short pos2;			//!< ������λ��2
	};

	class SCTeamMemberAppearanceChange		//!< 9180 ��Ա��۱仯֪ͨ
	{
	public:
		SCTeamMemberAppearanceChange();
		MessageHeader header;

		int member_index;					
		Appearance new_appearance;
	};

	class SCTeamPosInfo			//!< 9110 ����վλ��Ϣ
	{
	public:
		SCTeamPosInfo();
		MessageHeader header;

		struct MemberPos
		{
			int id;				//!< ��һ����id  Ϊ0������Ч����
			short is_partner;	//!< �Ƿ���
			short reserve_sh;	//!< ������
		};

		MemberPos pos_list[MAX_TEAM_MEMBER_NUM];
	};

	class SCTeamLeaderChangeNoticeArea		// 9111 �ӳ��ı�֪ͨ�������
	{
	public:
		SCTeamLeaderChangeNoticeArea();
		MessageHeader header;

		ObjID change_objid;		// �ı�Ķ���ID		
		short is_leader;		// �Ƿ�ӳ�[0:�� 1:��]
	};

	class SCTeamRemoveApply		// 9181 ���������б�ɾ��һ����ҵ�����
	{
	public:
		SCTeamRemoveApply();
		MessageHeader header;

		int remove_uid;
	};

	class SCApplyList			//!< 9182 �·������б�ת�ƶӳ�ʱ�·���
	{
	public:
		SCApplyList();
		MessageHeader		header;

		static const int MAX_APPLY_LIST_SIZE = 5;

		struct ApplyInfo
		{
			int req_index;
			int uid;
			int level;
			int profession;
			long long avatar_timestamp;
			unsigned int req_tmestamp;
			GameName name;
			short avatar_type;
			short headshot_id;
		};
		
		int count;
		ApplyInfo apply_list[MAX_APPLY_LIST_SIZE];
	};

	class SCTeamVoteStart					//!< ͶƱ��ʼ֪ͨ 9183
	{
	public:
		SCTeamVoteStart();
		MessageHeader header;

		unsigned int vote_end_timestamp;	//!< ͶƱ����ʱ���
		GameName campaigner_name;			//!< ����������
	};

	class CSTeamLaunchLeaderVote //!< ����ͶƱ 9184
	{
	public:
		CSTeamLaunchLeaderVote();
		MessageHeader header;
	};

	class CSTeamVote	//!< ͶƱ 9185
	{
	public:
		CSTeamVote();
		MessageHeader header;

		int is_yes;		//!< [0:��ͬ��  1:ͬ��]
	};

	class SCUpdateMemberInfo		//!< 9186 ͬ����Ա��Ϣ
	{
	public:
		SCUpdateMemberInfo();
		MessageHeader header;

		int uid;					//!< ��ɫuid ���� ���id
		int is_partner;				//!< 0:����  1:���
		int hp;						//!< Ѫ��
		int max_hp;
		int mp;						//!< ħ��
		int max_mp;					//!< ħ��
		int skin_id;				//!< Ƥ��ID  ���ʱ��Ч
		int level;					//!< �ȼ�
	};

	enum TEAM_LEADER_NPC_REQ_TYPE
	{
		TEAM_LEADER_NPC_REQ_TYPE_LOAD = 0,//!< ��ʼ��
		TEAM_LEADER_NPC_REQ_TYPE_ADD = 1,//!< ���
		TEAM_LEADER_NPC_REQ_TYPE_DELETE = 2,//!< ɾ��
	};
	const static int TEAM_LEADER_NPC_SEQ_COUNT = 150;// �ӳ�������npc�·��б�
	class CSTeamLeaderNpcReq		//!< 9187 �ӳ�����NPC����
	{
	public:
		CSTeamLeaderNpcReq();
		MessageHeader header;

		int type;
		int npc_count;
		int npc_seq_list[TEAM_LEADER_NPC_SEQ_COUNT];
	};

	class SCTeamLeaderNpcInfo		//!< 9188 �ӳ�����NPC�·�
	{
	public:
		SCTeamLeaderNpcInfo();
		MessageHeader header;

		int type;
		int npc_count;
		int npc_seq_list[TEAM_LEADER_NPC_SEQ_COUNT];
	};

	class SCUpdateMemberPetHPMP		//!< 9189 ��Ա��ս������Ϣ
	{
	public:
		SCUpdateMemberPetHPMP();
		MessageHeader header;

		int uid;					//!< ��ɫuid 
		int pet_id;
		int pet_level;
		int pet_str_level;
		int curr_hp;					//!< Ѫ��
		int max_hp;					//!< Ѫ��
		int curr_mp;					//!< ħ��
		int max_mp;					//!< ħ��
		int skin_id;				//!< Ƥ��
	};

	class SCTeamMemberAddHpMpNotice		//!< 9190 ��Ա������Ѫ����֪ͨ
	{
	public:
		SCTeamMemberAddHpMpNotice();
		MessageHeader header;

		short from_team_member_index;
		short is_to_use_pet;		
		int partner_id;
		short item_id;
		short use_gold_recovery;  //!< 1�ǻָ�Ѫ��2�ǻָ���
	};

	class SCTeamSimpleInfoChangeNotice	//!< 9191 ���������Ϣ����֪ͨ
	{
	public:
		SCTeamSimpleInfoChangeNotice();
		MessageHeader header;

		short			limit_level_low;
		short			limit_level_high;
		int				team_type;
		short			check_type;					//!< 0Ϊ�ܾ��������� 1Ϊ��Ҫ���� 2Ϊ�Զ�����
		char			is_cross;					//!< 0���ǿ��  1�����
		char			allow_red_join;				//!< 0���������������  1�������������
	};

	struct ChangeTeamMemberBaseInfo
	{
		int level;				//!< �ȼ�
		int uid;				//!< ��ɫuid
		int profession;			//!< ְҵ

		GameName member_name;	//!< ����
		Appearance appearance;	//!< ��������Ϣ
	};

	class SCTeamMemberBaseInfoChangeNotice	//!< 9192 ��Ա��Ϣ�仯֪ͨ
	{
	public:
		SCTeamMemberBaseInfoChangeNotice();
		MessageHeader header;

		ChangeTeamMemberBaseInfo member_info;
	};

	class CSTeamLeaderSummonMember	//!< 9193 �ٻ�����
	{
	public:
		CSTeamLeaderSummonMember();
		MessageHeader header;

		int summon_role_id;			//!< Ҫ�ٻص����ID
	};

	class SCTeamLeaderSummonMemberNotice	//!< 9194 ֪ͨ��Ա���ӳ���Ҫ�ٻ���
	{
	public:
		SCTeamLeaderSummonMemberNotice();
		MessageHeader header;
	};

	class CSTeamMemberReplyLeaderSummon	//!< 9195 ��Ա�ظ��ٻ�
	{
	public:
		CSTeamMemberReplyLeaderSummon();
		MessageHeader header;

		int is_return;					//!< �Ƿ�ع� 0�����ع�   1���ع�
	};

	enum TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE
	{
		TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_WORLD_BOSS1,			//����boss1 ��������
		TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_WORLD_BOSS2,			//����boss2 ��������
		TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_LEVEL,					//��Ա�ȼ�����  p1 Ŀ��ȼ�
		TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_ITEM,					//��Ա��Ʒp1 �ﲻ��Ŀ����p2��p1 item_id, p2 ����
		TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_JOIN_TIMES,				//xxx�Ѿ������μӴ���
		TEAM_MEMBER_INCOMPATIBLE_OVER_MEMBER_NUM,					// ��������̫��  p1 �����������
		TEAM_MEMBER_INCOMPATIBLE_NOT_ENOUGH_MEMBER_NUM,			// ������������ p1 ��Ҫ����
		TEAM_MEMBER_INCOMPATIBLE_NOT_TU_DI,							// ����(team_member_index)�������ͽ��
		TEAM_MEMBER_INCOMPATIBLE_BRAVE_GROUND_CANT_NEXT,			// ��ʱ�޷����룬��������ȼ��ﵽ p1 ���󿪷�
		TEAM_MEMBER_INCOMPATIBLE_HAS_JIE_YI_ZU,				//xxx�Ѿ������μӴ���
		TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_WORLD_BOSS3,			//����boss3(������) ��������
		TEAM_MEMBER_INCOMPATIBLE_NO_TASK_RECORD,					//��ԱX param1���� δ���
	};

	class TeamMemberIncompatibleNotice	//!< ���Ѳ���������֪ͨ 9196
	{
	public:
		TeamMemberIncompatibleNotice();
		MessageHeader header;

		int notice_type;
		int team_member_index;			//����index
		int param1;
		int param2;
	};

	class CSLeaderPartnerInfo	//!< ����ӳ��Ļ����Ϣ 9197
	{
	public:
		CSLeaderPartnerInfo();
		MessageHeader header;
	};

	class SCLeaderPartnerInfo	//!< ���ضӳ��Ļ����Ϣ 9198
	{
	public:
		SCLeaderPartnerInfo();
		MessageHeader header;

		struct Info
		{
			int partner_id;
			int maxhp;
			int curhp;
			int maxmp;
			int curmp;
			int member_index;
		};

		int num;
		Info info[MAX_TEAM_MEMBER_NUM];
	};

	class CSLeaderCheckMemberItem	//!< 9199 �ӳ��������Ա���е�����Ϣ
	{
	public:
		CSLeaderCheckMemberItem();
		MessageHeader header;

		int item_id;
		int item_num;
	};

	class SCLeaderCheckMemberItemRet	//!< 9200 �ӳ��������Ա���е�����Ϣ����
	{
	public:
		SCLeaderCheckMemberItemRet();
		MessageHeader header;
		struct Member_item_check
		{
			Member_item_check() :member_idx(-1), item_num(0) {}
			int member_idx;			//!< ��Աindex
			int item_num;			//!< ӵ�е�������
		};

		int check_num;				//!< ����Ա����
		Member_item_check member_info[MAX_TEAM_MEMBER_NUM];
	};

	class CSReqJoinTeamInfo	//!< 9202		�������������Ϣ
	{
	public:
		CSReqJoinTeamInfo();
		MessageHeader header;
		
		short is_from_cross;
		short invite_type;
		int team_index;
		int inviter_uid;
	};

	class SCTeamRedNameNotifyArea			//!<9203 �����Ƿ������֪ͨ��������
	{
	public:
		SCTeamRedNameNotifyArea();
		MessageHeader header;

		int is_red;
		int team_id;
	};

	class CSTeamLeaderEjectWindowsReq
	{
	public:
		CSTeamLeaderEjectWindowsReq();
		MessageHeader header;

		int windows_type;//��������--�ͻ����Լ�����
	};

	class SCTeamLeaderEjectWindowsInfo
	{
	public:
		SCTeamLeaderEjectWindowsInfo();
		MessageHeader header;

		int windows_type;
	};

	class SCTeamRoleRecruitInfo	//!< 9206 ���鵯��
	{
	public:
		SCTeamRoleRecruitInfo();
		MessageHeader header;

		int team_id;			// ����ID
		int team_type;			// ����Ŀ��
		GameName leader_name;	// �ӳ�����
		short level_low;		// ��͵ȼ�
		short level_high;		// ��ߵȼ�
		short cur_member_num;	// ��ǰ��Ա��
		short max_member_num;	// ����Ա��	
	};

	class CSTeamRoleRecruitAnswer	//!< 9207 ��Ӧ��ļ
	{
	public:
		CSTeamRoleRecruitAnswer();
		MessageHeader header;

		enum ANSWER_TYPE
		{
			ACCEPT = 0,				// ����
			REFUSE_MANUALLY = 1,	// �ֶ��ܾ�
			AUTO_REFUSE = 2,		// �Զ��ܾ�
		};

		int answer_type;
	};
}

#pragma pack(pop)

#endif

