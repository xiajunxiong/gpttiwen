#ifndef __ZAO_HUA_EQUIP_CONFIG_HPP__
#define __ZAO_HUA_EQUIP_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "item/zaohua/zaohuaitem.hpp"

#include <map>
#include <vector>


struct ZaoHuaVigorousCfg
{
	ZaoHuaVigorousCfg() :seq(0), zaohua_equip_suit(0), level(0) {}

	int seq;
	int zaohua_equip_suit;
	int	level;
};

struct ZaoHuaSuitCfg
{
	const static int MAX_SUIT_VOC_NUM = 3;//��װ�����������
	ZaoHuaSuitCfg() : zaohua_equip_suit(0), level(0), color(0), limit_prof(0), suit_num(0), sp_id(0), score(0)
	{
		for (int i = 0; i < MAX_SUIT_VOC_NUM; i++)
		{
			attr_type_list[i].attr_type = 0;
			attr_type_list[i].attr_value = 0;
		}
		memset(potential_add, 0, sizeof(potential_add));
	}
	enum ZAO_HUA_POTENTIAL_TYPE
	{
		ZAO_HUA_POTENTIAL_TYPE_0 = 0,//����
		ZAO_HUA_POTENTIAL_TYPE_1 = 1,//����
		ZAO_HUA_POTENTIAL_TYPE_2 = 2,//����
		ZAO_HUA_POTENTIAL_TYPE_3 = 3,//����
		ZAO_HUA_POTENTIAL_TYPE_4 = 4,//�ٶ�
		ZAO_HUA_POTENTIAL_TYPE_5 = 5,//����
		ZAO_HUA_POTENTIAL_TYPE_6 = 6,//�ظ�
		ZAO_HUA_POTENTIAL_TYPE_MAX,
	};
	int zaohua_equip_suit;
	int	level;
	int	color;
	int	limit_prof;
	int	suit_num;
	AttrInfo attr_type_list[MAX_SUIT_VOC_NUM];
	int	sp_id;
	int	score;
	int potential_add[ZAO_HUA_POTENTIAL_TYPE_MAX];
};

const static int PER_ZAO_HUA_KEY_SUIT_TYPE = 100;// �컯�����װ����
const static int PER_ZAO_HUA_KEY_COLOR = PER_ZAO_HUA_KEY_SUIT_TYPE * 100;// �컯���Ʒ��
const static int PER_ZAO_HUA_KEY_LEVEL = PER_ZAO_HUA_KEY_COLOR * 1000;// �컯��ǵȼ�
const static int PER_ZAO_HUA_KEY_SUIT_NUM = PER_ZAO_HUA_KEY_LEVEL * 10;// �컯��Ǽ���

struct ZaoHuaComposeCfg
{
	ZaoHuaComposeCfg() : seq(0), target(0), stuff_id_1(0), stuff_num_1(0), stuff_id_2(0), stuff_num_2(0), price(0) {}
	int seq;
	int	target;
	int	stuff_id_1;
	int	stuff_num_1;
	int	stuff_id_2;
	int	stuff_num_2;
	int	price;
};

struct ZaoHuaEquipOtherCfg
{
	int hun_li_max_value;//�������ֵ
	int hun_li_add_value; //ÿ�������������ӵ�ֵ
	int ling_xing_extra_hun_li_add_value; //ÿ�������������Ի����������ӵ�ֵ
	int hun_li_uplv_comsume_item_id;
	int hun_li_uplv_comsume_item_num;
	int long_zhu_draw_comsume_item_id; //�����齱���ĵ���id

	int ru_mo_comsume_item_id; //��ħ����id
	int ru_mo_comsume_item_num; //��ħ������Ʒ����
};

//Ԫ��������
struct YuanHunZhuCfg
{
	int type;
	int item_id;
	int next_item_id;
	int yuan_hun_zhu_level;
	int up_level_need_money_value;

	int slot_attr_rand_group_id[YUAN_HUN_ZHU_MAX_ATTR_NUM];  //Ԫ������ÿ�������������id
};

struct YuanHunZhuAttrGroupItemCfg
{
	int rate;
	int seq;
	AttrInfo attr;
};

