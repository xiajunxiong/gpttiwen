#ifndef __ROLE_COMMON_SAVE_DEF_H__
#define __ROLE_COMMON_SAVE_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

//���⣬���ҪС��,����ֱ����������������
enum ROLE_COMMON_SAVE_DATA_TYPE
{
	ROLE_COMMON_SAVE_DATA_TYPE_INVALID,
	ROLE_COMMON_SAVE_DATA_TYPE_ZU_HE_HE_LI = 1,				//��Ϻ���
	ROLE_COMMON_SAVE_DATA_TYPE_FASHION_SHOP = 2,			//ʱװ�̵�
	ROLE_COMMON_SAVE_DATA_TYPE_CUMULATIVE_RECHARGE = 3,		//�ۼƳ�ֵ(��������)
	ROLE_COMMON_SAVE_DATA_TYPE_ONE_YUAN_GOU = 4,			//һԪ��
	ROLE_COMMON_SAVE_DATA_TYPE_LING_YU_JIN_JI = 5,			//�������
	ROLE_COMMON_SAVE_DATA_TYPE_SHEN_SHOU_JIANG_LIN = 6,		//���޽���
	ROLE_COMMON_SAVE_DATA_TYPE_LUCKY_BAG = 7,				//����
	ROLE_COMMON_SAVE_DATA_TYPE_CAI_SHEN_JU_BAO = 8,			//����۱�
	ROLE_COMMON_SAVE_DATA_TYPE_DING_ZHI_BAO_XIANG = 9,		//���Ʊ���
	ROLE_COMMON_SAVE_DATA_TYPE_ZHUAN_SHU_LI_BAO = 10,		//ר�����
	ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE = 11,		//��ʱ����
	ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_DAI = 12,		//��ʱ����
	ROLE_COMMON_SAVE_DATA_TYPE_MYSTERIOUS_TRIAL = 13,		//��������
	ROLE_COMMON_SAVE_DATA_TYPE_MONTH_INVESTMENT = 14,		//�¶�Ͷ��
	ROLE_COMMON_SAVE_DATA_TYPE_XUN_BAO	= 15,				//Ѱ��
	ROLE_COMMON_SAVE_DATA_TYPE_PET_TRAIN_2 = 16,			//������������2
	ROLE_COMMON_SAVE_DATA_TYPE_PARTNER_TRAIN = 17,			//�����������
	ROLE_COMMON_SAVE_DATA_TYPE_ZHUAN_SHU_LI_BAO_DAY = 18,	//ר�����-ÿ�����
	ROLE_COMMON_SAVE_DATA_TYPE_FIRST_RECHARGE_EXCLUSIVE = 19,	//�׳�ר��-�ɳ�����
	ROLE_COMMON_SAVE_DATA_TYPE_MONTH_CARD = 20,					//�¿�/�ܿ�/�����̳�
	ROLE_COMMON_SAVE_DATA_TYPE_GIVE_ITEM = 21,					//������Ʒ
	ROLE_COMMON_SAVE_DATA_TYPE_SUMMER_STORE = 22,			//�����̳�
	ROLE_COMMON_SAVE_DATA_TYPE_SUPER_OFF_SHOP = 23,			//�����ۿ��̳�
	ROLE_COMMON_SAVE_DATA_TYPE_ZHEN_PIN_SHANG_PU = 24,			// ��Ʒ�̵�
	ROLE_COMMON_SAVE_DATA_TYPE_LUCK_BAG_BATCH = 25,			// ����
	ROLE_COMMON_SAVE_DATA_TYPE_WAN_LING_GONG_YING = 26,			// ���鹲ӯ
	ROLE_COMMON_SAVE_DATA_TYPE_HORCRUX_TIANCHENG = 27,			// �������
	ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_MET = 28,			// ɽ������
	ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_MEET = 29,			// ɽ�����
	ROLE_COMMON_SAVE_DATA_TYPE_ZHAOCAIJINBAO = 30,			//�вƽ���
	ROLE_COMMON_SAVE_DATA_TYPE_YUN_ZE_SLOT_MACHINE =31,		// ����ͬ����
	ROLE_COMMON_SAVE_DATA_TYPE_CHAOZHI_FENXIANG = 32,		// ��ֵ����
	ROLE_COMMON_SAVE_DATA_TYPE_COLORFUL_DOUBLE = 33,		// ����ӱ�
	ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_BAO_XIANG = 34,			//ɽ������
	ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_BAO_HE = 35,			//ɽ������
	ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_BAO_DAI = 36,			//ɽ������
	ROLE_COMMON_SAVE_DATA_TYPE_DING_ZHI_BAO_XIANG_2 = 37,		//���Ʊ���2
	ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE_2 = 38,		//��ʱ����2
	ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_DAI_2 = 39,		//��ʱ����2
	ROLE_COMMON_SAVE_DATA_TYPE_JIXING_GAOZHAO = 40,			// ���Ǹ���
	ROLE_COMMON_SAVE_DATA_TYPE_WEEKEND_GIFT = 41,			// ��ĩ���
	ROLE_COMMON_SAVE_DATA_TYPE_CHAO_ZHI_GIFT = 42,			// ��ֵ����
	ROLE_COMMON_SAVE_DATA_TYPE_YINHU_WENSHI = 43,			// ��������
	ROLE_COMMON_SAVE_DATA_TYPE_YIN_HU_BAI_SUI = 44,			// ��������
	ROLE_COMMON_SAVE_DATA_TYPE_JINGJI_YUEKA = 45,			// �����¿�
	ROLE_COMMON_SAVE_DATA_TYPE_QING_YUAN_SHI_ZHUANG = 46,	// ��Եʱװ
	ROLE_COMMON_SAVE_DATA_TYPE_YAO_XING_SHI_LIAN = 47,		// ��������
	ROLE_COMMON_SAVE_DATA_TYPE_GOD_BEAST_ADVENT = 48,		// ��������
	ROLE_COMMON_SAVE_DATA_TYPE_WAN_LING_ZHU_LI = 49,		// ��������
	ROLE_COMMON_SAVE_DATA_TYPE_JIN_YI_TIAN_XIANG = 50,		// ��������
	ROLE_COMMON_SAVE_DATA_TYPE_YUAN_QI_YAN_HUO = 51,		// Ԫ���̻�
	ROLE_COMMON_SAVE_DATA_TYPE_YUAN_QI_JISHIBU = 52,		// Ԫ�����²�
	ROLE_COMMON_SAVE_DATA_TYPE_DUAN_YANG_ZI_XUAN = 53,		// ������ѡ
	ROLE_COMMON_SAVE_DATA_TYPE_LUCKY_BLIND_BOX = 54,		// ����ä����ѡ
	ROLE_COMMON_SAVE_DATA_TYPE_DUAN_YANG_FU_LI = 55,		// ��������
	ROLE_COMMON_SAVE_DATA_TYPE_JIN_LONG_JU_BAO = 56,		// �����۱�
	ROLE_COMMON_SAVE_DATA_TYPE_SHEN_SHOU_EN_ZE = 57,		// ���޶���
	ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE_3 = 58,		// ��ʱ����3 �ļ�����
	ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE_4 = 59,		// ��ʱ����4 �ļ�����
	ROLE_COMMON_SAVE_DATA_TYPE_PET_EXCHANGE_SHOP = 60,		// ����һ��̵�
	ROLE_COMMON_SAVE_DATA_TYPE_QI_XI_TE_HUI = 61,			// ��Ϧ�ػ�
	ROLE_COMMON_SAVE_DATA_TYPE_DING_ZHI_BAO_XIANG_3 = 62,	// ���Ʊ���3 �ļ�����
	ROLE_COMMON_SAVE_DATA_TYPE_XIN_FU_LING_CHONG = 63,		// �·����
	ROLE_COMMON_SAVE_DATA_TYPE_SHEN_CI_HUA_FU = 64,			// ��ͻ���
	ROLE_COMMON_SAVE_DATA_TYPE_TIAN_YI_YAO_SHI = 65,		// ����ҫ��
	ROLE_COMMON_SAVE_DATA_TYPE_NEW_COURAGE_CHALLENGE = 66,	// ߱��֮��ս��
	ROLE_COMMON_SAVE_DATA_TYPE_ZHUI_YUE_SHANG_DIAN = 67,		// ׷���̵�
	ROLE_COMMON_SAVE_DATA_TYPE_DAN_BI_FAN_LI = 68,			// ���ʷ���

