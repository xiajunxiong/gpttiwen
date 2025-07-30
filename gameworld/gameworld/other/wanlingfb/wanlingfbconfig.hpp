#ifndef __WAN_LING_FN_CONFIG_HPP__
#define __WAN_LING_FN_CONFIG_HPP__

#include "servercommon/wanlingfbdef.hpp"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"

#include "gamedef.h"

namespace WanLingFB
{
	struct OtherCfg
	{
		OtherCfg()
		{
			day_battle_reward_time = 0;
			day_pass_reward_time = 0;
		}

		int day_battle_reward_time;	//ÿ��ս����������
		int day_pass_reward_time;	//ÿ��ͨ�ؽ�������
	};

	struct CreatNPCCfg
	{
		CreatNPCCfg()
		{
			npc_id = 0;
			npc_dir = 0;
		}
		int npc_id;
		//λ��
		Posi npc_pos;
		Dir npc_dir;
	};

	struct TaskCfg
	{
		TaskCfg()
		{
			task_seq = 0;
			task_type = 0;
			target_npc_id = 0;

			npc_action = 0;
		}
		int task_seq;		//�������

		int task_type;		//��������
		int target_npc_id;	//����Ŀ��npc_id

		int npc_action;
	};
	enum TaskType
	{//									npc_action
		TASK_TYPE_BATTLE = 1,	//ս��	  ������
		TASK_TYPE_TALK = 2,		//�Ի�	  �Ի�id

		TASK_TYPE_MAX,
	};
	struct FB_Cfg
	{
		FB_Cfg()
		{
			fb_index = 0;
			open_level = 0;
			scene_id = 0;

		}
		int fb_index;	//����index

		int open_level;	//��ɫ����ȼ�����
		int scene_id;	//����id

		Posi role_pos;	//��ɫ������

		Posi leave_pos;	//�뿪���� ������λ��

		//����npc vec
		std::vector<CreatNPCCfg> creat_npc_vec;
		//����vec[����index]
		std::vector<TaskCfg> task_vec;
		//���һ��ս��������seq
		int final_fight_task_seq;

		//ս������
		int battle_pass_coin_bind;	//ͭ��

		//ͨ�ؽ���
		int pass_exp;	//����
		std::vector<ItemConfigData> pass_reward_vec;
		//ͨ�����������id
		std::vector<int> pass_drop_grop_id_vec;
	};

	struct DropCfg
	{
		DropCfg()
		{
			is_broadcast = 0;
		}
		int is_broadcast;
		std::vector<ItemConfigData> base_prof_drop_;	//������Ʒ[����ְҵ]
	};
	//�����map<Ȩ��begin, ��Ʒ>
	typedef std::map<int, DropCfg> DropPool;

	struct DropGroupCfg
	{
		DropGroupCfg()
		{
			group_id = 0;
			total_power = 0;
		}
		int group_id;	//������id

		int total_power;//��Ȩ��
		DropPool drop_pool;
	};

	//������id_map<������id, ��������Ϣ>
	typedef std::map<int, DropGroupCfg> DropGroupIDMap;
};

class WanLingFBConfig : public ILogicConfig
{
public:
	WanLingFBConfig();
	virtual ~WanLingFBConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const WanLingFB::OtherCfg & GetOtherCfg() const { return m_other_cfg; };

	const WanLingFB::FB_Cfg * GetFBCfg(const int fb_index) const;
	const WanLingFB::FB_Cfg * GetFBByLevelCfg(int role_level) const;
	const WanLingFB::TaskCfg * GetTaskCfg(const int fb_index, const int task_seq) const;
	const ItemConfigData * GetDropItem(const int drop_group_id, const int prof, int &is_broadcast) const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitDropGroupCfg(TiXmlElement *RootElement);
	int InitFBCfg(TiXmlElement *RootElement);
	int InitTaskCfg(TiXmlElement *RootElement);
	int InitCreatNpcCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);

	WanLingFB::OtherCfg m_other_cfg;
	//��������[����index]
	std::vector<WanLingFB::FB_Cfg> m_fb_cfg_vec;

	//������map<������id, ����������>
	WanLingFB::DropGroupIDMap m_drop_group_id_map;
};

#endif