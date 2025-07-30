#ifndef __HONGMENG_TIANJIE_CONFIG_HPP__
#define __HONGMENG_TIANJIE_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/hongmengtianjiedef.hpp"

#include <map>

struct HongMengTianJieOtherCfg
{
	HongMengTianJieOtherCfg() : start_level(0), level_limit(0), help_times(0), boss_reward_limit(0), help_reward_limit(0)
	{
		memset(reward_times, 0, sizeof(reward_times));
	}

	int start_level;			// ��������ȼ�
	int level_limit;

	std::vector<int> boss_reward_repeat;			// ���콱������ʱ��
	std::vector<int> help_reward_repeat;			// ��ս��������ʱ��
	int reward_times[HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM];
	int help_times;				// ��ս����

	int boss_reward_limit;		// �����������
	int help_reward_limit;		// ��ս��������
};

struct HongMengTianJieFbCfg
{
	HongMengTianJieFbCfg():fb_id(0), equipment_requirement(0), scene_id(0), door_pos(0, 0), first_difficult(0), down_percent(0), min_difficult(0) {}

	int fb_id;
	int equipment_requirement;			// װ������
	int scene_id;
	Posi door_pos;

	int first_difficult;				// ��ʼ�Ѷ�
	int down_percent;					// �µ�����
	int min_difficult;					// ����Ѷ�
};

struct HongMengTianJieNpcCfg
{
	struct NpcData
	{
		int npc_id;
		int monster_group;
		Posi pos;
	};

	HongMengTianJieNpcCfg()
	{
		memset(npc_param, 0, sizeof(npc_param));
		memset(&leave_pos, 0, sizeof(leave_pos));
	}
	
	NpcData npc_param[HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM];

	Posi leave_pos;
};

struct HongMengTianJieRewardCfg
{
	std::vector<ItemConfigData> person_reward;			// ������ͨ����
	std::vector<ItemConfigData> world_reward;			// ȫ����ͨ����
	std::vector<ItemConfigData> help_reward;			// ��ս����

	std::vector<int> success_reward[HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM];
};

struct HongMengTianJieItemCfg
{
	ItemConfigData item;
	bool is_broadcast;
};

struct HongMengTianJieGroupCfg
{
	HongMengTianJieGroupCfg():max_rate(0) {}

	int max_rate;
	std::map<int, HongMengTianJieItemCfg> reward_list;
};

struct HongMengTianJieStoreCfg
{
	HongMengTianJieStoreCfg() : item_id(0), is_bind(true), num(0), price(0), conver_item_id(0), limit_seq(0), limit_convert_count(0) {}

	ItemID item_id;
	bool is_bind;
	int num;
	int price;
	ItemID conver_item_id;
	int limit_seq;
	int limit_convert_count;
};

class HongMengTianJieConfig : public ILogicConfig
{
public:
	HongMengTianJieConfig();
	virtual ~HongMengTianJieConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const HongMengTianJieOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const HongMengTianJieFbCfg * GetFBCfg(int fb_type, int fb_seq);
	const HongMengTianJieNpcCfg * GetNpcCfg(int fb_id);
	const HongMengTianJieRewardCfg * GetRewardCfg(int fb_id);
	const HongMengTianJieItemCfg * GetGroupReward(int group_id);
	const HongMengTianJieStoreCfg * GetStoreCfg(int seq);
private:
	int InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitFbCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitNpcCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitRewardCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitGroupCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitStoreCfg(TiXmlElement * RootElement, std::string& errormsg);


	HongMengTianJieOtherCfg m_other_cfg;
	std::map<int, std::vector<HongMengTianJieFbCfg> > m_fb_cfg;
	std::vector<HongMengTianJieNpcCfg> m_npc_cfg;
	std::vector<HongMengTianJieRewardCfg> m_reward_cfg;
	std::map<int, HongMengTianJieGroupCfg> m_group_cfg;
	std::vector<HongMengTianJieStoreCfg> m_store_cfg;
};

#endif