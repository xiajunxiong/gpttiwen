#ifndef __HUAN_SHOU_CONFIG_HPP__
#define __HUAN_SHOU_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/huanshoudef.hpp"
#include "servercommon/attributedef.hpp"
#include <set>

struct HuanShouOtherCfg
{
	HuanShouOtherCfg() : potential_item_id(0), exchange_price(0), wait_time(0), liandan_max(0), lock_item_id(0)
	{
		memset(need_skill_num_list, 0, sizeof(need_skill_num_list));
		memset(need_item_num_list, 0, sizeof(need_item_num_list));
	}

	int potential_item_id;		//Ǳ�ܹ�����ID
	int exchange_price;			//��¯��������
	int wait_time;				//��������ȴ�ʱ��
	int liandan_max;			//����¯�������
	int lock_item_id;			//���������������ĵ���ID
	int need_skill_num_list[2];	//�����������ѽ���������
	int need_item_num_list[2];	//����λ�������ĵ�����
};

struct HuanShouBaseCfg
{
	HuanShouBaseCfg() : quality(0), convert_item_id(0), attr_group_id(0), potential_group_id(0), skill_lattice(0)
	{}

	int quality;
	int convert_item_id;		//�Ѽ������Զ�ת��Ϊ�õ���
	int attr_group_id;			//������ID
	int potential_group_id;		//Ǳ����ID
	int skill_lattice;			//���鼼������
	std::vector<ItemConfigData> fenjie_item_list;		//�ֽ�ת�����߿ɵ�
};

struct HuanShouAttrGroupLevelCfg
{
	HuanShouAttrGroupLevelCfg() : level(0), skill_id(0), skill_level(0), cd_start(0), cd_perform(0), attr_count(0)
	{
		memset(attr_list, 0, sizeof(attr_list));
	}

	static const int MAX_ATTR_NUM = 2;

	int level;
	int skill_id;
	int skill_level;
	int cd_start;
	int cd_perform;
	int attr_count;
	AttrItemCfg attr_list[MAX_ATTR_NUM];
	std::vector<ItemConfigData> consume_list;
};

struct HuanShouAttrGroupCfg
{
	HuanShouAttrGroupCfg() : max_str_level(0)
	{}

	int max_str_level;
	HuanShouAttrGroupLevelCfg level_list[NAX_HUAN_SHOU_STR_LEVEL_NUM];
};

static const int MAX_HUAN_SHOU_EGG_RATE_NUM = 10000;		//���޵��̶�Ȩ���ܺ�
struct HuanShouEggCfg
{
	HuanShouEggCfg() : huanshou_id(0), rate(0), is_broadcast(false)
	{}

	int huanshou_id;
	int rate;
	bool is_broadcast;
	bool is_rare;			// �Ƿ�ϡ��
};

struct HuanShouEggOpenCfg
{
	HuanShouEggOpenCfg() : egg_id(0), item_id(0), rare_guarantee(0)
	{}

	int egg_id;				//���޵�ID
	int item_id;			//���޵���Ӧ����ID
	int rare_guarantee;		//ϡ�б��״���
};

struct HuanShouPointUnlockCfg
{
	HuanShouPointUnlockCfg() : need_active_level(0)
	{}

	int need_active_level;				//�����������ڵ�λ�ȼ�
	std::set<int> adjoin_list;			//���ڵ�λ,�ж��Ƿ�ɼ���ʱ,ֻ�迴�������Ƿ��������ȼ��ĵ�λ
};

enum HUAN_SHOU_POINT_TIAN_FU_TYPE
{
	HUAN_SHOU_POINT_TIAN_FU_TYPE_ATTR = 0,				//���� p1:�������� p2:����ֵ
	HUAN_SHOU_POINT_TIAN_FU_TYPE_PASSIVE_SKILL = 1,		//�������� p1:����ID p2:���ܵȼ�

	HUAN_SHOU_POINT_TIAN_FU_TYPE_MAX
};

