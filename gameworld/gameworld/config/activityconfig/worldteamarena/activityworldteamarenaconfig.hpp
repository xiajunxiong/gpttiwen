#ifndef __ACTIVITY_WORLD_TEAM_ARENA_CONFIG_HPP__
#define __ACTIVITY_WORLD_TEAM_ARENA_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"

struct ActivityWorldTeamArenaOtherCfg
{
	ActivityWorldTeamArenaOtherCfg() : need_level(0), scene_id(0), max_fail_num(0), max_rounds(0), win_score(0), lose_score(0), draw_score(0), refresh_time(0),
		countdown(0), registration_type(0), registration_price(0), battle_protect(0)
	{}

	int need_level;				//��ɫ�ȼ�/����ȼ�
	int scene_id;				//�����ID
	int max_fail_num;			//���ս��ʧ�ܴ���
	int max_rounds;				//���ս���غ���
	int win_score;				//ʤ����û���
	int lose_score;				//ʧ�ܻ�û���
	int draw_score;				//ƽ�ֻ�û���
	int refresh_time;			//����ˢ��ʱ��(��)
	int countdown;				//��������ʱ(��)
	int registration_type;		//�����ѻ�������
	int registration_price;		//�����۸�
	int battle_protect;			//ս������ʱ��
};

struct ActivityWorldTeamArenaParRewardCfg
{
	ActivityWorldTeamArenaParRewardCfg() : exp(0), coin(0)
	{}

	int exp;
	int coin;
	std::vector<ItemConfigData> reward_list;
};

struct ActivityWorldTeamArenaRankRewardCfg
{
	ActivityWorldTeamArenaRankRewardCfg() : coin(0), title_id(0)
	{}

	int coin;
	int title_id;
	std::vector<ItemConfigData> reward_list;
};

struct ActivityWorldTeamArenaLevelRankRewardCfg
{
	ActivityWorldTeamArenaLevelRankRewardCfg() : min_role_level(0), max_role_level(0)
	{}

	int min_role_level;
	int max_role_level;
	std::map<ParamSection, ActivityWorldTeamArenaRankRewardCfg> rank_reward;
};

class ActivityWorldTeamArenaConfig : public ILogicConfig
{
public:
	ActivityWorldTeamArenaConfig() {}
	virtual ~ActivityWorldTeamArenaConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ActivityWorldTeamArenaOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const ActivityWorldTeamArenaParRewardCfg * GetParRewardCfg(int level);
	const ActivityWorldTeamArenaRankRewardCfg * GetRankRewardCfg(int rank, int role_level);
	Posi GetRandBornPosi();
	const std::map<int, int> & GetRankTitleListCfg() { return m_rank_reward_title_list_cfg; }

private:
	int InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitParticicpationCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitRankAwardCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitBornPosiCfg(TiXmlElement * RootElement, std::string& errormsg);

	ActivityWorldTeamArenaOtherCfg m_other_cfg;
	std::map<ParamSection, ActivityWorldTeamArenaParRewardCfg> m_particicpation_map;	// ���뽱����    �� - �ȼ�����
	std::vector<ActivityWorldTeamArenaLevelRankRewardCfg> m_rank_award_cfg;				// �����������	�� - ��������
	std::vector<Posi> m_born_list;														// ������λ

	std::map<int, int> m_rank_reward_title_list_cfg;									// �洢���������ĳƺ�
};

#endif