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

enum COMMON_SAVE_DATA_TYPE		//��⣬�����Ҽ�
{
	COMMON_SAVE_DATA_TYPE_INVALID = 0,
	COMMON_SAVE_DATA_TYPE_BRAVE_GROUND_TEAM_RECORD = 1,		//��1,���ܺϣ��´��ع��ŶӲ�����¼ param1:pass_id  �� ���а���  
	COMMON_SAVE_DATA_TYPE_BRAVE_GROUND_ROLE = 2,				//��2,���Ժϣ��´��ع���ɫ���� param1:role_id			
	COMMON_SAVE_DATA_TYPE_HUAN_JIE_TIAN_TI_RECORD = 3,			//��3,���Ժϣ��ý�ս�� ��ɫ���߲�������ս������ûͬ������ɫ���ϵ�����   param1:role_id , param2: record_id 
	COMMON_SAVE_DATA_TYPE_PLATFORM_BATTLE_RECORD = 4,			//��4,���Ժϣ���̨սս��   param1:role_id	
	COMMON_SAVE_DATA_TYPE_BRAVE_GROUND_SAVE_DATA = 5,			//��5,���ܺϣ��´��ع� ��¼�ϸ�����ǰ3���ˣ��������ƺš�������¼ param1:0  
	COMMON_SAVE_DATA_TYPE_HUAN_JIE_SAVE_DATA = 6,					//��6,���ܺϣ��ý�ս�� ��¼ÿ������������ǰ���ˣ��������ƺ�,��Ϸ������¿���ġ�������¼ param1:0  
	COMMON_SAVE_DATA_TYPE_GUILD_FIGHT_DATA = 7,						//��7,�ܺϣ�������Ҷ�����,ÿ������һ�����Ļ��֣�ǰ16���������μӼ�������ս  param1:guild_id
	COMMON_SAVE_DATA_TYPE_GUILD_HONOR_DATA = 8,					//��8,���ܺϣ���������ս���� param1:0 ���� 					
	COMMON_SAVE_DATA_TYPE_GUILD_HONOR_RECORD_OLD_DATA = 9,			//��9,���ܺϣ��ɵļ�������ս������¼���ݣ� param1:0 ���� ����û���ˣ��ĳ���14�ģ�
	COMMON_SAVE_DATA_TYPE_PLATFORM_BATTLE_ACHIEVEMENT_DATA = 10,	//��10,�ܺϣ���̨ս��ҳɾ����� param1:role_id	
	COMMON_SAVE_DATA_TYPE_PLATFORM_BATTLE_OTHER_DATA = 11,			//��11,���ܺϣ���̨ս����	param1:0 ���� 
	COMMON_SAVE_DATA_TYPE_GIVE_GIFT = 12,									//��12,���Ժͣ���������
	COMMON_SAVE_DATA_TYPE_RA_DELAY_SET_TIME_DATA = 13,			//!< (13,���ܺ�) �ӳ���������ʱ������ param1:�id  param2:��ʼʱ���
	COMMON_SAVE_DATA_TYPE_GUILD_HONOR_RECORD_DATA = 14,			//��14,���ܺϣ���������ս������¼���� param1:0 ���� 
	COMMON_SAVE_DATA_TYPE_JIE_YI_ZU = 15,						//��15,���Ժϣ������� param1+ param2==������id
	COMMON_SAVE_DATA_TYPE_JIE_YI_ROLE_XUAN_YAN = 16,						//��16,���Ժϣ���������
	COMMON_SAVE_DATA_TYPE_JIE_YI_ZU_XUAN_YAN = 17,						//��17,���Ժϣ�����������
	COMMON_SAVE_DATA_TYPE_COW_REPORT_GOOD_NEWS_DATA = 18,		// (18, ���ܺ�)  ��ţ��ϲ���������� , param1 :0 ����
	COMMON_SAVE_DATA_TYPE_TIGER_BRINGS_BLESSING_DATA = 19,		// (19, ���ܺ�)  �����ɸ����������� , param1 :0 ����
	COMMON_SAVE_DATA_TYPE_QING_YUAN_DATA = 20,					// (20, ���Ժ�)  ��Ե���� param1+ param2==��Եid
	COMMON_SAVE_DATA_TYPE_HONGMENG_TIANJI = 21,					// (21, ���ܺ�)	������� ȫ����ͨ��¼

	COMMON_SAVE_DATA_TYPE_MAX,
};

unsigned int GetCommonSaveDataLengthByType(int save_type);

//---------------------�����Ǵ��ṹ��ע�����--------------------------------------
#pragma pack(push, 4)
//--------------------------�´��ع�-------------------
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

//--------------------------�ý�-------------------

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

const static int HUAN_JIE_RANK_TITLE_MAX_NUM = 50;  //�ý�ս����������ƺŵ��������
struct CommonSaveHuanJieData
{
	void Reset();

	int role_id[HUAN_JIE_RANK_TITLE_MAX_NUM];			//��¼ÿ��������ǰ����ˣ��������ƺ�
};

//--------------------------��̨ս-------------------

