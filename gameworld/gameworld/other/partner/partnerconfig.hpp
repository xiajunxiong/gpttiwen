#ifndef __PARTNER_CONFIG_HPP__
#define __PARTNER_CONFIG_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/partnerdef.h"
#include "item/itempool.h"

#include <vector>
#include <map>
#include <set>

static const int PARTNER_ELEMET_NUMBER_TOTAL = 10;		// ÿ������Ԫ�����������ܺ�
static const int PARTNER_ATTR_BKTH_ITEM_TYPE = 2;		// �������ͻ����Ҫ����������

// -----------------------------------------------------------------------------------
struct PartnerOtherCfg 
{
	PartnerOtherCfg() : fetter_wash_price(0), synfight_numb_max(0), quality_upgrade_limit(0), quality_segment_limit(0), upgrade_level_limit(0), soar_arms_limit(0)
	{
		reset_level_ret_rate = 0;
		reset_level_need_coint = 0;
	}

	int fetter_wash_price;		// ��Եϴ���۸� - �ɱ�
	int synfight_numb_max;		// ���ͬʱ��ս��
	int reset_level_ret_rate;
	int reset_level_need_coint;
	int quality_upgrade_limit;	//δ�������ɾ���Ʒ������
	int quality_segment_limit;	//δ�������ɾ��Ѷ�λ����
	int upgrade_level_limit;	//δ�������ɽ��׵ȼ�����
	int soar_arms_limit;		//ר�俪������Ʒ������
};

struct PartnerLevelCfg		// ����������
{
	PartnerLevelCfg()
	{
		comsume_money_type = 0;
		comsume_money_value = 0;
		memset(attr_list, 0, sizeof(attr_list));
		memset(resi_list, 0, sizeof(resi_list));
		memset(amen_list, 0, sizeof(amen_list));
	}

	int comsume_money_type;
	int comsume_money_value;
	Attribute attr_list[PARTNER_ATTR_MAX];		// ������ֵ
	Attribute resi_list[PARTNER_RESI_MAX];		// �ֿ���ֵ
	Attribute amen_list[PARTNER_AMEN_MAX];		// ������ֵ
};

struct PartnerBasicCfg		// ��������Ϣ����
{
	PartnerBasicCfg() : is_valid(false), sex(-1), is_long_range_unit(0), profession(0), race_type(0), fetter_group(0),
		unlock_task(0), activation_level(0), unlock_item(0), need_num(0) , unlock_item_norex(0), quality(FETTER_NONE), 
		crystal_item(0), soar_arms_id(0), soar_task_id(0)
	{
		unlock_item_xun_bao = 0;
		memset(name, 0, sizeof(GameName));
		memset(element, 0, sizeof(element)); 
		memset(skill_list, 0, sizeof(skill_list));
	}

	bool Invalid() const { return !is_valid; }
	
	bool is_valid;					// �����Ƿ���Ч
	char sex;						// �Ա�
	short is_long_range_unit;		// �Ƿ�Զ�̵�λ
	short profession;				// ���ְҵ
	short race_type;				// ��������
	short fetter_group;				// ��Ե��
	unsigned short unlock_task;		// ��������
	int activation_level;			// ����ʱ�ȼ�
	ItemID unlock_item;				// ��������
	UInt16 need_num;				// ��������
	ItemID unlock_item_norex;		// ������������
	ItemID unlock_item_xun_bao;		// Ѱ������

	short quality;					// Ʒ������
	GameName name;					// �������
	short element[PARTNER_ELE_MAX];	// ���Ԫ�� - ���ֻ���������Ԫ��

	ItemID crystal_item;			//����Դ���Ԫ��ˮ��id ȡ���˻�����е�Ԫ��
	UInt16 skill_list[PARTNER_COMMON_SKILL_MAX];

	int soar_arms_id;				// ��������ID
	int soar_task_id;				// ��������ID
};

