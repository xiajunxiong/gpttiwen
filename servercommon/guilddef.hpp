#ifndef __GUILD_DEF_HPP__
#define __GUILD_DEF_HPP__

#include <memory>
#include <map>
#include <vector>

#include "servercommon/struct/structcommon.h"
#include "servercommon.h"
#include "bitmapfunction.hpp"
	
static const GuildID INVALID_GUILD_ID = 0;						// 无效的军团ID
static const int GUILD_MAX_MEMBER_COUNT = 140;
static const int GUILD_BUILD_COUNT = 4;
static const int CREATE_GUILD_LEVEL_LIMTI = 12;
static const int MAX_ONCE_GUILD_COUNT = 10;						// 一次操作最大军团数量
static const int GUILD_EVENTHANDLER_COUNT = 20;					// 事件数量
static const int GUILD_EVENTHANDLER_PARAM = 100000;				// 事件处理参数
static const int GUILD_SAVE_INTERVAL = 60;
static const int PLAYER_APPLY_COUNT_MAX = 5;
static const int GUILD_JOURNAL_ADD_NUM = 10;					//家族日志中打造装备灵饰和锢魔之塔增长皆为10
static const int MAX_GUILD_REWARD_GROUP_ITEM_NUM = 5;			
static const int MAX_GUILD_BOSS_CHAPTER_RANK_NUM = 10;			// 家族试炼排名奖励每个章节拥有最大数量
static const int MAX_GUILD_CHAPTER_BOSS_NUM = 30;				// 家族试炼章节最大数量
static const int MAX_GUILD_GATHER_DREAM_NUM = 50;				// 家族集愿发布最大数量
static const int GUILD_SEVEN_DAY_ACTIVE = 7;					// 家族七日活跃
static const int GUILD_HALL_OF_FAME_REFRESH_TAMP = 5;			// 名人堂排行刷新间隔
static const int GUILD_CONSTRUCT_BUILDING = 6;					// 家族建筑最大数量+1[大厅，书房，厢房，金库，商铺],和配置对齐从下标1开始,0下标位置可留作保留字
UNSTD_STATIC_CHECK((GUILD_CONSTRUCT_BUILDING % 2) == 0);

//type * GUILD_EVENTHANDLER_PARAM + param
enum GUILD_EVENTHANDLER_TYPE
{
	GUILD_EVENTHANDLER_ADD_MEMBER = 1,						// xx加入了家族
	GUILD_EVENTHANDLER_DEC_MEMBER = 2,						// xx离开了家族
	GUILD_EVENTHANDLER_APPOINTMENT = 3,						// xx被任命为GUILD_POST_TYPE
	GUILD_EVENTHANDLER_TYPE_XUNBAO = 4,						//寻宝获得宠物	param  - pet_id
	GUILD_EVENTHANDLER_TYPE_ADD_PET = 5,					//获得宠物(除寻宝)  param - pet_id
	GUILD_EVENTHANDLER_TYPE_COURSE_OF_GROWTH = 6,			//成就达到哪一阶段的1星  param - cur_prog
	GUILD_EVENTHANDLER_TYPE_WORK_SHOP_COMPOUND = 7,			//工坊打造   param - item_id
	GUILD_EVENTHANDLER_TYPE_WORK_SHOP_CRAFT_JEWELRY = 8,	//打造灵饰   param - jewelry_id
	GUILD_EVENTHANDLER_TYPE_MIYI = 9,						//通过试炼之地  param - level
	GUILD_EVENTHANDLER_TYPE_COURAGE_CHALLENGE = 10,			//通过锢魔之塔第几层   10层增长 param - pass_layer
	GUILD_EVENTHANDLER_TYPE_GUILD_LEVEL_UP = 11,			//家族升级	param - guild_level

	GUILD_EVENTHANDLER_TYPE_MAX,
};

UNSTD_STATIC_CHECK(GUILD_EVENTHANDLER_TYPE_MAX == 12);

