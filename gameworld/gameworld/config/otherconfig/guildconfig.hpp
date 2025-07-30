#ifndef __GUILD_CONFIG_HPP__
#define __GUILD_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/serverdef.h"
#include "servercommon/guilddef.hpp"
#include "servercommon/partnerdef.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/petdef.h"
#include "servercommon/roleguildinborndef.hpp"
#include "servercommon/hongbaodef.hpp"
#include <map>
#include <vector>

const static int MAX_GATHER_DREAM_TYPE_NUM = PQT_MAX > MAX_PET_QUALITY_NUM ? PQT_MAX : MAX_PET_QUALITY_NUM;

struct GuildOtherCfg
{
	GuildOtherCfg() : create_cost(0), create_cost_type(0),level(0), exp_time(0), training_max_time(0), protect_time(0), max_coin(0LL), guild_boss_play_times(0),
	fund_return_rate(0), initial_funding(0), station_scene_id(0), change_badge_coin(0), pray_family_level(0), pray_give_maxnum(0), redbag_time_limit(0)
		,boss_round_limit(0), serve_build_capital(0), expend_coin(0), get_contribution(0), speed_up(0), speed_times(0), person_times(0), building_limit(0),
		post_title_id(0)
	{
		tan_he_zu_zhang_logout_time_length = 0;
		tan_he_zu_zhang_continued_time = 0;
	}

	int create_cost;
	int create_cost_type;
	int level;
	int exp_time;
	int training_max_time;
	int protect_time;
	long long max_coin;
	int guild_boss_play_times;
	int fund_return_rate;			// ���彵�������ʽ���� - �ٷֱ� %
	int initial_funding;			// �����ʼ�ʽ�
	int station_scene_id;			// ����פ�صĳ���ID
	int guild_rename_need_itemid;	// ���������Ҫ�ĵ���
	int change_badge_coin;			// �������Ҫ��ͭ������
	int pray_family_level;			// ��Ը�������ȼ�
	int pray_give_maxnum;			// ��Ըÿ�������������	
	unsigned int tan_he_zu_zhang_logout_time_length;		//�峤���߶���ܵ���
	unsigned int tan_he_zu_zhang_continued_time;				//�峤��������ʱ��

	unsigned int redbag_time_limit;					// �������ʱ��
	int boss_round_limit;			// ����boss���ƻغ���
	int bless_consume;
	int bless_all_add;
	int rank_amount;
	int bless_open_level;
	int bless_open_fund;
	int serve_build_capital;		//����ά����
	int expend_coin;				//��������ͭ��
	int get_contribution;			//���ٻ�ù���
	int speed_up;					//����ʱ��
	int speed_times;				//���ٿ�������
	int person_times;				//���˼��ٴ���
	int building_limit;				//ÿ�ս��������������
	std::pair<int, int> first_charge_create_cost;	//p1:�׳�������廨�ѻ������� p2:�׳��������廨�ѻ�������
	int post_title_id;				//�����Ա�ƺ�
};

struct GuildRoomCfg
{
	GuildRoomCfg() : level(0), exp(0), max_member(0), cost(0), hearsay(0), skill_points(0), life_skill_level_limit(0)
	{}

	int level;
	int exp;						// ������������ʽ�
	int max_member;
	int cost;						// ����ÿ��ά��
	int hearsay;
	int skill_points;				// �����������ܵ���
	int life_skill_level_limit;		// ����ܵȼ�����
};

enum PRIVILEGE_TYPE
{
	APPROVAL,					//����	
	APPOINT,					//����	
	KICK,						//����	
	NOTICE,						//�޸Ĺ���
	IMPEACH,					//�����峤
	GUILD_TASK_MAIL_NOTICE,				//�ʼ�֪ͨ����������
	PRIVILEGE_TYPE_MAX,
};

struct GuildMemberTypeCfg
{
	GuildMemberTypeCfg()
	{
		member_type = 0;
		max_num = 0;
		memset(privilege, 0, sizeof(privilege));
		modify_target = 0;
		level_up = 0;
		build_limit = 0;
	}

	int member_type;
	int max_num;
	int privilege[PRIVILEGE_TYPE_MAX];
	int modify_target;					// ����Ŀ��
	int level_up;						// ��������Ȩ��
	int build_limit;					// ��������/���Ȩ��
};

