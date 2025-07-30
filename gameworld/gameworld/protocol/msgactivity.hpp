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

	class CSActivityEnterReq											//!< 1203 进入活动
	{
	public:
		CSActivityEnterReq();
		MessageHeader header;

		int activity_type;
		int param1;
	};

	///////////////////////////////////////////////////小队竞技/////////////////////////////////////////////////////////////////
	class CSActivityTeamFightMatchReq									//!< 1204 小队竞技-请求匹配
	{
	public:
		CSActivityTeamFightMatchReq();
		MessageHeader header;

		enum TYPE
		{
			TYPE_CANCEL = 0,			//!<取消匹配
			TYPE_MATCH,					//!<请求匹配
			TYPE_ROLE_INFO,				//!<获取个人信息
			TYPE_RANK_INFO,				//!<获取排行榜信息
			TYPE_RECORD_INFO,			//!<获取战报信息信息
			TYPE_RECORD_ONLY_INFO,		//!<只看我的战报信息信息
			TYPE_FETCH_BOX,				//!<领取宝箱		param1首胜发0,三胜发1 详情对照ACT_TEAM_FIGHT_BOX_TYPE
		};

		int is_match;
		int param1;						//暂时只在领取宝箱时使用
	};

	class SCActivityTeamFightNotice									//!< 1205 小队竞技-匹配状态
	{
	public:
		SCActivityTeamFightNotice();
		MessageHeader header;

		enum MATCH_TYPE
		{
			MATCH_TYPE_NO = 0,	// 未匹配
			MATCH_TYPE_ING,		// 匹配中
		};

		int is_match;
	};

	class SCActivityTeamFightScore									//!< 1206 小队竞技-积分
	{
	public:
		SCActivityTeamFightScore();
		MessageHeader header;

		int score;				//!< 积分
		int rank;				//!< 排名
		int victory_count;		//!< 胜场
		int defeat_count;		//!< 败场
		int continue_victory_count;		//!< 连胜
		int box_fetch_flag;		//!< 宝箱领取记录(二进制标记)
		int battle_count;		//!< 战斗场数
	};
	static const int RANK_MAX_COUNT = 10;

	class SCActivityTeamFightRank									//!< 1210 小队竞技-排名
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
	class SCActivityTeamFightRecord									//!< 1211 小队竞技-战报
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

	class SCActivityTeamFightMatchSucc									//!< 1215 小队竞技-倒数通知
	{
	public:
		SCActivityTeamFightMatchSucc();
		MessageHeader header;

	};
	
	///////////////////////////////////////////////////迷宫竞速/////////////////////////////////////////////////////////////////
	class CSActivityMazeRacerHandleReq			//!< 1207 迷宫竞速-操作请求
	{
	public:
		CSActivityMazeRacerHandleReq();
		MessageHeader header;

		enum MazeRacerOperatorType
		{
			MAZE_RACER_REQ_TRANSP = 0,			//!< 请求传送
			MAZE_RACER_USE_AMULET = 1,			//!< 使用符咒
			MAZE_RACER_ROLE_INFO = 2,			//!< 角色信息

			MAZE_RACER_OP_TYPE_MAX
		};

		short op_type;	//!< 0:请求传送		1:使用符咒
		short param1;	//!< 0:				1:符咒ID
	};

	class SCActivityMazeRacerAmuletNum			//!< 1208 迷宫竞速-队伍符文数量
	{
	public:
		SCActivityMazeRacerAmuletNum();
		MessageHeader header;

		int type_count;		//!< 拥有的符文种类数
		PtcItemInfo amulet_num[ACTIVITY_MAZE_RACER_AMULET_MAX_TYPE];	//!< 拥有的符文列表
	};

	struct MazeRacerRankItem
	{
		MazeRacerRankItem() : pass_time(0), rank_posi(0), mem_count(0) {};

		UInt32 pass_time;		//!< 通关时长(秒)
		UInt16 rank_posi;		//!< 排名
		UInt16 mem_count;		//!< 队员数量
		ActivityRoleInform mem_list[MAX_TEAM_MEMBER_NUM];	//!< 队员信息列表
	};

	class SCActivityMazeRacerRankLists : public IMessageSerializer			//!< 1209 迷宫竞速 - 排行榜信息
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
		
		UInt16 maze_id;		//!< 迷宫ID
		UInt16 list_num;	//!< 排名列表数量
		MazeRacerRankItem rank_list[ACTIVITY_RANK_SHORTER_NUM_MAX];	//!< 排名列表信息
	};

	class SCActivityMazeRacerWaitsInfo			//!< 1212 迷宫竞速 - 等候大厅信息
	{
	public:
		SCActivityMazeRacerWaitsInfo();
		MessageHeader header;

		UInt32 next_begin_timestamp;		//!< 下一场的开启时间戳
		int cur_waiting_role_num;			//!< 当前大厅等待玩家数

		UInt16 min_level;		//!< 当前大厅最小等级
		UInt16 max_level;		//!< 当前大厅最大等级
	};

	class SCActivityMazeRacerCopysInfo			//!< 1213 迷宫竞速 - 副本信息
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

			GameName leader_name;		//!< 队长名字
			int cur_layer;				//!< 当前层数
			int leader_uid;				//!< 队长UID
			int team_index;
		};

		UInt32 begin_time;		//!< 本迷宫开始的时间戳(秒)
		UInt16 reserve_ush;		//!< 保留字，无实意
		UInt16 list_num;		//!< 排名列表总长
		MazeRacerCopyRankItem rank_list[ACTIVITY_RANK_SHORTER_NUM_MAX];		//!< 排名列表
	};

	class SCActivityMazeRacerPassdInfo			//!< 1214 迷宫竞速 - 通关信息
	{
	public:
		SCActivityMazeRacerPassdInfo();
		MessageHeader header;

		UInt32 pass_time;			//!< 通关所用时间(秒)
		UInt16 rank_posi;			//!< 当前排名
		UInt16 total_encounter;		//!< 总计遇怪次数
		int award_num;				//!< 奖品列表长度
		PtcItemInfo award_list[ACTIVITY_AWARD_NUM_MAX];		//!< 奖品详细信息
	};

	class SCActivityMazeRacerCurseInfo			//!< 1216 迷宫竞速 - 受咒信息
	{
	public:
		SCActivityMazeRacerCurseInfo();
		MessageHeader header;

		ObjID obj_id;
		UInt16 curse_type[MAZE_AMULET_EFFECT_TYPE_MAX];		//!< 大小4，符咒ID - (-1) 代表没有受到符文效果
	};

	class SCActivityMazeRacerActInsert			//!< 1217 迷宫竞速 - 活动排名新增
	{
	public:
		SCActivityMazeRacerActInsert();
		MessageHeader header;

		UInt16 maze_id;			//!< 迷宮ID
		UInt16 reserve_sh;		//!< 保留字
		MazeRacerRankItem rank_info;		//!< 排名信息
	};

	class SCActivityMazeRacerSceneInfo			//!< 1240 迷宫竞速 - 场景信息 (进入场景就发)
	{
	public:
		SCActivityMazeRacerSceneInfo();
		MessageHeader header;

		unsigned int time;  //进入某层的时间
		int x;			//传送门位置
		int y;
		unsigned int start_time;	//这次迷宫开始时间
		int layer;			//层数
	};

	class SCActivityMazeRacerRoleInfo			//!< 1253 迷宫竞速 - 角色信息
	{
	public:
		SCActivityMazeRacerRoleInfo();
		MessageHeader header;

		int get_pass_times;			//获得通关奖励次数
	};

	///////////////////////////////////////////////////家族大乱斗/////////////////////////////////////////////////////////////////
	class CSActivityGuildFightOperatorReq
	{
	public:
		CSActivityGuildFightOperatorReq();			//!< 1220: 家族大乱斗 - 操作请求
		MessageHeader header;

		enum GUILD_FIGHT_OPERATOR_TYPE
		{
			GF_OP_TYPE_REQ_ALL_RANK_INFOR = 0,		//!< 请求所有排名和本家族的信息(打开排行榜)
			GE_OP_TYPE_REQ_CLOSE_RANK = 1,			//!< 请求关闭排行榜 
		};

		int op_type;		//!< 请求类型
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

		int rank_posi;			//!< 排名位置 - 0 表示未上榜
		int total_score;		//!< 总积分
		int win_times;			//!< 胜利次数
		int battle_times;		//!< 战斗次数 - 注意，服务器计算的胜率会保留两位小数
		GuildName guild_name;	//!< 家族名称
	};

	class SCActivityGuildFightRankAllInfo
	{
	public:
		SCActivityGuildFightRankAllInfo();			//!< 1221: 家族大乱斗 - 排行榜信息
		MessageHeader header;

		int resi_role;			//!< 剩余玩家数
		int resi_family;		//!< 剩余家族数
		int list_num;
		GuildFightRankItem list_info[ACTIVITY_RANK_DEFAULT_NUM_MAX];
	};

	class SCActivityGuildFightMyGuildInfo
	{
	public:
		SCActivityGuildFightMyGuildInfo();			//!< 1222: 家族大乱斗 - 我的家族信息
		MessageHeader header;

		enum GUILD_FIGHT_TYPE
		{
			GUILD_FIGHT_TYPE_INFO = 0,					//正常下发信息
			GUILD_FIGHT_TYPE_END_NOTICE_AND_INFO = 1,	//结算时弹窗及下发我的家族排行信息
		};

		int type;
		GuildFightRankItem my_family_info;			//!< 我的家族的排行信息
	};

	class SCActivityGuildFightParticipNum
	{
	public:
		SCActivityGuildFightParticipNum();			//!< 1225: 家族大乱斗 - 参与人数信息
		MessageHeader header;

		int guild_participate_num;					//!< 家族参与人数
	};

	/////////////////////////////////////////////////// 充值 ///////////////////////////////////////////////////
	class SCChongzhiInfo						//!< 1218 玩家充值信息
	{
	public:
		SCChongzhiInfo();
		MessageHeader header;

		long long history_chongzhi_num;			//!< 历史总充值数
		long long today_chongzhi_num;			//!< 玩家今天充值的数量
		int chongzhi_fanli_reward_fetch_flag;	//!< 充值返利奖励领取标记
		int reset_level_num;					//!< 每日重置时的等级
	};

	class CSChongzhiOperateReq						//!< 1219 玩家充值相关功能请求
	{
	public:
		CSChongzhiOperateReq();
		MessageHeader header;

		enum OPER_TYPE
		{
			OPER_TYPE_SEND_CHONGZHI_INFO = 0,		//!< 0 获取玩家充值相关信息
			OPER_TYPE_FANLI_REWARD_FETCH = 1,		//!< 1 领取返利奖励  param1:seq
		};

		int oper_type;
		int param1;
		int param2;
	};

	////////////////////////////////////// 跨服 /////////////////////////////////
	class CSCrossStartReq			//!< 2800 玩家请求跨服
	{
	public:
		CSCrossStartReq();
		MessageHeader header;
	};

	class SCCrossStartAck			//!< 2801 请求跨服返回
	{
	public:
		SCCrossStartAck();
		MessageHeader header;

		char gateway_hostname[64];	//!< gateway的ip或域名
		int gateway_port;			//!< gateway的端口

		GameName role_name;			//!< 角色名
		int new_uid;				//!< 新的uid
		int scene_id;				//!< 场景ID			\note 用于验证
		int last_scene_id;			//!< 上一个场景ID	\note 用于验证
		unsigned int time;			//!< 时间戳			\note 用于验证
			
		SessionKey session_key;		//!< 会话秘钥		\note 用于验证
	};

	class CSBackToOriginServer		//!< 2802 请求回本服
	{
	public:
		CSBackToOriginServer();
		MessageHeader header;
	};

	class SCBackToOriginServer		//!< 2803 回本服返回信息
	{
	public:
		SCBackToOriginServer();
		MessageHeader header;

		int scene_id;				//!< 场景ID			\note 用于验证
		int last_scene_id;			//!< 上一个场景ID	\note 用于验证
		unsigned int time;			//!< 时间戳			\note 用于验证

		SessionKey session_key;		//!< 会话秘钥		\note 用于验证

		char gateway_hostname[64];	//!< gateway的域名
		unsigned short gateway_port;//!< gateway的端口
		short reserve_sh;			//!< 保留字
	};

	class CSNewChongzhiConfigInfoReq // 2018
	{
	public:
		CSNewChongzhiConfigInfoReq();
		MessageHeader header;

		int currency; // 金额类型 （0：人民币  1：新台币  2：美金）
		char spid_str[4]; //根据客户端显示的spid来做
	};

	class SCNewChongzhiConfigInfo // 2042
	{
	public:
		SCNewChongzhiConfigInfo();
		MessageHeader header;

		struct ChongzhiInfo
		{
			char seq;												// 序号，从0开始
			char extra_reward_type;									// 额外奖励类型 (0:不奖励  1：绑元  2：至尊会员  3：铜币  4：元宝）
			short reserve_sh;										// 保留字
			int add_gold;											// 加多少元宝
			int extra_reward;										// 额外送的奖励数量
			int money_show;											// 100倍的金额， 需要除以100（兼容美金带有小数）
			char description_first_chongzhi[DESCRIPTION_STR_LEN];	// 首次充值描述   DESCRIPTION_STR_LEN = 128
		};

		short currency_type;										// 原样返回需要显示的金额类型 （0：人民币  1：新台币  2：美金）
		short info_count;											// info数组元素数量
		ChongzhiInfo info_list[CHONGZHI_REWARD_CONFIG_MAX_NUM];		// info数组  CHONGZHI_REWARD_CONFIG_MAX_NUM = 32
	};

	class SCNewChongzhiConfigReloadNotice // 2043  客户端收到这协议时请求2042
	{
	public:
		SCNewChongzhiConfigReloadNotice();
		MessageHeader header;
	};

	class SCWorldQuestionNotice // 世界答题下发 3205
	{
	public:
		SCWorldQuestionNotice();
		MessageHeader header;

		int now_answer_subject;						//题目id
		long long answer_tamp;						//结束回答时间戳
		long long show_answer_tamp;					//展示答案时间戳
		long long next_subject_refresh_tamp;		//下一次题目刷新时间
		long long answer_end_tamp;					//结束时间戳
		char is_broadcast;							//是否广播
		char reserve_ch;							//是否答题正确(无用)
		short level_limit;							//等级限制,达到该等级的玩家才给显示
		int role_answer_idx;						//玩家答案下标【1-4】
	};

	class CSWorldQuestionReq// 世界答题请求 3206
	{
	public:
		CSWorldQuestionReq();
		MessageHeader header;

		int type;				// 0:请求下发
	};

	/////////////////////////////////////////////////以一敌百(个人版家族大乱斗)//////////////////////////////////////////////////////////
	class CSActivityOnlyFightOperatorReq			//!< 1228: 以一敌百(个人版家族大乱斗) - 操作请求
	{
	public:
		CSActivityOnlyFightOperatorReq();			
		MessageHeader header;

		enum ONLY_FIGHT_OPERATOR_TYPE
		{
			OF_OP_TYPE_REQ_ALL_RANK_INFO = 0,		//!< 请求所有排名信息(打开排行榜)
			OF_OP_TYPE_REQ_CLOSE_RANK = 1,			//!< 请求关闭排行榜 
		};

		int op_type;		//!< 请求类型
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

		int rank_posi;			//!< 排名位置 - 0 表示未上榜
		int total_score;		//!< 总积分
		int win_times;			//!< 胜利次数
		int battle_times;		//!< 战斗次数 - 注意，服务器计算的胜率会保留两位小数
		GameName role_name;		//!< 玩家名称
	};

	class SCActivityOnlyFightRankAllInfo			//!< 1229: 以一敌百(个人版家族大乱斗) - 排行榜信息
	{
	public:
		SCActivityOnlyFightRankAllInfo();			
		MessageHeader header;

		int resi_role_num;			//!< 剩余玩家数
		int list_num;
		OnlyFightRankItem list_info[ACTIVITY_RANK_DEFAULT_NUM_MAX];
	};

	class SCActivityOnlyFightMyInfo					//!< 1230: 以一敌百(个人版家族大乱斗) - 我的信息
	{
	public:
		SCActivityOnlyFightMyInfo();			
		MessageHeader header;

		enum ONLY_FIGHT_MY_INFO_TYPE
		{
			ONLY_FIGHT_MY_INFO_TYPE_SEND = 0,		//发送我的排行信息
			ONLY_FIGHT_MY_INFO_TYPE_NOTICE = 1,		//通知显示排行榜结果及下发我的排行信息
		};

		int type;							
		OnlyFightRankItem m_info;			//!< 我的排行信息
	};

	class CSActivitySignUpReq			//!< 1231: 限时活动报名请求
	{
	public:
		CSActivitySignUpReq();
		MessageHeader header;

		int seq;					//可由seq获取是哪个活动
	};

	class SCActivitySignUpInfo			//!< 1232: 限时活动报名信息下发
	{
	public:
		SCActivitySignUpInfo();
		MessageHeader header;

		BitMap<MAX_SIGN_UP_NUM> m_sign_up_flag;		//是否报名
	};

	class SCActivityEnterInfo			//!< 1241  活动入场信息下发
	{
	public:
		SCActivityEnterInfo();
		MessageHeader header;

		short   activity_type;					//ACTIVITY_TYPE_MAX
		short	is_enter_flag;					//是否可以入场 1:可以入场 0:不可入场
		unsigned int enter_end_timestamp;		//入场结束时间戳
	};

	class SCActivityGuildFightSpecialInfo		//!< 1242 家族大乱斗特殊信息下发
	{
	public:
		SCActivityGuildFightSpecialInfo();
		MessageHeader header;

		int resi_role;			//!< 剩余玩家数
		int resi_family;		//!< 剩余家族数		
	};

	class SCActivityOnlyFightSpecialInfo		//!< 1243 以一敌百特殊信息下发
	{
	public:
		SCActivityOnlyFightSpecialInfo();
		MessageHeader header;

		int resi_role;			//!< 剩余玩家数
	};

	class SCRAMoneyAddInfo		//!< 1244 随机活动特殊货币增加
	{
	public:
		SCRAMoneyAddInfo();
		MessageHeader header;

		int rand_activity;		//!< 活动号
		int add_money;			//!< 货币值
	};

	class SCRAGiveSecretKeyInfo		//!< 1245 赠寻宝次数
	{
	public:
		SCRAGiveSecretKeyInfo();
		MessageHeader header;

		int ra_reward_flag;		//!< 奖励领取（二进制）
		unsigned int frist_pass_flag[CRYSTAL_FB_FIRST_FLAG];
	};

	class SCRACumulativeChargeInfo		//!< 1246 累计充值
	{
	public:
		SCRACumulativeChargeInfo();
		MessageHeader header;

		int has_get_reward;				//!< 是否已经领取奖励
		long long acc_price;			//!< 累计充值金额
		int is_received;				//!< 是否已领取过免费奖励,0--未领取，1--已领取
	};

	class CSActivityAdvanceFetchReq				//!< 1247 限时活动提前领取奖励请求
	{
	public:
		CSActivityAdvanceFetchReq();
		MessageHeader header;

		enum ACTIVITY_ADVANCE_FETCH_OP_TYPE
		{
			ACTIVITY_ADVANCE_FETCH_OP_TYPE_PARTICIP_AWARD = 0,			// 领取参与奖励  
			ACTIVITY_ADVANCE_FETCH_OP_TYPE_SURVIVAL_AWARD = 1,			// 领取存活奖励 param1:live_num（存活人数)

			ACTIVITY_ADVANCE_FETCH_OP_TYPE_MAX
		};

		short activity_type;				//ACTIVITY_TYPE_MAX
		short op_type;
		int param1;
	};

	class SCActivityAdvanceFetchInfo		//!< 1248 限时活动提前领取奖励信息
	{
	public:
		SCActivityAdvanceFetchInfo();
		MessageHeader header;

		short activity_type;
		short level;
		short reserve_sh;
		char particip_award_flag;		// 参与奖励识标 0:未领取 1:已领取 
		char survival_award_flag;		// 存活奖励表示(二进制)
	};

	class SCActivityConfigReloadNotice			//1250 热更活动配置重读通知
	{
	public:
		SCActivityConfigReloadNotice();
		MessageHeader header;
	};

	class SCNoticeClientRestart				// 1264 通知客户端重启
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
			unsigned long long unique_id;     //相当于两个int，int1和int2
			unsigned int p[2];
		};
		
		int cfg_ver;
	};

	class CSCmdBuyItemCheckReq			//!< 1252 普通模块直购类型配置检测
	{
	public:
		CSCmdBuyItemCheckReq();
		MessageHeader header;
		
		int buy_type;				//BUY_TIME_TYPE	 
		int param1;	
		int param2;
		int cfg_ver;				
	};

	class CSMonthCardReq		//!< 2280 月卡周卡请求
	{
	public:
		CSMonthCardReq();
		MessageHeader header;

		enum MONTH_CARD_REQ_TYPE
		{
			MONTH_CARD_REQ_TYPE_INFO = 0,				//请求信息下发
			MONTH_CARD_REQ_TYPE_BUY_ITEM = 1,			//购买商城商品 P1:seq(配置)
			MONTH_CARD_REQ_TYPE_BUY_CMD_CHECK = 2,		//购买直购道具前检测 P1:seq P2:版号
			MONTH_CARD_REQ_TYPE_BUY_CARD_CHECK = 3,		//购买周卡/月卡前检测 P1:card_type P2:版号 
			MONTH_CARD_REQ_TYPE_FETCH_DAY_REWARD =4,	//领取每日奖励 P1:card_type
		};

		int req_type;
		int param1;
		int param2;
	};

	class SCMonthCardInfo		//!< 2281 月卡周卡信息下发
	{
	public:
		SCMonthCardInfo();
		MessageHeader header;

		unsigned char is_fetch_card_day_reward_flag;							//月卡/周卡每日奖励领取标识 二进制(card_type)
		char reserve_ch;
		short month_card_level;													//刷新等级
		unsigned int week_card_end_timestamp;									//周卡结束时间戳
		unsigned int month_card_end_timestamp;									//月卡结束时间戳
		int card_shop_buy_times[MAX_MONTH_CARD_SHOP_ITEM_NUM];					//月卡周卡商城购买记录
	};

	class SCQiongLouYuYanInfo					//1252 随机活动-琼楼玉宴下发
	{
	public:
		SCQiongLouYuYanInfo();
		MessageHeader header;

		int accumulate_num;				//筹办进度
		int now_status;					//当前状态
		unsigned int next_status_tamp;	//状态结束时间点
		int banquet_type;				//宴会类型
	};

	class SCQiongLouYuYanRoleInfo					//1253 随机活动-琼楼玉宴个人信息下发
	{
	public:
		SCQiongLouYuYanRoleInfo();
		MessageHeader header;

		int reward_flag;//领奖宝箱二进制
		int eat_food_times;//吃席次数
		int next_eat_tamp;//下次吃席时间（0也是可以吃席）
	};

	class SCMazeSummerMapInfo					//1254 随机活动-夏日迷宫地图下发
	{
	public:
		SCMazeSummerMapInfo();
		MessageHeader header;

		Posi m_pos;
		RAMazeSummerMap m_map[MAX_MAZE_SUMMER_POS_X][MAX_MAZE_SUMMER_POS_Y];
	};

	class SCMazeSummerSingleMapInfo					//1255 随机活动-夏日迷宫地图单格下发
	{
	public:
		SCMazeSummerSingleMapInfo();
		MessageHeader header;

		Posi change_pos;
		RAMazeSummerMap map_info;
	};

	class SCMazeSummerShopInfo					//1256 随机活动-夏日迷宫商店下发
	{
	public:
		SCMazeSummerShopInfo();
		MessageHeader header;

		int m_shop_num;
		RAMazeSummerShop m_shop[MAX_MAZE_SUMMER_SHOP_NUM];
	};

	class SCMazeSummerQuestionInfo					//1257 随机活动-夏日迷宫答题下发
	{
	public:
		SCMazeSummerQuestionInfo();
		MessageHeader header;

		int question_id;
	};

	class SCMazeSummerTaskInfo					//1258 随机活动-夏日迷宫任务下发
	{
	public:
		SCMazeSummerTaskInfo();
		MessageHeader header;

		int play_info[MAX_MAZE_SUMMER_TASK_NUM];//本次活动游戏记录
		RAMazeSummerTaskInfo m_task[MAX_MAZE_SUMMER_TASK_NUM];//任务记录
	};

	class SCMazeSummerStatusInfo					//1259 随机活动-夏日迷宫进行状态下发
	{
	public:
		SCMazeSummerStatusInfo();
		MessageHeader header;

		int is_playing;//是否正在进行游戏
		int play_times;
	};

	class SCMazeSummerPlayFinishInfo					//1260 随机活动-夏日迷宫玩法结算下发
	{
	public:
		SCMazeSummerPlayFinishInfo();
		MessageHeader header;

		int is_pass;//是否通关
	};

	class SCMazeSummerMyPosInfo					//1261 随机活动-夏日迷宫我的位置下发
	{
	public:
		SCMazeSummerMyPosInfo();
		MessageHeader header;

		Posi m_pos;//我的位置
	};


	class CSActivitySilkRoadsReq		//!< 1270  限时活动-丝绸之路请求
	{
	public:
		CSActivitySilkRoadsReq();
		MessageHeader header;

		enum SILK_ROADS_REQ_OP_TYPE
		{
			SILK_ROADS_REQ_OP_TYPE_ROLE_INFO = 0,				//请求角色活动信息
			SILK_ROADS_REQ_OP_TYPE_READY = 1,					//准备,先随机生成一些数据
			SILK_ROADS_REQ_OP_TYPE_START_OUTSET = 2,			//开始出发
			SILK_ROADS_REQ_OP_TYPE_KEEP_GOING = 3,				//继续出发
			SILK_ROADS_REQ_OP_TYPE_BUY_COMMODITY = 4,			//购买货物 param1:货物seq param2:count
			SILK_ROADS_REQ_OP_TYPE_SALE_COMMODITY = 5,			//售卖货物 param1:货物seq param2:count
			SILK_ROADS_REQ_OP_TYPE_BUY_NEWS = 6,				//购买消息 
			SILK_ROADS_REQ_OP_TYPE_EXPANSION_WAREHOUSE = 7,		//扩充仓库 param1:扩充次数
			SILK_ROADS_REQ_OP_TYPE_BUY_SUPPLY = 8,				//购买补给 param1:购买次数
			SILK_ROADS_REQ_OP_TYPE_END = 9,						//结束贸易之旅
			SILK_ROADS_REQ_OP_TYPE_RANK_ALL_INFO = 10,			//请求排行信息
			SILK_ROADS_REQ_OP_TYPE_ALL_TRADING_INFO = 11,		//请求贸易消息列表
		};

		int op_type;
		int param1;
		int param2;
	};

	class SCActivitySilkRoadsSimpleInfo		//!< 1271 限时活动-丝绸之路简易信息下发
	{
	public:
		SCActivitySilkRoadsSimpleInfo();
		MessageHeader header;

		enum SILK_ROADS_SIMPLE_TYPE
		{
			SILK_ROADS_SIMPLE_TYPE_READY = 0,					// 随机技能 param1:skill_id param2:line_group_id
			SILK_ROADS_SIMPLE_TYPE_START_OUTSET = 1,			// 开始出发 param1:cur_city_index
			SILK_ROADS_SIMPLE_TYPE_KEEP_GOING = 2,				// 继续出发 param1:cur_city_index param2:news_pool_count
			SILK_ROADS_SIMPLE_TYPE_BUY_NEWS = 3,				// 购买消息 param1:buy_news_count param2:news_pool_count
			SILK_ROADS_SIMPLE_TYPE_EXPANSION_WAREHOUSE = 4,		// 扩充仓库 param1:buy_warehouse_count 
			SILK_ROADS_SIMPLE_TYPE_BUY_SUPPLY = 5,				// 购买补给 param1:buy_supply_count
			SILK_ROADS_SIMPLE_TYPE_END = 6,						// 结束贸易之旅,结算 param1:have_coin_count
		};

		int simple_type;
		int param1;
		int param2;
	};

	class SCActivitySilkRoadsRoleAllInfo : public IMessageSerializer		//!< 1272 限时活动-丝绸之路角色所有信息
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

	class SCActivitySilkRoadsAssetsInfo		//!< 1273 限时活动-丝绸之路资产信息下发
	{
	public:
		SCActivitySilkRoadsAssetsInfo();
		MessageHeader header;

		int have_coin_count;			//当前拥有的货币(资金)
		int have_supply_count;			//当前拥有的补给
		short cur_max_grid_count;		//当前所能使用的数量(已开通的格子总数)
		short reserve_sh;
	};

	class SCActivitySilkRoadsWarehouseChange	//!< 1274 限时活动-丝绸之路仓库变化
	{
	public:
		SCActivitySilkRoadsWarehouseChange();
		MessageHeader header;

		enum NOTICE_TYPE
		{
			NOTICE_TYPE_DEFAULT = 0,				//默认
			NOTICE_TYPE_BUY_COMMODITY = 1,			//购买货物
			NOTICE_TYPE_SALE_COMMODITY = 2,			//售卖货物
			NOTICE_TYPE_REWARD = 3,					//奖励

			NOTICE_TYPE_MAX
		};

		short warehouse_grid_index;				//仓库哪个格子
		short notice_type;

		short warehouse_commodity_seq;			//该格子是哪个货物序号
		short warehouse_commodity_count;		//该格子货物数量
		float warehouse_commodity_cost;			//该格子货物成本
	};

	class SCActivitySilkRoadsAllRankInfo	//!< 1275 限时活动-丝绸之路排行榜信息下发
	{
	public:
		SCActivitySilkRoadsAllRankInfo();
		MessageHeader header;

		int rank_count;
		ActivitySilkRoadsRoleRankInfo rank_list[MAX_SILK_ROADS_RANK_NUM];
	};

	class SCActivitySilkRoadsAllTradingLogInfo	//!< 1276 限时活动-丝绸之路贸易消息列表下发
	{
	public:
		SCActivitySilkRoadsAllTradingLogInfo();
		MessageHeader header;

		short reserve_sh;
		short trading_log_count;
		ActivitySilkRoadsRoleTradingLogData trading_log_list[MAX_SILK_ROADS_TRADING_LOG_NUM];
	};

	class SCActivitySilkRoadsSingleTradingLogNotice		//!< 1277 限时活动-丝绸之路单条贸易消息下发
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