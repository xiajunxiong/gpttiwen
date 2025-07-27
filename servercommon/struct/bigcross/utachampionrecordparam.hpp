#ifndef __UTA_CHAMPION_RECORD_PARAM_HPP__
#define __UTA_CHAMPION_RECORD_PARAM_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/teamdef.hpp"

static const int MAX_ONCE_UTA_C_R_COUNT = 100;		// ���β�������

// ��ע UTA == Universe Team Arena 

class PB_UTAChampionRecordUserInfoData;
class PB_UTAChampionRecordUserTeamInfoData;
class PB_UTAChampionRecordData;

struct UTAChampionRecordUserInfo
{
	UTAChampionRecordUserInfo() { this->Reset(); }

	bool GetPBData(ARG_OUT PB_UTAChampionRecordUserInfoData * out_data) const;
	bool SetDataFromPB(const PB_UTAChampionRecordUserInfoData& pb_data);

	void Reset();

	int uid;										//!< ���UID
	GameName role_name;								//!< �������
	int plat_id;									//!< ����ƽ̨ID
	int server_id;									//!< ����������ID

	short level;									//!< �ȼ�
	short fly_flag;									//!< �����׶�
	int top_level;									//!< �۷�ȼ�
	GuildName guild_name;							//!< ��������
	GuildBanner	guild_banner;						//!< �������
	int title_id;									//!< �ƺ�
	int facescore_id;								//!< ��ֵϵͳͷ����ԴID 

	char prof_base;									//!< ְҵ[0,9)
	char advance_times;								//!< ���״���[0,6]
	char avatar_type;								//!< ��ɫģ��[1,6]
	char color;										//!< ��ɫ[0,4)
	ItemID weapon_id;								//!< ����ID
	ItemID sub_weapon_id;							//!< ������ID

	short headshot_id;								//!< ͷ��ID 
	short special_appearance_type;					//!< ������������
	int special_appearance_param;					//!< �����������

	short wear_surface[SURFACE_TYPE_MAX];			//!< ��װ���ĵĻû���Դid �����±�Ϊ���ñ��type

	char smart_mounts_index;						//!< ��ǰ��˵����� -1:δ���
	char is_use_advanced_flag;						//!< ������Ϣʹ�ñ�ʶ 0:δʹ�ý������� 1:ʹ�ý�������
	short reserve_sh;								//!< ��λ(Э�����õ���) PB�ṹ��û���õ� ,��Ҫ��ʱ����ȥPB�ṹ�����
};

struct UTAChampionRecordUserTeamInfo
{
	UTAChampionRecordUserTeamInfo() { this->Reset(); }

	bool GetPBData(ARG_OUT PB_UTAChampionRecordUserTeamInfoData * out_data) const;
	bool SetDataFromPB(const PB_UTAChampionRecordUserTeamInfoData& pb_data);

	void Reset();

	UTAChampionRecordUserInfo user_info[MAX_TEAM_MEMBER_NUM];	// ս�������Ϣ
};

struct UTAChampionRecordParam
{
	UTAChampionRecordParam() { this->Reset(); }

	bool GetPBData(ARG_OUT PB_UTAChampionRecordData * out_data) const;
	bool SetDataFromPB(const PB_UTAChampionRecordData& pb_data);

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	void Reset();

	char change_state;								// ���״̬

	int rank;										// ��X��
	GameName team_name;								// վ������
	UTAChampionRecordUserTeamInfo user_team_info;	// ս�������Ϣ
};

// ���ݱ������Protobuf���л�����BinToHex����ַ�����������2.5
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

