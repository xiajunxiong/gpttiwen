#ifndef __MSG_RAND_ACTIVITY_HPP__
#define __MSG_RAND_ACTIVITY_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/serverdef.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/struct/global/animalracedef.hpp"
#include "servercommon/bitmapfunction.hpp"
#include "servercommon/roleactivity/raspringfestivalstoreparam.hpp"
#include "servercommon/roleactivity/racowparam.hpp"
#include "servercommon/roleactivity/rapetlootparam.hpp"
#include "servercommon/roleactivity/rafortunemazeparam.hpp"
#include "servercommon/roleactivity/rabackrewardparam.hpp"
#include "servercommon/roleactivity/rawanlinggongyingparam.hpp"
#include "servercommon/roleactivity/rahorcruxtianchengparam.hpp"
#include "servercommon/roleactivity/raintegraldrawparam.hpp"
#include "servercommon/roleactivity/rashanhaimetparam.hpp"
#include "servercommon/roleactivity/rashanhaijubaoparam.hpp"
#include "servercommon/roleactivity/razhaocaijinbaoparam.hpp"
#include "servercommon/roleactivity/racolorfuldoubleparam.hpp"
#include "servercommon/roleactivity/razaizhanyunzeparam.hpp"
#include "servercommon/roleactivity/rachaozhifenxiangparam.hpp"
#include "servercommon/roleactivity/rashanhaihuadengparam.hpp"
#include "servercommon/roleactivity/rajixinggaozhaoparam.hpp"
#include "servercommon/roleactivity/raweekendgiftdef.hpp"
#include "servercommon/roleactivity/rachaozhigiftdef.hpp"
#include "servercommon/roleactivity/rayinhuactivityparam.hpp"
#include "servercommon/roleactivity/raqiyuankongmingdengparam.hpp"
#include "servercommon/roleactivity/rabozhongyouliparam.hpp"
#include "servercommon/roleactivity/rayuanqijishibuparam.hpp"
#include "servercommon/roleactivity/razongxiangparam.hpp"
#include "servercommon/roleactivity/raluckyblindboxparam.hpp"
#include "servercommon/roleactivity/rasanbaichouparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	class CSRandActivityOperaReq		//!<3000 �����������
	{
	public:
		CSRandActivityOperaReq();
		MessageHeader header;

		short rand_activity_type;
		short opera_type;
		int param_1;
		int param_2;
		int param_3;
		unsigned int cfg_ver;
	};

	class SCScoreRewardInfo				//!< 1976 ���ֽ�����Ϣ		������1973
	{
	public:
		SCScoreRewardInfo();
		MessageHeader header;

		int capability;									//��������
		BitMap<SCORE_REWARD_MAX_NUM> flag;				//���ֽ������   SCORE_REWARD_MAX_NUM-32  ��СΪint       					
	};

	struct SCMonthInvestmentInfo       //!< 1977 �¶�Ͷ����Ϣ    ������1973
	{
	public:
		SCMonthInvestmentInfo();
		MessageHeader header;

		unsigned short is_buy;													//�Ƿ��ѹ����¶�Ͷ��  --0δ����   --1�ѹ���
		unsigned short last_num;												//ʣ�ಹǩ����
		int day;																//��ǰΪ�����¶�Ͷ�ʵĵڼ���
		BitMap<RA_MONTH_INVESTMENT_MAX_NUM> month_investment_fetch_flag;		//30�����ȡ��־ RA_MONTH_INVESTMENT_MAX_NUM--30  ��СΪint 
	};


	struct SCRAFirstChongZhiInfo			//3011 �׳���Ϣ
	{
	public:
		SCRAFirstChongZhiInfo();
		MessageHeader header;

		unsigned char reward_first_flag;										//�׳���ȡ��ʾ ������(FRIST_RECHARGE_TYPE_MAX)
		unsigned char first_cmd_buy_flag;										//�׳�ֱ����ʶ���� ������(FIRST_RECHARGE_CMD_TYPE_MAX)
		short first_recharge_zhao_cai_progress;									//�׳�ר��-�в������ۼƽ���
		unsigned int first_recharge_grow_end_timestamp;							//�׳�ר��-�ɳ�����δ�������ʱ���
		BitMap<MAX_FIRST_RECHARGE_EXCLUSIVE_NUM> first_recharge_exclusive_flag;	//�׳�ר��������ȡ��ʶ
		int guild_create_consume_coin_bind;										//��ҳ�ֵ���һ��ͭ�Ҵ����������flag,(0-δʹ��, 1-��ʹ��)
	};

	struct SCRAFirstChongZhiReq			//3012 �׳�����
	{
	public:
		SCRAFirstChongZhiReq();
		MessageHeader header;

		enum FIRST_CHONG_ZHI_REQ_TYPE
		{
			FIRST_CHONG_ZHI_REQ_TYPE_INFO = 0,						//�����׳���Ϣ�·�
			FIRST_CHONG_ZHI_REQ_TYPE_FETCH_FIRST_REWARD = 1,		//������ȡ�׳佱�� P1:FRIST_RECHARGE_TYPE_MAX(����seq)
			FIRST_CHONG_ZHI_REQ_TYPE_FETCH_EXCLUSIVE = 2,			//������ȡר������ P1:����type
			FIRST_CHONG_ZHI_REQ_TYPE_BUY_CMD_CHECK = 3,				//����ֱ��ǰ��� P1:FIRST_RECHARGE_CMD_TYPE_MAX P2:���
		};

		int req_type;
		int param1;
		int param2;
	};
	
	class CSRandActivityDuiHuanLingYuReq		//!<3090 ����-�һ�����
	{
	public:
		CSRandActivityDuiHuanLingYuReq();
		MessageHeader header;

		int index;
	};

	enum RA_BONUS_OP_TYPE
	{
		RA_BONUS_OP_TYPE_INFO = 0,		
		RA_BONUS_OP_TYPE_FETCH = 1,		//��ȡ���⽱�� param_1:seq
	};

	class CSRandActivityBonusOperaReq		//!<3050 �������⽱��ͨ������
	{
	public:
		CSRandActivityBonusOperaReq();
		MessageHeader header;

		short rand_activity_type;
		short opera_type;
		int param_1;
	};
	class CSRandActivityMakeUpReq		//!< 3045 ������������
	{
	public:
		CSRandActivityMakeUpReq();
		MessageHeader header;
	
		short rand_activity_type;
		short make_up_type;				//RAND_ACTIVITY_MAKE_UP_TYPE
	};
	class SCRandActivityBonusInfo		//!<3051 �������⽱����Ϣ�·�
	{
	public:
		SCRandActivityBonusInfo();
		MessageHeader header;
		
		BitMap<MAX_BONUS_COUNT_NUM> bonus_flag;		//  0 δ��ȡ 1 ����ȡ MAX_BONUS_COUNT_NUM -- 32
	};

	class SCRandActivityCJYXInfo		//!<3052 ����-�Ǿ�ӭ��
	{
	public:
		SCRandActivityCJYXInfo();
		MessageHeader header;

		int today_answer;
		int id;
		int has_answer_count;
		int has_answer_list[2];
	};

	class SCRandActivityNXDYInfo		//!<3053 ����-ţ�д���
	{
	public:
		SCRandActivityNXDYInfo();
		MessageHeader header;

		int today_reward;			// �����Ƿ��콱
		int id;
	};

	class SCRASpringFestivalStoreInfo		//!<3054 ����-�����
	{
	public:
		SCRASpringFestivalStoreInfo();
		MessageHeader header;

		int open_day;//���˵ڼ�����
		int phase;//�׶�
		short today_buy_times[MAX_LOG_TIMES];//���칺�����
		short activity_buy_times[MAX_LOG_TIMES];//��������
	};

	class SCRAMonpolyGiftInfo		//!<3055 ����-ţ�˵�ͷ�·�
	{
	public:
		SCRAMonpolyGiftInfo();
		MessageHeader header;

		unsigned int m_show_tamp;				// չʾ����ʱ���
		int now_map_id;							// ��ͼid
		int old_map_index;						// ԭ������-1������Զ�㣬ͨ���·��Ż�������
		int now_map_index;						// �����ڵ�ͼ����һ����
		int rand_point;							// �������ֵ
		int has_play_special_plan;			// �����淨�Ƿ�ִ�й��ˣ������Ƿ���ȡ���ˣ�
		short today_buy_times[MAX_COW_LOG_TIMES];//���칺�����
	};

	class SCRAMonpolyGiftExchangeInfo		//!<3056 ����-ţ�˵�ͷ�һ��·�
	{
	public:
		SCRAMonpolyGiftExchangeInfo();
		MessageHeader header;

		short today_buy_times[MAX_COW_LOG_TIMES];	// ���칺�����
	};

	class SCRAMonpolyGiftQuestionInfo		//!<3057 ����-ţ�˵�ͷ�����·�
	{
	public:
		SCRAMonpolyGiftQuestionInfo();
		MessageHeader header;

		int question_id;//��Ŀid
	};

	class SCRACowExchangeInfo		//!<3058 ����-ţ�˶һ��·�
	{
	public:
		SCRACowExchangeInfo();
		MessageHeader header;

		int m_unlock_integral;
		int m_jifen;
		short today_buy_times[MAX_COW_LOG_TIMES];//���칺�����
		short activity_buy_times[MAX_COW_LOG_TIMES];//��������
	};

	class SCRACowStoreInfo		//!<3059 ����-ţ���̳�
	{
	public:
		SCRACowStoreInfo();
		MessageHeader header;

		int open_day;
		short today_buy_times[MAX_COW_LOG_TIMES];
		short activity_buy_times[MAX_COW_LOG_TIMES];
	};

	class SCRACowLuckyExchangeInfo		//!<3060 ����-ţ���ޱ�
	{
	public:
		SCRACowLuckyExchangeInfo();
		MessageHeader header;

		long long history_chongzhi_num;
		int initmacy;
	};

	class SCRALittleYearGiftInfo		//!<3061 ����-С������
	{
	public:
		SCRALittleYearGiftInfo();
		MessageHeader header;

		int thread_id;//����id
		int has_get_reward;//�Ƿ���ȡ������
	};

	class SCRAPetLootInfo		//!<3062 ����-����ᱦ
	{
	public:
		SCRAPetLootInfo();
		MessageHeader header;

		int open_day;
		PetLootTask task_info[MAX_PET_LOOT_TASK_LIST_COUNT];

		int task_finish_times;
		int task_final_reward_flag;//�������ۼƽ�����ȡ���
	};

	class SCRAFortuneMazeInfo		//!<3617 ����-�����Թ�
	{
	public:
		SCRAFortuneMazeInfo();
		MessageHeader header;

		int open_day;
		unsigned int begin_time;
		int rand_pin_tu_times;
		int today_pin_tu_times;
		char can_enter_fb;
		char reserve_ch;
		short reserve_sh;
		int task_info[MAX_FORTUNE_MAZE_TASK_NUM];
	};

	class SCRAFortuneMazePinTuInfo		//!<3618 ����-�����Թ� ƴͼ
	{
	public:
		SCRAFortuneMazePinTuInfo();
		MessageHeader header;
		int mark_index;
		int m_pin_tu[PIN_TU_TYPE_NUM][PIN_TU_INDEX_NUM];//!< PIN_TU_TYPE_NUM = 7,PIN_TU_INDEX_NUM = 9
	};

	class SCRAYuanQiTurningInfo		//!<3084 ����-Ԫ������
	{
	public:
		SCRAYuanQiTurningInfo();
		MessageHeader header;
		
		int open_index;//��¼��Ŀ��Ŀ�
		int info_id;//��¼����id
		int reward_id;//����id
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// �ع����
	class CSBackRewardReq					//3106 ����-�ع���� ����
	{
	public:
		CSBackRewardReq();
		MessageHeader header;

		int type;
		int param;
	};

	class SCBackRewardInfo					//3105 ����-�ع���� ������Ϣ
	{
	public:
		SCBackRewardInfo();
		MessageHeader header;

		unsigned int huigui_timestamp;
		char day_flag[MAX_RA_BACK_REWARD_DAY];
		RABackRewardTaskData task_list[MAX_RA_BACK_REWARD_TASK];
	};

	class SCRAHappyLookInfo		//!<3065 ����-�����ַ���
	{
	public:
		SCRAHappyLookInfo();
		MessageHeader header;

		short ra_frequency_num;										// ����ս����

		char challenge_index;										// ��սindex
		char challenge_is_succ;										// �Ƿ���ս�ɹ�
		int challenge_end_timestamp;								// ��սʧ��ʱ���
	};

	class SCRASignHappyInfo		//!<3066 ����-��¼�ͺ���
	{
	public:
		SCRASignHappyInfo();
		MessageHeader header;

		int login_day;
		int fetch_mark;
	};

	class SCRAWanLingGongYingInfo		//!<3067 ����-���鹲ӯ
	{
	public:
		SCRAWanLingGongYingInfo();
		MessageHeader header;

		unsigned int ra_buy_reward_timestamp;
		RAWLGYTaskData task_list[RA_WAN_LING_GONG_YING_TASK_MAX];
	};

	class SCRAResultNotice :public IMessageSerializer				//!< 1508 ����֪ͨ�·�
	{
	public:
		SCRAResultNotice();
		virtual ~SCRAResultNotice() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteShort(result_type, buffer, buffer_size, out_length);
			result = result && WriteShort(activity_type, buffer, buffer_size, out_length);
			result = result && WriteInt(param1, buffer, buffer_size, out_length);
			result = result && WriteInt(param2, buffer, buffer_size, out_length);
			result = result && WriteInt(count, buffer, buffer_size, out_length);

			for (int i = 0; result && i < count && i < ARRAY_LENGTH(item_list); ++i)
			{
				ItemInfo  curr = item_list[i];

				result = result && WriteUShort(curr.item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(curr.is_bind, buffer, buffer_size, out_length);
				result = result && WriteInt(curr.count, buffer, buffer_size, out_length);
				result = result && WriteShort(curr.has_param, buffer, buffer_size, out_length);

				if (0 != curr.has_param)
				{
					result = result && WriteUInt(curr.param_length, buffer, buffer_size, out_length);
					result = result && WriteStrN(curr.param_data, curr.param_length, buffer, buffer_size, out_length);
				}
			}

			return result;
		}

		enum RAND_ACTIVITY_RESULT_NOTICE_TYPE
		{
			RAND_ACTIVITY_RESULT_NOTICE_TYPE_BEGIN = 0,
			RAND_ACTIVITY_RESULT_NOTICE_TYPE_BATTLE_WIN = 1,		//ս��ʤ��
			RAND_ACTIVITY_RESULT_NOTICE_TYPE_BATTLE_FAIL = 2,		//ս��ʧ��
			RAND_ACTIVITY_RESULT_NOTICE_TYPE_GET_REWARD = 3,		//�����

			RAND_ACTIVITY_RESULT_NOTICE_TYPE_MAX
		};
		static const int MAX_REWARD_ITEM_NUM = 120;

		struct ItemInfo
		{
			void Reset()
			{
				item_id = 0;
				is_bind = 0;
				count = 0;
				has_param = 0;
				param_length = 0;
				memset(param_data, 0, sizeof(param_data));
			}

			ItemID			item_id;
			short			is_bind;
			int				count;
			short			reserve_sh;
			short			has_param;
			unsigned int	param_length;		//!< ��ֵ���������ݳ��� ����has_param��Ϊ0ʱ�ɶ���
			NetValueItemParamData param_data;	//!< ��ֵ���������ݣ�����param_length��ֵ����ȡ ����has_paramΪ1ʱ�ɶ���
		};

		short result_type;
		short activity_type;
		int param1;
		int param2;
		int count;
		ItemInfo item_list[MAX_REWARD_ITEM_NUM];
	};

	struct SCRAHorcruxTianChengRoleInfo
	{
	public:
		SCRAHorcruxTianChengRoleInfo();
		MessageHeader header;

		short shop_list[MAX_RA_HORCRUX_TIANCHENG_SHOP_INDEX];
		short exchange_list[MAX_RA_HORCRUX_TIANCHENG_EXCHANGE_INDEX];
		short consume_fetch_mark;
		int consume_gold;
	};

	struct SCRAHorcruxTianChengPoolInfo
	{
	public:
		SCRAHorcruxTianChengPoolInfo();
		MessageHeader header;

		short pool_type;
		unsigned short item_id;
		int num;
	};

	struct SCRAHorcruxTianChengDrawResult
	{
	public:
		SCRAHorcruxTianChengDrawResult();
		MessageHeader header;

		int seq;
		int num;
	};

	struct SCRAIntegralDrawRoleInfo
	{
	public:
		SCRAIntegralDrawRoleInfo();
		MessageHeader header;

		int draw_times;
		short reward_times[MAX_RA_INTEGRAL_DRAW_LINE_NUM];
	};

	struct SCRAIntegralDrawGameInfo
	{
	public:
		SCRAIntegralDrawGameInfo();
		MessageHeader header;

		int is_game_open;
	};

	struct SCRAIntegralDrawResult
	{
	public:
		SCRAIntegralDrawResult();
		MessageHeader header;

		char color[MAX_RA_INTEGRAL_DRAW_LINE_NUM];
	};
	

	//���� ɽ���۱�
	//��������
	enum RAShanHaiJuBaoReqType
	{
		RA_SHAN_HAI_JU_BAO_REQ_TYPE_DRAW = 0,		//�齱 p_1:��׶�(phase_seq)
		RA_SHAN_HAI_JU_BAO_REQ_TYPE_FETCH_EXTRA = 1,//��ȡ���⽱�� p_1:��׶�(phase_seq) p_2:���⽱��index
	};
	//��ɫ��Ϣ		4177
	class SCRAShanHaiJuBaoInfo
	{
	public:
		SCRAShanHaiJuBaoInfo();
		MessageHeader header;

		struct PhaseInfo
		{
			int draw_times;	//�ܳ�ȡ����
			int extra_reward_flag;// ���⽱����ȡ���
	};

		PhaseInfo phase_info_[SHAN_HAI_JU_BAO_EXTRA_PHASE_MAX];//�׶���Ϣ[��׶� - 1]
	};
	//�齱����		4178
	class SCRAShanHaiJuBaoDrawRet
	{
	public:
		SCRAShanHaiJuBaoDrawRet();
		MessageHeader header;

		int show_type;	//������Ϣ
	};

	//���� ɽ������
	//��������
	enum RAShanHaiMetReqType
	{
		RA_SHAN_HAI_MET_REQ_TYPE_FETCH_DAILY_REWARD = 0,//��ȡ���� p_1:�����index p_2:��ȡ����
		RA_SHAN_HAI_MET_REQ_TYPE_CMD_BUY = 1,			//ֱ����� 
	};
	//��ɫ��Ϣ		4185
	class SCRAShanHaiMetBase
	{
	public:
		SCRAShanHaiMetBase();
		MessageHeader header;

		int act_extra_rewaed_flag;	//���⽱��������

		int day_task_finish_flag;	//������ɱ��(ֻ����ȡ�Ϳ���ʱУ��)
		int fetch_reward_flag;		//�������ű��[�����index]
		int fetch_extra_rewaed_flag;//���⽱�����ű��[�����index]
	};
	//������Ϣ		4186
	class SCRAShanHaiMetTakeParam
	{
	public:
		SCRAShanHaiMetTakeParam();
		MessageHeader header;

		int task_param_count;
		//�������[��������]
		int task_param_list[RAShanHaiMet::TSAK_TYPE_MAX];
	};
	//����ı�		4187
	class SCRAShanHaiMetTaskParamChange
	{
	public:
		SCRAShanHaiMetTaskParamChange();
		MessageHeader header;

		int task_type;	//��������
		int task_param;	//�������
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// �вƽ���
	struct SCRAZhaoCaiJinBaoRoleInfo			// 3112
	{
	public:
		SCRAZhaoCaiJinBaoRoleInfo();
		MessageHeader header;

		unsigned int next_refresh_timestamp;						// �´�ˢ��ʱ���

		int history_chongzhi;										// ��ۼƳ�ֵ���
		int day_chongzhi;											// �ÿ���ۻ���ֵ���
		short chongzhi_time[MAX_RA_ZHAOCAIJINBAO_CHONGZHI_NUM];		// ���ʳ�ֵ�������

		short pool[MAX_RA_ZHAOCAIJINBAO_REFRESH_NUM];				// ���� seq
		short buy_list[MAX_RA_ZHAOCAIJINBAO_BUY_NUM];				// �������
		short chongzhi_list[MAX_RA_ZHAOCAIJINBAO_CHONGZHI_NUM];		// ��ֵ��������
	};

	struct SCRAZhaoCaiJinBaoDrawResult			// 3113
	{
	public:
		SCRAZhaoCaiJinBaoDrawResult();
		MessageHeader header;

		int count;
		int seq_list[50];
	};

	struct SCRAZhaoCaiJinBaoRecordListInfo		// 3171
	{
	public:
		SCRAZhaoCaiJinBaoRecordListInfo();
		MessageHeader header;

		int count;
		RAZhaoCaiJinBaoRecordInfo record_list[MAX_RA_ZHAOCAIJINBAO_RECORD_NUM];
	};

	////////////////////////////////////////////////////
	//////���� �ͷ׼ӱ�
	//��������
	enum RAColorfulDoubleReqType
	{
		RA_COLORFUL_DOUBLE_REQ_TYPE_FETCH_FREE_REWARD = 0,	//��ȡ��ѽ���
		RA_COLORFUL_DOUBLE_REQ_TYPE_ACT_BOX = 1,			//����佱�� P_1: ����seq 0:����ȫ��
		RA_COLORFUL_DOUBLE_REQ_TYPE_FETCH_BOX_REWARD = 2,	//��ȡ���佱�� p_1: ����seq
	};

	//��ɫ��Ϣ
	class SCRAColorfulDoubleBase
	{
	public:
		SCRAColorfulDoubleBase();
		MessageHeader header;

		int today_free_fetch_flag;		//������ѽ�����ȡ���

		int act_box_rewaed_flag;		//���佱��������
		int fetch_box_rewaed_flag;		//���佱�����ű��[����seq]

		int act_task_finish_num;		//����������
	};

	//������Ϣ
	class SCRAColorfulDoubleTakeParam
	{
	public:
		SCRAColorfulDoubleTakeParam();
		MessageHeader header;

		int task_param_count;
		//�������[��������]
		int task_param_list[RAColorfulDouble::TSAK_TYPE_MAX];
	};

	//����ı�
	class SCRAColorfulDoubleTaskParamChange
	{
	public:
		SCRAColorfulDoubleTaskParamChange();
		MessageHeader header;

		int task_type;	//��������
		int task_param;	//�������
	};

	////////////////////////////////////////////////////
	//////��ս����
	//��������
	enum RAZaiZhanYunZeReqType
	{
		RA_ZAI_ZHAN_YUN_ZE_REQ_TYPE_BOSS_BATTLE = 0,		//��սboss p_1:boss_index
	};

	//��ɫ��Ϣ  4190
	class SCRAZaiZhanYunZeBase
	{
	public:
		SCRAZaiZhanYunZeBase();
		MessageHeader header;

		//boss��սͨ�ش���[boss_index]
		int boss_info_count;
		int boss_pass_num[RAZaiZhanYunZe::BOSS_MAX];
	};

	////////////////////////////////////////////////////
	// ħ��������
	class SCRAMoZuZaiNaLiInfo					//3116 ����-ħ��������
	{
	public:
		SCRAMoZuZaiNaLiInfo();
		MessageHeader header;

		int day_times;
	};

	////////////////////////////////////////////////////
	// ��ֵ����
	class SCRAChaoZhiFenXiangInfo				//3117 ����-��ֵ����
	{
	public:
		SCRAChaoZhiFenXiangInfo();
		MessageHeader header;

		short task_fetch_mark;
		short zhigou_mark;

		short task_comple[RA_CHAOZHI_FENXIANG_TASK_TYPE_MAX];
		short re_sh;
	};

	////////////////////////////////////////////////////
	// ɽ������
	enum RA_SHANHAI_HUADENG_REQ_TYPE	//��Э��3000
	{
		RA_SHANHAI_HUADENG_REQ_TYPE_START_GAME = 0,					// ��ʼ��Ϸ
		RA_SHANHAI_HUADENG_REQ_TYPE_CATCH_LANTERN = 1,				// ��ץ����		param1:type
		RA_SHANHAI_HUADENG_REQ_TYPE_FETCH_REWARD = 2,				// ��ȡ������	param1:task_id
		RA_SHANHAI_HUADENG_REQ_TYPE_FORCE_CLOSE = 3,				// ǿ�ƹر�
		RA_SHANHAI_HUADENG_REQ_TYPE_FLOW_AWAY = 4,					// ��������		param1:type

		RA_SHANHAI_HUADENG_REQ_TYPE_MAX
	};

	class SCRAShanHaiHuaDengAllInfo		//!< 3123 ����-��ʻ�����Ϣ�·�
	{
	public:
		SCRAShanHaiHuaDengAllInfo();
		MessageHeader header;

		short last_game_score_count;								//��һ����Ϸ��õĻ���
		short day_single_score_count;								//���쵥�λ������ֵ
		short activity_single_score_count;							//��ڵ��λ������ֵ
		short day_join_times;										//���ղ������
		short activity_join_times;									//��ڲ������
		short day_score_count;										//���ջ�õ��ܷ�
		int activity_score_count;									//��ڻ�õ��ܻ���
		BitMap<MAX_RA_SHANHAI_HUADENG_TASK_NUM> m_task_list_flag;	//�����б��ʶ 0:δ��� 1:�����
	};

	enum RA_SHANHAI_HUADENG_GAME_NOTICE_TYPE
	{
		RA_SHANHAI_HUADENG_GAME_NOTICE_TYPE_START_GAME = 0,				//��Ϸ��ʼ
		RA_SHANHAI_HUADENG_GAME_NOTICE_TYPE_IN_THE_GAME = 1,			//��Ϸ������
		RA_SHANHAI_HUADENG_GAME_NOTICE_TYPE_GAME_OVER = 2,				//��Ϸ�����·�����

		RA_SHANHAI_HUADENG_GAME_NOTICE_TYPE_MAX
	};

	class SCRAShanHaiHuaDengGameInfo	//!< 3124 ����-��ʻ�����Ϸ��Ϣ�·�
	{
	public:
		SCRAShanHaiHuaDengGameInfo();
		MessageHeader header;

		short notice_type;							//RA_SHANHAI_HUADENG_GAME_NOTICE_TYPE_MAX
		short single_score_count;					//������Ϸ�ڵĻ���
		unsigned int game_end_timestamp;			//������Ϸ����ʱ���
	};

	class SCRAShanHaiHuaDengLightInfo	//!< 3125 ����-��ʻ��ƻ�����Ϣ�·�
	{
	public:
		SCRAShanHaiHuaDengLightInfo();
		MessageHeader header;

		short light_list[MAX_RA_SHANHAI_HUADENG_LIGHT_TYPE_NUM];		//��������  MAX_RA_SHANHAI_HUADENG_LIGHT_TYPE_NUM  -- 10
	};

	////////////////////////////////////////////////////
	// ���Ǹ���
	struct SCRAJiXingGaoZhaoRoleInfo					// 3127 ����-���Ǹ��� ��ɫ��Ϣ
	{
	public:
		SCRAJiXingGaoZhaoRoleInfo();
		MessageHeader header;

		int draw_times;
		int draw_reward_fetch_mark;
		short shop_list[MAX_RA_JIXING_GAOZHAO_SHOP_INDEX];
	};

	struct SCRAJiXingGaoZhaoPoolInfo					// 3128 ����-���Ǹ��� ������Ϣ
	{
	public:
		SCRAJiXingGaoZhaoPoolInfo();
		MessageHeader header;

		unsigned short item_id;
		short re_sh;
		unsigned int num;
	};

	struct SCRAJiXingGaoZhaoDrawResult					// 3129 ����-���Ǹ��� �齱����
	{
	public:
		SCRAJiXingGaoZhaoDrawResult();
		MessageHeader header;

		int seq;
		int num;
	};

	class SCRAJiXingGaoZhaoRecordListInfo			// 3130
	{
	public:
		SCRAJiXingGaoZhaoRecordListInfo();
		MessageHeader	header;

		int record_count;
		JiXingGaoZhaoRecordInfo record_list[10];
	};

	class SCRAJiXingGaoZhaoRecordInfo				// 3131
	{
	public:
		SCRAJiXingGaoZhaoRecordInfo();
		MessageHeader	header;

		JiXingGaoZhaoRecordInfo record;
	};

	///////////////////////////////////////////////////////////
	// ��ĩ���
	enum RAWeekendGiftReqType
	{
		RA_WEEKEND_GIFT_REQ_TYPE_CMD_BUY = 0,	//ֱ��: p_1: �������
	};
	// ��ɫ��Ϣ 4335
	class SCRAWeekendGiftInfo
	{
	public:
		SCRAWeekendGiftInfo();
		MessageHeader header;

		int count;
		struct GiftInfo
		{
			GiftInfo()
			{
				gift_index = 0;
				buy_times = 0;
				out_time_s = 0;
			}
			int gift_index;		//�������
			int buy_times;		//�ѹ������

			unsigned int out_time_s;//ʧЧʱ��
		};
		GiftInfo gift_list[RAWeekendGift::ROLE_GIFT_MAX];
	};

	///////////////////////////////////////////////////////////
	// ��ֵ����
	enum RAChaoZhiGifttReqType
	{
		RA_CHAO_ZHI_GIFT_REQ_TYPE_CMD_BUY = 0,	//ֱ��
		RA_CHAO_ZHI_GIFT_REQ_TYPE_FETCH = 1,	//��ȡ	p_1: gift_index
	};
	//��ɫ��Ϣ
	class SCRAChaoZhiGiftInfo
	{
	public:
		SCRAChaoZhiGiftInfo();
		MessageHeader header;

		int gift_act_flag;		//��������־

		int count;
		//��ȡ����[gift_index] 0: δ��ȡ
		int gift_num_list[RAChaoZhiGift::GIFT_MAX];
	};

	////////////////////////////////////////////////////
	// ��������
	struct SCRAYinHuWenShiInfo					// 3139 ����-�������� ��ɫ��Ϣ
	{
	public:
		SCRAYinHuWenShiInfo();
		MessageHeader header;

		short zhigou_flag;
		short reward_fetch_mark;

		short reward_type[RA_YINHU_WENSHI_DAY_MAX];
	};

	////////////////////////////////////////////////////
	// ��ţӭ�»�
	struct SCRACiNiuYingXinHuInfo					// 3142 ����-��ţӭ�»� ��ɫ��Ϣ
	{
	public:
		SCRACiNiuYingXinHuInfo();
		MessageHeader header;

		short awaken_mark;
		short awaken_reward_time;
		RACiNiuYinXinHuTypeData awaken_param[RA_CINIU_YINGXINHU_TYPE_MAX];
	};


	////////////////////////////////////////////////////
	// ���д���
	struct SCRAHuXingDaYunInfo					// 3144 ����-���д��� ��ɫ��Ϣ
	{
	public:
		SCRAHuXingDaYunInfo();
		MessageHeader header;

		unsigned int complete_time;					// ��Ϸ���ʱ���	
		short task_pos_id;								// ��ǰ�����
		short task_first_pass;							// �Ƿ���ͨ
		char is_reward;									// �Ƿ��ߵ������ ������ȡ����

		char is_be_invited_reward;						// �����뽱���Ƿ����ȡ
		short be_invited_times;							// ���������
		unsigned int be_invited_complete_time;			// ������ ���ʱ���
	};

	struct SCRAHuXingDaYunBeInvitedInfo			// 3145
	{
	public:
		SCRAHuXingDaYunBeInvitedInfo();
		MessageHeader header;

		int count;
		RAHuXingDaYunBeInvitedData user_info[RA_HU_XING_DA_YUN_USER_NUM];
	};

	////////////////////////////////////////////////////
	// ��������
	struct SCRAYinHuBaiSuiInfo					// 3143 ����-�������� ��ɫ��Ϣ
	{
	public:
		SCRAYinHuBaiSuiInfo();
		MessageHeader header;

		short zhigou_flag;
		short reward_fetch_mark;;
	};

	///////////////////////////////////////////////////////////
	// ��Եʱװ
	class SCRAQingYuanShiZhuangInfo			// 4425
	{
	public:
		SCRAQingYuanShiZhuangInfo();
		MessageHeader header;

		short buy_mark;
		short fetch_mark;
		short is_open_client;
		short is_role_buy_mark;
	};

	///////////////////////////////////////////////////////////
	// ��Ե�۳�
	class SCRAQingYuanLeiChongInfo			// 4426
	{
	public:
		SCRAQingYuanLeiChongInfo();
		MessageHeader header;

		unsigned int join_begin_timestamp;			// ���뿪ʼʱ���
		unsigned int join_end_timestamp;			// �������ʱ���

		int chongzhi_gold;							// ��ֵ���
		int fetch_mark;								// ��ȡ���
	};

	////////////////////////////////////////////////////
	// ��Ը������

	struct SCRAQiYuanKongMingDengInfo					// 3156 ����-��Ը������ ��ɫ��Ϣ
	{
	public:
		SCRAQiYuanKongMingDengInfo();
		MessageHeader header;

		int huozhong;
		int complete_flag;							// ���������� ��λ���
		unsigned short task_list[MAX_RA_QIYUAN_KONGMINGDENG_TASK_COUNT];		// �������
	};

	////////////////////////////////////////////////////
	// Ѱ�κ���
	struct SCRAXunMengHuDieInfo					// 3157 ����-Ѱ�κ��� ��ɫ��Ϣ
	{
	public:
		SCRAXunMengHuDieInfo();
		MessageHeader header;

		int frequency_num;
	};

	////////////////////////////////////////////////////
	// ��������
	struct SCRABoZhongYouLiInfo					// 3159 ����-�������� ��ɫ��Ϣ
	{
	public:
		SCRABoZhongYouLiInfo();
		MessageHeader header;

		int sowing_count;		// ʣ��ɲ��ִ���
		unsigned int next_refresh_timestamp;	// ��ˮ�´�ˢ��ʱ���
		unsigned int planting_timestamp[BO_ZHONG_YOU_LI_MAX_PLANT_NUM];		// ֲ����ֲʱ���
	};

	////////////////////////////////////////////////////
	// Ԫ������
	struct SCRAYuanQiFanPaiInfo					// 3161 ����-Ԫ������ ��ɫ��Ϣ
	{
	public:
		SCRAYuanQiFanPaiInfo();
		MessageHeader header;

		int lottery_count;				// ���ƴ���
		BitMap<MAX_YUAN_QI_FAN_PAI_BITMAP> is_receive_times;	// �Ƿ�����ȡ���ƴ�������
		int is_fetch_day_reward;
	};

	////////////////////////////////////////////////////
	// ��������
	struct SCRAWanLingZhuLiInfo					// 3164 ����-�������� ��ɫ��Ϣ
	{
	public:
		SCRAWanLingZhuLiInfo();
		MessageHeader header;

		short login_day;
		short fetch_reward_mark;				// ������ȡ���
		char day_reward_mark;					// ÿ����ȡ���
		char is_buy_jiagou;						// �Ƿ�ӹ�
		short re_sh;
	};

	////////////////////////////////////////////////////
	// ��������
	struct SCRABaoZiLaiLeInfo					//3165 ����-��������
	{
	public:
		SCRABaoZiLaiLeInfo();
		MessageHeader header;

		int join_count;			//�����Ѳ������
	};

	////////////////////////////////////////////////////
	// Ԫ�����²�
	struct SCRAYuanQiJiShiBuInfo					// 3166 ����-Ԫ�����²� ��ɫ��Ϣ
	{
	public:
		SCRAYuanQiJiShiBuInfo();
		MessageHeader header;

		short combat;							// ��õ�����ֵ, �����
		char is_jinjie_jiangli;					// �Ƿ�����׽���
		char re_ch;

		int quanming_reward_fetch_flag;			// ��ѽ���	
		int jinjie_reward_fetch_flag;			// ���׽���
		int complete_flag;					// ���������� ��λ���
		unsigned short task_list[MAX_RA_YUAN_QI_JISHIBU_TASK_COUNT];		// �������
	};

	////////////////////////////////////////////////////
	// �˷�����
	struct SCRAChengFengPoLangOpenInfo					//3167 ����-�˷����˿�����Ϣ�·�
	{
	public:
		SCRAChengFengPoLangOpenInfo();
		MessageHeader header;

		int is_open;			//��Ƿ��ڿ���ʱ����
	};

	struct SCRAChengFengPoLangInfo					//3168 ����-�˷�����
	{
	public:
		SCRAChengFengPoLangInfo();
		MessageHeader header;

		int answer_times;		// �����Ѵ������
		int cur_question_id;	// ��ǰ��Ŀid
	};

	struct SCRAMiaoBiShengHuaInfo					//!<3170  ����-�������
	{
	public:
		SCRAMiaoBiShengHuaInfo();
		MessageHeader header;

		int count;	
	};

	struct SCRAZongXiangManChengInfo//!< 3180 �������������·�
	{
	public:
		SCRAZongXiangManChengInfo();
		MessageHeader header;

		int light_type;
		BitMap<MAX_ZONG_XIANG_MAN_CHENG_FLAG> m_flag_map;
	};

	struct SCRASaltySweetBattleInfo//!< 3181 ��������֮���·�
	{
	public:
		SCRASaltySweetBattleInfo();
		MessageHeader header;

		int salty_value;//��ֵ
		int sweet_value;//��ֵ
		int status_mod;//��Ϸ״̬
	};

	struct SCRASaltySweetBattleRoleInfo//!< 3182 ��������֮�������·�
	{
	public:
		SCRASaltySweetBattleRoleInfo();
		MessageHeader header;

		int vote_type;//���ѡ�����͡�0����ѡ��1�̣�2��
		int reward_flag;
	};

	struct SCRALuckyBlindBoxInfo//!< 3185 ��������ä���·�
	{
	public:
		SCRALuckyBlindBoxInfo();
		MessageHeader header;

		long long chongzhi;
		char box_flag[MAX_LUCKY_BLIND_BOX_X][MAX_LUCKY_BLIND_BOX_Y];
		short today_shop_buy_times[MAX_LUCKY_BLIND_BOX_SEQ];//�̳ǽ��չ������
		short shop_buy_times[MAX_LUCKY_BLIND_BOX_SEQ];//�̳ǻ�ۻ��������

		short today_recharge_buy_times[MAX_LUCKY_BLIND_BOX_SEQ];//�ɾͽ�����ȡ����
		short recharge_buy_times[MAX_LUCKY_BLIND_BOX_SEQ];//�ɾͻ��ȡ����
	};

	////////////////////////////////////////////////////
	// 300��Ƥ��
	struct SCRASanBaiChouInfo					// 4805 ����-���ٳ� ��ɫ��Ϣ
	{
	public:
		SCRASanBaiChouInfo();
		MessageHeader header;

		int draw_times;												// �ܹ��齱����
		int reward_item_seq;										// ����ѡ��Ľ���

		RASanBaiChouTypeParam select[MAX_RA_SANBAI_CHOU_TYPE_NUM];		// ���Ͳ���
	};

	struct SCRASanBaiChouDrawInfo				// 4806 ����-���ٳ� �齱����
	{
	public:
		SCRASanBaiChouDrawInfo();
		MessageHeader header;

		short type;
		short select_req;
		int type_times;											
		int draw_times;												// �ܹ��齱����
		short pool[MAX_RA_SANBAI_CHOU_DRAW_NUM];
	};
}

#pragma pack(pop)
#endif	// __MSG_RAND_ACTIVITY_HPP__