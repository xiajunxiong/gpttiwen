#ifndef __GUILD_DEF_HPP__
#define __GUILD_DEF_HPP__

#include <memory>
#include <map>
#include <vector>

#include "servercommon/struct/structcommon.h"
#include "servercommon.h"
#include "bitmapfunction.hpp"
	
static const GuildID INVALID_GUILD_ID = 0;						// ��Ч�ľ���ID
static const int GUILD_MAX_MEMBER_COUNT = 140;
static const int GUILD_BUILD_COUNT = 4;
static const int CREATE_GUILD_LEVEL_LIMTI = 12;
static const int MAX_ONCE_GUILD_COUNT = 10;						// һ�β�������������
static const int GUILD_EVENTHANDLER_COUNT = 20;					// �¼�����
static const int GUILD_EVENTHANDLER_PARAM = 100000;				// �¼��������
static const int GUILD_SAVE_INTERVAL = 60;
static const int PLAYER_APPLY_COUNT_MAX = 5;
static const int GUILD_JOURNAL_ADD_NUM = 10;					//������־�д���װ�����κ���ħ֮��������Ϊ10
static const int MAX_GUILD_REWARD_GROUP_ITEM_NUM = 5;			
static const int MAX_GUILD_BOSS_CHAPTER_RANK_NUM = 10;			// ����������������ÿ���½�ӵ���������
static const int MAX_GUILD_CHAPTER_BOSS_NUM = 30;				// ���������½��������
static const int MAX_GUILD_GATHER_DREAM_NUM = 50;				// ���弯Ը�����������
static const int GUILD_SEVEN_DAY_ACTIVE = 7;					// �������ջ�Ծ
static const int GUILD_HALL_OF_FAME_REFRESH_TAMP = 5;			// ����������ˢ�¼��
static const int GUILD_CONSTRUCT_BUILDING = 6;					// ���彨���������+1[�������鷿���᷿����⣬����],�����ö�����±�1��ʼ,0�±�λ�ÿ�����������
UNSTD_STATIC_CHECK((GUILD_CONSTRUCT_BUILDING % 2) == 0);

//type * GUILD_EVENTHANDLER_PARAM + param
enum GUILD_EVENTHANDLER_TYPE
{
	GUILD_EVENTHANDLER_ADD_MEMBER = 1,						// xx�����˼���
	GUILD_EVENTHANDLER_DEC_MEMBER = 2,						// xx�뿪�˼���
	GUILD_EVENTHANDLER_APPOINTMENT = 3,						// xx������ΪGUILD_POST_TYPE
	GUILD_EVENTHANDLER_TYPE_XUNBAO = 4,						//Ѱ����ó���	param  - pet_id
	GUILD_EVENTHANDLER_TYPE_ADD_PET = 5,					//��ó���(��Ѱ��)  param - pet_id
	GUILD_EVENTHANDLER_TYPE_COURSE_OF_GROWTH = 6,			//�ɾʹﵽ��һ�׶ε�1��  param - cur_prog
	GUILD_EVENTHANDLER_TYPE_WORK_SHOP_COMPOUND = 7,			//��������   param - item_id
	GUILD_EVENTHANDLER_TYPE_WORK_SHOP_CRAFT_JEWELRY = 8,	//��������   param - jewelry_id
	GUILD_EVENTHANDLER_TYPE_MIYI = 9,						//ͨ������֮��  param - level
	GUILD_EVENTHANDLER_TYPE_COURAGE_CHALLENGE = 10,			//ͨ����ħ֮���ڼ���   10������ param - pass_layer
	GUILD_EVENTHANDLER_TYPE_GUILD_LEVEL_UP = 11,			//��������	param - guild_level

	GUILD_EVENTHANDLER_TYPE_MAX,
};

UNSTD_STATIC_CHECK(GUILD_EVENTHANDLER_TYPE_MAX == 12);

enum GUILD_POST_TYPE
{
	GUILD_POST_PATRIARCH = 0,								// �峤
	GUILD_POST_VICE_PATRIARCH,								// ���峤
	GUILD_POST_ELDER,										// ����
	GUILD_POST_CADRE,										// �Ǹ�
	GUILD_POST_ELITE,										// ��Ӣ
	GUILD_POST_MEMBER,										// ��Ա

	GUILD_POST_MAX,
};
UNSTD_STATIC_CHECK(GUILD_POST_MAX < 32);