struct PartnerUpGradeCfg	// ��������Ϣ����
{
	PartnerUpGradeCfg() : player_lv_need(0), talent_skill_id(0), talent_skill_lv(0), ji_ban_seq(0)
	{}	

	int player_lv_need;			// ��Ҫ��ҵȼ�
	int talent_skill_id;		// �츳����ID
	int talent_skill_lv;		// �츳���ܵȼ�
	int ji_ban_seq;
};

struct PartnerBreakThCfg		// ����ͻ������
{
	PartnerBreakThCfg() : seq(0), attr_type(0), add_value(0) {}

	short seq;				// ͻ��˳��

	short attr_type;		// ��������
	Attribute add_value;	// ���ӵ�����ֵ

	ItemPairCon need_item[PARTNER_ATTR_BKTH_ITEM_TYPE];
};

struct PartnerTalentCfg			// �츳��������
{
	PartnerTalentCfg() : effect_type(0), effect_target(0), param1(0), param2(0) {}

	short effect_type;		// Ч������
	short effect_target;	// ��ЧĿ��

	Attribute param1;		// ���� ID  | ��Ч���� |
	Attribute param2;		// ���Լ�ֵ | ��Ч���� |
};

struct PartnerFetterCfg			// ��Ե��������
{
	PartnerFetterCfg() : fetter_mark(0), need_partner(0)
	{
		memset(attr_add, 0, sizeof(attr_add));
		memset(resi_add, 0, sizeof(resi_add));
		memset(amen_add, 0, sizeof(amen_add));
	}

	short fetter_mark;		// ��Ե��־ - ��������������Ե֮���Ƿ�ͬһ����Ե
	short need_partner;		// ��������Ļ��

	Attribute attr_add[PARTNER_ATTR_MAX];	// ���Լ�ֵ
	Attribute resi_add[PARTNER_RESI_MAX];	// �ֿ���ֵ
	Attribute amen_add[PARTNER_AMEN_MAX];	// ������ֵ
};

struct FettersGroupsCfg			// ��Ե��������
{
	FettersGroupsCfg() : fetter_id(0), rand_rate(0) {}

	short fetter_id;		// ��Ե ID
	short rand_rate;		// ���Ȩ��
};

struct QualityUpgradeCfg					// ��Ʒ����
{
	enum KeyBaseNum   // key ��˻���
	{
		KEY_QUALITY_NUM = 100,
		KEY_BASE_NUM = 10000
	};
	QualityUpgradeCfg(): partner_id(0), quality_upgrade_type(0), quality_segment(0), level_limit(0), need_item(0), need_num(0), add_maxhp(0),
		add_maxmp(0), add_gongji(0), add_fangyu(0), add_minjie(0), add_jingshen(0), add_huifu(0)
	{
		memset(add_attr, 0, sizeof(add_attr));
		memset(common_skill_lv_list, 0, sizeof(common_skill_lv_list));
		memset(common_skill_id_list, 0, sizeof(common_skill_id_list));
	}

	int partner_id;							// ���id
	int quality_upgrade_type;				// ��Ʒ����
	int quality_segment;					// ����
	int level_limit;						// �ȼ�����
	ItemID need_item;						// ��Ʒ����
	unsigned short need_num;				// ��Ҫ��������
	unsigned int add_maxhp;					// ��������
	unsigned int add_maxmp;					// ����ħ��
	unsigned int add_gongji;				// ���ӹ���
	unsigned int add_fangyu;				// ���ӷ���
	unsigned int add_minjie;				// ��������
	unsigned int add_jingshen;				// ���Ӿ���
	unsigned int add_huifu;					// ���ӻظ�

	Attribute add_attr[PARTNER_ATTR_MAX];	// ��������ֵ������
	int common_skill_lv_list[PARTNER_COMMON_SKILL_MAX];
	int common_skill_id_list[PARTNER_COMMON_SKILL_MAX];			//��鼼���ɽ�����ϵ��ΪƷ����ϵ
};