struct HuanShouPotentialPointLevelCfg
{
	HuanShouPotentialPointLevelCfg() : level(0), tianfu_type(0), param1(0), param2(0)
	{}

	int level;
	int tianfu_type;
	int param1;
	int param2;
	std::vector<ItemConfigData> consume_list;
};

struct HuanShouPotentialPointGroupCfg
{
	HuanShouPotentialPointGroupCfg()
	{
		memset(max_level_list, 0, sizeof(max_level_list));
	}

	int max_level_list[MAX_HUAN_SHOU_POTENTIAL_POINT];		//Ǳ�ܵ�λ���ȼ��б�
	HuanShouPotentialPointLevelCfg point_list[MAX_HUAN_SHOU_POTENTIAL_POINT][MAX_HUAN_SHOU_POINT_LEVEL_NUM];
};

class HuanShouConfig : public ILogicConfig
{
public:
	HuanShouConfig();
	virtual ~HuanShouConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const HuanShouOtherCfg & GetOtherCfg() { return m_other_cfg; }
	int GetBaseCount() const { return m_base_count; }
	const HuanShouBaseCfg * GetBaseCfg(int huanshou_id) const;
	const std::vector<ItemConfigData> * GetDecomposeCfg(int item_id) const;
	const HuanShouAttrGroupCfg * GetAttrGroupCfg(int attr_group_id) const;
	const HuanShouAttrGroupLevelCfg * GetAttrGroupLevelCfg(int attr_group_id, int str_level) const;
	const HuanShouAttrGroupLevelCfg * GetAttrGroupLevelCfgByLevel(int attr_group_id, int str_level) const;
	int GetEggCount() const { return m_huan_shou_egg_count; }
	const HuanShouEggOpenCfg * GetHuanShouEggOpenCfg(int item_id) const;
	const HuanShouEggCfg * GetRandHuanShouEggCfgByEgg(int egg_id) const;
	const HuanShouEggCfg * GetRandRareHuanShouEggCfgByEgg(int egg_id) const;
	bool IsCanAddNewSkill(int has_hole_count)const;

	int GetPointCount() const { return m_point_count; }
	const HuanShouPointUnlockCfg * GetPointUnlockCfg(int point_index) const;
	const HuanShouPotentialPointLevelCfg * GetPointLevelCfg(int point_group_id, int point_index, int level)const;

private:
	int InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitBaseCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitAttrGroupCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitEggItemCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitEggCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitSkillHoleCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitPointUnlcokCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitPointGroupCfg(TiXmlElement * RootElement, std::string& errormsg);


	HuanShouOtherCfg m_other_cfg;
	int m_base_count;											//���޻��������б�����
	HuanShouBaseCfg m_base_cfg_list[MAX_HUAN_SHOU_NUM];			//���޻�������
	std::map<int, std::vector<ItemConfigData> > m_decompose_map;//���޷ֽ����� key:item_id
	std::map<int, HuanShouAttrGroupCfg> m_attr_group_map;		//������������(ǿ��) key:attr_group_id

	int m_huan_shou_egg_count;									//�б�����
	HuanShouEggOpenCfg m_huan_shou_egg_item_list[MAX_HUAN_SHOU_EGG_ID_NUM];		//���޵����߿�������
	std::vector<HuanShouEggCfg> m_huan_shou_egg_list[MAX_HUAN_SHOU_EGG_ID_NUM];	//���޵�����

	std::map<int, int> m_skill_hole_map;						//���ܿ�λ����

	int m_point_count;	//��Ǳ�ܵ�λ����ͬʱҲ��Ǳ�ܵ�λ���е�λ����
	HuanShouPointUnlockCfg m_point_unlock_list[MAX_HUAN_SHOU_POTENTIAL_POINT];	//����Ǳ�ܵ�λ��������

	int m_ppg_count;
	HuanShouPotentialPointGroupCfg m_ppg_list[MAX_HUAN_SHOU_POTENTIAL_POINT_GROUP_NUM];	//Ǳ�ܵ�λ������
};

#endif	//__HUAN_SHOU_CONFIG_HPP__