//���弯Ը����
enum GUILD_GATHER_DREAM_ITEM_TYPE
{
	GUILD_GATHER_DREAM_ITEM_TYPE_PET = 0,			//����
	GUILD_GATHER_DREAM_ITEM_TYPE_PARTNER = 1,		//���

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
	long long			avatar_timestamp;								//!< ������ϴ�ͷ��
	int					level;											//!< �ȼ�
	int					top_level;										//!< �۷�ȼ�
	int					profession;										//!< ְҵ
	GameName			name;											//!< ����
	long long			timeout_timestamp;								//!< ��ʱʱ��
	short				avatar_type;									//!< ͷ������
	char				baodi_fetch_flag_OBSOLETE;						//!< ��ҽ����Ƿ�����ȡ���׽���	0:δ��ȡ 1:����ȡ
	char				rank_fetch_flag;								//!< ��ҽ����Ƿ�����ȡ�������˺��������� 0:δ��ȡ 1:����ȡ
	int					baodi_reward_flag;								//!< ���׽������
};

struct SynGuildGatherDreamGiftInfo
{
	SynGuildGatherDreamGiftInfo(): guild_id(0), gift_role_uid(0), get_role_uid(0), index(0), item_id(0), quality(0), gather_dream_type(0), gift_post(0),get_post(0)
	{}

	int guild_id;
	int gift_role_uid;
	int get_role_uid;
	short index;				//�����б��±�
	ItemID item_id;
	short quality;	
	short gather_dream_type;
	short gift_post;
	short get_post;
};


struct GuildGatherDreamInfo			//���弯Ը��Ϣ
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

	GameName from_name;				//�����������
	int role_id;					//�������ID
	ItemID item_id;					//��Ը������Ƭ����ID
	short headshot_id;				//�������ͷ��
	char gather_num;				//��ʱ��Ը����
	char fetch_num;					//�������ȡ����
	char need_max_num;				//������������
	char quality;					//Ʒ��
	short avatar_type;
	short gather_dream_type;		//��Ը����
	unsigned int pulish_timestamp;	//����ʱ��
};

struct GuildGatherDreamData			//���弯ԸЭ���·�����
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

	GameName from_name;				//�����������
	int role_id;					//������
	ItemID item_id;					//��Ը��Ƭ����ID
	short headshot_id;				//�������ͷ��
	char gather_num;				//��ʱ��Ը����
	char fetch_num;					//�������ȡ����
	char need_max_num;				//������������
	char help_num;					//��Ұ�������
	short avatar_type;
	short gather_dream_type;		//��Ը����	
	unsigned int pulish_timestamp;	//����ʱ��
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
	int record_index;		//ÿ����һ���¼�¼������
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

struct InfoTypeReserve					//����,�Ա��ִ�ʱ�������ݲ������
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
	ItemID			item_id;			//!< ��Ʒid
	short			column;				//!< ��Ʒ���ĸ��������� [0,9)
	short			index;				//!< ��Ʒ�ڱ����е����
	short			num;				//!< ��Ʒ����
	char			is_bind;			//!< �Ƿ��  1Ϊ�� 0Ϊ�ǰ�
	char			has_param;			//!< �Ƿ��о�ֵ���� ��Ϊ1����Ϊ0   \note ��ֵ������װ���������Լ�����ֵ��ÿ����Ʒ����Ҫ���ɲ�һ����
	short			param_length;		//!< ��ֵ���������ݳ��� ����has_paramΪ1ʱ�ɶ���
	UInt32		    invalid_time;		//!< ����ʱ��
	NetValueItemParamData param_data;	//!< ��ֵ���������ݣ�����param_length��ֵ����ȡ ����has_paramΪ1ʱ�ɶ���
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
	int eventhandler_type;				// ������¼� type*GUILD_EVENTHANDLER_PARAM + param
	unsigned int eventhandler_time;		// �¼�����ʱ��
	GameName eventhandler_name;
	InfoTypeReserve item_info;					//�����¼�������й�ʱ��������
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

