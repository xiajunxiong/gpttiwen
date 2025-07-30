#ifndef __PET_GOD_FB_CONFIG_HPP__
#define __PET_GOD_FB_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/petgodfbdef.hpp"

#include "gamedef.h"

//��ӡ֮��
class PetGodFbConfig : public ILogicConfig
{
public:
	//����
	struct OtherCfg
	{
		OtherCfg()
		{
			day_base_times = 0;
		}

		int day_base_times;		//ÿ�ջ�������  С��0:������
	};

	//��������
	struct FBCfg
	{
		FBCfg()
		{
			fb_seq = 0;
			fb_type = 0;
			fb_level = 0;

			scene_id = 0;
			role_level_limit = 0;
			cost_power = 0;

			auto_limit_cap = 0;

			recover_npc_id = 0;
			recover_npc_dir = 0;

			boss_dir = 0;
			boss_npc_id = 0;
		}
		int fb_seq;				//����seq
		int fb_type;			//�������� 1��ʼ
		int fb_level;			//�����ȼ� 1��ʼ

		int scene_id;			//����id
		int role_level_limit;	//��ɫ�ȼ�����
		int cost_power;			//������������

		int auto_limit_cap;		//ɨ�������ۺ�ս��

		//������id[����������]
		std::vector<int> reward_group_list;
		int draw_num;


		//��ɫ������
		Posi role_pos;

		//�ָ�npc
		int recover_npc_id;
		Posi recover_npc_pos;
		Dir recover_npc_dir;


		Posi boss_pos;
		Dir boss_dir;
		int boss_npc_id;

	};


	//������ [������id][�ȼ�][Ȩ�س�]
	struct RewardItemInfo
	{
		RewardItemInfo()
		{
			group_id = 0;
			power = 0;
			role_min_level = 0;
			role_max_level = 0;
		}
		int group_id;		//������id
		int power;

		//��ɫ�ȼ����� ������
		int role_min_level;
		int role_max_level;

		ItemConfigData reward_item;
	};

	//����Ȩ��map <��ʼȨ��, ����>
	typedef std::map<int, RewardItemInfo> RewardPowerMap;
	struct RewardPool
	{
		RewardPool()
		{
			total_power = 0;
		}
		int total_power;
		RewardPowerMap power_pool;
	};

	//��������Ϣmap<��ͽ�ɫ�ȼ�, ��������Ϣ>
	typedef std::map<int, RewardPool> RewardPoolLevelMap;

	//������ID map <������id, ������>
	typedef std::map<int, RewardPoolLevelMap> RewardGroupIDMap;

	//�������
	struct BuyTimeCfg
	{
		BuyTimeCfg()
		{
			buy_times = 0;
			cost_gold = 0;
		}

		int buy_times;		//�ܹ������
		int cost_gold;		//��������Ԫ��
	};
	//������ID map <�ܹ������, ����>
	typedef std::map<int, BuyTimeCfg> BuyTimeCfgMap;
public:
	PetGodFbConfig();
	virtual ~PetGodFbConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const OtherCfg& GetOtherCfg() const { return m_other_cfg; };
	const FBCfg* GetFBCfg(int fb_seq) const;

	bool GetFBRewardData(int fb_seq, int role_level, std::vector<ItemConfigData> &out_item_list) const;

	const BuyTimeCfg * GetBuyTimeCfg(int buy_times) const;

	long long GetRewardExp(int  role_level) const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement *RootElement);
	int InitBuyTimesCfg(TiXmlElement *RootElement);
	int InitEXPRewardCfg(TiXmlElement *RootElement);
	int InitFBCfg(TiXmlElement *RootElement);

	const RewardItemInfo* GetGroupReward(int group_id, int role_level) const;


	OtherCfg m_other_cfg;
	//��������[����seq]
	std::map<int, FBCfg> m_fb_cfg_map;

	//������ [������id][�ȼ�]
	RewardGroupIDMap m_reward_group_id_map;

	//�������
	BuyTimeCfgMap m_buy_times_map;
	//���齱��[��ɫ�ȼ�]
	std::vector<long long> m_exp_reward_list;
};

#endif