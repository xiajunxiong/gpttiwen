#ifndef __MSG_OTHER_H__
#define __MSG_OTHER_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"
#include "servercommon/commondata.hpp"
#include "gamedef.h"
#include "servercommon/salarydef.hpp"
#include "servercommon/chatserverdef.h"
#include "servercommon/roleconvertshopparam.hpp"
#include "servercommon/mysterioustrialdef.hpp"
#include "servercommon/transactiontaskdef.hpp"
#include "servercommon/activesystemdef.hpp"
#include "servercommon/funcpreviewdef.h"
#include "servercommon/auto_catch_pet_def.hpp"
#include "servercommon/formationdef.hpp"
#include "servercommon/gatherdreamdef.hpp"
#include "servercommon/lunhuiwangchuandef.h"
#include "servercommon/bitmapfunction.hpp"
#include "servercommon/mapexplorationdef.hpp"
#include "servercommon/rankdef.hpp"
#include "servercommon/realmbreakdef.hpp"
#include "servercommon/custominstructiondef.hpp"
#include "servercommon/netherworldgatorydef.h"
#include "servercommon/bigdipperdef.h"
#include "servercommon/roleattrpelletdef.hpp"
#include "servercommon/flyupsystemdef.h"
#include "servercommon/mirageterritorydef.h"
#include "servercommon/loginsystemdef.hpp"
#include "servercommon/yaoxingshiliandef.hpp"
#include "servercommon/taoquannajidef.hpp"
#include "servercommon/dayrewarddef.hpp"
#include "global/rank/partnertoprankshadow.hpp"
#include "global/rank/pettoprankshadow.hpp"
#include "servercommon/roleactivity/racharismatictanabataparam.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCBuffInfo					//!< ���buffʱ�·� 2480
	{
	public:
		SCBuffInfo();
		MessageHeader		header;
		const static int MAX_BUFF_COUNT = 100;
		struct BuffBaseInfo
		{
			BuffBaseInfo() {}
			int origin;
			int type;
			unsigned int begin_time;
			unsigned int end_time;
			int scene_buff;
			NetValueBuffParamData data;
		};
		int buff_count;
		BuffBaseInfo buff_list[MAX_BUFF_COUNT];
	};

	class SCAnswerAllInfo					//!< 1960
	{
	public:
		SCAnswerAllInfo();
		MessageHeader		header;

		int					wst_questions_id;								//!< ��Ŀid
		char				wst_question_today_buy_exp_count;				//!< �ܹ���Ľ��ս�������
		char				wst_question_yesterday_buy_exp_count;			//!< �ܹ�������ս�������
		unsigned short		wst_answer_num;									//!< ����Ĵ�������
	};

	class CSAnswerOp					//!< 1961 
	{
	public:
		CSAnswerOp();
		MessageHeader		header;
		enum OP_TYPE
		{
			OP_TYPE_GET_INFO = 0,			//!< ��ȡ��Ϣ
			OP_TYPE_QUESTION,				//!< �ش���Ŀ	 param1��id[1,4]
			OP_TYPE_BUY_EXP,				//!< ������ param1 = 0 Ϊ�������� param1 = 1 Ϊ���������
			OP_TYPE_GET_WST_INFO,			//!< �����·�������Ϣ
		};

		int op_type;
		int param1;							// 0 - ��������  1 - ��id
	};

	class SCAnswerOverInfo					//!< 1962
	{
	public:
		SCAnswerOverInfo();
		MessageHeader		header;

		long long add_coin;
		long long add_exp;
	};

	class CSExpCrystalOp					//!< 2001 ��ˮ���Ŀ���
	{
	public:
		CSExpCrystalOp();
		MessageHeader		header;
		
	};

	class SCExpCrystalInfo					//!< 2002 ��ˮ������Ϣ
	{
	public:
		SCExpCrystalInfo();
		MessageHeader		header;

		short is_open;				//!< 0Ϊδ����
		short count;					//!< ʣ�����
	};

	class CSTreviFountainOp					//!< 2003 ��Ը�� ��Ը
	{
	public:
		CSTreviFountainOp();
		MessageHeader		header;

	};

	class SCTreviFountainInfo				//!< 2004 ��Ը����Ϣ
	{
	public:
		SCTreviFountainInfo();
		MessageHeader		header;

		struct buff_info
		{
			buff_info():trevi_fountain_buff_id(0), trevi_fountain_buff_expiration_time(0)
			{}

			unsigned int		trevi_fountain_buff_id;						//!< ��Ը�ػ�õ�buff_id
			unsigned int		trevi_fountain_buff_expiration_time;		//!< ��Ը��buff�ĵ���ʱ��
		};

		int count;					//!< ��Ը�ؽ���ʹ�ô���
		buff_info send_info[MAX_TREVI_FOUNTION_TODAY_BUFF_NUM];
	};

	class SCOnlineRewardInfo				//!< 1970 ���߽�����Ϣ
	{
	public:
		SCOnlineRewardInfo();
		MessageHeader		header;

		unsigned int	today_online_time;		//!< ��������ʱ��
		int				reward_fetch_flag;		//!< ������ȡflag
	};

	class SCSignRewardInfo				//!< 1971 ǩ��������Ϣ
	{
	public:
		SCSignRewardInfo();
		MessageHeader		header;

		int				sign_count;					//!< ǩ������
		int				reward_fetch_flag;			//!< ������ȡflag
		int             month_zero_open_day;		//!< ����1��0�����Ŀ�������
	};

	class SCLevelRewardInfo				//!< 1972 �ȼ�������Ϣ
	{
	public:
		SCLevelRewardInfo();
		MessageHeader		header;

		int				reward_fetch_flag;			//!< ������ȡflag
	};

	class CSFetchRewardReq				//!< 1973 ��ȡ����
	{
	public:
		CSFetchRewardReq();
		MessageHeader		header;

		enum REWARD_TYPE
		{
			ONLINE_REWARD = 0,		//!< ���߽���
			SIGN_REWARD = 1,		//!< ǩ������ seq �� day
			LEVEL_REWARD = 2,		//!< �ȼ�����
			SCORE_REWARD = 3,		//!< ���ֽ���
			MONTH_INVESTMENT = 4,	//!< �¶�Ͷ��ǩ��
			FETCH_DAY_FOOD = 5,		//!< ��ȡ������
			FETCH_ONLINE_GIFT = 6,	//!< ��ȡ�������
		};

		int reward_type;
		int seq;
	};

	class CSFindRewardReq				//!< 1974 �һؽ���
	{
	public:
		CSFindRewardReq();
		MessageHeader		header;

		int type;						//!< �����
		int consume_type;				//!< �������� 0Ϊͭ�� 1ΪԪ��
		int count;						//!< �һش���
	};

	class SCFindRewardInfo				//!< 1975 �һؽ�����Ϣ
	{
	public:
		SCFindRewardInfo();
		MessageHeader		header;
		
		BitMap<DAY_ACTIVITY_TYPE_MAX> find_reward_flag;	//!< �һر�ʶ
		int can_find_count[DAY_ACTIVITY_TYPE_MAX];		//!< ���һصĴ���
		int today_find_count[DAY_ACTIVITY_TYPE_MAX];	//!< �������һصĴ���
	};

	UNSTD_STATIC_CHECK(DAY_ACTIVITY_TYPE_MAX == 20 && MAX_DAY_ACTIVITY_TYPE_NUM <= 32);

	class CSRewardFindNotifyCancelReq     //1978 ����ȡ����������ʾ
	{
	public:
		CSRewardFindNotifyCancelReq();		
		MessageHeader header;

	};

	class SCRewardFindNotifys			//!< 1979 ���������һع��ܺ���·�
	{
	public:
		SCRewardFindNotifys();		
		MessageHeader header;

		int notifys;						// 0 �����δ�򿪽���,��ʾ���   1 ��ҽ����Ѿ��򿪹�����,ȡ�����
	};

	class SCDayRewardOnlineGiftInfo		//!< 1990 ��������-���������Ϣ�·�
	{
	public:
		SCDayRewardOnlineGiftInfo();
		MessageHeader header;

		unsigned int online_gift_record_begin_timestamp;	// ��ʼ��¼�������ʱ���
		unsigned int online_gift_times;						// �ۼ�����ʱ���������ۼ�ʱ��Ϊ��(now-online_gift_record_begin_timestamp +online_gift_times)
		BitMap<DAY_REWARD_ONLINE_GIFT_NUM> online_gift_flag;// ���������ȡ��ʶ
	};


	class SCDayRewardDayFoodInfo		//!< 1127 ��������Ϣ�·�
	{
	public:
		SCDayRewardDayFoodInfo();
		MessageHeader header;

		BitMap<DAY_REWARD_DAY_FOOD_SEQ_COUNT> day_food_flag;		//��������ȡ��ʶ	
	};

	class SCPetStrengthenRewardInfo							// ����ǿ��������Ϣ�·�
	{
	public:
		SCPetStrengthenRewardInfo();
		MessageHeader header;

		int pet_strengthen_lv;										//�������ǿ���ȼ�
		BitMap<PET_STRENGTHEN_REWARD_FLAG> has_pet_strengthen_flag;			//����ǿ�������Ƿ�����ȡ
	};

	class CSPetStrengthenRewardReq											//����ǿ����������
	{
	public:
		CSPetStrengthenRewardReq();
		MessageHeader header;

		int seq;
	};

	///////////////////////////////////////// ����Ԥ�� /////////////////////////////////////////
	class CSSystemNoticeGetAward
	{
	public:
		CSSystemNoticeGetAward();		//!< 1750 - ����Ԥ����ȡ����
		MessageHeader header;

		int type_seq;				//!< ���
	};

	class SCSystemNoticeFlagInfo
	{
	public:
		SCSystemNoticeFlagInfo();		//!< 1751 - ����Ԥ����ȡ��Ϣ
		MessageHeader header;

		int fetch_flag;				//!< �������룬������ȡ״̬
	};

	class CSChangeAppearance			//!< 2034 - �ı�����
	{
	public:
		CSChangeAppearance();
		MessageHeader header;
		
		short				avartar_type;		//!< ������������
		short				color;				//!< ��ɫ
	};
	
	//����֪ͨ
	class SCNoticeNearbyOnOpenTreasureChest		//!< 2040 ֪ͨ��������� ���䱻��
	{
	public:
		SCNoticeNearbyOnOpenTreasureChest();
		MessageHeader header;

		ObjID obj_id;			//����objid
		short reserved;
	};

	//���佱���·������ˣ�
	class SCTreasureChestRewardList		//!< 2039 ֪ͨ��ұ�������ʲô����
	{
	public:
		const static int MAX_TREASURE_CHEST_COUNT = 10;//!< ��������������
		struct reward_info
		{
			reward_info():item_id(0),num(0),is_bind(0){}
			int item_id;
			short num;
			short is_bind;
		};
		SCTreasureChestRewardList();
		MessageHeader header;

		ObjID obj_id;			//����objid
		short reward_count;
		int add_coin;
		reward_info reward_list[MAX_TREASURE_CHEST_COUNT];
	};

	class SCNoticeCfgUpdate : public IMessageSerializer  // 2041 �����ȸ�֪ͨ
	{
	public:
		SCNoticeCfgUpdate();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(notice_id, buffer, buffer_size, out_length);
			result = result && WriteInt(is_show, buffer, buffer_size, out_length);
			result = result && WriteInt(title_len, buffer, buffer_size, out_length);
			result = result && WriteStrN(title, title_len, buffer, buffer_size, out_length);
			result = result && WriteInt(subject_len, buffer, buffer_size, out_length);
			result = result && WriteStrN(subject, subject_len, buffer, buffer_size, out_length);
			result = result && WriteInt(content_len, buffer, buffer_size, out_length);
			result = result && WriteStrN(content, content_len, buffer, buffer_size, out_length);

			return result;
		}
		
		int notice_id;			//!< ����ID 
		int is_show;			//!< �Ƿ���ʾ [0:����ʾ  1:��ʾ]

		int title_len;			//!< ���ⳤ��
		NoticeTitle title;		//!< ����

		int subject_len;		//!< ���ⳤ��
		NoticeSubject subject;	//!< ����
			
		int content_len;		//!< ���ݳ���
		NoticeContent content;	//!< ����
	};


	class CSRoleSubscribe		//!< 1057 ����/ȡ������ϵͳ��Ϣ
	{
	public:
		CSRoleSubscribe();
		MessageHeader header;

		short is_unsubscribe;	//!< 0: ����   1:ȡ������
		short topic;			//!< ���ĵ�����  \see TOPIC_TYPE
	};

	class CSRoleSalaryInfo		//!< 2012 ������ҹ�����Ϣ
	{
	public:
		CSRoleSalaryInfo();
		MessageHeader header;

		int salary_type;		//!< 0:�ӳ�����  1:����ֵ
	};

	class SCRoleSalaryInfo		//!< 2013 ��ҹ�����Ϣ����
	{
	public:
		SCRoleSalaryInfo();
		MessageHeader header;

		struct SalaryInfo 
		{
			short seq;
			short times;
			int total_salary;
		};

		enum REASON_TYPE
		{
			REASON_TYPE_NORMAL = 0,	//!< 0 �·������б�
			REASON_TYPE_UPDATE = 1, //!< 1 ����
		};

		char salary_type;			//!< 0:�ӳ�����  1:����ֵ
		char reason;				//!< �·�ԭ�� \see REASON_TYPE
		short salary_list_size;		//!< �б���
		long long total_value;		//!< ��ֵ
		SalaryInfo salary_list[MAX_SALARY_INFO_NUM];	//!< �б���Ϣ \see SalaryInfo
	};
	
	class CSSpeaker		//!< 2014 ����ʹ������
	{
	public:
		CSSpeaker();
		MessageHeader		header;

		enum SPEAKER_TYPE
		{
			SPEAKER_TYPE_LOCAL,
			SPEAKER_TYPE_CROSS,

			SSPEAKER_TYPE_MAX,
		};

		void ResetSpeakMsg()
		{
			memset(speaker_msg, 0, sizeof(speaker_msg));
		}

		char speaker_type;
		char reserve_char;
		short reserve_short;
		unsigned int		speaker_msg_length;
		char				speaker_msg[MAX_SPEAKER_MSG_LENGTH];
	};

	class SCSpeaker		//!< 2015  ��������·�
	{
	public:
		SCSpeaker();
		MessageHeader header;

		int	from_uid;
		GameName username;
		PlatName plat_name;
		int server_id;
		char speaker_type;
		char reserve_char;
		short reserve_short;

		unsigned int speaker_msg_length;
		char speaker_msg[MAX_SPEAKER_MSG_LENGTH];
	};

	class SCConvertShopBuyRecord	//!< 1851 �̳ǣ��һ��̵꣩�����¼���� 
	{
	public:
		SCConvertShopBuyRecord();
		MessageHeader header;

		struct RecordInfo
		{
			ItemID item_id;			//!< ��ƷID
			short convert_type;		//!< �һ�����
			short buy_times;		//!< �������
			short seq;				//!< ����Ψһ��Ӧ
		};

		int record_num;				//!< �����¼������ ����������record_list
		RecordInfo record_list[CONVERT_TYPE_MAX_ITEM_COUNT * CONVERT_TYPE_MAX];
	};

	class SCConvertShopAddRecord	//!< 1852 ����һ�������¼
	{
	public:
		SCConvertShopAddRecord();
		MessageHeader header;

		struct RecordInfo
		{
			ItemID item_id;			//!< ��ƷID
			short convert_type;		//!< �һ�����
			short buy_times;		//!< �������
			short seq;				//!< ����Ψһ��Ӧ
		};

		RecordInfo info;			//!< �����¼
	};

	class CSConvertShopBuyReq	//!< 1850 ����Э�� 
	{
	public:
		CSConvertShopBuyReq();
		MessageHeader header;

		short convert_type;		//!< �һ��̵�����
		short seq;				//!< ��Ʒ��seq
		ItemID item_id;			//!< ��ƷID���������飬��ֹ�ȸ���ͻ��˺ͷ���˿�������Ʒ��һ��������� 
		short buy_num;			//!< ��������
	};
	class SCRoleSalaryDay			//!< 2045 ÿ���õ�����ֵ�͹�����ֵ
	{
	public:
		SCRoleSalaryDay();
		MessageHeader header;

		int salary_type;			//!< 0:�ӳ�����  1:����ֵ
		int day_value;				//1< ÿ�������ֵ���� 
	};

	class SCRoleHistoryChivalrous	//!< 2066 ��ʷ����ֵ�·�
	{
	public:
		SCRoleHistoryChivalrous();
		MessageHeader header;

		int history_chivalrous;		//!< ��ʷ�ۼƻ�õ�����ֵ
		int chivalrous_reward_flag;	//!< ����ȡ������ǣ������ƣ�
	};

	class CSRoleHistoryChivalrousRewardFetch	//!< 2067 ��ʷ����ֵ������ȡ����
	{
	public:
		CSRoleHistoryChivalrousRewardFetch();
		MessageHeader header;

		int fetch_seq; // ��ȡ�ĳɾͽ���seq
	};
	
	class SCLoginSystemInfo			//!< 2046 �ۼƵ�½�·�
	{
	public:
		SCLoginSystemInfo();
		MessageHeader header;

		int login_day;				//!< �ۼƵ�½����
		BitMap<MAX_LOGIN_SYSTEM_NUM> is_reward_flag;	// �Ƿ�����ȡ����
	};

	class CSLoginSystemREQ			//!< 2047 �ۼƵ�½����
	{
	public:
		CSLoginSystemREQ();
		MessageHeader header;

		int day;					//!< ��ȡ�ĵ�½����
	};

	class SCSpecialNotice			//!< 2048 ������Ϣϵͳ��֪ͨ
	{
	public:
		SCSpecialNotice();
		MessageHeader header;

		enum SPECIAL_NOTICE_TYPE
		{	
			SPECIAL_NOTICE_TYPE_CAPTAIN_SALARY = 0,				// �ӳ�����   param1 Ϊ��ʱ�ӳ���������
			SPECIAL_NOTICE_TYPE_GUILD_GATHER_DREAM_GIFT = 1,	// ���弯Ը   param1 �����ʽ� param2 ͭ�� param3 ���幱�� param4 ����ID param5 ��������

			SPECIAL_NOTICE_TYPE_MAX
		};

		int op_type;
		int param1;
		int param2;
		int param3;
		int param4;
		int param5;
	};

	class CSMysteriousTrialReq         //!<2250 ����������Ϣ����
	{
	public:
		CSMysteriousTrialReq();
		MessageHeader header;

		enum Mysterious_trial_type
		{
			MYSTERIOUS_TRIAL_INFO_TYPE = 0,				//����������Ϣ�·�
			MYSTERIOUS_TRIAL_REWARD_TYPE,				//��ȡ������ͨ����
			MYSTERIOUS_TRIAL_FINALLY_REWATD_TYPE,		//��ȡ���ս���
		};

		int op_type;
		int param1;
		int param2;
	};

	class SCMysteriousTrialInfo          //!<2251 ����������Ϣ�·�(����)
	{
	public:
		SCMysteriousTrialInfo();
		MessageHeader header;

		char open_day;																						//�����
		char goal_num;																						//��������			
		short task_num;																						//����ɵ���������(��������������)	
		short type;																							//��ǰ�׶� MYSTERIOUS_TRIAL_TYPE	
		short reserve_sh;																					
		BitMap<RA_MYSTERIOUS_TRIAL_FINALLY_MAX_NUM> activity_flag;											//��������״̬��ʶ RA_MYSTERIOUS_TRIAL_FINALLY_MAX_NUM---32
		ActivityFlagSchedule  ra_mysterious_trial[RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM][RA_NEW_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM];	//ÿ�������б�    7---5
	};

	class SCMysteriousTrialSingleInfo          //!<2252 ��������������Ϣ�·�
	{
	public:
		SCMysteriousTrialSingleInfo();
		MessageHeader header;

		char start_time;					//�ڼ����Ŀ��,0Ϊ��������
		char seq;
		short state;						//״̬-��ʶ
		int progress;						//����
		short target_num;					//����ɵ���������
		short reserve_sh;					//������
	};

	struct SCMysterialFinallyRewardInfo		//!<2253 �����������ս�����Ϣ�·�
	{
	public:
		SCMysterialFinallyRewardInfo();
		MessageHeader header;

		BitMap<RA_MYSTERIOUS_TRIAL_FINALLY_MAX_NUM> activity_flag;		//��������״̬��ʶ RA_MYSTERIOUS_TRIAL_FINALLY_MAX_NUM---32
	};

	struct CSTransactionTaskReq                         //2255 ��������(ÿ������) - ˿��֮·����
	{
	public:
		CSTransactionTaskReq();
		MessageHeader header;

		enum Transaction_Task_Type
		{
			TRANSACTION_TASK_INFO_TYPE = 0,				//����������Ϣ
			TRANSACTION_TASK_COMMON_TYPE,				//�����ύ��ȡ��ͨ��Ʒ�� index  refer_num
			TRANSACTION_TASK_FINALLY_TYPE				//������ȡ�ܽ���
		};

		int op_type;					
		int index;										// 0-8
		int refer_num;									// �ύ�ĵ�������
	};

	struct SCTransactionTaskInfo						//2256 ��������(ÿ������) - ˿��֮·��Ϣ�·�
	{
	public:
		SCTransactionTaskInfo();
		MessageHeader header;

		int task_id;										//����ID
		short task_status;									//����״̬  0 :������   1:�����
		short version_reset;								//�汾��,��ͬ������
		short level;										//��ȡ����ʱ�ĵȼ�
		short type;											//һ��type����һ���ȼ��鷶Χ ������鿴����
		short seq[TRANSACTION_TASK_REWARD_MAX_NUM];			//���9����Ӧ������seq   �������Ӧ
		short reserve;										//������,����
		BitMap<TRANSACTION_TASK_REWARD_MAX_NUM> flag;		//��ȡ��־
	};

	struct CSActiveSystemReq							//2257 ��Ծϵͳ - ��Ϣ����
	{
	public:
		CSActiveSystemReq();
		MessageHeader header;

		enum Active_System_Op_Type
		{
			ACTIVE_SYSTEM_INFO_TYPE = 0,					//������Ϣ�·�  ����index
			ACTIVE_SYSTEM_FETCH_REWARD,						//������ȡ��Ծ�ȱ��佱��  ��Ҫ����index
			ACTIVE_SYSTEM_WEEK_FETCH_REWARD,				//�ܳ�- ��ȡ���� ����seq
			ACTIVE_SYSTEM_FETCH_SPECIAL_REWARD,				//��ȡ�����Ծ���� index ��activie_special_type
			ACTIVE_SYSTEM_SPECIAL_INFO,						//���������Ծ��Ϣ
		};

		int op_type;
		int index;										    // 0-3
	};

	struct SCActiveSystemInfo							//2258 ��Ծϵͳ - ��Ϣ�·�
	{
	public:
		enum EXTRA_REWARD_FLAG
		{
			EXTRA_REWARD_FLAG_CANNOT_FETCH = 0,
			EXTRA_REWARD_FLAG_FETCHABLE = 1,
			EXTRA_REWARD_FLAG_ALREADY_FETCH = 2,
		};

	public:
		SCActiveSystemInfo();
		MessageHeader header;

		int level;												//��ҵ����������ʱ�ĵȼ�
		int integral_active;									//��ҵ�ǰ�Ļ�Ծ��
		BitMap<ACTIVE_SYSTEM_LEVEL_GROUP_MAX_NUM> active_flag;	//��Ծ������ȡ��־
		int task_count;											// ��������
		int progress[ACTIVE_SYSTEM_TYPE_MAX];					//�������͵Ľ���
		int extra_reward_flag_list[ACTIVE_SYSTEM_TYPE_MAX];		//�������͵Ķ��⽱����ȡ�����0:������ȡ  1:����ȡ��δ��ȡ  2:����ȡ)
	};

	struct SCActiveSystemSingleInfo							//2264 ��Ծϵͳ - ������Ϣ�·�
	{
	public:
		SCActiveSystemSingleInfo();
		MessageHeader header;

		int level;												//��ҵ����������ʱ�ĵȼ�
		int integral_active;									//��ҵ�ǰ�Ļ�Ծ��
		BitMap<ACTIVE_SYSTEM_LEVEL_GROUP_MAX_NUM> active_flag;	//��Ծ������ȡ��־
		int active_id;											// ����
		int progress;											//����
		int extra_reward_flag;									//�������͵Ķ��⽱����ȡ�����0:������ȡ  1:����ȡ��δ��ȡ  2:����ȡ)
	};

	struct CSActiveSystemExtraRewardFetch		//!< 2265 ��ȡ��Ծ��Ŀ�Ķ��⽱��
	{
	public:
		CSActiveSystemExtraRewardFetch();
		MessageHeader header;

		int active_type;						//!< ��Ծ��Ŀ
	};

	struct SCSpecialActiveInfo					//!< 2266 �����Ծ��Ϣ�·�
	{
	public:
		SCSpecialActiveInfo();
		MessageHeader header;

		BitMap<ACTIVE_SYSTEM_SPECIAL_TYPE_MAX_IN_DB> special_active_reward_flag;	//�����Ծ���ͽ�����ȡ��ʶ
	};

	//// 3910 �ܳ� �����Ϣ
	struct SCWeekActiveDegreeInfo
	{
		SCWeekActiveDegreeInfo();
		MessageHeader header;

		BitMap<BIT_COUNT_OF_LONG_64> week_fetch_flag;	//��-��Ծ������ȡ���[����index]
		BitMap<BIT_COUNT_OF_LONG_64> week_task_today_finish_flag;//��-ÿ�������ɱ��[����index]
		int week_active_degree;							//�ܻ�Ծ��
	};


	struct CSOneKeyFinish									// 2259 ����һ�����
	{
		CSOneKeyFinish();
		MessageHeader header;

		enum FinishType
		{
			FINISH_TYPE_RESEARCH_TASK		= 0,					//!< �����ħ
			FINISH_TYPE_WST_QUESTION		= 1,					//!< �����ӿ���
			FINISH_TYPE_BOUNTY_TASK			= 2,					//!< ��������
			FINISH_TYPE_FANTASY_FB			= 3,					//!< ����þ�
			FINISH_TYPE_SILK_ROAD			= 4,					//!< ˿��֮·
			FINISH_TYPE_JIBUDAOZEI			= 5,					//!< ��������
		};

		int op_type;										// һ����ɵ���������
	};

	/*class SCActiveSystemInfo
	{
	public:
		SCActiveSystemInfo();
		MessageHeader header;

		unsigned int time;
		unsigned char day;
		unsigned char hour;
		unsigned char min;
		unsigned char sec;
	}*/;

	class SCFuncPreviewInfo		// ����Ԥ����Ϣ	1870
	{
	public:
		SCFuncPreviewInfo();
		MessageHeader header;

		int notice_num;								//!< Ԥ������ ����ʵ��������ȡ������б�
		short notice_id_list[MAX_FUNC_PREVIEW_NUM];	//!< ��ȡ״̬�б��±��ʾԤ��ID��ֵ��ʾ��ȡ״̬��ȡֵ  0��������ȡ  1������ȡ  2������ȡ
	};

	class CSFuncPreviewClaimReward	//!< ��ȡԤ�潱�� 1871
	{
	public:
		CSFuncPreviewClaimReward();
		MessageHeader header;

		int notice_id;		//!< Ԥ��ID
	};

	class CSSetAutoCatchPet				//!< 2175 �Զ�ץ������
	{
	public:
		CSSetAutoCatchPet();
		MessageHeader header;

		struct AutoCatchSetting
		{
			int monster_id;				//!< ����ID
			int max_catch_num;			//!< ���ץ���� 
		};

		int scene_id;					//!< Ŀ�곡��ID
		short max_seal_card_common;		//!< ����տ�ʹ����
		short max_seal_card_silver;		//!< �������ʹ����
		short max_seal_card_gold;		//!< ����ʹ����
		short auto_catch_info_num;		//!< �Զ�ץ����Ϣ��
		
		AutoCatchSetting setting_list[MAX_AUTO_CATCH_NUM]; //!< �Զ�ץ����Ϣ
	};

	class SCAutoCatchPetInfo			//!< 2176 �Զ�ץ����Ϣ
	{
	public:
		SCAutoCatchPetInfo();
		MessageHeader header;

		struct AutoCatchInfo
		{
			int monster_id;				//!< ����ID
			short max_catch_num;		//!< ���ץ����
			short cur_catch_num;		//!< ��ǰץ����
		};

		int scene_id;					//!< ����ID
		short max_seal_card_common;		//!< ����տ�ʹ����
		short max_seal_card_silver;		//!< �������ʹ����
		short max_seal_card_gold;		//!< ����ʹ����

		short cur_seal_card_common;		//!< ��ǰ�տ�ʹ����
		short cur_seal_card_silver;		//!< ��ǰ����ʹ����
		short cur_seal_card_gold;		//!< ��ǰ��ʹ����

		int auto_catch_info_num;		//!< �Զ�ץ����Ϣ��
		AutoCatchInfo info_list[MAX_AUTO_CATCH_NUM]; //!< �Զ�ץ����Ϣ
	};

	class SCRoleGiftGatherDreamList		//!< 2177 ��ҽ��켯Ը������Ϣ
	{
	public:
		SCRoleGiftGatherDreamList();
		MessageHeader header;

		short today_is_publish_gather_dream_flag;			//�����Ƿ��ѷ������弯Ը	0:δ���� 1:�ѷ���
		short count;
		int gift_uild_list[MAX_GATHER_DREAM_GIFT_NUM];		//���켯Ը�����uid�б�  MAX_GATHER_DREAM_GIFT_NUM -- 10
	};

	class SCRoleCanPublishGahterDreamItemList	//!< 2178 ��ҿɷ����ļ�Ը��Ϣ�б�
	{
	public:
		SCRoleCanPublishGahterDreamItemList();
		MessageHeader header;

		short publish_is_send_flag;				//�������弯Ը�Ƿ�����Ϣ������Ƶ�� 0 :Ĭ�Ϲ�ѡ����  1:ȡ����ѡ,������
		short count;
		int pet_id_list[PET_COUNT_MAX];			//�ɷ����ĳ���ID�б� PET_COUNT_MAX -- 256
	};

	class SCRoleGatherDreamRecordInfoList			//!< 2179 ��Ҽ�Ը��¼�б�
	{
	public:
		SCRoleGatherDreamRecordInfoList();
		MessageHeader header;

		short type;
		short count;
		GatherDreamRecordInfo data_list[MAX_GATHER_DREAM_SEND_LIST_NUM];	//��¼�б�	MAX_GATHER_DREAM_SEND_LIST_NUM -- 30
	};


	struct XunBaoComsumeInfo
	{
		short col;
		short idx;
		int num;
	};

	class CSXunBaoReq		//!< 2500 Ѱ������
	{
	public:
		CSXunBaoReq();
		MessageHeader header;

		short guide_draw;			//�Ƿ�����ֳ���
		short draw_type;
		short draw_times;
		short is_use_free;
		XunBaoComsumeInfo comsume_info[5]; //���ò���
	};

	class SCXunBaoRet		//!< 2501 Ѱ�����
	{
	public:
		SCXunBaoRet();
		MessageHeader header;

		struct NodeItem
		{
			short type;			//0����Ʒ��1�ǳ���, 2�ǻ��
			unsigned short id;
			int num;
		};

		int count;
		NodeItem item_list[50];
	};


	class SCXunBaoOpenInfo	//!< 2502 Ѱ������
	{
	public:
		SCXunBaoOpenInfo();
		MessageHeader header;

		struct XunBaoOpenInfo
		{
			XunBaoOpenInfo()
			{
				draw_type = 0;
				is_open = 0;
				open_day = 0;
				open_type = 0;
				start_time = 0;
				end_time = 0;
				week_open_flag = 0;
				day_begin_time = 0;
				day_end_time = 0;
			}


			short draw_type;
			short is_open;
			int open_day;  //open_day��Ϊ0����ǰ����ʱ��С��������ֵ���ܳ�

			int open_type;
			//-------open_typeΪ1ʱ��
			unsigned int start_time;	//��ǰʱ�����ʱ������ܳ�
			unsigned int end_time;	//��ǰʱ�����ʱ������ܳ�
			//-------open_typeΪ2ʱ������ʱ�����һ������X��X��-X�㿪����
			unsigned int week_open_flag;			//�ܼ�����
			unsigned short day_begin_time;			//�ܼ�����ÿ��X�㿪��
			unsigned short day_end_time;				//�ܼ�����ÿ��x�����
		};

		int count;
		XunBaoOpenInfo open_info[100];
	};

	class SCXunBaoDrawTimesInfo	//!< 2503 Ѱ���齱������
	{
	public:
		SCXunBaoDrawTimesInfo();
		MessageHeader header;

		int draw_times[XUN_BAO_DRAW_CUMULATIVE_TIMES_GROUP_MAX_NUM];  //!< ��С10			�齱����	
		unsigned long long first_draw_flag;		//!< Ѱ��id �Ƿ��ȡ��һ���׳齱����־
		unsigned long long second_draw_flag;		//!< Ѱ��id �Ƿ��ȡ�ڶ����׳齱����־
		unsigned int free_draw_time[20];		//!< �´���ѳ齱ʱ��
		int xi_you_floor_curr_times[20];		//!< ϡ�б��׳齱����
	};

	struct XunBaoExchangeComsumeItem
	{
		XunBaoExchangeComsumeItem()
		{
			comsume_type = 0;
			type = 0;
			index = 0;
		}

		bool operator<(const XunBaoExchangeComsumeItem & other) const
		{
			if (comsume_type < other.comsume_type)
				return true;
			if (comsume_type > other.comsume_type)
				return false;
			if (type < other.type)
				return true;
			if (type > other.type)
				return false;
			if (index < other.index)
				return true;
			if (index > other.index)
				return false;

			return false;
		}

		unsigned short comsume_type; //0�ǲ�����1��1�ǲ�����2
		unsigned char type;			//���ĵı������ǲֿ� 0�Ǳ�����1�ǲֿ�
		unsigned char index;			//����index
	};

	class CSXunBaoExchange		//!< 2504 Ѱ���һ�
	{
	public:
		CSXunBaoExchange();
		MessageHeader header;


		unsigned short draw_type;		
		unsigned short get_index;
		unsigned short comsume_index1;		//������1�ڼ���
		unsigned short comsume_index2;		//������2�ڼ���
		XunBaoExchangeComsumeItem comsume_item[100];// ���ò���100��
	};

	class SCNoticeClientReloadSensitiveWords
	{
	public:
		SCNoticeClientReloadSensitiveWords();
		MessageHeader header;
	};

	class SCNoticeClientNewMessageFromCustomService		// 1005
	{
	public:
		SCNoticeClientNewMessageFromCustomService();
		MessageHeader header;

		int msg_count;	
	};

	class SCCmdRoute				// 1006
	{
	public:
		SCCmdRoute();
		MessageHeader header;

		int route_content_length;
		char route_content[2000];
	};

	class CSCatchFoxSpecialLogicReq	// 3403
	{
	public:
		CSCatchFoxSpecialLogicReq();
		MessageHeader header;

		int x;
		int y;
		int dir;
	};

	class SCHearsayInfo				//!< 2059 ������ʾ��Ϣ�·�
	{
	public:
		SCHearsayInfo();
		MessageHeader header;

		enum HEARSAY_TYPE
		{
			HEARSAY_TYPE_ACTIVE_FACE_SUCC = 0,			//������ֵ��Դ�ɹ�	param1:item_id
			HEARSAY_TYPE_ACTIVE_FACE_FAILD = 1,			//������ֵ��Դʧ��	param2:item_id
		};

		int op_type;
		int param1;
	};


	class CSXunBaoBelssReq			//!<2505 Ѱ��������
	{
	public:
		CSXunBaoBelssReq();
		MessageHeader header;

		short index;
		short reserve_sh;
		int pet_id;
	};

	class SCXunBaoBelssInfo			//!<2506 Ѱ������Ϣ�·�
	{
	public:
		SCXunBaoBelssInfo();
		MessageHeader header;

		int xun_bao_bless_pet_id_list[MAX_XUN_BAO_BLESS_PET_NUM];
	};

	class CSLunHuiWangChuanReq			//!<2507 �ֻ���������
	{
	public:
		CSLunHuiWangChuanReq();
		MessageHeader header;
		enum CS_LHWC_TYPE
		{
			CS_LHWC_TYPE_INFO = 0,		//��Ϣ�·�
			CS_LHWC_TYPE_BATTLE = 1,	//����ս��
			CS_LHWC_TYPE_REWARD = 2,	//��ȡ�������� param ��Ӧ����
		};
		int req_type;
		int param;
	};

	class SCLunHuiWangChuanInfo			//!<2508 �ֻ������·�
	{
	public:
		SCLunHuiWangChuanInfo();
		MessageHeader header;

		int start_level;		//����ȼ���¼
		int battle_monster_id;	//	 ս��������
		unsigned int previous_refresh_tamp;//��һ��ˢ��ʱ��
		BitMap<MAX_LUN_HUI_WANG_CHUAN_INDEX> pass_list;//MAX_LUN_HUI_WANG_CHUAN_INDEX = 24
		BitMap<MAX_LUN_HUI_WANG_CHUAN_INDEX> reward_list;//MAX_LUN_HUI_WANG_CHUAN_INDEX = 24
	};

	class SCNetherWorldPurgatoryInfo			//!<2630 ��ڤ�����·�
	{
	public:
		SCNetherWorldPurgatoryInfo();
		MessageHeader header;

		int pass_challenge_id;
		int pass_challenge_chapters;//ͨ���½�
		int low_key;
		int mid_key;
		int high_key;
		int show_video;
		int today_pass_times;
	};

	class SCNetherWorldPurgatoryChapterInfo			//!<2634 ��ڤ�������½��·�
	{
	public:
		SCNetherWorldPurgatoryChapterInfo();
		MessageHeader header;

		int chapter_id;
		NetherWorldGatoryChapters chapter_list;//�½�
	};

	class CSNetherWorldPurgatoryReq			//!<2631 ������ڤ����
	{
	public:
		CSNetherWorldPurgatoryReq();
		MessageHeader header;
		enum REQ_OP_TYPE
		{
			REQ_OP_TYPE_0 = 0,	//��Ϣ�·�
			REQ_OP_TYPE_1 = 1,	//����ս�� param1 ��Ӧ�½�id param2 ��Ӧ�ؿ�id
			REQ_OP_TYPE_2 = 2,	//��ȡ�������� param1 ��Ӧ�½�id param2 ��Ӧ�ؿ�id param3 ��Ӧ����id
			REQ_OP_TYPE_3 = 3,	//ʹ������齱 param1 ��Ӧ�½�id param2 ��������[1-3] param3 �齱����
			REQ_OP_TYPE_4 = 4,	//�ͻ��������Ƿ��Ѿ����Ź���
		};
		int req_type;
		int param1;
		int param2;
		int param3;
	};

	class SCNetherWorldPurgatorySigleInfo			//!<2632 ��ڤ���������·�
	{
	public:
		SCNetherWorldPurgatorySigleInfo();
		MessageHeader header;

		enum SEND_REASON
		{
			SEND_REASON_GET_REWARD = 0,//�콱
			SEND_REASON_FINISH_BATTLE = 1,//ս��
		};

		short send_reason;//�·�����
		char reserve_ch;
		unsigned char new_pass_flag;//�������������
		int pass_challenge_id;//ͨ�عؿ�
		int pass_challenge_chapters;//ͨ���½�
		int today_play_times;
		NetherWorldGatoryLevel challenge_info;
	};

	struct NetherWorldGatoryChapterMiLing
	{
		BitMap<MAX_NETHER_WORLD_GATORY_INDEX> silver_reward_flag;
		BitMap<MAX_NETHER_WORLD_GATORY_INDEX> gold_reward_flag;
	};

	class SCNetherWorldPurgatoryMiLingInfo			//!<2633 ��ڤ���������·�
	{
	public:
		SCNetherWorldPurgatoryMiLingInfo();
		MessageHeader header;

		int low_key;
		int mid_key;
		int high_key;
		
		NetherWorldGatoryChapterMiLing mi_ling_list[MAX_NETHER_WORLD_GATORY_CHAPTER_NUM];
	};

	class CSRealmBreakReq			//!< 1880 ���󾳽�ϵͳ��ز���
	{
	public:
		CSRealmBreakReq();
		MessageHeader header;

		enum REALM_BREAK_OP_TYPE
		{
			REALM_BREAK_OP_TYPE_BREAK = 0,			// ����ͻ��
			REALM_BREAK_OP_TYPE_TUNA = 1,			// ��������
			REALM_BREAK_OP_TYPE_FETCH_EXP = 2,		// ������ȡ��ɫ����
			REALM_BREAK_OP_TYPE_FIGHT = 3,			// ������ս����֮��

			REAL_BREAK_OP_TYPE_MAX
		};

		short notice_type;
		short reserve_sh;							// typeΪ3ʱ��seq
	};

	class SCRealmBreakAllInfo		//!< 1881 ����ϵͳȫ����Ϣ�·�
	{
	public:
		SCRealmBreakAllInfo();
		MessageHeader header;

		short cur_seq;										//��ǰ����
		short tuna_num;										//���ɴ���
		unsigned int cur_break_exp;							//��ǰ����ֵ
		unsigned int cumulation_user_exp_count;				//����ۻ��Ĵ���ȡ����ֵ
		unsigned int user_cumulation_pratice_time;			//����ۻ�������ʱ��
		unsigned int cumulation_user_coin_count;			//����ۻ��Ĵ���ȡͭ��
		short cur_fight_seq;								//����֮��ս������
		short reserver_sh;
	};

	class SCRealmBreakItemInfo		//!< 1883 ����ϵͳ������Ϣ�·�
	{
	public:
		SCRealmBreakItemInfo();
		MessageHeader header;

		int item_count;
		RealmBreakItem cumulation_item_list[MAX_REALM_BREAK_ITEM_NUM];	//����ۻ��Ĵ���ȡ�����б�
	};

	/*class SCRealBreakStartTuNa		//!< 1882 ����ϵͳ��ʼ�����·�	�ͻ��˲��������ʱ��ص�,Ŀǰʹ������ϴ����ɽ���ʱ���������
	{
	public:
		SCRealBreakStartTuNa();
		MessageHeader header;

		unsigned int need_time;			//!< ��������ʱ��(��)
	};*/

	class CSMapExplorationReq		//!< 1890 ��ͼ̽������
	{
	public:
		CSMapExplorationReq();
		MessageHeader header;
		enum MAP_EXPLORATION_TYPE
		{
			MAP_EXPLORATION_TYPE_INFO = 0,//�����·�
			MAP_EXPLORATION_TYPE_REQ = 1,//����̽�� param ���Ӧseq
		};
		int req_type;
		int param;
	};

	class SCMapExplorationInfo		//!< 1891 ��ͼ̽���·�
	{
	public:
		SCMapExplorationInfo();
		MessageHeader header;

		BitMap<MAX_MAP_EXPLORATION_NUM> m_map;
	};

	class SCSalaryBattleStatInfo		//!< 2920 ս��ǰͳ�Ʊ���ս������ֵ��Ϣ
	{
	public:
		SCSalaryBattleStatInfo();
		MessageHeader header;

		int seq;
		int salary;
	};

	enum SET_AUTO_ADD_POINT_FLAG
	{
		SET_AUTO_ADD_POINT_FLAG_ROLE,		//��ɫ ���� index��param�ӵ�����
		SET_AUTO_ADD_POINT_FLAG_PET,			//���� p0 pet_index , p1 flag
	};

	class CSSetAutoAddPointFlag //!< 3880 �����Զ��ӵ���Ϣ
	{
	public:
		CSSetAutoAddPointFlag();
		MessageHeader header;

		int req_type;
		short index;
		short param[5];
	};

	class CSChivalrousRankListReq //!< 10062 ����ֵ���а�����
	{
	public:
		CSChivalrousRankListReq();
		MessageHeader header;

		int rank_type;
	};

	class SCChivalrousRankList //!< 10063 ����ֵ���а��·�
	{
	public:
		SCChivalrousRankList();
		MessageHeader header;

		short rank_type;
		short count;
		ChivalrousRankInfo rank_list[CHIVALROUS_RANK_LIST_NUM]; // �������100
	};

	class SCWorldLevelInfo		//!< 1126 ����ȼ���Ϣ�·�
	{
	public:
		SCWorldLevelInfo();
		MessageHeader header;

		int open_day;							// ��������
		unsigned int today_zero_timestamp;		// ����0��ʱ���
	};

	class SCCustomInstructionInfo		//!< 3920 �Զ���ָ���·�
	{
	public:
		SCCustomInstructionInfo();
		MessageHeader header;

		int instruction_num;
		InstructionStr instruction_list[CUSTOM_INSTRUCTION_NUM];	// ���8��������instruction_num����
	};

	class CSCustomInstructionAdd		//!< 3921 �Զ���ָ������
	{
	public:
		CSCustomInstructionAdd();
		MessageHeader header;

		InstructionStr instruction_str;	//!< ָ������
	};

	class CSCustomInstructionRemove		//!< 3922 ɾ���Զ���ָ��
	{
	public:
		CSCustomInstructionRemove();
		MessageHeader header;

		int index;						//!< ָ��index
	};

	class CSCustomInstructionBatchRemove	//!< 3923 ����ɾ���Զ�������
	{
	public:
		CSCustomInstructionBatchRemove();
		MessageHeader header;

		int num;
		short index_list[CUSTOM_INSTRUCTION_NUM];
	};

	class CSBigDipperReq	//!< 3950 ������������
	{
	public:
		CSBigDipperReq();
		MessageHeader header;
		enum REQ_TYPE
		{
			REQ_TYPE_0 = 0,//�����·���Ϣ
			REQ_TYPE_1 = 1,//����ʼս�� param1 ��Ӧ�Ǿ�sn
			REQ_TYPE_2 = 2,//������ȡ��ͨ���� param1 ��Ӧ�Ǿ�sn
			REQ_TYPE_3 = 3,//������ȡͨ�ؽ׶ν��� param1 ��Ӧ�Ǿ�sn  param2 �׶��±�[0-2]
		};
		int type;
		int param1;
		int param2;
	};

	class SCBigDipperInfo	//!< 3951 ���������·�
	{
	public:
		SCBigDipperInfo();
		MessageHeader header;

		int monster_num;
		int open_monster_list[MAX_BIG_DIPPER_MONSTER_NUM];
	};

	class SCBigDipperMyInfo	//!< 3952 ��������-�ҵ��Ǿ�ͨ����Ϣ
	{
	public:
		SCBigDipperMyInfo();
		MessageHeader header;
		BigDipperMonster monster_list[MAX_BIG_DIPPER_MONSTER_NUM];//�ҵ������Ǿ���Ϣ
		char help_list[MAX_BIG_DIPPER_MONSTER_NUM];
	};

	class SCFallenGodInfo	//!< 3953 ����֮���ҵ�ͨ����Ϣ
	{
	public:
		SCFallenGodInfo();
		MessageHeader header;
		BigDipperMonster monster_list[MAX_FALLEN_GOD_MONSTER_NUM];//�ҵ�����boss��Ϣ
		char help_list[MAX_FALLEN_GOD_MONSTER_NUM];
	};

	class CSFallenGodReq	//!< 3954 ����֮������
	{
	public:
		CSFallenGodReq();
		MessageHeader header;
		enum REQ_TYPE
		{
			REQ_TYPE_0 = 0,//�����·���Ϣ
			REQ_TYPE_1 = 1,//����ʼս�� param1 ��Ӧsn
		};
		int type;
		int param1;
		int param2;
	};

	class SCSvipClientCustomData		//!< 2098 �·�SVip�ͻ����Զ�������
	{
	public:
		SCSvipClientCustomData();
		MessageHeader header;

		int data_1;
		int data_2;
	};

	class CSSvipClientCustomData		//!< 2099 SVIP�ͻ����Զ�����������
	{
	public:
		CSSvipClientCustomData();
		MessageHeader header;

		int oper_type;		//!< [0:��������  1:�ϴ�����]
		int param1;
		int param2;
	};

	class CSRoleAttrPelletReq		//!< 3960 ���Ե���������
	{
	public:
		CSRoleAttrPelletReq();
		MessageHeader header;

		enum ROLE_ATTR_PELLET_REQ_TYPE
		{
			ROLE_ATTR_PELLET_REQ_TYPE_ALL_INFO = 0,			// �����·���ɫ�������Ե�ʹ����Ϣ
			ROLE_ATTR_PELLET_REQ_TYPE_USE_ITEM = 1,			// ����ʹ�����Ե�(��ɫ) P1:pellet_id P2:num 
			ROLE_ATTR_PELLET_REQ_TYPE_USE_ITEM_PET = 2,		// ����ʹ�����Ե�(����) P1:pellet_id P2:num P3:pet_index
			ROLE_ATTR_PELLET_REQ_TYPE_USE_ITEM_PARNTER = 3, // ����ʹ�����Ե�(���) P1:pellet_id P2:num P3:partner_id
		};

		short op_type;
		short param1;
		int param2;
		int param3;
	};

	class SCRoleAttrPelletAllInfo : public IMessageSerializer	//!< 3961 ���Ե�������Ϣ�·�
	{
	public:
		SCRoleAttrPelletAllInfo();
		virtual ~SCRoleAttrPelletAllInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			int count = GetMin((int)ATTR_PELLET_TYPE_MAX, MAX_ATTR_PELLET_TYPE_NUM);
			result = result && WriteInt(count, buffer, buffer_size, out_length);
			for (int i = 0; i < count; ++i)
			{
				result = result && WriteShort(role_use_attr_list[i], buffer, buffer_size, out_length);
			}
			result = result && WriteInt(PARTNER_NUM_MAX, buffer, buffer_size, out_length);
			for (int i = 0; i < PARTNER_NUM_MAX; ++i)
			{
				for (int k = 0; k < count; k++)
				{
					result = result && WriteShort(partner_use_attr_list[i][k], buffer, buffer_size, out_length);
				}
			}
			return result;
		}

		short role_use_attr_list[MAX_ATTR_PELLET_TYPE_NUM];
		short partner_use_attr_list[PARTNER_NUM_MAX][MAX_ATTR_PELLET_TYPE_NUM];
	};

	class SCRoleAttrPelletSingleInfo	//!< 3962 ���Ե�������Ϣ�·�
	{
	public:
		SCRoleAttrPelletSingleInfo();
		MessageHeader header;

		short role_attr_pellet_type;			//ROLE_ATTR_PELLET_TYPE 
		short use_count;						//ʹ������
		int attr_pellet_type;					//ATTR_PELLET_TYPE_MAX
		int param1;								//����ROLE_ATTR_PELLET_TYPE 1:pet_index 2:partner_id
	};



	struct CSActiveTurntableReq				//!< 3965 ��Ծת������
	{
	public:
		enum REQ_TYPE
		{
			REQ_TYPE_INFO,				// ����������Ϣ
			REQ_TYPE_LUCK,				// ��ȡת�̽���
		};

		CSActiveTurntableReq();
		MessageHeader header;

		int type;
		int param;
	};

	struct SCActiveTurntableInfo			//!< 3966 ��Ծת��������Ϣ
	{
	public:
		SCActiveTurntableInfo();
		MessageHeader header;

		int turntable_time;												// ��Ծת�̴���
		BitMap<BIT_COUNT_OF_LONG_64> turntable_day_task_flag;			// ��Ծת�� ÿ��������ɱ��
		BitMap<BIT_COUNT_OF_LONG_64> turntable_week_task_flag;			// ��Ծת�� ÿ��������ɱ��
	};

	struct SCActiveTurntableRewardInfo		//!< 3967 ��Ծת�̳齱����
	{
	public:
		SCActiveTurntableRewardInfo();
		MessageHeader header;

		static const int MAX_NOTICE_ITEM_LIST_NUM = 120;

		struct NoticeItemInfo
		{
			NoticeItemInfo() : item_id(0), is_bind(0), num(0)
			{}

			ItemID item_id;
			short is_bind;
			int num;
		};

		int seq;
		int turntable_time;
		int count;
		NoticeItemInfo item_list[MAX_NOTICE_ITEM_LIST_NUM];
	};

	enum CAPACITY_TARGET_REQ_TYPE
	{
		CAPACITY_TARGET_REQ_TYPE_START,		//��ʼ��ս
		CAPACITY_TARGET_REQ_TYPE_FETCH,		//��ȡ����
	};

	struct CSCapacityTargetReq		//!< 3975 ϵͳ��ս ս���Ƚ�
	{
	public:
		CSCapacityTargetReq();
		MessageHeader header;

		int req_type;
	};


	struct SCCapacityTargetInfo		//!< 3976 ϵͳ��ս 
	{
	public:
		SCCapacityTargetInfo();
		MessageHeader header;


		GameName role_name;
		int  advance_times;
		int  avatar_type;
		int  headshot_id;
		int  color;
		int  prof;
		int  weapon_id;
		int  sub_weapon_id;

		unsigned int  capacity_target_end_time;			//����ʱ��
		char  capacity_target_can_start;			//�����������Ϊ1
		char  capacity_target_is_finish;			//��ʱ������Ϊ1
		short  capacity_target_is_fetch_reward;				//�Ƿ���ȡ�˽���
		int capacity_target_value;				//ʱ�䵽���ս��
		int capacity_target_robot_value;				//ʱ�䵽��Ķ���ս��
		unsigned int  capacity_target_begin_time;			//����ʱ��
	};

	struct SCEndlessTowerInfo//4120    �޾�֮���·�
	{
	public:
		SCEndlessTowerInfo();
		MessageHeader header;

		int battle_seq;//��ǰͨ��seq
	};

	struct CSEndlessTowerReq
	{
	public:
		CSEndlessTowerReq();// 4121 �޾�֮������
		MessageHeader header;
		enum REQ_TYPE
		{
			REQ_TYPE_INFO = 0,//�·���Ϣ
			REQ_TYPE_BATTLE = 1,//����ս��
		};
		int req_type;
		int param1;
		int param2;
	};

	struct SCFlyUpSystemInfo
	{
	public:
		SCFlyUpSystemInfo();// 4130 ����
		MessageHeader header;
		
		int task_idx;//��N������/������ɴ���
		int fly_skill_level;//�������ܵȼ�
		int fly_up_flag;//�����׶�
		int is_shilian;//�Ƿ�������
		int element[FlyUpSystemParam::FLY_UP_ELEMENT_MAX];//Ԫ��
		int organ[FlyUpSystemParam::FLY_UP_ORGAN_MAX];//���
		int is_one_click_finish;	// �Ƿ���һ�����
	};

	struct CSFlyUpSystemReq
	{
	public:
		CSFlyUpSystemReq();// 4131 ��������
		MessageHeader header;
		enum REQ_TYPE
		{
			REQ_TYPE_INFO = 0,//�����·�
			REQ_TYPE_ADV = 1,//����
			REQ_TYPE_FINISH = 2,//��ɷ���
			REQ_TYPE_SHILIAN = 3,//��ʼ����
			REQ_TYPE_UP_SKILL = 4,//������������
			REQ_TYPE_RAND_TASK = 5,//��ȡ��������
			REQ_TYPE_ADV_SAVE = 6,//���ױ���
			REQ_TYPE_ONE_KEY_UP_SKILL = 7, //һ����������
		};
		int type;
		int param1;
		int param2;
	};

	struct CSMirageTerritoryReq
	{
	public:
		CSMirageTerritoryReq();//4132 ����
		MessageHeader header;
		enum REQ_TYPE
		{
			REQ_TYPE_INFO = 0,			//�����·� param[���� 0 - 3]
			REQ_TYPE_BATTLE = 1,		//ս�� param1[���� 0 - 3]
			REQ_TYPE_ONE_KEY_PASS = 2,	//��ǲ param1[���� 0 - 3] param2 ��ǲ�Ļ��id param3 ��ǲ���id param4 ��ǲ���id
			REQ_TYPE_BATTLE_ZHEN_INFO = 3,//�·�������Ϣ param1[���� 0 - 3]
		};
		int type;
		int param1;
		int param2;
		int param3;
		int param4;
	};

	struct SCMirageTerritoryInfo
	{
	public:
		SCMirageTerritoryInfo();// 4133 �·�
		MessageHeader header;
		
		int today_pass_times;//������ǲ����
		int pass_area;//������ǲ����
		int pass_seq;//������ǲseq
		int area;
		int top_seq;//���ͨ�ؼ�¼
		int now_seq;//��ǰͨ�ؼ�¼
	};

	struct CSMirageTerritorySetPartnerReq
	{
	public:
		CSMirageTerritorySetPartnerReq();// 4134 ���������׾���鲼��
		MessageHeader header;

		int programme_idx;//����id
		int partner_id_list[MAX_MIRAGE_TERRITORY_PARTNER_NUM];
		int partner_posi_list[MAX_MIRAGE_TERRITORY_PARTNER_NUM];
	};

	struct SCMirageTerritorySetPartnerInfo
	{
	public:
		SCMirageTerritorySetPartnerInfo();// 4135 �·������׾���鲼��
		MessageHeader header;

		int programme_idx;//����id
		int partner_id_list[MAX_MIRAGE_TERRITORY_PARTNER_NUM];
		int partner_posi_list[MAX_MIRAGE_TERRITORY_PARTNER_NUM];
	};

	struct SCFlyUpSystemAdvInfo
	{
	public:
		SCFlyUpSystemAdvInfo();// 4136 ����ǿ��
		MessageHeader header;

		int fly_up_flag;//�����׶�
		int fly_adv_idx;//Ԫ��idx
		int value;//����ֵ
	};

	struct SCFlyUpSkillSigleInfo
	{
	public:
		SCFlyUpSkillSigleInfo();// 4137 ��������
		MessageHeader header;

		int skill_level;//��ǰ�ȼ�
		int skill_exp;//��ǰ����
	};

	class CSAuditBehaviorSubmitReq		//!< 1132 �ͻ�����Ϊ�������
	{
	public:
		CSAuditBehaviorSubmitReq();
		 MessageHeader header;

		int behavior_type;					//CLIENT_BEHAVIOR_TYPE_MAX
	};

	class SCYaoXingShiLianInfo		//!< 1133 ���������·�
	{
	public:
		SCYaoXingShiLianInfo();
		MessageHeader header;

		int pass_battle_seq;
		BitMap<MAX_YAOXINGSHILIAN_REWARD_LIST> m_reward_bit_map;
		BitMap<MAX_YAOXINGSHILIAN_REWARD2_LIST> m_reward_bit_map2;
		BitMap<32> m_cmd_map;
	};

	class CSYaoXingShiLianReq		//!< 1134 ������������
	{
	public:
		CSYaoXingShiLianReq();
		MessageHeader header;
		enum REQ_TYPE
		{
			REQ_TYPE_INFO = 0,//�·�
			REQ_TYPE_BATTLE = 1,//ս��
			REQ_TYPE_REWARD = 2,//�ɾͽ��� param ��ȡseq
			REQ_TYPE_REWARD2 = 3,//�ɾͽ���2 param ��ȡseq
		};
		int type;
		int param;
	};

	class CSSanXianGuiDongReq		//!< 1135 ���ɹ鶴����
	{
	public:
		CSSanXianGuiDongReq();
		MessageHeader header;
		enum SAN_XIAN_REQ
		{
			SAN_XIAN_INFO = 0,//��Ϣ
			SAN_XIAN_START = 1,//��ʼ
			SAN_XIAN_CHOOSE = 2,//ѡ�� param �Ƿ���ȷ��1��ȷ/2����
			SAN_XIAN_FB = 3,//���븱��
		};
		int type;
		int param;
	};

	class SCSanXianGuiDongInfo		//!< 1136 ���ɹ鶴�·�
	{
	public:
		SCSanXianGuiDongInfo();
		MessageHeader header;
		
		int today_play_times;//������Ϸ����
		int pass_idx;//��ǰͨ�عؿ�
	};

	class SCSanXianGuiDongPlayEndInfo		//!< 1137 ���ɹ鶴��Ϸ�·�
	{
	public:
		SCSanXianGuiDongPlayEndInfo();
		MessageHeader header;
		enum GAME_TYPE
		{
			GAME_TYPE_PLAYING = 0,//��Ϸ��
			GAME_TYPE_LOSE = 1,//ʧ��
			GAME_TYPE_WIN = 2,//ʤ��
		};
		int game_type;
		int pass_idx;//��ǰͨ�عؿ�
		unsigned int choose_tamp;//ѡ������ʱ��
		int sanxian_title_flag;//�ƺű��
	};

	class CSTaoQuanNaJiReq			//!< 1139 ��Ȧ�ɼ�����
	{
	public:
		CSTaoQuanNaJiReq();
		MessageHeader header;
		enum NA_JI_REQ
		{
			NA_JI_REQ_INFO = 0,//��Ϣ
			NA_JI_REQ_START = 1,//��ʼ
			NA_JI_REQ_CHOOSE = 2,//��Ȧ(param1 - X)(param2 - y)��������
			NA_JI_REQ_FB = 3,//���븱��
			NA_JI_REQ_LEAVE = 4,//�뿪����
			NA_JI_REQ_PLAY_INFO = 5,//��Ϸ��Ϣ
		};
		int req_type;
		int param1;
		int param2;
	};

	class SCTaoQuanNaJiInfo		//!< 1140 ��Ȧ�ɼ�
	{
	public:
		SCTaoQuanNaJiInfo();
		MessageHeader header;
		int map_init;
		int play_map[TAO_QUAN_NA_JI_X_NUM][TAO_QUAN_NA_JI_Y_NUM];//��ͼ
		int can_play_times;//����Ȧ����
		int today_play_times;//������ˣ����
		int flag_title;//(1/0)�Ƿ��Ѿ���ƺ�
	};

	class SCTaoQuanNaJiPlayEndInfo//!< 1141 ��Ȧ�ɼ�����
	{
	public:
		SCTaoQuanNaJiPlayEndInfo();
		MessageHeader header;

		int score;
		int item_1_count;
		int item_2_count;
		int item_3_count;
		int item_4_count;
	};

	class CSHundredGodBossReq	//!< 1145	����֮ʦ����
	{
	public:
		CSHundredGodBossReq();
		MessageHeader header;

		enum HUNDRED_GOD_BOSS_REQ_TYPE
		{
			HUNDRED_GOD_BOSS_REQ_TYPE_INFO = 0,					// ������Ϣ�·�
			HUNDRED_GOD_BOSS_REQ_TYPE_FETCH_TARGET = 1,			// ������ȡĿ�꽱�� param:target_id
			HUNDRED_GOD_BOSS_REQ_TYPE_FINISH_REWARD = 2,		// ������ȡ��ɽ���
			HUNDRED_GOD_BOSS_REQ_TYPE_START = 3,				// �������֮ʦ��ʼ
		};

		int req_type;
		int param;					
	};

	class SCHundredGodBossInfo	//!< 1146 ����֮ʦ��Ϣ�·�
	{
	public:
		SCHundredGodBossInfo();
		MessageHeader header;

		char reserve_ch;
		char is_finish_hundred_god_boss_flag;			// �Ƿ�����˰���֮ʦ 0:δ��� 1:�����
		short fetch_hundred_god_boss_flag;				// ����֮ʦ��ɽ�����ʾ(������) 1:����ȡ
		short hundred_god_boss_fin_bounty_count;		// ����֮ʦ���ʮ�������������
		short reserve_sh;
		unsigned int hundred_god_boss_end_timestamp;	//����֮ʦ����ʱ���
	};

	class CSPartnerPetRankReq	//!<5162 ���/����ǰʮ���а�����
	{
	public:
		CSPartnerPetRankReq();
		MessageHeader header;

		int rank_type;		// 0--��������, 1--�������
	};

	class SCPetTopRankInfo		//!<5163 ����ǰʮ��Ϣ�·�
	{
	public:
		SCPetTopRankInfo();
		MessageHeader header;
	
		PetTopRankInfo top_list[PET_TOP_RANK_NUM];
	};

	class SCPartnerTopRankInfo	//!<5164 ���ǰʮ��Ϣ�·�
	{
	public:
		SCPartnerTopRankInfo();
		MessageHeader header;

		PartnerTopRankInfo top_list[PARTNER_TOP_RANK_NUM];
	};

	class SCCharismaitcTanabataInfo	//!< 1148 ������Ϧ�·�
	{
	public:
		SCCharismaitcTanabataInfo();
		MessageHeader header;

		int admiration_value;//��Ľֵ
		BitMap<MAX_CHARIS_FLAG> admiration_map;
		int charm_value;//����ֵ
		BitMap<MAX_CHARIS_FLAG> charm_map;
	};

	class SCElementElfRewards	//!< 5185 Ԫ�ؾ��齱��֪ͨ�·�
	{
	public:
		SCElementElfRewards();
		MessageHeader header;
		
		int reward_seq;		// ��һ�õĽ���seq
	};

	class SCGuaJiExpInfo		//!< 5355 ÿ�չһ�����
	{
	public:
		SCGuaJiExpInfo();
		MessageHeader header;

		int today_add_guaji_exp;
	};

	class CSRealNameRewardReq		//!< 5410 ʵ����֤������ȡ����
	{
	public:
		CSRealNameRewardReq();
		MessageHeader header;
	};

	class SCRealNameRewardInfo		//!< 5411 ʵ����֤������ȡ��Ϣ�·�
	{
	public:
		SCRealNameRewardInfo();
		MessageHeader header;

		int receive_flag;
	};
}

#pragma pack(pop)

#endif

