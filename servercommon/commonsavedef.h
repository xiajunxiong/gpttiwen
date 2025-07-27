#ifndef __COMMON_SAVE_DEF_H__
#define __COMMON_SAVE_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/guildhonorbattledef.hpp"
#include "servercommon/jieyidef.h"
#include "servercommon/struct/cowreportgoodnewsparam.h"
#include "servercommon/struct/tigerbringsblessingparam.hpp"
#include "servercommon/qingyuandef.h"
#include "servercommon/hongmengtianjiedef.hpp"

enum COMMON_SAVE_DATA_TYPE		//存库，不能乱加
{
	COMMON_SAVE_DATA_TYPE_INVALID = 0,
	COMMON_SAVE_DATA_TYPE_BRAVE_GROUND_TEAM_RECORD = 1,		//（1,不能合）勇闯地宫团队层数记录 param1:pass_id  ； 排行榜用  
	COMMON_SAVE_DATA_TYPE_BRAVE_GROUND_ROLE = 2,				//（2,可以合）勇闯地宫角色数据 param1:role_id			
	COMMON_SAVE_DATA_TYPE_HUAN_JIE_TIAN_TI_RECORD = 3,			//（3,可以合）幻界战场 角色离线产生天梯战报，还没同步到角色身上的数据   param1:role_id , param2: record_id 
	COMMON_SAVE_DATA_TYPE_PLATFORM_BATTLE_RECORD = 4,			//（4,可以合）擂台战战报   param1:role_id	
	COMMON_SAVE_DATA_TYPE_BRAVE_GROUND_SAVE_DATA = 5,			//（5,不能合）勇闯地宫 记录上个星期前3的人，用来发称号。单条记录 param1:0  
	COMMON_SAVE_DATA_TYPE_HUAN_JIE_SAVE_DATA = 6,					//（6,不能合）幻界战场 记录每个赛季排名靠前的人，用来发称号,游戏服会更新跨服的。单条记录 param1:0  
	COMMON_SAVE_DATA_TYPE_GUILD_FIGHT_DATA = 7,						//（7,能合）家族大乱斗数据,每个家族一周里打的积分，前16用来决定参加家族荣誉战  param1:guild_id
	COMMON_SAVE_DATA_TYPE_GUILD_HONOR_DATA = 8,					//（8,不能合）家族荣誉战数据 param1:0 单条 					
	COMMON_SAVE_DATA_TYPE_GUILD_HONOR_RECORD_OLD_DATA = 9,			//（9,不能合）旧的家族荣誉战比赛记录数据， param1:0 单条 ，（没用了，改成用14的）
	COMMON_SAVE_DATA_TYPE_PLATFORM_BATTLE_ACHIEVEMENT_DATA = 10,	//（10,能合）擂台战玩家成就数据 param1:role_id	
	COMMON_SAVE_DATA_TYPE_PLATFORM_BATTLE_OTHER_DATA = 11,			//（11,不能合）擂台战数据	param1:0 单条 
	COMMON_SAVE_DATA_TYPE_GIVE_GIFT = 12,									//（12,可以和）赠送礼物
	COMMON_SAVE_DATA_TYPE_RA_DELAY_SET_TIME_DATA = 13,			//!< (13,不能合) 延迟设置随机活动时间数据 param1:活动id  param2:开始时间戳
	COMMON_SAVE_DATA_TYPE_GUILD_HONOR_RECORD_DATA = 14,			//（14,不能合）家族荣誉战比赛记录数据 param1:0 单条 
	COMMON_SAVE_DATA_TYPE_JIE_YI_ZU = 15,						//（15,可以合）结义组 param1+ param2==结义组id
	COMMON_SAVE_DATA_TYPE_JIE_YI_ROLE_XUAN_YAN = 16,						//（16,可以合）结义宣言
	COMMON_SAVE_DATA_TYPE_JIE_YI_ZU_XUAN_YAN = 17,						//（17,可以合）结义组宣言
	COMMON_SAVE_DATA_TYPE_COW_REPORT_GOOD_NEWS_DATA = 18,		// (18, 不能合)  福牛报喜服务器数据 , param1 :0 单条
	COMMON_SAVE_DATA_TYPE_TIGER_BRINGS_BLESSING_DATA = 19,		// (19, 不能合)  寅虎纳福服务器数据 , param1 :0 单条
	COMMON_SAVE_DATA_TYPE_QING_YUAN_DATA = 20,					// (20, 可以合)  情缘数据 param1+ param2==情缘id
	COMMON_SAVE_DATA_TYPE_HONGMENG_TIANJI = 21,					// (21, 不能合)	鸿蒙天界 全服首通记录

