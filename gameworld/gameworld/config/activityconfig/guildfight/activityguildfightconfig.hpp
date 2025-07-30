#ifndef __ACTIVITY_GUILD_FIGHT_CONFIG__
#define __ACTIVITY_GUILD_FIGHT_CONFIG__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/commondata.hpp"

#include <map>
#include <set>
#include <vector>

static const int GUILD_FIGHT_AWARD_MAX_COUNT = 10;
static const int GUILD_FIGHT_CHEST_MAX_AWARD = 5;
static const int GUILD_FIGHT_BORN_POSI_MAX_NUM = 8;			//����������λ�����

struct GuildFightDeleteData
{
	GuildFightDeleteData() :box_id(0), obj_id(0) {}
	GuildFightDeleteData(int _box_id, ObjID _obj_id) : box_id(_obj_id), obj_id(_obj_id) {}
	int box_id;
	ObjID obj_id;
};

struct GuildFightAwardCon
{
	GuildFightAwardCon() : award_num(0) {}

	int award_num;
	ItemConfigData award_list[GUILD_FIGHT_AWARD_MAX_COUNT];
};

struct GuildFightOtherCfg
{
	GuildFightOtherCfg() : scene_id(0), max_fail_times(0), max_rounds_num(0), wins_score(0),
		lose_score(0), draw_score(0), rank_refresh_interval(0), guild_fund(0), num(0), max_num(0), 
		countdown(0), join_reward_coin(0), join_reward_gongxian(0)
	{}

	int scene_id;					// ����ID
	int max_fail_times;				// ���ʧ�ܴ���
	int max_rounds_num;				// ����ս�����غ���
	int wins_score;					// ʤ�����û���
	int lose_score;					// ʧ�����û���
	int draw_score;					// ƽ�����û���
	int rank_refresh_interval;		// ����������ˢ�¼�� - ��λ:��(s)
	int guild_fund;					// �����ʽ�
	int time;						// ˢ��ʱ��
	int num;						// һ��ˢ�±������
	int max_num;					// ͬʱ���ڱ������
	int countdown;					// �볡����ʱ

	int join_reward_coin;
	int join_reward_gongxian;
	GuildFightAwardCon pati_award;	// ���뽱
};

struct GuildFightChestCfg
{
	GuildFightChestCfg() : box_id(0), rate(0), award_id_num(0), caotain_award_id_num(0)
	{
		memset(award_id_list, 0, sizeof(award_id_list));
		memset(caotain_award_id_list, 0, sizeof(caotain_award_id_list));
	}

	int box_id;
	int rate;

	int award_id_num;
	int award_id_list[GUILD_FIGHT_CHEST_MAX_AWARD];

	int caotain_award_id_num;
	int caotain_award_id_list[GUILD_FIGHT_CHEST_MAX_AWARD];
};

struct GuildFightItemRand
{
	GuildFightItemRand() : prob(0) {}

	int prob;
	ItemConfigData award_item;
};

struct GuildFightRankAward		// ������������
{
	GuildFightRankAward() : title(0), contribution(0), coin(0) {}

	int title;			// �ƺŽ���
	int contribution;
	int coin;			
	GuildFightAwardCon item_reward;
};

struct GuildFightSurvAward		// ����
{
	GuildFightSurvAward() : seq(0), coin_bind(0), experience(0) {}

	int seq;
	int coin_bind;
	int experience;
	GuildFightAwardCon item_reward;
};

struct GuildFightJoinAwardCfg
{
	GuildFightJoinAwardCfg() : min_level(0), max_level(0), exp(0), coin(0)
	{}
	
	int min_level;
	int max_level;
	int exp;
	int coin;
	std::vector<ItemConfigData> reward_list;
};

struct GuildFightFindRewardGroupCfg
{
	GuildFightFindRewardGroupCfg() :rate(0)
	{}

	int rate;
	ItemConfigData reward;
};

struct GuildFightFindCfg
{
	GuildFightFindCfg() :min_level(0), max_level(0), exp(0), coin(0), family_gongxian(0), group_id(0)
	{}