struct GuildPetTrainingCfg
{
	GuildPetTrainingCfg()
	{
		seq = 0;
		type = 0;
		level = 0;
		guild_level = 0;
		gongxian_cost = 0;
		coin_type = 0;
		coin_cost = 0;
		add_param = 0;
	}

	int seq;
	int type;
	int level;
	int guild_level;
	int gongxian_cost;
	int coin_type;
	int coin_cost;
	int add_param;
};

enum BUY_LIMIT_TYPE
{
	BUY_LIMIT_TYPE_UNLIMITED = -1,
	BUY_LIMIT_TYPE_DAY = 0,
	BUY_LIMIT_TYPE_WEEK = 1,

	BUY_LIMIT_TYPE_MAX
};

enum BUY_COST_TYPE
{
	BUY_COST_TYPE_GOLD = 0,
	BUY_COST_TYPE_IMMORTAL_COIN,
	BUY_COST_TYPE_BIND_COIN,
};

struct GuildShopCfg
{
	GuildShopCfg()
	{
		seq = 0;
		level = 0;
		item_id = 0;
		present_cost = 0;
		gongxian_cost = 0;
		buy_type = 0;
		buy_num = 0;
		cost_type = 0;
	}
	int seq;
	int level;
	int item_id;
	int present_cost;
	int gongxian_cost;
	int buy_type;
	int buy_num;
	int cost_type;
};

struct GuildBossCfg
{
	GuildBossCfg() : world_level(0)
	{

	}

	struct GuildBossCfgItem
	{
		GuildBossCfgItem() : boss_index(0), boss_group_id(0), join_coin(0), join_gongxian(0), family_coin(0), immortal(0), item_count(0)
		{}

		int boss_index;
		int boss_group_id;
		int join_coin;
		int join_gongxian;
		int family_coin;
		int immortal;		//������ 

		int item_count;
		ItemConfigData kill_reward_list[MAX_GUILD_REWARD_GROUP_ITEM_NUM];
	};

	int world_level;
	GuildBossCfgItem guild_boss_cfg;
};

struct GuildBossChapterRewardCfg
{
	struct ItemCfg
	{
		ItemCfg()
		{
			rate = 0;
			coin = 0;
			gongxian = 0;
			guild_coin_per_join = 0;
			item_count = 0;
		}

		int rate;
		int coin;
		int gongxian;
		int guild_coin_per_join;
		int item_count;
		ItemConfigData reward_list[MAX_GUILD_REWARD_GROUP_ITEM_NUM];
	};

	GuildBossChapterRewardCfg()
	{
		rate_count = 0;
	}

	int rate_count;
	std::vector<ItemCfg> group_reward;
};

struct GuildJournalCfg
{
	GuildJournalCfg():type(0), condition(0)
	{}

	int type;
	int condition;
};

struct GuildBaoDiRewardCfg			//���׽���
{
	GuildBaoDiRewardCfg() : exp(0), bind_coin(0), reward_group_id(0)
	{}

	int exp;
	int bind_coin;
	int reward_group_id;		//������ID
};

struct GuildRankRewardCfg
{
	GuildRankRewardCfg() : min_rank(0), max_rank(0), exp(0), bind_coin(0), reward_group_id(0), immortal(0)
	{}

	int min_rank;
	int max_rank;
	int exp;
	int bind_coin;
	int reward_group_id;
	int immortal;			//������
};

struct GuildGatherDreamCfg
{
	GuildGatherDreamCfg() : is_can_pray(0), pray_patch_num(0), coin(0), gongxian(0), guild_exp(0)
	{}

	int is_can_pray;			//�ܷ���Ը
	int pray_patch_num;			//���μ�Ը��Ƭ����
	int coin;					
	int gongxian;				
	int guild_exp;				//�����ʽ�
	ItemConfigData reward_item;
};

struct GuildActiveGiftCfg												// ��Ծ�����
{
	struct GiftCfg
	{
		GiftCfg():active_limit(0), exp(0), count(0){}

		int active_limit;													// ��ԾҪ��
		int exp;															// ���齱��
		int count;															// �������ñ�
		ItemConfigData reward_list[MAX_GUILD_REWARD_GROUP_ITEM_NUM];		// ��������
	};

	GuildActiveGiftCfg() { this->Reset(); }

	void Reset()
	{
		level_min = 0;
		level_max = 0;
		gift_cfg_vec.clear();
	}

	int level_min;
	int level_max;

	std::vector<GiftCfg> gift_cfg_vec;
};