const static int PLATFORM_BATTLE_RECORD_ITEM_MAX_NUM = 50;
const static int PLATFORM_BATTLE_MAX_RANK_REWARD = 2;				//��������̨���ý�������̨����
struct CommonSavePlatformBattleRecordRoleInfo
{
	void Reset();

	int role_id;
	int role_level;
	GameName role_name;
	short avatar_type;
	short prof;
	short headshot_id;
	short reserve_sh;			//�����������ǳ�ʼ����
};

struct CommonSavePlatformBattleRoleRecord
{
	void Reset();

	CommonSavePlatformBattleRecordRoleInfo role_info_1;
	CommonSavePlatformBattleRecordRoleInfo role_info_2;

	unsigned int time;
	short is_win;
	short platform_num;
	short curr_rank;		//��ǰ����
	short last_rank;		//�ϴ�����	
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
	unsigned char is_first_times;		//��û�������̨����0��û���
	unsigned char lose_record_flag;		//ÿ����̨�������ͣ���Ҫ�����ʾ
	unsigned short record_index;
	CommonSavePlatformBattleRoleRecord records[PLATFORM_BATTLE_RECORD_ITEM_MAX_NUM];
	CommonSavePlatformBattleLastRankItem last_rank_info[PLATFORM_BATTLE_MAX_RANK_REWARD];    //�ϴν���ʱ������Ϣ
};

//��̨ս�ɾ�
enum PLATFORM_BATTLE_ACHIEVEMENT
{
	PLATFORM_BATTLE_ACHIEVEMENT_ANY_TOP_X = 0,				//�κ�һ����̨,����ǰn��
	PLATFORM_BATTLE_ACHIEVEMENT_BA_ZHU_TOP_1_NUM = 1,		// �ڰ�����̨�Ƕ�n����̨
	PLATFORM_BATTLE_ACHIEVEMENT_QING_LONG_TOP_1_NUM = 2,	// ��������̨�Ƕ�n����̨
	PLATFORM_BATTLE_ACHIEVEMENT_BAI_HU_TOP_1_NUM = 3,		// �ڰ׻���̨�Ƕ�n����̨
	PLATFORM_BATTLE_ACHIEVEMENT_ZHU_QUE_TOP_1_NUM = 4,		// ����ȸ��̨�Ƕ�n����̨
	PLATFORM_BATTLE_ACHIEVEMENT_XUAN_WU_TOP_1_NUM = 5,	// ��������̨�Ƕ�n����̨
	PLATFORM_BATTLE_ACHIEVEMENT_ANY_ACK_TIMES = 6,		//��������̨��ս��ʤ5��
	PLATFORM_BATTLE_ACHIEVEMENT_ANY_DEF_TIMS = 7,			//��������̨���޻�ʤ5��
	PLATFORM_BATTLE_ACHIEVEMENT_TOP_1_DAY = 8,		//��������̨�����Ƕ�����N��
	PLATFORM_BATTLE_ACHIEVEMENT_PARTNER_NUM = 9,		//������̨�����������N��
	PLATFORM_BATTLE_ACHIEVEMENT_PET_NUM = 10,			//������̨������������n��
	PLATFORM_BATTLE_ACHIEVEMENT_CAPACITY_TIMES = 11,	//սʤ�ۺ����ָ����Լ������3��
	PLATFORM_BATTLE_ACHIEVEMENT_BA_ZHU_TOP_X = 12,		// �ڰ�����̨,����ǰn��
	PLATFORM_BATTLE_ACHIEVEMENT_QING_LONG_TOP_X = 13,	// ��������̨,����ǰn��
	PLATFORM_BATTLE_ACHIEVEMENT_BAI_HU_TOP_X = 14,		// �ڰ׻���̨,����ǰn��
	PLATFORM_BATTLE_ACHIEVEMENT_ZHU_QUE_TOP_X = 15,		// ����ȸ��̨,����ǰn��
	PLATFORM_BATTLE_ACHIEVEMENT_XUAN_WU_TOP_X = 16,	// ��������̨,����ǰn��
	PLATFORM_BATTLE_ACHIEVEMENT_MAX,
};

struct CommonSavePlatformBattleAchievementItem
{
	void Reset();


	unsigned short num;
	unsigned short fetch_flag;
};

bool IsPlatformBattleRankAchievementType(int type);			//�Ƿ����а����ͣ����ڵ�ǰ����� ҪС�ڵ��� ���õĲ���

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
	
	long long  achievement_next_reset_time;			//�ɾ�����ʱ�䣬ÿ������ȫ��ͳһ����
	int reset_version;
};

//--------------------------������Ҷ�-------------------
struct CommonSaveGuildFightRecord
{
	void Reset();

	unsigned int time;
	int score;
};

struct CommonSaveGuildFightData				//������Ҷ�����������������δ��Ҷ������ܺ͵ó�ǰ16������ս��������
{
	void Reset();

	int guild_id;
	int server_id;
	GuildName guild_name;
	GuildBanner banner;
	GameName patriarch_name;
	unsigned int join_guild_fight_day;
	CommonSaveGuildFightRecord records[7];			//һ��ÿ�������Ҷ�ս��
};
//--------------------------��������ս-------------------
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
	int guess_num[4];		//ÿ��֧�ֵ�����
	unsigned short wday_flag;			//��ʱû��
	unsigned short score_records[7];			//һ��ÿ�������Ҷ�ս��
};

