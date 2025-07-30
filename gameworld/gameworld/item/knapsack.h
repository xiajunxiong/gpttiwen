#ifndef __KNAPSACK_H__
#define __KNAPSACK_H__

#include "item/itembase.h"
#include "item/itemgriddata.h"

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/basedef.h"
#include "servercommon/serverdef.h"
#include "money.h"
#include <vector>
#include <set>

class Role;

enum GET_REASON_TYPE
{
	GET_REASON_INVALID = 0,							// ��Ч
	GET_REASON_MONSTER_WAVE,						//ħ��
	GET_REASON_TREASURE_MAP_TASK,					//��ͼ������ɻ����Ʒ
	GET_REASON_CRYSTAL_FB_BOSS_REWARD,				//ˮ������ boss ���� param1:����
	GET_REASON_FANTASY_FB_COUNT_REWARD,				//����þ�����
	GET_REASON_XUN_BAO,								//Ѱ��
	GET_REASON_TASK,								//���� param1:task_id
	GET_REASON_SHI_TU_FB,							//ʦͽ����
	GET_REASON_RAND_GIFT,							//������
	GET_REASON_WILD_BOSS,							//Ұ��boss
	GET_REASON_FAST_PASS_CRYSTAL_FB,				//ɨ��ˮ������ param1:���飬param2������
	GET_REASON_SHAN_HAI_BOSS,						//ɽ������
	GET_REASON_ANECDOTE_TASK,						//��������
	GET_REASON_DREAM_NOTES_CHALLENGE_FINISH,		//��Ԩ��¼ͨ��
	GET_REASON_DREAM_NOTES_CHALLENGE_FAST_PASS,		//��Ԩ��¼ɨ��
	GET_REASON_TASK_CHAIN_REWARD,					//���˵ķ���
	GET_REASON_EQUIP_DECOMPOSE,						//װ���ֽ�
	GET_REASON_MIJIQIWEN,							//�ؾ�����
	GET_REASON_PARTNER_LIENCE,						//���ƾ֤
	GET_REASON_MATERIAL_FAST_PASS,					//���龳 param1: 1.ͨ�� 0.ɨ��
	GET_REASON_ACTIVE_FA_BAO_GET_ITEM,				//�Ѿ��������õ���
	GET_REASON_ADD_GOD_PRINT_ITEM,					//�����ӡ����
	GET_REASON_PET_KEY_LEVEL_UP,					//����һ������
	GET_REASON_TIAN_TI_DAO_HEN,						//��ص���
	GET_REASON_PRESTIGE_TASK,						//��������
	GET_REASON_HUNDRED_GHOST,						//�ٹ�ҹ��
	GET_REASON_ACTVITIY_REWARD,						//��Ӫ����� param1:�ID
	GET_RAASON_ACTIVE_TURNTABLE,					//��Ծת��
	GET_REASON_TASK_CHAIN_COMPLETION,				//���˵ķ���-���ս���
	GET_REASON_DU_JIE,								//�ɽ�
	GET_REASON_REALM_BREAK_FIGHT,					//����ս��������֮����
	GET_REASON_HORCRUX_TIANCHENG,					//�������
	GET_REASON_ENDLESS_TOWER,						//�޾�֮��
	GET_REASON_ZAI_ZHAN_YUN_ZE,						//��ս����
	GET_REASON_YUNZE_ER_SAN_SHI,					//���������
	GET_REASON_JIXING_GAOZHAO,						//���Ǹ���
	GET_REASON_WAN_LING_FB,							//���黰��
	GET_REASON_YUNZE_SLOT_MACHINE,					//����ͬ����
	GET_REASON_BOUNTY_TASK_REWARD,					//�ͽ�ת�̽����ܽ���չʾ
	GET_REASON_LIFE_SKILL_MAKE,						//����ְҵ�������� P1:�����/ְҵ����
	GET_REASON_MIRAGE_TERRITORY,					//�����׾�
	GET_REASON_TREASURE_TASK,						//�����ر�
	GET_REASON_TREASURE_TASK_TOTAL_ITEM,			//�����ر� ������Ʒͳ��
	GET_REASON_FALLEN_GOD_REWARD,					//����֮��
	GET_REASON_WEDDING_SUB_ACT_XIN_XIN_XIANG_YIN,	//����-С��Ϸ ������ӡ/��������  p_1:ͭ��
	GET_REASON_PARTNER_PACT,						//���--Ԫ����Լ �����ʾ
	GET_REASON_SHI_TU_CHUAN_GONG,					//ʦͽ���� p_1:��ǰ�÷�  p_2:������ߵ÷�
	GET_REASON_WHERE_IS_THE_FAIRY,					//����ȥ�Ķ�
	GET_REASON_LANTERN_RIDDIE,						//ɽ���ƻ�
	GET_REASON_MONEY_BOX,							//���Һ�� p_1:�������� p_2:��������
	GET_REASON_LIANHUAN_TASK,						//����֧��������
	GET_REASON_RESERVE,								//����
	GET_REASON_YUAN_QI_YAN_HUO_OPEN,				//Ԫ���̻� ���̻�
	GET_REASON_YUAN_QI_FAN_PAI,						//Ԫ������ ����
	GET_REASON_BAO_ZI_LAI_LE,						//��������,p1:�Ƿ�ɹ�(1-�ɹ�,0-ʧ��)
	GET_REASON_HONGMENG_TIANJIE_PASS,				//������� p1:�Ƿ���ս
	GET_REASON_ZAO_HUA_DISCOMPOSE,					//�컯װ���ֽ�
	GET_REASON_DUAN_YANG_XIANG_NANG,				//��������
	GET_REASON_LAN_TANG_CHU_MU,						//��������
	GET_REASON_KE_JU_EXAM,							//�ƾٿ���
	GET_REASON_TIAN_YI_YAO_SHI,						//����ҫ��
	GET_REASON_SMART_MOUNTS_ACTIVE_CONVERT,			//�����������ת��
	GET_REASON_YUE_XI_DENG_MI,						//��Ϧ���� 
	GET_REASON_TEAM_CHALLENGE_REWARD,				//��ս������
	GET_REASON_HUAN_LING_DECOMPOSE,					//����ϵͳ�ֽ�
};


