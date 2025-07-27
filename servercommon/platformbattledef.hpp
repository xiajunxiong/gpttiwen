#ifndef __PLATFORM_BATTLE_DEF_H__
#define __PLATFORM_BATTLE_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/partnerdef.h"
#include "servercommon/attributedef.hpp"
#include "servercommon/traindef.hpp"
#include "servercommon/petdef.h"

#include <set>

static const int PLATFORM_BATTLE_PLATFORM_NUM = 12;
static const int PLATFORM_BATTLE_RANK_ITEM_MAX = 10;
static const int PLATFORM_BATTLE_PET_PARTNER_MAX = 5;				//��̨վ����ս�ĵ�λ��
static const int PLATFORM_BATTLE_MAX_JOIN_NUM = 3;				//���μ���̨������

//----------------------------------------------------------�����----------------------------------------------------------------------------
struct PlatformBattleRoleData;

struct PlatformBattleRoleBattleInfoItem				//����ṹ��ʾ ���ĳ����̨�������Ϣ,�������
{
	PlatformBattleRoleBattleInfoItem();
	PlatformBattleRoleBattleInfoItem(int platform_num, int rank = 0);
	void SetPetAndPartnerData(const PlatformBattleRoleData & rank_item);

	bool IsInvaild() const;

	short platform_number;
	short rank;

	std::vector<std::pair<unsigned long long, int> > battling_pet;
	std::vector<std::pair<int, int> > battling_partner;
};
#pragma pack(push, 4)

//------------------------------------------Э���õ���--------------------------------------
struct MsgPlatformBattlePlayItem
{
	MsgPlatformBattlePlayItem()
	{
		pet_index = 0;
		pet_battle_pos = 0;
	}

	union 
	{
		struct
		{
			short pet_index;
			short pet_battle_pos;
			unsigned char pet_exclusive_skill_disable_flag;
			unsigned char lian_yao_exclusive_skill_disable_flag;
			short reserved;
		};

		struct
		{
			short partner_id;
			short partner_battle_pos;
			int reserved_int;
		};
	};
};

struct MsgPlatformBattleRoleInfoItem			// ĳ����ҵ���ռ����̨��Ϣ
{
	MsgPlatformBattleRoleInfoItem()
	{
	}

	MsgPlatformBattleRoleInfoItem(const PlatformBattleRoleBattleInfoItem & role_info);


	short platform_number;
	short rank;
	unsigned long long pet_unique_id[PLATFORM_BATTLE_PET_PARTNER_MAX];	//����0 ��Ч
	unsigned char pet_pos[PLATFORM_BATTLE_PET_PARTNER_MAX];    //��С5,
	char reserved[3];
	MsgPlatformBattlePlayItem partner_play_item[PLATFORM_BATTLE_PET_PARTNER_MAX];		//����ս��Ϣ
};

//--------------------------------------------------Ҫ��⣬ע�����-------------------------------------------------------------

struct PlatformBattlePetData
{
	PlatformBattlePetData()
	{
		this->Reset();
	}

	void Reset()
	{
		pet_id = 0;
		memset(name, 0, sizeof(name));
		str_level = 0;
		level = 0;
		exp = 0;
		hp = 0;
		mp = 0;
		exclusive_skill = 0;
		exclusive_passive_skill = 0;
		
		for (int i = 0; i < ARRAY_LENGTH(passive_list); ++i)
		{
			passive_list[i] = 0;
		}


		lock = 0;
		memset(growth, 0, sizeof(growth));
		memset(attr, 0, sizeof(attr));
		reserve_sh1 = 0;
		// ����Ѫ��������
		max_hp = 0;
		max_mp = 0;
		pet_auto_move_type = 0;
		pet_auto_skill_id = 0;
		pet_auto_skill_level = 0;
		reserve_sh2 = 0;
	}

	int				pet_id;
	GameName		name;
	// ǿ���ȼ�
	unsigned short	str_level;
	// �ȼ�&����
	unsigned short	level;
	long long		exp;

	// ��ǰ��Ѫ��������
	int				hp;
	int				mp;

	// �����б� ���ܵȼ�*1000 + ����id
	int exclusive_skill;
	int exclusive_passive_skill;

	// �츳�б�
	unsigned short	passive_list[PET_PASSIVE_MAX_COUNT];

	//0ΪĬ��û������  1 Ϊ������
	unsigned char		lock;
	// ����
	unsigned char		growth[ADD_POINT_TYPE_MAX];

