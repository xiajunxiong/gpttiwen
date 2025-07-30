#ifndef __HOLY_EQUIPMENT_CONFIG_HPP__
#define __HOLY_EQUIPMENT_CONFIG_HPP__

#include <map>
#include <vector>
#include <string>

#include "item/itempool.h"
#include "gamelog.h"
#include "item/shengqi/runestoneitem.h"
#include "config/otherconfig/scoresystemconfig.hpp"

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/medaldef.h"
#include "item/medalitem/medalitem.hpp"
#include "obj/character/role.h"

static const int MAX_ROLE_ATTR_NUM = 12;	// ���ÿɻ�ó��������������
static const int MAX_HOLY_ATTR_NUM = 7;		// ���ÿɻ��ʥ�������������

struct HolyAttrItem
{
	HolyAttrItem() :attr_type(0), attr_min(0), attr_max(0)
	{}

	int attr_type;
	int attr_min;
	int attr_max;
};

struct HolyEquipBaseCfg
{
	HolyEquipBaseCfg():shengqi_id(0), item_id(0),shengqi_type(0),shengqi_quality(0),fenjie_exp(0)
	{}

	int shengqi_id;			// ����id
	ItemID item_id;			// ��Ʒid
	int shengqi_type;		// ����
	int shengqi_quality;	// Ʒ��
	int fenjie_exp;			// �ֽ⾭��
	std::vector<int> attr_type_list;		// ���������б�
	HolyAttrItem attrs[MAX_HOLY_ATTR_NUM];	// �����������ֵ��
};

struct HolyEquipTalentCfg
{
	HolyEquipTalentCfg():skill_id(0), skill_class(0), skill_type(0),appear_type(0)
	{}
	static const int MAX_HOLY_EQUIP_TALENT_COUNT = 2;

	enum SKILL_TYPE
	{
		SKILL_TYPE_COMMON = 1,		// ��ͨ�츳
		SKILL_TYPE_STRONGER = 2,	// �߼��츳
		SKILL_TYPE_SUPER = 3,		// �����츳
	};

	int skill_id;		// ����id
	int skill_class;	// ������,һ��ʥ����������������ͬ������ļ���
	int skill_type;		// ��������,�츳�ȼ�,SKILL_TYPE
	int appear_type;	// �����ڵ�ʥ������,HOLY_EQUIP_TYPE
};

struct HolyEquipInitiativeSkillCfg
{
	HolyEquipInitiativeSkillCfg():combo_level(0),skill_type(0)
	{}

	int combo_level;	// �ϼ��ȼ�,(���ʾ��ǲ�ͬ�ļ���id)
	int skill_type;		// ��������
	std::pair<int, int> skill_pair;		// value1--��ͨ����id, value2--�ó�����id
	std::set<int> shanchang_shengqi;	// �ó�ʥ��id
};

struct HolyEquipInitiativeSkillHoleCfg	// �������ܲۿ�������
{
	HolyEquipInitiativeSkillHoleCfg():unlock_cost(0),num(0)
	{}

	int unlock_cost;	// ������Ҫ����id
	int num;			// ������Ҫ��������
	int shengqi_quality;	// ��Ҫʥ��Ʒ��
};

struct HolyEquipLevelUpAttrCfg
{
	HolyEquipLevelUpAttrCfg():upgrade_exp(0),coin_cost(0), active_max(0)
	{}
	
	int upgrade_exp;	// ���辭��
	int coin_cost;		// ����ͭ��
	int active_max;		// ��Ծ������
	AttrInfo attr_list[BATTLE_ATTR_MAX];	//�����б� ,����ʥ���������û�ȡ��������, ʥ���ȼ����þ�����õ�����
};

struct HolyEquipLevelUpCfg
{
	HolyEquipLevelUpCfg():shengqi_quality(0)
	{}

	int shengqi_quality;	// ʥ��Ʒ��
	std::map<int, HolyEquipLevelUpAttrCfg> level_cfg;	// �ȼ�����,key-ʥ���ȼ�
};

struct RuneStoneBaseCfg
{
	RuneStoneBaseCfg():item_id(0), fushi_level(0)
	{}

