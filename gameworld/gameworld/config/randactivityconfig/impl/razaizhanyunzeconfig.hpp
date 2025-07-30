#ifndef __RA_ZAI_ZHAN_YUN_ZE_CONFIG_HPP__
#define __RA_ZAI_ZHAN_YUN_ZE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"
#include "servercommon/roleactivity/razaizhanyunzeparam.hpp"

namespace RAZaiZhanYunZe
{
	//����
	struct OtherCfg
	{
		OtherCfg()
		{
			battle_cost_item_id = 0;
		}
		ItemID battle_cost_item_id;		//��ս������Ʒid
	};

	struct BossCfg
	{
		BossCfg()
		{
			boss_index = 0;

			battle_cost_item_num = 0;

			act_open_day = 0;
			monster_group_id = 0;

			pass_rand_id = 0;
			first_pass_id = 0;
		}

		int boss_index;

		int battle_cost_item_num;

		int act_open_day;		//���������
		int monster_group_id;	//������id

		int pass_rand_id;	//ͨ�����������id(�����)

		int first_pass_id;	//��ͨ������id(�̶���)

		//ͨ�ض��⽱����map<ͨ�ش���, ������id(�̶���)>
		typedef std::map<int, int> PassNumIDMap;
		PassNumIDMap pass_num_id_map;
	};

	//����vec
	typedef std::vector<ItemConfigData> RewardVec;
	
	typedef std::map<int, RewardVec> DefSectionDayMap;
	//�̶�������
	struct DefRewardGroupCfg
	{
		DefRewardGroupCfg()
		{
			group_id = 0;
		}
		int group_id;

		//�̶������׶�����map<�����ʱ�Ŀ�������, ����>
		DefSectionDayMap section_cfg_map;
	};
	//�̶�������map<������id, �̶�������>
	typedef std::map<int, DefRewardGroupCfg> DefGroupIdMap;


	//�������
	struct RandCfg
	{
		RandCfg()
		{
			index = 0;
		}
		int index;				//����index
		RewardVec reward_vec;	//����
	};
	//����<Ȩ��end, ����>
	typedef std::map<int, RandCfg> RandPool;
	//��������׶�����
	struct RandSectionCfg
	{
		RandSectionCfg()
		{
			game_open_day_begin = 0;
			total_power = 0;
		}
		int game_open_day_begin;	//�����ʱ�Ŀ�������

		int total_power;			//��Ȩ��							
		RandPool rand_pool;
	};
	//�׶�����<�����ʱ�Ŀ�������, ����>
	typedef std::map<int, RandSectionCfg> RandSectionDayMap;
	//���������
	struct RandRewardGroupCfg
	{
		RandRewardGroupCfg()
		{
			group_id = 0;
		}
		int group_id;

		RandSectionDayMap section_day_map;
	};
	//���������map<������id, ���������>
	typedef std::map<int, RandRewardGroupCfg> RandGroupIdMap;
};

class RAZaiZhanYunZeConfig : public RandActivityConfig
{
public:
	RAZaiZhanYunZeConfig();
	virtual ~RAZaiZhanYunZeConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAZaiZhanYunZe::OtherCfg& GetOtherCfg() const { return m_other_cfg; };

	const RAZaiZhanYunZe::BossCfg* GetBossCfg(int boss_index) const;
	const RAZaiZhanYunZe::RewardVec* GetDefRewardGroupCfg(RandActivityManager* ramgr, int def_group_id) const;
	const RAZaiZhanYunZe::RewardVec* GetRandRewardGroupCfg(RandActivityManager* ramgr, int rand_group_id) const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitDefGroupIdMap(TiXmlElement *RootElement);
	int InitRandGroupIdMap(TiXmlElement *RootElement);
	int InitBossCfg(TiXmlElement *RootElement);

	//�鿴����������
	bool CheckDefGroupID(int group_id) const;
	bool CheckRandGroupID(int group_id) const;

	RAZaiZhanYunZe::OtherCfg m_other_cfg;	//����

	RAZaiZhanYunZe::DefGroupIdMap m_def_group_id_map;	//�̶�������
	RAZaiZhanYunZe::RandGroupIdMap m_rand_group_id_map;	//���������

	std::vector<RAZaiZhanYunZe::BossCfg> m_boss_cfg_vec;//boss����[boss_index]

};



#endif