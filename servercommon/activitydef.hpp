#ifndef __ACTIVITY_DEF_HPP__
#define __ACTIVITY_DEF_HPP__

#include <time.h>

#include "servercommon/servercommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "gameworld/gameworld/gamedef.h"
#include "servercommon/serverdef.h"
#include "servercommon/silkroadsdef.hpp"

static const int MAX_ONCE_ACTIVITY_ROLE_DATA_COUNT = 100;					// һ�β�����ɫ�����������
static const int INT_TYPE_BIT = 32;
static const int CLIENT_ACTIVITY_TYPE_BEGIN = 10000;
static const int MAX_ACTIVITY_TYPE_NUM = 64;


enum BUY_TIME_TYPE
{
	BUY_TIME_TYPE_LUCKY_BAG = 1,						//���򸣴�
	BUY_TIME_TYPE_MYSTERIOUS_TRIAL = 2,					//������������ÿ���ۿ� 
	BUY_TIME_TYPE_MONTH_PURCHASE = 3,					//�����¶�Ͷ��
	BUY_TIME_TYPE_GOD_TOKEN = 4,						//�������ս��
	BUY_TIME_TYPE_CAI_SHEN_JU_BAO = 5,					//�������۱�		param1 ����seq
	BUY_TIME_TYPE_DING_ZHI_BAO_XIANG = 6,				//�����Ʊ���	 param1 ����seq��param2 ѡ��ı��䰴λ��
	BUY_TIME_TYPE_ZHUAN_SHU_LI_BAO = 7,					//����ר�����  param1 �Э���·���type
	BUY_TIME_TYPE_XIAN_SHI_BAO_HE = 8,					//������ʱ����  param1 ����seq
	BUY_TIME_TYPE_XIAN_SHI_BAO_DAI = 9,					//������ʱ����  param1 ����seq
	BUY_TIME_TYPE_COW_EXCHANGE = 10,					//����ţ�˶һ�  param1 ����
	BUY_TIME_TYPE_SPRING_FAESTIVAL_STORE = 11,			//���������  param1 ����
	BUY_TIME_TYPE_COW_STORE = 12,						// ����ţ���̳� param1 ����
	BUY_TIME_TYPE_SECRET_TREASURE = 13,					// �����ػݹ屦 param1 ����
	BUY_TIME_TYPE_ZU_HE_HE_LI = 14,						// ��Ϻ��� param1 ��һ������ ����λ(bit),���õ�type
	BUY_TIME_TYPE_ZU_HE_HE_LI_2 = 15,					// ��Ϻ��� param1 (1251Э�� int1) ��param2 (1251Э�� int2)
	BUY_TIME_TYPE_FASHION_SHOP = 16,					// ʱװ�̵� param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_CUMULATIVE_RECHARGE = 17,				// �ۼƳ�ֵ(��������) param1 (1251Э�� int1)
	BUY_TIME_TYPE_ONE_YUAN_GOU = 18,					//һԪ�� param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_LING_YU_JI_JIN = 19,					//������� param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_SHEN_SHOU_JIANG_LIN = 20,				//���޽���ֱ�� param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_LUCKY_BAG_NEW = 21,					//���򸣴�	param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_CAI_SHEN_JU_BAO_NEW = 22,				//�������۱�		param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_DING_ZHI_BAO_XIANG_NEW = 23,			//�����Ʊ���	 param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_ZHUAN_SHU_LI_BAO_NEW = 24,			//����ר�����  param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_XIAN_SHI_BAO_HE_NEW = 25,				//������ʱ����  param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_XIAN_SHI_BAO_DAI_NEW = 26,			//������ʱ����  pparam1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_MYSTERIOUS_TRIAL_NEW = 27,			//������������ÿ���ۿ� param1 (1251Э�� int1) ��param2 (1251Э�� int2)
	BUY_TIME_TYPE_MONTH_PURCHASE_NEW = 28,				//�����¶�Ͷ�� param1 (1251Э�� int1) ��param2 (1251Э�� int2)
	BUY_TIME_TYPE_ZHUAN_SHU_LI_BAO_DAY_GIFT = 29,		//����ר�����-ÿ����� param1 (1251Э�� int1) ��param2 (1251Э�� int2)
	BUY_TIME_TYPE_FIRST_RECHARGE_EXCLUSIVE = 30,	//�����׳�ר��-�ɳ����� param1 (1251Э�� int1) ��param2 (1251Э�� int2)
	BUY_TIME_TYPE_MONTH_CARD = 31,					//�����ܿ�/�¿�/�����̳� param1 (1251Э�� int1) ��param2 (1251Э�� int2)
	BUY_TIME_TYPE_SUMMER_STORE = 32,					// �����̵� param1 (1251Э�� int1) ��param2 (1251Э�� int2)
	BUY_TIME_TYPE_SUPER_OFF_SHOP = 33,					// �����ۿ��̳� param1 (1251Э�� int1) ��param2 (1251Э�� int2)
	BUY_TIME_TYPE_ZHEN_PIN_SHANG_PU = 34,				// ��Ʒ���� param1 (1251Э�� int1) ��param2 (1251Э�� int2)
	BUY_TIME_TYPE_LUCKY_BAG_BATCH = 35,				// �������� ����N�� param1 (1251Э�� int1) ��param2 (1251Э�� int2)
	BUY_TIME_TYPE_WAN_LING_GONG_YING = 36,				// �������鹲ӯ���ȡ�����ʸ�
	BUY_TIME_TYPE_HORCRUX_TIANCHENG = 37,				//������������� param1: shop_index
	BUY_TIME_TYPE_SHAN_HAI_MET = 38,					// ����ɽ�������˫����ȡ�����ʸ�
	BUY_TIME_TYPE_SHAN_HAI_MEET = 39,					//���򼤻�ɽ�����
	BUY_TIME_TYPE_ZHAOCAIJINBAO = 40,					//�����вƽ������
	BUY_TIME_TYPE_YUN_ZE_SLOT_MACHINE = 41,				// �������ͬ����Կ��
	BUY_TYME_TYPE_CHAOZHI_FENXIANG = 42,				// ��ֵ����
	BUY_TIME_TYPE_COLORFUL_DOUBLE = 43,				// ����ӱ�		p_1:�������� p_2:box_seq
	BUY_TIME_TYPE_SHAN_HAI_BAO_XIANG = 44,			// ����ɽ������	 param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_SHAN_HAI_BAO_HE = 45,				// ����ɽ������	 param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_SHAN_HAI_BAO_DAI = 46,			// ����ɽ������	 param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_DING_ZHI_BAO_XIANG_2 = 47,		//�����Ʊ���2	 param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_XIAN_SHI_BAO_DAI_2 = 48,			//������ʱ����2	 param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_XIAN_SHI_BAO_HE_2 = 49,			//������ʱ����2	 param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_JIXING_GAOZHAO = 50,				// �����Ǹ��� param1: shop_index
	BUY_TIME_TYPE_WEEKEND_GIFT = 51,				// ��ĩ��� param1: gift_index
	BUY_TIME_TYPE_CHAO_ZHI_GIFT = 52,				// ��ֵ����
	BUY_TIME_TYPE_JINGJI_YUEKA = 53,				// ���򾺼��¿�  param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_YINHU_WENSHI = 54,					// ���� ��������
	BUY_TIME_TYPE_YIN_HU_BAI_SUI = 55,					// ���� ��������
	BUY_TIME_TYPE_QING_YUAN_SHIZHUANG = 56,			// ������Եʱװ param1:buy_index
	BUY_TIME_TYPE_YAO_XING_SHI_LIAN = 57,			// ������������ param1:seq param2:param1
	BUY_TIME_TYPE_GOD_BEAST_ADVENT = 58,				// ���� ��������
	BUY_TIME_TYPE_WAN_LING_ZHU_LI = 59,				// ��������������ȡ�����ʸ�
	BUY_TIME_TYPE_JIN_YI_TIAN_XIANG = 60,				// ���� ��������
	BUY_TIME_TYPE_YUAN_QI_YAN_HUO = 61,			//����Ԫ���̻�	 param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_YUAN_QI_JISHIBU = 62,				// ����Ԫ�����²���ȡ�����ʸ�
	BUY_TIME_TYPE_DUAN_YANG_ZI_XUAN = 63,			// ���������ѡ	 param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_LUCKY_BLIND_BOX = 64,				// ��������ä��	 param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_DUAN_YANG_FU_LI = 65,				// �����������	 param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_JIN_LONG_JU_BAO = 66,				// ��������۱�	 param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_SHEN_SHOU_EN_ZE = 67,				// �������޶���	 param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_XIAN_SHI_BAO_HE_3 = 68,			// ������ʱ����3 (�ļ�����)  param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_XIAN_SHI_BAO_HE_4 = 69,			// ������ʱ����4 (�ļ�����)  param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_PET_EXCHANGE_SHOP = 70,			// ����һ�-�̵�	 param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_QI_XI_TE_HUI = 71,				// ������Ϧ�ػ�	 param1 (1251Э�� int1) �� param2 (1251Э�� int2)	
	BUY_TIME_TYPE_DING_ZHI_BAO_XIANG_3 = 72,		// �����Ʊ���3 (�ļ�����)  param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_XIN_FU_LING_CHONG = 73,			// �����·����	 param1 (1251Э�� int1) �� param2 (1251Э�� int2)	
	BUY_TIME_TYPE_SHEN_CI_HUA_FU = 74,				// ������ͻ���   param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_TIAN_YI_YAO_SHI = 75,				// ��������ҫ�� param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_NEW_COURAGE_CHALLENGE = 76,		// ����߱��֮��ս��
	BUY_TIME_TYPE_ZHUI_YUE_SHANG_DIAN = 77,			// ����׷���̵� param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_DAN_BI_FAN_LI = 78,				// ���򵥱ʷ��� param1 (1251Э�� int1) �� param2 (1251Э�� int2)
	BUY_TIME_TYPE_MAX
	
};

