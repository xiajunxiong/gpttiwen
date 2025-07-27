/******************************************************************************************
FileName: rankparam.hpp
Author: dreamwang
Description: 排行榜在数据中的定义  
Version: 1.0
History: 
<author>    <time>					<version >    <desc>
dreamwang   2011/08/26/ 09:04:05     1.0           创建
*****************************************************************************************/

#ifndef __RANK_PARAM_HPP__
#define __RANK_PARAM_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/rankdef.hpp"
#include "common/tlvprotocol.h"
#include "gameworld/gameworld/item/itembase.h"
#include "gameworld/gameworld/other/attributelist.hpp"

class PersonRankParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct DBPersonRankItem
	{
		char change_state;
		int index;							// 排名 | 当前排行榜类型中的

		RoleBaseInfo role_info;

		int rank_type;
		long long rank_value;
		int record_index;					// 弃用
		int flexible_int;
		long long flexible_ll;
	};

	int count;
	DBPersonRankItem person_rank_list[ PERSON_RANK_MAX_SIZE];
};

class PetRankParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct DBPetRankItem
	{
		char change_state;
		int index;

		int str_buff_level;
		RoleBaseInfo role_info;
		PetPro pet_info;
		GodPrintListPro pet_god_print;		// 神印套装

		int rank_type;
		long long rank_value;
		int record_index;
		int flexible_int;
		long long flexible_ll;

		//宠物属性
		AttributeList pet_attr;
	};

	int count;
	DBPetRankItem pet_rank_list[PET_RANK_MAX_SIZE];
};

class TeamRankParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct DBTeamRankItem
	{
		char change_state;
		int index;
		
		TeamRankMemberInfo member_list[MAX_TEAM_MEMBER_NUM];
		int rank_type;
		long long rank_value;
		int flexible_int;
		long long flexible_ll;
	};

	int count;
	DBTeamRankItem team_rank_list[TEAM_RANK_TYPE_MAX * TEAM_RANK_MAX_SIZE];
};

class PartnerRankParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct DBPartnerRankItem
	{
		char change_state;
		int index;

		int user_uid;
		int partner_id;

		int rank_type;
		long long rank_value;
		int flexible_int;
		long long flexible_ll;
	};

	int count;
	DBPartnerRankItem partner_rank_list[PARTNER_RANK_MAX_SIZE];
};

#endif  // __RANK_PARAM_HPP__