struct YuanHunZhuAttrGroupCfg
{
	YuanHunZhuAttrGroupCfg()
	{
		rate_count = 0;
	}

	int group_id;

	int rate_count;
	std::map<int, YuanHunZhuAttrGroupItemCfg> item_list;
};

struct YuanHunZhuDecomposeCfg
{	
	int item_id;

	std::vector<ItemConfigData> decompose_get_item;
	int decompose_get_money_type;
	int decompose_get_money_value;
};

struct YuanHunZhuComposeCfg
{
	int item_id;
	int yuan_hun_zhu_compose_need_item_id;
	int yuan_hun_zhu_compose_need_item_num;
};

struct YuanHunZhuCombinationAttrCfgKey
{
	bool operator<(const YuanHunZhuCombinationAttrCfgKey & other)const
	{
		if (base_prof < other.base_prof)
			return true;
		if (base_prof > other.base_prof)
			return false;

		if (combination_id < other.combination_id)
			return true;
		if (combination_id > other.combination_id)
			return false;

		if (combination_level < other.combination_level)
			return true;
		if (combination_level > other.combination_level)
			return false;


		return false;
	}

	int base_prof;
	int combination_id;
	int combination_level;
};

struct YuanHunZhuCombinationAttrCfg
{
	int base_prof;
	int combination_id;
	int combination_level;

	int sp_id;
	std::vector<AttrItemCfg> attr_cfg;					//��ϵĹ̶�����
};


struct YuanHunZhuCombinationCfg
{
	int combination_group_id;
	std::map<int, int> yuan_hun_zhu_color_need_num;  //ÿ����ɫ��������Ҫ����
};

//����
struct ZaoHuaEquipLingXingValueItemCfg
{
	int rate;
	int ling_xing_value;
};

struct ZaoHuaEquipLingXingValueCfg
{
	ZaoHuaEquipLingXingValueCfg()
	{
		rate_count = 0;
	}

	int rate_count;
	std::vector<ZaoHuaEquipLingXingValueItemCfg> item_list;
};

//����
struct ZaoHuaEquipHunLiAttrCfgKey
{
	bool operator<(const ZaoHuaEquipHunLiAttrCfgKey & other) const
	{
		if (base_prof < other.base_prof)
			return true;
		if (base_prof > other.base_prof)
			return false;

		if (zao_hua_equip_index < other.zao_hua_equip_index)
			return true;
		if (zao_hua_equip_index > other.zao_hua_equip_index)
			return false;

		return false;
	}

	int base_prof;
	int zao_hua_equip_index;
};

struct ZaoHuaEquipHunLiAttrCfg
{
	int base_prof;
	int zao_hua_equip_index;

	int hun_li_value;
	std::vector<AttrItemCfg> attr_list;
};


//����
struct ZaoHuaEquipRefineCfgKey
{
	bool operator<(const ZaoHuaEquipRefineCfgKey & other)const
	{
		if (base_prof < other.base_prof)
			return true;
		if (base_prof > other.base_prof)
			return false;

		if (zao_hua_equip_index < other.zao_hua_equip_index)
			return true;
		if (zao_hua_equip_index > other.zao_hua_equip_index)
			return false;

		if (refine_level < other.refine_level)
			return true;
		if (refine_level > other.refine_level)
			return false;

		return false;
	}

	int base_prof;
	int zao_hua_equip_index;
	int refine_level;
};

struct ZaoHuaEquipRefineCfg
{
	int base_prof;
	int zao_hua_equip_index;
	int refine_level;

	int refine_need_equip_level;
	std::map<int, int> comsume_cfg;
	std::vector<AttrItemCfg> attr_cfg;
};

//��������
struct ZaoHuaEquipRefineResonanceCfg
{
	int base_prof;
	int need_refine_all_level;

	std::vector<AttrItemCfg> attr_cfg;
};

//��������
struct ZaoHuaLongZhuCfg
{
	int item_id; 
	std::vector<int> rand_ci_tiao_list;
	int limit_max_level;
	int ci_tiao_num;
};

struct ZaoHuaLongZhuCiTiaoCfg
{
	int ci_tiao_id;
	int need_long_zhu_level;

	std::map<int, AttrItemCfg> level_to_attr_cfg;
};