struct PartnerLevelSkillCfg
{
	PartnerLevelSkillCfg() : gift_skill_id(0), gift_skill_level(0)
	{
	//	memset(skill_id_list, 0, sizeof(skill_id_list));
	//	memset(skill_level_list, 0, sizeof(skill_level_list));
	}

	//int skill_id_list[PARTNER_COMMON_SKILL_MAX];
	//int skill_level_list[PARTNER_COMMON_SKILL_MAX];
	int gift_skill_id;
	int gift_skill_level;
};

struct PartnerUpGradeRoleAttrCfg
{
	PartnerUpGradeRoleAttrCfg()
	{
		seq = 0;
		ji_ban_skill_id = 0;
		ji_ban_skill_level = 0;

		for (int i = 0; i < ARRAY_LENGTH(attrs); ++i)
		{
			attrs[i] = 0;
		}
	}

	int seq;
	int ji_ban_skill_id;
	int ji_ban_skill_level;
	Attribute attrs[BATTLE_ATTR_MAX];
};

struct PartnerBiographyCfg
{
	PartnerBiographyCfg(): need_upgrade(0), coin(0)
	{}

	int need_upgrade;
	int coin;
	std::vector<ItemConfigData> reward_list;
};

struct PartnerComposePageCfg
{
	PartnerComposePageCfg() : num(0), reward_group_id(0)
	{}

	int num;
	int reward_group_id;
};

struct PartnerComposeCfg
{
	PartnerComposeCfg() : rate(0)
	{}

	int rate;
	ItemConfigData item;
};

struct PartnerComposeGroupCfg
{
	PartnerComposeGroupCfg() : rate_count(0)
	{}

	int rate_count;
	std::vector<PartnerComposeCfg> node_list;
};

struct PartnerSoarCfg
{
	PartnerSoarCfg() : soar_need_level(0), soar_need_score(0), soar_need_quality_upgrade(0), soar_need_quality_segment(0),
		upgrade_level(0), soar_quality_upgrade(0), soar_quality_segment(0), attr_pellet_limit(0), arms_level(0)
	{}

	int soar_need_level;				//����������ȼ�
	int soar_need_score;				//��������������
	int soar_need_quality_upgrade;		//����������Ʒ��
	int soar_need_quality_segment;		//�������������
	int upgrade_level;					//������ɽ���
	int soar_quality_upgrade;			//������ɾ���Ʒ��
	int soar_quality_segment;			//������ɾ��Ѷ���
	int attr_pellet_limit;				//���������Ե�����
	int arms_level;						//���������������ǿ������
};

static const int MAX_PARTNER_SOAR_ARMS_BASE_NUM = 10000;
struct PartnerSoarArmsCfg
{
	PartnerSoarArmsCfg() : arms_skill_id(0), arms_skill_level(0)
	{
		memset(attr_list, 0, sizeof(attr_list));
	}

	
	int arms_skill_id;
	int arms_skill_level;
	std::vector<ItemConfigData> break_consume_list;
	Attribute attr_list[BATTLE_ATTR_MAX];
};

struct PartnerIntensifyCfg
{
	struct AttrData
	{
		int type;
		int param;
	};

	PartnerIntensifyCfg():levels(0), feisheng_num(0), use_coin(0), expend_item_1(0), expend_num_1(0), expend_num_2(0),
		expend_item_3(0), expend_num_3(0) {}

	short levels;
	short feisheng_num;

	int use_coin;

	ItemID expend_item_1;
	short expend_num_1;

	short expend_num_2;

	ItemID expend_item_3;
	short expend_num_3;

	std::vector<AttrData> attr_list;
	bool is_broadcast;
};

