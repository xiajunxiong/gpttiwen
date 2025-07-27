#ifndef __UTA_CHAMPION_RECORD_PARAM_HPP__
#define __UTA_CHAMPION_RECORD_PARAM_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/teamdef.hpp"

static const int MAX_ONCE_UTA_C_R_COUNT = 100;		// 单次操作数量

// 备注 UTA == Universe Team Arena 

class PB_UTAChampionRecordUserInfoData;
class PB_UTAChampionRecordUserTeamInfoData;
class PB_UTAChampionRecordData;

struct UTAChampionRecordUserInfo
{
	UTAChampionRecordUserInfo() { this->Reset(); }

	bool GetPBData(ARG_OUT PB_UTAChampionRecordUserInfoData * out_data) const;
	bool SetDataFromPB(const PB_UTAChampionRecordUserInfoData& pb_data);

	void Reset();

	int uid;										//!< 玩家UID
	GameName role_name;								//!< 玩家名字
	int plat_id;									//!< 所属平台ID
	int server_id;									//!< 所属服务器ID

	short level;									//!< 等级
	short fly_flag;									//!< 飞升阶段
	int top_level;									//!< 巅峰等级
	GuildName guild_name;							//!< 家族名称
	GuildBanner	guild_banner;						//!< 家族族徽
	int title_id;									//!< 称号
	int facescore_id;								//!< 颜值系统头衔资源ID 

	char prof_base;									//!< 职业[0,9)
	char advance_times;								//!< 进阶次数[0,6]
	char avatar_type;								//!< 角色模型[1,6]
	char color;										//!< 颜色[0,4)
	ItemID weapon_id;								//!< 武器ID
	ItemID sub_weapon_id;							//!< 副武器ID

	short headshot_id;								//!< 头像ID 
	short special_appearance_type;					//!< 特殊形象类型
	int special_appearance_param;					//!< 特殊形象参数

	short wear_surface[SURFACE_TYPE_MAX];			//!< 已装备的的幻化资源id 数组下标为配置表的type

	char smart_mounts_index;						//!< 当前骑乘的灵骑 -1:未骑乘
	char is_use_advanced_flag;						//!< 灵骑信息使用标识 0:未使用进阶形象 1:使用进阶形象
	short reserve_sh;								//!< 补位(协议里用到了) PB结构里没有用到 ,需要的时候再去PB结构里添加
};

struct UTAChampionRecordUserTeamInfo
{
	UTAChampionRecordUserTeamInfo() { this->Reset(); }

	bool GetPBData(ARG_OUT PB_UTAChampionRecordUserTeamInfoData * out_data) const;
	bool SetDataFromPB(const PB_UTAChampionRecordUserTeamInfoData& pb_data);

	void Reset();

	UTAChampionRecordUserInfo user_info[MAX_TEAM_MEMBER_NUM];	// 战队玩家信息
};

struct UTAChampionRecordParam
{
	UTAChampionRecordParam() { this->Reset(); }

	bool GetPBData(ARG_OUT PB_UTAChampionRecordData * out_data) const;
	bool SetDataFromPB(const PB_UTAChampionRecordData& pb_data);

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	void Reset();

	char change_state;								// 变更状态

	int rank;										// 第X界
	GameName team_name;								// 站队名称
	UTAChampionRecordUserTeamInfo user_team_info;	// 战队玩家信息
};

// 数据保存的是Protobuf序列化后再BinToHex后的字符串，所以用2.5
UNSTD_STATIC_CHECK(sizeof(UTAChampionRecordUserTeamInfo) * 2.5 < 4096);

struct UTAChampionRecordParamList
{
	UTAChampionRecordParamList()
	{
		id_from = 0;
		count = 0;
		memset(record_arr, 0, sizeof(record_arr));
	}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	long long id_from;
	int count;
	UTAChampionRecordParam  record_arr[MAX_ONCE_UTA_C_R_COUNT];
};

#endif // __UTA_CHAMPION_RECORD_PARAM_HPP__

