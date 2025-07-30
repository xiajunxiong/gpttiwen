#ifndef __PET_EQUIPMENT_CONFIG_HPP__
#define __PET_EQUIPMENT_CONFIG_HPP__

#include "gamedef.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "equipment/petequipment.h"
#include "servercommon/workshopdef.h"

#include <map>
#include <set>
#include <vector>

struct PetEquipMentInfo		// ����װ��
{
	PetEquipMentInfo() : item_id(0), min_skill_num(0), max_skill_num(0), skill_rand_group(0){}

	int item_id;
	int min_skill_num;
	int max_skill_num;
	int skill_rand_group;
	std::vector<ItemConfigData> cost_item_vec;
};

struct PetEquipMentNumber		// ����װ��������
{
	PetEquipMentNumber() : weight(0), number(0){}

	int weight;
	int number;
};

struct PetEquipMentNumberGroup		// ����װ��
{
	PetEquipMentNumberGroup() : weight_count(0){}

	int weight_count;
	std::vector<PetEquipMentNumber> rand_vec;
};

struct PetEquipMentSkill		// ����װ������
{
	PetEquipMentSkill() : gift_id(0), weight(0) {}

	int gift_id;
	int weight;
};

struct PetEquipMentSkillGroup		// ����װ��������
{
	PetEquipMentSkillGroup() : weight_count(0) {}

	int weight_count;
	std::vector<PetEquipMentSkill> rand_vec;
};

struct PetEquipMentCompoundItemRand		// ����װ���ϳ�
{
	PetEquipMentCompoundItemRand() : target(0), weight(0) {}

	int target;
	int weight;
};

struct PetEquipMentCompoundItemRandGroup		// ����װ���ϳ���
{
	PetEquipMentCompoundItemRandGroup() : weight_count(0) {}

	int weight_count;
	std::vector<PetEquipMentCompoundItemRand> rand_vec;
};

struct PetEquipMentCompoundItemGroup		// ����װ���ϳɱ�
{
	const static int MAX_PET_EQUIPMENT_COMPOUND_NUM = 2;
	PetEquipMentCompoundItemGroup() : target(0)
	{
		memset(stuff_id_list, 0, sizeof(stuff_id_list));
		memset(stuff_num_list, 0, sizeof(stuff_num_list));
	}

	int stuff_id_list[MAX_PET_EQUIPMENT_COMPOUND_NUM];
	int stuff_num_list[MAX_PET_EQUIPMENT_COMPOUND_NUM];
	int target;
};

struct PetEquipMentRefiningGroup		// ����װ��ϴ����
{
	PetEquipMentRefiningGroup() : id(0), item_id(0), number(0){}

	int id;
	int item_id;
	int number;
};

class PetEquipMentConfig : public ILogicConfig
{
public:
	PetEquipMentConfig();
	virtual ~PetEquipMentConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const PetEquipMentInfo * GetPetEquipMentInfo(int seq);
	const PetEquipMentNumber * GetPetEquipMentRankSkillNumber(int seq);
	const PetEquipMentSkill * GetPetEquipMentRandSkill(int seq);
	const PetEquipMentSkillGroup * GetSkillRandMap(int seq);
	const PetEquipMentCompoundItemRand * GetPetEquipMentRandCompoundItem(int seq);
	const PetEquipMentCompoundItemGroup * GetPetEquipMentCompound(int seq);
	const PetEquipMentRefiningGroup * GetPetEquipMentRefiningCfg(int seq);
private:
	int InitNumberGroup(TiXmlElement * RootElement);
	int InitSkillGroup(TiXmlElement *RootElement);
	int InitCompoundItemRandGroup(TiXmlElement *RootElement);
	int InitCompoundGroup(TiXmlElement * RootElement);
	int InitRefiningCfg(TiXmlElement * RootElement);

	std::map<int, PetEquipMentInfo> m_pet_equipment_map;								// ����װ������map
	std::map<int, PetEquipMentNumberGroup > m_pet_equipment_number_map;					// ����װ�������������  key1:�����
	std::map<int, PetEquipMentSkillGroup > m_pet_equipment_skill_map;					// ����װ�����������	 key1:�����
	std::map<int, PetEquipMentCompoundItemRandGroup > m_pet_equipment_compound_item_map;// ����װ���ϳɲ��������	 key1:�����
	std::map<int, PetEquipMentCompoundItemGroup > m_pet_equipment_compound_map;			// ����װ���ϳ������	 key1:�����
	std::map<int, PetEquipMentRefiningGroup > m_pet_equipment_refining_map;				// ����װ��ϴ�������	 key1:�����
};

#endif
