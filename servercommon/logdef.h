#ifndef __LOGDEF_H__
#define __LOGDEF_H__

#include "utility/logagent.h"
#include "ilogmodule.h"

enum LOG_TYPE
{
	LOG_TYPE_INVALID = 0,
	LOG_TYPE_GOLD_ADD = 1,						// ����Ԫ��
	LOG_TYPE_GOLD_USE = 2,						// ʹ��Ԫ��
	LOG_TYPE_IMMORTAL_COIN_ADD = 3,				// �����ɱ�
	LOG_TYPE_IMMORTAL_COIN_USE = 4,				// ʹ���ɱ�
	LOG_TYPE_COIN_BIND_ADD = 5,					// ���Ӱ�ͭǮ
	LOG_TYPE_COIN_BIND_USE = 6,					// ʹ�ð�ͭǮ

	LOG_TYPE_ITEM_PUT = 7,						// �����Ʒ
	LOG_TYPE_ITEM_USE = 8,						// ʹ����Ʒ
	LOG_TYPE_ITEM_DISCARD = 9,					// ������Ʒ
	LOG_TYPE_ITEM_CONSUME = 10,					// ������Ʒ

	LOG_TYPE_TRADE_MARTKET_ADD = 11,			// �ϼ���Ʒ
	LOG_TYPE_TRADE_MARTKET_DEL = 12,			// �¼���Ʒ
	LOG_TYPE_TRADE_MARTKET_BUY = 13,			// ���׳ɹ�
	LOG_TYPE_TRADE_MARTKET_EXPIRED = 14,		// ��Ʒ����

	LOG_TYPE_MAIL_ITEM_IN = 15,					// �ʼ������Ʒ
	LOG_TYPE_MAIL_ITEM_OUT = 16,				// �ʼ�������Ʒ

	LOG_TYPE_FORBID_TALK = 17,					// ����
	LOG_TYPE_FORBID_ROLE = 18,					// ��ֹ�û���¼
	
	LOG_TYPE_ROLE_PRESTIGE = 20,				// ��ɫ����

	LOG_TYPE_ONLINE_ROLE_NUM = 49,				// ��������
	LOG_TYPE_XIANHUN = 50,						// �ɻ�
	LOG_TYPE_YUANLI = 51,						// Ԫ��
	LOG_TYPE_HONOUR = 52,						// ����
	LOG_TYPE_REGISTER = 53,						// ע��
	LOG_TYPE_LOGIN = 54,						// ��¼
	LOG_TYPE_LOGOUT = 55,						// �ǳ�
	LOG_TYPE_UPGRADE_LEVEL = 56,				// ����
	LOG_TYPE_CHANGE_AUTHORITY_TYPE = 57,		// ����Ȩ��

	LOG_TYPE_ACCEPT_TASK = 58,					// ��������
	LOG_TYPE_COMMIT_TASK = 59,					// �ύ����
	LOG_TYPE_GIVEUP_TASK = 60,					// ��������
	LOG_TYPE_COMPLETE_TASK = 61,				// �������

	LOG_TYPE_PAY = 64,							// ��ֵ
	LOG_TYPE_PLAT_ONLINE = 65,					// ƽ̨����(����ɫƽ̨��ǰ׺����ͳ��)

	LOG_TYPE_MAIL_SEND_GOLD = 70,				// �ʼ���Ԫ�� ��g6������
	LOG_TYPE_MAIL_RECEIVE_GOLD = 71,			// �ʼ����Ԫ��

	LOG_TYPE_RAND_ACTIVITY_CHONGZHI_RANK = 79,	// ��ֵ����
	LOG_TYPE_RAND_ACTIVITY_CONSUME_GOLD_RANK = 80, // ��������

	LOG_TYPE_TRADE_MARKET_DEAL = 90,			// �����гɽ���¼

	
	LOG_TYPE_RETRIEVE_GOLD_UNBIND = 111,		// ����Ԫ��
	LOG_TYPE_RETRIEVE_GOLD_BIND = 112,			// ���հ�Ԫ
	LOG_TYPE_RETRIEVE_COIN_BIND = 113,			// ���հ�ͭ
	LOG_TYPE_RETRIEVE_VIP_EXP = 114,			// ����vip����
	LOG_TYPE_RETRIEVE_ITEM = 115,				// ������Ʒ
	LOG_TYPE_CAPABILITY_CHANGE = 116,			// ս���仯
	LOG_TYPE_RETRIEVE_OTHER_CURRENCY = 117,		// �����������ͻ���
	LOG_TYPE_OTHER_MONEY_USE = 118,				// ʹ����������

	LOG_TYPE_CAPABILITY_CHANGE_LOGIN = 124,		// ��¼ʱ������ʱ��ս�������仯