enum GUILD_POST_TYPE
{
	GUILD_POST_PATRIARCH = 0,								// 族长
	GUILD_POST_VICE_PATRIARCH,								// 副族长
	GUILD_POST_ELDER,										// 长老
	GUILD_POST_CADRE,										// 骨干
	GUILD_POST_ELITE,										// 精英
	GUILD_POST_MEMBER,										// 成员

	GUILD_POST_MAX,
};
UNSTD_STATIC_CHECK(GUILD_POST_MAX < 32);

//家族集愿类型
enum GUILD_GATHER_DREAM_ITEM_TYPE
{
	GUILD_GATHER_DREAM_ITEM_TYPE_PET = 0,			//宠物
	GUILD_GATHER_DREAM_ITEM_TYPE_PARTNER = 1,		//伙伴

	GUILD_GATHER_DREAM_ITEM_TYPE_MAX
};
UNSTD_STATIC_CHECK(GUILD_GATHER_DREAM_ITEM_TYPE_MAX == 2);

#pragma pack(push, 4)

struct GuildApplyUserInfo
{
	GuildApplyUserInfo()
	{
		uid = 0;
		avatar_timestamp = 0;
		level = 0;
		top_level = 0;
		profession = 0;
		memset(name, 0, sizeof(name));
		timeout_timestamp = 0;
		avatar_type = 0;
		baodi_fetch_flag_OBSOLETE = 0;
		rank_fetch_flag = 0;
		baodi_reward_flag = 0;
	}

	int					uid;
	long long			avatar_timestamp;								//!< 玩家自上传头像
	int					level;											//!< 等级
	int					top_level;										//!< 巅峰等级
	int					profession;										//!< 职业
	GameName			name;											//!< 名字
	long long			timeout_timestamp;								//!< 超时时间
	short				avatar_type;									//!< 头像类型
	char				baodi_fetch_flag_OBSOLETE;						//!< 玩家今天是否已领取保底奖励	0:未领取 1:已领取
	char				rank_fetch_flag;								//!< 玩家今天是否已领取了试炼伤害排名奖励 0:未领取 1:已领取
	int					baodi_reward_flag;								//!< 保底奖励标记
};

struct SynGuildGatherDreamGiftInfo
{
	SynGuildGatherDreamGiftInfo(): guild_id(0), gift_role_uid(0), get_role_uid(0), index(0), item_id(0), quality(0), gather_dream_type(0), gift_post(0),get_post(0)
	{}

	int guild_id;
	int gift_role_uid;
	int get_role_uid;
	short index;				//发布列表下标
	ItemID item_id;
	short quality;	
	short gather_dream_type;
	short gift_post;
	short get_post;
};


struct GuildGatherDreamInfo			//家族集愿信息
{
	GuildGatherDreamInfo() { this->Reset(); }

	void Reset()
	{
		memset(from_name, 0, sizeof(from_name));
		role_id = 0;
		item_id = 0;
		headshot_id = 0;
		gather_num = 0;
		fetch_num = 0;
		need_max_num = 0;
		quality = 0;
		avatar_type = 0;
		gather_dream_type = 0;
		pulish_timestamp = 0;
	}

	GameName from_name;				//发布玩家名字
	int role_id;					//发布玩家ID
	ItemID item_id;					//集愿宠物碎片道具ID
	short headshot_id;				//发布玩家头像
	char gather_num;				//此时集愿进度
	char fetch_num;					//玩家已领取数量
	char need_max_num;				//所需的最大数量
	char quality;					//品质
	short avatar_type;
	short gather_dream_type;		//集愿类型
	unsigned int pulish_timestamp;	//发布时间
};

