#ifndef __UTA_GUESS_PARAM_HPP__
#define __UTA_GUESS_PARAM_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/protobuf/uta_guess_data.pb.h"

static const int MAX_ONCE_UTA_GUESS_COUNT = 1000;		// ���β�������
static const int MAX_MATCH_TEAM_NUM = 2;				// ÿ��������������������

// ��ע UTA == Universe Team Arena 

struct UTAGuessKey
{
	UTAGuessKey() : match_idx(-1), team_idx(-1) {}
	UTAGuessKey(const UTAGuessKey &v) : match_idx(v.match_idx), team_idx(v.team_idx) {}
	UTAGuessKey(long long  _match_idx, long long  _team_idx) : match_idx(_match_idx), team_idx(_team_idx) {}

	bool operator<(const UTAGuessKey &right) const
	{
		return match_idx < right.match_idx || (match_idx == right.match_idx && team_idx < right.team_idx);
	}

	bool operator==(const UTAGuessKey &right) const
	{
		return match_idx == right.match_idx && team_idx == right.team_idx;
	}

	bool operator!=(const UTAGuessKey &right) const
	{
		return match_idx != right.match_idx || team_idx != right.team_idx;
	}

	long long match_idx;											// �������
	long long team_idx;												// �������

};


struct UTAGuessUserInfoData
{
	UTAGuessUserInfoData() { this->Reset(); }

	bool GetPBData(ARG_OUT PB_UTAGuessUserInfoData * _out_data) const;
	bool SetDataFromPB(const PB_UTAGuessUserInfoData& _pb_data);

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	void Reset();

	char change_state;									// ���״̬

	int uid;										//!< ���UID
	int plat_id;									//!< ����ƽ̨ID
	int server_id;									//!< ����������ID

	long long team_seq;								//!< ��ע�Ķ������
	long long match_seq;							//!< ��ע�ı������
	int bet_coin_num;								//!< ��ע�Ľ��
	int is_get;										//!< �Ƿ�����ȡ
};

enum GUESS_STATUS_TYPE
{
	GUESS_STATUS_TYPE_START = 0 ,				//!< ������ע
	GUESS_STATUS_TYPE_STOP = 1,					//!< ֹͣ��ע
	GUESS_STATUS_TYPE_REWARD_WIN = 2,			//!< ����,Ŀ������ʤ
	GUESS_STATUS_TYPE_REWARD_LOSE = 3,			//!< ����,Ŀ�����ʧ��
	GUESS_STATUS_TYPE_REWARD_DRAW = 4,			//!< ����,ƽ��
};

struct UTAGuessTeamInfoData
{
	UTAGuessTeamInfoData() { this->Reset(); }

	bool GetPBData(ARG_OUT PB_UTAGuessTeamInfoData * _out_data) const;
	bool SetDataFromPB(const PB_UTAGuessTeamInfoData& _pb_data);

	void Reset();

	long long team_seq;								//!< ��ע�Ķ������
	long long match_seq;							//!< ��ע�ı������

	int plat_id;									//!< ����ƽ̨ID
	int server_id;									//!< ����������ID

	int total_bet_num;								//!< ��ע�Ľ��
	int guess_status;								//!< ����״̬  [0:������ע , 1:ֹͣ��ע , 2:����[��ʤ] , 3����[ʧ��]]
};

struct UTAGuessMatchInfoData
{
	UTAGuessMatchInfoData() { this->Reset(); }

	bool GetPBData(ARG_OUT PB_UTAGuessMatchInfoData * _out_data) const;
	bool SetDataFromPB(const PB_UTAGuessMatchInfoData& _pb_data);

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	void Reset();

	char change_state;									// ���״̬

	long long match_seq;								//!< �������
	UTAGuessTeamInfoData team_info[MAX_MATCH_TEAM_NUM];		//!< �μӸñ����Ķ�����Ϣ
};

// ���ݱ������Protobuf���л�����BinToHex����ַ�����������2.5
UNSTD_STATIC_CHECK(sizeof(UTAGuessTeamInfoData) * 2.5  < 256);

struct UTAGuessUserEarningsInfoParam
{
	UTAGuessUserEarningsInfoParam() { this->Reset(); }

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	void Reset();

	char change_state;								// ���״̬

	int uid;										//!< ���UID
	int plat_id;									//!< ����ƽ̨ID
	int server_id;									//!< ����������ID
	int earnings;									// ����
};

struct UTAGuessMatchInfoParamList	
{
	UTAGuessMatchInfoParamList()
	{
		id_from = 0;
		count = 0;
		memset(match_info_arr, 0, sizeof(match_info_arr));
	}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	long long id_from;
	int count;
	UTAGuessMatchInfoData  match_info_arr[MAX_ONCE_UTA_GUESS_COUNT];
};

struct UTAGuessUserInfoParamList
{
	UTAGuessUserInfoParamList()
	{
		id_from = 0;
		count = 0;
		memset(user_info_arr, 0, sizeof(user_info_arr));
	}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	long long id_from;
	int count;
	UTAGuessUserInfoData  user_info_arr[MAX_ONCE_UTA_GUESS_COUNT];
};

struct UTAGuessEarningsInfoParamList
{
	UTAGuessEarningsInfoParamList()
	{
		id_from = 0;
		count = 0;
		memset(earnings_info_arr, 0, sizeof(earnings_info_arr));
	}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	long long id_from;
	int count;
	UTAGuessUserEarningsInfoParam  earnings_info_arr[MAX_ONCE_UTA_GUESS_COUNT];
};


#endif // __UTA_GUESS_PARAM_HPP__