	COMMON_SAVE_DATA_TYPE_MAX,
};

unsigned int GetCommonSaveDataLengthByType(int save_type);

//---------------------以下是存库结构，注意对齐--------------------------------------
#pragma pack(push, 4)
//--------------------------勇闯地宫-------------------
struct CommonSaveBraveGroundRoleData
{
	void Reset();

	int role_id;
	int reached_level;
};

struct CommonSaveBraveGroundRankRoleInfoItem
{
	int role_id;
	GameName role_name;
};

struct CommonSaveBraveGroundTeamReceordData
{
	void Reset();

	unsigned int time;
	int reached_level;
	int record_key;
	int role_info_count;
	CommonSaveBraveGroundRankRoleInfoItem role_info[5];
};

const static int BRAVE_GROUND_TOP_ITEM_NUM = 3;
const static int BRAVE_GROUND_TOP_ROLE_NUM = 5;

struct CommonSaveBraveGroundData
{
	void Reset();

	int top_role_id[BRAVE_GROUND_TOP_ITEM_NUM][BRAVE_GROUND_TOP_ROLE_NUM];
};

//--------------------------幻界-------------------

static const int  HUAN_JIE_ZHAN_CHANGTEAM_ROLE_INFO_ITEM_MAX_NUM = 3;

struct HuanJieZhanChangRecordRoleItem
{
	void Reset();

	int role_id;
	int role_level;
	int rank_score;
	GameName role_name;
	short avatar_type;
	short prof;
	short headshot_id;
	short is_leader;
	unsigned char is_ding_ding;
	char reserved_ch;
	short reserved_sh;

	unsigned int shanghai;
	unsigned int zhi_liao;
	unsigned int chengshou_shanghai;
};

struct CommonSaveHuanJieTianTiReceordData
{
	void Reset();
	bool IsEmpty() const;

	HuanJieZhanChangRecordRoleItem role_info_1[HUAN_JIE_ZHAN_CHANGTEAM_ROLE_INFO_ITEM_MAX_NUM];
	HuanJieZhanChangRecordRoleItem role_info_2[HUAN_JIE_ZHAN_CHANGTEAM_ROLE_INFO_ITEM_MAX_NUM];

	unsigned int time;
	unsigned char fight_round;
	unsigned char fight_result;
	short add_score;
	unsigned short add_huan_jie_coin;
	unsigned char is_ding_ji;
	unsigned char is_protect;
};

const static int HUAN_JIE_RANK_TITLE_MAX_NUM = 50;  //幻界战场获得排名称号的最大人数
struct CommonSaveHuanJieData
{
	void Reset();

	int role_id[HUAN_JIE_RANK_TITLE_MAX_NUM];			//记录每赛季结束前面的人，用来发称号
};

//--------------------------擂台战-------------------

const static int PLATFORM_BATTLE_RECORD_ITEM_MAX_NUM = 50;
const static int PLATFORM_BATTLE_MAX_RANK_REWARD = 2;				//守擂中擂台能拿奖励的擂台个数
struct CommonSavePlatformBattleRecordRoleInfo
{
	void Reset();

	int role_id;
	int role_level;
	GameName role_name;
	short avatar_type;
	short prof;
	short headshot_id;
	short reserve_sh;			//不能用了忘记初始化了
};

struct CommonSavePlatformBattleRoleRecord
{
	void Reset();