struct GuildBossParam		//����ṹ�������棬ע��4�ֽڶ���
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

	int boss_index;				//!< ���½ڵ��ĸ�boss, ÿ����һ��boss��ֵ+1
	int chapter_id;					//!< ������½�(�½�ÿ����������ȼ�������
	int select_chapter_id_OBSOLETE;	//!< �᳤ѡ��ڶ�����ս�ĸ��½�
	int remain_hp_percent;			//!< �������·�Э��
	int max_chapter_OBSOLETE;		//!< ���ͨ���½�
	short monster_remain_percent_list[MONSTER_NUM];
	unsigned int kill_timestamp;	//!< �����ɱBOSS��ʱ���
	char reset_flag;
	char reserve_ch;				//������
	short week_kill_boss_num;		//���ܻ�ɱboss��
	unsigned int reserve_ui; //!< bossÿ�ܸ���ʱ���
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
	int week_total_guild_active_val;			// �����ܻ�Ծ��(���ɳ�Ա�����ڻ�õ��ܻ�Ծ��)ÿ������
	GuildName guild_name;
	GuildNotice guild_notice;

	int guild_level;
	long long guild_exp;

	int build_flag;
	int build_value[GUILD_BUILD_COUNT];
	UInt32 protect_timestamp;		// ���彵����������ʱ��
	int guild_honor_title_id;			//û����
	int origin_plat_type;
	int origin_server_id;
	int is_need_approval;

	GuildBanner guild_banner; //���

	char today_set_banner_flag;
	char reset_guild_eventhandler;				//����
	char reset_guild_eventhandler_move_flag;	//����
	char can_tan_he;
	int guild_fight_title_id;		//������Ҷ��ƺ�
	unsigned int tan_he_zu_zhang_start_time;
	unsigned short guild_fight_rank;		//ÿ�ܵ�һ�ι���ս����,����buff����Ҫ�õ�,û����
	unsigned short guild_fight_buff_rank;		//ÿ�ܵ�һ�ι���ս����,����buff����Ҫ�õ���û���ˣ��´ν���ĳ�������״̬�Ǳߵ�ֵ��
	int day_total_guild_active_val;				// �����ջ�Ծ��(���ɳ�Ա�����ڻ�õ��ܻ�Ծ��)ÿ������
	unsigned int last_guild_active_change_times;	// ��һ�ΰ��ɻ�Ծ�ȷ��������ʱ���

	int seven_day_active[GUILD_SEVEN_DAY_ACTIVE];			// 7�ջ�Ծ

	char reserve_ch[148];			//������
	char is_open_qifu;	//�Ƿ�����
	char is_send_will_overflow_mail;	//���շ��͹��ʽ𼴽�����ʼ�
	char is_send_overflow_mail;			//���շ��͹��ʽ����ʼ�
	char reserve_ch2;
	int qifu_value;		//������

	short build_level[GUILD_CONSTRUCT_BUILDING];		// [1-5],�±�0-��ǰ���ڽ���Ľ�����
	unsigned int reserve_uint6[6];
	unsigned int single_building_finish_timestamp;		// �����������ʱ���
	short quicken_building_count;						// �����ѿ������ٴ���
	short can_speed_up;									// �Ƿ�ɼ���
	GameName opener_name;								// ���ٿ����˵�����
	int opener_post;									// ������ְλ
	int building_count;									// ÿ���ѽ������

};
typedef char GuildBaseParamHex[sizeof(GuildBaseParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(GuildBaseParamHex) < 15360);

/* =========================================== �����Ա�� =========================================== */ 
struct GuildMemberData			//����ṹ�������棬ע��4�ֽڶ���
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

		role_guild_task_finish_times = 0;		//��ɴ���
		role_guild_task_is_mail_notice = 0;		//�ʼ�֪ͨ
		role_guild_task_is_solicit = 0;		//�Ƿ�����
		role_guild_task_solicit_task_id = 0;		//��������id
		role_guild_task_solicit_num = 0;		//��������
		role_guild_task_solicit_fetch_num = 0;	//���������Ƿ���ȡ
	}

	long long guild_boss_total_damage; //����Լ�������boss ���˺�
	int guild_boss_join_flag;			// �������� ������ս�ĸ�boss
	int guild_task_contribute;			// ���������׶�
	unsigned int last_change_task_contribute_time; // �ϴθı���������׶ȵ�ʱ�䣨��������
	int guild_boss_fetch_chapter_reward_times;
	char is_fetch_boss_chapter_baodi_reward_OBSOLETE;//��ҽ����Ƿ�����ȡ���׽���	0:δ��ȡ 1:����ȡ
	char is_fetch_boss_chapter_damage_rank_reward;	//��ҽ����Ƿ�����ȡ�������˺��������� 0:δ��ȡ 1:����ȡ
	unsigned char tan_he_join_flag;
	char is_qifu_get;								// �Ƿ���ȡ��������
	int week_active_val;							// ����ܻ�Ծ�� 
	unsigned int last_change_week_active_val_times;	// ��һ�α������ܻ�Ծ�ȵ�ʱ��
	int guild_day_active_reawrd_flag;				// �����ջ�Ծ�Ƚ�����ȡ��� [��ע:��һ������,��role.CommonDataParam�����¼��Ϊ��,������Ҫ�����ڲ�������]

	short seven_day_active[GUILD_SEVEN_DAY_ACTIVE];	// 7�ջ�Ծ�� , ����д����7��
	short server_sh;								// ������

	int guild_boss_baodi_reward_flag;				// ���׽������
	long long guild_boss_today_highest_damage;		// ����Լ�������boss ����˺�

	int battle_value;								// ս��ֵ
	unsigned int battle_value_change_last_tamp;		// ��һ��ս�����

	int rich_value;									// ����ֵ
	unsigned int rich_value_change_last_tamp;		// ��һ�θ���ֵ���

	int kinder_value;								// ����ֵ
	unsigned int kinder_value_change_last_tamp;		// ��һ������ֵ���

	int is_get_guild_attr;
	
	char join_honor_battle_flag;		// �Ƿ�����˼�������ս
	char reserve_ch11;
	short reserve_sh11;

	short role_guild_task_finish_times;		//��ɴ���
	char role_guild_task_is_mail_notice;		//�ʼ�֪ͨ
	char role_guild_task_is_solicit;		//�Ƿ�����
	int role_guild_task_solicit_task_id;		//��������id
	short role_guild_task_solicit_num;		//��������
	short role_guild_task_solicit_fetch_num;	//���������Ƿ���ȡ
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

