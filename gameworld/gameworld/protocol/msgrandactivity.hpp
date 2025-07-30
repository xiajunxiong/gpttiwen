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
	class CSRandActivityOperaReq		//!<3000 随机活动请求操作
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

	class SCScoreRewardInfo				//!< 1976 评分奖励信息		请求在1973
	{
	public:
		SCScoreRewardInfo();
		MessageHeader header;

		int capability;									//人物评分
		BitMap<SCORE_REWARD_MAX_NUM> flag;				//评分奖励标记   SCORE_REWARD_MAX_NUM-32  大小为int       					
	};

	struct SCMonthInvestmentInfo       //!< 1977 月度投资信息    请求在1973
	{
	public:
		SCMonthInvestmentInfo();
		MessageHeader header;

		unsigned short is_buy;													//是否已购买月度投资  --0未购买   --1已购买
		unsigned short last_num;												//剩余补签次数
		int day;																//当前为购买月度投资的第几天
		BitMap<RA_MONTH_INVESTMENT_MAX_NUM> month_investment_fetch_flag;		//30天的领取标志 RA_MONTH_INVESTMENT_MAX_NUM--30  大小为int 
	};


	struct SCRAFirstChongZhiInfo			//3011 首充信息
	{
	public:
		SCRAFirstChongZhiInfo();
		MessageHeader header;

		unsigned char reward_first_flag;										//首充领取表示 二进制(FRIST_RECHARGE_TYPE_MAX)
		unsigned char first_cmd_buy_flag;										//首充直购标识设置 二进制(FIRST_RECHARGE_CMD_TYPE_MAX)
		short first_recharge_zhao_cai_progress;									//首充专属-招财仙子累计进度
		unsigned int first_recharge_grow_end_timestamp;							//首充专属-成长助力未购买结束时间戳
		BitMap<MAX_FIRST_RECHARGE_EXCLUSIVE_NUM> first_recharge_exclusive_flag;	//首充专属奖励领取标识
		int guild_create_consume_coin_bind;										//玩家充值获得一次铜币创建家族机会flag,(0-未使用, 1-已使用)
	};

	struct SCRAFirstChongZhiReq			//3012 首充请求
	{
	public:
		SCRAFirstChongZhiReq();
		MessageHeader header;

		enum FIRST_CHONG_ZHI_REQ_TYPE
		{
			FIRST_CHONG_ZHI_REQ_TYPE_INFO = 0,						//请求首充信息下发
			FIRST_CHONG_ZHI_REQ_TYPE_FETCH_FIRST_REWARD = 1,		//请求领取首充奖励 P1:FRIST_RECHARGE_TYPE_MAX(配置seq)
			FIRST_CHONG_ZHI_REQ_TYPE_FETCH_EXCLUSIVE = 2,			//请求领取专属奖励 P1:配置type
			FIRST_CHONG_ZHI_REQ_TYPE_BUY_CMD_CHECK = 3,				//请求直购前检测 P1:FIRST_RECHARGE_CMD_TYPE_MAX P2:版号
		};

		int req_type;
		int param1;
		int param2;
	};
	
	class CSRandActivityDuiHuanLingYuReq		//!<3090 随机活动-兑换灵玉
	{
	public:
		CSRandActivityDuiHuanLingYuReq();
		MessageHeader header;

		int index;
	};

	enum RA_BONUS_OP_TYPE
	{
		RA_BONUS_OP_TYPE_INFO = 0,		
		RA_BONUS_OP_TYPE_FETCH = 1,		//领取额外奖励 param_1:seq
	};

	class CSRandActivityBonusOperaReq		//!<3050 随机活动额外奖励通用请求
	{
	public:
		CSRandActivityBonusOperaReq();
		MessageHeader header;

		short rand_activity_type;
		short opera_type;
		int param_1;
	};
	class CSRandActivityMakeUpReq		//!< 3045 奖励补齐请求
	{
	public:
		CSRandActivityMakeUpReq();
		MessageHeader header;
	
		short rand_activity_type;
		short make_up_type;				//RAND_ACTIVITY_MAKE_UP_TYPE
	};
	class SCRandActivityBonusInfo		//!<3051 随机活动额外奖励信息下发
	{
	public:
		SCRandActivityBonusInfo();
		MessageHeader header;
		
		BitMap<MAX_BONUS_COUNT_NUM> bonus_flag;		//  0 未领取 1 已领取 MAX_BONUS_COUNT_NUM -- 32
	};

	class SCRandActivityCJYXInfo		//!<3052 随机活动-辞旧迎新
	{
	public:
		SCRandActivityCJYXInfo();
		MessageHeader header;

		int today_answer;
		int id;
		int has_answer_count;
		int has_answer_list[2];
	};

	class SCRandActivityNXDYInfo		//!<3053 随机活动-牛行大运
	{
	public:
		SCRandActivityNXDYInfo();
		MessageHeader header;

		int today_reward;			// 今天是否领奖
		int id;
	};

	class SCRASpringFestivalStoreInfo		//!<3054 随机活动-年货节
	{
	public:
		SCRASpringFestivalStoreInfo();
		MessageHeader header;

		int open_day;//开了第几天了
		int phase;//阶段
		short today_buy_times[MAX_LOG_TIMES];//今天购买次数
		short activity_buy_times[MAX_LOG_TIMES];//活动购买次数
	};

	class SCRAMonpolyGiftInfo		//!<3055 随机活动-牛运当头下发
	{
	public:
		SCRAMonpolyGiftInfo();
		MessageHeader header;

		unsigned int m_show_tamp;				// 展示结束时间戳
		int now_map_id;							// 地图id
		int old_map_index;						// 原点数（-1则是无远点，通用下发才会这样）
		int now_map_index;						// 现在在地图的哪一个点
		int rand_point;							// 随机点数值
		int has_play_special_plan;			// 特殊玩法是否执行过了（奖励是否领取过了）
		short today_buy_times[MAX_COW_LOG_TIMES];//今天购买次数
	};

	class SCRAMonpolyGiftExchangeInfo		//!<3056 随机活动-牛运当头兑换下发
	{
	public:
		SCRAMonpolyGiftExchangeInfo();
		MessageHeader header;

		short today_buy_times[MAX_COW_LOG_TIMES];	// 今天购买次数
	};

	class SCRAMonpolyGiftQuestionInfo		//!<3057 随机活动-牛运当头答题下发
	{
	public:
		SCRAMonpolyGiftQuestionInfo();
		MessageHeader header;

		int question_id;//题目id
	};

	class SCRACowExchangeInfo		//!<3058 随机活动-牛运兑换下发
	{
	public:
		SCRACowExchangeInfo();
		MessageHeader header;

		int m_unlock_integral;
		int m_jifen;
		short today_buy_times[MAX_COW_LOG_TIMES];//今天购买次数
		short activity_buy_times[MAX_COW_LOG_TIMES];//活动购买次数
	};

	class SCRACowStoreInfo		//!<3059 随机活动-牛运商城
	{
	public:
		SCRACowStoreInfo();
		MessageHeader header;

		int open_day;
		short today_buy_times[MAX_COW_LOG_TIMES];
		short activity_buy_times[MAX_COW_LOG_TIMES];
	};

	class SCRACowLuckyExchangeInfo		//!<3060 随机活动-牛运无边
	{
	public:
		SCRACowLuckyExchangeInfo();
		MessageHeader header;

		long long history_chongzhi_num;
		int initmacy;
	};

	class SCRALittleYearGiftInfo		//!<3061 随机活动-小年送礼
	{
	public:
		SCRALittleYearGiftInfo();
		MessageHeader header;

		int thread_id;//线索id
		int has_get_reward;//是否领取奖励了
	};

	class SCRAPetLootInfo		//!<3062 随机活动-宠物夺宝
	{
	public:
		SCRAPetLootInfo();
		MessageHeader header;

		int open_day;
		PetLootTask task_info[MAX_PET_LOOT_TASK_LIST_COUNT];

		int task_finish_times;
		int task_final_reward_flag;//二进制累计奖励领取标记
	};

	class SCRAFortuneMazeInfo		//!<3617 随机活动-幸运迷宫
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

	class SCRAFortuneMazePinTuInfo		//!<3618 随机活动-幸运迷宫 拼图
	{
	public:
		SCRAFortuneMazePinTuInfo();
		MessageHeader header;
		int mark_index;
		int m_pin_tu[PIN_TU_TYPE_NUM][PIN_TU_INDEX_NUM];//!< PIN_TU_TYPE_NUM = 7,PIN_TU_INDEX_NUM = 9
	};

	class SCRAYuanQiTurningInfo		//!<3084 随机活动-元气翻牌
	{
	public:
		SCRAYuanQiTurningInfo();
		MessageHeader header;
		
		int open_index;//记录你的开的卡
		int info_id;//记录卡牌id
		int reward_id;//奖励id
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 回归豪礼
	class CSBackRewardReq					//3106 随机活动-回归豪礼 请求
	{
	public:
		CSBackRewardReq();
		MessageHeader header;

		int type;
		int param;
	};

	class SCBackRewardInfo					//3105 随机活动-回归豪礼 个人信息
	{
	public:
		SCBackRewardInfo();
		MessageHeader header;

		unsigned int huigui_timestamp;
		char day_flag[MAX_RA_BACK_REWARD_DAY];
		RABackRewardTaskData task_list[MAX_RA_BACK_REWARD_TASK];
	};

	class SCRAHappyLookInfo		//!<3065 随机活动-连连乐翻天
	{
	public:
		SCRAHappyLookInfo();
		MessageHeader header;

		short ra_frequency_num;										// 已挑战次数

		char challenge_index;										// 挑战index
		char challenge_is_succ;										// 是否挑战成功
		int challenge_end_timestamp;								// 挑战失败时间戳
	};

	class SCRASignHappyInfo		//!<3066 随机活动-登录送好礼
	{
	public:
		SCRASignHappyInfo();
		MessageHeader header;

		int login_day;
		int fetch_mark;
	};

	class SCRAWanLingGongYingInfo		//!<3067 随机活动-万灵共盈
	{
	public:
		SCRAWanLingGongYingInfo();
		MessageHeader header;

		unsigned int ra_buy_reward_timestamp;
		RAWLGYTaskData task_list[RA_WAN_LING_GONG_YING_TASK_MAX];
	};

	class SCRAResultNotice :public IMessageSerializer				//!< 1508 活动结果通知下发
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
			RAND_ACTIVITY_RESULT_NOTICE_TYPE_BATTLE_WIN = 1,		//战斗胜利
			RAND_ACTIVITY_RESULT_NOTICE_TYPE_BATTLE_FAIL = 2,		//战斗失败
			RAND_ACTIVITY_RESULT_NOTICE_TYPE_GET_REWARD = 3,		//活动奖励

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
			unsigned int	param_length;		//!< 净值参数的数据长度 （当has_param不为0时可读）
			NetValueItemParamData param_data;	//!< 净值参数的数据，根据param_length的值来读取 （当has_param为1时可读）
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
	

	//随机活动 山海聚宝
	//操作类型
	enum RAShanHaiJuBaoReqType
	{
		RA_SHAN_HAI_JU_BAO_REQ_TYPE_DRAW = 0,		//抽奖 p_1:活动阶段(phase_seq)
		RA_SHAN_HAI_JU_BAO_REQ_TYPE_FETCH_EXTRA = 1,//领取额外奖励 p_1:活动阶段(phase_seq) p_2:额外奖励index
	};
	//角色信息		4177
	class SCRAShanHaiJuBaoInfo
	{
	public:
		SCRAShanHaiJuBaoInfo();
		MessageHeader header;

		struct PhaseInfo
		{
			int draw_times;	//总抽取次数
			int extra_reward_flag;// 额外奖励领取标记
	};

		PhaseInfo phase_info_[SHAN_HAI_JU_BAO_EXTRA_PHASE_MAX];//阶段信息[活动阶段 - 1]
	};
	//抽奖返回		4178
	class SCRAShanHaiJuBaoDrawRet
	{
	public:
		SCRAShanHaiJuBaoDrawRet();
		MessageHeader header;

		int show_type;	//奖励信息
	};

	//随机活动 山海初遇
	//操作类型
	enum RAShanHaiMetReqType
	{
		RA_SHAN_HAI_MET_REQ_TYPE_FETCH_DAILY_REWARD = 0,//领取奖励 p_1:活动天数index p_2:领取类型
		RA_SHAN_HAI_MET_REQ_TYPE_CMD_BUY = 1,			//直购检测 
	};
	//角色信息		4185
	class SCRAShanHaiMetBase
	{
	public:
		SCRAShanHaiMetBase();
		MessageHeader header;

		int act_extra_rewaed_flag;	//额外奖励激活标记

		int day_task_finish_flag;	//任务完成标记(只在领取和跨天时校验)
		int fetch_reward_flag;		//奖励发放标记[活动天数index]
		int fetch_extra_rewaed_flag;//额外奖励发放标记[活动天数index]
	};
	//任务信息		4186
	class SCRAShanHaiMetTakeParam
	{
	public:
		SCRAShanHaiMetTakeParam();
		MessageHeader header;

		int task_param_count;
		//任务参数[任务类型]
		int task_param_list[RAShanHaiMet::TSAK_TYPE_MAX];
	};
	//任务改变		4187
	class SCRAShanHaiMetTaskParamChange
	{
	public:
		SCRAShanHaiMetTaskParamChange();
		MessageHeader header;

		int task_type;	//任务类型
		int task_param;	//任务参数
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 招财进宝
	struct SCRAZhaoCaiJinBaoRoleInfo			// 3112
	{
	public:
		SCRAZhaoCaiJinBaoRoleInfo();
		MessageHeader header;

		unsigned int next_refresh_timestamp;						// 下次刷新时间戳

		int history_chongzhi;										// 活动累计充值金额
		int day_chongzhi;											// 活动每日累积充值金额
		short chongzhi_time[MAX_RA_ZHAOCAIJINBAO_CHONGZHI_NUM];		// 单笔充值可领次数

		short pool[MAX_RA_ZHAOCAIJINBAO_REFRESH_NUM];				// 奖池 seq
		short buy_list[MAX_RA_ZHAOCAIJINBAO_BUY_NUM];				// 购买次数
		short chongzhi_list[MAX_RA_ZHAOCAIJINBAO_CHONGZHI_NUM];		// 充值奖励次数
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
	//////随机活动 缤纷加倍
	//操作类型
	enum RAColorfulDoubleReqType
	{
		RA_COLORFUL_DOUBLE_REQ_TYPE_FETCH_FREE_REWARD = 0,	//领取免费奖励
		RA_COLORFUL_DOUBLE_REQ_TYPE_ACT_BOX = 1,			//激活宝箱奖励 P_1: 宝箱seq 0:激活全部
		RA_COLORFUL_DOUBLE_REQ_TYPE_FETCH_BOX_REWARD = 2,	//领取宝箱奖励 p_1: 宝箱seq
	};

	//角色信息
	class SCRAColorfulDoubleBase
	{
	public:
		SCRAColorfulDoubleBase();
		MessageHeader header;

		int today_free_fetch_flag;		//今日免费奖励领取标记

		int act_box_rewaed_flag;		//宝箱奖励激活标记
		int fetch_box_rewaed_flag;		//宝箱奖励发放标记[宝箱seq]

		int act_task_finish_num;		//活动任务完成数
	};

	//任务信息
	class SCRAColorfulDoubleTakeParam
	{
	public:
		SCRAColorfulDoubleTakeParam();
		MessageHeader header;

		int task_param_count;
		//任务参数[任务类型]
		int task_param_list[RAColorfulDouble::TSAK_TYPE_MAX];
	};

	//任务改变
	class SCRAColorfulDoubleTaskParamChange
	{
	public:
		SCRAColorfulDoubleTaskParamChange();
		MessageHeader header;

		int task_type;	//任务类型
		int task_param;	//任务参数
	};

	////////////////////////////////////////////////////
	//////再战云泽
	//操作类型
	enum RAZaiZhanYunZeReqType
	{
		RA_ZAI_ZHAN_YUN_ZE_REQ_TYPE_BOSS_BATTLE = 0,		//挑战boss p_1:boss_index
	};

	//角色信息  4190
	class SCRAZaiZhanYunZeBase
	{
	public:
		SCRAZaiZhanYunZeBase();
		MessageHeader header;

		//boss挑战通关次数[boss_index]
		int boss_info_count;
		int boss_pass_num[RAZaiZhanYunZe::BOSS_MAX];
	};

	////////////////////////////////////////////////////
	// 魔族在哪里
	class SCRAMoZuZaiNaLiInfo					//3116 随机活动-魔族在哪里
	{
	public:
		SCRAMoZuZaiNaLiInfo();
		MessageHeader header;

		int day_times;
	};

	////////////////////////////////////////////////////
	// 超值纷享
	class SCRAChaoZhiFenXiangInfo				//3117 随机活动-超值纷享
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
	// 山海花灯
	enum RA_SHANHAI_HUADENG_REQ_TYPE	//由协议3000
	{
		RA_SHANHAI_HUADENG_REQ_TYPE_START_GAME = 0,					// 开始游戏
		RA_SHANHAI_HUADENG_REQ_TYPE_CATCH_LANTERN = 1,				// 捕抓花灯		param1:type
		RA_SHANHAI_HUADENG_REQ_TYPE_FETCH_REWARD = 2,				// 领取任务奖励	param1:task_id
		RA_SHANHAI_HUADENG_REQ_TYPE_FORCE_CLOSE = 3,				// 强制关闭
		RA_SHANHAI_HUADENG_REQ_TYPE_FLOW_AWAY = 4,					// 花灯流走		param1:type

		RA_SHANHAI_HUADENG_REQ_TYPE_MAX
	};

	class SCRAShanHaiHuaDengAllInfo		//!< 3123 随机活动-多彩花灯信息下发
	{
	public:
		SCRAShanHaiHuaDengAllInfo();
		MessageHeader header;

		short last_game_score_count;								//上一场游戏获得的积分
		short day_single_score_count;								//今天单次积分最大值
		short activity_single_score_count;							//活动内单次积分最大值
		short day_join_times;										//今日参与次数
		short activity_join_times;									//活动内参与次数
		short day_score_count;										//今日获得的总分
		int activity_score_count;									//活动内获得的总积分
		BitMap<MAX_RA_SHANHAI_HUADENG_TASK_NUM> m_task_list_flag;	//任务列表标识 0:未完成 1:已完成
	};

	enum RA_SHANHAI_HUADENG_GAME_NOTICE_TYPE
	{
		RA_SHANHAI_HUADENG_GAME_NOTICE_TYPE_START_GAME = 0,				//游戏开始
		RA_SHANHAI_HUADENG_GAME_NOTICE_TYPE_IN_THE_GAME = 1,			//游戏进行中
		RA_SHANHAI_HUADENG_GAME_NOTICE_TYPE_GAME_OVER = 2,				//游戏结束下发结算

		RA_SHANHAI_HUADENG_GAME_NOTICE_TYPE_MAX
	};

	class SCRAShanHaiHuaDengGameInfo	//!< 3124 随机活动-多彩花灯游戏信息下发
	{
	public:
		SCRAShanHaiHuaDengGameInfo();
		MessageHeader header;

		short notice_type;							//RA_SHANHAI_HUADENG_GAME_NOTICE_TYPE_MAX
		short single_score_count;					//本次游戏内的积分
		unsigned int game_end_timestamp;			//本次游戏结束时间戳
	};

	class SCRAShanHaiHuaDengLightInfo	//!< 3125 随机活动-多彩花灯花灯信息下发
	{
	public:
		SCRAShanHaiHuaDengLightInfo();
		MessageHeader header;

		short light_list[MAX_RA_SHANHAI_HUADENG_LIGHT_TYPE_NUM];		//花灯数量  MAX_RA_SHANHAI_HUADENG_LIGHT_TYPE_NUM  -- 10
	};

	////////////////////////////////////////////////////
	// 吉星高照
	struct SCRAJiXingGaoZhaoRoleInfo					// 3127 随机活动-吉星高照 角色信息
	{
	public:
		SCRAJiXingGaoZhaoRoleInfo();
		MessageHeader header;

		int draw_times;
		int draw_reward_fetch_mark;
		short shop_list[MAX_RA_JIXING_GAOZHAO_SHOP_INDEX];
	};

	struct SCRAJiXingGaoZhaoPoolInfo					// 3128 随机活动-吉星高照 奖池信息
	{
	public:
		SCRAJiXingGaoZhaoPoolInfo();
		MessageHeader header;

		unsigned short item_id;
		short re_sh;
		unsigned int num;
	};

	struct SCRAJiXingGaoZhaoDrawResult					// 3129 随机活动-吉星高照 抽奖返回
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
	// 周末礼包
	enum RAWeekendGiftReqType
	{
		RA_WEEKEND_GIFT_REQ_TYPE_CMD_BUY = 0,	//直购: p_1: 礼包索引
	};
	// 角色信息 4335
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
			int gift_index;		//礼包索引
			int buy_times;		//已购买次数

			unsigned int out_time_s;//失效时间
		};
		GiftInfo gift_list[RAWeekendGift::ROLE_GIFT_MAX];
	};

	///////////////////////////////////////////////////////////
	// 超值献礼
	enum RAChaoZhiGifttReqType
	{
		RA_CHAO_ZHI_GIFT_REQ_TYPE_CMD_BUY = 0,	//直购
		RA_CHAO_ZHI_GIFT_REQ_TYPE_FETCH = 1,	//领取	p_1: gift_index
	};
	//角色信息
	class SCRAChaoZhiGiftInfo
	{
	public:
		SCRAChaoZhiGiftInfo();
		MessageHeader header;

		int gift_act_flag;		//礼包激活标志

		int count;
		//领取数量[gift_index] 0: 未领取
		int gift_num_list[RAChaoZhiGift::GIFT_MAX];
	};

	////////////////////////////////////////////////////
	// 寅虎问世
	struct SCRAYinHuWenShiInfo					// 3139 随机活动-寅虎问世 角色信息
	{
	public:
		SCRAYinHuWenShiInfo();
		MessageHeader header;

		short zhigou_flag;
		short reward_fetch_mark;

		short reward_type[RA_YINHU_WENSHI_DAY_MAX];
	};

	////////////////////////////////////////////////////
	// 辞牛迎新虎
	struct SCRACiNiuYingXinHuInfo					// 3142 随机活动-辞牛迎新虎 角色信息
	{
	public:
		SCRACiNiuYingXinHuInfo();
		MessageHeader header;

		short awaken_mark;
		short awaken_reward_time;
		RACiNiuYinXinHuTypeData awaken_param[RA_CINIU_YINGXINHU_TYPE_MAX];
	};


	////////////////////////////////////////////////////
	// 虎行大运
	struct SCRAHuXingDaYunInfo					// 3144 随机活动-虎行大运 角色信息
	{
	public:
		SCRAHuXingDaYunInfo();
		MessageHeader header;

		unsigned int complete_time;					// 游戏完成时间戳	
		short task_pos_id;								// 当前任务点
		short task_first_pass;							// 是否首通
		char is_reward;									// 是否走到任务点 可以领取奖励

		char is_be_invited_reward;						// 被邀请奖励是否可领取
		short be_invited_times;							// 被邀请次数
		unsigned int be_invited_complete_time;			// 被邀请 完成时间戳
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
	// 寅虎拜岁
	struct SCRAYinHuBaiSuiInfo					// 3143 随机活动-寅虎拜岁 角色信息
	{
	public:
		SCRAYinHuBaiSuiInfo();
		MessageHeader header;

		short zhigou_flag;
		short reward_fetch_mark;;
	};

	///////////////////////////////////////////////////////////
	// 情缘时装
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
	// 情缘累充
	class SCRAQingYuanLeiChongInfo			// 4426
	{
	public:
		SCRAQingYuanLeiChongInfo();
		MessageHeader header;

		unsigned int join_begin_timestamp;			// 参与开始时间戳
		unsigned int join_end_timestamp;			// 参与结束时间戳

		int chongzhi_gold;							// 充值金额
		int fetch_mark;								// 领取标记
	};

	////////////////////////////////////////////////////
	// 祈愿孔明灯

	struct SCRAQiYuanKongMingDengInfo					// 3156 随机活动-祈愿孔明灯 角色信息
	{
	public:
		SCRAQiYuanKongMingDengInfo();
		MessageHeader header;

		int huozhong;
		int complete_flag;							// 任务完成情况 按位标记
		unsigned short task_list[MAX_RA_QIYUAN_KONGMINGDENG_TASK_COUNT];		// 任务进度
	};

	////////////////////////////////////////////////////
	// 寻梦蝴蝶
	struct SCRAXunMengHuDieInfo					// 3157 随机活动-寻梦蝴蝶 角色信息
	{
	public:
		SCRAXunMengHuDieInfo();
		MessageHeader header;

		int frequency_num;
	};

	////////////////////////////////////////////////////
	// 播种有礼
	struct SCRABoZhongYouLiInfo					// 3159 随机活动-播种有礼 角色信息
	{
	public:
		SCRABoZhongYouLiInfo();
		MessageHeader header;

		int sowing_count;		// 剩余可播种次数
		unsigned int next_refresh_timestamp;	// 浇水下次刷新时间戳
		unsigned int planting_timestamp[BO_ZHONG_YOU_LI_MAX_PLANT_NUM];		// 植物种植时间戳
	};

	////////////////////////////////////////////////////
	// 元气翻牌
	struct SCRAYuanQiFanPaiInfo					// 3161 随机活动-元气翻牌 角色信息
	{
	public:
		SCRAYuanQiFanPaiInfo();
		MessageHeader header;

		int lottery_count;				// 翻牌次数
		BitMap<MAX_YUAN_QI_FAN_PAI_BITMAP> is_receive_times;	// 是否已领取翻牌次数奖励
		int is_fetch_day_reward;
	};

	////////////////////////////////////////////////////
	// 万灵筑礼
	struct SCRAWanLingZhuLiInfo					// 3164 随机活动-万灵筑礼 角色信息
	{
	public:
		SCRAWanLingZhuLiInfo();
		MessageHeader header;

		short login_day;
		short fetch_reward_mark;				// 奖励领取标记
		char day_reward_mark;					// 每日领取标记
		char is_buy_jiagou;						// 是否加购
		short re_sh;
	};

	////////////////////////////////////////////////////
	// 包子来了
	struct SCRABaoZiLaiLeInfo					//3165 随机活动-包子来了
	{
	public:
		SCRABaoZiLaiLeInfo();
		MessageHeader header;

		int join_count;			//今日已参与次数
	};

	////////////////////////////////////////////////////
	// 元气记事簿
	struct SCRAYuanQiJiShiBuInfo					// 3166 随机活动-元气记事簿 角色信息
	{
	public:
		SCRAYuanQiJiShiBuInfo();
		MessageHeader header;

		short combat;							// 获得的劳作值, 不清空
		char is_jinjie_jiangli;					// 是否购买进阶奖励
		char re_ch;

		int quanming_reward_fetch_flag;			// 免费奖励	
		int jinjie_reward_fetch_flag;			// 进阶奖励
		int complete_flag;					// 任务完成情况 按位标记
		unsigned short task_list[MAX_RA_YUAN_QI_JISHIBU_TASK_COUNT];		// 任务进度
	};

	////////////////////////////////////////////////////
	// 乘风破浪
	struct SCRAChengFengPoLangOpenInfo					//3167 随机活动-乘风破浪开启信息下发
	{
	public:
		SCRAChengFengPoLangOpenInfo();
		MessageHeader header;

		int is_open;			//活动是否在开启时间内
	};

	struct SCRAChengFengPoLangInfo					//3168 随机活动-乘风破浪
	{
	public:
		SCRAChengFengPoLangInfo();
		MessageHeader header;

		int answer_times;		// 今日已答题次数
		int cur_question_id;	// 当前题目id
	};

	struct SCRAMiaoBiShengHuaInfo					//!<3170  随机活动-妙笔生花
	{
	public:
		SCRAMiaoBiShengHuaInfo();
		MessageHeader header;

		int count;	
	};

	struct SCRAZongXiangManChengInfo//!< 3180 随机活动粽香满城下发
	{
	public:
		SCRAZongXiangManChengInfo();
		MessageHeader header;

		int light_type;
		BitMap<MAX_ZONG_XIANG_MAN_CHENG_FLAG> m_flag_map;
	};

	struct SCRASaltySweetBattleInfo//!< 3181 随机活动咸甜之争下发
	{
	public:
		SCRASaltySweetBattleInfo();
		MessageHeader header;

		int salty_value;//咸值
		int sweet_value;//甜值
		int status_mod;//游戏状态
	};

	struct SCRASaltySweetBattleRoleInfo//!< 3182 随机活动咸甜之争个人下发
	{
	public:
		SCRASaltySweetBattleRoleInfo();
		MessageHeader header;

		int vote_type;//玩家选择类型【0，无选择，1咸，2甜】
		int reward_flag;
	};

	struct SCRALuckyBlindBoxInfo//!< 3185 随机活动幸运盲盒下发
	{
	public:
		SCRALuckyBlindBoxInfo();
		MessageHeader header;

		long long chongzhi;
		char box_flag[MAX_LUCKY_BLIND_BOX_X][MAX_LUCKY_BLIND_BOX_Y];
		short today_shop_buy_times[MAX_LUCKY_BLIND_BOX_SEQ];//商城今日购买次数
		short shop_buy_times[MAX_LUCKY_BLIND_BOX_SEQ];//商城活动累积购买次数

		short today_recharge_buy_times[MAX_LUCKY_BLIND_BOX_SEQ];//成就今日领取次数
		short recharge_buy_times[MAX_LUCKY_BLIND_BOX_SEQ];//成就活动领取次数
	};

	////////////////////////////////////////////////////
	// 300抽皮肤
	struct SCRASanBaiChouInfo					// 4805 随机活动-三百抽 角色信息
	{
	public:
		SCRASanBaiChouInfo();
		MessageHeader header;

		int draw_times;												// 总共抽奖次数
		int reward_item_seq;										// 最终选择的奖励

		RASanBaiChouTypeParam select[MAX_RA_SANBAI_CHOU_TYPE_NUM];		// 类型参数
	};

	struct SCRASanBaiChouDrawInfo				// 4806 随机活动-三百抽 抽奖返回
	{
	public:
		SCRASanBaiChouDrawInfo();
		MessageHeader header;

		short type;
		short select_req;
		int type_times;											
		int draw_times;												// 总共抽奖次数
		short pool[MAX_RA_SANBAI_CHOU_DRAW_NUM];
	};
}

#pragma pack(pop)
#endif	// __MSG_RAND_ACTIVITY_HPP__