#ifndef __ACTIVITY_RELIC_LOOTING_CONFIG__
#define __ACTIVITY_RELIC_LOOTING_CONFIG__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/commonsavedef.h"
#include "global/activity/activityimpl/activityreliclooting.hpp"

#include <set>
#include <map>
#include <vector>

struct RelicLootingOtherCfg
{
	RelicLootingOtherCfg() 
	{
		scene_id = 0;
		duration = 0;
		pick_distance = 0;
		first_props = 0;
		props_time = 0;
		props_num = 0;
		memset(defender_pos_list, 0, sizeof(defender_pos_list));
		memset(attacker_pos_list, 0, sizeof(attacker_pos_list));
		once_team_players = 1;
		win_box_num = 0;

		box_score = 0;
		capture_score = 0;
		first_score = 0;
		second_score = 0;
		third_score = 0;
		continu_score = 0;
		time_score_3 = 0;
		time_score_5 = 0;
		time_score_8 = 0;
		winner_score = 0;
		loser_score = 0;
	}

	int scene_id;
	int duration;
	int pick_distance;
	int first_props;
	int	props_time;
	int props_num;
	Posi defender_pos_list[RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM];
	Posi attacker_pos_list[RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM];
	int once_team_players;
	int win_box_num;

	int box_score;
	int	capture_score;
	int first_score;
	int	second_score;
	int	third_score;
	int	continu_score;
	int	time_score_3;
	int	time_score_5;
	int	time_score_8;
	int	winner_score;
	int	loser_score;
};

struct RelicLootingSkillCfg
{
	RelicLootingSkillCfg() :sn(SKILL_ID_0), skill_type(SKILL_TYPE_NO), param1(0), param2(0), param3(0), param4(0), skill_limit(SKILL_LIMIT_0), res_id(0), res_dir(0), res_range(0) {}

	enum SKILL_ID
	{
		SKILL_ID_0 = 0,		//��
		SKILL_ID_1 = 1,		//ץ��		param1���ӵļ�������ʱ��󣬶Է�δ�뿪ʩ����Χparam2��ɹ�ץ�����Է����������״̬�޷��ƶ�����ȴʱ��param3�롣Param4ץ����Χ
		SKILL_ID_2 = 2,		//����		��������param1����ƶ��ٶ�����param2%������param3�룬��ȴʱ��param4��
		SKILL_ID_3 = 3,		//����		ʩ��ʱ��param1�룬���͵������е�һ�����λ�ã���ȴʱ��param2��
		SKILL_ID_4 = 4,		//����		ʩ��ʱ��param1�룬��һ������ķ�ӡ������ȴʱ��param2�룬����Χ���ޱ���ʱ���޷��ͷţ�ʩ���������ƶ�����Ͽ������ܡ�Param3�����䷶Χ
		SKILL_ID_5 = 5,		//��Ԯ		ʩ��ʱ��param1�룬�����Χ��һ���ѷ���ɫ�Ľ���״̬����ȴʱ��param2�룬����Χ���ޱ���������ʱ���޷��ͷţ�ʩ���������ƶ�����Ͼ�Ԯ���ܣ����ܽ�����ȴ��Param3��Ԯ��Χ
		SKILL_ID_6 = 6,		//����		��������param1���param2������Ϊ���ɼ�״̬������״̬�ͷż��ܽ������У��޷���ץ����ץ������ʱ��δ�뿪ץ����Χ���Իᱻץ������ȴʱ��param3��
		SKILL_ID_7 = 7,		//�����	��������param1�룬��������param2%�ƶ��ٶȣ�����param3�룬Ч����������������״̬���ƶ��ٶȽ���param4%������param5��
		SKILL_ID_8 = 8,		//������	ʩ��ʱ��param1�룬�ͷż��ܽ�����ǰ����һ�������ˣ�param2�����ٴ��ͷż������͵������˵�λ�ã�param3������NPCid
		SKILL_ID_9 = 9,		//������	ʩ��ʱ��param1�룬������͵�������һλ��
		SKILL_ID_10 = 10,	//���ͷ���	����param1ʱ�䣬ѯ�ʶ����Ƿ��������͵�����ǰλ��,����ʱ���ڶ���ͬ�⽫���͵���ǰλ�á�
		SKILL_ID_11 = 11,	//����		�ڵ�ǰλ�÷���һ���������壬�������param1�룬��������·���������param2���ӣ��޷��ƶ���Param3������Χ��param4����NPCid
		SKILL_ID_12 = 12,	//��������	��ԭ���ͷ�һ������Ѳ���Ի���ˣ�Ѳ�߷���Ϊ�ͷ�ʱ��ҷ��򡣷������param1�����ʧ
		SKILL_ID_13 = 13,	//������	��������param1���param2������Ϊ���ɼ�״̬������״̬�ͷż��ܽ������У��޷���ץ����ץ������ʱ��δ�뿪ץ����Χ���Իᱻץ��
		SKILL_ID_14 = 14,	//Ų�Ƴ�	�ͷź�������ǰ������Ų��param1����
	};