	// �ѷ������Ե�
	unsigned short	attr[ADD_POINT_TYPE_MAX];
	unsigned short	reserve_sh1;

	// ����Ѫ��������
	int				max_hp;
	int				max_mp;

	short pet_auto_move_type;
	short pet_auto_skill_id;
	short pet_auto_skill_level;
	short reserve_sh2;
};

class PB_PlatformBattlePet;
struct PlatformBattlePet			// ����Ĳ���
{
	PlatformBattlePet()
	{
		this->Reset();
	}

	void Reset()
	{
		item_unique_id = 0;
		pet_id = 0;
		pos = 0;
		pet_capability = 0;
		pet_level = 0;
		pet_strengthen_level = 0;
		memset(pet_name, 0, sizeof(pet_name));

		pet_battle_data = PetBattleData();
	}

	bool IsValid() const
	{
		return   pet_id > 0;
	}

	bool SetDataFromPB(const PB_PlatformBattlePet& pb_data);
	bool GetPBData(ARG_OUT PB_PlatformBattlePet* out_data) const;

	unsigned long long item_unique_id;
	int pet_id;				//0��Ч
	int pos;
	int pet_capability;
	int pet_level;
	int pet_strengthen_level;
	GameName pet_name;

	PetBattleData pet_battle_data;
};

class PB_PlatformBattlePartner;
struct PlatformBattlePartner			// ���Ĳ���
{
	PlatformBattlePartner()
	{
		this->Reset();
	}

	void Reset()
	{
		pos = 0;
		level = 0;		
		partner_id = 0;
		capability = 0;

		partner_battle_data = PartnerBattleData();
	}

	bool IsValid() const
	{
		return  partner_id > 0;
	}

	bool SetDataFromPB(const PB_PlatformBattlePartner& pb_data);
	bool GetPBData(ARG_OUT PB_PlatformBattlePartner* out_data) const;

	int pos;
	int level;
	int partner_id;
	int capability;
	PartnerBattleData partner_battle_data;
};

class PB_PlatformBattleRoleData;
struct PlatformBattleRoleData
{
	PlatformBattleRoleData()
	{
		this->Reset();
	}

	void Reset()
	{
		role_id = 0;
		role_pet_train_skill_level = 0;
		role_level = 0;
		avatar_type = 0;
		headshort_id = 0;
		memset(role_name, 0, sizeof(role_name));
		prof = 0;
		param1 = 0;
		top1_time = 0;
		for (int i = 0; i < PLATFORM_BATTLE_PET_PARTNER_MAX; ++i)
		{
			battling_pet[i].Reset();
			battling_partner[i].Reset();
		}
	}

	bool IsRobot() const;
	int GetRobotDataIndex() const;
	int GetAllCapacity() const;

	bool SetDataFromPB(const PB_PlatformBattleRoleData& pb_data);
	bool GetPBData(ARG_OUT PB_PlatformBattleRoleData* out_data) const;

	int role_id;
	int role_pet_train_skill_level;
	int role_level;
	short avatar_type;
	short headshort_id;
	GameName role_name;
	short prof;
	short param1;
	unsigned int top1_time;		//��Ϊ��1��ʱ�䣬���������ϰ������

	PlatformBattlePet battling_pet[PLATFORM_BATTLE_PET_PARTNER_MAX];
	PlatformBattlePartner battling_partner[PLATFORM_BATTLE_PET_PARTNER_MAX];
};

typedef char PlatformBattleRoleDataaHex[sizeof(PlatformBattleRoleData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(PlatformBattleRoleDataaHex) < 14000);

struct PlatformBattleRankEntry
{
	PlatformBattleRankEntry()
	{
		this->Reset();
	}

	void Reset()
	{
		platform_number = 0;
		platform_rank = 0;
		memset(role_data_buffer, 0, sizeof(role_data_buffer));
		role_data_buffer_length = 0;
	}

	int platform_number;
	int platform_rank;
	char role_data_buffer[sizeof(PlatformBattleRoleData) * 2];
	int role_data_buffer_length;
};

#pragma pack(pop)

//----------------------�ɱ䳤��Э���õ�------------------------------------
struct MsgPlatformBattleRoleInfoItemList			// ĳ���������ռ����̨��Ϣ
{
	MsgPlatformBattleRoleInfoItemList() {}
	bool Serialize(TLVSerializer & outstream) const;
	bool Unserialize(TLVUnserializer & instream);

	int role_id;
	std::map<int, PlatformBattleRoleBattleInfoItem > role_info_items;
};

#endif