// -----------------------------------------------------------------------------------
class PartnerConfig : public ILogicConfig
{
public:
	struct PartnerCfgHead		// �������ӳ�� ��ֵ�ṹ��
	{
		PartnerCfgHead(int p_id = 0, int p_lv = 0)
		{
			this->partner_id = p_id;	// ��� ID | �츳 ID
			this->partner_lv = p_lv;	// ���׼� | �츳�ȼ� | ����ID
		}

		int partner_id;
		int partner_lv;

		bool operator < (const PartnerCfgHead & cobj) const
		{
			if (this->partner_id < cobj.partner_id) return true;
			else if (this->partner_id == cobj.partner_id &&
				this->partner_lv < cobj.partner_lv) return true;

			return false;
		}

		bool operator == (const PartnerCfgHead & cobj) const
		{
			return (this->partner_id == cobj.partner_id && this->partner_lv == cobj.partner_lv);
		}
	};

	typedef std::map<PartnerCfgHead, PartnerLevelSkillCfg> LevelSkillCfgMap;

	//--��Լ�ֿ�IDmap <��Ʒid, ����λ��>
	typedef std::map<ItemID, int> PactItemIDMap;
	typedef std::map<int, ItemID> PactSaveIndexMap;

public:
	PartnerConfig();
	virtual ~PartnerConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	inline int GetPartnerCount() const { return m_partner_num; }
	int GainARandFetter(int partner_id, int f_group_id, int exclude_mark = 0);

	int PartnerActiveCheck(int check_type, int param);
	int GetPartnerQualityByItemId(int item_id);
	const PartnerOtherCfg & GetPartnerOtherCfg() { return p_other; }
	const PartnerLevelCfg * GetPartnerLevelCfg(int partner, int level);
	const PartnerBasicCfg * GetPartnerBasicCfg(int partner_id);
	const PartnerUpGradeCfg * GetCurGradeAttrCfg(int partner_id, int grade_lv);
	const PartnerBreakThCfg * GetCurAttrBreakCfg(int partner_id, int grade_lv, int brk_seq);
	const PartnerTalentCfg * GetPTalentSkillCfg(int talent_id, int talent_lv);
	const PartnerFetterCfg * GetPartnerFetterCfg(int fetter_id);
	const PartnerUpGradeRoleAttrCfg * GetPartnerUpGradeRoleAttrCfg(int ji_ban_seq);
	std::vector<int> GetAllPartnerId();
	const PartnerBiographyCfg * GetPartnerBiographyCfg(int paratner_id, int biography);

	//��[���id]��[�����Ʒ����]Ψһȷ����Ʒ�ӳ���ֵ
	int ___QuaKey__(int _partner_id, int _quality_up_type = QUALITY_TYPE_BLUE_TO_PURPLE, int quality_segment = 1);
	const QualityUpgradeCfg* GetParnterQualityCfg(int _getcfg_key);     //�������ͱ���Ϊ  ___QuaKey__����
	int GetCanQualityUpType(int _parttner_quality);
	const PartnerLevelSkillCfg* GetLevelSkillCfg(int partner_id, int parner_level);

	const PartnerComposePageCfg * GetComposePageCfg(int item_id);
	const PartnerComposeCfg * GetRandComposeRewardCfg(int reward_group_id);

	//���--Ԫ����Լ
	int GetPactSaveIndex(ItemID item_id) const;
	ItemID GetPactItemID(int save_index) const;

	const PartnerSoarCfg * GetPartnerSoarCfg(int soar_frequency);
	const PartnerSoarArmsCfg * GetPartnerSoarArmsCfg(int arms_id, int arms_level);