	CommonSavePlatformBattleRecordRoleInfo role_info_1;
	CommonSavePlatformBattleRecordRoleInfo role_info_2;

	unsigned int time;
	short is_win;
	short platform_num;
	short curr_rank;		//当前排名
	short last_rank;		//上次排名	
};

struct CommonSavePlatformBattleLastRankItem
{
	void Reset();

	short platform_num;
	short rank;
};

struct CommonSavePlatformBattleReceordData
{
	void Reset();

	int role_id;	
	unsigned char is_first_times;		//有没有玩过擂台，非0是没玩过
	unsigned char lose_record_flag;		//每次擂台排名降低，需要红点提示
	unsigned short record_index;
	CommonSavePlatformBattleRoleRecord records[PLATFORM_BATTLE_RECORD_ITEM_MAX_NUM];
	CommonSavePlatformBattleLastRankItem last_rank_info[PLATFORM_BATTLE_MAX_RANK_REWARD];    //上次结算时排名信息
};

//擂台战成就
enum PLATFORM_BATTLE_ACHIEVEMENT
{
	PLATFORM_BATTLE_ACHIEVEMENT_ANY_TOP_X = 0,				//任何一个擂台,进入前n名
	PLATFORM_BATTLE_ACHIEVEMENT_BA_ZHU_TOP_1_NUM = 1,		// 在霸主擂台登顶n个擂台
	PLATFORM_BATTLE_ACHIEVEMENT_QING_LONG_TOP_1_NUM = 2,	// 在青龙擂台登顶n个擂台
	PLATFORM_BATTLE_ACHIEVEMENT_BAI_HU_TOP_1_NUM = 3,		// 在白虎擂台登顶n个擂台
	PLATFORM_BATTLE_ACHIEVEMENT_ZHU_QUE_TOP_1_NUM = 4,		// 在朱雀擂台登顶n个擂台
	PLATFORM_BATTLE_ACHIEVEMENT_XUAN_WU_TOP_1_NUM = 5,	// 在玄武擂台登顶n个擂台
	PLATFORM_BATTLE_ACHIEVEMENT_ANY_ACK_TIMES = 6,		//在任意擂台挑战获胜5次
	PLATFORM_BATTLE_ACHIEVEMENT_ANY_DEF_TIMS = 7,			//在任意擂台守擂获胜5次
	PLATFORM_BATTLE_ACHIEVEMENT_TOP_1_DAY = 8,		//在任意擂台持续登顶超过N天
	PLATFORM_BATTLE_ACHIEVEMENT_PARTNER_NUM = 9,		//上阵擂台伙伴总数超过N人
	PLATFORM_BATTLE_ACHIEVEMENT_PET_NUM = 10,			//上阵擂台宠物总数超过n个
	PLATFORM_BATTLE_ACHIEVEMENT_CAPACITY_TIMES = 11,	//战胜综合评分高于自己的玩家3次
	PLATFORM_BATTLE_ACHIEVEMENT_BA_ZHU_TOP_X = 12,		// 在霸主擂台,进入前n名
	PLATFORM_BATTLE_ACHIEVEMENT_QING_LONG_TOP_X = 13,	// 在青龙擂台,进入前n名
	PLATFORM_BATTLE_ACHIEVEMENT_BAI_HU_TOP_X = 14,		// 在白虎擂台,进入前n名
	PLATFORM_BATTLE_ACHIEVEMENT_ZHU_QUE_TOP_X = 15,		// 在朱雀擂台,进入前n名
	PLATFORM_BATTLE_ACHIEVEMENT_XUAN_WU_TOP_X = 16,	// 在玄武擂台,进入前n名
	PLATFORM_BATTLE_ACHIEVEMENT_MAX,
};

struct CommonSavePlatformBattleAchievementItem
{
	void Reset();


	unsigned short num;
	unsigned short fetch_flag;
};

bool IsPlatformBattleRankAchievementType(int type);			//是否排行榜类型，用于当前完成数 要小于等于 配置的参数