	ROLE_COMMON_SAVE_DATA_TYPE_MAX,
};
//���⣬���ҪС��,����ֱ����������������

unsigned int GetRoleCommonSaveDataLengthByType(int save_type);

#pragma pack(push, 4)
//---------------------�����Ǵ��ṹ��ע�����--------------------------------------

const static int CMD_BUY_ITEM_MAX_NUM = 16;
const static int CMD_BUY_ITEM_WAIT_TIME = 120;
struct RoleCommonSaveGameItem  //��Ϸ��Ʒ
{
	void Reset();

	unsigned short item_id;
	 short is_bind;
	int num;
};
//-----------------------ֱ������
struct RoleCommonSaveCmdBuyItem
{
	void Reset();

	int param1;
	int param2;
	int param3;
	int param4;
	int cfg_vesion;
	int need_gold;	
	int rewards_num;
	unsigned int create_time;
	unsigned int ra_begin_timestamp;
	long long unique_id;
	int param5;
	unsigned char reserved_ch[12];
	RoleCommonSaveGameItem rewards_item[CMD_BUY_ITEM_MAX_NUM];
};
//-------------Ѱ������------------------
struct RoleCommonSaveXunBao
{
	void Reset();

	int draw_type;
	int xi_you_floor_curr_times;
	unsigned int free_time;
};

