#ifndef __ROLE_BATTLE_DATA_CAPABILITY_CALCULATOR_HPP__
#define __ROLE_BATTLE_DATA_CAPABILITY_CALCULATOR_HPP__

struct RoleBattleData;
struct BattleCharacterData;
class RoleBattleDataCapabilityCalculator
{
public:
	static int CalcRoleCapability(const RoleBattleData& role_battle_data);
	static int CalcPartnerCapability(const BattleCharacterData& partner_data);
	static int CalcPetCapability(const BattleCharacterData& pet_data);
	static int CalcCapability(const RoleBattleData& role_battle_data);
};

#endif