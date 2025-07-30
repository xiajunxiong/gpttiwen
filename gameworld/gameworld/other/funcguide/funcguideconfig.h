#ifndef __FUNC_GUIDE_CONFIG_H__
#define __FUNC_GUIDE_CONFIG_H__

#include "gamedef.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"

#include <map>

struct GuideFBLevelCfg
{
	GuideFBLevelCfg()
	{
		scene_id=0;
		monster_npc_id=0;
		monster_pos= Posi(0,0);
		box_id = 0;
		box_pos= Posi(0, 0);
		boss_npc_id = 0;
		boss_pos = Posi(0, 0);
	}

	int scene_id;
	int monster_npc_id;
	Posi monster_pos;
	int box_id;
	Posi  box_pos;
	int boss_npc_id;
	Posi boss_pos;

	std::vector<ItemConfigData> box_reward;
};

struct FuncGuideOtherCfg
{
	FuncGuideOtherCfg() : guide_fb_task_id(0), draw_guide_pet_comsume_item(0), catch_pet_guide_monster_group_id(0), catch_pet_guide_monster_pet_item_id(0)
	{
		draw_guide_pet_2_reward_special_item_id = 0;
		draw_guide_pet_2_pet_level = 0;

		guide_pet_id = 0;
		guide_replace_passive_skill_item_id = 0;
		guide_replace_passive_skill_id = 0;
		guide_replace_passive_skill_replace_index = 0;
		guide_pet_id_2 = 0;

		guide_comprehend_skill_id = 0;

		draw_guide_partner_comsume_item_id = 0;
		draw_guide_partner_comsume_item_num = 0;
		draw_guide_partner_comsume_item_id_2 = 0;
		draw_guide_partner_comsume_item_num_2 = 0;

		fast_dream_notes_level = 0;
		pass_lunhui_wangchuan_level = 0;
	}

	int guide_fb_task_id;
	int draw_guide_pet_comsume_item;			//Ѱ��ָ�� ��������ĵ���
	int catch_pet_guide_monster_group_id;
	int catch_pet_guide_monster_pet_item_id;

	int draw_guide_pet_2_reward_special_item_id;		//Ѱ��ָ��2 ���Ĺ̶�����specialitem ��Ʒid 
	int draw_guide_pet_2_pet_level;			//Ѱ��ָ��2  ����̶��ȼ�
		
	int guide_pet_id;		//����������ָ����ѧ����ָ�� ����id
	int guide_replace_passive_skill_item_id;		//����ָ��������Ʒid
	int guide_replace_passive_skill_id;			//�������ָ������id
	int guide_replace_passive_skill_replace_index;	//���ò���index ��������-1�ˣ�ָ��ѧ�������滻�ļ���λ��
	int guide_pet_id_2;		//�ܳ���ϴ��ָ�� �� ����id

	int guide_comprehend_skill_id;

	int draw_guide_partner_comsume_item_id;			//Ѱ��ָ�� �������ĵ���
	int draw_guide_partner_comsume_item_num;		//Ѱ��ָ�� �������ĵ���
	std::vector<ItemConfigData> draw_guide_partner_reward;
	int draw_guide_partner_comsume_item_id_2;		//Ѱ��ָ�� �������ĵ���2
	int draw_guide_partner_comsume_item_num_2;		//Ѱ��ָ�� �������ĵ���2
	std::vector<ItemConfigData> draw_guide_partner_reward_2;

	int fast_dream_notes_level;						// ɨ����Ԩ��¼����
	int pass_lunhui_wangchuan_level;				// ͨ���ֻ���������
};

class FuncGuideConfig : public ILogicConfig
{
public:
	FuncGuideConfig();
	virtual ~FuncGuideConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ItemConfigData * GetGuidePet();

	const GuideFBLevelCfg * GetGuideFBLevelCfg(int layer);
	bool IsLastLayer(int layer);
	const FuncGuideOtherCfg & GetOtherCfg();
private:
	int InitPetChoiceOptionCfg(TiXmlElement * RootElement);
	int InitGuideFbLevelCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitOtherCfg2(TiXmlElement * RootElement);
	int InitOtherCfg3(TiXmlElement * RootElement);

	std::vector<ItemConfigData> m_guide_pet_cfg;			//Ѱ���������ֳ�������б�
	std::vector<GuideFBLevelCfg> m_guide_level_cfg;
	FuncGuideOtherCfg m_other_cfg;
};

#endif