enum BUY_GIFT_ITEM_RET_TYPE
{
	BUY_GIFT_ITEM_RET_TYPE_1 = 1,		//��������
	BUY_GIFT_ITEM_RET_TYPE_2 = 2,		//��������
};

enum BUY_GIFT_ITEM_TYPE
{
	BUY_GIFT_ITEM_TYPE_INVALID = 0,

	BUY_GIFT_ITEM_TYPE_LUCKY_BAG = 1,							//!< ���򸣴� param2 - seq
	BUY_GIFT_ITEM_TYPE_MYSTERIOUS_TRIAL = 2,					//!< ������������ÿ���ۿ� param3 - day
	BUY_GIFT_ITEM_TYPE_GOD_TOKEN = 3,							//!< �������ս�� 
	BUY_GIFT_ITEM_TYPE_CAI_SHEN_JU_BAO = 4,						//!< �������۱� param2 - seq
	BUY_GIFT_ITEM_TYPE_DING_ZHI_BAO_XIANG = 5,					//!< �����Ʊ��� param2 - seq
	BUY_GIFT_ITEM_TYPE_BUY_TIME_TYPE_ZHUAN_SHU_LI_BAO = 6,		//!< ����ר����� param2 - type(�޷�����Ӧ)
	BUY_GIFT_ITEM_TYPE_XIAN_SHI_BAO_HE = 7,						//!< ������ʱ���� param2 - seq
	BUY_GIFT_ITEM_TYPE_XIAN_SHI_BAO_DAI = 8,					//!< ������ʱ���� param2 - seq
	BUY_GIFT_ITEM_TYPE_ZU_HE_HE_LI = 9,							//!< ������Ϻ��� param2 - seq1 param3 - seq2	(param2����Ҫ��ֵ)
	BUY_GIFT_ITEM_TYPE_SPRING_FAESTIVAL_STORE = 10,				//!< ���������   param2 - seq
	BUY_GIFT_ITEM_TYPE_COW_STORE = 11,							//!< ����ţ���̳� param2 - seq
	BUY_GIFT_ITEM_TYPE_FASHION_SHOP = 12,						//!< ����ʱװ�̵� param2 - seq
	BUY_GIFT_ITEM_TYPE_LING_YU_JI_JIN = 13,						//!< ����������� param2 - phase
	BUY_GIFT_ITEM_TYPE_CUMULATIVE_RECHARG = 14,					//!< �����ۼƳ�ֵ(��װ����) 
	BUY_GIFT_ITEM_TYPE_MONTH_CARD = 15,							//!< �����¿�/�ܿ� param2 - ����P3 param3 - type(0:p2��card_type 1:p2����Ʒseq)
	BUY_GIFT_ITEM_TYPE_SHAN_HAI_BAO_XIANG = 16,					//!< ����ɽ������ param2 - seq
	BUY_GIFT_ITEM_TYPE_SHAN_HAI_BAO_DAI = 17,					//!< ����ɽ������ param2 - seq	
	BUY_GIFT_ITEM_TYPE_SHAN_HAI_BAO_HE = 18,					//!< ����ɽ������ param2 - seq
	BUY_GIFT_ITEM_TYPE_SHAN_HAI_MET = 19,						//!< ɽ������ 
	BUY_GIFT_ITEM_TYPE_SHAN_HAI_MEET = 20,						//!< ɽ�����
	BUY_GIFT_ITEM_TYPE_ZHAO_CAI_JIN_BAO = 21,					//!< �вƽ��� param2:seq
	BUY_GIFT_ITEM_TYPE_SHEN_SHOU_JIANG_LIN = 22,				//!< ���޽��� param2:seq
	BUY_GIFT_ITEM_TYPE_WAN_LING_GONG_YING = 23,					//!< ���鹲ӯ
	BUY_GIFT_ITEM_TYPE_HORCRUX_TIANCHENG = 24,					//!< ������� param2:shop_index
	BUY_GIFT_ITEM_TYPE_YUN_ZE_SLOT_MACHINE = 25,				//!< ����ͬ����
	BUY_GIFT_ITEM_TYPE_CHAOZHI_FENXIANG = 26,					//!< ��ֵ����
	BUY_GIFT_ITEM_TYPE_DING_ZHI_BAO_XIANG_2 = 27,				//!< ������(���Ʊ���2) param2:seq
	BUY_GIFT_ITEM_TYPE_XIAN_SHI_BAO_HE_2 = 28,					//!< ���󱦺�(��ʱ����2) param2:seq
	BUY_GIFT_ITEM_TYPE_XIAN_SHI_BAO_DAI_2 = 29,					//!< ���󱦴�(��ʱ����2) param2:seq
	BUY_GIFT_ITEM_TYPE_JIXIANG_GAOZHAO = 30,					//!< ���Ǹ��� param2:shop_index
	BUY_GIFT_ITEM_TYPE_CHAO_ZHI_GIFT = 31,						//!< ��ֵ����
	BUY_GIFT_ITEM_TYPE_YINHU_WENSHI = 32,						//!< ��������
	BUY_GIFT_ITEM_TYPE_YIN_HU_BAI_SUI = 33,						//!< ��������  
	BUY_GIFT_ITEM_TYPE_ZHUAN_SHU_LI_BAO_DAY_GIFT = 34,			//!< ר�����-ÿ����� param2 - phase param3:type
	BUY_GIFT_ITEM_TYPE_COLORFUL_DOUBLE = 35,					//!< �ͷ׼ӱ� P2 box_seq
	BUY_GIFT_ITEM_TYPE_YAO_XING_SHI_LIAN = 36,					//!< ��������  �� 1252 Э��p1 p2 ��ͬ
	BUY_GIFT_ITEM_TYPE_WANLING_ZHULI = 37,						//!< ��������
	BUY_GIFT_ITEM_TYPE_JIN_YI_TIAN_XIANG = 38,					//!< ���� ��������
	BUY_GIFT_ITEM_TYPE_YUAN_QI_YAN_HUO = 39,					//!< ����Ԫ���̻�	 
	BUY_GIFT_ITEM_TYPE_YUAN_QI_JISHIBU = 40,					//!< ����Ԫ�����²���ȡ�����ʸ�
	BUY_GIFT_ITEM_TYPE_LUCKY_BAG_BATCH = 41,					//!< �������� ����N�� param2:seq
	BUY_GIFT_ITEM_TYPE_CHONGZHI = 42,							//!< ��ֵ param2:chongzhi_num
	BUY_GIFT_ITEM_TYPE_FIRST_RECHARGE_EXCLUSIVE = 43,			//!< �׳�ר��-�ɳ����� param2:first_recharge_cmd_type
	BUY_GIFT_ITEM_TYPE_LUCKY_BLIND_BOX = 44,					//!< ����ä�� P2:seq
	BUY_GIFT_ITEM_TYPE_DUAN_YANG_FU_LI = 45,					//!< �������� P2:seq P3:is_buy_all
	BUY_GIFT_ITEM_TYPE_DUAN_YANG_ZI_XUAN = 46,					//!< ������ѡ
	BUY_GIFT_ITEM_TYPE_JIN_LONG_JU_BAO = 47,					//!< �����۱� P2:seq
	BUY_GIFT_ITEM_TYPE_SHEN_SHOU_EN_ZE = 48,					//!< �������޶��� P2:seq
	BUY_GIFT_ITEM_TYPE_XIAN_SHI_BAO_HE_3 = 49,					//!< ��ʱ����3 (�ļ�����) param2:seq
	BUY_GIFT_ITEM_TYPE_XIAN_SHI_BAO_HE_4 = 50,					//!< ��ʱ����4 (�ļ�����) param2:seq
	BUY_GIFT_ITEM_TYPE_PET_EXCHANGE_SHOP = 51,					//!< ����һ�-�̵� param2:seq
	BUY_GIFT_ITEM_TYPE_QI_XI_TE_HUI = 52,						//!< ��Ϧ�ػ� p2:seq
	BUY_GIFT_ITEM_TYPE_DING_ZHI_BAO_XIANG_3 = 53,				//!< ���Ʊ���3 (�ļ�����) param2 - seq
	BUY_GIFT_ITEM_TYPE_XIN_FU_LING_CHONG = 54,					//!< �·����
	BUY_GIFT_ITEM_TYPE_QING_YUAN_SHIZHUANG = 55,				//!< ������Եʱװ param2:buy_index
	BUY_GIFT_ITEM_TYPE_ZHEN_PIN_SHANG_PU = 56,					//!< ��Ʒ���� param2:seq param3:select_flag
	BUY_GIFT_ITEM_TYPE_TIAN_YI_YAO_SHI = 57,					//!< ����ҫ�� param2 - seq
	BUY_GIFT_ITEM_TYPE_SHEN_CI_HUA_FU = 58,						//!< ������ͻ��� param2 - seq
	BUY_GIFT_ITEM_TYPE_NEW_COURAGE_CHALLENGE = 59,				//!< ߱��֮��ս��
	BUY_GIFT_ITEM_TYPE_DAN_BI_FAN_LI = 60,						//!< ���ʷ��� param2:pool_seq param3:seq
	BUY_GIFT_ITEM_TYPE_ZHUI_YUE_SHANG_DIAN = 61,				//!< ׷���̵� param2 - seq