	enum SKILL_TYPE
	{
		SKILL_TYPE_NO = 0,	//��
		SKILL_TYPE_STOP = 1,//ʩ��
		SKILL_TYPE_MOVE = 2,//����
	};

	enum SKILL_LIMIT
	{
		SKILL_LIMIT_0 = 0,//ͨ�ü���
		SKILL_LIMIT_1 = 1,//�����߼���
		SKILL_LIMIT_2 = 2,//�����߼���
	};

	int sn;
	int	skill_type;
	int	param1;
	int	param2;
	int	param3;
	int	param4;
	int	param5;
	int	skill_limit;
	int	res_id;
	int	res_dir;
	int	res_range;
};

struct SkillDropCfg
{
	SkillDropCfg() :sn(0), res_chance(0), npc_id(0) {}
	int sn;
	int res_chance;
	int npc_id;
};

struct DropPositionCfg
{
	DropPositionCfg() :box_sn(0), box_x(0), box_y(0), boss_dir(0), npc_id(0){}
	int box_sn;
	int	box_x;
	int	box_y;
	int	boss_dir;
	int npc_id;
};

struct RewardBoxCfg
{
	RewardBoxCfg() : sn(0), param(0){}
	int sn;
	int param_type;
	int param;
	std::vector<ItemConfigData> m_reward_vec;
};

struct RankRewardCfg
{
	RankRewardCfg() : sn(0), rank_range_min(0), rank_range_max(0) {}
	int sn;
	int	rank_range_min;
	int	rank_range_max;
	std::vector<ItemConfigData> m_rank_reward_vec;
};

class ActivityRelicLootingConfig : public ILogicConfig
{
public:
	ActivityRelicLootingConfig();
	virtual ~ActivityRelicLootingConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const RelicLootingOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const std::map<int, RelicLootingSkillCfg> * GetSkillInfoList() { return &m_skill_info_map; }
	const RelicLootingSkillCfg * GetSkillDetail(int skill_id);
	const std::vector<DropPositionCfg> * GetDropPositionVec() { return &m_drop_posi_vec; }
	const SkillDropCfg* RandSkill();
	const RankRewardCfg * GetRankRewardCfg(int rank_pos);
	const RewardBoxCfg * GetRewardCfg(int sn);
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitSkillCfg(TiXmlElement *RootElement);
	int InitResReleaseCfg(TiXmlElement *RootElement);
	int InitBoxPositionCfg(TiXmlElement *RootElement);
	int InitBoxRewardCfg(TiXmlElement *RootElement);
	int InitRankRewardCfg(TiXmlElement *RootElement);

	RelicLootingOtherCfg m_other_cfg;
	std::map<int, RelicLootingSkillCfg> m_skill_info_map;
	int drop_weight;
	std::vector<SkillDropCfg> m_skill_drop_vec;

	std::vector<DropPositionCfg> m_drop_posi_vec;
	std::map<int, RewardBoxCfg> m_reward_box_map;
	std::map<int, RankRewardCfg> m_rank_reward_map;
};

#endif 