enum SEND_MAIL_TYPE
{
	SNED_MAIL_TYPE_DEFAULT = 0,								// Ĭ��֪ͨ��������,�����ʼ��·�
	SEND_MAIL_TYPE_SEVEN_COLOR = 1,							// �߲�ͼ����������
	SEND_MAIL_TYPE_SEVEN_COLOR_COUNT = 2,					// �߲�ͼ���ܽ���
	SEND_MAIL_TYPE_SIGN_UP = 3,								// ��ʱ�����
	SEND_MAIL_TYPE_GUILD_BOSS_CHAPTER_KILL = 4,				// ����������ɱ����
	SEND_MAIL_TYPE_GUILD_BOSS_CHAPTER_NOTICE = 5,			// ��������֪ͨ(�Ѳ��ã�
	SEND_MAIL_TYPE_TASK_REISSUE_ITEM = 6,					// ������߲���
	SEND_MAIL_TYPE_MAKE_UP_REWARD = 7,						// ��Ӫ�-���뽱��
	SEND_MAIL_TYPE_COURAGE_CHALLENGE = 8,					// ��ħ֮������������
	SEND_MAIL_TYPE_DREAM_NOTES_CHALLENGE = 9,				// ��Ԩ��¼����������
	SEND_MAIL_TYPE_GOOD_GIFT_TURN = 10,						// ����ת������������
	SEND_MAIL_TYPE_HELP_OTHERS = 11,						// �嵨���˱���������
	SEND_MAIL_TYPE_SECRET_TREASURE = 12,					// �ػ��ر�����������
	SEND_MAIL_TYPE_LUN_HUI_WANG_CHUAN = 13,					// �ֻ���������������
	SEND_MAIL_TYPE_RELIC_LOOTING = 14,						// �ż��ᱦ����������
	SEND_MAIL_TYPE_TOP_LEVEL = 15,							// �۷�ȼ�δ��ȡ����
	SEND_MAIL_TYPE_FIRST_RECHARGE_EXCLUSIVE = 16,			// �׳�ר������������
	SEND_MAIL_TYPE_MONTH_CARD_BUY_CARD_OR_DAY_REWARD = 17,	// �¿�/�ܿ�����/ÿ�ս�������������
	SEND_MAIL_TYPE_MONTH_CARD_BUY_ITEM = 18,				// �����¿��̳ǵ��߱���������
	SEND_MAIL_TYPE_ACTIVITY_REWARD = 19,					// �����,������������(ͨ��)
	SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP = 20,					// ����,����������(ͨ��)
	SEND_MAIL_TYPE_FANG_FRAUD_REWARD = 21,					// ��ȡ��թƭ��������������
	SEND_MAIL_TYPE_BACK_REWARD = 22,						// �ع���񱳰�������
	SEND_MAIL_TYPE_SERVER_COMPETITION = 23,					// ȫ����ƴ���������
	SEND_MAIL_TYPE_ACTIVITY_END = 24,						// �����,�����δ��ʱ��ȡ���� 
	SEND_MAIL_TYPE_STONE_BACK = 25,							// ��ʯ��Ƕ�����޸ĺ�ж�µͽ�װ����δ�ﵽ������ʯ
	SEND_MAIL_TYPE_HELP_OTHERS_2 = 26,						// �嵨����2����������
	SEND_MAIL_TYPE_SHAN_HAI_QUESTION = 27,					// ɽ���ʴ𱳰�������
	SEND_MAIL_TYPE_SHAN_HAI_MEET = 28,						// ɽ����걳��������
	SEND_MAIL_TYPE_FATE_ENTANGLEMENT = 29,					// ɽ��Ե�𱳰�������
	SEND_MAIL_TYPE_YUN_ZE_TERRITORY = 30,					// ���󻭾�����������
	SEND_MAIL_TYPE_YUN_ZE_SLOT_MACHINE = 31,				// ����ͬ���񱳰�������
	SEND_MAIL_TYPE_YUN_ZE_ER_SAN_SHI = 32,					// ��������±���������
	SEND_MAIL_TYPE_YUN_ZE_BLOOMING_RECORD = 33,				// ���󷱻�¼����������
	SEND_MAIL_TYPE_YUNZE_CATCH_ELVES = 34,					// �����ץ������������
	SEND_MAIL_TYPE_YUNZE_HAO_LI = 35,						// ������񱳰�������
	SEND_MAIL_TYPE_RA_XIAN_SHI_LEI_CHONG = 36,				// ������ʱ�۳䱳��������
	SEND_MAIL_TYPE_YUNZE_WISH = 37,							// ������Ը����������
	SEND_MAIL_TYPE_YUNZE_CUTE_PET = 38,						// �ȳ�ݵ�����������
	SEND_MAIL_TYPE_TREASURE_TASK = 39,						// �����ر�����������
	SEND_MAIL_TYPE_TIGER_FISH_EVERY_YEAR = 40,				// �������㱳��������
	SEND_MAIL_TYPE_TIGER_FIND_SPRING = 41,					// ����̽������������
	SEND_MAIL_TYPE_TIGER_TEASE_NEW_YEAR = 42,				// �������´�����������
	SEND_MAIL_TYPE_TIGER_LUCKY_MONEY = 43,					// ����ѹ�걳��������
	SEND_MAIL_TYPE_TIGER_AUSPICIOUS_SNOW = 44,				// ��ѩ���걳��������
	SEND_MAIL_TYPE_TIGER_FIND_SPRING_YESTERDAY = 45,		// ����̽��ǰһ��δ��ȡ��������
	SEND_MAIL_TYPE_JINGJI_YUEKA_REISSUE_REWARD = 46,		// �����¿���������
	SEND_MAIL_TYPE_RONG_LIAN_YOU_LI = 47,					// ����������δ��ȡ����
	SEND_MAIL_TYPE_WORLD_ARENA_WIN_BOX_REWARD = 48,			// �۽�����ʤ�����佱������
	SEND_MAIL_TYPE_WORLD_ARENA_SIGNUP_REWARD = 49,			// �۽����ݱ�����������
	SEND_MAIL_TYPE_WORLD_ARENA_DAILY_REWARD = 50,			// �۽�����ÿ�ս�������
	SEND_MAIL_TYPE_PET_TALENT_RESET_RETURN = 51,			// �����츳���÷���
	SEND_MAIL_TYPE_SAN_XIAN_GUI_DONG_REWARD = 52,			// ���ɹ鶴����������
	SEND_MAIL_TYPE_TIAN_GONG_XUN_BAO_REWARD = 53,			// �칬Ѱ������������
	SEND_MAIL_TYPE_HUNDRED_GOD_BOSS_TIME_OVER = 54,			// ����֮ʦ��ʱ����
	SEND_MAIL_TYPE_HUNDRED_GOD_BOSS_REWARD_REISSUE = 55,	// ����֮ʦ��������
	SEND_MAIL_TYPE_TIGER_ACTIVITY = 56,						// �ʼ�������ŵı���������
	SEND_MAIL_TYPE_GOD_BEAST_ADVENT = 57,					// ������������������
	SEND_MAIL_TYPE_GOD_BEAST_ADVENT_YESTERDAY = 58,			// ������������ǰһ�콱��
	SEND_MAIL_TYPE_BO_ZHONG_YOU_LI = 59,					// �������񱳰�������
	SEND_MAIL_TYPE_YUAN_QI_FAN_PAI = 60,					// Ԫ�����Ʊ���������
	SEND_MAIL_TYPE_BAO_ZI_LAI_LE = 61,						// �������˱���������
	SEND_MAIL_TYPE_MIAO_BI_SHENG_HUA = 62,					// �����������������
	SEND_MAIL_TYPE_CHENG_FENG_PO_LANG = 63,					// �˷����˱���������
	SEND_MAIL_TYPE_PINJIA_YOULI = 64,						// �������񱳰�������
	SEND_MAIL_TYPE_SALTY_SWEET_BATTLE = 65,					// ����֮������������
	SEND_MAIL_TYPE_DUAN_YANG_XIANG_NANG = 66,				// �������ұ���������
	SEND_MAIL_TYPE_CANG_LONG_CI_FU = 67,					// �����͸�����������
	SEND_MAIL_TYPE_LAN_TANG_CHU_MU = 68,					// �������屳��������
	SEND_MAIL_TYPE_ACTIVITY_REWARD_BAG_FULL = 69,			// �����,������������(ͨ��)(��19��ͬ���ı�)
	SEND_MAIL_TYPE_ACTIVITY_END_REWARD_REISSUE = 70,		// �����,�����δ��ʱ��ȡ����(ͨ��)(��24��ͬ�ı����ı�)
	SEND_MAIL_TYPE_ACTIVITY_BUY_BAG_FULL = 71,				// �����,����������
	SEND_MAIL_TYPE_COLOSSEUM_BATTLE_REWARD = 72,			// ������ս����������������
	SEND_MAIL_TYPE_KUA_FU_YING_XING_LU_BAG_FULL = 73,		// ���Ӣ��¼����,����������
	SEND_MAIL_TYPE_KUA_FU_YING_XING_LU_RANK_END_TASK = 74,	// ���Ӣ��¼����,�񵥽���,����δ��������
	SEND_MAIL_TYPE_NEW_COURAGE_CHALLENGE = 75,				// ߱��֮������������
	SEND_MAIL_TYPE_GUILD_INBORN_SKILL_RESET = 76,			// ���������������÷���

	SEND_MAIL_TYPE_MAX,
};

enum PUT_REASON_TYPE
{
	PUT_REASON_INVALID = 0,							//!< 0 ��Ч

	PUT_REASON_NO_NOTICE,							//!< 1 ��֪ͨ
	PUT_PET_CONVERSION_EXP,							//!< 2 ���ﾭ��ת��
	PUT_PET_REBIRTH,								//!< 3 ���ﷵ��
	PUT_REASON_GM,									//!< 4 GM����
	PUT_REASON_DEPOSIT,								//!< 5 ����ֿ�
	PUT_REASON_WITHDRAW_STORAGE,					//!< 6 ȡ���ֿ�
	PUT_REASON_WITHDRAW_TEMPORARY,					//!< 7 ȡ����ʱ����
	PUT_REASON_WITHDRAW_ALL_TEMPORARY,				//!< 8 ȡ����ʱ����ȫ��
	PUT_REASON_PRESTIGE_BUY,						//!< 9 �����̵�
	PUT_REASON_COLLECTION_ONCONVERT,				//!< 10 ��Ʒ�һ�
	PUT_REASON_GIFT_RAND_ITEM,						//!< 11 ������(��)
	PUT_REASON_TASK_REWARD,							//!< 12 ���񱨳�
	PUT_REASON_ACHIEVEMENT_REWARD,					//!< 13 �ɾ�
	PUT_REASON_DAY_REWARD,							//!< 14 ��������
	PUT_REASON_WORKSHOP_COMPOUND,					//!< 15 �����ϳ�
	PUT_REASON_GUILD_EXPLORE,						//!< 16 ����̽��
	PUT_REASON_GUILD_BOSS,							//!< 17 ����Boss
	PUT_REASON_MAIL_RECEIVE,						//!< 18 �ʼ�
	PUT_REASON_SYSTEM_NOTICE_REWARD,				//!< 19 ����Ԥ�潱��
	PUT_REASON_SYSTEM_COURAGE_CHALLENGE,			//!< 20 ������ս
	PUT_REASON_RESERVE,								//!< 21 �ѷ���������������
	PUT_REASON_SYSTEM_MONSTER_WAVE,					//!< 22 ħ��
	PUT_REASON_GATHER_OBJECT,						//!< 23 �ɼ�
	PUT_REASON_GUILD_SHOP,							//!< 24 �����̵�
	PUT_REASON_WILD_BOSS,							//!< 25 Ұ��boss 
	PUT_REASON_MEDAL,								//!< 26 ѫ��
	PUT_REASON_MEDAL_PUT_ON,						//!< 27 ѫ�´���������
	PUT_REASON_MI_YI,								//!< 28 ��ҽ
	PUT_REASON_SCHOOL_TASK,							//!< 29 ��������
	PUT_REASON_DROP,								//!< 30 ����
	PUT_REASON_ESCORT,								//!< 31 ����
	PUT_REASON_TREASURE_MAP,						//!< 32 �ѵ���ͼ/��������
	PUT_REASON_TRADE_MARKET_BUY,					//!< 33 ����������
	PUT_REASON_FIND_REWARD,							//!< 34 �����һ�
	PUT_REASON_TASK_CHAIN,							//!< 35 ������
	PUT_REASON_FABAO_DECOMPOSE,						//!< 36 �����ֽ�
	PUT_REASON_CRYSTAL_FB,							//!< 37 ˮ������
	PUT_REASON_SHOP,								//!< 38 �̵�
	PUT_REASON_XUN_BAO,								//!< 39 Ѱ��
	PUT_REASON_EXCHANGE,							//!< 40 �һ�
	PUT_REASON_BRAVE_GROUND,						//!< 41 �´��ع�
	PUT_REASON_RA_TIME_LIMIT_STORE,					//!< 42 ����-��ʱ�̵�
	PUT_REASON_MARRIAGE_TASK,						//!< 43 ��Ե�ճ�����
	PUT_REASON_GUILD_FIGHT,							//!< 44 ������Ҷ�
	PUT_REASON_COLOSSEUM,							//!< 45 ������
	PUT_REASON_GUILD_ANSWER,						//!< 46 �������
	PUT_REASON_OPTIONAL_GIFT,						//!< 47 ��ѡ���
	PUT_REASON_CHALLENGE_FB,						//!< 48 ��ս����
	PUT_REASON_TRADE_MARKET_OTHER,					//!< 49 ���������ǹ���
	PUT_REASON_DEF_GIFT,							//!< 50 �̶����
	PUT_REASON_RAND_GIFT,							//!< 51 ������
	PUT_REASON_CHONGZHI_FANLI,						//!< 52 ��ֵ����
	PUT_REASON_FUNC_GUIDE,							//!< 53 �������� / �����¼�
	PUT_REASON_GUIDE_FB,							//!< 54 ָ������
	PUT_REASON_MAZERACER,							//!< 55 �Թ�����
	PUT_REASON_TREVIFOUNTION,						//!< 56 ��Ը��
	PUT_REASON_LOW_TREASURE_MAP,					//!< 57 �ͼ���ͼ
	PUT_REASON_NPC_SHOP,							//!< 58 npc����̵�
	PUT_REASON_CONVERT_SHOP,						//!< 59 �һ��̵�
	PUT_REASON_GIFT_OPTIONAL_ITEM,					//!< 60 ��ѡ���(��)

