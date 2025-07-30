#ifndef __ZHU_LING_CONFIG_HPP__
#define __ZHU_LING_CONFIG_HPP__

#include <vector>
#include <map>
#include <set>
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"

//�������������黥�⣬����ͬʱǶ��ͬһ��װ���У����ǻ����������ͬʱ��������������
enum ZHU_LING_STONE_TYPE
{
	ZHU_LING_STONE_TYPE_HUN_DUN,		//����
	ZHU_LING_STONE_TYPE_YIN,				//��
	ZHU_LING_STONE_TYPE_YANG,			//��	
};

struct ZhuLingCfg
{
	ZhuLingCfg();

	struct AttrItem
	{
		AttrItem();

		int attr_type;
		int attr_value;
	};

	int type;
	int item_id;
	int zhu_ling_level;
	int need_coin;
	int need_item_num;
	int spirit_stone_type;
	std::vector<AttrItem> attr_list;

	int need_base_item_num;		
};

struct ZhuLingCfgTypeAndLevelKey
{
	bool operator<(const ZhuLingCfgTypeAndLevelKey & other) const
	{
		if (type < other.type)
			return true;
		if (type > other.type)
			return false;

		if (zhu_ling_level < other.zhu_ling_level)
			return true;
		if (zhu_ling_level > other.zhu_ling_level)
			return false;

		return false;
	}

	int type;
	int zhu_ling_level;
};

struct ZhuLingDecomposeCfgKey
{
	bool operator<(const ZhuLingDecomposeCfgKey & other) const
	{
		if (spirit_stone_type < other.spirit_stone_type)
			return true;
		if (spirit_stone_type > other.spirit_stone_type)
			return false;

		if (color < other.color)
			return true;
		if (color > other.color)
			return false;

		if (zhu_ling_level < other.zhu_ling_level)
			return true;
		if (zhu_ling_level > other.zhu_ling_level)
			return false;

		return false;
	}

	int spirit_stone_type;
	int color;
	int zhu_ling_level;
};

struct ZhuLingDecomposeCfg
{
	int spirit_stone_type;
	int color;
	int zhu_ling_level;
	int get_fen_chen_num;
	int get_item_reward_rate;
	int get_item_reward_group_id;
};

struct ZhuLingDecomposeRandRewardCfg
{
	ZhuLingDecomposeRandRewardCfg()
	{
		group_id = 0;
		rate_count = 0;
	}

	struct ItemCfg
	{
		int rate;
		ItemConfigData reward;
	};

	int group_id;
	int rate_count;
	std::vector<ItemCfg> group_item_cfg;
};

struct ZhuLingOtherCfg
{
	ZhuLingOtherCfg()
	{
		
	}
};

struct ZhuLingCombinationAttrCfgKey
{
	bool operator<(const ZhuLingCombinationAttrCfgKey & other)const
	{
		if (group_id < other.group_id)
			return true;
		if (group_id > other.group_id)
			return false;

		if (base_prof < other.base_prof)
			return true;
		if (base_prof > other.base_prof)
			return false;

		if (equip_type < other.equip_type)
			return true;
		if (equip_type > other.equip_type)
			return false;

		if (combination_level < other.combination_level)
			return true;
		if (combination_level > other.combination_level)
			return false;


		return false;
	}

	int group_id;
	int base_prof;
	int equip_type;
	int combination_level;
};

struct ZhuLingCombinationAttrCfg
{
	struct AttrItemCfg
	{
		int type;
		int value;
	};

	int group_id;
	int base_prof;
	int equip_type;
	int combination_level;

	std::vector<AttrItemCfg> attr_cfg;					//��ϵĹ̶�����
	std::vector<AttrItemCfg> attr_percentage_cfg;			//��ϰ��ٷֱ����� һ��װ������������ ��������Ĺ̶�����
};


struct ZhuLingCombinationCfg
{
	int attr_group_id;
	std::map<int, int> zhu_ling_color_need_num;  //ÿ����ɫ��������Ҫ����
};

// -----------------------------------------------------------------------------------
class ZhuLingConfig : public ILogicConfig
{
public:
	ZhuLingConfig();
	virtual ~ZhuLingConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	int GetZhuLingUpLevelBaseItemId(int type);  //���0����Ʒ
	const ZhuLingCfg * GetZhuLingCfgByTypeAndLevel(int type, int level);
	const ZhuLingCfg * GetZhuLingCfg(int item_id);
	const  ZhuLingDecomposeCfg * GetZhuLingDecomposeCfg(ZhuLingDecomposeCfgKey key);
	const  ZhuLingDecomposeRandRewardCfg::ItemCfg * GetDecomposeRandRewardCfg(int group_id);
	int GetSlotNeedLevel(int slot_index);
	const ZhuLingOtherCfg & GetOtherCfg()const;
	bool CanPutOn(int equip_item_id, int zhu_ling_id);

	const ZhuLingCombinationAttrCfg * GetZhuLingCombinationAttrCfg(const ZhuLingCombinationAttrCfgKey & key);
	const ZhuLingCombinationCfg * GetZhuLingCombinationCfg(const std::map<int, int> & m, std::map<int, std::vector<int> > & zhu_ling_level_info, int ARG_OUT & combination_zhu_ling_level);
private:
	int InitZhuLingCfg(TiXmlElement * RootElement);
	int InitDecomposeCfg(TiXmlElement * RootElement);
	int InitDecomposeRandRewardCfg(TiXmlElement * RootElement);
	int InitZhuLingCombinationAttrCfg(TiXmlElement * RootElement);
	int InitZhuLingCombinationCfg(TiXmlElement * RootElement);
	int InitZhuLingSlotCfg(TiXmlElement * RootElement);
	int InitZhuLingPutOnCfg(TiXmlElement * RootElement);

	std::map<int, ZhuLingCfg> m_zhu_ling_cfg;
	std::map<ZhuLingCfgTypeAndLevelKey, ZhuLingCfg * > m_zhu_ling_type_and_level_map_cfg;    //����m_zhu_ling_cfg ���ɵ�ӳ�䣬�������

	std::map<int, ZhuLingDecomposeRandRewardCfg> m_decompose_rand_reward_cfg;
	std::map<ZhuLingDecomposeCfgKey, ZhuLingDecomposeCfg>  m_decompose_cfg;
	ZhuLingOtherCfg m_other_cfg;
	std::map<int, int> m_zhu_ling_slot_cfg;
	std::map<int, std::set<int> > m_zhu_ling_put_on_cfg;

	std::map<ZhuLingCombinationAttrCfgKey, ZhuLingCombinationAttrCfg> m_combination_attr_cfg;
	std::vector<ZhuLingCombinationCfg> m_combination_cfg;
};

#endif