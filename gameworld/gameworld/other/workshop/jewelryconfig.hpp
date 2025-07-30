#ifndef __JEWELRY_CONFIG_HPP__
#define __JEWELRY_CONFIG_HPP__

#include "gamedef.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "equipment/jewelry.h"
#include "servercommon/workshopdef.h"

#include <map>
#include <set>
#include <vector>

static const int SUIT_ATTR_COUNT_MAX = 3;			// 套装属性数量
static const int CRAFT_MATE_NEED_MAX = 3;			// 灵饰打造所需材料种类最大数

enum LEGENDARY_PROPERTY			// 套装传说属性 - 按照百分比加成
{
	LGP_TYPE_INVALID = 0,				// 无属性

	LGP_TYPE_BASIC_ATK = 1,				// 基础攻击力
	LGP_TYPE_BASIC_SPI = 2,				// 基础精神力
	LGP_TYPE_BASIC_REC = 3,				// 基础恢复力
	LGP_TYPE_BASIC_MHP_AND_MMP = 4,		// 基础血和魔

	LGP_TYPE_MAX
};

struct JewelryBasic
{
	JewelryBasic(int _j_type = 0, int _j_level = 0, int _j_color = 0) : jewelry_type(_j_type), jewelry_level(_j_level), jewelry_color(_j_color) {}

	int jewelry_type;
	int jewelry_level;
	int jewelry_color;

	bool operator < (const JewelryBasic & cobj) const
	{
		if (this->jewelry_type != cobj.jewelry_type) return this->jewelry_type < cobj.jewelry_type;
		if (this->jewelry_level != cobj.jewelry_level) return this->jewelry_level < cobj.jewelry_level;
		if (this->jewelry_color != cobj.jewelry_color) return this->jewelry_color < cobj.jewelry_color;

		return false;
	}

	bool operator == (const JewelryBasic & cobj) const
	{
		return (this->jewelry_type == cobj.jewelry_type && this->jewelry_level == cobj.jewelry_level && this->jewelry_color == cobj.jewelry_color);
	}
};

struct JewelryInfor
{
	JewelryInfor(int _j_at = 0, int _j_ji = 0) : attr_type(_j_at), jewelry_id(_j_ji) {}

	int attr_type;
	int jewelry_id;
};

struct SuitInfoHead			// 套装词牌配置 - 键
{
	SuitInfoHead(int p_lv = 0, int p_cl = 0) : lyrics_level(p_lv), lyrics_color(p_cl) {}

	int lyrics_level;		// 词牌等级
	int lyrics_color;		// 词牌品质

	bool operator < (const SuitInfoHead & cobj) const
	{
		if (this->lyrics_level < cobj.lyrics_level) return true;
		
		return (this->lyrics_level == cobj.lyrics_level && this->lyrics_color < cobj.lyrics_color);
	}

	bool operator == (const SuitInfoHead & cobj) const
	{
		return (this->lyrics_level == cobj.lyrics_level && this->lyrics_color == cobj.lyrics_color);
	}
};

struct SuitInfoTail			// 套装词牌配置
{
	SuitInfoTail(int p_lt = 0) : lyric_type(p_lt), rand_prob(0), lyrics_id(0) {}

	int lyric_type;		// 词牌类型

	int rand_prob;		// 随机几率 - 万分比
	int lyrics_id;		// 词牌ID - 注意，不是词牌类型

	BattleAttrPair attr_add[SUIT_ATTR_COUNT_MAX];	// 套装属性

	bool operator < (const SuitInfoTail cobj) const
	{
		return (this->lyric_type < cobj.lyric_type);
	}

	bool operator == (const SuitInfoTail cobj) const
	{
		return (this->lyric_type == cobj.lyric_type);
	}
};

struct SuitBasicCfg		// 基本信息
{
	SuitBasicCfg(int l_t = 0, int s_l = 0, int s_c = 0) : lyric_type(l_t), suit_level(s_l), suit_color(s_c) {}

	int lyric_type;			// 词牌名
	int suit_level;			// 等级
	int suit_color;			// 品质

	bool operator < (const SuitBasicCfg & cobj) const		// 品质优先对比，品质相同再比较等级
	{
		if (this->suit_color < cobj.suit_color) return true;
		
		return (this->suit_color == cobj.suit_color && this->suit_level < cobj.suit_level);
	}
};

struct CompoundRand		// 灵饰 打造 随机配置
{
	CompoundRand() : jewelry_id(0), rand_prob(0) {}