struct GuildActiveRankRewardCfg											// �����Ծ����������
{
	GuildActiveRankRewardCfg() :rank_min(0), rank_max(0) , gongxian(0) , coin(0), count(0) {}

	int rank_min;														// ��������
	int rank_max;														// ��������

	int gongxian;														// ����
	int coin;															// ͭ��
	int count;															// �������ñ�
	ItemConfigData reward_list[MAX_GUILD_REWARD_GROUP_ITEM_NUM];		// ��������
};

struct GuildActivePersonRankRewardCfg									// ������˻�Ծ����������
{
	GuildActivePersonRankRewardCfg() :rank_min(0), rank_max(0), title(0), gongxian(0), coin(0), count(0){}

	int rank_min;														// ��������
	int rank_max;														// ��������

	int title;															// �ƺ�
	int gongxian;														// ����
	int coin;															// ͭ��
	int count;															// �������ñ�
	ItemConfigData reward_list[MAX_GUILD_REWARD_GROUP_ITEM_NUM];		// ��������
};

struct GuildInbornLevelCfg
{
	GuildInbornLevelCfg() : inborn_skill_id(0), prepose_seq(0), level_limit(0), guild_level_limit(0), reset_consume(0), skill_grade(0), gain_target(0)
	{}

	int inborn_skill_id;
	int prepose_seq;
	int level_limit;
	int guild_level_limit;
	int reset_consume;
	int skill_grade;
	int gain_target;
	std::vector<ItemConfigData> consume_list;	
};

struct GuildInbornCfg
{
	GuildInbornCfg() : max_level(0)
	{}

	int max_level;
	GuildInbornLevelCfg level_list[NAX_GUILD_INBORN_SKILL_LEVEL_NUM];
};

struct GuildHongBaoCfg
{
	struct HongBaoCfg
	{
		HongBaoCfg() :lingyu_sum(0), redbag_num(0), lingyu_min(0), lingyu_max(0), lingyu_change(0), is_take_effect(false), day_times_limit(-1), need_chong_zhi(0), redbag_heroism(0) {}

		int lingyu_sum;					// ��������
		int redbag_num;					// �������
		int lingyu_min;					// ������Сֵ
		int lingyu_max;					// �������ֵ
		int lingyu_change;				// ����仯ֵ

		bool is_take_effect;			// �Ƿ���Ч [Ϊ�˼��ݾ����� , ���ֶα�ʾ�����ͺ���Ƿ񻹻��ٻ��] 
		int day_times_limit;			// ÿ�տɻ�ô������� , -1��ʾ��������

		int need_chong_zhi;				// �����ֵ���[��ֵ���Ϊ���ʳ�ֵ , �۳���Ϊ�ۼƳ�ֵ]
		int redbag_heroism;				// �����Ӧ�������ֵ
	};

	HongBaoCfg hong_bao_cfg[HONGBAO_GUILD_TYPE_MAX];
};

struct HallOfFameRank
{
	HallOfFameRank() :type(0), rank_min(0), rank_max(0), title(0), gongxian(0), coin(0) {}
	int type;
	int rank_min;
	int rank_max;
	int title;
	int gongxian;
	int coin;
};

struct BlessTimesList
{
	BlessTimesList() :bless_list(0), gongxian_reward(0), cost_type(0), bless_consume(0), bless_add_once(0) {}
	int bless_list;
	int gongxian_reward;
	int cost_type;
	int bless_consume;
	int bless_add_once;
};

struct BlessAttrInfo
{
	BlessAttrInfo() :attr_type(0), attr_value(0) {}
	int attr_type;
	int attr_value;
};

struct BlessTimesAttr
{
	const static int MAX_ATTR_LIST_NUM = 6;
	BlessTimesAttr() :world_level(0){}
	int world_level;
	BlessAttrInfo attr_list[MAX_ATTR_LIST_NUM];
};

struct WarthAddInfo
{
	WarthAddInfo() :once_warth_add(0), captain_warth_add(0) {}
	int once_warth_add;
	int captain_warth_add;
};

const static int MAX_BLESS_TIMES = 5;//������

enum GUILD_BUILD_TYPE
{
	GUILD_BUILD_TYPE_HALL = 1,			// ����
	GUILD_BUILD_TYPE_STUDY = 2,			// �鷿
	GUILD_BUILD_TYPE_GUEST_ROOM = 3,	// �᷿
	GUILD_BUILD_TYPE_VAULT = 4,			// ���
	GUILD_BUILD_TYPE_STORE = 5,			// ����

	GUILD_BUILD_TYPE_MAX,
};

struct GuildBaseConstructCfg
{
	GuildBaseConstructCfg():up_expend_capital(0),up_expend_time(0),paramet_1(0)
	{}

	int up_expend_capital;	// �������������ʽ�
	int up_expend_time;		// ����������Ҫ��ʱ��
	int paramet_1;			// ����Ч��
};

struct GuildConstructCfg
{
	GuildConstructCfg() :build_type(0)
	{}

	int build_type;
	std::map<int, GuildBaseConstructCfg> base_list;		//key-�����ȼ�
};


class GuildBossKey
{
public:
	bool operator < (const GuildBossKey& other) const
	{
		if (chapter_id != other.chapter_id)
		{
			return chapter_id < other.chapter_id;
		}
		else
		{
			return boss_index < other.boss_index;
		}
	}

	int chapter_id;
	int boss_index;
};

class GuildConfig : public ILogicConfig
{
public:
	GuildConfig();
	virtual ~GuildConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const GuildOtherCfg* GetOtherCfg() { return &m_other_cfg; }
	const GuildOtherCfg& GetOtherCfgRef() { return m_other_cfg; }
	const GuildRoomCfg* GetRoomCfg(int level);
	const GuildRoomCfg* GetRoomCfgByLevel(int level);
	const GuildMemberTypeCfg* GetMemberTypeCfg(int type);
	const GuildPetTrainingCfg* GetPetTrainingCfg(int type, int level);
	const GuildJournalCfg* GetJournalCfg(int type);
	const GuildBaseConstructCfg* GetConstructCfg(int build_type, int build_level);

	
	int GetMaxBuildLevel(bool is_hall = false)const { return is_hall ? m_hall_max_level : m_other_build_max_level; }

	//------------------��������
	const GuildBossCfg::GuildBossCfgItem * GetBossCfg(int chapter_id);
	const GuildBossCfg* GetBossChapterCfg(int chapter_id);
	int GetBossChapterLimitWorldLevel(int chapter_id);
	int GetBossChapterByWorldLevel(int world_level);
	bool IsVaildGuildBossChapterId(int chapter_id);
	const GuildBossChapterRewardCfg::ItemCfg * GetGuildBossChapterRankReward(int reward_group_id);
	const GuildRankRewardCfg * GetRankRewardCfg(int chapter_id, int rank);
	//---------------------------���弯Ը
	const GuildGatherDreamCfg * GetGatherDreamCfg(int quality, int type);

	int GetPetTrainingExp(int level);
	int GetPetTrainingGongXian(int level);

	void ResetBuyCount(int type, unsigned short shop_buy_list[GUILD_SHOP_COUNT_MAX]);
	const GuildShopCfg* GetShopCfg(int seq);
	int GetGuildBuildInit() { return m_guild_build_init; }

	// �����Ծ�� ��������а�
	const GuildActiveGiftCfg::GiftCfg  * GetGuildActiveGiftCfg(int _seq , int _level);
	const GuildActiveRankRewardCfg * GetGuildActiveRankRewardCfg(int _rank);
	const GuildActivePersonRankRewardCfg * GetGuildActivePersonRankRewardCfg(int _rank);

	//��������
	const GuildInbornLevelCfg * GetGuildInbornCfg(int inborn_type, int type_seq, int level);
	int GetMaxGuildInbornLevel(int inborn_type, int type_seq) const;
	//��������,��������(������+ԭʼĿ��)
	void GetGuildInbornResetSkill(int inborn_type, int seq, GuildInbornSkill * learn_list, int learn_count, ARG_OUT std::vector<ItemConfigData> & return_item_list) const;

	const GuildHongBaoCfg & GetGuildHongBaoCfg() { return m_hongbao_cfg; }
	const GuildHongBaoCfg::HongBaoCfg * GetGuildHongBaoCfgByType(int _hong_bao_type);

