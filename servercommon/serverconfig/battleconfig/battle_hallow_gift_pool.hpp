#ifndef __BATTLE_HALLOW_GIFT_POOL_HPP__
#define __BATTLE_HALLOW_GIFT_POOL_HPP__

#include <vector>
#include "servercommon/struct/battle/battle_hallow_gift_def.hpp"
#include "servercommon/configcommon.h"

struct HallowGiftEffectCfg
{
	HallowGiftEffectCfg() : effect_seq(0), effect_type(0), param1(0),
	param2(0), param3(0), param4(0), trigger_timing(0),
	limit_trigger_times(0), limit_round_trigger_times(0) {}

	int effect_seq;
	int effect_type;
	int param1;
	int param2;
	int param3;
	int param4;
	int trigger_timing;
	int limit_trigger_times;
	int limit_round_trigger_times;
};

struct HallowGiftEffectKey 
{
	HallowGiftEffectKey() : effect_class(0), effect_seq(0) {}

	bool operator< (const HallowGiftEffectKey& key) const
	{
		if (effect_class != key.effect_class)
		{
			return effect_class < key.effect_class;
		}
		else
		{
			return effect_seq < key.effect_seq;
		}
	}

	int effect_class;
	int effect_seq;
};

struct HallowGiftCfg
{
	HallowGiftCfg() : gift_id(0) {}

	int gift_id;
	std::vector<HallowGiftEffectKey> gift_effects;
};

class BattleHallowGiftPool
{
public:
	typedef std::map<int, HallowGiftCfg> GiftCfgMap;
	typedef std::map<HallowGiftEffectKey, HallowGiftEffectCfg> GiftEffectCfgMap;
public:
	BattleHallowGiftPool();
	~BattleHallowGiftPool();

	static BattleHallowGiftPool& Instance();
	bool Init(const std::string &configname, std::string *err);
	static bool Reload(const std::string &configname, std::string *err);

	const HallowGiftCfg* GetGiftCfg(int gift_id) const;
	const HallowGiftEffectCfg* GetGiftEffectCfg(const HallowGiftEffectKey& key) const;
	static bool CheckEffectType(short effect_class, short effect_type);
private:
	int InitGiftCfg(TiXmlElement *RootElement);
	int InitGiftEffectCfg(TiXmlElement *RootElement, short effect_class);
	bool CheckGiftEffectCfg(std::string *err);

	GiftCfgMap m_gift_cfg_map;
	GiftEffectCfgMap m_gift_effect_cfg_map;
};

#endif