/* =========================================== ���������� =========================================== */
static const int GUILD_WEEK_TASK_RANK_LIST_NUM_MAX = 30;		// �������� - ���а񳤶�

enum GWTTaskCondition
{
	GWT_COND_KILL_RACE_MONSTER = 1,		// �����������	| �������� | ��������
	GWT_COND_ITEM_TYPE_GATHERS = 2,		// �ɼ����͵���	| �ɼ����� | �������
	GWT_COND_KILL_WILD_BOSSNUM = 3,		// ����Ұ��BOSS	|    /     | ��������
	GWT_COND_PLAY_COPY_CRYSTAL = 4,		// ͨ��ˮ������	|    /     | �������
	GWT_COND_KILL_MONSTER = 5,			// ��ɱС��     |    /     | ��������
	GWT_COND_SEAL_PET = 6,				// ��ӡ			|    /     | ��������

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

	long long next_refresh_task_timestamp;		// ��һ��ˢ������ʱ��
	long long next_awarding_timestamp;			// ��һ�η��Ž�Ʒʱ��

	int cur_finish_times;		// �����ڵ�ǰ�������Ĵ���
	int cur_gwt_task_id;		// ��ǰ���������������ID
	int cur_gwt_task_prog;		// ��ǰ������������������
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

	PET_FRIENDS,				// ���ӳ���ѵ������
	TRAINING_EXPERIENCE,		// ����ѵ���ӳ�
	TRAINING_ENDURANCE,			// ����ѵ�����ʱ��

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

	//  ����ѵ���ȼ�
	int guild_training_level[PET_TRAINING_TYPE_MAX];
	UNSTD_STATIC_CHECK(4 == PET_TRAINING_TYPE_MAX);
	// ѵ���еĳ���
	GuildTrainingPetParam guild_training_pet_list[GUILD_PET_TRAINING_PET_MAX_COUNT];
	UNSTD_STATIC_CHECK(3 == GUILD_PET_TRAINING_PET_MAX_COUNT);

	short guild_explore_flag;				// ����̽�ձ�� 6������
	short guild_explore_seq;				// ѡ���̽��seq
	unsigned int guild_explore_open_time;	// ��ʼ��ʱ��

	unsigned short shop_buy_list[GUILD_SHOP_COUNT_MAX];
	unsigned short build_donate_count[GUILD_GUILD_DONATE_COUNT_MAX];

	BitMap<32> position_mark;				// ���ι�xxְλ
	int guild_boss_play_times;				// ������������������
	int guild_boss_participate_reward_fetch_flag; //��������Boss���뽱����ȡ��ǣ�ÿ��ˢ�£�

	int today_guild_qifu_times;
	int reserve_int;
};
typedef char GuildCommonParamHex[sizeof(GuildCommonParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(GuildCommonParamHex) < 2048);

struct GuildBossRankItem		//�����������а���  ������
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

struct GuildBossChapterRank			//��������֮��ļ��������������� - �����
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

	int guild_id;						//����ID
	int chapter_id;						//�½�ID
	short boss_index;					//���ĸ�boss
	short boss_num;						//��ɱ��BOSS����
	unsigned int kill_timestamp;		//��ɱBOSSʱ��
	unsigned int create_guild_timestamp;//���崴��ʱ��
	GuildName guild_name;				//��������
	GuildBanner guild_banner;			//���
};

struct SynGuildBossChapterRankInfo		//����Э�鴫��
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

	short boss_index;			//boss�Ǽ�
	short kill_num;				//���½ڻ�ɱ��BOSS����
	GuildName guild_name;		//��������
	GuildBanner guild_banner;	//���
};

#pragma pack(pop)

#endif  // __GUILD_DEF_HPP__