	LOG_TYPE_ROLE_LV1_MINE_MONSTER = 150,		// �������1����
	LOG_TYPE_SYSTEM_LV1_MONSTER = 151,			// ϵͳͳ��1����
	LOG_TYPE_ROLE_EXP = 152,					// ��ɫ����
	LOG_TYPE_ACTIVE_POINT = 153,				// ��Ծ��
	LOG_TYPE_WORK_SHOP_EQUIPMENT = 154,			// ���� - װ��
	LOG_TYPE_WORK_SHOP_JEWELRY = 155,			// ���� - ����
	LOG_TYPE_PARTNER_CHANGE_ADD = 156,			// �������
	LOG_TYPE_PARTNER_CHANGE_ATTR = 157,			// ������
	LOG_TYPE_COLLECTION = 158,					// ͼ��
	LOG_TYPE_FABAO_ACTIVE = 159,				// �����
	LOG_TYPE_FABAO_UP = 160,					// ��������
	LOG_TYPE_MIYI = 161,						// ����֮��
	LOG_TYPE_COURAGECHALLENGE = 162,			// ��ħ֮��
	LOG_TYPE_PET_ADD = 163,						// ��������
	LOG_TYPE_PET_ABANDONMENT = 164,				// �������
	LOG_TYPE_PET_BREAKTHROUGHS = 165,			// ����ǿ��
	LOG_TYPE_OTHER_MONEY = 166,					// �������
	LOG_TYPE_ADD_TITLE = 167,					// ��óƺ�
	LOG_TYPE_ACTIVITY_QUICK = 168,				// �ܳ������(��̨ͳ��������)
	LOG_TYPE_PET_REMOVE = 169,					// ����ɾ��
	LOG_TYPE_ROLE_FIRST_KILL_MONSTER_GROUP = 170,//��ͨ������ 
	LOG_TYPE_FUNCTION_STATUS = 171,				// ���ܲ���
	LOG_TYPE_HUANHUA_ACTIVE = 172,				// ���ǻû�����
	LOG_TYPE_PET_SKIN_ACTIVE = 173,				// ����Ƥ������
	LOG_TYPE_PARTNER_SKIN_ACTIVE = 174,			// ���Ƥ������
	LOG_TYPE_FACESCORE_ACTIVE = 175,			// ��ֵϵͳ����
	LOG_TYPE_RAND_ACTIVITY = 176,				// ����
	LOG_TYPE_PARTNER_PACT = 177,				// ���Ԫ��
	LOG_TYPE_PET_GOD_PRINT = 178,				// ������ӡ
	LOG_TYPE_EQUIP_ZHULING = 179,				// װ������
	LOG_TYPE_GEM_INLAY = 180,					// ��ʯ��Ƕ
	LOG_TYPE_HUAN_JIE_ROLE = 181,				// �ý��ɫ
	LOG_TYPE_HUAN_JIE_PET = 182,				// �ý����
	LOG_TYPE_MARKET = 183,						// �г�
	LOG_TYPE_ACTIVE_STAT = 184,					// ���ܻ�Ծͳ��
	LOG_TYPE_RAND_ACTIVITY_PARTICIPATE = 185,	// ������Ҳ���
	LOG_TYPE_RESET_NAME = 186,					// ��Ҹ���
	LOG_TYPE_PARTNER_AWAKENING = 187,			// ������
	LOG_TYPE_UNIVERSAL_ACTIVE_CARD = 188,		// ���ܼ��ʹ��
	LOG_TYPE_CLIENT_BEHAVIOR = 189,				// �ͻ�����Ϊ����¼
	LOG_TYPE_ENTER_UTA_BATTLE = 190,			// �������µ�һս��
	LOG_TYPE_GOD_PRINT_DISCOMPOSE = 191,		// ��ӡ�ֽ�
	LOG_TYPE_TOP_LEVEL = 192,					// �۷�ȼ�
	LOG_TYPE_UTA_QUALIFICATION = 193,			// ���µ�һ�ʸ���
	LOG_TYPE_UTA_FIGHT_RESULT = 194,			// ���µ�һ�������
	LOG_TYPE_UTA_USER_FIGHT_RESULT = 195,		// ���µ�һ���ս�����
	LOG_TYPE_UTA_PET_FIGHT_RESULT = 196,		// ���µ�һ����ս�����
	LOG_TYPE_UTA_TEAM_MEMBER = 197,				// ���µ�һ�����Ա��Ϣ
	LOG_TYPE_ZAO_HUA_ADD_ITEM = 198,			// �컯װ�����
	LOG_TYPE_YUAN_HUN_ZHU = 199,				// Ԫ����
	LOG_TYPE_HOLY_EQUIP = 200,					// ʥ�����
	LOG_TYPE_FU_SHI = 201,						// ��ʯ���
	LOG_TYPE_SHENG_QI_LEVEL_UP = 202,			// ʥ������
	LOG_TYPE_SHENG_QI_STATE_CHANGE = 203,		// ʥ��״̬�ı�
	LOG_TYPE_FU_YU = 204,						// ����
	LOG_TYPE_GUILD_HONOR_RESULT = 205,			// ��������ս���

	LOG_TYPE_MAX,
};


// ��־ID=184 ���ܻͳ�� ö�ٶ���
enum LOG_ACTIVE_TYPE
{
	LOG_ACTIVE_TYPE_INVALID = 0,
	LOG_ACTIVE_TYPE_RESEARCH_TASK = 1,			//!< 1 �����ħ
	LOG_ACTIVE_TYPE_CLOUD_ARENA = 2,			//!< 2 񷶥֮��
	LOG_ACTIVE_TYPE_CHALLENGE_FB = 3,			//!< 3 ��ս����
	LOG_ACTIVE_TYPE_CHALLENGE_TASK = 4,			//!< 4 ��ս����
	LOG_ACTIVE_TYPE_ESCORT = 5,					//!< 5 ����
	LOG_ACTIVE_TYPE_PLATFORM_BATTLE = 6,		//!< 6 ��̨
	LOG_ACTIVE_TYPE_MAZE_RACE = 7,				//!< 7 �Թ�����
	LOG_ACTIVE_TYPE_MIYI = 8,					//!< 8 Զ��ħڣ
	LOG_ACTIVE_TYPE_TASK_CHAIN = 9,				//!< 9 ���˵ķ���
	LOG_ACTIVE_TYPE_TEAM_FIGHT = 10,			//!< 10 �����Կ���
	LOG_ACTIVE_TYPE_WORLD_BOSS = 11,			//!< 11 ħ������
	LOG_ACTIVE_TYPE_GUILD_FIGHT = 12,			//!< 12 ������Ҷ�
	LOG_ACTIVE_TYPE_BRAVE_GROUND = 13,			//!< 13 �´��ع�
	LOG_ACTIVE_TYPE_CHIEF_ELECT = 14,			//!< 14 ��ϯ��ѡ
	LOG_ACTIVE_TYPE_BOUNTY_TASK = 15,			//!< 15 ��������
	LOG_ACTIVE_TYPE_COURAGE_CHALLENGE = 16,		//!< 16 ��ħ֮��
	LOG_ACTIVE_TYPE_WORLD_BOSS_2 = 17,			//!< 17 �ַ�����
	LOG_ACTIVE_TYPE_ONLY_FIGHT = 18,			//!< 18 ��һ�а�
	LOG_ACTIVE_TYPE_GUILD_QUESTION = 19,		//!< 19 �������
	LOG_ACTIVE_TYPE_FIELD_BOSS = 20,			//!< 20 Ұ��BOSS
	LOG_ACTIVE_TYPE_GUILD_HONOR = 21,			//!< 21 ��������ս
	LOG_ACTIVE_TYPE_HUANJIE_BATTLE = 22,		//!< 22 �ý�ս��
	LOG_ACTIVE_TYPE_DREAM_NOTE = 23,			//!< 23 ��Ԩ��¼
	LOG_ACTIVE_TYPE_SCHOOL_EXERCISE = 24,		//!< 24 У������
	LOG_ACTIVE_TYPE_LUN_HUI_WANG_CHUAN = 25,	//!< 25 �ֻ�����
	LOG_ACTIVE_TYPE_SHAN_HAI_BOSS = 26,			//!< 26 ɽ������
	LOG_ACTIVE_TYPE_PET_MONOPLY = 27,			//!< 27 ���޵�
	LOG_ACTIVE_TYPE_MATERIAL_FB = 28,			//!< 28 ���龳
	LOG_ACTIVE_TYPE_CRYSTAL_FB_COMMON = 29,		//!< 29 ������ˮ��������
	LOG_ACTIVE_TYPE_CRYSTAL_FB_RARE = 30,		//!< 30 �þ�����-ϡ�и�����ˮ������ϡ�У�
	LOG_ACTIVE_TYPE_WORLD_PURGATORY = 31,		//!< 31 ��ڤ����
	LOG_ACTIVE_TYPE_ENDLESS_TOWER = 32,			//!< 32 �޾�֮��
	LOG_ACTIVE_TYPE_BIG_DIPPER = 33,			//!< 33 ��������
	LOG_ACTIVE_TYPE_TIAN_DI_DAO_HEN = 34,		//!< 34 ��ص���
	LOG_ACTIVE_TYPE_JIEYI = 35,					//!< 35 ����
	LOG_ACTIVE_TYPE_LUNCH_DINNER = 36,			//!< 36 ������
	LOG_ACTIVE_TYPE_CRYSTAL_FB_ELITE = 37,		//!< 37 �Ϲ��ż���ˮ��������Ӣ��
	LOG_ACTIVE_TYPE_HUARONGDAO = 38,			//!< 38 ����-���ݵ�
	LOG_ACTIVE_TYPE_TREASURE_BOX_UNLOCK = 39,	//!< 39 ����-�������
	LOG_ACTIVE_TYPE_FIND_FOOD = 40,				//!< 40 ����-Ѱζ��ʳ
	LOG_ACTIVE_TYPE_POEMS_AND_SONGS = 41,		//!< 41 ����-ʫ�ʸ踳
	LOG_ACTIVE_TYPE_REAL_FAKE_PAINTING = 42,	//!< 42 ����-��ٻ���
	LOG_ACTIVE_TYPE_PET_WISH_GOD_PRINT = 43,	//!< 43 ��������
	LOG_ACTIVE_TYPE_WORLD_BOSS_3 = 44,			//!< 44 ����BOSS(������)

