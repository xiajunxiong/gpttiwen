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

static const int SUIT_ATTR_COUNT_MAX = 3;			// ��װ��������
static const int CRAFT_MATE_NEED_MAX = 3;			// ���δ�������������������

enum LEGENDARY_PROPERTY			// ��װ��˵���� - ���հٷֱȼӳ�
{
	LGP_TYPE_INVALID = 0,				// ������

	LGP_TYPE_BASIC_ATK = 1,				// ����������
	LGP_TYPE_BASIC_SPI = 2,				// ����������
	LGP_TYPE_BASIC_REC = 3,				// �����ָ���
	LGP_TYPE_BASIC_MHP_AND_MMP = 4,		// ����Ѫ��ħ

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

struct SuitInfoHead			// ��װ�������� - ��
{
	SuitInfoHead(int p_lv = 0, int p_cl = 0) : lyrics_level(p_lv), lyrics_color(p_cl) {}

	int lyrics_level;		// ���Ƶȼ�
	int lyrics_color;		// ����Ʒ��

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

struct SuitInfoTail			// ��װ��������
{
	SuitInfoTail(int p_lt = 0) : lyric_type(p_lt), rand_prob(0), lyrics_id(0) {}

	int lyric_type;		// ��������

	int rand_prob;		// ������� - ��ֱ�
	int lyrics_id;		// ����ID - ע�⣬���Ǵ�������

	BattleAttrPair attr_add[SUIT_ATTR_COUNT_MAX];	// ��װ����

	bool operator < (const SuitInfoTail cobj) const
	{
		return (this->lyric_type < cobj.lyric_type);
	}

	bool operator == (const SuitInfoTail cobj) const
	{
		return (this->lyric_type == cobj.lyric_type);
	}
};

struct SuitBasicCfg		// ������Ϣ
{
	SuitBasicCfg(int l_t = 0, int s_l = 0, int s_c = 0) : lyric_type(l_t), suit_level(s_l), suit_color(s_c) {}

	int lyric_type;			// ������
	int suit_level;			// �ȼ�
	int suit_color;			// Ʒ��

	bool operator < (const SuitBasicCfg & cobj) const		// Ʒ�����ȶԱȣ�Ʒ����ͬ�ٱȽϵȼ�
	{
		if (this->suit_color < cobj.suit_color) return true;
		
		return (this->suit_color == cobj.suit_color && this->suit_level < cobj.suit_level);
	}
};

struct CompoundRand		// ���� ���� �������
{
	CompoundRand() : jewelry_id(0), rand_prob(0) {}

	int jewelry_id;		// װ��ID
	int rand_prob;		// ���Ȩ��
};

struct CompoundCont		// ���δ��������Ϣ
{
	CompoundCont() : compound_lv(0), jewelry_type(0), random_group(0), compound_pay(0) {}

	int compound_lv;		// ����ȼ�
	int jewelry_type;		// ��������
	int random_group;		// �����
	int compound_pay;		// ����۸�

	ItemPairCon mate_list[CRAFT_MATE_NEED_MAX];	// �����б�
};

struct AnimaRandCfg		// ����ֵ�������
{
	AnimaRandCfg() : rand_prob(0), anima_val(0) {}

	int rand_prob;			// ����
	int anima_val;			// ����ֵ
};

struct JewelryAttrValue		// ��������
{
	JewelryAttrValue() : id(0), attr_type(0), attr_min(0), attr_max(0), attr_type_2(0), attr_min_2(0), attr_max_2(0){}

	int id;					// ����
	int attr_type;			// ��������
	int attr_min;
	int attr_max;
	int attr_type_2;			// ��������2
	int attr_min_2;
	int attr_max_2;
};

struct JewelryColorInfo		// ����Ʒ��
{
	JewelryColorInfo() : quality_id(0), color(0), rate(0), fine_tuning_value(0), id(0){}

	int quality_id;
	int color;
	int rate;
	int fine_tuning_value;
	int id;
};

struct JewelryColorInfoList		// ����Ʒ����
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

	const int RandomAJewelryID(int group_id);										// ����ʱ�����һ���Ϸ�������ID
	const int RandomALyricsID(int jewelry_lv, int jewelry_color);					// ����ʱ�����һ������ID
	const int RandomAnimaValue();													// ���һ������ֵ
	const int SearchLegalJewelry(const JewelryBasic & s_head, int attr1, int attr2);// ������Ϣ�����Ϸ�������ID
	const int SearchLegalLyricID(int j_lv, int j_color, int lyric_1, int lyric_2);	// ������Ϣ�����Ϸ��Ĵ���ID

	const JewelryColorInfoList * GetJewelryColorListCfgByGroup(int group_id);

private:
	int InitSuitLyricsCfg(TiXmlElement * RootElement);
	int InitCompoundGroup(TiXmlElement * RootElement);
	int InitCraftInforCfg(TiXmlElement * RootElement);
	int InitAnimaNumbRand(TiXmlElement * RootElement);
	int InitQualityGroupCfg(TiXmlElement * RootElement);

	void InitJewelryMap();
	//void InsertJewelryInfo(const JewelryBasic & p_jb, const JewelryInfor & p_ji);

	std::map<int, SuitBasicCfg> m_basic_cfg;								// ���ƻ������� - �� �� ����ID

	std::map<SuitInfoHead, std::set<SuitInfoTail> > m_suit_basic_cfg;		// ��װ��������
	std::map<int, BattleAttrPair> m_legendary_attr;							// ��˵���Լӳ� - �� �� �������� type

	std::map<int, std::vector<CompoundRand> > m_coumpound_cfg;				// ���� ���� �������
	std::map<int, CompoundCont> m_craft_info;								// ������Ϣ

	std::vector<AnimaRandCfg> m_anima_rand;									// ����ֵ���

	//std::map<JewelryBasic, std::vector<JewelryInfor> > m_jewelry_list;		// ���ζ�Ӧ��
	DynamicArray<JewelryInfor> m_jewelry_list[Jewelry::J_TYPE_MAX][MAX_ROLE_LEVEL + 1][ItemBase::I_COLOR_MAX];		// ���ζ�Ӧ��

	std::map<int, JewelryAttrValue> m_jewelry_attr_value_map;				//���ζ�Ӧ���������

	std::map<int, JewelryColorInfoList> m_jewelry_color_map;				//���ζ�ӦƷ�����
};

#endif
