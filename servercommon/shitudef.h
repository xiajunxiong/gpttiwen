#ifndef __SHI_TU_DEF_H__
#define __SHI_TU_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
//------------------------------------------------不存库-----------------------------------------------------

struct ShiTuFbMonsterInfoItem
{
	ShiTuFbMonsterInfoItem()
	{
		group = 0;
		index = 0;
		is_kill = 0;
		obj_id = 0;
		pos_x = 0;
		pos_y = 0;
	}

	short group;
	short index;
	short is_kill;
	short obj_id;
	int pos_x;
	int pos_y;
};




//-----------------------------------------------------------------------------------------------------
const static int SHI_TU_SEEKING_MAX_COUNT = 50;							// 宣言最大个数
static const int SHITU_MAX_APPRENTICE_COUNT = 3;						// 可拥有最多的徒弟数
static const int SHITU_MAX_MASTER_COUNT = 2;							// 可拥有最多的师傅数
const static int SHI_TU_SEEKING_BEING_TIME = 3 * 24 * 3600;				// 宣言最长保存72小时
const static int SHI_TU_SEEKING_SAVE_TO_DB_TIME = 60;					// 每隔60秒保存一次

enum SHITU_GONGFA_TYPE
{
	SHITU_GONGFA_TYPE_STATIC = 0,											// 固定功法
	SHITU_GONGFA_TYPE_NORMAL,												// 普通功法

	SHITU_GONGFA_TYPE_MAX
};

enum SHI_TU_SEEKING_TYPE
{
	SHI_TU_SEEKING_TYPE_APPRENTICE,		//徒弟
	SHI_TU_SEEKING_TYPE_MASTER,			//师傅

	SHI_TU_SEEKING_TYPE_MAX
};

//师徒--里程类型   //当日领取次数 等限制需求经常修改. 没有存数组, 需要的单独加
enum SHI_TU_MILE_TYPE
{
	SHI_TU_MILE_TYPE_CAP_UP = 1,	// 评分提升
	SHI_TU_MILE_TYPE_PASS_DAY,		// 拜师天数
	SHI_TU_MILE_TYPE_MAX,
};

#pragma pack(push, 4)

struct RelationInfoOld
{
	RelationInfoOld()
	{
		this->Reset();
	}

	void Reset()
	{
		role_id = 0;
		relation_break_time = 0;
		is_graduation = 0;
		reserved_ch = 0;
		reserved_sh = 0;
	}

	int role_id;
	unsigned int relation_break_time;
	unsigned char is_graduation;
	unsigned char reserved_ch;
	short reserved_sh;
};

struct ShiTuRelationDataOld
{
	ShiTuRelationDataOld()
	{
		this->Reset();
	}

	void Reset()
	{
		for (int index = 0; index < SHITU_MAX_MASTER_COUNT; ++index)
		{
			master_list[index].Reset();
		}

		for (int index = 0; index < SHITU_MAX_APPRENTICE_COUNT; ++index)
		{
			apprentice_list[index].Reset();
		}
	}

	int ApprenticeCount()
	{
		int count = 0;
		for (int index = 0; index < SHITU_MAX_APPRENTICE_COUNT; ++index)
		{
			if (apprentice_list[index].role_id > 0)
			{
				++count;
			}
		}

		return count;
	}

	bool IsApprentice(int uid)
	{
		for (int index = 0; index < SHITU_MAX_APPRENTICE_COUNT; ++index)
		{
			if (uid == apprentice_list[index].role_id)
			{
				return true;
			}
		}

		return false;
	}

	void AddApprentice(int uid)
	{
		for (int index = 0; index < SHITU_MAX_APPRENTICE_COUNT; ++index)
		{
			if (apprentice_list[index].role_id <= 0)
			{
				apprentice_list[index].role_id = uid;
				apprentice_list[index].relation_break_time = 0;
				return;
			}
		}
	}

	void DeleteApprentice(int uid)
	{
		for (int index = 0; index < SHITU_MAX_APPRENTICE_COUNT; ++index)
		{
			if (apprentice_list[index].role_id == uid)
			{
				apprentice_list[index].Reset();
				return;
			}
		}
	}

	RelationInfoOld master_list[SHITU_MAX_MASTER_COUNT];
	RelationInfoOld apprentice_list[SHITU_MAX_APPRENTICE_COUNT];
};