struct ZaoHuaLongZhuDrawUpLevelCfg
{
	int rate;
	int level;
};

struct ZaoHuaLongZhuDrawCfg
{
	ZaoHuaLongZhuDrawCfg()
	{
		rate_count = 0;
	}

	int GetRandUpLevel(int bao_di_ci_shu) const;


	int level;
	int comsume_item_num_draw_1;
	int comsume_item_num_draw_10;
	int up_level_bao_di_ci_shu;

	int rate_count;
	std::vector<ZaoHuaLongZhuDrawUpLevelCfg> item_list;
};

//��ħ����
struct ZaoHuaRuMoCfg
{
	int item_id;    //��ħǰ��Ʒid
	int to_item_id;//��ħ����Ʒid	
};

struct ZaoHuaChangeJobCfg
{
	ZaoHuaChangeJobCfg() :equip_level(0), limit_prof(0), color(0), equip_type(0), id(0) {}
	int equip_level;
	int limit_prof;
	int	color;
	int	equip_type;
	int	id;
};

class ZaoHuaEquipConfig : public ILogicConfig
{
public:
	ZaoHuaEquipConfig();
	virtual ~ZaoHuaEquipConfig();

	bool Init(const std::string &configname, std::string *err);

	const ZaoHuaSuitCfg * GetSuitCfg(int suit_id, int color, int level, int suit_num);
	const ZaoHuaComposeCfg * GetComposeCfg(int compose_seq);
	const ZaoHuaVigorousCfg * GetZaoHuaVigorousCfg(int seq);

	const ZaoHuaEquipOtherCfg & GetOtherCfg();

	//Ѫ��
	const ZaoHuaEquipHunLiAttrCfg * GetZaoHuaEquipHunLiAttrCfg(int base_prof, int zao_hua_equip_index, int hun_li_value);
	int GetRandLingXingValue();

	//Ԫ����
	const YuanHunZhuCfg * YuanHunZhuGetCfg(int item_id);
	const YuanHunZhuAttrGroupCfg * YuanHunZhuGetAttrGroupCfg(int group_id);
	const YuanHunZhuAttrGroupItemCfg * YuanHunZhuGetAttrGroupItemCfg(int group_id, int seq);
	const YuanHunZhuAttrGroupItemCfg * YuanHunZhuGetRandAttrGroupItemCfg(int group_id);
	const YuanHunZhuCombinationAttrCfg * YuanHunZhuGetCombinationAttrCfg(int base_prof, int combination_id, int combination_level);
	const YuanHunZhuCombinationAttrCfg * YuanHunZhuGetCombinationAttrCfg2(int base_prof, std::map<int, std::vector<int> > & zhu_ling_level_info);
	const YuanHunZhuDecomposeCfg * YuanHunZhuGetDecomposeCfg(int item_id);
	const YuanHunZhuComposeCfg * YuanHunZhuGetComposeCfg(int item_id);

	//����
	const ZaoHuaEquipRefineCfg * GetZaoHuaEquipRefineCfg(int base_prof, int zao_hua_equip_index, int refine_level);
	const ZaoHuaEquipRefineResonanceCfg * GetZaoHuaEquipRefineResonanceCfg(int base_prof, int refine_level);

	//����
	int GetZaoHuaLongZhuMaxLevel();
	const ZaoHuaLongZhuCfg * GetZaoHuaLongZhuCfg(int item_id);
	const ZaoHuaLongZhuCiTiaoCfg * GetZaoHuaLongZhuCiTiaoCfg(int ci_tiao_id);
	const AttrItemCfg * GetZaoHuaLongZhuCiTiaoAttrItemCfg(int ci_tiao_id, int ci_tiao_level);
	const ZaoHuaLongZhuDrawCfg * GetZaoHuaLongZhuDrawCfg(int long_zhu_level);
	int GetZaoHuaLongRandZhuCiTiao(int item_id, int long_zhu_level);