struct GuildGatherDreamData			//家族集愿协议下发数据
{
	GuildGatherDreamData() { this->Reset(); }
	GuildGatherDreamData(const GuildGatherDreamInfo info)
	{
		this->Reset(); 
		F_STRNCPY(from_name, info.from_name, sizeof(from_name));
		role_id = info.role_id;
		item_id = info.item_id;
		headshot_id = info.headshot_id;
		gather_num = info.gather_num;
		fetch_num = info.fetch_num;
		need_max_num = info.need_max_num;
		avatar_type = info.avatar_type;
		gather_dream_type = info.gather_dream_type;
		pulish_timestamp = info.pulish_timestamp;
	}
	void Reset()
	{
		memset(from_name, 0, sizeof(from_name));
		role_id = 0;
		item_id = 0;
		headshot_id = 0;
		gather_num = 0;
		fetch_num = 0;
		need_max_num = 0;
		help_num = 0;
		avatar_type = 0;
		gather_dream_type = 0;
		pulish_timestamp = 0;
	}

	GameName from_name;				//发布玩家名字
	int role_id;					//发布人
	ItemID item_id;					//集愿碎片道具ID
	short headshot_id;				//发布玩家头像
	char gather_num;				//此时集愿进度
	char fetch_num;					//玩家已领取数量
	char need_max_num;				//所需的最大数量
	char help_num;					//玩家帮助次数
	short avatar_type;
	short gather_dream_type;		//集愿类型	
	unsigned int pulish_timestamp;	//发布时间
};
class GuildGatherDreamParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	GuildGatherDreamParam() { this->Reset(); }

	void Reset()
	{
		count = 0;
		record_index = 0;
		for (int i = 0; i < GUILD_MAX_MEMBER_COUNT; i++)
		{
			data_list[i].Reset();
		}
	}

	void DayReset()
	{
		count = 0;
		for (int i = 0; i < GUILD_MAX_MEMBER_COUNT; i++)
		{
			data_list[i].Reset();
		}
	}

	int count;
	int record_index;		//每产生一个新记录则自增
	GuildGatherDreamInfo data_list[GUILD_MAX_MEMBER_COUNT];
};
typedef char GuilGatherDreamParamHex[sizeof(GuildGatherDreamParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(GuilGatherDreamParamHex) < 32768);

class AllGuildGathersDreamParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	AllGuildGathersDreamParam() { this->Reset(); }

	void Reset()
	{
		count = 0;
		for (int i = 0; i < MAX_ONCE_GUILD_COUNT; i++)
		{
			guild_gather_dream_list[i].Reset();
		}
	}

	int count;
	GuildGatherDreamParam guild_gather_dream_list[MAX_ONCE_GUILD_COUNT];
};

struct InfoTypeReserve					//保留,以保持此时家族数据不会错乱
{
	InfoTypeReserve() { this->Reset(); }

	void Reset()
	{
		item_id = 0;
		column = 0;
		index = 0;
		num = 0;
		is_bind = 0;
		has_param = 0;
		param_length = 0;
		invalid_time = 0;
		memset(param_data, 0, sizeof(param_data));
	}
	ItemID			item_id;			//!< 物品id
	short			column;				//!< 物品在哪个背包栏中 [0,9)
	short			index;				//!< 物品在背包中的序号
	short			num;				//!< 物品数量
	char			is_bind;			//!< 是否绑定  1为绑定 0为非绑
	char			has_param;			//!< 是否有净值参数 有为1，无为0   \note 净值就是像装备那样有自己的数值，每件物品都需要看成不一样的
	short			param_length;		//!< 净值参数的数据长度 （当has_param为1时可读）
	UInt32		    invalid_time;		//!< 过期时间
	NetValueItemParamData param_data;	//!< 净值参数的数据，根据param_length的值来读取 （当has_param为1时可读）
};

struct GuildEventhandlerParam
{
	GuildEventhandlerParam() { this->Reset(); }
	void Reset()
	{
		eventhandler_type = 0;
		eventhandler_time = 0;
		memset(eventhandler_name, 0, sizeof(eventhandler_name));
		item_info.Reset();
	}
	int eventhandler_type;				// 保存的事件 type*GUILD_EVENTHANDLER_PARAM + param
	unsigned int eventhandler_time;		// 事件发生时间
	GameName eventhandler_name;
	InfoTypeReserve item_info;					//仅当事件与道具有关时才有数据
};