	BUY_GIFT_ITEM_TYPE_MAX
};


//ֱ������,����Ҳ˳�����/˳��֪ͨ�ͻ���Ҳ����
UNSTD_STATIC_CHECK(BUY_TIME_TYPE_MAX == 79);
UNSTD_STATIC_CHECK(BUY_GIFT_ITEM_TYPE_MAX == 62);	

enum ACTIVITY_TYPE
{
	ACTIVITY_TYPE_INVALID = 0,
	ACTIVITY_TYPE_WORLD_BOSS = 1,				// �ַ�����BOSS
	ACTIVITY_TYPE_TEAM_FIGHT = 2,				// С�Ӿ���
	ACTIVITY_TYPE_MAZE_RACER = 3,				// �Թ�����
	ACTIVITY_TYPE_BRAVE_GROUND = 4,				// �´��ع�
	ACTIVITY_TYPE_GUILD_FIGHT = 5,				// ������Ҷ�
	ACTIVITY_TYPE_CHIEF_ELECTION = 6,			// ��ϯ��ѡ
	ACTIVITY_TYPE_CHIEF_PEAK_BATTLE = 7,		// ��ϯ�۷�ս
	ACTIVITY_TYPE_GUILD_HONOR_BATTLE = 8,		// ������ҫս
	ACTIVITY_TYPE_GUILD_ANSWER = 9,				// �������
	ACTIVITY_TYPE_WORLD_BOSS_2 = 10,			// �ַ�����BOSS2 (����boss)
	ACTIVITY_TYPE_ONLY_FIGHT = 11,				// ��һ�а�(���˰������Ҷ�)
	ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG = 12,		// �ý�ս��
	ACTIVITY_TYPE_RELIC_LOOTING = 13,			// �ż��ᱦ
	ACTIVITY_TYPE_HUNDRED_GHOST = 14,			// �ٹ�ҹ��
	ACTIVITY_TYPE_SILK_ROADS = 15,				// ˿��֮·
	ACTIVITY_TYPE_WORLD_TEAM_ARENA = 16,		// Ӣ�ۻ���
	ACTIVITY_TYPE_QING_YUAN_SHENG_HUI = 17,		// ��Եʢ��
	ACTIVITY_TYPE_WORLD_BOSS_3 = 18,			// �ַ�����BOSS3 (������)

	ACTIVITY_TYPE_MAX,
};
UNSTD_STATIC_CHECK(ACTIVITY_TYPE_MAX < MAX_ACTIVITY_TYPE_NUM);
UNSTD_STATIC_CHECK(ACTIVITY_TYPE_MAX < CLIENT_ACTIVITY_TYPE_BEGIN);

enum SPECIAL_ACTIVITY_TYPE
{
	SPECIAL_ACTIVITY_TYPE_INVALID = 1024,

	SPECIAL_ACTIVITY_TYPE_OPENGAME,										// �����
	SPECIAL_ACTIVITY_TYPE_CLOSEDBETA,									// ���
	SPECIAL_ACTIVITY_TYPE_COMMON_ACTIVITY,								// ͨ�û
	SPECIAL_ACTIVITY_TYPE_COMBINE_SERVER,								// �Ϸ��

	SPECIAL_ACTIVITY_TYPE_RESVERED,
};
UNSTD_STATIC_CHECK(SPECIAL_ACTIVITY_TYPE_RESVERED < CLIENT_ACTIVITY_TYPE_BEGIN);

static const int SPECIAL_ACTIVITY_TYPE_MAX = SPECIAL_ACTIVITY_TYPE_RESVERED - SPECIAL_ACTIVITY_TYPE_INVALID;

// ����
enum RAND_ACTIVITY_TYPE
{
	RAND_ACTIVITY_TYPE_BEGIN = 2048,