	int jewelry_id;		// 装备ID
	int rand_prob;		// 随机权重
};

struct CompoundCont		// 灵饰打造基本信息
{
	CompoundCont() : compound_lv(0), jewelry_type(0), random_group(0), compound_pay(0) {}

	int compound_lv;		// 打造等级
	int jewelry_type;		// 灵饰类型
	int random_group;		// 随机组
	int compound_pay;		// 打造价格

	ItemPairCon mate_list[CRAFT_MATE_NEED_MAX];	// 材料列表
};

struct AnimaRandCfg		// 灵气值随机配置
{
	AnimaRandCfg() : rand_prob(0), anima_val(0) {}

	int rand_prob;			// 概率
	int anima_val;			// 灵气值
};

struct JewelryAttrValue		// 灵饰属性
{
	JewelryAttrValue() : id(0), attr_type(0), attr_min(0), attr_max(0), attr_type_2(0), attr_min_2(0), attr_max_2(0){}

	int id;					// 概率
	int attr_type;			// 属性类型
	int attr_min;
	int attr_max;
	int attr_type_2;			// 属性类型2
	int attr_min_2;
	int attr_max_2;
};

struct JewelryColorInfo		// 灵饰品质
{
	JewelryColorInfo() : quality_id(0), color(0), rate(0), fine_tuning_value(0), id(0){}

	int quality_id;
	int color;
	int rate;
	int fine_tuning_value;
	int id;
};

struct JewelryColorInfoList		// 灵饰品质组
{
	JewelryColorInfoList(){}

	JewelryColorInfo jewelry_color_list[INTJEWELRY_SUCC_MAX_LIST];
};

class JewelryConfig : public ILogicConfig
{
public:
	JewelryConfig();
	virtual ~JewelryConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const SuitBasicCfg * GetBasicCfgByLyricID(int lyric_id);
	const SuitInfoTail * GetSuitInfo(int level, int color, int lyric_type);
	const CompoundCont * GetCompoundInfo(int list_id);
	const BattleAttrPair * GetLegendaryAttr(int lyric_type);

	const int RandomAJewelryID(int group_id);										// 打造时，随机一个合法的灵饰ID
	const int RandomALyricsID(int jewelry_lv, int jewelry_color);					// 打造时，随机一个词牌ID
	const int RandomAnimaValue();													// 随机一次灵气值
	const int SearchLegalJewelry(const JewelryBasic & s_head, int attr1, int attr2);// 根据信息搜索合法的灵饰ID
	const int SearchLegalLyricID(int j_lv, int j_color, int lyric_1, int lyric_2);	// 根据信息搜索合法的词牌ID

	const JewelryColorInfoList * GetJewelryColorListCfgByGroup(int group_id);

private:
	int InitSuitLyricsCfg(TiXmlElement * RootElement);
	int InitCompoundGroup(TiXmlElement * RootElement);
	int InitCraftInforCfg(TiXmlElement * RootElement);
	int InitAnimaNumbRand(TiXmlElement * RootElement);
	int InitQualityGroupCfg(TiXmlElement * RootElement);

	void InitJewelryMap();
	//void InsertJewelryInfo(const JewelryBasic & p_jb, const JewelryInfor & p_ji);

	std::map<int, SuitBasicCfg> m_basic_cfg;								// 词牌基本配置 - 键 是 词牌ID

	std::map<SuitInfoHead, std::set<SuitInfoTail> > m_suit_basic_cfg;		// 套装基本配置
	std::map<int, BattleAttrPair> m_legendary_attr;							// 传说属性加成 - 键 是 词牌类型 type

	std::map<int, std::vector<CompoundRand> > m_coumpound_cfg;				// 灵饰 打造 随机配置
	std::map<int, CompoundCont> m_craft_info;								// 打造信息

	std::vector<AnimaRandCfg> m_anima_rand;									// 灵气值随机

	//std::map<JewelryBasic, std::vector<JewelryInfor> > m_jewelry_list;		// 灵饰对应表
	DynamicArray<JewelryInfor> m_jewelry_list[Jewelry::J_TYPE_MAX][MAX_ROLE_LEVEL + 1][ItemBase::I_COLOR_MAX];		// 灵饰对应表

	std::map<int, JewelryAttrValue> m_jewelry_attr_value_map;				//灵饰对应属性随机表

	std::map<int, JewelryColorInfoList> m_jewelry_color_map;				//灵饰对应品质随机
};

#endif