struct  CommonSavePlatformBattleAchievementData
{
	void Reset();
	bool SetTypeProcess(int type, int num);
	bool SetTypeRankProcess(int type, int rank);
	void AddTypeProcess(int type, int num = 1);
	int GetTypeProcessNum(int type);
	CommonSavePlatformBattleAchievementItem * GetCommonSavePlatformBattleAchievementItem(int type);
	
	CommonSavePlatformBattleAchievementItem achievement_info[PLATFORM_BATTLE_ACHIEVEMENT_MAX];
};

static const int COMMON_SAVE_PLATFORM_BATTLE_OTHER_DATA_VERSION = 1;

struct  CommonSavePlatformBattleOtherData
{
	void Reset();
	
	long long  achievement_next_reset_time;			//成就重制时间，每两个月全服统一重置
	int reset_version;
};

//--------------------------家族大乱斗-------------------
struct CommonSaveGuildFightRecord
{
	void Reset();

	unsigned int time;
	int score;
};

struct CommonSaveGuildFightData				//家族大乱斗积分排名，算出两次大乱斗积分总和得出前16名荣誉战参赛名单
{
	void Reset();

	int guild_id;
	int server_id;
	GuildName guild_name;
	GuildBanner banner;
	GameName patriarch_name;
	unsigned int join_guild_fight_day;
	CommonSaveGuildFightRecord records[7];			//一周每天家族大乱斗战绩
};
//--------------------------家族荣誉战-------------------
struct CommonSaveGuildHonorRecordOldDataItem
{
	void Reset();

	int number;
	int match_round;
	int match_round_2;		//match_round_2
	int guild_fight_rank;

	int guild_id;
	int server_id;
	GuildName guild_name;
	GuildBanner banner;
	GameName patriarch_name;
	int champion_guess_num;
	int guess_num[4];		//每轮支持的人数
	unsigned short wday_flag;			//暂时没用
	unsigned short score_records[7];			//一周每天家族大乱斗战绩
};

struct CommonSaveGuildHonorRecordOldData
{
	void Reset();

	CommonSaveGuildHonorRecordOldDataItem record_item[16];
};

enum GUILD_HONOR_BATTLE_PROCESS
{
	GUILD_HONOR_BATTLE_PROCESS_WEEK_START = 0,			//每周开始
	GUILD_HONOR_BATTLE_PROCESS_GUILD_FIGHT,		//本周开始第一次大乱斗
	GUILD_HONOR_BATTLE_PROCESS_READY,				//大于名单时间
	GUILD_HONOR_BATTLE_PROCESS_FINISH,			//大于名单时间，荣誉战结束
	GUILD_HONOR_BATTLE_PROCESS_MAX,			
};


struct CommonSaveGuildHonorData				//家族大乱斗积分排名，算出两次大乱斗积分总和得出前16名荣誉战参赛名单
{
	void Reset();
	
	int guild_honor_process;
	unsigned int settlement_list_time;   //结算名单时间
};

struct CommonSaveGuildHonorRecordDataItem
{
	void Reset();

	short start_first_round;
	short win_rank;
	short finish_rank;
	short number;
	int match_round;
	int guild_fight_rank;

	int guild_id;
	int server_id;
	GuildName guild_name;
	GuildBanner banner;
	GameName patriarch_name;
	int champion_guess_num;
	int guess_num[GUILD_HONOR_BATTLE_MAX_ROUND];		//每轮支持的人数
	unsigned short wday_flag;			//暂时没用
	unsigned short score_records[7];			//一周每天家族大乱斗战绩
}; 

struct CommonSaveGuildHonorRecordData				
{
	void Reset();

	CommonSaveGuildHonorRecordDataItem record_item[42];
};
//------------------------赠送礼物-------------------------------

const static int COMMON_SAVE_GIVE_GIFT_STR_NUM = 64;

struct CommonSaveGiveGiftItemData
{
	void Reset();

	unsigned short item_id;
	unsigned short num;
	unsigned int invaild_time;
};

struct CommonSaveGiveGiftData
{
	void Reset();

