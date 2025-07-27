#ifndef __BATTLE_VIDEO_DEF_HPP__
#define __BATTLE_VIDEO_DEF_HPP__

#include "servercommon/servercommon.h"
#include "common/tlvprotocol.h"

#pragma pack(push, 4)

struct BattleVideoParam
{
	BattleVideoParam()
	{
		this->Reset();
	}

	void Reset()
	{
		is_server_first_skill = 0;
		version = 0;
		battle_mode = 0;
		monster_group_id = 0;
		average_score = 0;
		record_timestamp = 0;
		video_file_id = 0;
	}

	short is_server_first_skill;   // 是否首杀
	short version;
	short battle_mode;
	int monster_group_id;
	int average_score;
	unsigned int record_timestamp;
	long long video_file_id;
};

class BattleVideoListParam
{
public:
	static const int MAX_VIDEO_NUM = 256;

public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct __changevideo
	{
		char change_state;
		long long id;			// 仅初始化时有用
		BattleVideoParam param;
	};

	int count;
	long long next_id;
	__changevideo video_list[MAX_VIDEO_NUM];
};

#pragma pack(pop)

#endif