struct RelationInfo
{
	RelationInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		role_id = 0;
		relation_break_time = 0;
		shi_tu_time = 0;
		reward_flag = 0;

		apprentice_old_cap = 0;
		mile_reward_flag = 0;
		relation_break_time_type = 0;
		memset(reserved_ch, 0, sizeof(reserved_ch));
	}

	int role_id;	
	unsigned int relation_break_time;			//师徒自动解除时间, 一方 对方不同意也会72小时毕业或出师还要不要保留
	unsigned int shi_tu_time;				//师徒组成时间
	unsigned int reward_flag;

	long long apprentice_old_cap;			//拜师时 徒弟的战力
	long long mile_reward_flag;				//里程奖励标志[序号]

	unsigned char day_online_flag;			//进入在线通知标记
	unsigned char hold_ch;
	unsigned short hold_sh;
	char relation_break_time_type;
	unsigned char reserved_ch[11];
};

struct ShiTuRelationData
{
	ShiTuRelationData()
	{
		this->Reset();
	}

	void Reset()
	{
		for (int index = 0; index < SHITU_MAX_MASTER_COUNT; ++index)
		{
			master_list[index].Reset();
		}

		for (int index = 0; index < SHITU_MAX_APPRENTICE_COUNT; ++index)
		{
			apprentice_list[index].Reset();
		}
	}

	RelationInfo master_list[SHITU_MAX_MASTER_COUNT];
	RelationInfo apprentice_list[SHITU_MAX_APPRENTICE_COUNT];
};

const static int SHI_TU_PARAM_MOD_VER = 1;

#ifdef _DEBUG
const static unsigned int SHI_TU_TIME = 1631808000;
#else
const static unsigned int SHI_TU_TIME = 1632672000;
#endif

struct ShiTuParam
{
	ShiTuParam()
	{
		this->Reset();
	}

	void Reset()
	{
		relation_data_old.Reset();
		shi_tu_fb_reward_flag = 0;		
		relation_data.Reset();
		mod_ver = 0;

		day_mile_cap_fetch_num = 0;

		week_chuan_gong_flag = 0;
		week_chuan_gong_exp_flag = 0;
		week_chuan_gong_score_max = 0;

		day_mile_pass_day_fetch_num = 0;
		mile_reward_flag = 0;
	}

	ShiTuRelationDataOld relation_data_old;
	int shi_tu_fb_reward_flag;
	UNSTD_STATIC_CHECK(240 == sizeof(ShiTuRelationData));
	ShiTuRelationData relation_data;
	int mod_ver;

	int day_mile_cap_fetch_num;	//今日战力理程 领取次数
	
	int week_chuan_gong_flag;		//本周传功奖励标记
	int week_chuan_gong_exp_flag;	//本周传功经验标记
	int week_chuan_gong_score_max;	//本周传功最高得分

	int day_mile_pass_day_fetch_num;	//今日拜师天数理程 领取次数
	long long mile_reward_flag;				//里程奖励标志[序号]
};
typedef char ShiTuParamHex[sizeof(ShiTuParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ShiTuParamHex) < 1024);

struct ShiTuSeeking
{
	ShiTuSeeking()
	{
		this->Reset();
	}

	void Reset()
	{
		uid = 0;
		seeking_type = -1;
		timestamp = 0;
		end_timestamp = 0;
		memset(seeking_notice, 0, sizeof(seeking_notice));
	}

	bool Invalid() const
	{
		return uid <= 0;
	}

	int uid;								
	int seeking_type;						//宣言类型			//1 寻找徒弟， 0 寻找师傅
	unsigned int timestamp;					//发布宣言的时间戳
	unsigned int end_timestamp;				//宣言清掉的时间戳
	ShiTuSeekingNotice seeking_notice;
};

#pragma pack(pop)

struct ShiTuSeekingParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct ShiTuSeekingEntry
	{
		ShiTuSeekingEntry() : change_state(0), index(0)
		{}

		char change_state;
		int index;

		ShiTuSeeking data;
	};

	ShiTuSeekingEntry apprentice_list[SHI_TU_SEEKING_MAX_COUNT];
	ShiTuSeekingEntry master_list[SHI_TU_SEEKING_MAX_COUNT];
};



#endif