struct GuildEventhandlerCountParam
{	
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);
	
	GuildEventhandlerCountParam() { this->Reset(); }

	void Reset()
	{
		count = 0;
		for (int i = 0; i < GUILD_EVENTHANDLER_COUNT; i++)
		{
			guild_eventhandler_list[i].Reset();
		}
	}
	
	int count;
	GuildEventhandlerParam guild_eventhandler_list[GUILD_EVENTHANDLER_COUNT];
};

typedef char GuildEventhandlerParamHex[sizeof(GuildEventhandlerCountParam) * 2 + 1];
//UNSTD_STATIC_CHECK(sizeof(GuildEventhandlerParamHex) < 34200);

struct GuildBossParam		//这个结构整个保存，注意4字节对齐
{
	const int static BOSS_HP_PERCENT = 10000;
	const int static MONSTER_NUM = 10;

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	GuildBossParam() { this->Reset(); }
	void Reset()
	{
		boss_index = 0;
		chapter_id = 0;
		select_chapter_id_OBSOLETE = -1;
		remain_hp_percent = BOSS_HP_PERCENT;
		max_chapter_OBSOLETE = 0;
		for (int i = 0; i < MONSTER_NUM; ++i)
		{
			monster_remain_percent_list[i] = (short)BOSS_HP_PERCENT;
		}
		kill_timestamp = 0;
		reset_flag = 0;
		reserve_ch = 0;
		week_kill_boss_num = 0;
		reserve_ui = 0;
	}