	PUT_REASON_CATCH_PET,							//!< 61 �������
	PUT_REASON_PET_ABANDONMENT_SHARDS,				//!< 62 ���������ȡ����Ƭ

	PUT_REASON_DECOMPOSE_SURFACE,					//!< 63 �ֽ�ʱװ / ʱװ�̵�һ��ظ�ת�� /���Ƥ��/����Ƥ��
	PUT_REASON_RA_TURN_TABLE,						//!< 64 ����-����ת��
	PUT_REASON_RA_DISCOUNT_STORE,					//!< 65 ����-�ۿ��̵�
	PUT_REASON_CATCH_PET_SHARD,						//!< 66 �������õ���Ƭ����
	PUT_REASON_RA_MYSTERIOUS_TRIAL,					//!< 67 ����-��������
	PUT_REASON_GUILD_BOSS_CHAPTER_REWARD,			//!< 68 ���������½ڽ���
	PUT_REASON_RA_CONSUME_CAROUSEL,					//!< 69 ����-����ת��
	PUT_REASON_RA_FIRST_CHONGZHI_YIXIANER,			//!< 70 �׳�
	PUT_REASON_RA_LOGIN_SYSTEM_REWARD,				//!< 71 �ۼƵ�½�
	PUT_REASON_BOSS_CHAPTER_BOSS_REWARD,			//!< 72 BOSS�½�BOSS����
	PUT_REASON_BOSS_CHAPTER_CHAPTER_REWARD,			//!< 73 BOSS�½��½ڽ���
	PUT_REASON_BOUNTY_TASK_TURN_TABLE_REWARD,       //!< 74 �ͽ�����ת�̽���
	PUT_REASON_SERVER_FIRST_KILL,					//!< 75 ȫ����ɱ����
	PUT_REASON_TRANSACTION_TASK_REWARD,				//!< 76 �������� - ˿��֮·����(Զ��ó��)
	PUT_REASON_TASKCHAIN_DROP_SCENE,				//!< 77 �������׵���
	PUT_REASON_ACTIVE_SYSTEM_REWARD,				//!< 78 ��Ծϵͳ����
	PUT_REASON_FANTASY_FB,							//!< 79	����þ�����
	PUT_REASON_WORLD_QUESTION_REWARD,				//!< 80 ������⽱��
	PUT_REASON_RA_HERITAGE_TREASURE,				//!< 81 ����-���й屦
	PUT_REASON_EXPEL_MONSTER_BUY_ITEM,				//!< 82 �������޻������Ʒ
	PUT_REASON_ADVANCE_REWARD,						//!< 83 ���׸���
	PUT_REASON_SALE_SHOP_BUY,						//!< 84 �����̵�
	PUT_REASON_HAPPY_UP_EXCHANGE,					//!< 85 ��������
	PUT_REASON_RA_LUCKY_BAG,						//!< 86 ����-����
	PUT_REASON_RA_TIAN_JIANG_HAO_LI,				//!< 87 �콵����
	PUT_REASON_EXPEL_MONSTER,						//!< 88 ��������
	PUT_REASON_MEDAL_EXCAHNG_MATERIAL,				//!< 89 ѫ�¶һ�����
	PUT_REASON_JOY_SEEKS_FAVORS,					//!< 90 ����Ѱ��
	PUT_REASON_JIAN_DAN_QIN_XIN,					//!< 91 ��������
	PUT_REASON_ONLY_FIGHT,							//!< 92 ��һ�а�(���˰������Ҷ�)
	PUT_REASON_TEAM_FIGHT,							//!< 93 С�Ӿ���
	PUT_REASON_LINKAGE_GIFT,						//!< 94 ��������
	PUT_REASON_RELATED_TO_THE_EVENT,				//!< 95 �����(������һ����)
	PUT_REASON_RA_PARTNER_EXCHANGE,					//!< 96 ���һ�
	PUT_REASON_RA_COLLECT_WORD_EXCHANGE,			//!< 97 ���ֻ�һ� / ÿ�����
	PUT_REASON_RA_DISCOUNT_CARNIVAL,				//!< 98 �ۿۿ�
	PUT_REASON_RA_LUCKY_DISCOUNT,					//!< 99 �����ۿ�
	PUT_REASON_RA_PET_EXCHANGE,						//!< 100 ����һ�
	PUT_REASON_LUCKY_TURNTABLE,						//!< 101 ����ת��
	PUT_REASON_RA_ADVENTURE_GIFT,					//!< 102 ����һ�
	PUT_REASON_SURFACE_SHOP,						//!< 103 ʱװ�̵�
	PUT_REASON_RA_QUAN_MIN_DISCOUNT,				//!< 104 ȫ���Ź�
	PUT_REASON_RAND_GIFT_PET_EGG,					//!< 105 ���������ﵰ
	PUT_REASON_OPTIONAL_GIFT_PET_EGG,				//!< 106 ��ѡ������ﵰ
	PUT_REASON_DEF_GIFT_PET_EGG,					//!< 107 �̶�������ﵰ
	PUT_REASON_RA_SPROTS_REWARD,					//!< 108 �·�����
	PUT_REASON_PET_EQUIPMENT_TAKEOFF,				//!< 109 ����װ������
	PUT_REASON_PARTNER_ELEMENT_TAKEOFF_DEFAULT,		//!< 110 Ԫ��ˮ��ж��(δ��)
	PUT_REASON_DECOMPOSE_MEDAL,						//!< 111 �ֽ�ѫ��
	PUT_REASON_RA_TU_JIAN_TREASURE,					//!< 112 ͼ���ᱦ
	PUT_REASON_PARTNER_ELEMENT_TAKEOFF_BAD,			//!< 113 Ԫ��ˮ��ж��(��)
	PUT_REASON_RA_GOD_TOKEN,						//!< 114 ����ս��
	PUT_REASON_FUNC_PREVIEW,						//!< 115 ����Ԥ��
	PUT_REASON_SEVEN_COLOR_PICTURE,					//!< 116 �߲�ͼ��
	PUT_REASON_RA_ELEMENT_CARDS,					//!< 117 Ԫ�ط���
	PUT_REASON_SHI_TU_FB,							//!< 118 ʦͽ����
	PUT_REASON_RA_SIGN_IN_CONTINUITY,				//!< 119 ���Ǻ���
	PUT_REASON_WORKSHOP_BATCH_COMPOUND,				//!< 120 ������������
	PUT_REASON_GUILD_GATHER_DREAM,					//!< 121 ���弯Ը��
	PUT_REASON_MYSTERY_STORE,						//!< 122 �����̵꣨�����̵꣩
	PUT_REASON_COMPUOUND_WORK_SHOP,					//!< 123 �����һ����ϳ�
	PUT_REASON_CREATE_ROLE,							//!< 124 �����������
	PUT_REASON_SHAN_HAI_BOSS,						//!< 125 ɽ������
	PUT_REASON_RA_CONSUME_CAROUSEL_ALL,				//!< 126 ����-����ת��ת��ȫ��
	PUT_REASON_DING_ZHI_BAO_XIANG,					//!< 127 ���Ʊ���
	PUT_REASON_ZHUAN_SHU_LI_BAO,					//!< 128 ר�����
	PUT_REASON_LIU_LI_DING,							//!< 129 ������
	PUT_REASON_XIAN_SHI_BAO_HE,						//!< 130 ��ʱ����
	PUT_REASON_XIAN_SHI_BAO_DAI,					//!< 131 ��ʱ����
	PUT_REASON_CRYSTAL_FB_FAST_PASS,				//!< 132 ˮ������ɨ��
	PUT_REASON_ANECDOTE_TASK,						//!< 133 ��������
	PUT_REASON_PARTNER_BACK_REWARD,					//!< 134 ����������
	PUT_REASON_RA_GIVE_PET_DRAW,					//!< 135 ��30���飨��Ѱ�����飩
	PUT_REASON_DRAW_MANAGER,						//!< 136 ���ƹ�����
	PUT_REASON_RA_BONUS,							//!< 137 ��Ӫ-���⽱��
	PUT_REASON_RA_CI_JIU_YING_XIN,					//!< 138 �Ǿ�ӭ�½���
	PUT_REASON_RA_NIU_XING_DA_YUN,					//!< 139 ţ�д��˽���
	PUT_REASON_RA_SPRING_FESTIVAL_STORE,			//!< 140 ����ڽ���
	PUT_REASON_RA_COW_EXCHANGE_STORE,				//!< 141 ţ�˶һ�����
	PUT_REASON_RA_MONPOLY_GIFT,						//!< 142 ţ�˵�ͷ����
	PUT_REASON_RA_COW_LUCKY_EXCHANGE,				//!< 143 ţ���ޱ߽���
	PUT_REASON_RA_LITTLE_YEAR_GIFT,					//!< 144 С������
	PUT_REASON_RA_COW_STORE,						//!< 145 ţ���̳�
	PUT_REASON_BUG_REPAIR,							//!< 146 BUG�޸�
	PUT_REASON_BRAVE_GROUND_SAO_DANG,				//!< 147 �´��ع�ɨ��
	PUT_REASON_GIVE_SECRET_KEY,						//!< 148 ����Ѱ������
	PUT_REASON_PET_LOOT,							//!< 149 ����ᱦ
	PUT_REASON_DREAM_NOTES_CHALLENGE,				//!< 150 ��Ԩ��¼
	PUT_REASON_STAR_TREASURE_MAP,					//!< 151 �ǳ��ر�ͼ
	PUT_REASON_HIGH_TREASURE_MAP,					//!< 152 �߼��ر�ͼ
	PUT_REASON_SHEN_SHOU_JIANG_LIN,					//!< 153 ���޽���
	PUT_REASON_SCHOOL_EXERCISE,						//!< 154 У������
	PUT_REASON_CHIEF_ELECTION,						//!< 155 ��ϯ��ѡ
	PUT_REASON_NOREXITEM_EQUIP,						//!< 156 δ����װ��
	PUT_REASON_PET_BREAKTHROUGH,					//!< 157 ����ǿ���Ե�����
	PUT_REASON_GOOD_GIFT_TURN,						//!< 158 ����ת��
	PUT_REASON_HELP_OTHERS,							//!< 159 �嵨����
	PUT_REASON_SECRET_TREASURE,						//!< 160 �ػݹ屦
	PUT_REASON_CHUN_RI_DENG_FENG,					//!< 161 ���յǷ�
	PUT_REASON_MI_JING_QI_WEN_FB,					//!< 162 �ؾ����Ÿ���
	PUT_REASON_ZU_HE_HE_LI,							//!< 163 ��Ϻ���
	PUT_REASON_LING_PAI_TIAN_CI,					//!< 164 �������
	PUT_REASON_FASHION_SHOP,						//!< 165 ʱװ�̵�
	PUT_REASON_PARTNER_BIOGRAPHY,					//!< 166 ��鴫��
	PUT_REASON_NOREXITEM_ITEM,						//!< 167 �������ĵ���
	PUT_REASON_ACTIVE_SYSTEM_EXTRA_REWARD,			//!< 168 ��Ծϵͳ���⽱��
	PUT_REASON_SMART_MOUNTS_SYSTEM,					//!< 169 ����ϵͳ
	PUT_REASON_REALM_BREAK,							//!< 170 ����ϵͳͻ��
	PUT_REASON_PET_MONOPOLY,						//!< 171 ����ؾ�
	PUT_REASON_LUN_HUI_WANG_CHUAN,					//!< 172 �ֻ�����
	PUT_REASON_MATERIAL,							//!< 173 ���龳(���ϸ���)
	PUT_REASON_MATERIAL_ACHIEVEMENT_REWARD,			//!< 174 ���龳�ɾͽ���(���ϸ���)
	PUT_REASON_CUMULATIVE_RECHARGE,					//!< 175 �ۼƳ�ֵ(��װ����)
	PUT_REASON_FA_BAO_DRAW_REWARD,					//!< 176 �����齱
	PUT_REASON_CLOUD_ARENA_RANK_REWARD,				//!< 177 �ƶ˾�����λ����
	PUT_REASON_PLATFORM_ACHIEVEMENT_REWARD,			//!< 178 ��̨ս�ɾͽ���
	PUT_REASON_CLOUD_ARENA_ACHIEVE_REWARD,			//!< 179 �ƶ˾����ɾͽ���
	PUT_REASON_RELIC_LOOTING_REWARD,				//!< 180 �ż��ᱦ�ɾͽ���
	PUT_REASON_CRYSTAL_FB_RETURN_TICKET,			//!< 181 ��ͨ�ؾ�������Ʊ
	PUT_REASON_MAP_EXPLORATION_REWARD,				//!< 182 ��ͼ̽������
	PUT_REASON_MARKET,								//!< 183 �г�
	PUT_REASON_CHIVALROUS_ACHIEVE,					//!< 184 ����ֵ�ɾ�
	PUT_REASON_NOREX_CONVERT_ITEM,					//!< 185 ��������ʹ��
	PUT_REASON_TRANSFER_KNAPSACK_COLUMN,			//!< 186 ת�Ʊ�����Ʒ����ȷ����
	PUT_REASON_MI_YI_FETCH_WEEK_KILL_REWARD,		//!< 187 ��ҽ��ȡ����ɱ����
	PUT_REASON_RESEARCH_TASK_REWARD,				//!< 188 �����ħ������
	PUT_REASON_ROLE_JOIN_ACTIVITY_REWARD,			//!< 189 ��ɫ������ʱ�����
	PUT_REASON_SHEN_SHOU_JIANG_LIN_BUY,				//!< 190 ���޽���ֱ��
	PUT_REASON_GUILD_ACTIVE_DAY_GIFT,				//!< 191 �����ջ�Ծ�����
	PUT_REASON_ACHIEVEMENT_PROG_REWARD,				//!< 192 �ɾͽ׶ν���
	PUT_REASON_PET_SOUL_EQUIPMENT_TAKEOFF,			//!< 193 �����������
	PUT_REASON_TOP_LEVEL,							//!< 194 �۷�ȼ�
	PUT_REASON_GOD_PRINT_WISH_REWARD,				//!< 195 ��ӡ����
	PUT_REASON_GOD_PRINT_EXCHANGE_REWARD,			//!< 196 ��ӡ�һ�
	PUT_REASON_NETHER_WORLD_PURGATORY_REWARD,		//!< 197 ��ڤ����
	PUT_REASON_ZHU_LING_DECOMPOSE,					//!< 198 ����ֽ�
	PUT_REASON_PET_GOD_FB,							//!< 199 ��ӡ֮��
	PUT_REASON_HEART_SKILL_COMPOSE,					//!< 200 �ķ��ϳ�
	PUT_REASON_HEART_SKILL_CONVERT,					//!< 201 �ķ�ת��
	PUT_REASON_ZHU_LING_UP_LEVEL,					//!< 202 ��������
	PUT_REASON_ZHU_LING_PUT_ON,						//!< 203 ���鴩���滻����
	PUT_REASON_ZHU_LING_TAKE_OFF,					//!< 204 ��������
	PUT_REASON_FIRST_RECHARGE_EXCLUSIVE,			//!< 205 �׳�ר��
	PUT_REASON_MONTH_CARD_REWARD,					//!< 206 �ܿ�/�¿�
	PUT_REASON_BIG_DIPPER_REWARD,					//!< 207 ��������
	PUT_REASON_ONE_KEY_GRADE_MEDAL,					//!< 208 һ����������
	PUT_REASON_TIAN_DI_DAO_HEN_ACTIVE,				//!< 209 ��ص��۽���
	PUT_REASON_TIAN_DI_DAO_HEN_FINAL,				//!< 210 ��ص������ս���
	PUT_REASON_TIAN_DI_DAO_HEN_DROP,				//!< 211 ��ص������ս���
	PUT_REASON_FETCH_GIVE_GIFT,						//!< 212 ��ȡ��������
	PUT_REASON_FETCH_RESOURCE_DOWNLOAD_REWARD,		//!< 213 ��ȡ��Դ���ؽ���
	PUT_REASON_EXCHANGE_PET_SOUL_EQUIP,				//!< 214 �����һ�
	PUT_REASON_PRESTIGE_TASK,						//!< 215 ��������
	PUT_REASON_HUNDRED_GHOST,						//!< 216 �ٹ�ҹ��
	PUT_REASON_SUMMER_LI_LIAN,						//!< 217 ��������
	PUT_REASON_COLORFUL_LANTERN,					//!< 218 ��ʻ���
	PUT_REASON_SUMMER_STORE,						//!< 219 �����̵�
	PUT_REASON_LUCKY_SIGN,							//!< 220 ���˻�ǩ
	PUT_REASON_QIONG_LOU_YU_YAN,					//!< 221 ��¥����
	PUT_REASON_SUPER_OFF_SHOP,						//!< 222 �����ۿ��̳�
	PUT_REASON_YUAN_QI_TURNING,						//!< 223 Ԫ������
	PUT_REASON_COOL_BREEZE,							//!< 224 ������ˬ
	PUT_REASON_LOTUS_MI_BAO,						//!< 225 �����ٱ�
	PUT_REASON_MAZE_SUMMER,							//!< 226 �����Թ�
	PUT_REASON_MI_BAO_FANG_DIAN,					//!< 227 �ٱ�����
	PUT_REASON_ZHEN_PIN_SHANG_PU,					//!< 228 ��Ʒ����
	PUT_REASON_FANG_FRUAD_REWARD,					//!< 229 ��թƭ����
	PUT_REASON_PET_SOUL_EQUIP_CHANGE,				//!< 230 �������
	PUT_REASON_ATTR_PELLET_RET,						//!< 231 ���Ե�����
	PUT_REASON_ACTIVE_TURNTABLE,					//!< 232 ��Ծת��
	PUT_REASON_PARTNER_PACT_GET,					//!< 233 ���--Ԫ����Լ--���
	PUT_REASON_PARTNER_PACT_PUT_ON,					//!< 234 ���--Ԫ����Լ--װ��
	PUT_REASON_PARTNER_PACT_UP_STAR,				//!< 235 ���--Ԫ����Լ--����
	PUT_REASON_RA_BACK_REWARD,						//!< 236 �汾�-�ع����
	PUT_REASON_DUEL,								//!< 237 ����
	PUT_REASON_RA_HAPPY_LOOK,						//!< 238 �����ַ���
	PUT_REASON_SKILL_BOOK_CONVERT,					//!< 239 ����������һ� 
	PUT_REASON_SERVER_COMPETITION,					//!< 240 ȫ����ƴ
	PUT_REASON_DU_JIE,								//!< 241 �ɽ�
	PUT_REASON_YIN_YANG_EYE,						//!< 242 ������
	PUT_REASON_REALM_BREAK_FIGHT,					//!< 243 ����ϵͳս��������֮����
	PUT_REASON_CAPACITY_TARGET,						//!< 244 ϵͳ��ս
	PUT_REASON_HELP_OTHERS_2,						//!< 245 �嵨����2 (��԰һ����)
	PUT_REASON_ANECDOTE_BIG_EVENT,					//!< 246 ���Ŵ��¼�
	PUT_REASON_YUN_ZE_TERRITORY,					//!< 247 ���󻭾�
	PUT_REASON_RA_SIGN_HAPPY,						//!< 248 ��¼�ͺ���
	PUT_REASON_RA_WANLING_GONGYING,					//!< 249 ���鹲ӯ
	PUT_REASON_RA_HORCRUX_TIANCHENG,				//!< 250 �������
	PUT_REASON_RA_HORCRUX_TIANCHENG_DRAW,			//!< 251 �������-�齱
	PUT_REASON_RA_INTEGRAL_DRAW,					//!< 252 ����齱
	PUT_REASON_SHAN_HAI_QUESTION_REWARD,			//!< 253 ɽ���ʴ���
	PUT_REASON_SHAN_HAI_JU_BAO_DRAW,				//!< 254 ɽ���۱� �齱
	PUT_REASON_SHAN_HAI_JU_BAO_EXTRA_REWARD,		//!< 255 ɽ���۱� ���⽱��
	PUT_REASON_SHAN_HAI_MET_REWARD,					//!< 256 ɽ������ ���⽱��
	PUT_REASON_RA_SHAN_HAI_MEET,					//!< 257 ɽ�����
	PUT_REASON_RA_FATE_ENTANGLEMENT,				//!< 258 ɽ��Ե��
	PUT_REASON_ENDLESS_TOWER_REWARD,				//!< 259 �޾�֮��
	PUT_REASON_HUAN_JIE_TODAY_WIN_REWARD,			//!< 260 �ý�ս��ʤ�����佱��
	PUT_REASON_RA_ZHAOCAIJINBAO_DRAW,				//!< 261 �вƽ��� �齱
	PUT_REASON_RA_ZHAOCAIJINBAO,					//!< 262 �вƽ���
	PUT_REASON_RA_ZAI_ZHAN_YUN_ZE,					//!< 263 ��ս����
	PUT_REASON_YUN_ZE_SLOT_MACHINE,					//!< 264 ����ͬ����
	PUT_REASON_MO_ZU_ZAI_NA_LI,						//!< 265 ħ��������
	PUT_REASON_YUN_ZE_SLOT_MACHINE_BUY_KEY,			//!< 266 ����ͬ������Կ��
	PUT_REASON_CHAOZHI_FENXIANG,					//!< 267 ��ֵ����
	PUT_REASON_RA_COLORFUL_DOUBLE,					//!< 268 ����ӱ�
	PUT_REASON_SHAN_HAI_ZHENG_FENG_REWARD,			//!< 269 ɽ������
	PUT_REASON_SHAN_HAI_MI_BAO_REWARD,				//!< 270 ɽ���ٱ�
	PUT_REASON_SHAN_HAI_BAO_XIANG,					//!< 271 ɽ������
	PUT_REASON_SHAN_HAI_BAO_HE,						//!< 272 ɽ������
	PUT_REASON_SHAN_HAI_BAO_DAI,					//!< 273 ɽ������
	PUT_REASON_RA_YUN_ZE_ER_SAN_SHI,				//!< 274 ���������ת�̽����·�
	PUT_REASON_SHANHAI_HUADENG,						//!< 275 ɽ������
	PUT_REASON_WAN_LING_FB,							//!< 276 ���黰��
	PUT_REASON_YUNZE_BLOOMING_RECORD,				//!< 277 ���󷱻�¼
	PUT_REASON_RA_JIXING_GAOZHAO,					//!< 278 ���Ǹ���
	PUT_REASON_RA_JIXING_GAOZHAO_DRAW,				//!< 279 ���Ǹ���-�齱
	PUT_REASON_RA_YUNZE_ER_SAN_SHI_REWARD,			//!< 280 ���������
	PUT_REASON_BOUNTY_FINISH_REWARD,				//!< 281 �ͽ�������ɽ���
	PUT_REASON_DIAN_HUA_SHI,						//!< 282 ����ʯ��ȡ
	PUT_REASON_FISHING,								//!< 283 ����
	PUT_REASON_NEW_LIFE_MAKE,						//!< 284 ����ְҵ����
	PUT_REASON_MIRAGE_TERRITORY,					//!< 285 �����׾�
	PUT_REASON_RA_YUNZE_CATCH_ELVES,				//!< 286 �����ץ��
	PUT_REASON_RA_YUNZE_HAO_LI,						//!< 287 �������
	PUT_REASON_RA_YUNZE_HAO_LI_BUY_GOLD_HAMMER,		//!< 288 ����������
	PUT_REASON_RA_XIAN_SHI_LEI_CHONG,				//!< 289 ��ʱ�۳�
	PUT_REASON_RA_YUNZE_WISH,						//!< 290 ������Ը
	PUT_REASON_RA_WEEKEND_GIFT,						//!< 291 ��ĩ���
	PUT_REASON_RA_YUNZE_CUTE_PET,					//!< 292 �ȳ�ݵ�
	PUT_REASON_RA_CHAO_ZHI_GIFT,					//!< 293 ��ֵ����
	PUT_REASON_RA_FLY_UP_REWARD,					//!< 294 ����������
	PUT_REASON_TREASURE_TASK,						//!< 295 �����ر�
	PUT_REASON_RA_TIGER_AUSPICIOUS_SNOW,			//!< 296 ��ѩ����
	PUT_REASON_RA_TIGER_FISH_EVERY_YEAR,			//!< 297 ��������
	PUT_REASON_RA_YINHU_WENSHI,						//!< 298 ��������
	PUT_REASON_RA_TIGER_FIND_SPRING,				//!< 299 ����̽��
	PUT_REASON_RA_TIGER_TEASE_NEW_YEAR,				//!< 300 �������´�
	PUT_REASON_RA_CINIU_YINGXINHU,					//!< 301 ��ţӭ�»�
	PUT_REASON_RA_TIGER_LUCKY_MONEY,				//!< 302 ����ѹ��
	PUT_REASON_RA_HU_XING_DA_YUN,					//!< 303 ���д���
	PUT_REASON_RA_YIN_HU_BAI_SUI,					//!< 304 ��������
	PUT_REASON_RA_AUSPICIOUS_SNOW_PICK_UP,			//!< 305 ��ѩ����ѩ��ʰȡ
	PUT_REASON_RA_JINGJI_YUEKA_REWARD,				//!< 306 �����¿�����
	PUT_REASON_WEDDING,								//!< 307 ����
	PUT_REASON_RONG_LIAN_YOU_LI,					//!< 308 ��������
	PUT_REASON_FALLEN_GOD_REWARD,					//!< 309 ����֮�ؽ���
	PUT_REASON_PET_FEI_SHENG_REWARDS,				//!< 310 ��������¼
	PUT_REASON_RA_QINGYUAN_SHIZHUANG,				//!< 311 ��Եʱװ
	PUT_REASON_PET_BATCH_EXCHANGE,					//!< 312 ������Ƭ�����ϳɷֽ�õ���Ʒ
	PUT_REASON_MINI_GAME_STAGE_REWARD,				//!< 313 С��Ϸ�ؿ�����
	PUT_REASON_MINI_GAME_PHASE_REWARD,				//!< 314 С��Ϸ�׶ν���
	PUT_REASON_QING_YUAN_GIFT_DAY_REWARD,			//!< 315 ��Ե���� ��ȡÿ�ս���
	PUT_REASON_RA_QINGYUAN_LEICHONG,				//!< 316 ��Ե�۳�
	PUT_REASON_FALLEN_GOD_HELP_PASS_REWARD,			//!< 317 ����֮����ս����
	PUT_REASON_WORLD_ARENA_WIN_BOX,					//!< 318 �۽�����ʤ������
	PUT_REASON_GOD_PRINT_EXCHANGE_2,				//!< 319 ��ӡ�һ�2
	PUT_REASON_QING_YUAN_MARRAY_REWARD,				//!< 320 ��Ե��齱��
	PUT_REASON_WORLD_ARENA_SIGNUP_REWARD,			//!< 321 �۽���������
	PUT_REASON_WORLD_ARENA_DAILY_REWARD,			//!< 322 �۽�ÿ�ս���
	PUT_REASON_PET_BADGE_TAKE_OFF,					//!< 323 �����������
	PUT_REASON_PET_BADGE_COMPOSE,					//!< 324 ������ºϳ�
	PUT_REASON_PET_BADGE_DECOMPOSE,					//!< 325 ������·ֽ�
	PUT_REASON_WORLD_TEAM_ARENA,					//!< 326 Ӣ�ۻ���
	PUT_REASON_PEAK_TOKEN,							//!< 327 ����������
	PUT_REASON_PET_STRENGTHEN_REWARD,				//!< 328 ����ǿ������
	PUT_REASON_PARTNER_INTENSIFY,					//!< 329 ���ǿ��
	PUT_REASON_QING_YUAN_SHENG_HUI,					//!< 330 ��Եʢ��
	PUT_REASON_YAO_XING_SHI_LIAN,					//!< 331 ��������
	PUT_REASON_SHI_YU_MILE,							//!< 332 ʦͽ���
	PUT_REASON_SHI_YU_CHUAN_GONG,					//!< 333 ʦͽ����
	PUT_REASON_ACCESSORIES_BACK_STRENGTHEN,			//!< 334 ���ηֽ�ǿ�������˻�
	PUT_REASON_SAN_XIAN_GUI_DONG_REWARD,			//!< 335 ���ɹ鶴����
	PUT_REASON_LANTERN_RIDDIE,						//!< 336 ɽ���ƻ����ս���
	PUT_REASON_WHERE_IS_THE_FAIRY,					//!< 337 �������Ķ�
	PUT_REASON_TIAN_GONG_XUN_BAO,					//!< 338 �칬Ѱ��
	PUT_REASON_QINGYUAN_DUIDUIPENG,					//!< 339 ��Ե�Զ��� ��������
	PUT_REASON_LOVERS_MARKET,						//!< 340 ��ԵС��
	PUT_REASON_ANSWER_LANTERN_RIDDIE,				//!< 341 ɽ���ƻ���⽱��
	PUT_REASON_CONCENTRIC_KNOT,						//!< 342 Եǣͬ�Ľ�
	PUT_REASON_TAO_QUAN_NA_JI,						//!< 343 ��Ȧ�ɼ�
	PUT_REASON_TU_ZI_SAI_PAO,						//!< 344 ��������
	PUT_REASON_GUESS_BET_REWARD,					//!< 345 ���½���
	PUT_REASON_GUESS_BET_FAILD_RETURN,				//!< 346 ����ʧ�ܷ���
	PUT_REASON_UTA_WIN_BOX,							//!< 347 ���µ�һ ʤ������
	PUT_REASON_UTA_TODAY_JOIN_REWARD,				//!< 348 ���µ�һ ÿ�ղ��뽱��
	PUT_REASON_UTA_ACTIVE_REWARD,					//!< 349 ���µ�һ ��Ծֵ����
	PUT_REASON_ROLE_HUNDRED_GOD_BOSS,				//!< 350 ����֮ʦ
	PUT_REASON_XUN_BAO_GUIDE_PARTNER,				//!< 351 Ѱ�����ָ��
	PUT_REASON_XUN_BAO_GUIDE_PARTNER_2,				//!< 352 Ѱ�����ָ��2
	PUT_REASON_RA_QIYUAN_KONGMINGDENG,				//!< 353 ��Ը������
	PUT_REASON_RA_YUAN_QI_SPEED,					//!< 354 Ԫ������
	PUT_REASON_RA_YUAN_QI_HELP,						//!< 355 Ԫ������
	PUT_REASON_RA_XUNMENG_HUDIE,					//!< 356 Ѱ�κ���
	PUT_REASON_RA_GOD_BEAST_ADVENT,					//!< 357 ��������
	PUT_REASON_RA_BO_ZHONG_YOU_LI,					//!< 358 ��������
	PUT_REASON_RA_YUAN_QI_FAN_PAI,					//!< 359 Ԫ������
	PUT_REASON_RA_YUAN_QI_FAN_PAI_REWARD,			//!< 360 Ԫ�����Ʒ��ƽ���
	PUT_REASON_RA_WANLING_ZHULI,					//!< 361 ��������
	PUT_REASON_RA_BAO_ZI_LAI_LE,					//!< 362 ��������
	PUT_REASON_RA_JIN_YI_TIAN_XIANG,				//!< 363 ��������
	PUT_REASON_FETCH_PINJIA_REWARD,					//!< 364 ��ȡ����������
	PUT_REASON_RA_MIAO_BI_SHENG_HUA,				//!< 365 �������
	PUT_REASON_RA_YUANQI_JISHIBU,					//!< 366 Ԫ�����²�
	PUT_REASON_RA_YUAN_QI_YAN_HUO,					//!< 367 Ԫ���̻�
	PUT_REASON_RA_YUAN_QI_YAN_HUO_OPEN_ITEM,		//!< 368 Ԫ���̻����̻���Ʒ
	PUT_REASON_RA_YUAN_QI_FAN_PAI_DAY_FREE_REWARD,	//!< 369 Ԫ������ÿ�ս���
	PUT_REASON_RA_CHENG_FENG_PO_LANG,				//!< 370 �˷�����
	PUT_REASON_YUAN_HUN_ZHU_UP_LEVEL,				//!< 371 Ԫ��������
	PUT_REASON_YUAN_HUN_ZHU_PUT_ON,					//!< 372 Ԫ���鴩���滻����
	PUT_REASON_YUAN_HUN_ZHU_TAKE_OFF,				//!< 373 Ԫ��������
	PUT_REASON_ZAO_HUA_DISCOMPOSE,					//!< 374 �컯װ���ֽ�
	PUT_REASON_HONGMENG_TIANJIE,					//!< 375 ���������佱��
	PUT_REASON_HONGMENG_TIANJIE_BUY,				//!< 376 ������繺�����
	PUT_REASON_YUAN_HUN_ZHU_DECOMPOSE,				//!< 377 Ԫ����ֽ�
	PUT_REASON_YUAN_HUN_ZHU_COMPOSE,				//!< 378 Ԫ����ϳ�
	PUT_REASON_ZAO_HUA_COMPOSE,						//!< 379 �컯װ���ϳ�
	PUT_REASON_RA_ZONG_XIANG_REWARD,				//!< 380 ��������
	PUT_REASON_RA_SALTY_SWEET_BATTLE,				//!< 381 ����֮��
	PUT_REASON_RA_DUAN_YANG_XIANG_NANG,				//!< 382 ��������
	PUT_REASON_RA_DUAN_YANG_ZI_XUAN,				//!< 383 ������ѡ
	PUT_REASON_RA_CANG_LONG_CI_FU,					//!< 384 �����͸�
	PUT_REASON_RA_LUCKY_BLIND_BOX_REWARD,			//!< 385 ����ä��
	PUT_REASON_RA_LAN_TANG_CHU_MU,					//!< 386 ��������
	PUT_REASON_RA_SANBAI_CHOU,						//!< 387 ���ٳ�Ƥ��
	PUT_REASON_RA_FU_LI,							//!< 388 ��������
	PUT_REASON_RA_LUCKY_BLIND_EXTRA_REWARD,			//!< 389 ����ä�ж��⽱��
	PUT_REASON_RA_JIN_LONG_JU_BAO_DRAW,				//!< 390 �����۱��齱
	PUT_REASON_RA_JIN_LONG_JU_BAO_FETCH,			//!< 391 �����۱���ȡ
	PUT_REASON_RA_JIN_LONG_JU_BAO_BUY,				//!< 392 �����۱�����
	PUT_REASON_SPECIAL_ACTIVE_REWARD,				//!< 393 �����Ծ����
	PUT_REASON_RA_FORTUNE_MAZE,						//!< 394 �����Թ�	
	PUT_REASON_KE_JU_EXAM_ANSWER_REWARD,			//!< 395 �ƾٿ��Դ��⽱��
	PUT_REASON_KE_JU_EXAM_LAST_REWARD,				//!< 396 �ƾٿ������ս���
	PUT_REASON_GOTO_SCHOOL,							//!< 397 ��Ҫ��ѧ����
	PUT_REASON_ZHOU_QI_SHOP,						//!< 398 �����̵�
	PUT_REASON_CHONG_JI_FAN_LI,						//!< 399 �輼����
	PUT_REASON_RA_SHEN_SHOU_EN_ZE,					//!< 400 ���޶���
	PUT_REASON_RA_SHEN_SHOU_EN_ZE_FETCH,			//!< 401 ���޶�����ȡ��ѡ����
	PUT_REASON_RA_SHEN_SHOU_EN_ZE_BUY,				//!< 402 ���޶�����
	PUT_REASON_RA_XIN_FU_QIANG_HUA,					//!< 403 �·�ǿ��
	PUT_REASON_GOD_EQUIP_WORK_SHOP,					//!< 404 ������װ����
	PUT_REASON_RA_QI_XI_TE_HUI,						//!< 405 ��Ϧ�ػ�
	PUT_REASON_CHARISMAITC_TANABATA_REWARD,			//!< 406 ������Ϧ����
	PUT_REASON_XIAN_SHI_BAO_HE_3,					//!< 407 ��ʱ����3
	PUT_REASON_XIAN_SHI_BAO_HE_4,					//!< 408 ��ʱ����4
	PUT_REASON_RA_PET_EXCHANGE_BUY_ITEM,			//!< 409 ����һ�-�̵깺��
	PUT_REASON_HALLOWS_SKILL_CONVERT,				//!< 410 ʥ��������һ�
	PUT_REASON_SHENG_QI_TREASURE_MAP,				//!< 411 ʥ���ر�ͼ
	PUT_REASON_TAKE_OFF_FU_SHI,						//!< 412 ���·�ʯ
	PUT_REASON_RA_SHEN_CI_HUA_FU_DRAW,				//!< 413 ��ͻ���-�齱
	PUT_REASON_FU_YU_TAKE_OFF,						//!< 414 ����ж��
	PUT_REASON_FU_YU_UPGRADE_SUCC,					//!< 415 ����ǿ���ɹ�
	PUT_REASON_FU_YU_UPGRADE_FAIL,					//!< 416 ����ǿ��ʧ��
	PUT_REASON_FU_YU_USE_GIFT,						//!< 417 �������ʹ��
	PUT_REASON_ELEMENT_ELF_REWARD,					//!< 418 Ԫ�ؾ��齱�����ʵ����ｱ����
	PUT_REASON_COLOSSEUM_BATTLE_REWARD,				//!< 419 ������ս������
	PUT_REASON_XIN_FU_LING_CHONG,					//!< 420 �·����
	PUT_REASON_RA_HUN_QI_FAN_LI,					//!< 421 ��������
	PUT_REASON_RA_SHEN_CI_HUA_FU_BUY,				//!< 422 ��ͻ���-�̵깺��
	PUT_REASON_RA_TIAN_YI_YAO_SHI,					//!< 423 ����ҫ��
	PUT_REASON_KUA_FU_YING_XIONG_LU,				//!< 424 ���Ӣ��¼
	PUT_REASON_SMART_MOUNTS_CONVERT_ITEM,			//!< 425 ������Ʒת��
	PUT_REASON_ONE_CLICK_FINISH,					//!< 426 һ����ɻ�Ծ����
	PUT_REASON_SMART_MOUNTS_ACTIVE_CONVERT,			//!< 427 �����������ת��
	PUT_REASON_DAY_REWARD_ONLINE_GIFT,				//!< 428 ��������-�������
	PUT_REASON_STAR_CHART_RESET,					//!< 429 ��ͼ����
	PUT_REASON_NEW_COURAGE_CHALLENGE,				//!< 430 ߱��֮��
	PUT_REASON_FASHION_COLLECTION_DECOMPOSE,		//!< 431 �������ϵͳ���߷ֽ�
	PUT_REASON_SMART_MOUNTS_SKILL_COMPOSE,			//!< 432 �鼼�ϳ�
	PUT_REASON_STAR_CHART_LEVEL_UP,					//!< 433 ��ͼ����
	PUT_REASON_YUE_XI_LEI_CHONG,					//!< 434 ��Ϧ�۳�
	PUT_REASON_YUE_XI_DENG_MI,						//!< 435 ��Ϧ����
	PUT_REASON_RA_ZHUI_YUE_SHANG_DIAN_BUY,			//!< 436 ׷���̵깺��
	PUT_REASON_RA_ZHUI_YUE_SHANG_DIAN_DRAW,			//!< 437 ׷���̵�齱
	PUT_REASON_FU_YU_SUBLIME,						//!< 438 ��������
	PUT_REASON_FU_YU_SWITCH,						//!< 439 ����ת��
	PUT_REASON_SPECIAL_ANECDOTE,					//!< 440 ������������
	PUT_REASON_TEAM_CHALLENGE,						//!< 441 �����ս����
	PUT_REASON_HUAN_SHOU_ACTIVE_CONVERT,			//!< 442 ���޼���ת��
	PUT_REASON_HUAN_SHOU_LIAN_DAN,					//!< 443 ����ϵͳ����
	PUT_REASON_HUAN_SHOU_DECOMPOSE,					//!< 444 ����ϵͳ�ֽ�
	PUT_REASON_GUILD_TASK_GIVE,						//!< 445 �����������
	PUT_REASON_GUILD_TASK_FINISH,					//!< 446 �����������
	PUT_REASON_GUILD_TASK_FETCH,					//!< 447 ����������ȡ����
	PUT_REASON_PET_ABANDONMENT_GAI_MING,			//!< 448 ������ص��߷���
	PUT_REASON_GUILD_INBORN_RESET_RETURN,			//!< 449 �����������ü��ܷ���
	PUT_REASON_REAL_NAME_REWARD,					//!< 450 ʵ����֤����

	PUT_REASON_MAX,									// ���ԭ��
};

enum KNAPSACK_CONSUME_TYPE							// ������Ʒ������
{
	KNAPSACK_CONSUME_TYPE_CANNOT_BUY = 0,			// �������Զ�����
	KNAPSACK_CONSUME_TYPE_GOLD_BUY,					// ����ǰ�Ԫ���Զ�����

	KNAPSACK_CONSUME_TYPE_MAX
};

struct SortStruct
{
	void Set(long long _prior_id, bool _is_bind, int _src_index)
	{
		prior_id = _prior_id;
		is_bind = _is_bind;
		src_index = _src_index;
	}

	static long long GetSupplyPriorId(short _item_type, short _item_sub_type, short _item_color, ItemID _item_id)
	{
		// ����Ʒ ��item_id���Ⱥ�

		return _item_id;
	}

	static long long GetPriorId(short _item_level, short _item_type, short _item_sub_type, short _item_color, short _equip_type, ItemID _item_id, short _param);
	static long long GetPriorId_1(long long base_prior, short _item_level, short _item_type, short _item_sub_type, short _item_color, short _equip_type, ItemID _item_id, short _param);
	static long long GetPriorId_2(long long base_prior, short _item_level, short _item_type, short _item_sub_type, short _item_color, short _equip_type, ItemID _item_id, short _param);