	ItemID item_id;
	int fushi_level;
	HolyAttrItem attrs[MAX_HOLY_ATTR_NUM];
	std::map<int, AttrInfo> role_attr_list;	 // ����ְҵ��ȡ��ͬ������, key-����ְҵ����PROF_TYPE
};

struct RuneStoneCfg
{
	RuneStoneCfg():fushi_type(0)
	{}

	int fushi_type;		// ��ʯ����
	std::vector<RuneStoneBaseCfg> fushi_attr;	// ��ʯ�ȼ�����
};

struct RuneStoneHoleCfg
{
	RuneStoneHoleCfg():shengqi_level(0),unlock_cost(0),consume_num(0)
	{}

	int shengqi_level;		// ����ʥ���ȼ�
	ItemID unlock_cost;		// ��������
	int consume_num;		// ��������
};

struct RuneStoneFusingCfg
{
	RuneStoneFusingCfg():need_item_num(0),cost_coin(0)
	{}

	int need_item_num;	// ��ʯ�ϳ���Ҫ���ĵķ�ʯ����
	int cost_coin;		// ͭ������
};

struct RuneStoneRecastCfg
{
	RuneStoneRecastCfg() :need_item_num(0), cost_coin(0)
	{}

	int need_item_num;	// ��ʯ������Ҫ���ĵķ�ʯ����
	int cost_coin;		// ͭ������
};

struct HolyEquipOtherCfg
{
	HolyEquipOtherCfg():feisheng_type(0), exp_back(0)
	{}
	static const int HOLY_EQUIP_EXP_BACK_PROPORTION = 1000;

	int feisheng_type;		// ��ɫ��Ҫ�����׶�,�ﵽ�ý׶β��ܿ���ϵͳ
	int exp_back;			// ��ʥ����Ϊ����ʱ�ľ��鷵������, ǧ�ֱ�
	ItemConfigData baoxiang1;	// �߼�ʥ������
	ItemConfigData baoxiang2;	// ����ʥ������
};


// -------------------------------------------------------------------------------------
class HolyEquipMentConfig :public ILogicConfig
{
public:
	HolyEquipMentConfig();
	virtual ~HolyEquipMentConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const HolyEquipBaseCfg* GetBaseCfgById(ItemID item_id)const;
	const HolyEquipLevelUpAttrCfg* GetHolyLevelCfg(int shengqi_quality, int level)const;
	int GetLevelBySkillId(int skill_id)const;
	int GetShengQiIdByItemId(ItemID item_id)const;
	int GetSkillIdByShanChangId(int shanchang_id)const;		// return 0,�鲻��

	bool RandHolyItemAttr(HolyEquipParam & data, ItemID item_id, int shengqi_quality, int shengqi_type, std::string& error);
	int RandHolyItemTaLent(HolyEquipParam& data,int shengqi_quality, int shengqi_type);
	int RandHolyItemSkill(const HolyEquipParam& data, ItemID item_id, int shengqi_type, int skill_quality, int skill_hole);
	bool RandRuneStoneAttr(Role* role, RuneStoneParam& data, int runestone_type, ItemID item_id);
	const HolyEquipOtherCfg& GetOtherCfg()const { return m_other_cfg; }

	const HolyEquipInitiativeSkillHoleCfg* GetSkillHoleCfg(int hole)const;
	const RuneStoneHoleCfg* GetFuShiHoleCfg(int fushi_hole)const;
	const RuneStoneFusingCfg* GetFuShiFusingCfg(int fushi_level);
	const RuneStoneRecastCfg* GetFuShiRecastCfg(int fushi_level);
	const ItemID* GetFuShiIdByTypeLevelCfg(int fushi_type, int fushi_level);
	int GetItemExpCfg(ItemID item_id)const;		// ��ȡʥ�������ɻ�õ�exp
	int GetHolyEquipExp(ItemID item_id)const;	// ��ȡʥ������ķֽ⾭��
	int GetSkillScoreCfg(int skill_id)const;
	int GetSkillIdByTypeAndItem(int shengqi_type, int lingwu_item)const;
	bool GetSkillLevel(int skill_id, ARG_OUT int* out_skill_id, ARG_OUT int* out_skill_lv) const;