	int min_level;
	int max_level;
	int exp;
	int coin;
	int family_gongxian;
	int group_id;
};

struct GuildFightFindGroupCfg
{
	GuildFightFindGroupCfg()
	{}

	std::vector<GuildFightFindCfg> node_list;
};

struct GuildFightRankBuffCfg
{
	GuildFightRankBuffCfg()
	{
	}

	std::vector<int> buff_id;
};

struct GuildFightRoleLevelRankAward
{
	GuildFightRoleLevelRankAward()
	{
		min_role_level = 0;
		max_role_level = 0;
	}

	int min_role_level;
	int max_role_level;

	std::map<ParamSection, GuildFightRankAward> rank_reward;
};

class ActivityGuildFightConfig : public ILogicConfig
{
public:
	ActivityGuildFightConfig() {}
	virtual ~ActivityGuildFightConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	void GetChestIDList(std::vector<int> & out_list);
	void GetSurvivalCheckNumList(std::set<int> & out_list);

	const GuildFightOtherCfg & GetGuildFightOtherCfg() { return m_other_cfg; }
	const GuildFightChestCfg * GetGuildFightChestCfg(int chest_id);
	const GuildFightRankAward * GetGuildFightActivityAward(int rank_posi, int role_level);
	const GuildFightSurvAward * GetGuildFightSurvivalAward(int check_num, int level);
	bool RandAwardListByChestID(int chest_id, std::vector<ItemConfigData> & out_award_list, bool is_leader);
	const GuildFightChestCfg* GetRandGuildFightChestCfg();
	const GuildFightJoinAwardCfg * GetGuildFightJoinAwardCfg(int role_level);

	const GuildFightFindCfg * GetFindCfg(int find_type, int role_level)const;
	const GuildFightFindRewardGroupCfg * GetFindRewardGroupCfg(int group_id)const;

	Posi GetRandBornPosi();
	int GetGuildFightRankTitle() { return guild_fight_title_id; }
private:
	int InitGuildFightOthersCfg(TiXmlElement * RootElement);
	int InitGuildFightRankAward(TiXmlElement * RootElement);
	int InitGuildFightSurvAward(TiXmlElement * RootElement);
	int InitGuildFightChestsCfg(TiXmlElement * RootElement);
	int InitGuildFightItemsRand(TiXmlElement * RootElement);
	int InitGuildFightBornPosiCfg(TiXmlElement * RootElement);
	int InitGuildFightJoinAwardCfg(TiXmlElement * RootElement);
	int InitGuildFightFindAwardGroupCfg(TiXmlElement * RootElement);
	int InitGuildFightFindAwardCfg(TiXmlElement * RootElement);

	GuildFightOtherCfg m_other_cfg;
	std::vector<GuildFightRoleLevelRankAward> m_rank_award;						// �����������	�� - ��������
	std::map<int, std::map<ParamSection, GuildFightSurvAward> > m_survival_award;	// �Ҵ��߽�����	�� - �Ҵ����ٽ��������ȼ�����
	int chest_weight;
	std::map<int, GuildFightChestCfg> m_chest_cfg;									// �������ã�	�� - ����ID
	std::map<int, std::vector<GuildFightItemRand> > m_item_group_cfg;				// ���佱���飺	�� - ��ID

	int m_born_count;																// ��ǰ��������
	Posi m_born_list[GUILD_FIGHT_BORN_POSI_MAX_NUM];								// ������λ
	int guild_fight_title_id;														// ������Ҷ���������ID

	std::vector<GuildFightJoinAwardCfg> m_join_award_vec;							// ���뽱��
	std::map<int, std::vector<GuildFightFindRewardGroupCfg> > m_find_reward_group_map;			// �һص�����:key - group_id
	GuildFightFindGroupCfg m_find_cfg[RB_TYPE_MAX];			// �����һ�
};

#endif // !__ACTIVITY_GUILD_FIGHT_CONFIG__