	bool is_bind;
	int src_index;

	long long prior_id;
};
int SortCompare(const void *arg1, const void *arg2);

class Knapsack
{
public:
	enum ITEM_CHANGE_NOTICE_REASON
	{
		ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE = 0,	//!< 0 ���ݸı�
		ITEM_CHANGE_NOTICE_REASON_PUT,				//!< 1 ���ⲿ���
		ITEM_CHANGE_NOTICE_REASON_CONSUME,			//!< 2 ���ڲ�����
		ITEM_CHANGE_NOTICE_REASON_MOVE,				//!< 3 ���ڲ�ת��
	};

public:
	Knapsack();
	~Knapsack();

	void* operator new(size_t c);
	void  operator delete(void* m);

	void Init(RoleModuleManager* module_mgr, int param_num, const ItemListParam::ItemListData *param_list[], const KnapsackOtherParam& other_param);
	void GetOtherInitParam(KnapsackOtherParam* other_param);
	void Update(time_t now_second);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnLevelUp(int new_level);
	void OnLogin();

	void EquipGemMounting(int stone_type, int equip_grid, int stone_grid);	// ��ʯ��Ƕ
	void EquipGemDismount(int stone_type, int equip_grid, int stone_slot);	// ��ʯ��ж
	void EquipGemUpgrades(int stone_type, int equip_grid, int stone_slot);	// ��ʯ����
	void ConfirmGemUpgrade();												// ȷ��������ʯ