	const AttrInfo* GetAttrBySkill(int skill_id)const;	// ������������id��ȡ����������
	const AttrInfo* GetRoleAttr(Role* role, ItemID fushi_id, int fushi_type)const;		// ���ݷ�ʯid,��ʯ���ͺ�ְҵ��ȡ��������
private:
	int InitBaseCfg(TiXmlElement * RootElement, std::string& errormsg);				// ʥ�����ͷ�ҳ
	int InitTaLentCfg(TiXmlElement * RootElement, std::string& errormsg);			// �츳���ܷ�ҳ
	int InitSkillCfg(TiXmlElement * RootElement, std::string& errormsg);			// �������ܷ�ҳ
	int InitSkillStudyCfg(TiXmlElement * RootElement, std::string& errormsg);		// ���������ҳ
	int InitSkillHoleCfg(TiXmlElement * RootElement, std::string& errormsg);		// ���ܿ׿�����ҳ
	int InitHolyLevelUpCfg(TiXmlElement * RootElement, std::string& errormsg);		// ʥ��������ҳ
	int InitLevelUpItemCfg(TiXmlElement * RootElement, std::string& errormsg);		// ʥ�����鵤��ҳ
	int InitRuneStoneCfg(TiXmlElement * RootElement, std::string& errormsg);		// ʥ����ʯ��ҳ
	int InitRuneStoneHoleCfg(TiXmlElement * RootElement, std::string& errormsg);	// ��ʯ�׷�ҳ
	int InitRuneStoneFusingCfg(TiXmlElement * RootElement, std::string& errormsg);	// ��ʯ�ϳɷ�ҳ
	int InitRuneStoneRecastCfg(TiXmlElement * RootElement, std::string& errormsg);	// ��ʯ������ҳ
	int InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg);			// ������ҳ


	std::vector<HolyEquipBaseCfg> m_shengqi_base_cfg;		//!< ʥ����������

	std::vector<HolyEquipTalentCfg> m_talent_cfg;			//!< �츳����

	std::vector<HolyEquipInitiativeSkillCfg> m_initiative_skill_cfg;	//!< ��������
	std::map<int, std::map<ItemID, int> > m_skill_group_cfg;	//!< ������������ key1-ʥ������,key2-���ĵ���id
//	std::map<int, ItemID> m_skill_material_cfg;				//!< ʥ��Ҫ��������,key-��������,value-��ӦҪ���ĵĵ���id
	std::map<int, HolyEquipInitiativeSkillHoleCfg> m_skill_hole_cfg;	//!< �������ܿ�λ����,key-��λ,0-4
	std::map<int, int> m_skill_find_level_cfg;				//!< ���ݼ���id���Ҽ��ܵȼ�,key-��������id, value-���ܵȼ�
	std::map<int, int> m_skill_shanchang_find_cfg;			//!< �����ó�����id������ͨ����id,key-�ó�����id,value-[��ͨ����id	

	std::vector<HolyEquipLevelUpCfg> m_level_up_cfg;		//!< ʥ���ȼ�����
	std::map<ItemID, int> m_upgrade_consume_cfg;			//!< ʥ�����������ṩ�ľ���,key-��Ʒid,value-�ṩ�ľ���

	std::vector<RuneStoneCfg> m_rune_stone_cfg;				//!< ��ʯ����
	std::map<int, RuneStoneHoleCfg> m_runestone_hole_cfg;	//!< ��ʯ��λ��������,key-��λ

	std::map<int, RuneStoneFusingCfg> m_fushi_fusing_cfg;	//!< ��ʯ�ϳ�����, key-��ʯ�ȼ�
	std::map<int, RuneStoneRecastCfg> m_fushi_recast_cfg;	//!< ��ʯ��������, key-��ʯ�ȼ�

	std::map<int, int> m_skill_score_map_cfg;				//!< ����id���ֶ��ձ�, key-����id/�츳id,value-����
	std::map<int, AttrInfo> m_skill_attr_map_cfg;			//!< ����id���Զ��ձ�, key-����id/�츳id,value-���ӵ�ʥ������

	HolyEquipOtherCfg m_other_cfg;
};

#endif