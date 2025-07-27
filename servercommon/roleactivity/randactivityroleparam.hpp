#ifndef __RA_RAND_ACTIVITY_ROLE_PARAM_HPP__
#define __RA_RAND_ACTIVITY_ROLE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


#pragma pack(push, 4)
//////////////////////////////////////////// 2084 ���� - ����۱�  //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_CAI_SHEN_JU_BAO_MAX_ITEM = 20;
const int static RA_CAI_SHEN_JU_BAO_BUY_MAX_ITEM = 32;
UNSTD_STATIC_CHECK(RA_CAI_SHEN_JU_BAO_BUY_MAX_ITEM >= RA_CAI_SHEN_JU_BAO_MAX_ITEM);
struct RACaiShenJuBaoParam
{
	RACaiShenJuBaoParam() { this->Reset(); }
	void Reset()
	{
		ra_cai_shen_ju_bao_begin_timestamp = 0;
		
		for (int i = 0; i < RA_CAI_SHEN_JU_BAO_MAX_ITEM; ++i)
		{
			ra_cai_shen_ju_bao_buy_day_times[i] = 0;
			ra_cai_shen_ju_bao_buy_times[i] = 0;
		}
		buy_flag.Reset();
	}

	unsigned int ra_cai_shen_ju_bao_begin_timestamp;
	unsigned short ra_cai_shen_ju_bao_buy_day_times[RA_CAI_SHEN_JU_BAO_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
	unsigned short ra_cai_shen_ju_bao_buy_times[RA_CAI_SHEN_JU_BAO_MAX_ITEM];				//ÿ����Ʒ�޹�����
	BitMap<RA_CAI_SHEN_JU_BAO_BUY_MAX_ITEM> buy_flag;										//�����ʶ 0:δ���� 1:�ѹ���
};

UNSTD_STATIC_CHECK(sizeof(RACaiShenJuBaoParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2085 ���� - ���Ʊ���  //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_DING_ZHI_BAO_XIANG_MAX_ITEM = 20;
const int static RA_DING_ZHI_BAO_XIANG_BUY_MAX_ITEM = 32;
UNSTD_STATIC_CHECK(RA_DING_ZHI_BAO_XIANG_BUY_MAX_ITEM >= RA_DING_ZHI_BAO_XIANG_MAX_ITEM);
struct RADingZhiBaoXiangParam
{
	RADingZhiBaoXiangParam() { this->Reset(); }
	void Reset()
	{
		ra_ding_zhi_bao_xiang_begin_timestamp = 0;
		has_free_reward = 0;

		for (int i = 0; i < RA_DING_ZHI_BAO_XIANG_MAX_ITEM; ++i)
		{
			ra_ding_zhi_bao_xiang_day_buy_times[i] = 0;
			ra_ding_zhi_bao_xiang_buy_times[i] = 0;
		}
		buy_flag.Reset();
	}

	unsigned int ra_ding_zhi_bao_xiang_begin_timestamp;
	unsigned short ra_ding_zhi_bao_xiang_day_buy_times[RA_DING_ZHI_BAO_XIANG_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
	unsigned short ra_ding_zhi_bao_xiang_buy_times[RA_DING_ZHI_BAO_XIANG_MAX_ITEM];				//ÿ����Ʒ�޹�����
	BitMap<RA_DING_ZHI_BAO_XIANG_BUY_MAX_ITEM> buy_flag;										//�����ʶ 0:δ���� 1:�ѹ���
	int has_free_reward;																		//��ѱ��䣬0-δ��ȡ��1-����ȡ
};

UNSTD_STATIC_CHECK(sizeof(RADingZhiBaoXiangParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2086 ���� - ר�����  //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_ZHUAN_SHU_LI_BAO_MAX_EXIST_ITEM = 5;
const int static RA_ZHUAN_SHU_LI_BAO_CHONG_ZHI_RECORD_NUM= 5;
const int static RA_ZHUAN_SHU_LI_BAO_DAY_MAX_ITEM_OLD_NUM = 8;		
const int static RA_ZHUAN_SHU_LI_BAO_DAY_MAX_ITEM_NUM = 5;

struct ZhuanShuLiBaoAciveItem
{
	ZhuanShuLiBaoAciveItem()
	{
		this->Reset();
	}

	void Reset()
	{
		is_active = 0;
		type = 0;
		buy_times = 0;
		end_time = 0;
	}

	short is_active;				//��0���Ǽ�����
	short type;				//���ñ�type			
	int buy_times;				//�������
	unsigned int end_time;		//����ʱ��
};

//ר�����-ÿ�����(����)
struct ZhuanShuLiBaoDayOldItem
{
	ZhuanShuLiBaoDayOldItem() { this->Reset(); }

	bool IsInVaild()
	{
		return phase <= 0 || type < 0;
	}
	void Reset()
	{
		phase = 0;
		type = 0;
		buy_times = 0;
		reserve_sh = 0;
	}

	short phase;		//�׶�
	short type;			//����
	short buy_times;
	short reserve_sh;
};

//ר�����-ÿ�����
struct ZhuanShuLiBaoDayItem
{
	ZhuanShuLiBaoDayItem() { this->Reset(); }
	ZhuanShuLiBaoDayItem(const ZhuanShuLiBaoDayOldItem & day_item)
	{
		phase = day_item.phase;
		type = day_item.type;
		buy_times = day_item.buy_times;
		reserve_sh = day_item.reserve_sh;
	}

	bool IsInVaild()
	{
		return phase <= 0 || type < 0;
	}

	void Reset()
	{
		phase = 0;
		type = 0;
		buy_times = 0;
		reserve_sh = 0;
		end_timestamp = 0;
	}

	short phase;		//�׶�
	short type;			//����
	short buy_times;
	short reserve_sh;
	unsigned int end_timestamp;		//�������ʱ���
};

struct RAZhuanShuLiBaoParam
{
	RAZhuanShuLiBaoParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		active_role_level = 0;
		for (int i = 0; i < ARRAY_LENGTH(active_item); ++i)
		{
			active_item[i].Reset();
		}
		for (int i = 0; i < ARRAY_LENGTH(reserve_active_item); ++i)
		{
			reserve_active_item[i].Reset();
		}
		login_day = 0;
		last_phase = 0;
		check_init_next_zero_timestamp = 0;
		for (int i = 0; i < ARRAY_LENGTH(old_day_item); i++)
		{
			old_day_item[i].Reset();
		}
		for (int i = 0; i < ARRAY_LENGTH(new_day_item); i++)
		{
			new_day_item[i].Reset();
		}
		reserve_ch = 0;
		is_data_move_flag = 0;
		reserve_sh = 0;
	}

	unsigned int ra_begin_timestamp;
	int active_role_level;
	ZhuanShuLiBaoAciveItem active_item[RA_ZHUAN_SHU_LI_BAO_MAX_EXIST_ITEM];
	ZhuanShuLiBaoAciveItem reserve_active_item[15];		//����,��ֹ��չ
	int login_day;
	int last_phase;
	unsigned int check_init_next_zero_timestamp;		//��������δ��¼��ҳ�ʼ����daychange,��������ֱ��Ϊ2
	ZhuanShuLiBaoDayOldItem old_day_item[RA_ZHUAN_SHU_LI_BAO_DAY_MAX_ITEM_OLD_NUM];		//ÿ�����
	ZhuanShuLiBaoDayItem new_day_item[RA_ZHUAN_SHU_LI_BAO_DAY_MAX_ITEM_NUM];
	char reserve_ch;
	char is_data_move_flag;				//ÿ�����������Ǩ�� 0:δǨ�� 
	short reserve_sh;
			
};

UNSTD_STATIC_CHECK(sizeof(RAZhuanShuLiBaoParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2087 ���� - ������� //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
struct RAJingYanBiaoShengParam
{
	RAJingYanBiaoShengParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		day_buy_times = 0;
		buy_times = 0;
	}

	unsigned int ra_begin_timestamp;
	int day_buy_times;
	int buy_times;
};

UNSTD_STATIC_CHECK(sizeof(RAJingYanBiaoShengParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2088 ���� - ��ʱ����  //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_XIAN_SHI_BAO_HE_MAX_ITEM = 20;
const int static RA_XIAN_SHI_BAO_HE_BUY_MAX_ITEM = 32;
UNSTD_STATIC_CHECK(RA_XIAN_SHI_BAO_HE_BUY_MAX_ITEM >= RA_XIAN_SHI_BAO_HE_MAX_ITEM);
struct RAXianShiBaoHeParam
{
	RAXianShiBaoHeParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		has_free_reward = 0;

		for (int i = 0; i < RA_XIAN_SHI_BAO_HE_MAX_ITEM; ++i)
		{
			ra_day_buy_times[i] = 0;
			ra_buy_times[i] = 0;
		}
		buy_flag.Reset();
	}

	unsigned int ra_begin_timestamp;
	unsigned short ra_day_buy_times[RA_XIAN_SHI_BAO_HE_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
	unsigned short ra_buy_times[RA_XIAN_SHI_BAO_HE_MAX_ITEM];				//ÿ����Ʒ�޹�����
	BitMap<RA_XIAN_SHI_BAO_HE_BUY_MAX_ITEM> buy_flag;						//�����ʶ 0:δ���� 1:�ѹ���
	int has_free_reward;													//��ѱ��䣬0-δ��ȡ��1-����ȡ
};

UNSTD_STATIC_CHECK(sizeof(RAXianShiBaoHeParam) <= sizeof(RoleActivityRoleData))
//////////////////////////////////////////// 2089 ���� - ��ʱ����  //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_XIAN_SHI_BAO_DAI_MAX_ITEM = 20;
const int static RA_XIAN_SHI_BAO_DAI_BUY_MAX_ITEM = 32;
UNSTD_STATIC_CHECK(RA_XIAN_SHI_BAO_DAI_BUY_MAX_ITEM >= RA_XIAN_SHI_BAO_DAI_MAX_ITEM);
struct RAXianShiBaoDaiParam
{
	RAXianShiBaoDaiParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		can_buy_seq = 0;
		has_free_reward = 0;
		for (int i = 0; i < RA_XIAN_SHI_BAO_DAI_MAX_ITEM; ++i)
		{
			ra_day_buy_times[i] = 0;
			ra_buy_times[i] = 0;
		}
		buy_flag.Reset();
	}

	unsigned int ra_begin_timestamp;
	int can_buy_seq;							//����������seq��ֻ�й����ǰһ�������������
	unsigned short ra_day_buy_times[RA_XIAN_SHI_BAO_DAI_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
	unsigned short ra_buy_times[RA_XIAN_SHI_BAO_DAI_MAX_ITEM];				//ÿ����Ʒ�޹�����
	BitMap<RA_XIAN_SHI_BAO_DAI_BUY_MAX_ITEM> buy_flag;						//�����ʶ 0:δ���� 1:�ѹ���
	int has_free_reward;													//��ѱ��䣬0-δ��ȡ��1-����ȡ
};

UNSTD_STATIC_CHECK(sizeof(RAXianShiBaoDaiParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2103 ���� - ���޽���  //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_SHEN_SHOU_JIANG_SHOU_MAX_ITEM = 20;

struct RAShenShouJiangLinParam
{
	RAShenShouJiangLinParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		for (int i = 0; i < RA_SHEN_SHOU_JIANG_SHOU_MAX_ITEM; ++i)
		{
			ra_day_buy_times[i] = 0;
			ra_buy_times[i] = 0;
		}

		ArrayCallReset(bao_di_info_list);
	}

	unsigned int ra_begin_timestamp;
	unsigned short ra_day_buy_times[RA_SHEN_SHOU_JIANG_SHOU_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
	unsigned short ra_buy_times[RA_SHEN_SHOU_JIANG_SHOU_MAX_ITEM];				//ÿ����Ʒ�޹�����
	BaoDiInfoItem bao_di_info_list[BAO_DI_INFO_ITEM_MAX_NUM];
};

UNSTD_STATIC_CHECK(sizeof(RAShenShouJiangLinParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2107 ���� - ���յǷ� //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_CHUN_RI_DENG_FENG_GET_ITEM_NUM = 16;
const int static RA_CHUN_RI_DENG_FENG_MAX_LEVEL = 6;

struct RAChunRiDengFengParam
{
	RAChunRiDengFengParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		layer = 0;
		explore_total_times = 0;
		explore_times = 0;
		get_item_flag = 0;
	}

	unsigned int ra_begin_timestamp;
	unsigned int layer;
	unsigned int explore_total_times;		//̽���ܴ���							
	unsigned short explore_times;		//û������߲�ô������������ñ�ô���ֱ�ӵ����߲�
	unsigned short get_item_flag;		//��7�����ֻ�ɻ��һ�Σ������ظ���ã���¼���߲��ȡʲô����	
};


UNSTD_STATIC_CHECK(RA_CHUN_RI_DENG_FENG_GET_ITEM_NUM <= (int)(sizeof( ((RAChunRiDengFengParam *)0)->get_item_flag) * 8))

UNSTD_STATIC_CHECK(sizeof(RAChunRiDengFengParam) <= sizeof(RoleActivityRoleData))
//////////////////////////////////////////// 2109 ���� - ��Ϻ��� //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_ZU_HE_HE_LI_MAX_ITEM = 60;

struct RAZuHeHeLiBuyTimes
{
	RAZuHeHeLiBuyTimes() { this->Reset(); }
	void Reset()
	{
		ra_day_buy_times = 0;
		ra_buy_times = 0;
	}

	unsigned short ra_day_buy_times;			//ÿ����Ʒÿ���޹�����
	unsigned short ra_buy_times;				//ÿ����Ʒ�޹�����
};

struct RAZuHeHeLiParam
{
	RAZuHeHeLiParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;

		for (int i = 0; i < RA_ZU_HE_HE_LI_MAX_ITEM; ++i)
		{
			buy_times[i].Reset();
		}
	}

	unsigned int ra_begin_timestamp;
	RAZuHeHeLiBuyTimes buy_times[RA_ZU_HE_HE_LI_MAX_ITEM];
};

UNSTD_STATIC_CHECK(sizeof(RAZuHeHeLiParam) <= sizeof(RoleActivityRoleData))
//////////////////////////////////////////// 2112 ���� - ������� //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_LING_PAI_TIAN_CI_MAX_ITEM = 32;

struct RALingPaiTianCiParam
{
	RALingPaiTianCiParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		chong_zhi_num = 0;
		fetch_flag = 0;
	}

	unsigned int ra_begin_timestamp;
	unsigned int chong_zhi_num;
	unsigned int fetch_flag;
};

UNSTD_STATIC_CHECK(sizeof(RALingPaiTianCiParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2113 ���� - ʱװ�̵� //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_FASHION_SHOP_MAX_ITEM = 64;

struct RAFashionShopParam
{
	RAFashionShopParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;

		for (int i = 0; i < ARRAY_LENGTH(buy_times); ++i)
		{
			buy_times[i] = 0;
		}
	}

	unsigned int ra_begin_timestamp;
	unsigned int buy_times[RA_FASHION_SHOP_MAX_ITEM];
};

UNSTD_STATIC_CHECK(sizeof(RAFashionShopParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2114 ���� - 1Ԫ�� //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RAOneYuanGouParam
{
	RAOneYuanGouParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		buy_times = 0;
	}

	unsigned int ra_begin_timestamp;
	int buy_times;
};

UNSTD_STATIC_CHECK(sizeof(RAOneYuanGouParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2115 ���� - �������  //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_LING_YU_JI_JIN_MAX_ITEM = 20;			//���ܳ���32��RALingYuJinJiParam::fetch_flag
const int static RA_LING_YU_JI_JIN_MAX_PHASE = 8;			//���ܳ���32��RALingYuJinJiParam::active_flag

struct RALingYuJinJiParam
{
	RALingYuJinJiParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		active_flag = 0;
		for (int i = 0; i < RA_LING_YU_JI_JIN_MAX_PHASE; ++i)
		{
			fetch_flag[i] = 0;
		}
	}

	unsigned int ra_begin_timestamp;
	int active_flag;				//�����ĸ��׶εı�־
	int fetch_flag[RA_LING_YU_JI_JIN_MAX_PHASE];		//ÿ���׶���ȡ������־
};

UNSTD_STATIC_CHECK(sizeof(RALingYuJinJiParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2116 ���� - �ķ�����(�������2) //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_LING_PAI_TIAN_CI_2_MAX_ITEM = 32;

struct RALingPaiTianCi2Param
{
	RALingPaiTianCi2Param() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		chong_zhi_num = 0;
		fetch_flag = 0;
	}

	unsigned int ra_begin_timestamp;
	unsigned int chong_zhi_num;
	unsigned int fetch_flag;
};

UNSTD_STATIC_CHECK(sizeof(RALingPaiTianCi2Param) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2130 ���� - ��������(�������3) //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_LING_PAI_TIAN_CI_3_MAX_ITEM = 32;

struct RALingPaiTianCi3Param
{
	RALingPaiTianCi3Param() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		chong_zhi_num = 0;
		fetch_flag = 0;
	}

	unsigned int ra_begin_timestamp;
	unsigned int chong_zhi_num;
	unsigned int fetch_flag;
};

UNSTD_STATIC_CHECK(sizeof(RALingPaiTianCi3Param) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2128 ���� - ��Ʒ����  //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_ZHEN_PIN_SHANG_PU_MAX_ITEM = 100;

struct RAZhenPinShangPuBuyTimes
{
	RAZhenPinShangPuBuyTimes() { this->Reset(); }
	void Reset()
	{
		type =0;
		buy_times = 0;
	}

	short type;  //���ñ��type
	short buy_times;  //���ñ��type
};

struct RAZhenPinShangPuParam
{
	RAZhenPinShangPuParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;

		for (int i = 0; i < ARRAY_LENGTH(ra_day_buy_times); ++i)
		{
			ra_day_buy_times[i].Reset();
		}

		for (int i = 0; i < ARRAY_LENGTH(ra_buy_times); ++i)
		{
			ra_buy_times[i].Reset();
		}
	}

	unsigned int ra_begin_timestamp;
	RAZhenPinShangPuBuyTimes ra_day_buy_times[RA_ZHEN_PIN_SHANG_PU_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
	RAZhenPinShangPuBuyTimes ra_buy_times[RA_ZHEN_PIN_SHANG_PU_MAX_ITEM];				//ÿ����Ʒ�޹�����
};

UNSTD_STATIC_CHECK(sizeof(RAZhenPinShangPuParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2161 ���� - ���Ʊ���2  //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_DING_ZHI_BAO_XIANG_2_MAX_ITEM = 20;
const int static RA_DING_ZHI_BAO_XIANG_2_BUY_MAX_ITEM = 32;
UNSTD_STATIC_CHECK(RA_DING_ZHI_BAO_XIANG_2_BUY_MAX_ITEM >= RA_DING_ZHI_BAO_XIANG_2_MAX_ITEM);
struct RADingZhiBaoXiang2Param
{
	RADingZhiBaoXiang2Param() { this->Reset(); }
	void Reset()
	{
		ra_ding_zhi_bao_xiang_begin_timestamp = 0;
		has_free_reward = 0;

		for (int i = 0; i < RA_DING_ZHI_BAO_XIANG_2_MAX_ITEM; ++i)
		{
			ra_ding_zhi_bao_xiang_day_buy_times[i] = 0;
			ra_ding_zhi_bao_xiang_buy_times[i] = 0;
		}
		buy_flag.Reset();
	}

	unsigned int ra_ding_zhi_bao_xiang_begin_timestamp;
	unsigned short ra_ding_zhi_bao_xiang_day_buy_times[RA_DING_ZHI_BAO_XIANG_2_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
	unsigned short ra_ding_zhi_bao_xiang_buy_times[RA_DING_ZHI_BAO_XIANG_2_MAX_ITEM];				//ÿ����Ʒ�޹�����
	BitMap<RA_DING_ZHI_BAO_XIANG_2_BUY_MAX_ITEM> buy_flag;										//�����ʶ 0:δ���� 1:�ѹ���
	int has_free_reward;													//��ѱ��䣬0-δ��ȡ��1-����ȡ
};
UNSTD_STATIC_CHECK(sizeof(RADingZhiBaoXiang2Param) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2162 ���� - ��ʱ����2 //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_XIAN_SHI_BAO_HE_2_MAX_ITEM = 20;
const int static RA_XIAN_SHI_BAO_HE_2_BUY_MAX_ITEM = 32;
UNSTD_STATIC_CHECK(RA_XIAN_SHI_BAO_HE_2_BUY_MAX_ITEM >= RA_XIAN_SHI_BAO_HE_2_MAX_ITEM);
struct RAXianShiBaoHe2Param
{
	RAXianShiBaoHe2Param() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		has_free_reward = 0;

		for (int i = 0; i < RA_XIAN_SHI_BAO_HE_2_MAX_ITEM; ++i)
		{
			ra_day_buy_times[i] = 0;
			ra_buy_times[i] = 0;
		}
		buy_flag.Reset();
	}

	unsigned int ra_begin_timestamp;
	unsigned short ra_day_buy_times[RA_XIAN_SHI_BAO_HE_2_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
	unsigned short ra_buy_times[RA_XIAN_SHI_BAO_HE_2_MAX_ITEM];				//ÿ����Ʒ�޹�����
	BitMap<RA_XIAN_SHI_BAO_HE_2_BUY_MAX_ITEM> buy_flag;						//�����ʶ 0:δ���� 1:�ѹ���
	int has_free_reward;													//��ѱ��䣬0-δ��ȡ��1-����ȡ
};

UNSTD_STATIC_CHECK(sizeof(RAXianShiBaoHe2Param) <= sizeof(RoleActivityRoleData))
//////////////////////////////////////////// 2163 ���� - ��ʱ����2 //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_XIAN_SHI_BAO_DAI_2_MAX_ITEM = 20;
const int static RA_XIAN_SHI_BAO_DAI_2_BUY_MAX_ITEM = 32;
UNSTD_STATIC_CHECK(RA_XIAN_SHI_BAO_DAI_2_BUY_MAX_ITEM >= RA_XIAN_SHI_BAO_DAI_2_MAX_ITEM);
struct RAXianShiBaoDai2Param
{
	RAXianShiBaoDai2Param() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		can_buy_seq = 0;
		for (int i = 0; i < RA_XIAN_SHI_BAO_DAI_MAX_ITEM; ++i)
		{
			ra_day_buy_times[i] = 0;
			ra_buy_times[i] = 0;
		}
		buy_flag.Reset();
	}

	unsigned int ra_begin_timestamp;
	int can_buy_seq;							//����������seq��ֻ�й����ǰһ�������������
	unsigned short ra_day_buy_times[RA_XIAN_SHI_BAO_DAI_2_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
	unsigned short ra_buy_times[RA_XIAN_SHI_BAO_DAI_2_MAX_ITEM];				//ÿ����Ʒ�޹�����
	BitMap<RA_XIAN_SHI_BAO_DAI_2_BUY_MAX_ITEM> buy_flag;						//�����ʶ 0:δ���� 1:�ѹ���
	int has_free_reward;													//��ѱ��䣬0-δ��ȡ��1-����ȡ
};

UNSTD_STATIC_CHECK(sizeof(RAXianShiBaoDai2Param) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2182 ���� - �������� //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
namespace RARongLianYouLi
{
	static const int RONG_LIAN_YOU_LI_MAX_TASK_NUM = 12;		// �����������
	UNSTD_STATIC_CHECK(RONG_LIAN_YOU_LI_MAX_TASK_NUM % 2 == 0)

	static const int RONG_LIAN_YOU_LI_MAX_BITMAP = 32;

};

struct RARongLianYouLiParam
{
	RARongLianYouLiParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		total_task_finish = 0;
		reserved_sh = 0;
		ra_activity_end_stamp = 0;

		has_task_stage_reward.Reset();

		memset(task_score, 0, sizeof(task_score));
		memset(reward_receive_num, 0, sizeof(reward_receive_num));
	}

	unsigned int ra_begin_timestamp;
	unsigned int ra_activity_end_stamp;		// �����ʱ������жϻ�����Ƿ����δ��ȡ����

	// �ۼ��������
	short total_task_finish;			// �������������
	short reserved_sh;					// Ԥ��
	BitMap<RARongLianYouLi::RONG_LIAN_YOU_LI_MAX_BITMAP> has_task_stage_reward;	// ����׶ν����Ƿ�����ȡ

	// �б�����
	short task_score[RARongLianYouLi::RONG_LIAN_YOU_LI_MAX_TASK_NUM];			// �������[0-11]
	short reward_receive_num[RARongLianYouLi::RONG_LIAN_YOU_LI_MAX_TASK_NUM];	// ��������ȡ����[0-11]
};
UNSTD_STATIC_CHECK(sizeof(RARongLianYouLiParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2191 ���� - ��������  //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
struct RAJinYiTianXiangParam
{
	RAJinYiTianXiangParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		draw_times = 0;
		get_reward_flag = 0;
		mark = 0;
		has_draw_value = 0;
	}

	unsigned int ra_begin_timestamp;
	int draw_times;
	unsigned int get_reward_flag;		
	int mark;
	int has_draw_value;
};
UNSTD_STATIC_CHECK(sizeof(RAJinYiTianXiangParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2195 ���� - Ԫ���̻�  //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
struct RAYuanQiYanHuoParam
{
	RAYuanQiYanHuoParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		is_buy = 0;
		is_fetch_free_reward = 0;
		get_reward_flag = 0;
	}

	unsigned int ra_begin_timestamp;
	int is_buy;
	int is_fetch_free_reward;
	unsigned int get_reward_flag;
};
UNSTD_STATIC_CHECK(sizeof(RAYuanQiYanHuoParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2200 ���� - ������ѡ  //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

const static int DUAN_YANG_ZI_XUAN_BUY_ITEM_MAX_NUM = 32;
struct RADuanYangZiXuanParam
{
	RADuanYangZiXuanParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		is_buy_reward = 0;
		fetch_reward_flag = 0;
		ArrayFillZero(select_reward_seq);
	}

	unsigned int ra_begin_timestamp;
	int is_buy_reward;
	unsigned int fetch_reward_flag;
	unsigned char select_reward_seq[DUAN_YANG_ZI_XUAN_BUY_ITEM_MAX_NUM];  //ֻ�Ǽ�¼���ͻ��˿�ѡ���ĸ����
};

UNSTD_STATIC_CHECK((int)(sizeof(((RADuanYangZiXuanParam *)0)->fetch_reward_flag) * 8) <= DUAN_YANG_ZI_XUAN_BUY_ITEM_MAX_NUM)
UNSTD_STATIC_CHECK(sizeof(RADuanYangZiXuanParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2207 ���� - ��������  //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const static int DUAN_YANG_FU_LI_BUY_ITEM_MAX_NUM = 20;

struct RADuanYangFuLiItem
{
	RADuanYangFuLiItem() { this->Reset(); }
	void Reset()
	{
		is_buy = 0;
		fetch_reward_times = 0;
	}

	short is_buy;		//�Ƿ�����
	short fetch_reward_times;  //��ȡ��������
};

struct RADuanYangFuLiParam
{
	RADuanYangFuLiParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		ArrayCallReset(item_list);
	}

	unsigned int ra_begin_timestamp;
	RADuanYangFuLiItem item_list[DUAN_YANG_FU_LI_BUY_ITEM_MAX_NUM];
};

UNSTD_STATIC_CHECK(sizeof(RADuanYangFuLiParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2208 ���� - �����۱� //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const static int JIN_LONG_JU_BAO_SELECT_NUM_SELECT = 4;		//��ѡ��������
const static int JIN_LONG_JU_BAO_SELECT_NUM_RAND = 8;		//����ѡ��������
const static int JIN_LONG_JU_BAO_MAX_BUY_ITEM = 20;		//
const static int JIN_LONG_JU_BAO_RECORD_NUM = 20;		//
		
struct RAJinLongJuBaoItem
{
	RAJinLongJuBaoItem() { this->Reset(); }
	void Reset()
	{
		is_get = 0;
		is_select = 0;
		reserved_sh = 0;
		reward_group_id = 0;
		reward_seq = 0;
	}

	unsigned char is_get;
	unsigned char is_select;
	unsigned short reserved_sh;
	int reward_group_id;
	int reward_seq;
};

struct RAJinLongJuBaoRecord
{
	RAJinLongJuBaoRecord() { this->Reset(); }
	void Reset()
	{
		time = 0;
		times = 0;		
		item_id = 0;
		num = 0;
	}

	unsigned int time;
	int times;		//��������
	unsigned short item_id;
	unsigned short num;
};

struct RAJinLongJuBaoParam
{
	RAJinLongJuBaoParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		ArrayFillZero(ra_day_buy_times);
		ArrayFillZero(ra_buy_times);
		draw_times = 0;
		times_reward_fetch_flag = 0;
		ArrayCallReset(item_list_select);
		ArrayCallReset(item_list_rand);
		record_index = 0;
		ArrayCallReset(record_list);
		ArrayCallReset(bao_di_info_list);
	}

	unsigned int ra_begin_timestamp;
	unsigned short ra_day_buy_times[JIN_LONG_JU_BAO_MAX_BUY_ITEM];		//20 ÿ����Ʒÿ���޹�����
	unsigned short ra_buy_times[JIN_LONG_JU_BAO_MAX_BUY_ITEM];			//20 ÿ����Ʒ�޹�����
	int draw_times;						//   �齱����
	unsigned int times_reward_fetch_flag;					//   ����������ȡ��־
	RAJinLongJuBaoItem item_list_select [JIN_LONG_JU_BAO_SELECT_NUM_SELECT] ;	//4 ��ѡ�����б�
	RAJinLongJuBaoItem item_list_rand[JIN_LONG_JU_BAO_SELECT_NUM_RAND];		//8 ����ѡ�����б�
	int record_index;
	RAJinLongJuBaoRecord record_list[JIN_LONG_JU_BAO_RECORD_NUM];
	BaoDiInfoItem bao_di_info_list[BAO_DI_INFO_ITEM_MAX_NUM];
};

UNSTD_STATIC_CHECK(sizeof(RADuanYangFuLiParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2210 ���� - ���޶��� //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

const static int SHEN_SHOU_EN_ZE_MAX_BUY_ITEM = 20;		//
const static int SHEN_SHOU_EN_ZE_RECORD_MAX_NUM = 30;		//

struct RAShenShouEnZeRecord
{
	RAShenShouEnZeRecord() { this->Reset(); }
	void Reset()
	{
		time = 0;
		times = 0;
		item_id = 0;
		num = 0;
	}

	unsigned int time;
	int times;		//��������
	unsigned short item_id;
	unsigned short num;
};

struct RAShenShouEnZeParam
{
	RAShenShouEnZeParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		ArrayFillZero(ra_day_buy_times);
		ArrayFillZero(ra_buy_times);
		draw_times = 0;
		is_select = 0;
		select_reward_seq = 0;
		record_index = 0;
		ArrayCallReset(record_list);
	}

	unsigned int ra_begin_timestamp;
	unsigned short ra_day_buy_times[SHEN_SHOU_EN_ZE_MAX_BUY_ITEM];		//20 ÿ����Ʒÿ���޹�����
	unsigned short ra_buy_times[SHEN_SHOU_EN_ZE_MAX_BUY_ITEM];			//20 ÿ����Ʒ�޹�����
	int draw_times;						//   �齱����
	int is_select;					//�Ƿ�ѡ������Ʒ
	int select_reward_seq;				//ѡ����Ʒ��seq
	int record_index;
	RAShenShouEnZeRecord record_list[SHEN_SHOU_EN_ZE_RECORD_MAX_NUM];
};

UNSTD_STATIC_CHECK(sizeof(RAShenShouEnZeParam) <= sizeof(RoleActivityRoleData))



//////////////////////////////////////////// 2214 ���� - ��ʱ����3  (�ļ�����)//////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_XIAN_SHI_BAO_HE_3_MAX_ITEM = 20;

struct RAXianShiBaoHe3Param
{
	RAXianShiBaoHe3Param() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;	
		for (int i = 0; i < RA_XIAN_SHI_BAO_HE_3_MAX_ITEM; ++i)
		{
			ra_day_buy_times[i] = 0;
			ra_buy_times[i] = 0;
		}

		has_free_reward = 0;
	}

	unsigned int ra_begin_timestamp;
	unsigned short ra_day_buy_times[RA_XIAN_SHI_BAO_HE_3_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
	unsigned short ra_buy_times[RA_XIAN_SHI_BAO_HE_3_MAX_ITEM];				//ÿ����Ʒ�޹�����
	int has_free_reward;							//��ѱ��䣬0-δ��ȡ��1-����ȡ
};

UNSTD_STATIC_CHECK(sizeof(RAXianShiBaoHe3Param) <= sizeof(RoleActivityRoleData))


//////////////////////////////////////////// 2215 ���� - ��ʱ����4  (�ļ�����)//////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_XIAN_SHI_BAO_HE_4_MAX_ITEM = 20;

struct RAXianShiBaoHe4Param
{
	RAXianShiBaoHe4Param() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		for (int i = 0; i < RA_XIAN_SHI_BAO_HE_4_MAX_ITEM; ++i)
		{
			ra_day_buy_times[i] = 0;
			ra_buy_times[i] = 0;
		}

		has_free_reward = 0;
	}

	unsigned int ra_begin_timestamp;
	unsigned short ra_day_buy_times[RA_XIAN_SHI_BAO_HE_4_MAX_ITEM];			//ÿ����Ʒÿ���޹�����
	unsigned short ra_buy_times[RA_XIAN_SHI_BAO_HE_4_MAX_ITEM];				//ÿ����Ʒ�޹�����
	int has_free_reward;							//��ѱ��䣬0-δ��ȡ��1-����ȡ
};

UNSTD_STATIC_CHECK(sizeof(RAXianShiBaoHe4Param) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2216 ���� - ���Ʊ���3 �ļ�����  //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const int static RA_DING_ZHI_BAO_XIANG_3_MAX_ITEM = 20;
struct RADingZhiBaoXiang3Param
{
	RADingZhiBaoXiang3Param() { this->Reset(); }
	void Reset()
	{
		ra_ding_zhi_bao_xiang_begin_timestamp = 0;
	
		for (int i = 0; i < RA_DING_ZHI_BAO_XIANG_3_MAX_ITEM; ++i)
		{
			ra_ding_zhi_bao_xiang_day_buy_times[i] = 0;
			ra_ding_zhi_bao_xiang_buy_times[i] = 0;
		}

		has_free_reward = 0;
	}

	unsigned int ra_ding_zhi_bao_xiang_begin_timestamp;
	unsigned short ra_ding_zhi_bao_xiang_day_buy_times[RA_DING_ZHI_BAO_XIANG_3_MAX_ITEM];		//ÿ����Ʒÿ���޹�����
	unsigned short ra_ding_zhi_bao_xiang_buy_times[RA_DING_ZHI_BAO_XIANG_3_MAX_ITEM];		//ÿ����Ʒ�޹�����
	int has_free_reward;								//��ѱ��䣬0-δ��ȡ��1-����ȡ
};
UNSTD_STATIC_CHECK(sizeof(RADingZhiBaoXiang2Param) <= sizeof(RoleActivityRoleData))

//�����ۻ����¼ӵķŶ����ļ���

#pragma pack(pop)

#endif	