//-----------������������
struct RoleCommonSavePetTrainData2
{
	void Reset()
	{
		level = 0;
		reserve_ush = 0;
		exp = 0;
	}

	unsigned short		level;
	unsigned short		reserve_ush;
	int				exp;
};

//-----------�����������
struct RoleCommonSavePartnerTrainData
{
	void Reset()
	{
		level = 0;
		reserve_ush = 0;
		exp = 0;
	}

	unsigned short		level;
	unsigned short		reserve_ush;
	int				exp;
};

//-------------����------------------
struct RoleCommonSaveRoleGiveGiftData
{
	void Reset();

	int give_times;
};


union RoleCommonSaveData
{
	void Reset(int save_type);

	RoleCommonSaveCmdBuyItem  cmd_buy_item_data;
	RoleCommonSaveXunBao  xun_bao_data;
	RoleCommonSavePetTrainData2 pet_train_data2;
	RoleCommonSavePartnerTrainData partner_train_data;
	RoleCommonSaveRoleGiveGiftData role_give_gift_data;
};

typedef char RoleCommonSaveDataHex[sizeof(RoleCommonSaveData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleCommonSaveDataHex) < 1024);

#ifdef _MSC_VER
static_assert(std::is_pod<RoleCommonSaveData>::value, "RoleCommonSaveData is not pod type");
#endif

class RoleCommonSaveDataParam
{
public:
	bool Serialize(TLVSerializer & outstream) const;
	bool Unserialize(TLVUnserializer & instream);

	struct DBRoleCommonSaveDataItem
	{
		DBRoleCommonSaveDataItem()
		{
			this->Reset(0);
		}

		void Reset(int save_type)
		{
			change_state = 0;
			save_data_type = ROLE_COMMON_SAVE_DATA_TYPE_INVALID;
			param1 = 0;
			param2 = 0;

			data.Reset(save_type);
		}

		char change_state;
		int save_data_type;
		int param1;
		int param2;

		RoleCommonSaveData data;
	};

	int count;
	DBRoleCommonSaveDataItem data_list[5000];
};

#pragma pack(pop)

#endif