	int boss_index;				//!< 这章节的哪个boss, 每击败一个boss数值+1
	int chapter_id;					//!< 今天的章节(章节每天会根据世界等级而定）
	int select_chapter_id_OBSOLETE;	//!< 会长选择第二天挑战哪个章节
	int remain_hp_percent;			//!< 仅用于下发协议
	int max_chapter_OBSOLETE;		//!< 最高通关章节
	short monster_remain_percent_list[MONSTER_NUM];
	unsigned int kill_timestamp;	//!< 今天击杀BOSS的时间戳
	char reset_flag;
	char reserve_ch;				//保留字
	short week_kill_boss_num;		//本周击杀boss数
	unsigned int reserve_ui; //!< boss每周更新时间戳
};
typedef char GuildBossParamHex[sizeof(GuildBossParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(GuildBossParamHex) < 128);

class GuildBaseParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	GuildBaseParam() { this->Reset(); }
	void Reset()
	{
		guild_id = 0;
		create_timestamp = 0;
		memset(guild_name, 0, sizeof(guild_name));
		memset(guild_notice, 0, sizeof(guild_notice));
		memset(guild_banner, 0, sizeof(guild_banner));
		guild_level = 0;
		guild_exp = 0;
		build_flag = 0;
		memset(build_value, 0, sizeof(build_value));
		protect_timestamp = 0;
		guild_honor_title_id = 0;
		origin_plat_type = 0;
		origin_server_id = 0;
		is_need_approval = 0;

		today_set_banner_flag = 0;
		reset_guild_eventhandler = 0;
		reset_guild_eventhandler_move_flag = 0;
		can_tan_he = 0;
		guild_fight_title_id = 0;
		tan_he_zu_zhang_start_time = 0;
		guild_fight_rank = 0;
		guild_fight_buff_rank = 0;
		memset(reserve_ch, 0, sizeof(reserve_ch));		
		day_total_guild_active_val = 0;
		week_total_guild_active_val = 0;
		last_guild_active_change_times = 0;
		is_send_will_overflow_mail = 0;
		is_send_overflow_mail = 0;
		reserve_ch2 = 0;
		single_building_finish_timestamp = 0;
		can_speed_up = 0;
		opener_post = 0;
		building_count = 0;

		memset(seven_day_active, 0, sizeof(seven_day_active));	

		is_open_qifu = 0;
		qifu_value = 0;

		memset(build_level, 0, sizeof(build_level));
		memset(reserve_uint6, 0, sizeof(reserve_uint6));
		memset(opener_name, 0, sizeof(opener_name));

	}

	GuildID guild_id;
	unsigned int create_timestamp;
	int week_total_guild_active_val;			// 帮派周活跃度(帮派成员本周内获得的总活跃度)每周重置
	GuildName guild_name;
	GuildNotice guild_notice;

	int guild_level;
	long long guild_exp;

	int build_flag;
	int build_value[GUILD_BUILD_COUNT];
	UInt32 protect_timestamp;		// 家族降级保护到期时间
	int guild_honor_title_id;			//没用了
	int origin_plat_type;
	int origin_server_id;
	int is_need_approval;

	GuildBanner guild_banner; //族徽

	char today_set_banner_flag;
	char reset_guild_eventhandler;				//保留
	char reset_guild_eventhandler_move_flag;	//保留
	char can_tan_he;
	int guild_fight_title_id;		//家族大乱斗称号
	unsigned int tan_he_zu_zhang_start_time;
	unsigned short guild_fight_rank;		//每周第一次工会战排名,家族buff机制要用到,没用了
	unsigned short guild_fight_buff_rank;		//每周第一次工会战排名,家族buff机制要用到，没用了，下次结算改成用世界状态那边的值了
	int day_total_guild_active_val;				// 帮派日活跃度(帮派成员本日内获得的总活跃度)每日重置
	unsigned int last_guild_active_change_times;	// 上一次帮派活跃度发生变更的时间戳

	int seven_day_active[GUILD_SEVEN_DAY_ACTIVE];			// 7日活跃

	char reserve_ch[148];			//保留字
	char is_open_qifu;	//是否开启祈福
	char is_send_will_overflow_mail;	//今日发送过资金即将溢出邮件
	char is_send_overflow_mail;			//今日发送过资金满邮件
	char reserve_ch2;
	int qifu_value;		//祈福进度

	short build_level[GUILD_CONSTRUCT_BUILDING];		// [1-5],下标0-当前正在建造的建筑号
	unsigned int reserve_uint6[6];
	unsigned int single_building_finish_timestamp;		// 建筑升级完成时间戳
	short quicken_building_count;						// 今日已开启加速次数
	short can_speed_up;									// 是否可加速
	GameName opener_name;								// 加速开启人的名字
	int opener_post;									// 开启人职位
	int building_count;									// 每日已建造次数

};
typedef char GuildBaseParamHex[sizeof(GuildBaseParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(GuildBaseParamHex) < 15360);

/* =========================================== 家族成员表 =========================================== */ 
struct GuildMemberData			//这个结构整个保存，注意4字节对齐
{
	GuildMemberData()
	{
		this->Reset();
	}

	void Reset()
	{
		guild_boss_total_damage = 0;
		guild_boss_join_flag = 0;
		guild_task_contribute = 0;
		last_change_task_contribute_time = 0;
		guild_boss_fetch_chapter_reward_times = 0;
		is_fetch_boss_chapter_baodi_reward_OBSOLETE = 0;
		is_fetch_boss_chapter_damage_rank_reward = 0;
		tan_he_join_flag = 0;
		is_qifu_get = 0;
		week_active_val = 0;
		last_change_week_active_val_times = 0;
		guild_day_active_reawrd_flag = 0;

		memset(seven_day_active, 0, sizeof(seven_day_active));
		server_sh = 0;

		guild_boss_baodi_reward_flag = 0;
		guild_boss_today_highest_damage = 0;

		battle_value = 0;
		battle_value_change_last_tamp = 0;
		rich_value = 0;
		rich_value_change_last_tamp = 0;
		kinder_value = 0;
		kinder_value_change_last_tamp = 0;

		is_get_guild_attr = 0;

		join_honor_battle_flag = 0;
		reserve_ch11 = 0;
		reserve_sh11 = 0;

		role_guild_task_finish_times = 0;		//完成次数
		role_guild_task_is_mail_notice = 0;		//邮件通知
		role_guild_task_is_solicit = 0;		//是否征集
		role_guild_task_solicit_task_id = 0;		//征集任务id
		role_guild_task_solicit_num = 0;		//征集进度
		role_guild_task_solicit_fetch_num = 0;	//征集进度是否领取
	}

	long long guild_boss_total_damage; //今天对家族试炼boss 总伤害
	int guild_boss_join_flag;			// 家族试炼 参与挑战哪个boss
	int guild_task_contribute;			// 家族任务贡献度
	unsigned int last_change_task_contribute_time; // 上次改变家族任务贡献度的时间（用于排序）
	int guild_boss_fetch_chapter_reward_times;
	char is_fetch_boss_chapter_baodi_reward_OBSOLETE;//玩家今天是否已领取保底奖励	0:未领取 1:已领取
	char is_fetch_boss_chapter_damage_rank_reward;	//玩家今天是否已领取了试炼伤害排名奖励 0:未领取 1:已领取
	unsigned char tan_he_join_flag;
	char is_qifu_get;								// 是否领取了祈福属性
	int week_active_val;							// 玩家周活跃度 
	unsigned int last_change_week_active_val_times;	// 上一次变更玩家周活跃度的时间
	int guild_day_active_reawrd_flag;				// 帮派日活跃度奖励领取标记 [备注:不一定可信,以role.CommonDataParam下面记录的为主,这里主要是用于补发奖励]

	short seven_day_active[GUILD_SEVEN_DAY_ACTIVE];	// 7日活跃用 , 这里写死了7天
	short server_sh;								// 对齐用

	int guild_boss_baodi_reward_flag;				// 保底奖励标记
	long long guild_boss_today_highest_damage;		// 今天对家族试炼boss 最高伤害

	int battle_value;								// 战绩值
	unsigned int battle_value_change_last_tamp;		// 上一次战绩变更

	int rich_value;									// 富豪值
	unsigned int rich_value_change_last_tamp;		// 上一次富豪值变更

	int kinder_value;								// 善人值
	unsigned int kinder_value_change_last_tamp;		// 上一次善人值变更

	int is_get_guild_attr;
	
	char join_honor_battle_flag;		// 是否参与了家族荣誉战
	char reserve_ch11;
	short reserve_sh11;

	short role_guild_task_finish_times;		//完成次数
	char role_guild_task_is_mail_notice;		//邮件通知
	char role_guild_task_is_solicit;		//是否征集
	int role_guild_task_solicit_task_id;		//征集任务id
	short role_guild_task_solicit_num;		//征集进度
	short role_guild_task_solicit_fetch_num;	//征集进度是否领取
};
UNSTD_STATIC_CHECK(0 == (sizeof(GuildMemberData::seven_day_active) + sizeof(GuildMemberData::server_sh)) % 4);

typedef char GuildMemberDataHex[sizeof(GuildMemberData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(GuildMemberDataHex) < 4096);


class GuildMemberParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct Member
	{
		Member()
		{
			this->Reset();
		}
		void Reset()
		{
			change_state = structcommon::CS_NONE;
			index = -1;
			uid = 0;
			memset(name, 0, sizeof(name));
			level = 0;
			m_profession = 0;
			appearance.Reset();
			post = 0;
			gongxian = 0;
			total_gongxian = 0;
			join_time = 0;
			last_login_time = 0;
			member_data.Reset();
		}

		int change_state;
		int index;

		int uid;
		GameName name;
		int level;
		int m_profession;
		Appearance appearance;

		int post;

		int gongxian;
		int total_gongxian;

		long long join_time;
		long long last_login_time;
	
		GuildMemberData member_data;
	};

	int count;
	Member member_list[GUILD_MAX_MEMBER_COUNT];
};
/* ================================================================================================= */

/* =========================================== 家族周任务 =========================================== */
static const int GUILD_WEEK_TASK_RANK_LIST_NUM_MAX = 30;		// 家族任务 - 排行榜长度

enum GWTTaskCondition
{
	GWT_COND_KILL_RACE_MONSTER = 1,		// 击败种族怪物	| 种族类型 | 需求数量
	GWT_COND_ITEM_TYPE_GATHERS = 2,		// 采集类型道具	| 采集类型 | 需求次数
	GWT_COND_KILL_WILD_BOSSNUM = 3,		// 击败野外BOSS	|    /     | 需求数量
	GWT_COND_PLAY_COPY_CRYSTAL = 4,		// 通关水晶副本	|    /     | 需求次数
	GWT_COND_KILL_MONSTER = 5,			// 击杀小怪     |    /     | 需求数量
	GWT_COND_SEAL_PET = 6,				// 封印			|    /     | 需求数量

	GWT_COND_TYPE_MAX
};

struct GuildWeekTaskParam
{
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	GuildWeekTaskParam() { this->Reset(); }

	void Reset()
	{
		next_refresh_task_timestamp = 0;
		next_awarding_timestamp = 0;
		cur_finish_times = 0;
		cur_gwt_task_id = 0;
		cur_gwt_task_prog = 0;
	}

	long long next_refresh_task_timestamp;		// 下一次刷新任务时间
	long long next_awarding_timestamp;			// 下一次发放奖品时间

	int cur_finish_times;		// 本周内当前完成任务的次数
	int cur_gwt_task_id;		// 当前家族周任务的任务ID
	int cur_gwt_task_prog;		// 当前家族周任务的任务进度
};
typedef char GuildWeekTaskParamHex[sizeof(GuildWeekTaskParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(GuildWeekTaskParamHex) < 64);
/* ================================================================================================= */

class AllGuildParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct GuildParam
	{
		GuildBaseParam base_param;
		GuildMemberParam member_param;
		GuildWeekTaskParam gwt_param;
		GuildBossParam guild_boss_param;
	};

	int count;
	GuildParam guild_list[MAX_ONCE_GUILD_COUNT];
};

enum PET_TRAINING_TYPE
{
	PET_TRAINING_TYPE_MIN = 0,

	PET_FRIENDS,				// 增加宠物训练个数
	TRAINING_EXPERIENCE,		// 增加训练加成
	TRAINING_ENDURANCE,			// 增加训练最大时间

	PET_TRAINING_TYPE_MAX,
};
static const int GUILD_PET_TRAINING_PET_MAX_COUNT = 3;
struct GuildTrainingPetParam
{
	GuildTrainingPetParam() { this->Reset(); }
	void Reset()
	{
		pet_index = -1;
		begin_time = 0;
	}

	int pet_index;
	unsigned int begin_time;
};

static const int GUILD_SHOP_COUNT_MAX = 100;
static const int GUILD_GUILD_DONATE_COUNT_MAX = 20;
class GuildCommonParam
{
public:
	GuildCommonParam() { this->Reset(); }
	void Reset()
	{
		for (int i = 0; i < PET_TRAINING_TYPE_MAX; ++i)
		{
			guild_training_level[i] = 1;
		}
		for (int i = 0; i < GUILD_PET_TRAINING_PET_MAX_COUNT; ++i)
		{
			guild_training_pet_list[i].Reset();
		}
		guild_explore_flag = 0;
		guild_explore_seq = 0;
		guild_explore_open_time = 0;

		memset(shop_buy_list, 0, sizeof(shop_buy_list));
		memset(build_donate_count, 0, sizeof(build_donate_count));
		position_mark.Reset();
		guild_boss_play_times = 0;
		guild_boss_participate_reward_fetch_flag = 0;

		today_guild_qifu_times = 0;
		reserve_int = 0;
	}

	//  宠物训练等级
	int guild_training_level[PET_TRAINING_TYPE_MAX];
	UNSTD_STATIC_CHECK(4 == PET_TRAINING_TYPE_MAX);
	// 训练中的宠物
	GuildTrainingPetParam guild_training_pet_list[GUILD_PET_TRAINING_PET_MAX_COUNT];
	UNSTD_STATIC_CHECK(3 == GUILD_PET_TRAINING_PET_MAX_COUNT);

	short guild_explore_flag;				// 今日探险标记 6点重置
	short guild_explore_seq;				// 选择的探险seq
	unsigned int guild_explore_open_time;	// 开始的时间

	unsigned short shop_buy_list[GUILD_SHOP_COUNT_MAX];
	unsigned short build_donate_count[GUILD_GUILD_DONATE_COUNT_MAX];

	BitMap<32> position_mark;				// 担任过xx职位
	int guild_boss_play_times;				// 家族试炼今天的玩次数
	int guild_boss_participate_reward_fetch_flag; //家族试炼Boss参与奖励获取标记（每日刷新）

	int today_guild_qifu_times;
	int reserve_int;
};
typedef char GuildCommonParamHex[sizeof(GuildCommonParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(GuildCommonParamHex) < 2048);

struct GuildBossRankItem		//家族试炼排行榜项  不保存
{			
	GuildBossRankItem()
	{
		role_id = 0;
		guild_boss_total_damage = 0;
	}

	bool operator<(const GuildBossRankItem & other) const
	{
		if (guild_boss_total_damage > other.guild_boss_total_damage)		
			return true;
		if (guild_boss_total_damage < other.guild_boss_total_damage)		
			return false;
		
		if (role_id < other.role_id)
			return true;
		if (role_id > other.role_id)
			return false;

		return false;
	}

	int role_id;
	long long guild_boss_total_damage;
};

struct GuildBossChapterRank			//各个家族之间的家族试炼进度排行 - 不存库
{
	GuildBossChapterRank() { this->Reset(); }

	void Reset()
	{
		guild_id = 0;
		boss_index = 0;
		boss_num = 0;
		kill_timestamp = 0;
		create_guild_timestamp = 0;
		memset(guild_name, 0, sizeof(guild_name));
		memset(guild_banner, 0, sizeof(guild_banner));
	}

	bool operator<(const GuildBossChapterRank & other) const
	{
		if (boss_num > other.boss_num)
			return true;
		if (boss_num < other.boss_num)
			return false;

		if(boss_index > other.boss_index)
			return true;
		if(boss_index < other.boss_index)
			return false;

		if(kill_timestamp < other.kill_timestamp)
			return true;
		if(kill_timestamp > other.kill_timestamp)
			return false;

		if(create_guild_timestamp < other.create_guild_timestamp)
			return true;
		if(create_guild_timestamp > other.create_guild_timestamp)
			return false;
		
		return false;
	}

	int guild_id;						//家族ID
	int chapter_id;						//章节ID
	short boss_index;					//打到哪个boss
	short boss_num;						//击杀的BOSS数量
	unsigned int kill_timestamp;		//击杀BOSS时间
	unsigned int create_guild_timestamp;//家族创建时间
	GuildName guild_name;				//家族名字
	GuildBanner guild_banner;			//族徽
};

struct SynGuildBossChapterRankInfo		//用于协议传输
{
	SynGuildBossChapterRankInfo() { this->Reset(); }
	SynGuildBossChapterRankInfo(const GuildBossChapterRank & info)
	{
		boss_index = info.boss_index;
		kill_num = info.boss_num;
		F_STRNCPY(guild_name, info.guild_name, sizeof(guild_name));
		F_STRNCPY(guild_banner, info.guild_banner, sizeof(guild_banner));
	}

	void Reset()
	{
		boss_index = 0;
		kill_num = 0;
		memset(guild_name, 0, sizeof(guild_name));
		memset(guild_banner, 0, sizeof(guild_banner));
	}

	short boss_index;			//boss星级
	short kill_num;				//该章节击杀的BOSS数量
	GuildName guild_name;		//家族名字
	GuildBanner guild_banner;	//族徽
};

#pragma pack(pop)

#endif  // __GUILD_DEF_HPP__