	long long uuid;
	int from_role_id;
	int to_role_id;
	unsigned int fetch_time;
	unsigned int time;
	char str[COMMON_SAVE_GIVE_GIFT_STR_NUM];
	CommonSaveGiveGiftItemData item;
	int time_group_id;
};
std::string CommonSaveGiveGiftDataToString(const CommonSaveGiveGiftData & other);

//-----------------------------------------------------------------随机活动设置时间--------------------------------------
struct CommonSaveRADelaySetTimeData
{
	void Reset()
	{
		is_force = 0;
		reserve_sh = 0;
		rand_activty_type = 0;
		begin_time = 0;
		end_time = 0;
		open_type = 0;
		delay_to_time = 0;
		memset(reserve_ll, 0, sizeof(reserve_ll));
	}

	short is_force;
	short reserve_sh;
	int rand_activty_type;
	unsigned int begin_time;
	unsigned int end_time;
	int open_type;
	unsigned int delay_to_time; // 延时到何时设置时间
	long long reserve_ll[2];
};

//-----------------------------------------------------------------结义组--------------------------------------
struct CommonSaveJieYiZuRoleInfoItem
{
	void Reset();

	int role_id;
	GameName role_name;
	short level;
	short avatar_type;
	short prof;
	unsigned char is_zu_zhang;
	unsigned char has_fisrt_member_name;		//是否已经改过成员名字，第一次免费
	int headshot_id;
	unsigned int  join_time;
	unsigned int leave_end_time;		//到时间就离开， 申请离开时间, ②点击割袍断义，弹出确认弹窗，若玩家点击确认则进入24小时冷静期
	unsigned last_login_time;
	JieYIZuMemberName member_name;	
	char is_vote;			//是否已经投票过
	unsigned char tan_he_number_of_votes;	//弹劾 投票获取票数
	char reserved_arr[62];
};

struct CommonSaveJieYiZu	
{
	void Reset();

	long long jie_yi_zu_id;
	long long create_time;
	JieYIZuName jie_yi_name;
	JieYIZuText jie_yi_text;		

	CommonSaveJieYiZuRoleInfoItem role_info[JIE_YI_ZU_MAX_ROLE_ITEM_NUM];

	unsigned int tan_he_vote_end_time;		//弹劾组长投票结束时间
};

struct CommonSaveJieYiRoleXuanYan
{
	void Reset();

	int role_id;
	unsigned int time;
	JieYIZuXuanYanText  xuan_yan_text;
};

struct CommonSaveJieYiZuXuanYan
{
	void Reset();

	long long jie_yi_zu_id;
	unsigned int time;
	JieYIZuXuanYanText  xuan_yan_text;
};





// 福牛报喜
struct CommonSaveCowReportGoodNewsData
{
	void Reset();

	int curren_status;				// 当前的活动状态

	int buy_start_times;			// 购买开启时间
	int buy_end_times;				// 购买结束时间
	int show_start_times;			// 展示期开启时间
	int	show_end_times;				// 展示期结束时间

	CowReportGoodNewsRewardUserInfo reward_user_info[MAX_COW_REPORT_GOOD_NEWS_REWARD_USER_COUNT];	// 得奖玩家信息
};

// 寅虎纳福
struct CommonSaveTigerBringsBlessingData
{
	void Reset();

	int curren_status;				// 当前的活动状态

	int buy_start_times;			// 购买开启时间
	int buy_end_times;				// 购买结束时间
	int show_start_times;			// 展示期开启时间
	int	show_end_times;				// 展示期结束时间

	TigerBringsBlessingRewardUserInfo reward_user_info[MAX_TIGER_BRINGS_BLESSING_REWARD_USER_COUNT];	// 得奖玩家信息
};

//----------------------------------------------------------------情缘--------------------------------------
struct CommonSaveQingYuanRoleInfoItem
{
	void Reset();


	int role_id;
	GameName role_name;
	short level;
	short avatar_type;
	short prof;
	short headshot_id;	
	unsigned int last_login_time;	

