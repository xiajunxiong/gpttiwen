#ifndef __BABY_PARAM_HPP__
#define __BABY_PARAM_HPP__

#include "common/tlvprotocol.h"
#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

struct BabyDataParam
{
	BabyDataParam()
	{
		this->Reset();
	}

	void Reset()
	{
		last_reset_time = 0;
		baby_id = 0;
		level = 0;
		duet_create_ji_fen = 0;
		is_duet = 0;
		add_jifen_times = 0;
		create_stage = 0;
		reserved_ch = 0;
		exp = 0;
		father_id = 0;
		mother_id = 0;

		memset(baby_name, 0, sizeof(baby_name));
		memset(father_name, 0, sizeof(father_name));
		memset(mother_name, 0, sizeof(mother_name));
	}

	unsigned int last_reset_time;
	long long baby_id;
	unsigned short level;			//等级
	unsigned short duet_create_ji_fen;	//双人生产积分
	unsigned char is_duet;		//单人生产，还是双人生产
	unsigned char add_jifen_times;
	unsigned char create_stage;
	unsigned char reserved_ch;
	int exp;					//当前经验
	int father_id;
	int mother_id;
	GameName baby_name;
	GameName father_name;
	GameName mother_name;
};

typedef char BabyDataParamHex[sizeof(BabyDataParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(BabyDataParamHex) < 1024);

struct AllBabyDataParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	void Reset()
	{
		count = 0;
	}

	struct AdditionInfo
	{
		AdditionInfo()
		{
		}

		long long baby_inc_id;
		int change_state;
	};	


	static const int MAX_ONCE_BABY_COUNT = 100;

	int count;
	AdditionInfo addition_info[MAX_ONCE_BABY_COUNT];
	BabyDataParam baby_list[MAX_ONCE_BABY_COUNT];
};

#pragma pack(pop)




#endif  // __BABY_PARAM_HPP__

