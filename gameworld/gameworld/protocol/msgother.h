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
	class SCBuffInfo					//!< 获得buff时下发 2480
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

		int					wst_questions_id;								//!< 题目id
		char				wst_question_today_buy_exp_count;				//!< 能购买的今日奖励次数
		char				wst_question_yesterday_buy_exp_count;			//!< 能购买的昨日奖励次数
		unsigned short		wst_answer_num;									//!< 今天的答题数量
	};

	class CSAnswerOp					//!< 1961 
	{
	public:
		CSAnswerOp();
		MessageHeader		header;
		enum OP_TYPE
		{
			OP_TYPE_GET_INFO = 0,			//!< 获取信息
			OP_TYPE_QUESTION,				//!< 回答题目	 param1答案id[1,4]
			OP_TYPE_BUY_EXP,				//!< 购买经验 param1 = 0 为购买今天的 param1 = 1 为购买昨天的
			OP_TYPE_GET_WST_INFO,			//!< 请求下发答题信息
		};

		int op_type;
		int param1;							// 0 - 购买类型  1 - 答案id
	};

	class SCAnswerOverInfo					//!< 1962
	{
	public:
		SCAnswerOverInfo();
		MessageHeader		header;

		long long add_coin;
		long long add_exp;
	};

	class CSExpCrystalOp					//!< 2001 打卡水晶的开关
	{
	public:
		CSExpCrystalOp();
		MessageHeader		header;
		
	};

	class SCExpCrystalInfo					//!< 2002 打卡水晶的信息
	{
	public:
		SCExpCrystalInfo();
		MessageHeader		header;

		short is_open;				//!< 0为未开启
		short count;					//!< 剩余次数
	};

	class CSTreviFountainOp					//!< 2003 许愿池 许愿
	{
	public:
		CSTreviFountainOp();
		MessageHeader		header;

	};

	class SCTreviFountainInfo				//!< 2004 许愿池信息
	{
	public:
		SCTreviFountainInfo();
		MessageHeader		header;

		struct buff_info
		{
			buff_info():trevi_fountain_buff_id(0), trevi_fountain_buff_expiration_time(0)
			{}

			unsigned int		trevi_fountain_buff_id;						//!< 许愿池获得的buff_id
			unsigned int		trevi_fountain_buff_expiration_time;		//!< 许愿池buff的到期时间
		};

		int count;					//!< 许愿池今日使用次数
		buff_info send_info[MAX_TREVI_FOUNTION_TODAY_BUFF_NUM];
	};

	class SCOnlineRewardInfo				//!< 1970 在线奖励信息
	{
	public:
		SCOnlineRewardInfo();
		MessageHeader		header;

		unsigned int	today_online_time;		//!< 今日在线时长
		int				reward_fetch_flag;		//!< 奖励领取flag
	};

	class SCSignRewardInfo				//!< 1971 签到奖励信息
	{
	public:
		SCSignRewardInfo();
		MessageHeader		header;

		int				sign_count;					//!< 签到次数
		int				reward_fetch_flag;			//!< 奖励领取flag
		int             month_zero_open_day;		//!< 当月1号0点计算的开服天数
	};

	class SCLevelRewardInfo				//!< 1972 等级奖励信息
	{
	public:
		SCLevelRewardInfo();
		MessageHeader		header;

		int				reward_fetch_flag;			//!< 奖励领取flag
	};

	class CSFetchRewardReq				//!< 1973 领取奖励
	{
	public:
		CSFetchRewardReq();
		MessageHeader		header;

		enum REWARD_TYPE
		{
			ONLINE_REWARD = 0,		//!< 在线奖励
			SIGN_REWARD = 1,		//!< 签到奖励 seq 发 day
			LEVEL_REWARD = 2,		//!< 等级奖励
			SCORE_REWARD = 3,		//!< 评分奖励
			MONTH_INVESTMENT = 4,	//!< 月度投资签到
			FETCH_DAY_FOOD = 5,		//!< 领取午餐晚餐
			FETCH_ONLINE_GIFT = 6,	//!< 领取在线礼包
		};

		int reward_type;
		int seq;
	};

	class CSFindRewardReq				//!< 1974 找回奖励
	{
	public:
		CSFindRewardReq();
		MessageHeader		header;

		int type;						//!< 活动类型
		int consume_type;				//!< 消耗类型 0为铜币 1为元宝
		int count;						//!< 找回次数
	};

	class SCFindRewardInfo				//!< 1975 找回奖励信息
	{
	public:
		SCFindRewardInfo();
		MessageHeader		header;
		
		BitMap<DAY_ACTIVITY_TYPE_MAX> find_reward_flag;	//!< 找回标识
		int can_find_count[DAY_ACTIVITY_TYPE_MAX];		//!< 能找回的次数
		int today_find_count[DAY_ACTIVITY_TYPE_MAX];	//!< 今天已找回的次数
	};

	UNSTD_STATIC_CHECK(DAY_ACTIVITY_TYPE_MAX == 20 && MAX_DAY_ACTIVITY_TYPE_NUM <= 32);

	class CSRewardFindNotifyCancelReq     //1978 请求取消今天红点提示
	{
	public:
		CSRewardFindNotifyCancelReq();		
		MessageHeader header;

	};

	class SCRewardFindNotifys			//!< 1979 奖励界面找回功能红点下发
	{
	public:
		SCRewardFindNotifys();		
		MessageHeader header;

		int notifys;						// 0 玩家尚未打开界面,提示红点   1 玩家今天已经打开过界面,取消红点
	};

	class SCDayRewardOnlineGiftInfo		//!< 1990 奖励界面-在线礼包信息下发
	{
	public:
		SCDayRewardOnlineGiftInfo();
		MessageHeader header;

		unsigned int online_gift_record_begin_timestamp;	// 开始记录在线礼包时间戳
		unsigned int online_gift_times;						// 累计在线时长　　总累计时间为：(now-online_gift_record_begin_timestamp +online_gift_times)
		BitMap<DAY_REWARD_ONLINE_GIFT_NUM> online_gift_flag;// 在线礼包领取标识
	};


	class SCDayRewardDayFoodInfo		//!< 1127 午餐晚餐信息下发
	{
	public:
		SCDayRewardDayFoodInfo();
		MessageHeader header;

		BitMap<DAY_REWARD_DAY_FOOD_SEQ_COUNT> day_food_flag;		//午餐晚餐领取标识	
	};

	class SCPetStrengthenRewardInfo							// 宠物强化奖励信息下发
	{
	public:
		SCPetStrengthenRewardInfo();
		MessageHeader header;

		int pet_strengthen_lv;										//宠物最高强化等级
		BitMap<PET_STRENGTHEN_REWARD_FLAG> has_pet_strengthen_flag;			//宠物强化奖励是否已领取
	};

	class CSPetStrengthenRewardReq											//宠物强化奖励请求
	{
	public:
		CSPetStrengthenRewardReq();
		MessageHeader header;

		int seq;
	};

	///////////////////////////////////////// 功能预告 /////////////////////////////////////////
	class CSSystemNoticeGetAward
	{
	public:
		CSSystemNoticeGetAward();		//!< 1750 - 功能预告领取奖励
		MessageHeader header;

		int type_seq;				//!< 序号
	};

	class SCSystemNoticeFlagInfo
	{
	public:
		SCSystemNoticeFlagInfo();		//!< 1751 - 功能预告领取信息
		MessageHeader header;

		int fetch_flag;				//!< 二进制码，奖励领取状态
	};

	class CSChangeAppearance			//!< 2034 - 改变形象
	{
	public:
		CSChangeAppearance();
		MessageHeader header;
		
		short				avartar_type;		//!< 虚拟人物类型
		short				color;				//!< 颜色
	};
	
	//宝箱通知
	class SCNoticeNearbyOnOpenTreasureChest		//!< 2040 通知附近的玩家 宝箱被打开
	{
	public:
		SCNoticeNearbyOnOpenTreasureChest();
		MessageHeader header;

		ObjID obj_id;			//宝箱objid
		short reserved;
	};

	//宝箱奖励下发（个人）
	class SCTreasureChestRewardList		//!< 2039 通知玩家宝箱获得了什么奖励
	{
	public:
		const static int MAX_TREASURE_CHEST_COUNT = 10;//!< 宝箱掉落最大种类
		struct reward_info
		{
			reward_info():item_id(0),num(0),is_bind(0){}
			int item_id;
			short num;
			short is_bind;
		};
		SCTreasureChestRewardList();
		MessageHeader header;

		ObjID obj_id;			//宝箱objid
		short reward_count;
		int add_coin;
		reward_info reward_list[MAX_TREASURE_CHEST_COUNT];
	};

	class SCNoticeCfgUpdate : public IMessageSerializer  // 2041 公告热更通知
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
		
		int notice_id;			//!< 公告ID 
		int is_show;			//!< 是否显示 [0:不显示  1:显示]

		int title_len;			//!< 标题长度
		NoticeTitle title;		//!< 标题

		int subject_len;		//!< 主题长度
		NoticeSubject subject;	//!< 主题
			
		int content_len;		//!< 内容长度
		NoticeContent content;	//!< 内容
	};


	class CSRoleSubscribe		//!< 1057 订阅/取消订阅系统信息
	{
	public:
		CSRoleSubscribe();
		MessageHeader header;

		short is_unsubscribe;	//!< 0: 订阅   1:取消订阅
		short topic;			//!< 订阅的主题  \see TOPIC_TYPE
	};

	class CSRoleSalaryInfo		//!< 2012 请求玩家工资信息
	{
	public:
		CSRoleSalaryInfo();
		MessageHeader header;

		int salary_type;		//!< 0:队长工资  1:侠义值
	};

	class SCRoleSalaryInfo		//!< 2013 玩家工资信息返回
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
			REASON_TYPE_NORMAL = 0,	//!< 0 下发整个列表
			REASON_TYPE_UPDATE = 1, //!< 1 更新
		};

		char salary_type;			//!< 0:队长工资  1:侠义值
		char reason;				//!< 下发原因 \see REASON_TYPE
		short salary_list_size;		//!< 列表长度
		long long total_value;		//!< 总值
		SalaryInfo salary_list[MAX_SALARY_INFO_NUM];	//!< 列表信息 \see SalaryInfo
	};
	
	class CSSpeaker		//!< 2014 请求使用喇叭
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

	class SCSpeaker		//!< 2015  喇叭语句下发
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

	class SCConvertShopBuyRecord	//!< 1851 商城（兑换商店）购买记录汇总 
	{
	public:
		SCConvertShopBuyRecord();
		MessageHeader header;

		struct RecordInfo
		{
			ItemID item_id;			//!< 物品ID
			short convert_type;		//!< 兑换类型
			short buy_times;		//!< 购买次数
			short seq;				//!< 用于唯一对应
		};

		int record_num;				//!< 购买记录数量， 根据数量读record_list
		RecordInfo record_list[CONVERT_TYPE_MAX_ITEM_COUNT * CONVERT_TYPE_MAX];
	};

	class SCConvertShopAddRecord	//!< 1852 增加一条购买记录
	{
	public:
		SCConvertShopAddRecord();
		MessageHeader header;

		struct RecordInfo
		{
			ItemID item_id;			//!< 物品ID
			short convert_type;		//!< 兑换类型
			short buy_times;		//!< 购买次数
			short seq;				//!< 用于唯一对应
		};

		RecordInfo info;			//!< 购买记录
	};

	class CSConvertShopBuyReq	//!< 1850 购买协议 
	{
	public:
		CSConvertShopBuyReq();
		MessageHeader header;

		short convert_type;		//!< 兑换商店类型
		short seq;				//!< 物品的seq
		ItemID item_id;			//!< 物品ID（用作检验，防止热更后客户端和服务端看到的物品不一样的情况） 
		short buy_num;			//!< 购买数量
	};
	class SCRoleSalaryDay			//!< 2045 每天获得的侠义值和工资数值
	{
	public:
		SCRoleSalaryDay();
		MessageHeader header;

		int salary_type;			//!< 0:队长工资  1:侠义值
		int day_value;				//1< 每天的侠义值或工资 
	};

	class SCRoleHistoryChivalrous	//!< 2066 历史侠义值下发
	{
	public:
		SCRoleHistoryChivalrous();
		MessageHeader header;

		int history_chivalrous;		//!< 历史累计获得的侠义值
		int chivalrous_reward_flag;	//!< 已领取奖励标记（二进制）
	};

	class CSRoleHistoryChivalrousRewardFetch	//!< 2067 历史侠义值奖励领取请求
	{
	public:
		CSRoleHistoryChivalrousRewardFetch();
		MessageHeader header;

		int fetch_seq; // 领取的成就奖励seq
	};
	
	class SCLoginSystemInfo			//!< 2046 累计登陆下发
	{
	public:
		SCLoginSystemInfo();
		MessageHeader header;

		int login_day;				//!< 累计登陆天数
		BitMap<MAX_LOGIN_SYSTEM_NUM> is_reward_flag;	// 是否已领取奖励
	};

	class CSLoginSystemREQ			//!< 2047 累计登陆请求
	{
	public:
		CSLoginSystemREQ();
		MessageHeader header;

		int day;					//!< 领取的登陆天数
	};

	class SCSpecialNotice			//!< 2048 特殊信息系统栏通知
	{
	public:
		SCSpecialNotice();
		MessageHeader header;

		enum SPECIAL_NOTICE_TYPE
		{	
			SPECIAL_NOTICE_TYPE_CAPTAIN_SALARY = 0,				// 队长工资   param1 为此时队长工资数量
			SPECIAL_NOTICE_TYPE_GUILD_GATHER_DREAM_GIFT = 1,	// 家族集愿   param1 家族资金 param2 铜币 param3 家族贡献 param4 道具ID param5 道具数量

			SPECIAL_NOTICE_TYPE_MAX
		};

		int op_type;
		int param1;
		int param2;
		int param3;
		int param4;
		int param5;
	};

	class CSMysteriousTrialReq         //!<2250 神秘试炼信息请求
	{
	public:
		CSMysteriousTrialReq();
		MessageHeader header;

		enum Mysterious_trial_type
		{
			MYSTERIOUS_TRIAL_INFO_TYPE = 0,				//神秘试炼信息下发
			MYSTERIOUS_TRIAL_REWARD_TYPE,				//领取七天普通奖励
			MYSTERIOUS_TRIAL_FINALLY_REWATD_TYPE,		//领取最终奖励
		};

		int op_type;
		int param1;
		int param2;
	};

	class SCMysteriousTrialInfo          //!<2251 神秘试炼信息下发(七天活动)
	{
	public:
		SCMysteriousTrialInfo();
		MessageHeader header;

		char open_day;																						//活动天数
		char goal_num;																						//解锁天数			
		short task_num;																						//已完成的任务数量(不包含最终任务)	
		short type;																							//当前阶段 MYSTERIOUS_TRIAL_TYPE	
		short reserve_sh;																					
		BitMap<RA_MYSTERIOUS_TRIAL_FINALLY_MAX_NUM> activity_flag;											//最终任务状态标识 RA_MYSTERIOUS_TRIAL_FINALLY_MAX_NUM---32
		ActivityFlagSchedule  ra_mysterious_trial[RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM][RA_NEW_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM];	//每日任务列表    7---5
	};

	class SCMysteriousTrialSingleInfo          //!<2252 神秘试炼单个信息下发
	{
	public:
		SCMysteriousTrialSingleInfo();
		MessageHeader header;

		char start_time;					//第几天的目标,0为最终任务
		char seq;
		short state;						//状态-标识
		int progress;						//进度
		short target_num;					//已完成的任务数量
		short reserve_sh;					//保留字
	};

	struct SCMysterialFinallyRewardInfo		//!<2253 神秘试炼最终奖励信息下发
	{
	public:
		SCMysterialFinallyRewardInfo();
		MessageHeader header;

		BitMap<RA_MYSTERIOUS_TRIAL_FINALLY_MAX_NUM> activity_flag;		//最终任务状态标识 RA_MYSTERIOUS_TRIAL_FINALLY_MAX_NUM---32
	};

	struct CSTransactionTaskReq                         //2255 购买任务(每日任务) - 丝绸之路请求
	{
	public:
		CSTransactionTaskReq();
		MessageHeader header;

		enum Transaction_Task_Type
		{
			TRANSACTION_TASK_INFO_TYPE = 0,				//请求任务信息
			TRANSACTION_TASK_COMMON_TYPE,				//请求提交领取普通物品组 index  refer_num
			TRANSACTION_TASK_FINALLY_TYPE				//请求领取总奖励
		};

		int op_type;					
		int index;										// 0-8
		int refer_num;									// 提交的道具数量
	};

	struct SCTransactionTaskInfo						//2256 购买任务(每日任务) - 丝绸之路信息下发
	{
	public:
		SCTransactionTaskInfo();
		MessageHeader header;

		int task_id;										//任务ID
		short task_status;									//任务状态  0 :进行中   1:已完成
		short version_reset;								//版本号,不同需重置
		short level;										//领取任务时的等级
		short type;											//一个type代表一个等级组范围 详情请查看配置
		short seq[TRANSACTION_TASK_REWARD_MAX_NUM];			//随机9个对应的配置seq   跟下面对应
		short reserve;										//保留字,对齐
		BitMap<TRANSACTION_TASK_REWARD_MAX_NUM> flag;		//领取标志
	};

	struct CSActiveSystemReq							//2257 活跃系统 - 信息请求
	{
	public:
		CSActiveSystemReq();
		MessageHeader header;

		enum Active_System_Op_Type
		{
			ACTIVE_SYSTEM_INFO_TYPE = 0,					//请求信息下发  无需index
			ACTIVE_SYSTEM_FETCH_REWARD,						//请求领取活跃度宝箱奖励  需要参数index
			ACTIVE_SYSTEM_WEEK_FETCH_REWARD,				//周常- 领取奖励 奖励seq
			ACTIVE_SYSTEM_FETCH_SPECIAL_REWARD,				//领取特殊活跃奖励 index 填activie_special_type
			ACTIVE_SYSTEM_SPECIAL_INFO,						//请求特殊活跃信息
		};

		int op_type;
		int index;										    // 0-3
	};

	struct SCActiveSystemInfo							//2258 活跃系统 - 信息下发
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

		int level;												//玩家当天符合条件时的等级
		int integral_active;									//玩家当前的活跃度
		BitMap<ACTIVE_SYSTEM_LEVEL_GROUP_MAX_NUM> active_flag;	//活跃宝箱领取标志
		int task_count;											// 类型总数
		int progress[ACTIVE_SYSTEM_TYPE_MAX];					//各个类型的进度
		int extra_reward_flag_list[ACTIVE_SYSTEM_TYPE_MAX];		//各个类型的额外奖励领取情况（0:不可领取  1:可领取但未领取  2:已领取)
	};

	struct SCActiveSystemSingleInfo							//2264 活跃系统 - 单个信息下发
	{
	public:
		SCActiveSystemSingleInfo();
		MessageHeader header;

		int level;												//玩家当天符合条件时的等级
		int integral_active;									//玩家当前的活跃度
		BitMap<ACTIVE_SYSTEM_LEVEL_GROUP_MAX_NUM> active_flag;	//活跃宝箱领取标志
		int active_id;											// 类型
		int progress;											//进度
		int extra_reward_flag;									//各个类型的额外奖励领取情况（0:不可领取  1:可领取但未领取  2:已领取)
	};

	struct CSActiveSystemExtraRewardFetch		//!< 2265 领取活跃项目的额外奖励
	{
	public:
		CSActiveSystemExtraRewardFetch();
		MessageHeader header;

		int active_type;						//!< 活跃项目
	};

	struct SCSpecialActiveInfo					//!< 2266 特殊活跃信息下发
	{
	public:
		SCSpecialActiveInfo();
		MessageHeader header;

		BitMap<ACTIVE_SYSTEM_SPECIAL_TYPE_MAX_IN_DB> special_active_reward_flag;	//特殊活跃类型奖励领取标识
	};

	//// 3910 周常 玩家信息
	struct SCWeekActiveDegreeInfo
	{
		SCWeekActiveDegreeInfo();
		MessageHeader header;

		BitMap<BIT_COUNT_OF_LONG_64> week_fetch_flag;	//周-活跃奖励领取标记[奖励index]
		BitMap<BIT_COUNT_OF_LONG_64> week_task_today_finish_flag;//周-每日任务达成标记[任务index]
		int week_active_degree;							//周活跃数
	};


	struct CSOneKeyFinish									// 2259 任务一键完成
	{
		CSOneKeyFinish();
		MessageHeader header;

		enum FinishType
		{
			FINISH_TYPE_RESEARCH_TASK		= 0,					//!< 三界除魔
			FINISH_TYPE_WST_QUESTION		= 1,					//!< 神算子考验
			FINISH_TYPE_BOUNTY_TASK			= 2,					//!< 门派任务
			FINISH_TYPE_FANTASY_FB			= 3,					//!< 四象幻境
			FINISH_TYPE_SILK_ROAD			= 4,					//!< 丝绸之路
			FINISH_TYPE_JIBUDAOZEI			= 5,					//!< 缉捕盗贼
		};

		int op_type;										// 一键完成的任务类型
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

	class SCFuncPreviewInfo		// 功能预告信息	1870
	{
	public:
		SCFuncPreviewInfo();
		MessageHeader header;

		int notice_num;								//!< 预告数量 （按实际数量读取下面的列表）
		short notice_id_list[MAX_FUNC_PREVIEW_NUM];	//!< 领取状态列表，下标表示预告ID，值表示领取状态，取值  0：不可领取  1：可领取  2：已领取
	};

	class CSFuncPreviewClaimReward	//!< 领取预告奖励 1871
	{
	public:
		CSFuncPreviewClaimReward();
		MessageHeader header;

		int notice_id;		//!< 预告ID
	};

	class CSSetAutoCatchPet				//!< 2175 自动抓宠设置
	{
	public:
		CSSetAutoCatchPet();
		MessageHeader header;

		struct AutoCatchSetting
		{
			int monster_id;				//!< 怪物ID
			int max_catch_num;			//!< 最大抓宠数 
		};

		int scene_id;					//!< 目标场景ID
		short max_seal_card_common;		//!< 最大普卡使用量
		short max_seal_card_silver;		//!< 最大银卡使用量
		short max_seal_card_gold;		//!< 最大金卡使用量
		short auto_catch_info_num;		//!< 自动抓宠信息数
		
		AutoCatchSetting setting_list[MAX_AUTO_CATCH_NUM]; //!< 自动抓宠信息
	};

	class SCAutoCatchPetInfo			//!< 2176 自动抓宠信息
	{
	public:
		SCAutoCatchPetInfo();
		MessageHeader header;

		struct AutoCatchInfo
		{
			int monster_id;				//!< 怪物ID
			short max_catch_num;		//!< 最大抓宠数
			short cur_catch_num;		//!< 当前抓宠数
		};

		int scene_id;					//!< 场景ID
		short max_seal_card_common;		//!< 最大普卡使用量
		short max_seal_card_silver;		//!< 最大银卡使用量
		short max_seal_card_gold;		//!< 最大金卡使用量

		short cur_seal_card_common;		//!< 当前普卡使用量
		short cur_seal_card_silver;		//!< 当前银卡使用量
		short cur_seal_card_gold;		//!< 当前金卡使用量

		int auto_catch_info_num;		//!< 自动抓宠信息数
		AutoCatchInfo info_list[MAX_AUTO_CATCH_NUM]; //!< 自动抓宠信息
	};

	class SCRoleGiftGatherDreamList		//!< 2177 玩家今天集愿赠予信息
	{
	public:
		SCRoleGiftGatherDreamList();
		MessageHeader header;

		short today_is_publish_gather_dream_flag;			//今天是否已发布家族集愿	0:未发布 1:已发布
		short count;
		int gift_uild_list[MAX_GATHER_DREAM_GIFT_NUM];		//今天集愿赠予的uid列表  MAX_GATHER_DREAM_GIFT_NUM -- 10
	};

	class SCRoleCanPublishGahterDreamItemList	//!< 2178 玩家可发布的集愿信息列表
	{
	public:
		SCRoleCanPublishGahterDreamItemList();
		MessageHeader header;

		short publish_is_send_flag;				//发布家族集愿是否发送消息到家族频道 0 :默认勾选发送  1:取消勾选,不发送
		short count;
		int pet_id_list[PET_COUNT_MAX];			//可发布的宠物ID列表 PET_COUNT_MAX -- 256
	};

	class SCRoleGatherDreamRecordInfoList			//!< 2179 玩家集愿记录列表
	{
	public:
		SCRoleGatherDreamRecordInfoList();
		MessageHeader header;

		short type;
		short count;
		GatherDreamRecordInfo data_list[MAX_GATHER_DREAM_SEND_LIST_NUM];	//记录列表	MAX_GATHER_DREAM_SEND_LIST_NUM -- 30
	};


	struct XunBaoComsumeInfo
	{
		short col;
		short idx;
		int num;
	};

	class CSXunBaoReq		//!< 2500 寻宝请求
	{
	public:
		CSXunBaoReq();
		MessageHeader header;

		short guide_draw;			//是否抽新手宠物
		short draw_type;
		short draw_times;
		short is_use_free;
		XunBaoComsumeInfo comsume_info[5]; //不用补齐
	};

	class SCXunBaoRet		//!< 2501 寻宝结果
	{
	public:
		SCXunBaoRet();
		MessageHeader header;

		struct NodeItem
		{
			short type;			//0是物品，1是宠物, 2是伙伴
			unsigned short id;
			int num;
		};

		int count;
		NodeItem item_list[50];
	};


	class SCXunBaoOpenInfo	//!< 2502 寻宝请求
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
			int open_day;  //open_day不为0，当前开服时间小于这个这个值才能抽

			int open_type;
			//-------open_type为1时，
			unsigned int start_time;	//当前时间这个时间戳才能抽
			unsigned int end_time;	//当前时间这个时间戳才能抽
			//-------open_type为2时，与限时活动类型一样，周X，X点-X点开启；
			unsigned int week_open_flag;			//周几开启
			unsigned short day_begin_time;			//周几开启每天X点开启
			unsigned short day_end_time;				//周几开启每天x点结束
		};

		int count;
		XunBaoOpenInfo open_info[100];
	};

	class SCXunBaoDrawTimesInfo	//!< 2503 寻宝抽奖励次数
	{
	public:
		SCXunBaoDrawTimesInfo();
		MessageHeader header;

		int draw_times[XUN_BAO_DRAW_CUMULATIVE_TIMES_GROUP_MAX_NUM];  //!< 大小10			抽奖次数	
		unsigned long long first_draw_flag;		//!< 寻宝id 是否获取第一次首抽奖励标志
		unsigned long long second_draw_flag;		//!< 寻宝id 是否获取第二次首抽奖励标志
		unsigned int free_draw_time[20];		//!< 下次免费抽奖时间
		int xi_you_floor_curr_times[20];		//!< 稀有保底抽奖次数
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

		unsigned short comsume_type; //0是材料组1，1是材料组2
		unsigned char type;			//消耗的背包还是仓库 0是背包，1是仓库
		unsigned char index;			//消耗index
	};

	class CSXunBaoExchange		//!< 2504 寻宝兑换
	{
	public:
		CSXunBaoExchange();
		MessageHeader header;


		unsigned short draw_type;		
		unsigned short get_index;
		unsigned short comsume_index1;		//材料组1第几条
		unsigned short comsume_index2;		//材料组2第几条
		XunBaoExchangeComsumeItem comsume_item[100];// 不用补齐100个
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

	class SCHearsayInfo				//!< 2059 传闻提示信息下发
	{
	public:
		SCHearsayInfo();
		MessageHeader header;

		enum HEARSAY_TYPE
		{
			HEARSAY_TYPE_ACTIVE_FACE_SUCC = 0,			//激活颜值资源成功	param1:item_id
			HEARSAY_TYPE_ACTIVE_FACE_FAILD = 1,			//激活颜值资源失败	param2:item_id
		};

		int op_type;
		int param1;
	};


	class CSXunBaoBelssReq			//!<2505 寻宝祈福请求
	{
	public:
		CSXunBaoBelssReq();
		MessageHeader header;

		short index;
		short reserve_sh;
		int pet_id;
	};

	class SCXunBaoBelssInfo			//!<2506 寻宝祈福信息下发
	{
	public:
		SCXunBaoBelssInfo();
		MessageHeader header;

		int xun_bao_bless_pet_id_list[MAX_XUN_BAO_BLESS_PET_NUM];
	};

	class CSLunHuiWangChuanReq			//!<2507 轮回忘川请求
	{
	public:
		CSLunHuiWangChuanReq();
		MessageHeader header;
		enum CS_LHWC_TYPE
		{
			CS_LHWC_TYPE_INFO = 0,		//信息下发
			CS_LHWC_TYPE_BATTLE = 1,	//请求战斗
			CS_LHWC_TYPE_REWARD = 2,	//领取层数奖励 param 对应层数
		};
		int req_type;
		int param;
	};

	class SCLunHuiWangChuanInfo			//!<2508 轮回忘川下发
	{
	public:
		SCLunHuiWangChuanInfo();
		MessageHeader header;

		int start_level;		//参与等级记录
		int battle_monster_id;	//	 战斗怪物组
		unsigned int previous_refresh_tamp;//上一次刷新时间
		BitMap<MAX_LUN_HUI_WANG_CHUAN_INDEX> pass_list;//MAX_LUN_HUI_WANG_CHUAN_INDEX = 24
		BitMap<MAX_LUN_HUI_WANG_CHUAN_INDEX> reward_list;//MAX_LUN_HUI_WANG_CHUAN_INDEX = 24
	};

	class SCNetherWorldPurgatoryInfo			//!<2630 幽冥炼狱下发
	{
	public:
		SCNetherWorldPurgatoryInfo();
		MessageHeader header;

		int pass_challenge_id;
		int pass_challenge_chapters;//通关章节
		int low_key;
		int mid_key;
		int high_key;
		int show_video;
		int today_pass_times;
	};

	class SCNetherWorldPurgatoryChapterInfo			//!<2634 幽冥炼狱单章节下发
	{
	public:
		SCNetherWorldPurgatoryChapterInfo();
		MessageHeader header;

		int chapter_id;
		NetherWorldGatoryChapters chapter_list;//章节
	};

	class CSNetherWorldPurgatoryReq			//!<2631 请求幽冥炼狱
	{
	public:
		CSNetherWorldPurgatoryReq();
		MessageHeader header;
		enum REQ_OP_TYPE
		{
			REQ_OP_TYPE_0 = 0,	//信息下发
			REQ_OP_TYPE_1 = 1,	//请求战斗 param1 对应章节id param2 对应关卡id
			REQ_OP_TYPE_2 = 2,	//领取层数奖励 param1 对应章节id param2 对应关卡id param3 对应任务id
			REQ_OP_TYPE_3 = 3,	//使用密令抽奖 param1 对应章节id param2 密令类型[1-3] param3 抽奖次数
			REQ_OP_TYPE_4 = 4,	//客户端设置是否已经播放过了
		};
		int req_type;
		int param1;
		int param2;
		int param3;
	};

	class SCNetherWorldPurgatorySigleInfo			//!<2632 幽冥炼狱单个下发
	{
	public:
		SCNetherWorldPurgatorySigleInfo();
		MessageHeader header;

		enum SEND_REASON
		{
			SEND_REASON_GET_REWARD = 0,//领奖
			SEND_REASON_FINISH_BATTLE = 1,//战斗
		};

		short send_reason;//下发理由
		char reserve_ch;
		unsigned char new_pass_flag;//二进制任务完成
		int pass_challenge_id;//通关关卡
		int pass_challenge_chapters;//通关章节
		int today_play_times;
		NetherWorldGatoryLevel challenge_info;
	};

	struct NetherWorldGatoryChapterMiLing
	{
		BitMap<MAX_NETHER_WORLD_GATORY_INDEX> silver_reward_flag;
		BitMap<MAX_NETHER_WORLD_GATORY_INDEX> gold_reward_flag;
	};

	class SCNetherWorldPurgatoryMiLingInfo			//!<2633 幽冥炼狱密令下发
	{
	public:
		SCNetherWorldPurgatoryMiLingInfo();
		MessageHeader header;

		int low_key;
		int mid_key;
		int high_key;
		
		NetherWorldGatoryChapterMiLing mi_ling_list[MAX_NETHER_WORLD_GATORY_CHAPTER_NUM];
	};

	class CSRealmBreakReq			//!< 1880 请求境界系统相关操作
	{
	public:
		CSRealmBreakReq();
		MessageHeader header;

		enum REALM_BREAK_OP_TYPE
		{
			REALM_BREAK_OP_TYPE_BREAK = 0,			// 请求突破
			REALM_BREAK_OP_TYPE_TUNA = 1,			// 请求吐纳
			REALM_BREAK_OP_TYPE_FETCH_EXP = 2,		// 请求领取角色经验
			REALM_BREAK_OP_TYPE_FIGHT = 3,			// 请求挑战心灵之境

			REAL_BREAK_OP_TYPE_MAX
		};

		short notice_type;
		short reserve_sh;							// type为3时传seq
	};

	class SCRealmBreakAllInfo		//!< 1881 境界系统全部信息下发
	{
	public:
		SCRealmBreakAllInfo();
		MessageHeader header;

		short cur_seq;										//当前阶数
		short tuna_num;										//吐纳次数
		unsigned int cur_break_exp;							//当前灵力值
		unsigned int cumulation_user_exp_count;				//玩家累积的待领取经验值
		unsigned int user_cumulation_pratice_time;			//玩家累积修炼的时间
		unsigned int cumulation_user_coin_count;			//玩家累积的待领取铜币
		short cur_fight_seq;								//心灵之境战斗层数
		short reserver_sh;
	};

	class SCRealmBreakItemInfo		//!< 1883 境界系统道具信息下发
	{
	public:
		SCRealmBreakItemInfo();
		MessageHeader header;

		int item_count;
		RealmBreakItem cumulation_item_list[MAX_REALM_BREAK_ITEM_NUM];	//玩家累积的待领取道具列表
	};

	/*class SCRealBreakStartTuNa		//!< 1882 境界系统开始吐纳下发	客户端不做服务端时间回调,目前使用玩家上次吐纳结束时间戳来限制
	{
	public:
		SCRealBreakStartTuNa();
		MessageHeader header;

		unsigned int need_time;			//!< 所需吐纳时间(秒)
	};*/

	class CSMapExplorationReq		//!< 1890 地图探索请求
	{
	public:
		CSMapExplorationReq();
		MessageHeader header;
		enum MAP_EXPLORATION_TYPE
		{
			MAP_EXPLORATION_TYPE_INFO = 0,//请求下发
			MAP_EXPLORATION_TYPE_REQ = 1,//请求探索 param 填对应seq
		};
		int req_type;
		int param;
	};

	class SCMapExplorationInfo		//!< 1891 地图探索下发
	{
	public:
		SCMapExplorationInfo();
		MessageHeader header;

		BitMap<MAX_MAP_EXPLORATION_NUM> m_map;
	};

	class SCSalaryBattleStatInfo		//!< 2920 战斗前统计本场战斗侠义值信息
	{
	public:
		SCSalaryBattleStatInfo();
		MessageHeader header;

		int seq;
		int salary;
	};

	enum SET_AUTO_ADD_POINT_FLAG
	{
		SET_AUTO_ADD_POINT_FLAG_ROLE,		//角色 方案 index，param加点属性
		SET_AUTO_ADD_POINT_FLAG_PET,			//宠物 p0 pet_index , p1 flag
	};

	class CSSetAutoAddPointFlag //!< 3880 设置自动加点信息
	{
	public:
		CSSetAutoAddPointFlag();
		MessageHeader header;

		int req_type;
		short index;
		short param[5];
	};

	class CSChivalrousRankListReq //!< 10062 侠义值排行榜请求
	{
	public:
		CSChivalrousRankListReq();
		MessageHeader header;

		int rank_type;
	};

	class SCChivalrousRankList //!< 10063 侠义值排行榜下发
	{
	public:
		SCChivalrousRankList();
		MessageHeader header;

		short rank_type;
		short count;
		ChivalrousRankInfo rank_list[CHIVALROUS_RANK_LIST_NUM]; // 长度最大100
	};

	class SCWorldLevelInfo		//!< 1126 世界等级信息下发
	{
	public:
		SCWorldLevelInfo();
		MessageHeader header;

		int open_day;							// 开服天数
		unsigned int today_zero_timestamp;		// 当天0点时间戳
	};

	class SCCustomInstructionInfo		//!< 3920 自定义指令下发
	{
	public:
		SCCustomInstructionInfo();
		MessageHeader header;

		int instruction_num;
		InstructionStr instruction_list[CUSTOM_INSTRUCTION_NUM];	// 最多8个，根据instruction_num来读
	};

	class CSCustomInstructionAdd		//!< 3921 自定义指令新增
	{
	public:
		CSCustomInstructionAdd();
		MessageHeader header;

		InstructionStr instruction_str;	//!< 指令文字
	};

	class CSCustomInstructionRemove		//!< 3922 删除自定义指令
	{
	public:
		CSCustomInstructionRemove();
		MessageHeader header;

		int index;						//!< 指令index
	};

	class CSCustomInstructionBatchRemove	//!< 3923 批量删除自定义命令
	{
	public:
		CSCustomInstructionBatchRemove();
		MessageHeader header;

		int num;
		short index_list[CUSTOM_INSTRUCTION_NUM];
	};

	class CSBigDipperReq	//!< 3950 北斗七星请求
	{
	public:
		CSBigDipperReq();
		MessageHeader header;
		enum REQ_TYPE
		{
			REQ_TYPE_0 = 0,//请求下发信息
			REQ_TYPE_1 = 1,//请求开始战斗 param1 对应星君sn
			REQ_TYPE_2 = 2,//请求领取首通奖励 param1 对应星君sn
			REQ_TYPE_3 = 3,//请求领取通关阶段奖励 param1 对应星君sn  param2 阶段下标[0-2]
		};
		int type;
		int param1;
		int param2;
	};

	class SCBigDipperInfo	//!< 3951 北斗七星下发
	{
	public:
		SCBigDipperInfo();
		MessageHeader header;

		int monster_num;
		int open_monster_list[MAX_BIG_DIPPER_MONSTER_NUM];
	};

	class SCBigDipperMyInfo	//!< 3952 北斗七星-我的星君通关信息
	{
	public:
		SCBigDipperMyInfo();
		MessageHeader header;
		BigDipperMonster monster_list[MAX_BIG_DIPPER_MONSTER_NUM];//我的所有星君信息
		char help_list[MAX_BIG_DIPPER_MONSTER_NUM];
	};

	class SCFallenGodInfo	//!< 3953 殒神之地我的通关信息
	{
	public:
		SCFallenGodInfo();
		MessageHeader header;
		BigDipperMonster monster_list[MAX_FALLEN_GOD_MONSTER_NUM];//我的所有boss信息
		char help_list[MAX_FALLEN_GOD_MONSTER_NUM];
	};

	class CSFallenGodReq	//!< 3954 殒神之地请求
	{
	public:
		CSFallenGodReq();
		MessageHeader header;
		enum REQ_TYPE
		{
			REQ_TYPE_0 = 0,//请求下发信息
			REQ_TYPE_1 = 1,//请求开始战斗 param1 对应sn
		};
		int type;
		int param1;
		int param2;
	};

	class SCSvipClientCustomData		//!< 2098 下发SVip客户端自定义数据
	{
	public:
		SCSvipClientCustomData();
		MessageHeader header;

		int data_1;
		int data_2;
	};

	class CSSvipClientCustomData		//!< 2099 SVIP客户端自定义数据请求
	{
	public:
		CSSvipClientCustomData();
		MessageHeader header;

		int oper_type;		//!< [0:请求数据  1:上传数据]
		int param1;
		int param2;
	};

	class CSRoleAttrPelletReq		//!< 3960 属性丹操作请求
	{
	public:
		CSRoleAttrPelletReq();
		MessageHeader header;

		enum ROLE_ATTR_PELLET_REQ_TYPE
		{
			ROLE_ATTR_PELLET_REQ_TYPE_ALL_INFO = 0,			// 请求下发角色与伙伴属性丹使用信息
			ROLE_ATTR_PELLET_REQ_TYPE_USE_ITEM = 1,			// 请求使用属性丹(角色) P1:pellet_id P2:num 
			ROLE_ATTR_PELLET_REQ_TYPE_USE_ITEM_PET = 2,		// 请求使用属性丹(宠物) P1:pellet_id P2:num P3:pet_index
			ROLE_ATTR_PELLET_REQ_TYPE_USE_ITEM_PARNTER = 3, // 请求使用属性丹(伙伴) P1:pellet_id P2:num P3:partner_id
		};

		short op_type;
		short param1;
		int param2;
		int param3;
	};

	class SCRoleAttrPelletAllInfo : public IMessageSerializer	//!< 3961 属性丹所有信息下发
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

	class SCRoleAttrPelletSingleInfo	//!< 3962 属性丹单个信息下发
	{
	public:
		SCRoleAttrPelletSingleInfo();
		MessageHeader header;

		short role_attr_pellet_type;			//ROLE_ATTR_PELLET_TYPE 
		short use_count;						//使用数量
		int attr_pellet_type;					//ATTR_PELLET_TYPE_MAX
		int param1;								//根据ROLE_ATTR_PELLET_TYPE 1:pet_index 2:partner_id
	};



	struct CSActiveTurntableReq				//!< 3965 活跃转盘请求
	{
	public:
		enum REQ_TYPE
		{
			REQ_TYPE_INFO,				// 请求数据信息
			REQ_TYPE_LUCK,				// 抽取转盘奖励
		};

		CSActiveTurntableReq();
		MessageHeader header;

		int type;
		int param;
	};

	struct SCActiveTurntableInfo			//!< 3966 活跃转盘任务信息
	{
	public:
		SCActiveTurntableInfo();
		MessageHeader header;

		int turntable_time;												// 活跃转盘次数
		BitMap<BIT_COUNT_OF_LONG_64> turntable_day_task_flag;			// 活跃转盘 每日任务完成标记
		BitMap<BIT_COUNT_OF_LONG_64> turntable_week_task_flag;			// 活跃转盘 每周任务完成标记
	};

	struct SCActiveTurntableRewardInfo		//!< 3967 活跃转盘抽奖返回
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
		CAPACITY_TARGET_REQ_TYPE_START,		//开始邀战
		CAPACITY_TARGET_REQ_TYPE_FETCH,		//领取奖励
	};

	struct CSCapacityTargetReq		//!< 3975 系统邀战 战力比较
	{
	public:
		CSCapacityTargetReq();
		MessageHeader header;

		int req_type;
	};


	struct SCCapacityTargetInfo		//!< 3976 系统邀战 
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

		unsigned int  capacity_target_end_time;			//结束时间
		char  capacity_target_can_start;			//完成任务设置为1
		char  capacity_target_is_finish;			//到时间设置为1
		short  capacity_target_is_fetch_reward;				//是否领取了奖励
		int capacity_target_value;				//时间到后的战力
		int capacity_target_robot_value;				//时间到后的对手战力
		unsigned int  capacity_target_begin_time;			//结束时间
	};

	struct SCEndlessTowerInfo//4120    无尽之塔下发
	{
	public:
		SCEndlessTowerInfo();
		MessageHeader header;

		int battle_seq;//当前通关seq
	};

	struct CSEndlessTowerReq
	{
	public:
		CSEndlessTowerReq();// 4121 无尽之塔请求
		MessageHeader header;
		enum REQ_TYPE
		{
			REQ_TYPE_INFO = 0,//下发信息
			REQ_TYPE_BATTLE = 1,//发起战斗
		};
		int req_type;
		int param1;
		int param2;
	};

	struct SCFlyUpSystemInfo
	{
	public:
		SCFlyUpSystemInfo();// 4130 飞升
		MessageHeader header;
		
		int task_idx;//第N环任务/今天完成次数
		int fly_skill_level;//飞升技能等级
		int fly_up_flag;//飞升阶段
		int is_shilian;//是否试炼中
		int element[FlyUpSystemParam::FLY_UP_ELEMENT_MAX];//元素
		int organ[FlyUpSystemParam::FLY_UP_ORGAN_MAX];//五官
		int is_one_click_finish;	// 是否是一键完成
	};

	struct CSFlyUpSystemReq
	{
	public:
		CSFlyUpSystemReq();// 4131 飞升请求
		MessageHeader header;
		enum REQ_TYPE
		{
			REQ_TYPE_INFO = 0,//请求下发
			REQ_TYPE_ADV = 1,//进阶
			REQ_TYPE_FINISH = 2,//完成飞升
			REQ_TYPE_SHILIAN = 3,//开始试炼
			REQ_TYPE_UP_SKILL = 4,//升级飞升技能
			REQ_TYPE_RAND_TASK = 5,//接取飞升任务
			REQ_TYPE_ADV_SAVE = 6,//进阶保存
			REQ_TYPE_ONE_KEY_UP_SKILL = 7, //一键升级技能
		};
		int type;
		int param1;
		int param2;
	};

	struct CSMirageTerritoryReq
	{
	public:
		CSMirageTerritoryReq();//4132 请求
		MessageHeader header;
		enum REQ_TYPE
		{
			REQ_TYPE_INFO = 0,			//请求下发 param[分区 0 - 3]
			REQ_TYPE_BATTLE = 1,		//战斗 param1[分区 0 - 3]
			REQ_TYPE_ONE_KEY_PASS = 2,	//派遣 param1[分区 0 - 3] param2 派遣的伙伴id param3 派遣伙伴id param4 派遣伙伴id
			REQ_TYPE_BATTLE_ZHEN_INFO = 3,//下发布阵信息 param1[分区 0 - 3]
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
		SCMirageTerritoryInfo();// 4133 下发
		MessageHeader header;
		
		int today_pass_times;//今天派遣次数
		int pass_area;//今天派遣分区
		int pass_seq;//今天派遣seq
		int area;
		int top_seq;//最高通关记录
		int now_seq;//当前通关记录
	};

	struct CSMirageTerritorySetPartnerReq
	{
	public:
		CSMirageTerritorySetPartnerReq();// 4134 设置琉璃蜃境伙伴布阵
		MessageHeader header;

		int programme_idx;//方案id
		int partner_id_list[MAX_MIRAGE_TERRITORY_PARTNER_NUM];
		int partner_posi_list[MAX_MIRAGE_TERRITORY_PARTNER_NUM];
	};

	struct SCMirageTerritorySetPartnerInfo
	{
	public:
		SCMirageTerritorySetPartnerInfo();// 4135 下发琉璃蜃境伙伴布阵
		MessageHeader header;

		int programme_idx;//方案id
		int partner_id_list[MAX_MIRAGE_TERRITORY_PARTNER_NUM];
		int partner_posi_list[MAX_MIRAGE_TERRITORY_PARTNER_NUM];
	};

	struct SCFlyUpSystemAdvInfo
	{
	public:
		SCFlyUpSystemAdvInfo();// 4136 飞升强化
		MessageHeader header;

		int fly_up_flag;//飞升阶段
		int fly_adv_idx;//元素idx
		int value;//增加值
	};

	struct SCFlyUpSkillSigleInfo
	{
	public:
		SCFlyUpSkillSigleInfo();// 4137 飞升技能
		MessageHeader header;

		int skill_level;//当前等级
		int skill_exp;//当前经验
	};

	class CSAuditBehaviorSubmitReq		//!< 1132 客户端行为埋点请求
	{
	public:
		CSAuditBehaviorSubmitReq();
		 MessageHeader header;

		int behavior_type;					//CLIENT_BEHAVIOR_TYPE_MAX
	};

	class SCYaoXingShiLianInfo		//!< 1133 妖行试炼下发
	{
	public:
		SCYaoXingShiLianInfo();
		MessageHeader header;

		int pass_battle_seq;
		BitMap<MAX_YAOXINGSHILIAN_REWARD_LIST> m_reward_bit_map;
		BitMap<MAX_YAOXINGSHILIAN_REWARD2_LIST> m_reward_bit_map2;
		BitMap<32> m_cmd_map;
	};

	class CSYaoXingShiLianReq		//!< 1134 妖行试炼请求
	{
	public:
		CSYaoXingShiLianReq();
		MessageHeader header;
		enum REQ_TYPE
		{
			REQ_TYPE_INFO = 0,//下发
			REQ_TYPE_BATTLE = 1,//战斗
			REQ_TYPE_REWARD = 2,//成就奖励 param 领取seq
			REQ_TYPE_REWARD2 = 3,//成就奖励2 param 领取seq
		};
		int type;
		int param;
	};

	class CSSanXianGuiDongReq		//!< 1135 三仙归洞请求
	{
	public:
		CSSanXianGuiDongReq();
		MessageHeader header;
		enum SAN_XIAN_REQ
		{
			SAN_XIAN_INFO = 0,//信息
			SAN_XIAN_START = 1,//开始
			SAN_XIAN_CHOOSE = 2,//选择 param 是否正确（1正确/2错误）
			SAN_XIAN_FB = 3,//进入副本
		};
		int type;
		int param;
	};

	class SCSanXianGuiDongInfo		//!< 1136 三仙归洞下发
	{
	public:
		SCSanXianGuiDongInfo();
		MessageHeader header;
		
		int today_play_times;//今天游戏次数
		int pass_idx;//当前通关关卡
	};

	class SCSanXianGuiDongPlayEndInfo		//!< 1137 三仙归洞游戏下发
	{
	public:
		SCSanXianGuiDongPlayEndInfo();
		MessageHeader header;
		enum GAME_TYPE
		{
			GAME_TYPE_PLAYING = 0,//游戏中
			GAME_TYPE_LOSE = 1,//失败
			GAME_TYPE_WIN = 2,//胜利
		};
		int game_type;
		int pass_idx;//当前通关关卡
		unsigned int choose_tamp;//选择限制时间
		int sanxian_title_flag;//称号标记
	};

	class CSTaoQuanNaJiReq			//!< 1139 套圈纳吉请求
	{
	public:
		CSTaoQuanNaJiReq();
		MessageHeader header;
		enum NA_JI_REQ
		{
			NA_JI_REQ_INFO = 0,//信息
			NA_JI_REQ_START = 1,//开始
			NA_JI_REQ_CHOOSE = 2,//套圈(param1 - X)(param2 - y)横纵坐标
			NA_JI_REQ_FB = 3,//进入副本
			NA_JI_REQ_LEAVE = 4,//离开副本
			NA_JI_REQ_PLAY_INFO = 5,//游戏信息
		};
		int req_type;
		int param1;
		int param2;
	};

	class SCTaoQuanNaJiInfo		//!< 1140 套圈纳吉
	{
	public:
		SCTaoQuanNaJiInfo();
		MessageHeader header;
		int map_init;
		int play_map[TAO_QUAN_NA_JI_X_NUM][TAO_QUAN_NA_JI_Y_NUM];//地图
		int can_play_times;//可套圈次数
		int today_play_times;//今天玩耍次数
		int flag_title;//(1/0)是否已经活动称号
	};

	class SCTaoQuanNaJiPlayEndInfo//!< 1141 套圈纳吉结算
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

	class CSHundredGodBossReq	//!< 1145	百神之师请求
	{
	public:
		CSHundredGodBossReq();
		MessageHeader header;

		enum HUNDRED_GOD_BOSS_REQ_TYPE
		{
			HUNDRED_GOD_BOSS_REQ_TYPE_INFO = 0,					// 请求信息下发
			HUNDRED_GOD_BOSS_REQ_TYPE_FETCH_TARGET = 1,			// 请求领取目标奖励 param:target_id
			HUNDRED_GOD_BOSS_REQ_TYPE_FINISH_REWARD = 2,		// 请求领取完成奖励
			HUNDRED_GOD_BOSS_REQ_TYPE_START = 3,				// 请求百神之师开始
		};

		int req_type;
		int param;					
	};

	class SCHundredGodBossInfo	//!< 1146 百神之师信息下发
	{
	public:
		SCHundredGodBossInfo();
		MessageHeader header;

		char reserve_ch;
		char is_finish_hundred_god_boss_flag;			// 是否完成了百神之师 0:未完成 1:已完成
		short fetch_hundred_god_boss_flag;				// 百神之师达成奖励表示(二进制) 1:已领取
		short hundred_god_boss_fin_bounty_count;		// 百神之师完成十环门派任务次数
		short reserve_sh;
		unsigned int hundred_god_boss_end_timestamp;	//百神之师结束时间戳
	};

	class CSPartnerPetRankReq	//!<5162 伙伴/宠物前十排行榜请求
	{
	public:
		CSPartnerPetRankReq();
		MessageHeader header;

		int rank_type;		// 0--宠物排行, 1--伙伴排行
	};

	class SCPetTopRankInfo		//!<5163 宠物前十信息下发
	{
	public:
		SCPetTopRankInfo();
		MessageHeader header;
	
		PetTopRankInfo top_list[PET_TOP_RANK_NUM];
	};

	class SCPartnerTopRankInfo	//!<5164 伙伴前十信息下发
	{
	public:
		SCPartnerTopRankInfo();
		MessageHeader header;

		PartnerTopRankInfo top_list[PARTNER_TOP_RANK_NUM];
	};

	class SCCharismaitcTanabataInfo	//!< 1148 魅力七夕下发
	{
	public:
		SCCharismaitcTanabataInfo();
		MessageHeader header;

		int admiration_value;//爱慕值
		BitMap<MAX_CHARIS_FLAG> admiration_map;
		int charm_value;//魅力值
		BitMap<MAX_CHARIS_FLAG> charm_map;
	};

	class SCElementElfRewards	//!< 5185 元素精灵奖励通知下发
	{
	public:
		SCElementElfRewards();
		MessageHeader header;
		
		int reward_seq;		// 玩家获得的奖励seq
	};

	class SCGuaJiExpInfo		//!< 5355 每日挂机经验
	{
	public:
		SCGuaJiExpInfo();
		MessageHeader header;

		int today_add_guaji_exp;
	};

	class CSRealNameRewardReq		//!< 5410 实名认证奖励领取请求
	{
	public:
		CSRealNameRewardReq();
		MessageHeader header;
	};

	class SCRealNameRewardInfo		//!< 5411 实名认证奖励领取信息下发
	{
	public:
		SCRealNameRewardInfo();
		MessageHeader header;

		int receive_flag;
	};
}

#pragma pack(pop)

#endif

