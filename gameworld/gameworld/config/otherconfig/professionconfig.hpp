#ifndef __PROFESSION_CONFIG_HPP__
#define __PROFESSION_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/serverdef.h"
#include "servercommon/basedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/struct/advancecareerparam.hpp"

#include <map>
#include <set>
#include <vector>

const static int PROFESSION_SKILL_INIT_MAX_NUM = 4;
const static int CHANGE_PRO_ITEM_MAX_NUM = 5;

struct ProfSkillCfg
{
	ProfSkillCfg() : prof_base_type(0), proficient_type(0), skill_id(0), skill_level_limit(0), skill_learn_level(0) {}

	short prof_base_type;
	short proficient_type; // [0:���⼼  1:ͨ�ü�  2:�����]
	UInt16 skill_id;
	short skill_level_limit;
	short skill_learn_level;
};

enum ADVANCE_COND_TYPE			// ������������
{
	ADVANCE_COND_TYPE_NO = 0,						// ������
	ADVANCE_COND_TYPE_MISSION = 1,					// �������				p1 : ����ID
	ADVANCE_COND_TYPE_XIN_LING_ZHI_JING = 2,		// ͨ������֮����X��		p1 : ��ħ�ؿ�seq(>=seq)
	ADVANCE_COND_TYPE_MENG_YUAN = 3,				// ͨ����Ԩ��X��			p1 : ����seq(>=seq)
	ADVANCE_COND_TYPE_GU_MO_ZHI_TA = 4,				// ͨ����ħ֮����X��		p1 : ����seq(>=seq)
	ADVANCE_COND_TYPE_LUN_HUI_WANG_CHUAN = 5,		// ͨ���ֻ�������X��		p1 : �ؿ�id(>=id)
	ADVANCE_COND_TYPE_SHAN_HAI_BOSS = 6,			// ����XX�Ѷ�ɽ������		p1 : �Ѷ�����(���Ѷ��������)
	ADVANCE_COND_TYPE_YUAN_ZHENG_BOSS = 7,			// ����Զ�����죺XXXX		p1 : ����seq p2:boss_index (��Ҫ����ָ���Ĺ���) 
	ADVANCE_COND_TYPE_DU_JIE = 8,					// ͨ��X�ؽ�				p1 : seq(>=seq)
	ADVANCE_COND_TYPE_WU_JIN_ZHI_TA = 9,			// ͨ���޾�֮��XX��		P1 : seq(>=seq)
	ADVANCE_COND_TYPE_SHANG_HAI_BOSS_SUCC = 10,		// սʤɽ��BOSS����		P1 : ����(>=cond_param1)
	ADVANCE_COND_TYPE_BIG_DIPPER_SUCC = 11,			// սʤ���ⱱ���Ǿ�����	P1 : ����(>=cond_param1)
	ADVANCE_COND_TYPE_NETHER_WORLD_PURGATORY = 12,	// ͨ����ڤ������X��Y��	P1 : X�½�	P2:�ؿ�ID (P1> COND_P1 OR P1== COND_P1 AND P2>= COND_P2) (����������Ҫ��ȫ�Ǹùؿ�,����Ҳ��)
	ADVANCE_COND_TYPE_YAO_XING_SHI_LIAN = 13,		// ͨ������������X��		P1 : �ؿ�seq(>=seq)
	ADVANCE_COND_TYPE_CHALLENGE_MONSTER_SUCC = 14,	// սʤ������			P1 : ������ID(==)
	ADVANCE_COND_TYPE_SHI_SHEN_ZHI_TA = 15,			// ͨ��߱��֮����X��		P1 : ����seq(>=seq)
	ADVANCE_COND_TYPE_MIRAGE_TERRITORY = 16,		// �����׾�����������X��	P1 : �ؿ�seq(>=num)
	ADVANCE_COND_TYPE_HONG_MENG_TIAN_JIE = 17,		// ͨ�غ������			P1 : ����ID(==����ID)

	ADVANCE_COND_TYPE_MAX,
};

struct CareerAdvanceCfg
{
	CareerAdvanceCfg() : profession_id(0), pre_profession(0), order(0), job_seq(0), is_broadcast(0), prof_base(0), need_level(0), pre_task(0), pre_title(0), pre_village(0), pre_relation(0), monster_group(0),
		pre_score(0), need_bind_coin(0), image(0), reward_title(0), max_equip_level(0), max_skill_level(0), all_sp_bonus(0), change_color_coin(0), change_item_id(0), change_item_num(0),
		juanzeng_up_limit(0), shengwangka_up_limit(0)
	{ 
		memset(init_skill_id, 0, sizeof(init_skill_id));
		memset(m_attrs, 0, sizeof(m_attrs));
	}

	int profession_id;				// ְҵID
	int pre_profession;				// ǰ��ְҵID
	int order;						// ����
	int job_seq;					// ְҵС��
	int is_broadcast;				// ����ʱ�Ƿ��� 0:������
	int prof_base;					// ְҵ����
	int need_level;					// ����ȼ�
	int pre_task;					// ǰ������ID
	int pre_title;					// ǰ�óƺ�ID
	int pre_village;				// ǰ�ô���ID
	int pre_relation;				// ǰ�ô��ӹ�ϵ		[û��ʹ����]
	int monster_group;				// �ؿ�������		[û��ʹ����]
	int cond_type;					// �������			[��Ӧö��ADVANCE_COND_TYPE]
	int cond_param1;				// �����������1
	int cond_param2;				// �����������2
	int pre_score;					// ǰ�ý�ɫ����
	int need_bind_coin;				// ����ͭ������
	int image;						// ��ѡ����
	int reward_title;				// �����ƺ�
	int max_equip_level;			// װ���ȼ�����
	int max_skill_level;			// ���ܵȼ�����

	int init_skill_num;				// ��ʼ��������
	int init_skill_id[PROFESSION_SKILL_INIT_MAX_NUM];						//��ʼ���ܵ��б�
	std::set<int> unlock_skill_set;

	JobName job_name;														//��ְҵ�׼��ƺ�
	Attribute m_attrs[BATTLE_ATTR_MAX];										//ְҵ����
	SuitName  suit_name;													//��װ�������

	int all_sp_bonus;								// ȫ����Ч�ӳ�  ��ЧID

	int change_color_coin;							// ��ɫͭ������
	int change_item_id;								// ��ɫ������������
	int change_item_num;							// ��ɫ������������

	int juanzeng_up_limit;							// װ����������
	int shengwangka_up_limit;						// ������ʹ������
};

struct ProfOtherCfg
{
	ProfOtherCfg() : change_career_cost_min(0), change_career_cost_max(0), change_cost_ratio_per_time(0), ver(0){}

	int change_career_cost_min;
	int change_career_cost_max;
	int change_cost_ratio_per_time;			// ÿ�κķ������ı���
	int ver;								// ְҵ��������汾��
};

struct CareerAdvanceGiveItemCfg
{
	CareerAdvanceGiveItemCfg()
	{
		role_level_min = 0;
		role_level_max = 0;
	}

	int role_level_min;
	int role_level_max;
	std::vector<ItemConfigData> change_prof_item;				//תְ����һ��װ��
};

struct SkillChangeCfg
{
	SkillChangeCfg() : max_level(0)
	{}

	struct SkillChangeLevelCfg
	{
		SkillChangeLevelCfg() : coin_consume_all(0), bind_coin_consume(0), add_exp(0)
		{}
		int coin_consume_all;				//����������ͭ��	�Ѿ�������֮ǰ�ĵȼ�����
		int bind_coin_consume;				//�����������Ľ��
		int add_exp;						//���ε����������
	};

	int max_level;
	SkillChangeLevelCfg skill_list[MAX_BATTLE_SKILL_LEVEL + 1];
};

struct ProfSkillLearnLevelCfg
{
	int skill_id;
	int learn_level;
};

class Role;
class ProfessionConfig : public ILogicConfig
{
public:
	static const int PROF_HASH_BASE = 100000;
	typedef std::map<int, ProfSkillCfg> ProfSkillCfgMap; //  key = base_prof * PROF_HASH_BASE + skill_id
	typedef std::map<int, std::vector <CareerAdvanceCfg> > CareerAdvanceCfgMap;
	typedef std::map<int, std::vector<ProfSkillLearnLevelCfg> > ProfSkillListMap;

public:
	ProfessionConfig();
	virtual ~ProfessionConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	
	bool CanLearnSkill(Role* role, UInt16 skill_id);
	int GetMaxLearnSkillLevel(Role* role, UInt16 skill);
	int GetSkillLeanLevel(Role* role, UInt16 skill);

	static int GetHashKey(int prof_base_type, UInt16 skill_id);
	void GetAllTitleByBaseProf(int prof_base, std::vector<int> & out_list);
	std::vector<int> GetUnlockSkillList(int prof_base, int role_level);
	std::vector<int> GetProfSkillList(int prof_base);
	std::set<int> GetProfSkillSet(int prof_base);

	const CareerAdvanceCfg* GetCareerAdvanceCfg(int profession , int _order);
	const std::vector<CareerAdvanceCfg> * GetCareerAdvanceCfgByCondType(int _cond_type);

	int GetCareerAdvanceStageSize(int _profession);
	const CareerAdvanceGiveItemCfg * GetCareerAdvanceGiveItemCfg(int base_prof, int role_level);
	const ProfOtherCfg& GetOtherCfg();
	const ProfSkillCfg* GetProfSkillCfgByHashKey(int hash_key);

	bool IsNeedReturn(int skill_id);
	int GetSkillChangeReturn(int skill_id, int skill_level, int skill_exp);

private:
	int InitProfSkillCfg(TiXmlElement *RootElement);
	int InitCareerAdvanceCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitChangeProfGiveCfg(TiXmlElement * RootElement);
	int InitSkillChangeCfg(TiXmlElement * RootElement);

	ProfSkillCfgMap m_prof_skill_map;
	CareerAdvanceCfgMap m_career_advance_cfg_map;
	CareerAdvanceCfgMap m_advance_cond_cfg_map;		// <cond_type , std::vector<cfg > >

	ProfOtherCfg m_other_cfg;
	std::map<int, std::vector<int> > m_prof_title_vec;		// ְҵ���׳ƺű��� - ְҵ��ֵID��ֵ - �ƺ�ID��

	std::map<int, std::vector<CareerAdvanceGiveItemCfg> > m_change_prof_give_item_cfg;
	std::map<int, SkillChangeCfg> m_skill_change_map;		// �����޸����� key:����ID

	ProfSkillListMap m_prof_skilllist_map;
};

#endif