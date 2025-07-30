#ifndef __MSG_ACTIVITY_HPP__
#define __MSG_ACTIVITY_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/bitmapfunction.hpp"
#include "servercommon/userprotocal/msgheader.h"
#include "config/activityconfig/chongzhiconfig.hpp"
#include "gameworld/global/randactivity/randactivityimpl/randactivitytianmingdivination.hpp" 
#include "servercommon/rolecrystalfbdef.h"
#include "servercommon/roleactivity/ramazesummerparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	class SCActivityStatus		//!< 1200
	{
	public:
		SCActivityStatus();
		MessageHeader header;

		int activity_open_type;
		short activity_type;
		char status;
		char is_broadcast;
		unsigned int next_status_switch_time;
		unsigned int param_1;
		unsigned int param_2;
	};

	class CSActivityEnterReq											//!< 1203 ����
	{
	public:
		CSActivityEnterReq();
		MessageHeader header;

		int activity_type;
		int param1;
	};

	///////////////////////////////////////////////////С�Ӿ���/////////////////////////////////////////////////////////////////
	class CSActivityTeamFightMatchReq									//!< 1204 С�Ӿ���-����ƥ��
	{
	public:
		CSActivityTeamFightMatchReq();
		MessageHeader header;

		enum TYPE
		{
			TYPE_CANCEL = 0,			//!<ȡ��ƥ��
			TYPE_MATCH,					//!<����ƥ��
			TYPE_ROLE_INFO,				//!<��ȡ������Ϣ
			TYPE_RANK_INFO,				//!<��ȡ���а���Ϣ
			TYPE_RECORD_INFO,			//!<��ȡս����Ϣ��Ϣ
			TYPE_RECORD_ONLY_INFO,		//!<ֻ���ҵ�ս����Ϣ��Ϣ
			TYPE_FETCH_BOX,				//!<��ȡ����		param1��ʤ��0,��ʤ��1 �������ACT_TEAM_FIGHT_BOX_TYPE
		};

		int is_match;
		int param1;						//��ʱֻ����ȡ����ʱʹ��
	};

	class SCActivityTeamFightNotice									//!< 1205 С�Ӿ���-ƥ��״̬
	{
	public:
		SCActivityTeamFightNotice();
		MessageHeader header;

		enum MATCH_TYPE
		{
			MATCH_TYPE_NO = 0,	// δƥ��
			MATCH_TYPE_ING,		// ƥ����
		};

		int is_match;
	};

	class SCActivityTeamFightScore									//!< 1206 С�Ӿ���-����
	{
	public:
		SCActivityTeamFightScore();
		MessageHeader header;

		int score;				//!< ����
		int rank;				//!< ����
		int victory_count;		//!< ʤ��
		int defeat_count;		//!< �ܳ�
		int continue_victory_count;		//!< ��ʤ
		int box_fetch_flag;		//!< ������ȡ��¼(�����Ʊ��)
		int battle_count;		//!< ս������
	};
	static const int RANK_MAX_COUNT = 10;

	class SCActivityTeamFightRank									//!< 1210 С�Ӿ���-����
	{
	public:
		SCActivityTeamFightRank();
		MessageHeader header;

		struct UserData
		{
			GameName name;
			short win_rate;
			short score;
			int level;
		};

		int count;
		UserData rank_list[RANK_MAX_COUNT];
	};

	static const int RECORD_MAX_COUNT = 20;
	class SCActivityTeamFightRecord									//!< 1211 С�Ӿ���-ս��
	{
	public:
		SCActivityTeamFightRecord();
		MessageHeader header;

		struct UserData
		{
			GameName name1;
			GameName name2;
		};

		int count;
		UserData record_list[RECORD_MAX_COUNT];
	};

	class SCActivityTeamFightMatchSucc									//!< 1215 С�Ӿ���-����֪ͨ
	{
	public:
		SCActivityTeamFightMatchSucc();
		MessageHeader header;

	};
	
	///////////////////////////////////////////////////�Թ�����/////////////////////////////////////////////////////////////////
	class CSActivityMazeRacerHandleReq			//!< 1207 �Թ�����-��������
	{
	public:
		CSActivityMazeRacerHandleReq();
		MessageHeader header;

		enum MazeRacerOperatorType
		{
			MAZE_RACER_REQ_TRANSP = 0,			//!< ������
			MAZE_RACER_USE_AMULET = 1,			//!< ʹ�÷���
			MAZE_RACER_ROLE_INFO = 2,			//!< ��ɫ��Ϣ

			MAZE_RACER_OP_TYPE_MAX
		};

		short op_type;	//!< 0:������		1:ʹ�÷���
		short param1;	//!< 0:				1:����ID
	};

	class SCActivityMazeRacerAmuletNum			//!< 1208 �Թ�����-�����������
	{
	public:
		SCActivityMazeRacerAmuletNum();
		MessageHeader header;

		int type_count;		//!< ӵ�еķ���������
		PtcItemInfo amulet_num[ACTIVITY_MAZE_RACER_AMULET_MAX_TYPE];	//!< ӵ�еķ����б�
	};

	struct MazeRacerRankItem
	{
		MazeRacerRankItem() : pass_time(0), rank_posi(0), mem_count(0) {};

		UInt32 pass_time;		//!< ͨ��ʱ��(��)
		UInt16 rank_posi;		//!< ����
		UInt16 mem_count;		//!< ��Ա����
		ActivityRoleInform mem_list[MAX_TEAM_MEMBER_NUM];	//!< ��Ա��Ϣ�б�
	};

	class SCActivityMazeRacerRankLists : public IMessageSerializer			//!< 1209 �Թ����� - ���а���Ϣ
	{
	public:
		SCActivityMazeRacerRankLists();
		MessageHeader header;
		
		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteUShort(maze_id, buffer, buffer_size, out_length);
			result = result && WriteUShort(list_num, buffer, buffer_size, out_length);

			for (int rank_idx = 0; rank_idx < list_num && rank_idx < ACTIVITY_RANK_SHORTER_NUM_MAX; ++rank_idx)
			{
				const MazeRacerRankItem & list_obj = rank_list[rank_idx];
				result = result && WriteUInt(list_obj.pass_time, buffer, buffer_size, out_length);
				result = result && WriteUShort(list_obj.rank_posi, buffer, buffer_size, out_length);
				result = result && WriteUShort(list_obj.mem_count, buffer, buffer_size, out_length);

				for (int mem_idx = 0; mem_idx < list_obj.mem_count && mem_idx < MAX_TEAM_MEMBER_NUM; ++mem_idx)
				{
					const ActivityRoleInform & mem_obj = list_obj.mem_list[mem_idx];
					result = result && WriteInt(mem_obj.role_uid, buffer, buffer_size, out_length);
					result = result && WriteUShort(mem_obj.level, buffer, buffer_size, out_length);
					result = result && WriteUShort(mem_obj.prof, buffer, buffer_size, out_length);
					result = result && WriteUShort(mem_obj.avatar, buffer, buffer_size, out_length);
					result = result && WriteUShort(mem_obj.headshot_id, buffer, buffer_size, out_length);
					result = result && WriteStrN(mem_obj.role_name, sizeof(GameName), buffer, buffer_size, out_length);
					result = result && WriteStrN(mem_obj.guild_name, sizeof(GuildName), buffer, buffer_size, out_length);
				}
			}

			return result;
		}
		
		UInt16 maze_id;		//!< �Թ�ID
		UInt16 list_num;	//!< �����б�����
		MazeRacerRankItem rank_list[ACTIVITY_RANK_SHORTER_NUM_MAX];	//!< �����б���Ϣ
	};

	class SCActivityMazeRacerWaitsInfo			//!< 1212 �Թ����� - �Ⱥ������Ϣ
	{
	public:
		SCActivityMazeRacerWaitsInfo();
		MessageHeader header;

		UInt32 next_begin_timestamp;		//!< ��һ���Ŀ���ʱ���
		int cur_waiting_role_num;			//!< ��ǰ�����ȴ������

		UInt16 min_level;		//!< ��ǰ������С�ȼ�
		UInt16 max_level;		//!< ��ǰ�������ȼ�
	};

	class SCActivityMazeRacerCopysInfo			//!< 1213 �Թ����� - ������Ϣ
	{
	public:
		SCActivityMazeRacerCopysInfo();
		MessageHeader header;

		struct MazeRacerCopyRankItem
		{
			MazeRacerCopyRankItem()
			{
				memset(leader_name, 0, sizeof(GameName));
				cur_layer = 0;
				leader_uid = 0;
				team_index = 0;
			}

			GameName leader_name;		//!< �ӳ�����
			int cur_layer;				//!< ��ǰ����
			int leader_uid;				//!< �ӳ�UID
			int team_index;
		};

		UInt32 begin_time;		//!< ���Թ���ʼ��ʱ���(��)
		UInt16 reserve_ush;		//!< �����֣���ʵ��
		UInt16 list_num;		//!< �����б��ܳ�
		MazeRacerCopyRankItem rank_list[ACTIVITY_RANK_SHORTER_NUM_MAX];		//!< �����б�
	};

	class SCActivityMazeRacerPassdInfo			//!< 1214 �Թ����� - ͨ����Ϣ
	{
	public:
		SCActivityMazeRacerPassdInfo();
		MessageHeader header;

		UInt32 pass_time;			//!< ͨ������ʱ��(��)
		UInt16 rank_posi;			//!< ��ǰ����
		UInt16 total_encounter;		//!< �ܼ����ִ���
		int award_num;				//!< ��Ʒ�б���
		PtcItemInfo award_list[ACTIVITY_AWARD_NUM_MAX];		//!< ��Ʒ��ϸ��Ϣ
	};

	class SCActivityMazeRacerCurseInfo			//!< 1216 �Թ����� - ������Ϣ
	{
	public:
		SCActivityMazeRacerCurseInfo();
		MessageHeader header;

		ObjID obj_id;
		UInt16 curse_type[MAZE_AMULET_EFFECT_TYPE_MAX];		//!< ��С4������ID - (-1) ����û���ܵ�����Ч��
	};

	class SCActivityMazeRacerActInsert			//!< 1217 �Թ����� - ���������
	{
	public:
		SCActivityMazeRacerActInsert();
		MessageHeader header;

		UInt16 maze_id;			//!< �ԌmID
		UInt16 reserve_sh;		//!< ������
		MazeRacerRankItem rank_info;		//!< ������Ϣ
	};

	class SCActivityMazeRacerSceneInfo			//!< 1240 �Թ����� - ������Ϣ (���볡���ͷ�)
	{
	public:
		SCActivityMazeRacerSceneInfo();
		MessageHeader header;

		unsigned int time;  //����ĳ���ʱ��
		int x;			//������λ��
		int y;
		unsigned int start_time;	//����Թ���ʼʱ��
		int layer;			//����
	};

	class SCActivityMazeRacerRoleInfo			//!< 1253 �Թ����� - ��ɫ��Ϣ
	{
	public:
		SCActivityMazeRacerRoleInfo();
		MessageHeader header;

		int get_pass_times;			//���ͨ�ؽ�������
	};

	///////////////////////////////////////////////////������Ҷ�/////////////////////////////////////////////////////////////////
	class CSActivityGuildFightOperatorReq
	{
	public:
		CSActivityGuildFightOperatorReq();			//!< 1220: ������Ҷ� - ��������
		MessageHeader header;

		enum GUILD_FIGHT_OPERATOR_TYPE
		{
			GF_OP_TYPE_REQ_ALL_RANK_INFOR = 0,		//!< �������������ͱ��������Ϣ(�����а�)
			GE_OP_TYPE_REQ_CLOSE_RANK = 1,			//!< ����ر����а� 
		};

		int op_type;		//!< ��������
	};

	struct GuildFightRankItem
	{
		GuildFightRankItem()
		{
			this->Reset();
		}

		void Reset()
		{
			rank_posi = 0;
			total_score = 0;
			win_times = 0;
			battle_times = 0;
			memset(guild_name, 0, sizeof(GuildName));
		}

		int rank_posi;			//!< ����λ�� - 0 ��ʾδ�ϰ�
		int total_score;		//!< �ܻ���
		int win_times;			//!< ʤ������
		int battle_times;		//!< ս������ - ע�⣬�����������ʤ�ʻᱣ����λС��
		GuildName guild_name;	//!< ��������
	};

	class SCActivityGuildFightRankAllInfo
	{
	public:
		SCActivityGuildFightRankAllInfo();			//!< 1221: ������Ҷ� - ���а���Ϣ
		MessageHeader header;

		int resi_role;			//!< ʣ�������
		int resi_family;		//!< ʣ�������
		int list_num;
		GuildFightRankItem list_info[ACTIVITY_RANK_DEFAULT_NUM_MAX];
	};

	class SCActivityGuildFightMyGuildInfo
	{
	public:
		SCActivityGuildFightMyGuildInfo();			//!< 1222: ������Ҷ� - �ҵļ�����Ϣ
		MessageHeader header;

		enum GUILD_FIGHT_TYPE
		{
			GUILD_FIGHT_TYPE_INFO = 0,					//�����·���Ϣ
			GUILD_FIGHT_TYPE_END_NOTICE_AND_INFO = 1,	//����ʱ�������·��ҵļ���������Ϣ
		};

		int type;
		GuildFightRankItem my_family_info;			//!< �ҵļ����������Ϣ
	};

	class SCActivityGuildFightParticipNum
	{
	public:
		SCActivityGuildFightParticipNum();			//!< 1225: ������Ҷ� - ����������Ϣ
		MessageHeader header;

		int guild_participate_num;					//!< �����������
	};

	/////////////////////////////////////////////////// ��ֵ ///////////////////////////////////////////////////
	class SCChongzhiInfo						//!< 1218 ��ҳ�ֵ��Ϣ
	{
	public:
		SCChongzhiInfo();
		MessageHeader header;

		long long history_chongzhi_num;			//!< ��ʷ�ܳ�ֵ��
		long long today_chongzhi_num;			//!< ��ҽ����ֵ������
		int chongzhi_fanli_reward_fetch_flag;	//!< ��ֵ����������ȡ���
		int reset_level_num;					//!< ÿ������ʱ�ĵȼ�
	};

	class CSChongzhiOperateReq						//!< 1219 ��ҳ�ֵ��ع�������
	{
	public:
		CSChongzhiOperateReq();
		MessageHeader header;

		enum OPER_TYPE
		{
			OPER_TYPE_SEND_CHONGZHI_INFO = 0,		//!< 0 ��ȡ��ҳ�ֵ�����Ϣ
			OPER_TYPE_FANLI_REWARD_FETCH = 1,		//!< 1 ��ȡ��������  param1:seq
		};

		int oper_type;
		int param1;
		int param2;
	};

	////////////////////////////////////// ��� /////////////////////////////////
	class CSCrossStartReq			//!< 2800 ���������
	{
	public:
		CSCrossStartReq();
		MessageHeader header;
	};

	class SCCrossStartAck			//!< 2801 ����������
	{
	public:
		SCCrossStartAck();
		MessageHeader header;

		char gateway_hostname[64];	//!< gateway��ip������
		int gateway_port;			//!< gateway�Ķ˿�

		GameName role_name;			//!< ��ɫ��
		int new_uid;				//!< �µ�uid
		int scene_id;				//!< ����ID			\note ������֤
		int last_scene_id;			//!< ��һ������ID	\note ������֤
		unsigned int time;			//!< ʱ���			\note ������֤
			
		SessionKey session_key;		//!< �Ự��Կ		\note ������֤
	};

	class CSBackToOriginServer		//!< 2802 ����ر���
	{
	public:
		CSBackToOriginServer();
		MessageHeader header;
	};

	class SCBackToOriginServer		//!< 2803 �ر���������Ϣ
	{
	public:
		SCBackToOriginServer();
		MessageHeader header;

		int scene_id;				//!< ����ID			\note ������֤
		int last_scene_id;			//!< ��һ������ID	\note ������֤
		unsigned int time;			//!< ʱ���			\note ������֤

		SessionKey session_key;		//!< �Ự��Կ		\note ������֤

		char gateway_hostname[64];	//!< gateway������
		unsigned short gateway_port;//!< gateway�Ķ˿�
		short reserve_sh;			//!< ������
	};

	class CSNewChongzhiConfigInfoReq // 2018
	{
	public:
		CSNewChongzhiConfigInfoReq();
		MessageHeader header;

		int currency; // ������� ��0�������  1����̨��  2������
		char spid_str[4]; //���ݿͻ�����ʾ��spid����
	};

	class SCNewChongzhiConfigInfo // 2042
	{
	public:
		SCNewChongzhiConfigInfo();
		MessageHeader header;

		struct ChongzhiInfo
		{
			char seq;												// ��ţ���0��ʼ
			char extra_reward_type;									// ���⽱������ (0:������  1����Ԫ  2�������Ա  3��ͭ��  4��Ԫ����
			short reserve_sh;										// ������
			int add_gold;											// �Ӷ���Ԫ��
			int extra_reward;										// �����͵Ľ�������
			int money_show;											// 100���Ľ� ��Ҫ����100�������������С����
			char description_first_chongzhi[DESCRIPTION_STR_LEN];	// �״γ�ֵ����   DESCRIPTION_STR_LEN = 128
		};

		short currency_type;										// ԭ��������Ҫ��ʾ�Ľ������ ��0�������  1����̨��  2������
		short info_count;											// info����Ԫ������
		ChongzhiInfo info_list[CHONGZHI_REWARD_CONFIG_MAX_NUM];		// info����  CHONGZHI_REWARD_CONFIG_MAX_NUM = 32
	};

	class SCNewChongzhiConfigReloadNotice // 2043  �ͻ����յ���Э��ʱ����2042
	{
	public:
		SCNewChongzhiConfigReloadNotice();
		MessageHeader header;
	};

	class SCWorldQuestionNotice // ��������·� 3205
	{
	public:
		SCWorldQuestionNotice();
		MessageHeader header;

		int now_answer_subject;						//��Ŀid
		long long answer_tamp;						//�����ش�ʱ���
		long long show_answer_tamp;					//չʾ��ʱ���
		long long next_subject_refresh_tamp;		//��һ����Ŀˢ��ʱ��
		long long answer_end_tamp;					//����ʱ���
		char is_broadcast;							//�Ƿ�㲥
		char reserve_ch;							//�Ƿ������ȷ(����)
		short level_limit;							//�ȼ�����,�ﵽ�õȼ�����ҲŸ���ʾ
		int role_answer_idx;						//��Ҵ��±꡾1-4��
	};

	class CSWorldQuestionReq// ����������� 3206
	{
	public:
		CSWorldQuestionReq();
		MessageHeader header;

		int type;				// 0:�����·�
	};

	/////////////////////////////////////////////////��һ�а�(���˰������Ҷ�)//////////////////////////////////////////////////////////
	class CSActivityOnlyFightOperatorReq			//!< 1228: ��һ�а�(���˰������Ҷ�) - ��������
	{
	public:
		CSActivityOnlyFightOperatorReq();			
		MessageHeader header;

		enum ONLY_FIGHT_OPERATOR_TYPE
		{
			OF_OP_TYPE_REQ_ALL_RANK_INFO = 0,		//!< ��������������Ϣ(�����а�)
			OF_OP_TYPE_REQ_CLOSE_RANK = 1,			//!< ����ر����а� 
		};

		int op_type;		//!< ��������
	};

	struct OnlyFightRankItem
	{
		OnlyFightRankItem()
		{
			this->Reset();
		}

		void Reset()
		{
			rank_posi = 0;
			total_score = 0;
			win_times = 0;
			battle_times = 0;
			memset(role_name, 0, sizeof(GameName));
		}

		int rank_posi;			//!< ����λ�� - 0 ��ʾδ�ϰ�
		int total_score;		//!< �ܻ���
		int win_times;			//!< ʤ������
		int battle_times;		//!< ս������ - ע�⣬�����������ʤ�ʻᱣ����λС��
		GameName role_name;		//!< �������
	};

	class SCActivityOnlyFightRankAllInfo			//!< 1229: ��һ�а�(���˰������Ҷ�) - ���а���Ϣ
	{
	public:
		SCActivityOnlyFightRankAllInfo();			
		MessageHeader header;

		int resi_role_num;			//!< ʣ�������
		int list_num;
		OnlyFightRankItem list_info[ACTIVITY_RANK_DEFAULT_NUM_MAX];
	};

	class SCActivityOnlyFightMyInfo					//!< 1230: ��һ�а�(���˰������Ҷ�) - �ҵ���Ϣ
	{
	public:
		SCActivityOnlyFightMyInfo();			
		MessageHeader header;

		enum ONLY_FIGHT_MY_INFO_TYPE
		{
			ONLY_FIGHT_MY_INFO_TYPE_SEND = 0,		//�����ҵ�������Ϣ
			ONLY_FIGHT_MY_INFO_TYPE_NOTICE = 1,		//֪ͨ��ʾ���а������·��ҵ�������Ϣ
		};

		int type;							
		OnlyFightRankItem m_info;			//!< �ҵ�������Ϣ
	};

	class CSActivitySignUpReq			//!< 1231: ��ʱ���������
	{
	public:
		CSActivitySignUpReq();
		MessageHeader header;

		int seq;					//����seq��ȡ���ĸ��
	};

	class SCActivitySignUpInfo			//!< 1232: ��ʱ�������Ϣ�·�
	{
	public:
		SCActivitySignUpInfo();
		MessageHeader header;

		BitMap<MAX_SIGN_UP_NUM> m_sign_up_flag;		//�Ƿ���
	};

	class SCActivityEnterInfo			//!< 1241  ��볡��Ϣ�·�
	{
	public:
		SCActivityEnterInfo();
		MessageHeader header;

		short   activity_type;					//ACTIVITY_TYPE_MAX
		short	is_enter_flag;					//�Ƿ�����볡 1:�����볡 0:�����볡
		unsigned int enter_end_timestamp;		//�볡����ʱ���
	};

	class SCActivityGuildFightSpecialInfo		//!< 1242 ������Ҷ�������Ϣ�·�
	{
	public:
		SCActivityGuildFightSpecialInfo();
		MessageHeader header;

		int resi_role;			//!< ʣ�������
		int resi_family;		//!< ʣ�������		
	};

	class SCActivityOnlyFightSpecialInfo		//!< 1243 ��һ�а�������Ϣ�·�
	{
	public:
		SCActivityOnlyFightSpecialInfo();
		MessageHeader header;

		int resi_role;			//!< ʣ�������
	};

	class SCRAMoneyAddInfo		//!< 1244 ���������������
	{
	public:
		SCRAMoneyAddInfo();
		MessageHeader header;

		int rand_activity;		//!< ���
		int add_money;			//!< ����ֵ
	};

	class SCRAGiveSecretKeyInfo		//!< 1245 ��Ѱ������
	{
	public:
		SCRAGiveSecretKeyInfo();
		MessageHeader header;

		int ra_reward_flag;		//!< ������ȡ�������ƣ�
		unsigned int frist_pass_flag[CRYSTAL_FB_FIRST_FLAG];
	};

	class SCRACumulativeChargeInfo		//!< 1246 �ۼƳ�ֵ
	{
	public:
		SCRACumulativeChargeInfo();
		MessageHeader header;

		int has_get_reward;				//!< �Ƿ��Ѿ���ȡ����
		long long acc_price;			//!< �ۼƳ�ֵ���
		int is_received;				//!< �Ƿ�����ȡ����ѽ���,0--δ��ȡ��1--����ȡ
	};

	class CSActivityAdvanceFetchReq				//!< 1247 ��ʱ���ǰ��ȡ��������
	{
	public:
		CSActivityAdvanceFetchReq();
		MessageHeader header;

		enum ACTIVITY_ADVANCE_FETCH_OP_TYPE
		{
			ACTIVITY_ADVANCE_FETCH_OP_TYPE_PARTICIP_AWARD = 0,			// ��ȡ���뽱��  
			ACTIVITY_ADVANCE_FETCH_OP_TYPE_SURVIVAL_AWARD = 1,			// ��ȡ���� param1:live_num���������)

			ACTIVITY_ADVANCE_FETCH_OP_TYPE_MAX
		};

		short activity_type;				//ACTIVITY_TYPE_MAX
		short op_type;
		int param1;
	};

	class SCActivityAdvanceFetchInfo		//!< 1248 ��ʱ���ǰ��ȡ������Ϣ
	{
	public:
		SCActivityAdvanceFetchInfo();
		MessageHeader header;

		short activity_type;
		short level;
		short reserve_sh;
		char particip_award_flag;		// ���뽱��ʶ�� 0:δ��ȡ 1:����ȡ 
		char survival_award_flag;		// ������ʾ(������)
	};

	class SCActivityConfigReloadNotice			//1250 �ȸ�������ض�֪ͨ
	{
	public:
		SCActivityConfigReloadNotice();
		MessageHeader header;
	};

	class SCNoticeClientRestart				// 1264 ֪ͨ�ͻ�������
	{
	public:
		SCNoticeClientRestart();
		MessageHeader header;

		char version_msg[64];
	};

	class SCCmdBuyItemCheckRet					//1251 
	{
	public:
		SCCmdBuyItemCheckRet();
		MessageHeader header;

		int buy_type;
		union 
		{
			unsigned long long unique_id;     //�൱������int��int1��int2
			unsigned int p[2];
		};
		
		int cfg_ver;
	};

	class CSCmdBuyItemCheckReq			//!< 1252 ��ͨģ��ֱ���������ü��
	{
	public:
		CSCmdBuyItemCheckReq();
		MessageHeader header;
		
		int buy_type;				//BUY_TIME_TYPE	 
		int param1;	
		int param2;
		int cfg_ver;				
	};

	class CSMonthCardReq		//!< 2280 �¿��ܿ�����
	{
	public:
		CSMonthCardReq();
		MessageHeader header;

		enum MONTH_CARD_REQ_TYPE
		{
			MONTH_CARD_REQ_TYPE_INFO = 0,				//������Ϣ�·�
			MONTH_CARD_REQ_TYPE_BUY_ITEM = 1,			//�����̳���Ʒ P1:seq(����)
			MONTH_CARD_REQ_TYPE_BUY_CMD_CHECK = 2,		//����ֱ������ǰ��� P1:seq P2:���
			MONTH_CARD_REQ_TYPE_BUY_CARD_CHECK = 3,		//�����ܿ�/�¿�ǰ��� P1:card_type P2:��� 
			MONTH_CARD_REQ_TYPE_FETCH_DAY_REWARD =4,	//��ȡÿ�ս��� P1:card_type
		};

		int req_type;
		int param1;
		int param2;
	};

	class SCMonthCardInfo		//!< 2281 �¿��ܿ���Ϣ�·�
	{
	public:
		SCMonthCardInfo();
		MessageHeader header;

		unsigned char is_fetch_card_day_reward_flag;							//�¿�/�ܿ�ÿ�ս�����ȡ��ʶ ������(card_type)
		char reserve_ch;
		short month_card_level;													//ˢ�µȼ�
		unsigned int week_card_end_timestamp;									//�ܿ�����ʱ���
		unsigned int month_card_end_timestamp;									//�¿�����ʱ���
		int card_shop_buy_times[MAX_MONTH_CARD_SHOP_ITEM_NUM];					//�¿��ܿ��̳ǹ����¼
	};

	class SCQiongLouYuYanInfo					//1252 ����-��¥�����·�
	{
	public:
		SCQiongLouYuYanInfo();
		MessageHeader header;

		int accumulate_num;				//������
		int now_status;					//��ǰ״̬
		unsigned int next_status_tamp;	//״̬����ʱ���
		int banquet_type;				//�������
	};

	class SCQiongLouYuYanRoleInfo					//1253 ����-��¥���������Ϣ�·�
	{
	public:
		SCQiongLouYuYanRoleInfo();
		MessageHeader header;

		int reward_flag;//�콱���������
		int eat_food_times;//��ϯ����
		int next_eat_tamp;//�´γ�ϯʱ�䣨0Ҳ�ǿ��Գ�ϯ��
	};

	class SCMazeSummerMapInfo					//1254 ����-�����Թ���ͼ�·�
	{
	public:
		SCMazeSummerMapInfo();
		MessageHeader header;

		Posi m_pos;
		RAMazeSummerMap m_map[MAX_MAZE_SUMMER_POS_X][MAX_MAZE_SUMMER_POS_Y];
	};

	class SCMazeSummerSingleMapInfo					//1255 ����-�����Թ���ͼ�����·�
	{
	public:
		SCMazeSummerSingleMapInfo();
		MessageHeader header;

		Posi change_pos;
		RAMazeSummerMap map_info;
	};

	class SCMazeSummerShopInfo					//1256 ����-�����Թ��̵��·�
	{
	public:
		SCMazeSummerShopInfo();
		MessageHeader header;

		int m_shop_num;
		RAMazeSummerShop m_shop[MAX_MAZE_SUMMER_SHOP_NUM];
	};

	class SCMazeSummerQuestionInfo					//1257 ����-�����Թ������·�
	{
	public:
		SCMazeSummerQuestionInfo();
		MessageHeader header;

		int question_id;
	};

	class SCMazeSummerTaskInfo					//1258 ����-�����Թ������·�
	{
	public:
		SCMazeSummerTaskInfo();
		MessageHeader header;

		int play_info[MAX_MAZE_SUMMER_TASK_NUM];//���λ��Ϸ��¼
		RAMazeSummerTaskInfo m_task[MAX_MAZE_SUMMER_TASK_NUM];//�����¼
	};

	class SCMazeSummerStatusInfo					//1259 ����-�����Թ�����״̬�·�
	{
	public:
		SCMazeSummerStatusInfo();
		MessageHeader header;

		int is_playing;//�Ƿ����ڽ�����Ϸ
		int play_times;
	};

	class SCMazeSummerPlayFinishInfo					//1260 ����-�����Թ��淨�����·�
	{
	public:
		SCMazeSummerPlayFinishInfo();
		MessageHeader header;

		int is_pass;//�Ƿ�ͨ��
	};

	class SCMazeSummerMyPosInfo					//1261 ����-�����Թ��ҵ�λ���·�
	{
	public:
		SCMazeSummerMyPosInfo();
		MessageHeader header;

		Posi m_pos;//�ҵ�λ��
	};


	class CSActivitySilkRoadsReq		//!< 1270  ��ʱ�-˿��֮·����
	{
	public:
		CSActivitySilkRoadsReq();
		MessageHeader header;

		enum SILK_ROADS_REQ_OP_TYPE
		{
			SILK_ROADS_REQ_OP_TYPE_ROLE_INFO = 0,				//�����ɫ���Ϣ
			SILK_ROADS_REQ_OP_TYPE_READY = 1,					//׼��,���������һЩ����
			SILK_ROADS_REQ_OP_TYPE_START_OUTSET = 2,			//��ʼ����
			SILK_ROADS_REQ_OP_TYPE_KEEP_GOING = 3,				//��������
			SILK_ROADS_REQ_OP_TYPE_BUY_COMMODITY = 4,			//������� param1:����seq param2:count
			SILK_ROADS_REQ_OP_TYPE_SALE_COMMODITY = 5,			//�������� param1:����seq param2:count
			SILK_ROADS_REQ_OP_TYPE_BUY_NEWS = 6,				//������Ϣ 
			SILK_ROADS_REQ_OP_TYPE_EXPANSION_WAREHOUSE = 7,		//����ֿ� param1:�������
			SILK_ROADS_REQ_OP_TYPE_BUY_SUPPLY = 8,				//���򲹸� param1:�������
			SILK_ROADS_REQ_OP_TYPE_END = 9,						//����ó��֮��
			SILK_ROADS_REQ_OP_TYPE_RANK_ALL_INFO = 10,			//����������Ϣ
			SILK_ROADS_REQ_OP_TYPE_ALL_TRADING_INFO = 11,		//����ó����Ϣ�б�
		};

		int op_type;
		int param1;
		int param2;
	};

	class SCActivitySilkRoadsSimpleInfo		//!< 1271 ��ʱ�-˿��֮·������Ϣ�·�
	{
	public:
		SCActivitySilkRoadsSimpleInfo();
		MessageHeader header;

		enum SILK_ROADS_SIMPLE_TYPE
		{
			SILK_ROADS_SIMPLE_TYPE_READY = 0,					// ������� param1:skill_id param2:line_group_id
			SILK_ROADS_SIMPLE_TYPE_START_OUTSET = 1,			// ��ʼ���� param1:cur_city_index
			SILK_ROADS_SIMPLE_TYPE_KEEP_GOING = 2,				// �������� param1:cur_city_index param2:news_pool_count
			SILK_ROADS_SIMPLE_TYPE_BUY_NEWS = 3,				// ������Ϣ param1:buy_news_count param2:news_pool_count
			SILK_ROADS_SIMPLE_TYPE_EXPANSION_WAREHOUSE = 4,		// ����ֿ� param1:buy_warehouse_count 
			SILK_ROADS_SIMPLE_TYPE_BUY_SUPPLY = 5,				// ���򲹸� param1:buy_supply_count
			SILK_ROADS_SIMPLE_TYPE_END = 6,						// ����ó��֮��,���� param1:have_coin_count
		};

		int simple_type;
		int param1;
		int param2;
	};

	class SCActivitySilkRoadsRoleAllInfo : public IMessageSerializer		//!< 1272 ��ʱ�-˿��֮·��ɫ������Ϣ
	{
	public:
		SCActivitySilkRoadsRoleAllInfo();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(role_data.cumulation_coin_count, buffer, buffer_size, out_length);
			result = result && WriteInt(role_data.cumulation_trading_count, buffer, buffer_size, out_length);
			result = result && WriteInt(role_data.finish_num, buffer, buffer_size, out_length);

			result = result && WriteShort(role_data.line_group_id, buffer, buffer_size, out_length);
			result = result && WriteShort(role_data.buy_warehouse_count, buffer, buffer_size, out_length);
			result = result && WriteShort(role_data.buy_supply_count, buffer, buffer_size, out_length);
			result = result && WriteInt(role_data.have_coin_count, buffer, buffer_size, out_length);
			result = result && WriteInt(role_data.have_supply_count, buffer, buffer_size, out_length);

			result = result && WriteShort(role_data.init_skill, buffer, buffer_size, out_length);
			result = result && WriteShort(role_data.cur_city_index, buffer, buffer_size, out_length);
			result = result && WriteShort(role_data.trading_num, buffer, buffer_size, out_length);

			result = result && WriteShort(role_data.warehouse_data.cur_max_grid_count, buffer, buffer_size, out_length);
			result = result && WriteShort(role_data.warehouse_data.cur_has_commodity_count, buffer, buffer_size, out_length);
			result = result && WriteInt(news_pool_count, buffer, buffer_size, out_length);
			
			short grid_count = 0;
			int grid_index_list[MAX_SILK_ROADS_GRID_NUM] = {0};
			UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(grid_index_list) >= ARRAY_ITEM_COUNT(role_data.warehouse_data.grid_list));
			for (int i = 0; i < role_data.warehouse_data.cur_max_grid_count && i < ARRAY_LENGTH(role_data.warehouse_data.grid_list); i++)
			{
				if (role_data.warehouse_data.grid_list[i].commodity_seq > 0 && role_data.warehouse_data.grid_list[i].commodity_count > 0)
				{
					grid_index_list[grid_count] = i;
					grid_count++;
				}
			}
			result = result && WriteShort(grid_count, buffer, buffer_size, out_length);
			for (int i = 0; i < ARRAY_LENGTH(grid_index_list) && i < grid_count; i++)
			{
				const ActivitySilkRoadsRoleWarehouseData::GridInfo & grid_info = role_data.warehouse_data.grid_list[grid_index_list[i]];
				result = result && WriteShort(grid_index_list[i], buffer, buffer_size, out_length);
				result = result && WriteShort(grid_info.commodity_seq, buffer, buffer_size, out_length);
				result = result && WriteShort(grid_info.commodity_count, buffer, buffer_size, out_length);
				result = result && WriteFloat(grid_info.commodity_cost, buffer, buffer_size, out_length);
			}

			result = result && WriteShort(trading_log_count, buffer, buffer_size, out_length);
			for (int i = 0; i < trading_log_count && i < ARRAY_LENGTH(trading_log_list); i++)
			{
				const ActivitySilkRoadsRoleTradingLogData & log_data = trading_log_list[i];
				result = result && WriteShort(log_data.log_type, buffer, buffer_size, out_length);
				result = result && WriteShort(log_data.freight_id, buffer, buffer_size, out_length);
				result = result && WriteShort(log_data.from_city_map_seq, buffer, buffer_size, out_length);
				result = result && WriteShort(log_data.target_city_map_seq, buffer, buffer_size, out_length);
				result = result && WriteShort(log_data.commodity_seq, buffer, buffer_size, out_length);
				result = result && WriteInt(log_data.price, buffer, buffer_size, out_length);
			}

			result = result && WriteShort(role_data.city_count, buffer, buffer_size, out_length);
			for (int i = 0; i < role_data.city_count && i < ARRAY_LENGTH(role_data.city_list); i++)
			{	
				const ActivitySilkRoadsRoleCityData & city_info = role_data.city_list[i];
				result = result && WriteShort(city_info.city_map_seq, buffer, buffer_size, out_length);
				result = result && WriteShort(city_info.cur_city_buy_news_count, buffer, buffer_size, out_length);
				result = result && WriteShort(city_info.commondity_count, buffer, buffer_size, out_length);
				for (int k = 0; k < city_info.commondity_count && k < ARRAY_LENGTH(city_info.commodity_list); k++)
				{
					const ActivitySilkRoadsRoleCityData::CommodityItem & commodity_info = city_info.commodity_list[k];
					result = result && WriteShort(commodity_info.commodity_seq, buffer, buffer_size, out_length);
					result = result && WriteUShort(commodity_info.price, buffer, buffer_size, out_length);
				}
			}
		
			return result;
		}

		short reserve_sh;
		short trading_log_count;
		int news_pool_count;
		ActivitySilkRoadsRoleData role_data;
		ActivitySilkRoadsRoleTradingLogData trading_log_list[MAX_SILK_ROADS_TRADING_LOG_NUM];
	};

	class SCActivitySilkRoadsAssetsInfo		//!< 1273 ��ʱ�-˿��֮·�ʲ���Ϣ�·�
	{
	public:
		SCActivitySilkRoadsAssetsInfo();
		MessageHeader header;

		int have_coin_count;			//��ǰӵ�еĻ���(�ʽ�)
		int have_supply_count;			//��ǰӵ�еĲ���
		short cur_max_grid_count;		//��ǰ����ʹ�õ�����(�ѿ�ͨ�ĸ�������)
		short reserve_sh;
	};

	class SCActivitySilkRoadsWarehouseChange	//!< 1274 ��ʱ�-˿��֮·�ֿ�仯
	{
	public:
		SCActivitySilkRoadsWarehouseChange();
		MessageHeader header;

		enum NOTICE_TYPE
		{
			NOTICE_TYPE_DEFAULT = 0,				//Ĭ��
			NOTICE_TYPE_BUY_COMMODITY = 1,			//�������
			NOTICE_TYPE_SALE_COMMODITY = 2,			//��������
			NOTICE_TYPE_REWARD = 3,					//����

			NOTICE_TYPE_MAX
		};

		short warehouse_grid_index;				//�ֿ��ĸ�����
		short notice_type;

		short warehouse_commodity_seq;			//�ø������ĸ��������
		short warehouse_commodity_count;		//�ø��ӻ�������
		float warehouse_commodity_cost;			//�ø��ӻ���ɱ�
	};

	class SCActivitySilkRoadsAllRankInfo	//!< 1275 ��ʱ�-˿��֮·���а���Ϣ�·�
	{
	public:
		SCActivitySilkRoadsAllRankInfo();
		MessageHeader header;

		int rank_count;
		ActivitySilkRoadsRoleRankInfo rank_list[MAX_SILK_ROADS_RANK_NUM];
	};

	class SCActivitySilkRoadsAllTradingLogInfo	//!< 1276 ��ʱ�-˿��֮·ó����Ϣ�б��·�
	{
	public:
		SCActivitySilkRoadsAllTradingLogInfo();
		MessageHeader header;

		short reserve_sh;
		short trading_log_count;
		ActivitySilkRoadsRoleTradingLogData trading_log_list[MAX_SILK_ROADS_TRADING_LOG_NUM];
	};

	class SCActivitySilkRoadsSingleTradingLogNotice		//!< 1277 ��ʱ�-˿��֮·����ó����Ϣ�·�
	{
	public:
		SCActivitySilkRoadsSingleTradingLogNotice();
		MessageHeader header;

		int trading_log_index;
		ActivitySilkRoadsRoleTradingLogData trading_log;
	};

}

#pragma pack(pop)
#endif	// __MSG_ACTIVITY_HPP__