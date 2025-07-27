#ifndef __UTA_GUESS_PARAM_HPP__
#define __UTA_GUESS_PARAM_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/protobuf/uta_guess_data.pb.h"

static const int MAX_ONCE_UTA_GUESS_COUNT = 1000;		// 单次操作数量
static const int MAX_MATCH_TEAM_NUM = 2;				// 每场比赛的最大参数队伍数

// 备注 UTA == Universe Team Arena 

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

	long long match_idx;											// 比赛序号
	long long team_idx;												// 队伍序号

};


struct UTAGuessUserInfoData
{
	UTAGuessUserInfoData() { this->Reset(); }

	bool GetPBData(ARG_OUT PB_UTAGuessUserInfoData * _out_data) const;
	bool SetDataFromPB(const PB_UTAGuessUserInfoData& _pb_data);

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	void Reset();

	char change_state;									// 变更状态

	int uid;										//!< 玩家UID
	int plat_id;									//!< 所属平台ID
	int server_id;									//!< 所属服务器ID

	long long team_seq;								//!< 下注的队伍序号
	long long match_seq;							//!< 下注的比赛序号
	int bet_coin_num;								//!< 下注的金额
	int is_get;										//!< 是否已领取
};

enum GUESS_STATUS_TYPE
{
	GUESS_STATUS_TYPE_START = 0 ,				//!< 可以下注
	GUESS_STATUS_TYPE_STOP = 1,					//!< 停止下注
	GUESS_STATUS_TYPE_REWARD_WIN = 2,			//!< 开奖,目标队伍获胜
	GUESS_STATUS_TYPE_REWARD_LOSE = 3,			//!< 开奖,目标队伍失败
	GUESS_STATUS_TYPE_REWARD_DRAW = 4,			//!< 开奖,平局
};

struct UTAGuessTeamInfoData
{
	UTAGuessTeamInfoData() { this->Reset(); }

	bool GetPBData(ARG_OUT PB_UTAGuessTeamInfoData * _out_data) const;
	bool SetDataFromPB(const PB_UTAGuessTeamInfoData& _pb_data);

	void Reset();

	long long team_seq;								//!< 下注的队伍序号
	long long match_seq;							//!< 下注的比赛序号

	int plat_id;									//!< 所属平台ID
	int server_id;									//!< 所属服务器ID

	int total_bet_num;								//!< 下注的金额
	int guess_status;								//!< 竞猜状态  [0:可以下注 , 1:停止下注 , 2:开奖[获胜] , 3开奖[失败]]
};

struct UTAGuessMatchInfoData
{
	UTAGuessMatchInfoData() { this->Reset(); }

	bool GetPBData(ARG_OUT PB_UTAGuessMatchInfoData * _out_data) const;
	bool SetDataFromPB(const PB_UTAGuessMatchInfoData& _pb_data);

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	void Reset();

	char change_state;									// 变更状态

	long long match_seq;								//!< 比赛序号
	UTAGuessTeamInfoData team_info[MAX_MATCH_TEAM_NUM];		//!< 参加该比赛的队伍信息
};

// 数据保存的是Protobuf序列化后再BinToHex后的字符串，所以用2.5
UNSTD_STATIC_CHECK(sizeof(UTAGuessTeamInfoData) * 2.5  < 256);

struct UTAGuessUserEarningsInfoParam
{
	UTAGuessUserEarningsInfoParam() { this->Reset(); }

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	void Reset();

	char change_state;								// 变更状态

	int uid;										//!< 玩家UID
	int plat_id;									//!< 所属平台ID
	int server_id;									//!< 所属服务器ID
	int earnings;									// 收益
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