	//���ǿ��
	const PartnerIntensifyCfg * GetPartnerIntensifyCfg(int partner_id, int star_level);

private:
	int InitPartnerOtherCfg(TiXmlElement *RootElement);
	int InitPartnerBasicCfg(TiXmlElement *RootElement);
	int InitSkillsUnlockCfg(TiXmlElement *RootElement);
	int InitPartnerUpGradeCfg(TiXmlElement *RootElement);
	int InitPartnerBreakThCfg(TiXmlElement *RootElement);
	int InitPartnerAttribute(TiXmlElement *RootElement);
	int InitPartnerTalentCfg(TiXmlElement *RootElement);
	int InitPartnerFetterCfg(TiXmlElement *RootElement);
	int InitFettersGroupsCfg(TiXmlElement *RootElement);
	int InitPartnerQualityUpCfg(TiXmlElement *RootElement);
	int InitPartnerUpGradeRoleAttrCfg(TiXmlElement * RootElement);
	int InitPartnerBiographyCfg(TiXmlElement * RootElement);
	int InitPartnerComposeGroupCfg(TiXmlElement * RootElement);
	int InitPartnerComposePageCfg(TiXmlElement * RootElement);
	int InitPactSaveIndexCfg(TiXmlElement *RootElement);
	int InitPartnerSoarCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitPartnerSoarArmsCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitPartnerIntensifyCfg(TiXmlElement *RootElement, std::string& errormsg);

	int GetSoarArmsKey(int arms_id, int arms_level);
	int CheckFetterGroupsCfg();
	int InitLevelSkillCfgMap();
	PartnerBasicCfg * MutablePartnerBasicCfg(int partner_id);

	PartnerOtherCfg p_other;

	int m_partner_num;
	PartnerBasicCfg m_basic_cfg_list[PARTNER_NUM_MAX];	 // ��������Ϣ�� �±����partner_id		
	PartnerLevelCfg m_lv_attr_cfg_list[PARTNER_NUM_MAX][MAX_ROLE_LEVEL + 1];		// ���������Ա�
	int max_upgrade_level;
	std::map<PartnerCfgHead, PartnerUpGradeCfg> m_s_upgrade;				// �ȼ����ױ�
	std::map<PartnerCfgHead, std::vector<PartnerBreakThCfg> > m_v_breakth;	// ����ͻ�Ʊ�
	std::map<PartnerCfgHead, PartnerTalentCfg> m_s_talents;					// �츳�� - ��ֵ���츳��������
	std::map<PartnerCfgHead, PartnerBiographyCfg> m_biography_map;			// �������� key.partner_lv < BIT_COUNT_OF_BYTE

	std::map<int, PartnerFetterCfg> m_s_fetters;							// ��Ե�� - ��ֵ����Ե���� ID
	std::map<int, std::vector<FettersGroupsCfg> > m_v_f_group;				// ��Ե����� - ��ֵ�Ƿ��� ID

	//��Ʒ
	std::map<int, QualityUpgradeCfg> m_idqua_to_upg;				//KEY = [partner_id] * 10000 +[quality_upgrade] * 100 + quality_segment  -->   ������Ϣ
	
	LevelSkillCfgMap m_level_skill_cfg_map;

	std::map<int, int> m_basic_cfg_map;							//key:��������ID second:���Ʒ��

	std::map<int, PartnerUpGradeRoleAttrCfg> m_upgrade_role_attr_map;	

	std::map<int, PartnerComposePageCfg> m_partner_compose_page_cfg;			//����ҳ�ϳ� key:����ID
	std::map<int, PartnerComposeGroupCfg> m_partner_compose_cfg;				//���ϳ������� key :reward_group_id

	//--��Լ�ֿ�IDmap <��Ʒid, ����λ��>
	PactItemIDMap m_pact_item_id_map;
	PactSaveIndexMap m_pact_save_index_map;

	std::map<int, PartnerSoarCfg> m_partner_soar_cfg;				// �������� key:soar_frequency
	std::map<int, PartnerSoarArmsCfg> m_partner_soar_arms_cfg;		// ������������ key:arms_id * MAX_PARTNER_SOAR_ARMS_BASE_NUM + arms_level

	//���ǿ��
	std::map<int, std::vector<PartnerIntensifyCfg> > m_partner_intensify_cfg;
};

#endif // __PARTNER_CONFIG_HPP__