	LOG_ACTIVE_TYPE_MAX
};

enum LOG_GET_ITEM_WAY
{
	LOG_GET_ITEM_WAY_XUNBAO = 0,
	LOG_GET_ITEM_WAY_SHOP,
	LOG_GET_ITEM_WAY_COMPOSE,
	LOG_GET_ITEM_WAY_DUIHUAN,

	LOG_GET_ITEM_WAY_MAX,
};

enum LOG_MONEY_TYPE
{
	LOG_MONEY_TYPE_GOLD = 0,
	LOG_MONEY_TYPE_GOLD_BIND,
	LOG_MONEY_TYPE_COIN,
	LOG_MONEY_TYPE_HONOR,
	LOG_MONEY_TYPE_SHENGWANG,
	LOG_MONEY_TYPE_CROSS_HONOR,

	LOG_MONEY_TYPE_MAX,
};

enum LOG_TYPE_FUNCTION_STATUS_TYPE
{
	LOG_TYPE_FUNCTION_STATUS_TYPE_QIFU_COIN = 0,
	LOG_TYPE_FUNCTION_STATUS_TYPE_QIFU_YUANLI,
	LOG_TYPE_FUNCTION_STATUS_TYPE_QIFU_XIANHUN,
	LOG_TYPE_FUNCTION_STATUS_TYPE_ACCEPT_HUSONG,
	LOG_TYPE_FUNCTION_STATUS_TYPE_ACCEPT_DAILY,
	LOG_TYPE_FUNCTION_STATUS_TYPE_ACCEPT_GUILDTASK,
	LOG_TYPE_FUNCTION_STATUS_TYPE_XUNBAO,
	LOG_TYPE_FUNCTION_STATUS_TYPE_ACCEPT_LOOPTASK,

	LOG_TYPE_FUNCTION_STATUS_TYPE_ACTIVITY = 10000,

	LOG_TYPE_FUNCTION_STATUS_TYPE_FB = 20000,
};

inline void LOG_QUICK8(LogAgent &log_agent, long long n1, long long n2, const char *z1, const char *z2, long long n3, long long n4, const char *z3, const char *z4)
{
	log_agent.printf(LL_INFO, "%lld\t%lld\t%s\t%s\t%lld\t%lld\t%s\t%s", 
		(long long)n1, 
		(long long)n2,
		(NULL == z1 ? "-" : z1),
		(NULL == z2 ? "-" : z2),
		(long long)n3,
		(long long)n4,
		(NULL == z3 ? "-" : z3),
		(NULL == z4 ? "-" : z4)
		);
}

inline void LOG_QUICK12(LogAgent &log_agent, long long n1, long long n2, const char *z1, const char *z2, long long n3, long long n4, const char *z3, const char *z4, 
	long long n5, long long n6, long long n7, long long n8, long long n9)
{
	log_agent.printf(LL_INFO, "%lld\t%lld\t%s\t%s\t%lld\t%lld\t%s\t%s\t%lld\t%lld\t%lld\t%lld\t%lld", 
		(long long)n1, 
		(long long)n2,
		(NULL == z1 ? "-" : z1),
		(NULL == z2 ? "-" : z2),
		(long long)n3,
		(long long)n4,
		(NULL == z3 ? "-" : z3),
		(NULL == z4 ? "-" : z4),
		(long long)n5,
		(long long)n6,
		(long long)n7,
		(long long)n8,
		(long long)n9
		);
}

#endif