	//װ������
	void EquipUpStar(int seq, int equip_type, int equip_grid, int cost_grid, int cost_grid_2);
	void CalcGemOneKeyUpgradeNeed(std::map<ItemID, int> & owned_list);
	void SendGemOneKeyUpgradeNeed();

	//������ҫ����װ����
	void UpEquipGrade(int equip_type, int equip_grid, int role_type = ROLE_TYPE_BENZUN);
	void BreakThroughGarde(int equip_type, int equip_grid, int role_type = ROLE_TYPE_BENZUN);

	void DianHuaEquip(int equip_grid, int dianhuashi_coloumn, int dianhuashi_grid);
	void SaveDianHuaEquip(int equip_grid);
	void XiFuEquip(int equip_grid);
	void FuMoEquip(int equip_grid, int *new_spid = NULL, int *new_level = NULL);
	void ComposeDianHuaShi(int main_stone_idx, int cost_stone_1, int cost_stone_2);

	bool Use(short column, short index, short num, short param1, short param2, short param3, int item_param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL);
	bool Discard(short column, short index, short discard_num);
	bool InOrder(short column, short index_start, short index_end, bool ignore_time_interval = false);
	bool Deposit(short column, short index, short storage_page); // ������Ʒ���ֿ�
	bool Withdraw(short column, short index, ARG_OUT short* out_column, bool notice_on_error = false); // �Ӳֿ�(��ʱ����)ȡ����Ʒ
	bool WithdrawAll(short column);
	bool Extend(short column, short add_grid_num); // ����ָ��������
	bool DecomposeSurface(short column, short index, short discard_num);   //ʱװ�ֽ�
	bool SendMail(const ItemConfigData * item_list, int item_list_num, int type = SNED_MAIL_TYPE_DEFAULT, bool is_notice = false, int param1 = 0, const char * param2 = NULL, int param3 = 0);	//�����������ʼ��·�
	bool SendMail(const std::vector<ItemConfigData> & item_list, int type = SNED_MAIL_TYPE_DEFAULT, bool is_notice = false, int param1 = 0, const char * param2 = NULL, int param3 = 0);	//�����������ʼ��·�

	bool Put(const ItemConfigData &item_configdata, int reason_type, int* out_column = NULL, int *out_index = NULL, UInt16 drop_monster_id = 0, int gold_price = 0, bool is_notice = true);
	bool Put(const ItemDataWrapper &item_wrapper, int reason_type, int* out_column = NULL, int *out_index = NULL, UInt16 drop_monster_id = 0, bool is_notice = true);
	bool Put2(int item_id, int num, int reason_type, bool is_bind = true, int * out_column = NULL, int * out_index = NULL, UInt16 drop_monster_id = 0, int gold_price = 0, bool is_notice = true);
	bool PutList(short put_list_num, const ItemConfigData *item_put_list, int reason_type, bool is_noitce = true);		// �о�ֵ��Ʒ�����ô˽ӿ�
	bool PutList3(short put_list_num, const ItemConfigData * item_put_list, int reason_type, std::vector< std::pair<int, int> > * out_column_index_list = NULL, bool is_noitce = true);		// �о�ֵ��Ʒ�����ô˽ӿ�
	bool PutList2(short list_num, const ItemConfigData * item_configdata_list, int reason_type, int get_reson = GET_REASON_INVALID, int param1 = 0);		// �о�ֵ��Ʒ�����ô˽ӿ�	
	bool PutList(short list_num, const ItemDataWrapper *item_wrapper_list, int reason_type, int *out_column_list = NULL, int *out_index_list = NULL);
	bool PutVec(const std::vector<ItemConfigData> & item_configdata_list, int reason_type);
	bool PutVec(const std::vector<ItemDataWrapper>& item_wrapper_list, int reason_type);
	bool MergeBeforePutList(short list_num, const ItemConfigData *item_configdata_list, ARG_OUT short* out_list_num, ARG_OUT ItemConfigData* out_item_list, ARG_IN short out_item_max_num);
	void MergeItemList(std::vector<ItemConfigData> item_configdata_list, std::vector<ItemConfigData> & out_item_list);
	//bool PutOrMail(const ItemConfigData &item_configdata, int reason_type);
	//bool PutListOrMail(const ItemConfigData item_configdata_list[MAX_ATTACHMENT_ITEM_NUM], int reason_type);

	// PutListǰ��������	
	bool CheckForPutList(short list_num, const ItemConfigData *item_configdata_list, bool need_check_temperary = true);
	bool CheckForPutList2(const std::vector<ItemConfigData> & item_configdata_list, bool need_check_temperary = true);
	bool CheckForPutList(short list_num, const ItemDataWrapper *item_wrapper_list, bool need_check_temperary = true);
	bool CheckForPutListWithoutTemporary(short list_num, const ItemConfigData * item_configdata_list);
	bool CheckForPutListWithoutTemporaryVec(const std::vector<ItemConfigData> & item_configdata_list);
	bool CheckEmptyGridNoLessThan(int column, int check_num, int * out_need_num = NULL, bool is_notice_no_space = false);		// ��鵱ǰ������λ�Ƿ��������ֵ