struct CommonSaveGuildHonorRecordOldData
{
	void Reset();

	CommonSaveGuildHonorRecordOldDataItem record_item[16];
};

enum GUILD_HONOR_BATTLE_PROCESS
{
	GUILD_HONOR_BATTLE_PROCESS_WEEK_START = 0,			//ÿ�ܿ�ʼ
	GUILD_HONOR_BATTLE_PROCESS_GUILD_FIGHT,		//���ܿ�ʼ��һ�δ��Ҷ�
	GUILD_HONOR_BATTLE_PROCESS_READY,				//��������ʱ��
	GUILD_HONOR_BATTLE_PROCESS_FINISH,			//��������ʱ�䣬����ս����
	GUILD_HONOR_BATTLE_PROCESS_MAX,			
};


struct CommonSaveGuildHonorData				//������Ҷ�����������������δ��Ҷ������ܺ͵ó�ǰ16������ս��������
{
	void Reset();
	
	int guild_honor_process;
	unsigned int settlement_list_time;   //��������ʱ��
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
	int guess_num[GUILD_HONOR_BATTLE_MAX_ROUND];		//ÿ��֧�ֵ�����
	unsigned short wday_flag;			//��ʱû��
	unsigned short score_records[7];			//һ��ÿ�������Ҷ�ս��
}; 

struct CommonSaveGuildHonorRecordData				
{
	void Reset();

	CommonSaveGuildHonorRecordDataItem record_item[42];
};
//------------------------��������-------------------------------

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

//-----------------------------------------------------------------��������ʱ��--------------------------------------
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
	unsigned int delay_to_time; // ��ʱ����ʱ����ʱ��
	long long reserve_ll[2];
};

//-----------------------------------------------------------------������--------------------------------------
struct CommonSaveJieYiZuRoleInfoItem
{
	void Reset();

	int role_id;
	GameName role_name;
	short level;
	short avatar_type;
	short prof;
	unsigned char is_zu_zhang;
	unsigned char has_fisrt_member_name;		//�Ƿ��Ѿ��Ĺ���Ա���֣���һ�����
	int headshot_id;
	unsigned int  join_time;
	unsigned int leave_end_time;		//��ʱ����뿪�� �����뿪ʱ��, �ڵ�����۶��壬����ȷ�ϵ���������ҵ��ȷ�������24Сʱ�侲��
	unsigned last_login_time;
	JieYIZuMemberName member_name;	
	char is_vote;			//�Ƿ��Ѿ�ͶƱ��
	unsigned char tan_he_number_of_votes;	//���� ͶƱ��ȡƱ��
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

	unsigned int tan_he_vote_end_time;		//�����鳤ͶƱ����ʱ��
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





// ��ţ��ϲ
struct CommonSaveCowReportGoodNewsData
{
	void Reset();

	int curren_status;				// ��ǰ�Ļ״̬

	int buy_start_times;			// ������ʱ��
	int buy_end_times;				// �������ʱ��
	int show_start_times;			// չʾ�ڿ���ʱ��
	int	show_end_times;				// չʾ�ڽ���ʱ��

	CowReportGoodNewsRewardUserInfo reward_user_info[MAX_COW_REPORT_GOOD_NEWS_REWARD_USER_COUNT];	// �ý������Ϣ
};

// �����ɸ�
struct CommonSaveTigerBringsBlessingData
{
	void Reset();

	int curren_status;				// ��ǰ�Ļ״̬

	int buy_start_times;			// ������ʱ��
	int buy_end_times;				// �������ʱ��
	int show_start_times;			// չʾ�ڿ���ʱ��
	int	show_end_times;				// չʾ�ڽ���ʱ��

	TigerBringsBlessingRewardUserInfo reward_user_info[MAX_TIGER_BRINGS_BLESSING_REWARD_USER_COUNT];	// �ý������Ϣ
};

//----------------------------------------------------------------��Ե--------------------------------------
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

	int gift_type;			//��ͨ�¿�����
	unsigned int fetch_gift_flag;		// bit 0 �Ƿ���ȡ��ͨ������bit 1 �Ƿ���ȡ���콱��
	unsigned int gift_type_end_time;		//��ͨ�¿�����ʱ��
	unsigned int leave_end_time;		//������������ǿ�����ʱ��
	unsigned long long skill_no_effect_flag;		//��Ե���������Ƿ���Ч��־

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
	unsigned int leave_end_time;		//Э�����ʱ��
	int qing_yuan_duration_reward_already_fetch_day;
	CommonSaveQingYuanRoleInfoItem role_info[QING_YUAN_MAX_ROLE_NUM];
	CommonSaveQingYuanCommonData common_data;
};

struct CommonSaveWorldHongMengTianJie
{
	void Reset();

	HongMengTianJie::WorldFBData first_pass[HongMengTianJie::MAX_FB_INDEX_NUM];		// ȫ��������ͨ��¼
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
