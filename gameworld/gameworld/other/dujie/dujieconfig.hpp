#ifndef __DU_JIE_CONFIG_HPP__
#define __DU_JIE_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/dujiedef.hpp"

#include "gamedef.h"

struct DuJieOtherCfg
{
	DuJieOtherCfg() :level_gap(0)
	{}

	int level_gap;		//��Ա�ȼ����

	//��ս
	int day_assist_times;		//ÿ����ս��������
	std::vector<ItemConfigData> assist_reward;	//��ս����
};

//////�ɽ�
//�ؿ�����
struct DuJieFbCfg
{
	static const int MAX_REWARD_NUM = 5;

	DuJieFbCfg()
	{
		fb_index = 0;
		front_task_type = 0;
		front_task_param_1 = 0;
		front_task_param_2 = 0;

		scene_id = 0;

		boss_dir = 0;
		boss_npc_id = 0;

		reward_num = 0;

		reward_title_id = 0;
	}

	int fb_index;	//�ؿ�index

	//ǰ������
	int front_task_type;	//��������
	int front_task_param_1;	//�������
	int front_task_param_2;	//�������

	//������Ϣ
	int scene_id;	//����id

	//������Ϣ
	Posi boss_pos;
	Dir boss_dir;
	int boss_npc_id;

	//��ɫ������
	Posi role_pos;

	//����
	int reward_num;
	ItemConfigData reward_list_[MAX_REWARD_NUM];
	UInt16 reward_title_id;

	////��ս
	//int day_assist_times;		//ÿ����ս��������
	//std::vector<ItemConfigData> assist_reward;	//��ս����
};

class DuJieConfig : public ILogicConfig
{
public:
	DuJieConfig();
	virtual ~DuJieConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const DuJieOtherCfg & GetOtherCfg()const { return m_other_cfg; }
	const DuJieFbCfg* GetFbCfg(int fb_index) const;

	int GetTaskShanHaiBossHard() const { return m_task_shan_hai_boss_hard; };
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitFBCfg(TiXmlElement *RootElement);

	//������Ϣ[����level]
	DuJieOtherCfg m_other_cfg;
	int m_fb_count;
	DuJieFbCfg m_fb_cfg_[DU_JIE_MAX_FB_NUM];

	//�����¼��ɽ��boss�Ѷ�
	int m_task_shan_hai_boss_hard;

};

#endif