	int gift_type;			//开通月卡类型
	unsigned int fetch_gift_flag;		// bit 0 是否领取开通奖励，bit 1 是否领取今天奖励
	unsigned int gift_type_end_time;		//开通月卡结束时间
	unsigned int leave_end_time;		//单方面离婚或者强制离婚时间
	unsigned long long skill_no_effect_flag;		//情缘技能设置是否生效标志

	char reserved_arr[64];
};

struct CommonSaveQingYuanCommonData
{
	void Reset();

	unsigned int ra_ra_shizhuang_join_timestamp;
	int ra_qingyuan_shizhuang_buy_mark;
	unsigned int ra_qingyuan_leichong_begin_timestamp;
	unsigned int ra_qingyuan_leichong_end_timestamp;
	int ra_qingyuan_leichong_chongzhi_gold;
};

struct CommonSaveQingYuan
{
	void Reset();

	long long qing_yuan_id;
	int has_hun_yan;
	unsigned int create_time;	
	unsigned int leave_end_time;		//协议离婚时间
	int qing_yuan_duration_reward_already_fetch_day;
	CommonSaveQingYuanRoleInfoItem role_info[QING_YUAN_MAX_ROLE_NUM];
	CommonSaveQingYuanCommonData common_data;
};

struct CommonSaveWorldHongMengTianJie
{
	void Reset();

	HongMengTianJie::WorldFBData first_pass[HongMengTianJie::MAX_FB_INDEX_NUM];		// 全服副本首通记录
};

//------------------------------------------------------------------
union CommonSaveData
{
	void Reset(int save_type);

	CommonSaveBraveGroundTeamReceordData brave_ground_team_record_data;
	CommonSaveBraveGroundRoleData brave_ground_role_data;	
	CommonSaveHuanJieTianTiReceordData huan_jie_tian_ti_record_data;
	CommonSavePlatformBattleReceordData platform_battle_record_data;
	CommonSaveBraveGroundData brave_ground_data;
	CommonSaveHuanJieData        huan_jie_data;
	CommonSaveGuildFightData guild_fight_data;
	CommonSaveGuildHonorRecordOldData guild_honor_old_data;
	CommonSaveGuildHonorRecordData guild_honor_record_data;
	CommonSavePlatformBattleAchievementData platform_battle_achievement_data;
	CommonSavePlatformBattleOtherData platform_battle_other_data;
	CommonSaveGiveGiftData give_gift_data;

	CommonSaveRADelaySetTimeData ra_delay_set_time_data;
	CommonSaveGuildHonorData guild_honor_data;
	CommonSaveJieYiZu jie_yi_zu_data;
	CommonSaveJieYiRoleXuanYan jie_yi_role_xuan_yan_data;
	CommonSaveJieYiZuXuanYan jie_yi_zu_xuan_yan_data;
	CommonSaveCowReportGoodNewsData cow_report_good_news_data;
	CommonSaveTigerBringsBlessingData tiger_brings_blessing_data;
	CommonSaveQingYuan qing_yuan_data;
	CommonSaveWorldHongMengTianJie world_hongmeng_tianji_data;
};

typedef char CommonSaveDataHex[sizeof(CommonSaveData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(CommonSaveDataHex) < 18192);

class CommonSaveDataParam
{
public:
	bool Serialize(TLVSerializer & outstream) const;
	bool Unserialize(TLVUnserializer & instream);

	struct DBCommonSaveDataItem
	{
		DBCommonSaveDataItem()
		{
			this->Reset(0);
		}

		void Reset(int save_type)
		{
			inc_id = 0;
			change_state = 0;
			save_data_type = COMMON_SAVE_DATA_TYPE_INVALID;
			param1 = 0;
			param2 = 0;
			param3 = 0;

			data.Reset(save_type);
		}

		long long inc_id;
		char change_state;
		int save_data_type;
		int param1;
		int param2;
		int param3;

		CommonSaveData data;
	};

	int count;
	DBCommonSaveDataItem data_list[100];
};

#pragma pack(pop)

#endif
