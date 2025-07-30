#ifndef __UTA_OB_FIGHT_DATA_HPP__
#define __UTA_OB_FIGHT_DATA_HPP__

#include <cstring>

struct UTAOBRoundData
{
	UTAOBRoundData() : sc_battle_info(0), length_sc_battle_info(0),
		sc_buff_list(0), length_sc_buff_list(0),
		sc_special_effect_list(0), length_sc_special_effect_list(0),
		sc_halo_list(0), length_sc_halo_list(0) 
	{
		memset(sc_fight_result, 0, sizeof(sc_fight_result));
		memset(length_fight_result, 0, sizeof(length_fight_result));
	}

	bool IsReadyForOB() const;

	char* sc_battle_info;
	int length_sc_battle_info;

	char* sc_buff_list;
	int length_sc_buff_list;

	char* sc_special_effect_list;
	int length_sc_special_effect_list;

	char* sc_halo_list;
	int length_sc_halo_list;

	char* sc_fight_result[2]; // 分别为准备阶段和战斗阶段的数据
	int length_fight_result[2];
};

class Role;
class UTAOBFightData
{
public:
	UTAOBFightData();
	~UTAOBFightData();

	void* operator new(size_t c);
	void operator delete(void* m);

	bool IsReadyForOB() const;
	void OnRoleStartOB(Role* role) const;
	unsigned int GetCreateTimestamp() const { return m_create_timestamp; }

	void SetData(int sc_battle_info_len, const char* sc_battle_info,
		int sc_buff_list_len, const char* sc_buff_list,
		int sc_special_effect_list_len, const char* sc_special_effect_list,
		int sc_halo_list_len, const char* sc_halo_list);
	void SetFightResultData(const char* msg, int length);
private:
	void Release();
	void ReleaseRoundData(UTAOBRoundData& round_data);
	void ReleaseFightResultData(UTAOBRoundData& round_data);
	void SendRoundResultData(Role* role, const UTAOBRoundData& round_data, bool is_send_battle_info = true) const;
	void SendFightResultData(Role* role, const char* msg, int length) const;

	void SetDataToRoundData(UTAOBRoundData& round_data, int sc_battle_info_len, const char* sc_battle_info,
		int sc_buff_list_len, const char* sc_buff_list,
		int sc_special_effect_list_len, const char* sc_special_effect_list,
		int sc_halo_list_len, const char* sc_halo_list);
	void SetFightResultToRoundData(UTAOBRoundData& round_data, const char* msg, int length);
	void TransferData(UTAOBRoundData& source, UTAOBRoundData& target);

	UTAOBRoundData m_cur_round_data;
	//UTAOBRoundData m_last_round_data;

	unsigned int m_create_timestamp;
};

#endif