	//��ħ
	const ZaoHuaRuMoCfg * GetZaoHuaRuMoCfg(int item_id);
	//תְ
	const ZaoHuaChangeJobCfg * GetChangeJobCfg(int level, int prof, int color, int equip_type);
private:
	int InitEquipSuitCfg(TiXmlElement * RootElement); 
	int InitEquipComposeCfg(TiXmlElement * RootElement );
	int InitOhterCfg(TiXmlElement * RootElement);
	int InitVigorousCfg(TiXmlElement * RootElement);
	int InitZaoHuaEquipRefineCfg(TiXmlElement * RootElement);
	int InitZaoHuaEquipRefineResonanceCfg(TiXmlElement * RootElement);
	int InitZaoHuaEquipLingXingValueCfg(TiXmlElement * RootElement);
	int InitZaoHuaEquipHunLiAttrCfg(TiXmlElement * RootElement);
	int InitYuanHunZhuCfg(TiXmlElement * RootElement);
	int InitYuanHunZhuAttrGroupCfg(TiXmlElement * RootElement);
	int InitYuanHunZhuCombinationAttrCfg(TiXmlElement * RootElement);
	int InitYuanHunZhuCombinationCfg(TiXmlElement * RootElement);
	int InitYuanHunZhuDecomposeCfg(TiXmlElement * RootElement);
	int InitYuanHunZhuComposeCfg(TiXmlElement * RootElement);
	int InitZaoHuaLongZhuDrawCfg(TiXmlElement * RootElement);
	int InitZaoHuaLongZhuCfg(TiXmlElement * RootElement);
	int InitZaoHuaLongZhuCiTiaoCfg(TiXmlElement * RootElement);
	int InitZaoHuaRuMoCfg(TiXmlElement * RootElement);
	int InitChangeJobCfg(TiXmlElement * RootElement);

	long long GetKeyByLevelAndSuitAndColor(int suit_id, int color, int level, int suit_num);
	void GetLevelAnsSuitAndColorAndSuitNumByKey(long long key, int *suit_id, int *color, int *level, int *suit_num);
	
	std::map<long long, ZaoHuaSuitCfg> m_suit_map;
	std::map<int, ZaoHuaComposeCfg> m_compose_map;

	//Ԫ����
	std::map<int, YuanHunZhuCfg> m_yuan_hun_zhu_cfg;
	std::map<int, YuanHunZhuAttrGroupCfg> m_yuan_hun_zhu_attr_group_cfg;
	std::map<YuanHunZhuCombinationAttrCfgKey, YuanHunZhuCombinationAttrCfg> m_yuan_hun_zhu_combination_attr_cfg;
	std::vector<YuanHunZhuCombinationCfg> m_yuan_hun_zhu_combination_cfg;
	std::map<int, YuanHunZhuDecomposeCfg> m_yuan_hun_zhu_decompose_cfg;
	std::map<int, YuanHunZhuComposeCfg> m_yuan_hun_zhu_compose_cfg;

	//Ѫ��
	ZaoHuaEquipLingXingValueCfg m_ling_xing_cfg;			//����	
	std::map<ZaoHuaEquipHunLiAttrCfgKey, std::vector<ZaoHuaEquipHunLiAttrCfg > > m_hun_li_attr_cfg;  //����
	//-----------
	ZaoHuaEquipOtherCfg m_other_cfg;

	std::map<int, ZaoHuaVigorousCfg> m_vigorous_map;

	//-----����
	std::map<ZaoHuaEquipRefineCfgKey, ZaoHuaEquipRefineCfg> m_refine_cfg;   //����
	std::map<int, std::vector<ZaoHuaEquipRefineResonanceCfg> > m_refine_resonance_cfg; //��������

	//����
	std::map<int, ZaoHuaLongZhuDrawCfg> m_long_zhu_draw_cfg;	//������ȡ��Ϣ
	std::map<int, ZaoHuaLongZhuCfg> m_long_zhu_cfg;	//������Ϣ
	std::map<int, ZaoHuaLongZhuCiTiaoCfg> m_long_zhu_ci_tiao_cfg;	//����������Ϣ

	//��ħ
	std::map<int, ZaoHuaRuMoCfg> m_ru_mo_cfg;

	ZaoHuaChangeJobCfg * m_change_job_list[ItemNamespace::EQUIPMENT_MAX_LEVEL][PROFESSION_TYPE_NUM][ItemBase::I_COLOR_MAX][ZaoHuaItem::Z_TYPE_MAX];
	std::vector<ZaoHuaChangeJobCfg> m_change_job_vec;
};

#endif