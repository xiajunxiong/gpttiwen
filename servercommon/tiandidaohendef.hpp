#ifndef __TIAN_DI_DAO_HEN_DEF_H__
#define __TIAN_DI_DAO_HEN_DEF_H__


#pragma pack(push, 4)

static const int TIAN_DI_DAO_HEN_MAX_ACTIVE_NUM = 4;

struct RoleTianDiDaoHenParam
{
	RoleTianDiDaoHenParam() { this->Reset(); }

	void Reset()
	{
		day_seq = 0;
		active_flag = 0;
		is_fetch_reward = 0;
	}

	int day_seq;
	unsigned int active_flag;
	int is_fetch_reward;
};

typedef char RoleTianDiDaoHenParamHex[sizeof(RoleTianDiDaoHenParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleTianDiDaoHenParamHex) < 512);

#pragma pack(pop)


#endif