	RAND_ACTIVITY_TYPE_TIME_LIMIT_STORE = 2049,			//��ʱ�̵�
	RAND_ACTIVITY_TYPE_TRUN_TABLE = 2051,				//�����-ת�̻
	RAND_ACTIVITY_TYPE_DISCOUNT_STORE = 2052,			//�����-�ۿ��̵�
	RAND_ACTIVITY_TYPE_SKY_DROP_LUXURY_GIFT = 2054,		//�콵����
	RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL = 2055,			//����ת��
	RAND_ACTIVITY_TYPE_EXPEL_MONSTER = 2056,			//��������
	RAND_ACTIVITY_TYPE_HERITAGE_TREASURE = 2057,		//���й屦
	RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE = 2058,		//�߲�ͼ��
	RAND_ACTIVITY_TYPE_SALE_SHOP = 2059,				//�����̵�
	RAND_ACTIVITY_TYPE_HAPPY_UP = 2060,					//��������/ӡ�Ƕһ�
	RAND_ACTIVITY_TYPE_ANIMAL_RACE = 2061,				//���޾���
	RAND_ACTIVITY_TYPE_LUCKY_BAG = 2062,				// ����
	RAND_ACTIVITY_TYPE_REDUCE_TAX_RATE = 2063,			//˰���Ż�
	RAND_ACTIVITY_TYPE_TIAN_JIANG_HAO_LI = 2064,		//�콵����
	RAND_ACTIVITY_TYPE_SEAL_PROMOTE = 2065,				//��ӡ����
	RAND_ACTIVITY_TYPE_DROP_DOUBLE = 2066,				//���䷭��
	RAND_ACTIVITY_TYPE_JOYSEEKSFAVORS = 2067,			//����Ѱ��
	RAND_ACTIVITY_TYPE_COLLECT_WORD = 2068,				// ���ֻ
	RAND_ACTIVITY_TYPE_DISCOUNT_CARNIVAL = 2069,		//�ۿۿ�
	RAND_ACTIVITY_TYPE_PARTNER_EXCHANGE = 2070,			//���һ�
	RAND_ACTIVITY_TYPE_LUCKY_DISCOUNT = 2071,			//�����ۿ�
	RAND_ACTIVITY_TYPE_PET_EXCHANGE = 2072,				//����һ�
	RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN = 2073,			//�������ģ��þ�������
	RAND_ACTIVITY_TYPE_ADVENTURE_GIFT = 2074,			//�������
	RAND_ACTIVITY_TYPE_LINKAGE_GIFT = 2075,				//��������
	RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE = 2076,			//����ת��
	RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION = 2077,		//��������
	RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT = 2078,		//ȫ���Ź�
	RAND_ACTIVITY_TYPE_SPORTS = 2079,					//��������������
	RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE = 2080,			//ͼ���ᱦ
	RAND_ACTIVITY_TYPE_GOD_TOKEN = 2081,				//����ս��
	RAND_ACTIVITY_TYPE_ELEMENT_CARDS = 2082,			//Ԫ�ط���
	RAND_ACTIVITY_TYPE_SIGN_IN_CONTINUITY = 2083,		//���Ǻ���
	RAND_ACTIVITY_TYPE_CAI_SHEN_JU_BAO = 2084,			// ����۱�
	RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG = 2085,		// ���Ʊ���
	RAND_ACTIVITY_TYPE_ZHUAN_SHU_LI_BAO = 2086,			// ר�����
	RAND_ACTIVITY_TYPE_JING_YAN_BIAO_SHENG = 2087,		// �������
	RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE = 2088,			// ��ʱ����
	RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI = 2089,			// ��ʱ����
	RAND_ACTIVITY_TYPE_GIVE_PET_DRAW = 2090,			// ��30���飨��Ѱ�����飩
	RAND_ACTIVITY_TYPE_CI_JIU_YING_XIN = 2091,			// �Ǿ�ӭ��
	RAND_ACTIVITY_TYPE_NIU_XING_DA_YUN = 2092,			// ţ�д���
	RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE = 2093,	// �����
	RAND_ACTIVITY_TYPE_MONPOLY_GIFT = 2094,				// ţ�˵�ͷ
	RAND_ACTIVITY_TYPE_COW_EXCHANGE = 2095,				// ţ�˶һ�
	RAND_ACTIVITY_TYPE_COW_STORE = 2096,				// ţ���̳�
	RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE = 2097,		// ţ���ޱ�
	RAND_ACTIVITY_TYPE_LITTLE_YEAR_GIFT = 2098,			// С������
	RAND_ACTIVITY_TYPE_GIVE_SECRET_KEY = 2099,			// ��Ѱ������
	RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE = 2100,		// �ۼƳ�ֵ
	RAND_ACTIVITY_TYPE_PET_LOOT = 2101,					// ����ᱦ
	RAND_ACTIVITY_TYPE_TRADEMARKET_GOLD_RETURN = 2102,	// ����������
	RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN= 2103,		// ���޽���
	RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN = 2104,			// ����ת��
	RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE = 2105,	// �ػ��ر�
	RAND_ACTIVITY_TYPE_HELP_OTHERS = 2106,				// �嵨����
	RAND_ACTIVITY_TYPE_FORTUNE_MAZE = 2107,				// �����Թ�
	RAND_ACTIVITY_TYPE_CHUN_RI_DENG_FENG = 2108,		// ���յǷ�
	RAND_ACTIVITY_TYPE_ANECDOTE = 2109,					// �ؾ�����
	RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI = 2110,			// �Ͷ�����
	RAND_ACTIVITY_TYPE_ZU_HE_HE_LI = 2111,				// ��Ϻ���
	RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI = 2112,			// �������
	RAND_ACTIVITY_TYPE_FASHION_SHOP = 2113,				// ʱװ�̵�
	RAND_ACTIVITY_TYPE_ONE_YUAN_GOU = 2114,				// һԪ��
	RAND_ACTIVITY_TYPE_LING_YU_JI_JIN = 2115,			// �������
	RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI_2 = 2116,		// �ķ�����(�������2)
	RAND_ACTIVITY_TYPE_LUCKY_SIGN = 2117,				// ���˻�ǩ
	RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN = 2118,			// ��¥����
	RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP = 2119,			// �����ۿ��̳�
	RAND_ACTIVITY_TYPE_YUAN_QI_TURNING = 2120,			// Ԫ������
	RAND_ACTIVITY_TYPE_COOL_BREEZE = 2121,				// ������ˬ
	RAND_ACTIVITY_TYPE_LOTUS_MI_BAO = 2122,				// �����ٱ�
	RAND_ACTIVITY_TYPE_MAZE_SUMMER = 2123,				// �����Թ�
	RAND_ACTIVITY_TYPE_SUMMER_LI_LIAN = 2124,			// ��������
	RAND_ACTIVITY_TYPE_COLORFUL_LANTERN = 2125,			// ��ʻ���
	RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN = 2126,			// �ٱ�����
	RAND_ACTIVITY_TYPE_SUMMER_STORE = 2127,				// �����̵�
	RAND_ACTIVITY_TYPE_ZHEN_PIN_SHANG_PU = 2128,		// ��Ʒ����
	RAND_ACTIVITY_TYPE_PAI_MAI_HANG = 2129,				// ������
	RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI_3 = 2130,		// ��������(�������3)
	RAND_ACTIVITY_TYPE_DUI_HUAN_LING_YU = 2131,			// �һ�����
	RAND_ACTIVITY_TYPE_BACK_REWARD = 2132,				// �ع����
	RAND_ACTIVITY_TYPE_SERVER_COMPETITION = 2133,		// ȫ����ƴ 
	RAND_ACTIVITY_TYPE_HAPPY_LOOK = 2134,				// �����ַ���
	RAND_ACTIVITY_TYPE_SIGN_HAPPY = 2135,				// ��¼�ͺ���
	RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING = 2136,		// ���鹲ӯ
	RAND_ACTIVITY_TYPE_HELP_OTHERS_2 = 2137,			// ��԰һ����
	RAND_ACTIVITY_TYPE_ANECDOTE_2 = 2138,				// �ؾ�����2 ���������ţ�
	RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG = 2139,		// �������
	RAND_ACTIVITY_TYPE_INTEGRAL_DRAW = 2140,			// ����齱 ��ɽ�����£�
	RAND_ACTIVITY_TYPE_SHAN_HAI_MET = 2141,				// ɽ������
	RAND_ACTIVITY_TYPE_SHANHAI_QUESTION = 2142,			// ɽ���ʴ�
	RAND_ACTIVITY_TYPE_SHAN_HAI_JU_BAO = 2143,			// ɽ���۱�
	RAND_ACTIVITY_TYPE_SHAN_HAI_MEET = 2144,			// ɽ�����
	RAND_ACTIVITY_TYPE_FATE_ENTANGLEMENT = 2145,		// ɽ��Ե��
	RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO = 2146,			// �вƽ���
	RAND_ACTIVITY_TYPE_YUN_ZE_TERRITORY = 2147,			// ���󻭾�
	RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD = 2148,	// ���󷱻�¼
	RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE = 2149,		// ����ͬ����
	RAND_ACTIVITY_TYPE_ZAI_ZHAN_YUN_ZE = 2150,			// ��ս����
	RAND_ACTIVITY_TYPE_MO_ZU_ZAI_NA_LI = 2151,			// ħ��������
	RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG = 2152,			// ��ֵ����
	RAND_ACTIVITY_TYPE_YUN_ZE_ER_SAN_SHI =2153,			// ���������
	RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE = 2154,			// ����ӱ�
	RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG = 2155,		// ɽ������
	RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO = 2156,			// ɽ���ٱ�
	RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_XIANG = 2157,		// ɽ������
 	RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_HE = 2158,			// ɽ������
 	RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_DAI = 2159,			// ɽ������
	RAND_ACTIVITY_TYPE_SHANHAI_HUADENG = 2160,			// ɽ������
	RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_2 = 2161,		// ���Ʊ���2 (����2085) ������
	RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_2 = 2162,		// ��ʱ����2 (����2088) ���󱦺�
	RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI_2 = 2163,		// ��ʱ����2 (����2089) ���󱦴�
	RAND_ACTIVITY_TYPE_JIXING_GAOZHAO = 2164,			// ���Ǹ���
	RAND_ACTIVITY_TYPE_YUN_ZE_WISH = 2165,				// ������Ը
	RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI = 2166,			// �������
	RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET = 2167,			// �ȳ�ݵ�
	RAND_ACTIVITY_TYPE_YUN_ZE_CATCH_ELVES = 2168,		// �����ץ��
	RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG = 2169,		// ��ʱ�۳�
	RAND_ACTIVITY_TYPE_WEEKEND_GIFT = 2170,				// ��ĩ���
	RAND_ACTIVITY_TYPE_CHAO_ZHI_GIFT = 2171,			// ��ֵ����
	RAND_ACTIVITY_TYPE_JING_JI_YUE_KA = 2172,			// �����¿�
 	RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR = 2173,	// ��������
 	RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING = 2174,		// ����̽��
 	RAND_ACTIVITY_TYPE_TIGER_TEASE_NEWYEAR = 2175,		// ��������(�������´�)
 	RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY = 2176,		// ����ѹ��
	RAND_ACTIVITY_TYPE_YINHU_WENSHI = 2177,				// ��������
	RAND_ACTIVITY_TYPE_CINIU_YINGXINHU = 2178,			// ��ţӭ�»�
	RAND_ACTIVITY_TYPE_HU_XING_DA_YUN = 2179,			// ���д���
	RAND_ACTIVITY_TYPE_YIN_HU_BAI_SUI = 2180,			// ��������
	RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW = 2181,	// ��ѩ����
	RAND_ACTIVITY_TYPE_RONG_LIAN_YOU_LI = 2182,			// ��������
	RAND_ACTIVITY_TYPE_QING_YUAN_SHI_ZHUANG = 2183,		// ��Եʱװ
	RAND_ACTIVITY_TYPE_QING_YUAN_LEI_CHONG = 2184,		// ��Ե�۳�
	RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT = 2185,			// ��������
	RAND_ACTIVITY_TYPE_QIYUAN_KONGMINGDENG = 2186,		// ��Ը������
	RAND_ACTIVITY_TYPE_YUAN_QI_SPEED = 2187,			// Ԫ������
	RAND_ACTIVITY_TYPE_YUAN_QI_HELP = 2188,				// Ԫ������	
	RAND_ACTIVITY_TYPE_XUNMENG_HUDIE = 2189,			// Ѱ�κ���
	RAND_ACTIVITY_TYPE_BO_ZHONG_YOU_LI = 2190,			// ��������
	RAND_ACTIVITY_TYPE_YUNA_QI_FAN_PAI = 2191,			// Ԫ������
	RAND_ACTIVITY_TYPE_BAO_ZI_LAI_LE = 2192,			// ��������
	RAND_ACTIVITY_TYPE_WANLING_ZHULI = 2193,			// ��������
	RAND_ACTIVITY_TYPE_JIN_YI_TIAN_XIANG = 2194,		// ��������
	RAND_ACTIVITY_TYPE_MIAO_BI_SHENG_HUA = 2195,		// �������
	RAND_ACTIVITY_TYPE_YUAN_QI_YAN_HUO = 2196,			// Ԫ���̻�
	RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU = 2197,		// Ԫ�����²�
	RAND_ACTIVITY_TYPE_CHENG_FENG_PO_LANG = 2198,		// �˷�����
	RAND_ACTIVITY_TYPE_ZONG_XIANG_MAN_CHENG = 2199,		// ��������
	RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE = 2200,		// ����֮��
	RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG = 2201,		// ��������
	RAND_ACTIVITY_TYPE_DUAN_YANG_ZI_XUAN = 2202,		// ������ѡ
	RAND_ACTIVITY_TYPE_CANG_LONG_CI_FU = 2203,			// �����͸�
	RAND_ACTIVITY_TYPE_LUCKY_BLIND_BOX = 2204,			// ����ä��
	RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU = 2205,			// ��������
	RAND_ACTIVITY_TYPE_300_CHOU = 2206,					// ���ٳ�Ƥ��
	RAND_ACTIVITY_TYPE_DUAN_YANG_FU_LI = 2207,			// ��������
	RAND_ACTIVITY_TYPE_JIN_LONG_JU_BAO = 2208,			// �����۱�
	RAND_ACTIVITY_TYPE_CHONG_JI_FAN_LI = 2209,			// �輼����
	RAND_ACTIVITY_TYPE_SHEN_SHOU_EN_ZE = 2210,			// ���޶���
	RAND_ACTIVITY_TYPE_XIN_FU_QIANG_HUA = 2211,			// �·�ǿ��
	RAND_ACTIVITY_TYPE_QI_XI_TE_HUI = 2212,				// ��Ϧ�ػ�
	RAND_ACTIVITY_TYPE_CHARISMATIC_TANABATA = 2213,		// ������Ϧ
	RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_3 = 2214,		// ��ʱ����3 (�ļ�����)
	RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_4 = 2215,		// ��ʱ����4 (�ļ�����)
	RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_3 = 2216,		// ���Ʊ���3 (�ļ�����) ����2085 
	RAND_ACTIVITY_TYPE_XIN_FU_LING_CHONG = 2217,		// �·����
	RAND_ACTIVITY_TYPE_SHEN_CI_HUA_FU = 2218,			// ��ͻ��� 
	RAND_ACTIVITY_TYPE_TIAN_YI_YAO_SHI = 2219,			// ����ҫ��
	RAND_ACTIVITY_TYPE_HUN_QI_FAN_LI = 2220,			// �������� 
	RAND_ACTIVITY_TYPE_YUE_XI_DENG_MI = 2221,			// ��Ϧ����
	RAND_ACTIVITY_TYPE_YUE_XI_LEI_CHONG = 2222,			// ��Ϧ�۳� 
	RAND_ACTIVITY_TYPE_DAN_BI_FAN_LI = 2223,			// ���ʷ���
	RAND_ACTIVITY_TYPE_ZHUI_YUE_SHANG_DIAN = 2224,		// ׷���̵�
	RAND_ACTIVITY_TYPE_ZENG_100_CHOU = 2225,			// ��100��

	RAND_ACTIVITY_TYPE_END,
};
UNSTD_STATIC_CHECK(RAND_ACTIVITY_TYPE_END < CLIENT_ACTIVITY_TYPE_BEGIN);

static const int RAND_ACTIVITY_TYPE_MAX = RAND_ACTIVITY_TYPE_END - RAND_ACTIVITY_TYPE_BEGIN; 

// ������������
enum RAND_ACTIVITY_OPEN_TYPE
{
	RAND_ACTIVITY_OPEN_TYPE_BEGIN = 0,

	RAND_ACTIVITY_OPEN_TYPE_NORMAL = RAND_ACTIVITY_OPEN_TYPE_BEGIN,		// ��ͨ����

	RAND_ACTIVITY_OPEN_TYPE_END,
};

enum ACTIVITY_STATUS
{
	ACTIVITY_STATUS_CLOSE = 0,											// ��ر�״̬
	ACTIVITY_STATUS_STANDY,												// �׼��״̬
	ACTIVITY_STATUS_OPEN,												// ���ʼ�� 
	ACTIVITY_STATUS_MAX,
};

enum RAND_ACTIVITY_MAKE_UP_TYPE
{
	RAND_ACTIVITY_MAKE_UP_TYPE_COIN = 0,			//ʹ��ͭ�Ҳ���
	RAND_ACTIVITY_MAKE_UP_TYPE_GOLD = 1,			//ʹ��Ԫ������

	RAND_ACTIVITY_MAKE_UP_TYPE_MAX
};

struct ActivityStatus
{
	short activity_type;
	short status;
	unsigned int activity_begin_time;
	unsigned int activity_end_time;
};

struct SyncActivityStatus
{
	SyncActivityStatus()
	{
		activity_status = 0;
		next_status_switch_time = 0;
	}

	int activity_status;
	unsigned int next_status_switch_time;
};

struct SpecialActivityCfg
{
	SpecialActivityCfg() : type(SPECIAL_ACTIVITY_TYPE_INVALID), is_open(false), begin_time(0), end_time(0) {}

	bool Invalid() { return SPECIAL_ACTIVITY_TYPE_INVALID == type; }

	int type;
	bool is_open;

	unsigned int begin_time;
	unsigned int end_time;
};

struct RAMoneyInfo
{
	RAMoneyInfo() : money_type(0), money_count(0) {}
	RAMoneyInfo(short _money_type, int _money_count) : money_type(_money_type), money_count(_money_count) {}

	short money_type;
	int money_count;
};

struct RandActivityCfg
{
	RandActivityCfg() : type(0), is_open(false), duration_day(0) {}

	bool Invalid() { return 0 == type; }

	int type;
	bool is_open;

	int duration_day;
};

//�¿��ܿ�
static const int MAX_MONTH_CARD_SHOP_ITEM_NUM = 30;

//�׳�
enum FRIST_RECHARGE_TYPE
{
	FRIST_RECHARGE_TYPE_YIXIANER = 1,//ҽ�ɶ�

	FRIST_RECHARGE_TYPE_MAX,
};
static const int MAX_FIRST_RECHARGE_DAY_NUM = 3;			//��������,��ȡʱĬ�ϵ�һ��
static const int MAX_FIRST_RECHARGE_TYPE_NUM = BIT_COUNT_OF_BYTE;		//�޸�ע�����
UNSTD_STATIC_CHECK(MAX_FIRST_RECHARGE_DAY_NUM <= BIT_COUNT_OF_BYTE);
UNSTD_STATIC_CHECK(MAX_FIRST_RECHARGE_TYPE_NUM >= FRIST_RECHARGE_TYPE_MAX);

static const int MAX_FIRST_RECHARGE_EXCLUSIVE_NUM = 32;		//�׳�ר���������ֵ
enum FIRST_RECHARGE_CMD_TYPE
{
	FIRST_RECHARGE_CMD_TYPE_GROW = 0,			//�׳�ר��-�ɳ�����

	FIRST_RECHARGE_CMD_TYPE_MAX
};

enum FIRST_RECHARGE_EXCLUSIVE_TYPE
{
	FIRST_RECHARGE_EXCLUSIVE_TYPE_INVAILD = 0,
	FIRST_RECHARGE_EXCLUSIVE_TYPE_ZHAO_CAI_XIAN_ZI = 1,		// �в�����
	FIRST_RECHARGE_EXCLUSIVE_TYPE_CHENG_ZHANG_ZHU_LI = 2,	// �ɳ�����

	FIRST_RECHARGE_EXCLUSIVE_TYPE_MAX
};

inline int GetFirstRechargeCmdTypeByBuyType(int buy_type)
{
	int first_charge_cmd_type = -1;
	switch (buy_type)
	{
		case BUY_TIME_TYPE_FIRST_RECHARGE_EXCLUSIVE:
		{
			first_charge_cmd_type = FIRST_RECHARGE_CMD_TYPE_GROW;
		}
	default:
		break;
	}

	return first_charge_cmd_type;
}

//////////////////////////////////////////////// � ��ض��� ////////////////////////////////////////////////
struct ActivityRoleInform
{
	ActivityRoleInform() : role_uid(0), level(0), prof(0), avatar(0), headshot_id(0)
	{
		memset(role_name, 0, sizeof(GameName));
		memset(guild_name, 0, sizeof(GuildName));
	}

	int role_uid;			//!< ��ɫ��UID
	UInt16 level;			//!< ��ɫ�ȼ�
	UInt16 prof;			//!< ��ɫְҵ
	UInt16 avatar;			//!< ��ɫģ��
	UInt16 headshot_id;		//!< ͷ��ID
	GameName role_name;		//!< ��ɫ����
	GuildName guild_name;	//!< ��������
};

//--------------------------------------�Թ����� ----------------------------------------------------
static const int MAZE_RACER_MIN_LAYER = 1;

enum MAZE_RACER_AMULET_EFFECT_TYPE		// �Թ����� - ���� - Ч������
{
	MAZE_AMULET_EFFECT_SET_BATTLE_SPEED = 0,		// ����ս������
	MAZE_AMULET_EFFECT_SET_WAR_INTERVAL = 1,		// �������ּ��
	MAZE_AMULET_EFFECT_SET_MOVES_PERMIT = 2,		// �����ƶ����
	MAZE_AMULET_EFFECT_SET_ENCNT_PERMIT = 3,		// �����������

	MAZE_AMULET_EFFECT_TYPE_MAX
};

enum MAZE_RACER_AMULET_TARGET_TYPE
{
	MAZE_AMULET_TARGET_ENTIRE_TEAM = 0,			// ��������
	MAZE_AMULET_TARGET_ANOTHER_TEAM = 1,		// ���һ����������

	MAZE_AMULET_TARGET_TYPE_MAX
};
struct MazeRacerTeamInfo			// �Թ����� - ������Ϣ
{
	MazeRacerTeamInfo(int _pass_id = 0, int _mem_count = 0,int _team_index=0) : pass_id(_pass_id), mem_count(_mem_count)
	{
		team_index = _team_index;
	}

	int team_index;
	int pass_id;		// ͨ�ر��
	int mem_count;		// ͨ��ʱ�Ķ�Ա����
	ActivityRoleInform pass_member[MAX_TEAM_MEMBER_NUM];	// ͨ��ʱȫ��Ա����Ϣ(�����������)
};
//----------------------------------------------------------------------------------------------------------


static const int ACTIVITY_RANK_DEFAULT_NUM_MAX = 100;								// ����Ĭ�ϵ����а񳤶�
static const int ACTIVITY_RANK_SHORTER_NUM_MAX = 50;								// �����϶̵����а񳤶�
static const int ACTIVITY_RANK_LIST_COUNT_10 = 10;
static const int ACTIVITY_AWARD_NUM_MAX = 10;										// ���������б���

//����boss
static const int ACTIVITY_WORLD_BOSS_MAX_RANK_ROLE_NUM = 20;

//����boss2(����boss)
static const int ACTIVITY_WORLD_BOSS_2_MAX_RANK_ROLE_NUM = 20;

//����boss3(������)
static const int ACTIVITY_WORLD_BOSS_3_MAX_RANK_ROLE_NUM = 20;

//�Թ�����
static const int ACTIVITY_MAZE_RACER_AMULET_MAX_TYPE = 10;							// �Թ����� - ��������������

//-----------------------------�´��ع�-----------------------------
const static int ACTIVITY_BRAVE_GROUND_LIMIT_ITEM_MAX = 100;		//���������Ʒ����
static const int ACTIVITY_BRAVE_GROUND_MAX_RANK = 100;			//�´��ع����а�������

//---------------------------------����----------------------------------------
const static int MAX_RATE_COUNT_NUM = 10000;
const static int MAX_BONUS_COUNT_NUM = 128;
//-----------------------------����-�������------------------------------------
enum ADVENTURE_GIFT_CONDITION_TYPE
{
	ADVENTURE_GIFT_CONDITION_TYPE_ROLE_LEVEL,						//��ɫ�ȼ�
	ADVENTURE_GIFT_CONDITION_TYPE_COURAGE_CHALLENGE,			//������ս����
	ADVENTURE_GIFT_CONDITION_TYPE_COURSE_OF_GROWTH_PROG,		//�ɾͽ׶�
};

//-----------------------------�¶�Ͷ��------------------------------------
static const int RA_MONTH_INVESTMENT_MAX_NUM = 30;	   
static const int RA_MONTH_PURCHASE_REWARD = 5;			// �����¶�Ͷ�ʽ���

//-----------------------------���ֻ------------------------------------
static const int SCORE_REWARD_MAX_NUM = 32;
UNSTD_STATIC_CHECK(SCORE_REWARD_MAX_NUM == 32);//�������Ҫ��չ,�洢�Ǳ���Ҫת��

enum ACTVITY_BUY_LIMIT_TYPE
{
	ACTVITY_BUY_LIMIT_TYPE_BEGIN = 1,
	ACTVITY_BUY_LIMIT_TYPE_ALL = ACTVITY_BUY_LIMIT_TYPE_BEGIN,										//���޹�
	ACTVITY_BUY_LIMIT_TYPE_DAY = 2,										//ÿ���޹�
	
	ACTVITY_BUY_LIMIT_TYPE_MAX,
};

//-----------------------------С�Ӿ���------------------------------------
const static int  TUAN_DUI_JING_JI_MAX_MEMBER_NUM = 3;

struct role_info
{
	role_info()	{ this->Reset(); }

	void Reset()
	{	
		role_id = 0;
		memset(role_name, 0, sizeof(GameName));
		obj_id = 0;
		level = 0;
		behavior_state = 0;
		vip_level = 0; 
		avatar_type = 0;
		profession = 0;
		appearance.Reset();
		move_speed = 0;
		pos_x = 0;
		pos_y = 0;
		distance = 0;
		dir = 0;
		scene_id = 0;
		escort_id = 0;
		memset(guildname, 0, sizeof(GuildName));
		is_leader = 0;
		is_in_team = 0;
		title_id = 0;
	}

	bool Invalid()
	{
		return role_id == 0;
	}
	
	int				role_id;		//!< ���ID
	GameName		role_name;		//!< �����

	ObjID			obj_id;			//!< ��Ҷ���ID
	short			level;			//!< ��ҵȼ�

	char			behavior_state;	//!< ��Ϊ״̬ \see RoleStatusManager::ROLE_STATUS
	char			vip_level;		//!< vip�ȼ�
	short			avatar_type;	//!< ��������
	int				profession;		//!< ְҵ
	Appearance		appearance;		//!< ���

	int				move_speed;		//!< �ٶ�
	short			pos_x;			//!< �ڳ����е�λ��X
	short			pos_y;			//!< �ڳ����е�λ��Y
	Scalar			distance;		//!< ������� (���ڵ�ǰ������Ҫ�ߵľ��룩
	Dir				dir;			//!< ���� [-PI,-PI]
	int				scene_id;		//!< ��ʶ�����ĸ�������obj
	int				escort_id;		//!< �ڳ�id
	GuildName		guildname;		//!< ������

	char			is_leader;		//!< �Ƿ�ӳ�[0:�� 1:��]
	char			is_in_team;		//!< �Ƿ��ڶ���[0:�� 1:��]
	UInt16			title_id;		//!< �ƺ�id
};


//-----------------------------��ʱ�����------------------------------------
const static int MAX_SIGN_UP_NUM = 32;

//---------------------������,����һ����--------------------------------
enum ACTIVITY_GIVE_ITEM_TYPE
{
	ACTIVITY_GIVE_ITEM_TYPE_PATROL_MINING = 0,		//Ѳ�ߺͲɼ�
	ACTIVITY_GIVE_ITEM_TYPE_FIELD_BOSS = 1,			//Ұ��BOSS
	ACTIVITY_GIVE_ITEM_TYPE_CRYSTAL_BOSS = 2,		//ˮ������BOSS
};




//-----------------------------��ϯ��ѡ------------------------------------
static const int ACTIVITY_CHIEF_ELECTION_BATTLE_TIME = 600;			//��ϯ��ѡÿ�غ����ս��ʱ��
static const int ACTIVITY_CHIEF_ELECTION_FIGTHER_NUM = 16;			//��ϯ��ѡ�μ�ǰ��λ�μӶ�ս����
static const int ACTIVITY_CHIEF_ELECTION_JOIN_PROF_NUM = 8;			//��ϯ��ѡ ְҵ�������
UNSTD_STATIC_CHECK(ACTIVITY_CHIEF_ELECTION_JOIN_PROF_NUM >= PROFESSION_TYPE_NUM - 1)

struct ChiefElectionWinnerInfo
{
	int prof;
	int role_id;
};

//-----------------------------��ϯ�۷�ս------------------------------------
static const int ACTIVITY_CHIEF_PEAK_BATTLE_BATTLE_TIME = 600;			//��ϯ�۷�սÿ�غ����ս��ʱ��
static const int ACTIVITY_CHIEF_PEAK_BATTLE_FIGTHER_NUM = 8;			//��ϯ�۷�ս�μ�ǰ��λ�μӶ�ս����
static const int ACTIVITY_CHIEF_PEAK_BATTLE_DECLARATION_LENGTH = 128;			//��ϯ�۷�ս ���Գ���

struct ChiefPeakBattleInfo
{
	int match_round;
	int role_id;
};

// ----------------------------- ������� ----------------------------- //
struct GuildAnswerAwardTotalCon
{
	GuildAnswerAwardTotalCon(int _coin = 0, int _exp = 0, int _contri = 0) 
		: coin(_coin), experience(_exp), guild_contribution(_contri) {}

	int coin;
	int experience;
	int guild_contribution;

	std::map<int, std::map<int, int> > item_list;

	GuildAnswerAwardTotalCon & operator += (GuildAnswerAwardTotalCon & cobj)
	{
		this->coin += cobj.coin;
		this->experience += cobj.experience;
		this->guild_contribution += cobj.guild_contribution;

		for (std::map<int, std::map<int, int> >::iterator item_it = cobj.item_list.begin(); item_it != cobj.item_list.end(); ++item_it)
		{
			for (std::map<int, int>::iterator bind_it = item_it->second.begin(); bind_it != item_it->second.end(); ++bind_it)
			{
				this->item_list[item_it->first][bind_it->first] += bind_it->second;
			}
		}

		return *this;
	}

	void AddItemGet(const ItemConfigData & _item)
	{
		this->item_list[_item.item_id][_item.is_bind] += _item.num;
	}
};


// ----------------------------- ��һ�а� ----------------------------- //
static const int ONLY_FIGHT_SCNEE_MAX_NUM = 20;			//��һ�аٵȼ������ֵ
static const int ONLY_FIGHT_RANK_TITLE_MAX_NUM = 3;		//����ǰ�����гƺŽ���

#pragma pack(push, 4)

// ----------------------------- �ʱ��ṹ----------------------------- //
struct StatusTimeCfg											//�л��⣬��¼��ʼ�رջʱ����Ϣ
{
	StatusTimeCfg() : status(0), start_hour(0), start_min(0), start_sec(0), dur_s(0) {}

	bool Invalid() { return 0 == dur_s; }

	int status;															// ״̬
	int start_hour;														// ��ʼ H
	int start_min;														// ��ʼ M
	int start_sec;														// ��ʼ S
	int dur_s;															// ��������S
};

struct StatusTimeCfgList												//�л��⣬��¼��ʼ�رջʱ����Ϣ
{
	StatusTimeCfg statustime[ACTIVITY_STATUS_MAX];
};

//-----------------------------------�����------------------------------
bool IsRandActivity(int activity_type);
bool IsNormalActivity(int activity_type);
unsigned int GetActivityDataLength(int activity_type);

struct RandActivityData
{
	static const int DATA_LENGTH = 899;

	void Reset()
	{
		begin_time = 0;
		end_time = 0;
		activity_open_type = RAND_ACTIVITY_OPEN_TYPE_NORMAL;
		memset(data, 0, sizeof(data));
	}

	unsigned int begin_time;
	unsigned int end_time;
	char data[DATA_LENGTH];
	char activity_open_type;
};

struct NormalActivityData					//Ĭ�������������ͨ�����
{
	static const int DATA_LENGTH = 400;

	void Reset()
	{
		memset(data, 0, sizeof(data));
	}

	char data[DATA_LENGTH];
};

struct HuanJieZhanChangActivityData					//�ý�ս�������
{
	void Reset()
	{
		season_num = 0;
	}

	int season_num;
};

struct ActivityWorldTeamArenaParam
{
	void Reset()
	{
		season_num = 1;
		start_sign_up_timestamp = 0;
		end_sign_up_timestamp = 0;
		activity_open_next_zero_timestamp = 0;
	}

	int season_num;				//����
	unsigned int start_sign_up_timestamp;		//��ʼ����ʱ���
	unsigned int end_sign_up_timestamp;			//ֹͣ����ʱ���
	unsigned int activity_open_next_zero_timestamp;	//���ʼ����0��ʱ���
};


union ActivityData
{
	void Reset(int act_type);

	RandActivityData rand_activity_data;
	NormalActivityData activity_data;
	HuanJieZhanChangActivityData huan_jie_activity_data;
	ActivityWorldTeamArenaParam world_team_arena;
};

typedef char ActivityDataHex[sizeof(ActivityData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ActivityDataHex) < 2048);

struct RandActivityParam
{
	RandActivityParam() {}

	void Reset()
	{
		memset(data_list, 0, sizeof(data_list));
	}

	RandActivityData data_list[RAND_ACTIVITY_TYPE_MAX];
};

struct CrossRandActivityDataParam
{
	CrossRandActivityDataParam() : activity_type(0), open_time(0), close_time(0), reserved_char(0)
	{
		memset(data, 0, sizeof(data));
	}

	int activity_type;
	unsigned int open_time;
	unsigned int close_time;
	char data[RandActivityData::DATA_LENGTH];
	char reserved_char;
};

struct RADelaySetTimeData
{
	RADelaySetTimeData() 
	{
		this->Reset();
	}

	void Reset()
	{
		is_force = 0;
		reserve_sh = 0;
		rand_activty_type = 0;
		begin_time = 0;
		end_time = 0;
		open_type = 0;
		delay_to_time = 0;
		memset(reserve_ll, 0, sizeof(reserve_ll));
	}

	short is_force;
	short reserve_sh;
	int rand_activty_type;
	unsigned int begin_time;
	unsigned int end_time;
	int open_type;
	unsigned int delay_to_time; // ��ʱ����ʱ����ʱ��
	long long reserve_ll[2];
};

struct RADelaySetTimeParam
{
	static const int MAX_DELAY_NUM = 20;

	RADelaySetTimeParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(reserve_ll, 0, sizeof(reserve_ll));
		delay_num = 0;
		for (int i = 0; i < MAX_DELAY_NUM; ++i)
		{
			delay_list[i].Reset();
		}
	}

	long long reserve_ll[4];
	int delay_num;
	RADelaySetTimeData delay_list[MAX_DELAY_NUM];
};