	// Putǰ������������
	bool CheckForPut(ItemID item_id, int num, bool need_check_temporary = true);	// ���ָ����Ʒ�������ܷ�λ�÷Ž�ȥ���������ݶѵ�������ռ�ø�������
	bool CheckForPut2(const ItemConfigData & item, bool need_notice = true, bool need_check_temporary = true);	// ���ָ����Ʒ�������ܷ�λ�÷Ž�ȥ���������ݶѵ�������ռ�ø�������
	bool CheckForPutItemDataWrapper(const ItemDataWrapper & item, bool need_notice = true, bool need_check_temporary = true);	// ���ָ����Ʒ�������ܷ�λ�÷Ž�ȥ���������ݶѵ�������ռ�ø�������

	bool CheckEmptyGridNoLessThanInTemporary(int check_num);		// ��鵱ǰ��ʱ������λ�Ƿ��������ֵ
	bool HaveSpaceInTemporary();
	bool CheckEmptyGridNoLessThanForItem(ItemID item_id, int num);

	// ����ͬʱ��ִ��Put����������Check��PutҪ�ֿ��ĳ���
	//bool CheckNumAndPut(const ItemConfigData &item_configdata, int reason_type, bool check_pile_limit = true); // ��������Ʒ�������Գ������ѵ��������Կ���Put���Ż��汾

	int  Count(ItemID item_id, bool is_check_temporary = false, bool is_check_storage = false);
	bool HasItem(ItemID item_id, int num, bool is_check_temporary = true);
	bool HasItemByMap(const std::map<int, int> & check_item_map, bool is_check_temporary = true);

	bool ConsumeItemByMap(const std::map<int, int> & check_item_map, const char * reason);
	bool ConsumeItem(ItemID item_id, int num, const char * reason, int consume_type = KNAPSACK_CONSUME_TYPE_CANNOT_BUY, bool notify_lack = false);
	bool ConsumeItemByIndex(int column, int index, int num, const char * reason, bool is_real_consume = true, bool need_audit_log = true);
	bool ConsumeItemByIndexList(int list_num, const short* column_list, const short *knapsack_index_list, const short *consume_num_list, const char *reason, bool is_real_consume = true, bool need_audit_log = true);		// ����index�ظ�
																																																							//���ĵ����������ڵı�����+�ֿ�+��ʱ����
	bool ConsumeKnapsackItem(ItemID item_id, int consume_num, const char *reason, int consume_type = KNAPSACK_CONSUME_TYPE_CANNOT_BUY, bool notify_lack = false);

	void SendKnapsackInfoToRole(short reason = 0);

	void OnCheckNorexConvertItem(ItemConfigData & item_data);		//�������ĵ���ת��
	bool OnRoleAttrPelletConvert(ItemConfigData item_data, std::vector<ItemConfigData> & convert_item_list);	//���Ե�ת��

	bool GetItemGridData(int column, int index, ItemGridData *item_data);
	ItemGridData * GetItemGridData2(int column, int index);
	const ItemGridData* GetItemGridData(int column, int index) const;
	bool GetItemGridWrapData(int column, int index, ItemDataWrapper* out_wrapper);
	void SetItemDirty(int column, int index, bool to_notice_client, short reason_type, short put_reason = 0);
	void RefreshValidGridIndexEnd(int column, int change_index);											// �����ǿո����������

	void GridSetBind(int column, int index, bool need_set_dirty);
	void GridClearInvalidTime(int column, int index, bool need_set_dirty);

	void GetItemList(int *count, ItemListParam::ItemListData *save_list);
	void GetChangeItemList(int *count, ItemListParam::ItemListData *save_list);
	void ClearDirtyMark();

	bool GetListOfItem(ItemID item_id, short max_num,
		short *bind_column_list, short *bind_index_list, short *bind_num_list, short *bind_count,
		short *nobind_column_list, short *nobind_index_list, short *nobind_num_list, short *nobind_count);

	int GetGridIndex(ItemID item_id);
	bool GetGridColumnIndex(ItemID item_id, int & out_column, int & out_index);

	void NotifyGetItemList(short list_num, const ItemConfigData *item_configdata_list, int get_reson = 0, int param1 = 0, int param2 = 0);
	void NotifyGetItemListVec(const std::vector< ItemConfigData> & list, int get_reson = 0, int param1 = 0, int param2 = 0);
	void NotifyGetItemList3(int add_gold, int add_coin_bind, int add_exp, short list_num, const ItemConfigData *item_configdata_list, int get_reson = 0, int param1 = 0);

	static bool GetRealIndexColumn(int index, int *out_real_column, int *out_real_index);
	static int GetIndexByRealIndexColumn(int real_column, int real_index);
	void ClearGrid(int column, int index);
	void ClearAll();																			// ��ձ���������GM����ʹ��

																								//��������б�
	void ClearItemList(int column, std::vector<ItemID>* item_list, const char *reason);//���ĳ�౳���������Ʒ�б�

	bool UseExpCrystal();

	void OnChangeProf();

	void OnRoleLogout();
	void SendExpCrystalInfo();
	void OnRoleModifyExpCrystal();
	void RetrieveItem(ItemID item_id, short num, int reason);
	void SendGetItemNotice(int notice_type, ItemID item_id, short num, int param1 = 0);

	int GetKnapsackVaildNum(int column);

	//�������
	void SynItemData(int column, ItemGridData * item_data);			//ͬ������������� ����->����
	void SynHuaShenItemData(std::set<long long> * item_set = NULL, bool is_element_crystal = false);	//������ʼ,ͬ�����𱳰�������Ϣ  ����->����
	void SynItemBindInfo(int column, long long item_unique_id, bool is_bind);	//ͬ��������߰����  ����->����

	int ReclaimItem(ItemID item_id);			//������Ʒ
	bool ReturnGodEquip();//��װͻ���޸�
	bool ReturnGodEquip2();//��װͻ���޸�

	void CheckItemReturn();
	void CheckGodEquipErrUplevel();
	void CheckGodEquipChangeLevel();
	void FixGodEquipBaseVoc();

	void SendErrorNotice();
	void GodEquipReBack();
	void GmCheckItemInWrongColumn();

	//�������
	//�������ֿ���ղ��Ƿ񼤻�
	void CheckPetSoulCollection(bool need_sync = false);

	void ChangeNewSuit();
	void ChangeNewVocEatra();

public:
	// �ѵ����߹��ߺ���
	static void AddRewards(std::vector<ItemConfigData>& rewards, ItemConfigData item);
	// ת��Ϊ��־�ַ���
	static std::string RewardsLog(const std::vector<ItemConfigData>& rewards);

private:
	bool CheckZhouQiItemExpired(ItemGridData * item_grid,const ItemBase * item_base);

	enum ITEM_CHANGE_TYPE
	{
		ITEM_CHANGE_TYPE_SNEAKY_IN = -4,	// ͵͵�ķ��� ����Ҫ֪ͨ��һ�� ������װ���ͱ�ʯ��Ƕ�ͷ�ʱʹ�����
		ITEM_CHANGE_TYPE_CHANGE = -3,		// �����ı�
		ITEM_CHANGE_TYPE_OUT = -2,			// �ӱ��������ⲿ
		ITEM_CHANGE_TYPE_IN = -1,			// ���ⲿ���뱳��
											// 0���ϱ�ʾ�Ǵӱ���/�ֿ�������������ƶ�����/ȥ ֵ��ʾԭ�����±�
	};

	void SendknapsackGridInfoToRole(int column, int index, short reason_type, short put_reason = 0);

	void CertainPut(int column, const ItemDataWrapper & item_info, int reason_type, int * out_column, int * out_index, UInt16 drop_monster_id = 0, bool is_notice = true);
	void CertainPut(const ItemDataWrapper &item_info, int reason_type, int* out_column, int *out_index, UInt16 drop_monster_id = 0);
	bool HaveSpace(int column, bool notice_on_error = false) { return CheckEmptyGridNoLessThan(column, 1, NULL, notice_on_error); }

	bool PutHelper(const ItemDataWrapper &item_wrapper, int reason_type, int* out_column = NULL, int *out_index = NULL, UInt16 drop_monster_id = 0, bool is_notice = true);
	bool CheckVirtualBagHaveGrids(int bags_type, int grid_num);
	bool PutItemIntoCorrespondBag(const ItemDataWrapper &item_wrapper, int reason_type);

	bool CertainMoveHelper(int from_column, int from_index, int to_column, int offset_index = 0, int range = 0); // �ú��������to_column��λ�Ŀռ䣬���������������ж�
	void SendNoSpaceNotice(int column);
	void AddNoSpaceErrorNotice(int column);
	void AddErrorNotice(int error_num);
	void ClearErrorNotice();
	void CheckItemInWrongColumn();
	void CheckColumnInitialGridNum();
	void CheckEquipZhuLing();
	bool PutExpenseItemAutoUse(const ItemBase* itembase, int num, bool is_bind, unsigned int invalid_time, bool is_notice);

	RoleModuleManager* m_module_mgr;
	bool need_confirm;
	GemUpgradeInfo m_gem_upgrade_param;
	bool m_old_state[ItemNamespace::ITEM_COLUMN_TYPE_MAX][ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX];
	bool m_dirty_mark[ItemNamespace::ITEM_COLUMN_TYPE_MAX][ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX];
	bool m_knapsack_is_change;

	short m_valid_grid_index_end_list[ItemNamespace::ITEM_COLUMN_TYPE_MAX];	// ��ǰ�����зǿո��ӵ�����һ���������
	short m_curr_valid_grid_num_list[ItemNamespace::ITEM_COLUMN_TYPE_MAX];	// ��ǰ��������������Ч����
	short m_extend_column_times_list[ItemNamespace::ITEM_COLUMN_TYPE_MAX];	// ��չ�������Ĵ���

	ItemGridData* m_grid_list[ItemNamespace::ITEM_COLUMN_TYPE_MAX][ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX]; // �����ֿ�����

	UInt32 m_knapsack_last_inorder_time;									// �ϴα�������ʱ��
	UInt32 m_storage_last_inorder_time;										// �ϴβֿ�����ʱ��
	UInt32 m_last_inorder_merge_time;										// �ϴ������ϲ�ʱ��

	char m_init_flag;
	char m_novice_gift_flag;

	unsigned int m_clear_temporary_column_time;

	std::vector<int> m_error_notice;
};

#endif