	const HallOfFameRank * GetRankTypeCfg(int type, int rank_idx);
	const WarthAddInfo* GetWarthAdd(int battle_mode);
	const BlessTimesList * GetBlessBaseCfg(int times);
	const BlessTimesAttr * GetBlessAttrCfg(int world_level);
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitRoomCfg(TiXmlElement * RootElement);
	int InitMemberTypeCfg(TiXmlElement * RootElement);
	int InitPetTrainingCfg(TiXmlElement * RootElement);
	int InitTrainingExpCfg(TiXmlElement * RootElement);
	int InitGuildShopCfg(TiXmlElement * RootElement);
	int InitGuildBossCfg(TiXmlElement* RootElement);
	int InitGuildBossChapterRewardCfg(TiXmlElement* RootElement);
	int InitGuildJournalCfg(TiXmlElement *RootElement);
	int InitGuildRankRewardCfg(TiXmlElement *RootElement);
	int InitGuildGatherDreamCfg(TiXmlElement *RootElement);
	int InitGuildActiveGiftCfg(TiXmlElement *RootElement);
	int InitGuildActiveRankRewardCfg(TiXmlElement *RootElement);
	int InitGuildActivePersonRankRewardCfg(TiXmlElement *RootElement);
	int InitGuildInbornCfg(TiXmlElement *RootElement);
	int InitGuildHongbaoCfg(TiXmlElement *RootElement);
	int InitGuildConstructCfg(TiXmlElement *RootElement);

	int InitGuildHofSortCfg(TiXmlElement *RootElement);
	int InitGuildWarthGetCfg(TiXmlElement *RootElement);
	int InitGuildBlessBaseCfg(TiXmlElement *RootElement);
	int InitGuildBlessRewardCfg(TiXmlElement *RootElement);

	int GetGuildInbornKey(int inborn_type, int type_seq) const;
	void CalcGuildInbornResetSkill(int guild_inborn_key, GuildInbornSkill * learn_list, int learn_count, ARG_OUT std::map<ItemID, int> & return_item_map) const;
	void GetGuildInbornReturnItem(int guild_inborn_key, int skill_level, ARG_OUT std::map<ItemID, int> & return_item_map) const;

	GuildOtherCfg m_other_cfg;
	std::map<int, GuildRoomCfg> m_room_map;
	std::map<int, GuildMemberTypeCfg> m_member_type_map;
	std::map<int, std::map<int, GuildPetTrainingCfg> > m_pet_training_map;
	std::map<int, int> m_training_exp_map;
	std::map<int, int> m_training_gongxian_map;
	std::map<int, GuildShopCfg> m_shop_map;
	int m_guild_build_init;

	 std::map<int, GuildBossCfg> m_boss_cfg_list;
	 std::map<int, GuildBossChapterRewardCfg> m_guild_boss_chapter_item_reward_map;				//���߽����� key:������ID

	 std::vector<GuildJournalCfg> m_journal_cfg;      //������־

	 GuildRankRewardCfg m_rank_reward_cfg[MAX_GUILD_CHAPTER_BOSS_NUM][MAX_GUILD_BOSS_CHAPTER_RANK_NUM];	//������������ ��һ��Ϊ�½�ID,�ڶ���Ϊ���
	 GuildGatherDreamCfg m_gather_dream_cfg[GUILD_GATHER_DREAM_ITEM_TYPE_MAX][MAX_GATHER_DREAM_TYPE_NUM];		//���弯Ը����

	 // ��Ծ�����
	 std::vector<GuildActiveGiftCfg > m_active_gift_cfg;								// ��Ծ���������
	 std::vector<GuildActiveRankRewardCfg > m_active_rank_reward_cfg;					// �����Ծ�����а���
	 std::vector<GuildActivePersonRankRewardCfg > m_active_person_rank_reward_cfg;		// ������˻�Ծ�����а���

	 GuildInbornCfg m_guild_inborn_list[MAX_GUILD_INBORN_SKILL_TYPE_CUR_NUM][MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_CUR_NUM];	//��������

	 std::map<int, std::vector<int> > m_guild_inborn_relation_map;		// ����������ϵ��(������) vector_value:inborn_type * 100 + type_seq
	 GuildHongBaoCfg m_hongbao_cfg;

	 std::map<int, std::vector<HallOfFameRank> > m_hof_sort_map;
	 std::map<int, WarthAddInfo> m_warth_map;
	 BlessTimesList bless_time_config[MAX_BLESS_TIMES];
	 std::map<int, BlessTimesAttr> m_bless_attr_map;

	 std::vector<GuildConstructCfg> m_guild_construct_cfg;			// ������������
	 int m_hall_max_level;
	 int m_other_build_max_level;
};

#endif