typedef char RADelaySetTimeParamHex[sizeof(RADelaySetTimeParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RADelaySetTimeParamHex) < 2048);

//-----------------------���ɫ����---------------------------------
struct ActivityRankBraveGroundRoleData
{
	void Reset()
	{
		role_id = 0;
		reached_level = 0;
		rand_param = 0;
		play_times = 0;
	}

	int role_id;
	int reached_level;
	int rand_param;
	int play_times;
};

struct ActivityChiefElectionRoleData
{
	void Reset()
	{
		base_prof = 0;
		capability = 0;
		memset(role_name, 0, sizeof(role_name));
		avator_type = 0;
		level = 0;
		headshot_id = 0;
		reserve_sh = 0;
	}

	int base_prof;		//��ֹ���ط�usercacheΪNULL����Ϸ��û����usercache �ǿյ�
	int capability;		//��ֹ���ط�usercacheΪNULL��
	GameName role_name;	//��ֹ���ط�usercacheΪNULL����Ϸ��û����usercache �ǿյ�
	short avator_type;	//��ֹ���ط�usercacheΪNULL����Ϸ��û����usercache �ǿյ�
	short level;		//��ֹ���ط�usercacheΪNULL����Ϸ��û����usercache �ǿյ�
	short headshot_id;  //��ֹ���ط�usercacheΪNULL����Ϸ��û����usercache �ǿյ�
	short reserve_sh;   //��ֹ���ط�usercacheΪNULL����Ϸ��û����usercache �ǿյ�
};

struct ActivityChiefPeakBattleRoleData
{
	void Reset()
	{
		bet_role_id = 0;
		bet_type = 0;
	}

	int bet_role_id;
	int bet_type;
};

struct ActivityHuanJieZhanChangRoleData
{
	void Reset()
	{
		rank_score = 0;
		max_rank_score = 0;
		winning_streak_times = 0;
		total_times = 0;
		win_times = 0;
		draw_times = 0;
		last_join_time = 0;
		memset(role_name, 0, sizeof(role_name));
		prof = 0;
		avatar_type = 0;
		is_finish_ding_ji = 0;
		today_get_huan_jie_coin = 0;
		today_win_times = 0;
		today_win_reward_flag = 0;
	}

	int rank_score;			//��ɫ����
	int max_rank_score;		//��ǰ�����ﵽ�����λ����
	int winning_streak_times;  //��ǰ��ʤ����
	int total_times;
	int win_times;
	int draw_times;
	unsigned int last_join_time;	//���μ�ʱ�䣬��ͬ������������
	GameName role_name;
	short prof;
	short avatar_type;
	int is_finish_ding_ji;			//�Ƿ�����˶�����
	int today_get_huan_jie_coin;	//�����ûý������
	int today_win_times;	//����ʤ��
	int today_win_reward_flag;	//����ʤ��������ȡ��־
};
const static int MAX_RELIC_LOOTING_REWARD_NUM = 4;
struct ActivityRelicLootingRoleData
{
	void Reset()
	{
		memset(param, 0, sizeof(param));
		memset(reward_status, 0, sizeof(reward_status));
		show_flag = 0;
	}
	enum RELIC_LOOTING_PARAM_TYPE
	{
		RELIC_LOOTING_PARAM_TYPE_0 = 0,	//ʤ������
		RELIC_LOOTING_PARAM_TYPE_1 = 1,	//��ǰ��ʤ
		RELIC_LOOTING_PARAM_TYPE_2 = 2,	//�����ʤ����
		RELIC_LOOTING_PARAM_TYPE_3 = 3, //ʧ�ܴ���
		RELIC_LOOTING_PARAM_TYPE_MAX,
	};

	int param[RELIC_LOOTING_PARAM_TYPE_MAX];
	int reward_status[MAX_RELIC_LOOTING_REWARD_NUM];//�Ƿ��Ѿ���ȡ[0-1]
	int show_flag;
};

union ActivityRoleData
{
	void Reset(int act_type)
	{
		switch (act_type)
		{
			case ACTIVITY_TYPE_RELIC_LOOTING:
				{
					relic_looting_role_data.Reset();
				}
			break;
			case ACTIVITY_TYPE_BRAVE_GROUND:
				{
					brave_ground_role_data.Reset();
				}
				break;
			case ACTIVITY_TYPE_CHIEF_ELECTION:
				{
					chief_election_role_data.Reset();
				}
				break;
			case ACTIVITY_TYPE_CHIEF_PEAK_BATTLE:
				{
					chief_peak_battle_role_data.Reset();
				}
				break;
			case ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG:
				{
					huan_jie_zhan_chang_role_data.Reset();
				}
				break;
			default:
				{
					memset(this, 0, sizeof(*this));
				}
				break;
		}
	}

	ActivityRelicLootingRoleData relic_looting_role_data;
	ActivityRankBraveGroundRoleData brave_ground_role_data;
	ActivityChiefElectionRoleData	chief_election_role_data;
	ActivityChiefPeakBattleRoleData chief_peak_battle_role_data;
	ActivityHuanJieZhanChangRoleData  huan_jie_zhan_chang_role_data;
};

typedef char ActivityRoleDataHex[sizeof(ActivityRoleData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ActivityRoleDataHex) < 512);

// ----------------------------- �ý�ս��----------------------------- //
static const int ACTIVITY_HUAN_JIE_ZHAN_CHANG_MAX_RAND_NUM = 6;   //�������ƥ������
static const int ACTIVITY_HUAN_JIE_ZHAN_CHANG_RANK_LIST_MAX_NUM = 100;   //ÿ�����а�����ϰ�����

struct ActivityHuanJieZhanChangRankItemCommon   //���а���
{
	ActivityHuanJieZhanChangRankItemCommon()
	{
		role_id = 0;
		memset(role_name, 0, sizeof(role_name));
		prof = 0;
		score = 0;
	}

	int role_id;
	GameName role_name;
	int prof;
	int score;
};

struct ActivityHuanJieZhanChangSaveData			//���
{
	int season_num;
};

//���޽���
const static int SHEN_SHOU_JIANG_LIN_BAO_DI_MAX_NUM = 20;		//Ҫ���׽�������
struct RandActivityShenShouJiangLinParam
{
	RandActivityShenShouJiangLinParam()
	{
		draw_times = 0;
		gold_num = 0;
		item_num = 0;
	}

	int draw_times;
	int gold_num;
	int item_num;
};

#pragma pack(pop)


static const int MAX_ACTIVIYT_TIME_CFG_NUM = 5;							// ÿ�������������
static const int MAX_ACTIVITY_OPEN_WEEKDAY = 7;							// ÿ��������������������
struct ActivityCfg
{
	ActivityCfg() : type(ACTIVITY_TYPE_INVALID), is_open(false), statustime_list_count(0)
	{
		memset(open_weekday_list, 0, sizeof(open_weekday_list));
	}

	bool Invalid() { return ACTIVITY_TYPE_INVALID == type; }

	int type;
	bool is_open;

	int open_weekday_list[MAX_ACTIVITY_OPEN_WEEKDAY];

	int statustime_list_count;
	StatusTimeCfgList statustime_list[MAX_ACTIVIYT_TIME_CFG_NUM];
};

// ��������

struct TianMingDivinationInfo
{
	TianMingDivinationInfo() :uid(0), rate_item_id(0), item_num(0)
	{
		memset(role_name, 0, sizeof(GameName));
	}

	TianMingDivinationInfo& operator=(const TianMingDivinationInfo &data)
	{
		uid = data.uid;
		F_STRNCPY(role_name, data.role_name, sizeof(GameName));
		rate_item_id = data.rate_item_id;
		item_num = data.item_num;
		return *this;
	}

	int uid;
	GameName role_name;
	int rate_item_id;
	int item_num;
};

//�Ͷ���������
struct RandActivityLaoDongHaoLiParam
{
	RandActivityLaoDongHaoLiParam()
	{
		refresh_box_time = 0;
		close_box_time = 0;
		next_refresh_timestamp = 0;
		notice_tamp = 0;
	}
	void Reset()
	{
		refresh_box_time = 0;
		close_box_time = 0;
		next_refresh_timestamp = 0;
		notice_tamp = 0;
	}
	time_t refresh_box_time;
	time_t close_box_time;
	time_t next_refresh_timestamp;
	time_t notice_tamp;
};
UNSTD_STATIC_CHECK(sizeof(RandActivityLaoDongHaoLiParam) <= sizeof(ActivityRoleDataHex));

#endif // __ACTIVITY_DEF_HPP__