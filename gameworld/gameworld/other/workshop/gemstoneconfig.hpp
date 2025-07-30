#ifndef __GEM_STONE_CONFIG_HPP__
#define __GEM_STONE_CONFIG_HPP__

#include "gamedef.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/battle/battle_def.hpp"

#include <map>
#include <set>
#include <vector>

static const int GEM_ATTR_COUNT_MAX = 2;		// ��ʯ�����������ֵ

struct GemstoneOtherInfoCfg
{
	GemstoneOtherInfoCfg() : compose_price_once(0) {}

	int compose_price_once;
};

struct GemstoneBasicInfoCfg 
{
	GemstoneBasicInfoCfg() : gemstone_type(0) , level(0), level_limit(0), godequip_level(0){}

	int gemstone_type;
	int level;
	int level_limit;
	int godequip_level;
	BattleAttrPair attr_add[GEM_ATTR_COUNT_MAX];
};

struct SlotOpenConfig
{
	SlotOpenConfig() :equiplimit_lv(0), godequiplimit_lv(0) {}

	int equiplimit_lv;
	int godequiplimit_lv;

};

enum GemstoneType
{										
	GEMSTONE_TYPE_HELMET = 0,		// ͷ��
	GEMSTONE_TYPE_ARMOUR,			// ����
	GEMSTONE_TYPE_WEAPON,			// ����
	GEMSTONE_TYPE_SHIELD,			// ����
	GEMSTONE_TYPE_SHOE,				// Ь��
	GEMSTONE_TYPE_RING,				// ��ָ
	GEMSTONE_TYPE_BRACELET,			// ����
	GEMSTONE_TYPE_PENDANTS,			// ׹��
	GEMSTONE_TYPE_MAX,
};

class GemstoneConfig : public ILogicConfig
{
public:
	GemstoneConfig();
	virtual ~GemstoneConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	bool CheckGemstoneCanMount(int equip_type, int gem_type);		// �жϱ�ʯ�Ƿ����װ������
	bool CheckGemMountSlotOpen(int slot_id, int equip_limit, bool isgodequip );			// �жϱ�ʯ��Ƕ���Ƿ���
	int GetGemstoneItemID(int gem_type, int gem_level);				// ���ݱ�ʯ���ͺ͵ȼ���ȡ��Ӧ��ʯ�ĵ���ID

	int GetGemstoneByType(int equip_type,int gem_type);

	const GemstoneOtherInfoCfg & GetGemstoneOtherInfoCfg() { return m_gemstone_other_cfg; }
	const GemstoneBasicInfoCfg * GetGemstoneBasicInfoCfg(int gem_item_id);

private:
	int InitGemstoneOtherCfg(TiXmlElement * RootElement);
	int InitGemstoneAttrsAdd(TiXmlElement * RootElement);
	int InitGemstoneCanMount(TiXmlElement * RootElement);
	int InitGemstoneSlotOpen(TiXmlElement * RootElement);

	GemstoneOtherInfoCfg m_gemstone_other_cfg;

	std::map<int, GemstoneBasicInfoCfg> m_gem_attrs_cfg;		// ��ʯ�������� - �� ��ʯID
	std::set<int> m_can_mount_cfg[GEMSTONE_TYPE_MAX];			// װ������Ƕ��ʯ�������� - �� װ������
	std::map<int, SlotOpenConfig> m_slot_open_cfg;			// ��Ƕ�׿��ŵȼ���Ϣ���� - �� ��Ƕ��ID��ֵ װ���ȼ�����||��װ��������

	std::map<int, std::map<int, ItemID> > m_gemstone_map;		// ��ʯ ���� -> �ȼ� -> ����ID ӳ���
};

#endif