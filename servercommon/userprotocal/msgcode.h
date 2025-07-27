
#ifndef __PROTOCAL_H__
#define __PROTOCAL_H__

#include "servercommon/servercommon.h"

namespace Protocol
{
	enum _MsgType
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		MT_ENTER_GS_ACK_SC = 1000,						// ������볡���������ظ�
		MT_CHANGE_GS_SC = 1001,
		MT_SERVER_BUSY_SC = 1002,						// ������̫æ�������û����ﵽ�������ޣ�
		MT_HEARTBEAT_SC = 1003,							// ����������
		MT_RESET_NAME_SC = 1004,						// �������ֻذ�
		MT_NEW_MESSAGE_FROM_CUSTOM_SERVICE = 1005,		// �ͷ���֪ͨ
		MT_CMD_ROUTE_SC = 1006,		// ������̨->�����->�ͻ���͸��ָ�� ת��

		MT_ENTER_GS_REQ_CS = 1050,						// ������볡��������
		MT_LOGOUT_CS = 1051,
		MT_TRANSPORT_REQ = 1052,
		MT_HEARTBEAT_CS = 1053,							// ����������
		MT_RESET_NAME_CS = 1054,						// ��������
		MT_SPECIAL_TRANSPORT_REQ_CS = 1055,				// �����ر��߼�������
		MT_TREASURE_CHEST_REQ_CS = 1056,				// �������
		MT_ROLE_SUBSCRIBE_REQ_CS = 1057,				// ����/ȡ������ϵͳ��Ϣ
	
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		MT_ENTER_SCENE_SC = 1100,
		MT_LEAVE_SCENE_SC = 1101,
		MT_VISIBLE_ENTER_ROLE_SC = 1102,				// Role��������������
		MT_VISIBLE_LEAVE_SC = 1103,						// �����˳���������
		MT_OBJ_MOVE_SC = 1104,							// ����λ���ƶ�ͬ��
		MT_RESET_POST_SC = 1105,						// ǿ������λ��
		MT_CHANGE_SCENE_NOTICE_SC = 1106,				// �ı䳡��֪ͨ
		MT_ENTER_FB_CS = 1107,							// ������븱��
		MT_LEAVE_FB_CS = 1108,							// �����뿪����
		MT_VISIBLE_ENTER_NPC_MONSTER_SC = 1109,			// NPC���������Ұ֪ͨ
		MT_VISIBLE_ENTER_NPC_GATHERS_SC = 1110,			// NPC�ɼ��������Ұ֪ͨ
		MT_STOP_MOVE_NOTICE_SC = 1111,					// ֪ͨ�ͻ���ֹͣ�ƶ�
		MT_APPEARACNE_CHANGE_SC = 1112,					// ����ı�
		MT_VISIBLE_ENTER_SPECIAL_TRANSPORT_SC = 1113,	// �ر��߼������Ž�����Ұ
		MT_VISIBLE_ENTER_TREASURE_CHEST_SC = 1114,		// ���������Ұ
		MT_VISIBLE_ENTER_NPC_FUNCNPC_SC = 1115,			// ������NPC������Ұ֪ͨ
		MT_SCENE_OBJ_LIST_SC = 1116,					// ������objλ���б�
		MT_UPDATE_FOLLOW_LIST_SC = 1117,				// ���¸����б�
		MT_ADD_FOLLOW_SC = 1118,						// ��������
		MT_REMOVE_FOLLOW_SC = 1119,						// ɾ������
		MT_JOIN_TEAM_NOTIFY_AREA_SC = 1120,				// �������֪ͨ��������
		MT_LEAVE_TEAM_NOTIFY_AREA_SC = 1121,			// �뿪����֪ͨ��������
		MT_ROLE_BUFF_EFFECT_NOTIFY_AREA_SC = 1122,		// ���buff��Ч֪ͨ��������
		MT_SPECIAL_APPEARANCE_CHANGE_NOTICE_SC = 1123,	// ��������ı�֪ͨ  
		MT_ROLE_TALK_TO_NPC_CS = 1124,					// ��NPC�Ի�
		MT_GET_CUR_POS_SC = 1125,						// �����˿ͻ��˲�ѯ�Լ���ǰλ�÷���
		MT_WORLD_LEVEL_INFO_SC = 1126,					// ����ȼ���Ϣ�·�
		MT_DAY_REWARD_DAY_FOOD_INFO_SC = 1127,			// ��������Ϣ�·�
		MT_VISIBLE_ENTER_XUN_YOU_CAR_SC = 1128,			// Ѳ�λ���������Ұ
		MT_VISIBLE_ENTER_PEAK_TOKEN_OBJ_SC = 1129,		// ���������� ���������Ұ
		MT_PET_STRENGTHEN_REWARD_INFO_SC = 1130,		// ����ǿ��������Ϣ�·�
		MT_PET_STRENGTHEN_REWARD_REQ_CS = 1131,			// ����ǿ����������
		MT_CLIENT_BEHAVIOR_REQ_CS = 1132,				// �ͻ�����Ϊ�������

		MT_YAO_XING_SHI_LIAN_INFO_SC = 1133,			// ���������·�
		MT_YAO_XING_SHI_LIAN_REQ_CS = 1134,				// ������������

		MT_SAN_XIAN_GUI_DONG_REQ_CS = 1135,				// ���ɹ鶴����
		MT_SAN_XIAN_GUI_DONG_INFO_SC = 1136,			// ���ɹ鶴�·�
		MT_SAN_XIAN_GUI_DONG_PLAY_INFO_SC = 1137,		// ���ɹ鶴��Ϸ�·�
		MT_VISIBLE_ENTER_TU_ZI_KUAI_PAO_TU_ZI_OBJ_SC = 1138,		// ���ӿ��ܻ���ӽ�����Ұ

		MT_TAO_QUAN_NA_JI_REQ_CS = 1139,				// ��Ȧ�ɼ�����
		MT_TAO_QUAN_NA_JI_INFO_SC = 1140,				// ��Ȧ�ɼ��·�
		MT_TAO_QUAN_NA_JI_PLAY_END_INFO_SC = 1141,		// ��Ȧ�ɼ������·�

		MT_HUNDRED_GOD_BOSS_REQ_CS = 1145,				// ����֮ʦ����
		MT_HUNDRED_GOD_BOSS_INFO_SC = 1146,				// ����֮ʦ��Ϣ�·�

		MT_WILD_BOSS_OTHER_INFO_SC = 1147,				// Ұ������������Ϣ�·�

		MT_CHARISMATIC_TANABATA_INFO_SC = 1148,			// ������Ϧ�·�

		MT_OBJ_MOVE_CS = 1150,							// �ƶ�
		MT_FLY_BY_SHOE_CS = 1151,						// ��Ь
		MT_ESCAPE_FROM_CURRENT_POSITION = 1152,			// �ͻ������뿨��
		MT_AUTO_ESCAPE_FROM_CURRENT_POSITION = 1153,	// �ͻ����Զ����뿨��
		MT_GET_CUR_POS_CS = 1154,						// �����˿ͻ��˲�ѯ�Լ���ǰλ��
		MT_OBJ_SPEED_CHANGE_NOTICE = 1155,				// OBJ �ٶȸı�֪ͨ

		//////////////////////////////////////////////////�///////////////////////////////////////////////////////////////////
		MT_ACTIVITY_STATUS_SC = 1200,					// �״̬����
		MT_ACTIVITY_ENTER_REQ_CS = 1203,				// �������
		MT_ACTIVITY_TEAM_FIGHT_MATCH_CS = 1204,			// С�Ӿ���-����ƥ��
		MT_ACTIVITY_TEAM_FIGHT_NOTIC_SC = 1205,			// С�Ӿ���-ƥ��״̬
		MT_ACTIVITY_TEAM_FIGHT_SCORE_SC = 1206,			// С�Ӿ���-����
		MT_ACTIVITY_MAZE_RACER_HANDLE_REQ_CS = 1207,	// �Թ����� - ��������
		MT_ACTIVITY_MAZE_RACER_AMULET_NUM_SC = 1208,	// �Թ����� - ��������
		MT_ACTIVITY_MAZE_RACER_RANK_INFOR_SC = 1209,	// �Թ����� - ������Ϣ
		MT_ACTIVITY_TEAM_FIGHT_RANK_SC = 1210,			// С�Ӿ���-���а�
		MT_ACTIVITY_TEAM_FIGHT_RECORD_SC = 1211,		// С�Ӿ���-��¼
		MT_ACTIVITY_MAZE_RACER_HALL_INFOR_SC = 1212,	// �Թ����� - ������Ϣ
		MT_ACTIVITY_MAZE_RACER_COPY_INFOR_SC = 1213,	// �Թ����� - ������Ϣ
		MT_ACTIVITY_MAZE_RACER_PASS_INFOR_SC = 1214,	// �Թ����� - ͨ����Ϣ
		MT_ACTIVITY_TEAM_FIGHT_MATCH_SUCC_NOTICE_SC = 1215,	// С�Ӿ���-����֪ͨ
		MT_ACTIVITY_MAZE_RACER_BUFF_INFOR_SC = 1216,	// �Թ����� - ������Ϣ
		MT_ACTIVITY_MAZE_RACER_ACT_INSERT_SC = 1217,	// �Թ����� - �ͨ������
		MT_CHONGZHI_INFO_SC = 1218,						// ���س�ֵ�����Ϣ
		MT_CHONGZHI_OPERATE_CS = 1219,					// �����ֵ��ع��ܲ��� 
		MT_ACTIVITY_GUILD_FIGHT_HANDLE_REQ_CS = 1220,	// ������Ҷ� - ��������
		MT_ACTIVITY_GUILD_FIGHT_RANK_INFOR_SC = 1221,	// ������Ҷ� - ���а���Ϣ
		MT_ACTIVITY_GUILD_FIGHT_GUILD_INFO_SC = 1222,	// ������Ҷ� - �ҵļ�����Ϣ
		MT_ACTIVITY_GUILD_ANSWER_CUR_QUESTION_SC = 1223,// ������� - ������Ϣ
		MT_ACTIVITY_GUILD_ANSWER_ANSWER_QUEST_CS = 1224,// ������� - �ش���Ϣ
		MT_ACTIVITY_GUILD_FIGHT_PARTIC_NUM_SC = 1225,	// ������Ҷ� - �����������
		MT_ACTIVITY_GUILD_ANSWER_RESULT_INFOR_SC = 1226,// ������� - ������
		MT_ACTIVITY_GUILD_ANSWER_AWARD_SETTLE_SC = 1227,// ������� - ������Ϣ
		MT_ACTIVITY_ONLY_FIGHT_HANDLE_REQ_CS = 1228,	// ��һ�а�(���˰������Ҷ�) - ��������
		MT_ACTIVITY_ONLY_FIGHT_RANK_INFO_SC = 1229,		// ��һ�а�(���˰������Ҷ�) - ���а���Ϣ
		MT_ACTIVITY_ONLY_FIGHT_MY_INFO_SC = 1230,		// ��һ�а�(���˰������Ҷ�) - �ҵ���Ϣ
		MT_ACTIVITY_SIGN_UP_REQ_CS = 1231,				// ��ʱ���������
		MT_ACTIVITY_SIGN_UP_INFO_SC = 1232,				// ��ʱ�������Ϣ�·�
		MT_ACTIVITY_PARTNER_EXCHANGE_INFO_SC = 1233,	// �������һ��·�
		MT_ACTIVITY_PET_EXCHANGE_INFO_SC = 1234,		// ��������һ��·�
		MT_RA_TIAN_MING_DIVINATION_INFO_SC = 1235,					// ����-�������Ի��Ϣ
		MT_RA_TIANMING_DIVINATION_START_CHOU_RESULT_SC = 1236,		// ����-�������Բ��Խ��
		MT_RA_TIANMING_DIVINATION_REWARD_POOL_SC = 1237,		// ����-�������Ի񽱳���Ϣ
		MT_RA_SPORTS_HISTROY_SC = 1239,					// ����-�·�����
		MT_ACTIVITY_MAZE_RACER_TRANSPORT_POS_SC = 1240,		// �Թ����� - ������λ��
		MT_ACTIVITY_ENTER_INFO_SC = 1241,				// ��볡��Ϣ�·�
		MT_ACTIVITY_GUILD_FIGHT_SPECIAL_INFO_SC = 1242,	//������Ҷ�������Ϣ�·�
		MT_ACTIVITY_ONLY_FIGHT_SPECIAL_INFO_SC = 1243,	//��һ�а�������Ϣ�·�
		MT_RAND_ACTIVITY_MONEY_ADD_INFO_SC = 1244,		// ���������������
		MT_RAND_ACTIVITY_GIVE_SECRET_KEY_INFO_SC = 1245,// ��������Ѱ�������·�
		MT_RAND_ACTIVITY_CUMULATIVE_RECHARGE_SC = 1246,// �����ۼƳ�ֵ�·�
		MT_ACTIVITY_ADVANCE_FETCH_REQ_CS = 1247,		// ��ʱ���ǰ��ȡ��������
		MT_ACTIVITY_ADVANCE_FETCH_INFO_SC = 1248,		// ��ʱ���ǰ��ȡ������Ϣ
		MT_RELOAD_ACTIVITY_NOTICE_SC = 1250,			// �ȸ�������ض�֪ͨ
		MT_CMD_BUY_ITEM_CHECK_RET_SC = 1251,			// 
		MT_CMD_BUY_ITEM_CHECK_REQ_CS = 1252,			// ��ͨģ��ֱ���������ü��
		MT_ACTIVITY_MAZE_RACER_ROLE_INFO = 1253,		// �Թ����� - ��ɫ��Ϣ
		

		MT_RA_MAZE_SUMMER_MAP_INFO_SC = 1254,			// ����-�����Թ���ͼ�·�
		MT_RA_MAZE_SUMMER_MAP_SINGLE_INFO_SC = 1255,	// ����-�����Թ������ͼ�·�
		MT_RA_MAZE_SUMMER_SHOP_INFO_SC = 1256,			// ����-�����Թ��̵��·�
		MT_RA_MAZE_SUMMER_QUESTION_INFO_SC = 1257,		// ����-�����Թ���Ŀ�·�
		MT_RA_MAZE_SUMMER_TASK_INFO_SC = 1258,			// ����-�����Թ������·�
		MT_RA_MAZE_SUMMER_STATUS_INFO_SC = 1259,		// ����-�����Թ�����״̬�·�
		MT_RA_MAZE_SUMMER_FLAYINFO_FINISH_SC = 1260,	// ����-�����Թ��淨�����·�
		MT_RA_MAZE_SUMMER_MY_POS_INFO_SC = 1261,		// ����-�����Թ��ҵ�λ���·�

		MT_RA_QIONG_LOU_YU_YAN_INFO_SC = 1262,			// ����-��¥�����·�
		MT_RA_QIONG_LOU_YU_YAN_ROLE_INFO_SC = 1263,		// ����-��¥���������Ϣ�·�

		MT_NOTICE_CLIENT_RESTART_SC = 1264,				// ֪ͨ�ͻ�������

		MT_ACTIVITY_SILK_ROADS_REQ_CS = 1270,			// ��ʱ�-˿��֮·����
		MT_ACTIVITY_SILK_ROADS_SIMPLE_INFO_SC = 1271,	// ��ʱ�-˿��֮·������Ϣ�·�
		MT_ACTIVITY_SILK_ROADS_ROLE_ALL_INFO_SC = 1272,	// ��ʱ�-˿��֮·��ɫ������Ϣ
		MT_ACTIVITY_SILK_ROADS_ASSETS_INFO_SC = 1273,	// ��ʱ�-˿��֮·�ʲ���Ϣ�·�
		MT_ACTIVITY_SILK_ROADS_WAREHOUSE_CHANGE_SC = 1274,//��ʱ�-˿��֮·�ֿ�仯
		MT_ACTIVITY_SILK_ROADS_ALL_RANK_INFO_SC = 1275,	// ��ʱ�-˿��֮·���а���Ϣ�·�
		MT_ACTIVITY_SILK_ROADS_ALL_TRADING_LOG_SC = 1276,//��ʱ�-˿��֮·ó����Ϣ�б��·�
		MT_ACTIVITY_SILK_ROADS_SINGLE_TRADING_SC = 1277,// ��ʱ�-˿��֮·����ó����Ϣ�·�

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		MT_SKILL_LIST_SC = 1300,						// �����б�
		MT_SKILL_CHANGE_NOTICE = 1301,					// ������Ϣ���֪ͨ
		MT_LIFE_SKILL_INFO_SC = 1302,					// �������Ϣ
		MT_COMMON_USE_SKILL_CHANGE_NOTICE = 1303,		// ���ü��ܱ��֪ͨ
		MT_SKILL_EXP_CHANGE_NOTIFY_SC = 1304,			// ���ܾ����ֵ֪ͨ

		MT_NEW_LIFE_SKILL_REQ_CS = 1310,				// ���������
		MT_NEW_LIFE_SKILL_ALL_INFO_SC = 1311,			// �������Ϣ�·�
		MT_NEW_LIFE_SKILL_SINGLE_INFO_SC = 1312,		// ����ܵ�����Ϣ�·�
		MT_NEW_LIFE_SKILL_PROPS_ALL_INFO_SC = 1313,		// ����ʳ��������Ϣ�·�
		MT_NEW_LIFE_SKILL_PROPS_SINGLE_INFO_SC = 1314,	// ����ʳ�����͵����·�

		MT_FISH_GROUND_REQ_CS = 1315,					// ��������
		MT_FISHING_NOTIFY_SC = 1316,					// ����㲥
		MT_STOP_FISHING_NOTIFY_SC = 1317,				// ��������
		MT_LIFE_SKILL_CHANGE_CARD_SINGLE_INFO_SC = 1318,// ����������Ϣ�·�

		MT_NOTICE_CLIENT_RELOAD_SENSITIVE_WORDS_SC = 1320, //֪ͨ�ͻ����ض����д��б� (��G4����һ�µ�Э��ţ�
		
		MT_SKILL_OPERATE_REQ_CS = 1350,					// ���ܲ�������
		MT_SKILL_POSITION_MODIFY_CS = 1351,				// ��������˳��

		MT_TASK_WAITING_LIST = 1352,					// ����ȴ��б�

		MT_SPECIAL_ANECDOTE_STAR_CS = 1354,					// ������������ʼ
		MT_SPECIAL_ANECDOTE_TASK_LIST_SC = 1355,			// �������������б�
		MT_SPECIAL_ANECDOTE_TASK_SINGLE_SC = 1356,			// ������������ - ����

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		MT_ROLE_INFO_ACK_SC = 1400,						// �����ɫ������Ϣ����
		MT_ROLE_MONEY_INFO_SC = 1401,					// ��ɫ������Ϣ����
		MT_ROLE_ATTRIBUTE_LIST_SC = 1402,				// ��ɫ������Ϣ����
		MT_ROLE_ATTRIBUTE_PLAN_INFO_SC = 1403,			// ��ɫ���Լӵ���Ϣ����
		MT_ROLE_EXP_CHANGE_NOTICE_SC = 1404,			// ��ɫ����ı�֪ͨ
		MT_ROLE_LEVEL_CHANGE_NOTICE_SC = 1405,			// ��ɫ�ȼ��ı�֪ͨ
		MT_ROLE_PROFESSION_CHANGE_NOTICE_SC = 1406,		// ��ɫְҵ���֪ͨ
		MT_ROLE_PROFESSION_EXTRA_INFO_SC = 1407,		// ��ɫְҵ������Ϣ����
		MT_ROLE_SPEED_CHANGE_NOTICE_SC = 1408,			// ��ɫ���ٸı�֪ͨ
		MT_ROLE_STATUS_CHANGE_NOTICE_SC = 1409,			// ��ɫ״̬�ı�֪ͨ
		MT_ROLE_AUTO_TALK_INFO_SC = 1410,				// ��ɫ�Զ��Ի���Ϣ�·�
		MT_ROLE_AUTHORITY_INFO_CHANGE_SC = 1411,		// ��ɫȨ����Ϣ�ı��·�
		MT_ROLE_HISTORY_MAX_CAPABILITY_CHANGE_SC = 1412,// ��ɫ��ʷ������ֱ仯�·�


		MT_ALL_INFO_REQ_CS = 1450,						// ����������ҵ�ģ����Ϣ
		MT_ATTRIBUTE_PLAN_CREATE_CS = 1451,				// �����ɫ�����µĽ�ɫ�ӵ㷽��
		MT_ATTRIBUTE_PLAN_CHANGE_NAME_CS = 1452,		// ����Լӵ㷽������
		MT_ATTRIBUTE_PLAN_SWITCH_PLAN_CS = 1453,		// �����л��ӵ㷽��
		MT_ATTRIBUTE_PLAN_ADD_POINT_CS = 1454,			// ����ӵ�
		MT_ATTRIBUTE_PLAN_CLEAR_POINT_CS = 1455,		// ����ϴ��
		MT_ROLE_CAREER_ADVANCE_REQ_CS = 1456,			// ��ɫ��������
		MT_ROLE_CARRER_CHANGE_REQ_CS = 1457,			// ��ɫתְ����
		MT_ROLE_PROFESSION_EXTRA_INFO_REQ_CS = 1458,	// ��ɫְҵ������Ϣ����
		MT_ROLE_CHANGE_COLOR_REQ_CS = 1459,				// ��ɫְҵ�ı���ɫ����

		MT_ROLE_SYSTEM_SET_REQ_CS = 1460,				// ��ҿͻ�����������
		MT_ROLE_SYSTEM_SET_INFO_SC = 1461,				// ��ҿͻ���������Ϣ
		MT_ROLE_SYSTEM_SET_CHAT_GROUP_CS = 1462,		// ��ҿͻ�������Ⱥ������
		MT_ROLE_SYSTEM_SET_CHAT_GROUP_INFO_SC = 1463,	// ��ҿͻ�������Ⱥ����Ϣ

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		MT_KNAPSACK_INFO_ACK_SC = 1500,					// ���󱳰���Ϣ����
		MT_KNAPSACK_GRID_CHANGE_SC = 1501,				// �����������ӱ仯
		MT_USE_ITEM_SUC_SC = 1504,						// ʹ����Ʒ�ɹ�
		MT_KNAPSACK_EXTEND_SC = 1505,					// ��������������
		MT_LACK_ITEM_SC = 1506,							// ȱ����Ʒ��Ϣ
		MT_ITEM_INTO_KNAPSACK_SPECIAL = 1507,			// �����Ʒ����֪ͨ
		MT_RAND_ACTIVITY_RESUILT_NOTICE_SC = 1508,		// ����֪ͨ�·�

		MT_KNAPSACK_OPERA_REQ_CS = 1550,				// ������������
		MT_USE_ITEM_CS = 1551,							// ʹ����Ʒ
		MT_USE_OPTIONAL_GIFT_CS = 1552,					// ʹ����ѡ���
		MT_LACK_ITEM_NOTICE_FOR_TASK_COMMIT_SC = 1553,	// ȱ����Ʒ�ύ֪ͨ
		MT_COMMIT_TASK_ITEM_NOTICE_SC = 1554,			// �ύ������Ʒ֪ͨ
		MT_USE_OPTIONAL_GIFT_2_CS = 1555,				// ʹ����ѡ���2
		MT_USE_GOLD_RECOVERY_CS = 1556,					// ʹ��Ԫ���ָ�
		MT_USE_RECOVERY_NOTICE_SC = 1557,				// ʹ��Ԫ���ָ�
		MT_KNAPSACK_ITEM_NOTICE_SC = 1558,				// �����õ���Ʈ��֪ͨ		
		MT_USE_SPECIAL_LOGIC_ITEM_CS = 1559,			// ʹ����Ҫ�����߼�����
		MT_ACTIVE_SPECIAL_LOGIC_ITEM_CS = 1560,			// ���������߼�����
		MT_HEARSAY_INFO_CS = 1561,						// �����·�����
		MT_BUY_CMD_GIFT_REQ_CS = 1562,					// ������ֱ������(���)
		MT_HONG_BAO_USE_RET_SC = 1563,					// ���ʹ�÷���

		///////////////////////////////////////////////////////////////////// װ�� /////////////////////////////////////////////////////////////////////
		MT_EQUIPMENT_INFO_ACK_SC = 1600,				// װ����Ϣ����
		MT_EQUIPMENT_GRID_CHANGE_SC = 1601,				// ����װ�����ӱ仯
		MT_ELEMENT_CRYSTAL_CHANGE_NOTCIE_SC = 1602,		// Ԫ��ˮ�����֪ͨ
		MT_EQUIP_VOC_LOCK_INFO_SC = 1603,				// װ����������
		MT_EQUIPMENT_JEWERLY_UP_GREADE = 1604,			// װ��ϵͳǿ���·�
		MT_EQUIP_JEWERLY_RESONACE_INFO_SC = 1605,		// ��װ�����·�

		MT_ZAOHUA_EQUIP_OPERA_REQ_CS = 1640,			// �컯װ����������
		MT_ZAOHUA_BAG_LIST_SC = 1641,					// �컯����list
		MT_ZAOHUA_BAG_SIGNLE_INFO_SC = 1642,			// �컯���������޸�
		MT_ZAOHUA_WEAR_LIST_SC = 1643,					// �컯װ�����·�
		MT_ZAOHUA_WEAR_SIGNLE_SC = 1644,				// �컯װ���������·�
		MT_ZAOHUA_COMPOSE_RESULT_SC = 1645,				// �컯װ���������·�
		MT_ZAOHUA_WEAR_APPEARANCE_SC = 1646,			// �컯װ������·�
		MT_ZAOHUA_DISCOMPOSE_CS = 1647,					// �컯װ���б�ֽ�
		MT_ZAOHUA_COMMON_INFO_SC = 1648,				// �컯װ��ͨ���·�

		MT_EQUIPMENT_OPERA_REQ_CS = 1650,				// װ����������


		/////////////////////////////////////////////////////////////��ӡ///////////////////////////////////////////////////////////////
		MT_PET_GOD_PRINT_SIGLE_SC = 1660,// ��ӡ���� - �����޸�
		MT_PET_GOD_PRINT_BAG_INFO_SC = 1661,// ��ӡ����		
		MT_PET_GOD_PRINT_EXP_INFO_SC = 1662,// ��ӡ����� 
		MT_PET_GOD_PRINT_WEAR_SIGLE_INFO_SC = 1663,	//��ӡ�����������
		MT_PET_GOD_PRINT_SET_WEAR_SIGLE_REQ_CS = 1664,	//������ӡ��������
		MT_PET_GOD_PRINT_LOG_LIST_SC = 1665,	//��ӡ������־�б�
		MT_PET_GOD_PRINT_LOG_DETAIL_INFO_SC = 1666,	//������־��ϸ�·�
		MT_PET_GOD_PRINT_WISH_REWARD_SC = 1667,		//�������·�
		MT_PET_GOD_PRINT_USE_ITEM_UPLEVEL_CS = 1668,//��ӡʹ����ӡ����

		///////////////////////////////////////////////////////////////////// ����/////////////////////////////////////////////////////////////////////
		MT_PET_FOLLOW_PET_INFO_SC = 1690,				// ���������Ϣ
		MT_SEAL_PET_GET_ITEM_INFO_SC = 1691,				// ��ץ����õ�ʲô��Ʒ
		MT_PET_ID_RECORD_LIST_SC = 1692,				// ����ͼ��
		MT_ABANDON_PET_BY_LIST_CS = 1693,				// ������������
		MT_MOVE_PET_GRID_NOTICE_SC = 1694,				// �ƶ����ﵽ�ֿ�򱳰�֪ͨ
		MT_PET_SKIN_INFO_SC = 1695,						//  ����Ƥ��
		MT_PET_CAL_ATTR_RET_SC = 1696,					//  
		MT_PET_SOUL_UPLEVEL_REQ_CS = 1697,				// ��������
		MT_PET_BENEDICTION_ALL_INFO_SC = 1698,			// ����ף���б���Ϣ�·�
		MT_PET_BENEDICTION_SINGLE_INFO_SC = 1699,		// ��������ף����Ϣ�·�

		///////////////////////////////////////////////////////////////////// ����2 /////////////////////////////////////////////////////////////////////
		MT_PET_OP_CS = 1700,							// ����
		MT_PET_RENAME_CS = 1701,						// ����
		MT_PET_ASSIGN_ATTRIBUTE_POINTS_CS = 1702,		// �������Ե�
		MT_SEND_PET_ALL_INFO_SC = 1703,					// �������е���Ϣ ֻ�ڵ�¼ʱ��һ��
		MT_SEND_PET_SINGLE_INFO_SC = 1704,				// �����������Ϣ
		MT_SEND_PET_OTHER_INFO_SC = 1705,				// ����������Ϣ
		MT_SEND_PET_DELETE_INFO_SC = 1706,				// ɾ������
		MT_SEND_PET_ATTRS_INFO_SC = 1707,				// �������Է���
		MT_SEND_PET_BREAKTHROUGHS_RET_SC = 1708,		// ����ǿ������
		MT_SEND_PET_GET_NOTICE_SC = 1709,				// ��ó��ﷵ��

		///////////////////////////////////////////////////////////////////// ��� /////////////////////////////////////////////////////////////////////
		MT_PARTNER_HANDLE_REQUEST_CS = 1710,			// ���ϵͳ��������
		MT_PARTNER_ALL_DATA_INFOR_SC = 1711,			// ���л���б���Ϣ
		MT_PARTNER_COMBATS_STATUS_SC = 1712,			// ���ս��״̬��Ϣ
		MT_PARTNER_ATTRIBUTE_LIST_SC = 1713,			// ��������б���Ϣ
		MT_PARTNER_FETTERS_INFOMA_SC = 1714,			// �����б���Ϣ
		MT_PARTNER_ID_GRADE_STAGE_SC = 1715,			// �������׼���Ϣ
		MT_PARTNER_SKILL_SELECTOR_CS = 1716,			// ��鼼�ܱ�������
		MT_PARTNER_SKILL_INFORMAT_SC = 1717,			// ��鵱ǰ������Ϣ
		MT_PARTNER_SYN_FIGHT_LIST_SC = 1718,			// ����ս˳���б�
		MT_PARTNER_SYN_ALTER_POSI_CS = 1719,			// ����ս�б����

		///////////////////////////////////////////////////////////////////// ���� /////////////////////////////////////////////////////////////////////
		MT_TASK_SINGLE_HANDLE_CS = 1720,				// ���������������
		MT_TASK_INFORMAT_LIST_SC = 1721,				// ���������б���Ϣ
		MT_TASK_DETELE_HANDLE_SC = 1722,				// �Ƴ�������Ϣ
		MT_TASK_TALK_WITH_NPC_CS = 1723,				// �� NPC ��̸
		MT_TASK_GIVE_NPC_ITEM_CS = 1724,				// ���� NPC ����
		MT_TASK_COMPLETE_TASK_SC = 1725,				// ����ɵ������б�
		MT_TASK_CAN_ABANDONED_SC = 1726,				// �����ܷ񱻷���		0 - ���ܣ� 1 - ����
		MT_TASK_SINGLE_STATUS_SC = 1727,				// ��������ǰ״̬   0 - δ���У� 1 - ����ɣ� 2 - ������
		MT_TASK_CHAIN_OP_CS		 = 1728,				// ����������
		MT_TASK_CHAIN_INFO_SC	 = 1729,				// ��������Ϣ

		///////////////////////////////////////////////////////////////////// ���� /////////////////////////////////////////////////////////////////////
		MT_WORKSHOP_HANDLE_REQUEST_CS = 1730,			// ����ϵͳ��������
		MT_WORKSHOP_ALL_DATA_INFOR_SC = 1731,			// ��������������Ϣ
		MT_WORKSHOP_INDIVID_RECIPE_SC = 1732,			// �����䷽������Ϣ
		MT_WORKSHOP_REFORGE_RESULT_SC = 1733,			// �����ضͽ���б�
		MT_WORKSHOP_GEM_UPGRA_MATE_SC = 1734,			// ��ʯһ����������
		MT_WORKSHOP_EXC_SKILL_BOOK_CS = 1735,			// �һ����＼����
		MT_WORKSHOP_COMPOSE_RESULT_SC = 1736,			// �����ϳɽ���ظ�
		MT_WORKSHOP_EQUIP_VOC_CHANGE_CS = 1737,			// װ����������
		MT_WORKSHOP_JEWELRY_TIMES_CS = 1738,			// ���κϳ�ʧ�ܴ���

		MT_PET_EQUIPMENT_REQ_CS = 1739,					// ����װ������

		///////////////////////////////////////////////////////////////////// ���� /////////////////////////////////////////////////////////////////////
		MT_JEWELRY_HANDLE_REQUEST_CS = 1740,			// ����ϵͳ��������
		MT_JEWELRY_ALL_INFO_REPLY_SC = 1741,			// ����������Ϣ�ظ�
		MT_JEWELRY_SINGLE_CHANGES_SC = 1742,			// ���ε����ı�֪ͨ

		/////////////////////////////////////////////////////////////////// ����Ԥ�� ///////////////////////////////////////////////////////////////////
		MT_SYSTEM_NOTICE_GET_AWARD_CS = 1750,			// ����Ԥ����ȡ����
		MT_SYSTEM_NOTICE_FLAG_INFO_SC = 1751,			// ����Ԥ����ȡ��Ϣ

		/////////////////////////////////////////////////////////////////// ���߾��� ///////////////////////////////////////////////////////////////////
		MT_OFFLINE_EXP_ADD_INFO_SC = 1755,				// ���߾����ۻ���Ϣ
		MT_OFFLINE_EXP_RECEIVES_CS = 1756,				// ���߾�����ȡ����

		///////////////////////////////////////////////////////////////////// NPC /////////////////////////////////////////////////////////////////////
		MT_NPC_HANDLE_REQUEST_CS = 1760,				// NPC ģ���������
		MT_NPC_GATHER_TIMER_S_SC = 1761,				// NPC ģ��ɼ�ʱ��
		MT_NPC_CANCEL_GATHERS_SC = 1762,				// NPC ģ��ȡ���ɼ�
		MT_NPC_GATHER_NOTIFYS_SC = 1763,				// NPC ģ��ɼ��㲥
		MT_NPC_GATHER_SUCCESS_SC = 1764,				// NPC ģ��ɼ��ɹ�
		MT_NPC_INFO_NOTIFY_SC = 1765,					// NPC ģ����Ϣ�����ı�
		MT_NPC_COMMIT_BACK_SC = 1766,					// NPC ģ�齻����Ϣ����
		MT_NPC_SHOP_INFO_SC = 1767,						// NPC �̵���Ϣ����
		MT_NPC_SHOP_BUY_CS = 1768,						// NPC ��������

		/////////////////////////////////////////////////////////////////// ���װ�� ///////////////////////////////////////////////////////////////////
		MT_PARTNER_EQUIP_OPERATOR_CS = 1770,			// ���װ�� ��������
		MT_PARTNER_EQUIP_SLOT_INF_SC = 1771,			// ���װ�� ������λ��Ϣ

		/////////////////////////////////////////////////////////////////// �����Ʒ ///////////////////////////////////////////////////////////////////
		MT_PARTNER_QUALITY_UP_SC = 1772,				// �����Ʒ��Ϣ
		MT_PARTNER_QUALITY_UP_INFO_CS = 1773,			// �����Ʒ����
		
		/////////////////////////////////////////////////////////////////// ���ˮ�� ///////////////////////////////////////////////////////////////////
		MT_PARTNER_USE_ELEMENT_CRYTAL_INFO_SC = 1774,			// ���Ԫ��ˮ��װ��
		MT_PARTNER_USE_ELEMENT_CRYTAL_TO_FIGHT_SC = 1775,		// ���Ԫ�ؿ���װ��
		MT_PARTNER_ELEMENT_CRYTAL_REQ_CS = 1776,				// ���Ԫ��ˮ������

		/////////////////////////////////////////////////////////////////// ���Ƥ�� ///////////////////////////////////////////////////////////////////
		MT_PARTNER_SKIN_ACTIVE_SC = 1777,						// ���Ƥ����������Ϣ
		MT_PARTNER_SKIN_SINGLE_OPER_SC = 1778,					// ���Ƥ�����������·�
		MT_PARTNER_SKIN_REQ_CS = 1779,							// ���Ƥ������

		/////////////////////////////////////////////////////////////////// �ɳ�֮· ///////////////////////////////////////////////////////////////////
		MT_COG_OPERATOR_REQUEST_CS = 1780,				// �ɳ�֮· ��������
		MT_COG_ALL_INFORMATIONS_SC = 1781,				// �ɳ�֮· ������Ϣ
		MT_COG_SINGLE_TASK_PROG_SC = 1782,				// �ɳ�֮· �������� ����������Ϣ
		MT_COG_SINGLE_FLAG_STAT_SC = 1783,				// �ɳ�֮· �������� ������ȡ״̬
		MT_COG_STAR_REWARD_STAT_SC = 1784,				// �ɳ�֮· �Ǽ����� ��ǰ�Ǽ�����״̬
		MT_COG_SINGLE_SE_NOTIFY_SC = 1785,				// �ɳ�֮· �����������֪ͨ
		MT_ROLE_COURSE_ALL_INFO_SC = 1786,				// �ɳ�֮· ����������Ϣ�·�
		MT_ROLE_COURSE_SINGLE_INFO_SC = 1787,			// �ɳ�֮· ���̵�����Ϣ�·�
		MT_ROLE_COURSE_NOTICE_INFO_SC = 1788,			// �ɳ�֮· ������Ϣ֪ͨ�·�
		MT_SERVER_COURSE_ALL_INFO_SC = 1789,			// �ɳ�֮· ���������Ϣ�·�

		/////////////////////////////////////////////////////////////////// ��ֵϵͳ ///////////////////////////////////////////////////////////////////
		MT_FSS_OPERATOR_REQUEST_CS = 1790,				// ��ֵϵͳ - ��������
		MT_FSS_ALL_INFORMATIONS_SC = 1791,				// ��ֵϵͳ - ������Ϣ
		MT_FSS_SINGLE_GRID_INFO_SC = 1792,				// ��ֵϵͳ - ����������Ϣ
		MT_FSS_SINGLE_RES_STATE_SC = 1793,				// ��ֵϵͳ - ������Դ��Ϣ
		MT_FSS_LEVEL_EXPERIENCE_SC = 1794,				// ��ֵϵͳ - �ȼ��뾭��ֵ
		MT_FSS_TIME_SC			   = 1795,				// ��ֵϵͳ - ʱ�����Ϣ
		MT_RES_TIME_ALL_SC		   = 1796,				// ��ֵϵͳ - �����ʱ�����Ϣ

		///////////////////////////////////////////////////////////////////// ���� /////////////////////////////////////////////////////////////////////
		MT_PRESTIGE_ALL_VILLAGE_INFO_SC = 1800,			// ����ȫ��������Ϣ
		MT_PRESTIGE_ALL_SHOP_IINFO_SC = 1801,			// ����ȫ���̵���Ϣ
		MT_PRESTIGE_SINGLE_VILLAGE_INFO_SC = 1802,		// �����������ӵ��鱨
		MT_PRESTIGE_SINGLE_SHOP_INFO_SC = 1803,			// ���������̵���鱨
		MT_PRESTIGE_SHOPING_CS = 1804,					// ������Ʒ
		MT_PRESTIGE_DONATE_INFO_SC = 1805,				// ������Ϣ
		MT_PRESTIGE_DONATE_OP_CS = 1806,				// ��������
		MT_PRESTIGE_DONATE_NOTICE_SC = 1807,			// ����֪ͨ
		MT_PRESTIGE_CHANGE_NOTICE_SC = 1808,			// �����䶯֪ͨ

		///////////////////////////////////////////////////////////////////// ���� /////////////////////////////////////////////////////////////////////
		MT_FUNCGUIDE_OPERATOR_REQUEST_CS = 1830,		// �������� - ��������
		MT_FUNCGUIDE_EVENT_FLAG_INFOR_SC = 1831,		// �������� - �¼����												
		MT_PET_DEAD_GUIDE_TRIGGER_CS = 1832,			// ���������ٻ�ָ������
		MT_PET_DEAD_GUIDE_INFO_SC = 1833,				// ���������ٻ�ָ�������·�

		//////////////////////////////// �һ��̵�(�ٻ�) ///////////////////////////////////////////
		MT_CONVERT_SHOP_BUY_REQ_CS = 1850,
		MT_CONVERT_SHOP_BUY_RECORD_LIST_SC = 1851,
		MT_CONVERT_SHOP_ADD_BUY_RECORD_SC = 1852,

		//////////////////////////////// Boss Chapter /////////////////////////////////////////////
		MT_BOSS_CHAPTER_INFO_SC = 1860,
		MT_BOSS_CHAPTER_REQ_CS = 1861,

		//////////////////////////////// ����Ԥ�� /////////////////////////////////////
		MT_FUNC_PREVIEW_INFO_SC = 1870,
		MT_FUNC_PREVIEW_CLAIM_REWARD_CS = 1871,

		/////////////////////////////// ����ϵͳ //////////////////////////////////////
		MT_REALM_BREAK_REQ_CS = 1880,					// ���󾳽�ϵͳ��ز���
		MT_REALM_BREAK_ALL_INFO_SC = 1881,				// ����ϵͳȫ����Ϣ�·�
		//MT_REALM_BREAK_START_TUNA_SC = 1882,			// ����ϵͳ��ʼ�����·�
		MT_REALM_BREAK_ITEM_INFO_SC = 1883,				// ����ϵͳ������Ϣ�·�

		/////////////////////////////��ͼ̽��//////////////////////////////
		MT_MAP_EXPLORATION_REQ_CS = 1890,				// ��ͼ̽������
		MT_MAP_EXPLORATION_INFO_SC = 1891,				// ��ͼ̽���·�
		///////////////////////////////// �ƺ� /////////////////////////////////////////////////////////////////////
		MT_TITLE_ALL_INFO_SC = 1900,					// �ƺ�ȫ����Ϣ
		MT_TITLE_SINGLE_INFO_SC = 1901,					// �ƺŵ�����Ϣ
		MT_TITLE_OTHER_INFO_SC = 1902,					// �ƺ�������Ϣ
		MT_TITLE_OP_CS = 1903,							// �ƺŲ���
		MT_TITLE_CHANGE_CS= 1904,						// �����ƺŸ���
		MT_TITLE_NOTICE_SC = 1905,						// �ƺ���Ϣ֪ͨ

		///////////////////////////////////////////////////////////////////// ���� /////////////////////////////////////////////////////////////////////
		MT_TRAIN_ALL_INFO_SC = 1920,					// ������Ϣ
		MT_TRAIN_REQ_CS = 1921,							// ��������
		MT_ROLE_PET_TRAIN_SKILL_REQ_CS = 1922,			// ���� �������Լ���
		MT_TRAIN_INFO_PROMOTED_SC = 1923,				// ����������������Ϣ�·�
		MT_TRAIN_RED_DOT_FLAG_SC = 1924,				// �������ܺ���ʶ�·�

		/////////////////////////////////////////////////////////////////// �������� ///////////////////////////////////////////////////////////////////
		MT_GUILD_WEEK_TASK_ALL_INFOR_CS = 1930,			// �������� - ������Ϣ
		MT_GUILD_WEEK_TASK_RANK_LIST_CS = 1931,			// �������� - ������Ϣ

		///////////////////////////////////////////////////////////////////// ��Ʒ /////////////////////////////////////////////////////////////////////
		MT_COLLECTION_ALL_INFO_SC = 1950,				// ��Ʒ������Ϣ
		MT_COLLECTION_SINGLE_INFO_SC = 1951,			// ��Ʒ������Ϣ
		MT_COLLECTION_OP_CS = 1952,						// ��Ʒ����
		MT_COLLECTION_INCLUDED_SC = 1953,				// ��Ʒ��¼
		MT_COLLECTION_INCLUDED_SIGNLE_SC = 1954,		// ��Ʒ������¼�·�
		MT_COLLECTION_ATLAS_CARD_INFO_SC = 1955,		// ��Ʒͼ�����·�
		MT_COLLECTION_ATLAS_CARD_SINGLE_SC = 1956,		// ��Ʒͼ���������·�

		///////////////////////////////////////////////////////////////// ����ͨ�Ŀ��� /////////////////////////////////////////////////////////////////
		MT_WST_QUESTION_ALL_INFO_SC = 1960,
		MT_WST_QUESTION_OP_CS = 1961,
		MT_WST_QUESTION_OVER_INFO_SC = 1962,

		/////////////////////////////////////////////////////////////////// �������� ///////////////////////////////////////////////////////////////////
		MT_ONLINE_REWARD_INFO_SC = 1970,				// ���߽�����Ϣ
		MT_SIGN_REWARD_INFO_SC = 1971,					// ǩ��������Ϣ
		MT_LEVEL_REWARD_INFO_SC = 1972,					// �ȼ�������Ϣ
		MT_FETCH_REWARD_REQ_CS = 1973,					// ��ȡ����
		MT_FIND_REWARD_REQ_CS = 1974,					// �����һ�
		MT_FIND_REWARD_INFO_SC = 1975,					// �����һ���Ϣ
		MT_SCORE_REWARD_INFO_SC = 1976,					// ���ֽ�����Ϣ
		MT_MONTH_INVESTMENT_INFO = 1977,				// �¶�Ͷ����Ϣ
		MT_REWARD_NOTIFY_REQUEST_CS = 1978,				// ���������һع���ȡ�������ʾ
		MT_REWARD_NOTIFY_INFO_SC = 1979,				// ���������һع��ܺ���·�

		///////////////////////////////////////////////////////////////////// �û� /////////////////////////////////////////////////////////////////////
		MT_SURFACE_ALL_INFO_SC = 1980,					// �û�ȫ����Ϣ
		MT_SURFACE_OP_CS = 1981,						// �û�����
		MT_SURFACE_SINGLE_INFO_SC = 1982,				// �û�������Ϣ
		MT_SURFACE_WEAR_INFO_SC = 1983,					// �û�������Ϣ
		MT_SURFACE_DECOMPOSE_CS = 1984,					// ʱװ�ֽ�(ͨ����Ʒ�ֽ�)

		MT_FASHION_COLLECTION_REQ_CS = 1985,			// ʱװ��������
		MT_FASHION_COLLECTION_INFO_SC = 1986,			// ʱװ����ȫ����Ϣ

		// ------------- �������� ------------
		MT_DAY_REWARD_ONLINE_GIFT_INFO_SC = 1990,		// ��������-���������Ϣ�·�

		////////////////////////////////////////////////////////////////// ����Сϵͳ //////////////////////////////////////////////////////////////////
		MT_GM_COMMAND_SC = 2000,
		MT_EXP_CRYSTAL_OPEN_CS = 2001,					// ��ˮ������
		MT_EXP_CRYSTAL_INFO_SC = 2002,					// ��ˮ����Ϣ
		MT_TREVI_FOUNTAIN_OPEN_CS = 2003,				// ��Ը
		MT_TREVI_FOUNTAIN_INFO_SC = 2004,				// ��Ը����Ϣ
		MT_ENCOUNTER_MINE_MONSTER_FLAG_SC = 2005,		// �����������÷���
		MT_SET_BATTLE_POSITION_SC = 2006,				// ��������վλ����
		MT_SET_BATTLE_AUTO_INFO_SC = 2007,				// ����ս���Զ���Ϣ����
		MT_TASK_SET_TRACE_REQ_CS = 2008,				// ��������׷��
		MT_TASK_TRACE_INFORMA_SC = 2009,				// ����׷����Ϣ
		MT_REFER_ROLE_EQUIPMENT_CS = 2010,				// ����װ�����Բ�ѯ
		MT_ROLE_QUERY_INFOR_SC = 2011,					// ����װ�������·�
		MT_ROLE_SALARY_INFO_CS = 2012,					// ���ǹ�����Ϣ����
		MT_ROLE_SALARY_INFO_SC = 2013,					// ���ǹ�����Ϣ�·�
		MT_LOUD_SPEAKER_CS = 2014,						// ʹ������ȫ���㲥
		MT_SPEAKER_INFO_SC = 2015,						// ������Ϣ
		MT_TASK_CHAIN_MONSTER_SC = 2016,				// ������ - ˢ������֪ͨ
		MT_PATROL_CS = 2017,							// Ѳ����������
		MT_NEW_CHONG_ZHI_CONFIG_INFO_CS = 2018,			// �³�ֵ����Ϣ����
		MT_TASK_PATH_FINDING_CS = 2019,					// �������Ѱ·֪ͨ�����
		MT_TASK_PATROL_CS = 2020,						// Ѳ�������·�
		MT_TASK_SKIP_BATTLE_CS = 2021,					// ��������ս��
		MT_PARTNER_SINGLE_INFO_SC = 2022,				// �·���鵥����Ϣ
		MT_PARTNER_GIFT_TREE_LIST_INFO_SC = 2023,		// �·�����츳����Ϣ
		MT_PARTNER_SINGLE_GIFT_TREE_INFO_SC = 2024,		// �·���鵥���츳����Ϣ
		MT_ROLE_QUERY_SIMPLE_INFO_SC = 2025,			// ������������·�
		MT_TASK_GAME_PLAY_CS = 2026,					// ����С��Ϸ
		MT_SERVER_COURSE_SINGLE_INFO_SC = 2027,			// �ɳ�֮· ������ɵ�����Ϣ�·�
		MT_PARTNER_SOAR_NOTICE_SC = 2028,				// �·����������֪ͨ
		MT_TASK_COMMIT_MONEY_CS = 2029,					// �ύ����

		MT_SET_ENCOUNTER_MINE_MONSTER_FLAG_CS = 2030,	// ������������
		MT_SET_BATTLE_POSITION_CS = 2031,				// ��������վλ
		MT_SET_BATTLE_AUTO_CS = 2032,					// ����ս���Զ�
		MT_SET_BATTLE_AUTO_MOVE_CS = 2033,				// ����ս���Զ���Ϊ
		MT_CHANGE_APPEARANCE_CS = 2034,					// �ı�����
		MT_CHANGE_HEADSHOT_CS = 2035,					// ��ɫ�ı�ͷ������
		MT_ATTR_LIST_REASON_SC = 2036,					// ������Դ�б��·�
		MT_SIGNLE_ATTR_REASON_REQ_CS = 2037,			// ����������Դ�б��·�

		MT_TREASURE_CHEST_REWARD_LIST_SC = 2039,		// ֪ͨ��ұ��佱��
		MT_NOTICE_NEARBY_OPEN_TREASURE_CHEST_SC = 2040,	// ֪ͨ�������䱻��
		MT_NOTICE_CFG_UPDATE_SC = 2041,					// �������ø���֪ͨ
		MT_NEW_CHONG_ZHI_CONFIG_INFO_SC = 2042,			// �³�ֵ���·���Ϣ
		MT_NEW_CHONG_ZHI_CONFIG_RELOAD_NOTICE_SC = 2043,// �³�ֵ���ض�

		MT_PARTNER_SWITCH_FIGHT_POS_CS = 2044,			// ��黥����սλ��
		MT_ROLE_SALARY_DAY_SC = 2045,					// ����ÿ�������ֵ�빤��
		MT_ROLE_LOGIN_SYSTEM_INFO_SC = 2046,			// �ۼƵ�½�·�
		MT_ROLE_LOGIN_SYSTEM_REQ_CS = 2047,				// �ۼƵ�½����
		MT_ROLE_SPECIAL_NOTICE_SC = 2048,				// ������Ϣϵͳ��֪ͨ

		MT_PARTNER_SKILL_ACTIVATE_CS = 2049,			// ��鼼�����á�����

		// �������м�

		MT_GM_COMMAND_CS = 2050,		

		MT_HEARSAY_INFO_SC = 2059,						// ������ʾ��Ϣ�·�
		MT_BABY_INFO_SC = 2060,							// ��Ů��Ϣ
		MT_BABY_REQ_CS = 2061,							// ��Ů�������
		MT_BABY_RENAME_CS = 2062,						// ��Ů������
		MT_BABY_PLAY_ROUTE_SC = 2063,					// ѯ�ʿ�ʼС��Ϸ
		MT_BABY_START_PLAY_SC = 2064,					// С��Ϸ��ʼ
		MT_BABY_ACTIVE_CS = 2065,						// ������Ů
		MT_ROLE_HISTORY_CHIVALROUS = 2066,				// ��ʷ����ֵ�·�
		MT_ROLE_HISTORY_CHIVALROUS_REWARD_FETCH = 2067,	// ��ʷ����ֵ������ȡ����

		MT_FRIEND_REQ_CS = 2070,						// ���Ѳ�������
		MT_FRIEND_INFO_SC = 2071,						// ������Ϣ		
		MT_FRIEND_LIST_CHANGE_SC = 2072,				// ���� �б�仯֪ͨ
		MT_ADD_FRIEND_ROUTE_SC = 2073,					// ���� ���ת��
		MT_ASK_ADD_FRIEND_LIST_SC = 2074,				// ��Ӻ��������б�
		MT_BLACK_LIST_CHANGE_SC = 2075,					// ������ �б�仯֪ͨ
		MT_BLACK_INFO_SC = 2076,						// ������Ϣ	
		MT_FRIEND_EDIT_GRUOP_CS = 2077,					// ��������
		MT_FRIEND_MOVE_GRUOP_CS = 2078,					// �ƶ����ѵ��ĸ�����
		MT_FRIEND_RENAME_GRUOP_CS = 2079,				// ����������
		MT_FRIEND_GRUOP_INFO_SC = 2080,					// ���ѷ�����Ϣ
		MT_RECENTLY_TEAM_INFO_SC = 2081,				// ��������Ϣ
		MT_ONE_RECENTLY_TEAM_INFO_SC = 2082,			// ��������Ϣ�ı�
		MT_RECENTLY_CHAT_CHANGE_SC = 2083,				// �����ϵ�˸ı�
		MT_RECENTLY_CHAT_INFO_SC = 2084,				// �����ϵ����Ϣ
		MT_REPLY_ROLE_IS_ONLINE_SC = 2085,				// �ظ�����Ƿ�����

		MT_RECOMMEND_FRIEND_LIST_REQ_CS = 2090,			// �����Ƽ��б�
		MT_RECOMMEND_FRIEND_LIST_SC = 2091,				// �����Ƽ��б�
		MT_ADD_RECOMMEND_FRIEND_CS = 2092,				// ������Ӻ����Ƽ�
		MT_RECOMMEND_FRIEND_ONE_SC = 2093,				// ����һ�������Ƽ�
		MT_REFRESH_RECOMMEND_FRIEND_CS = 2094,			// �����Ƽ��б�һ�� 
		MT_FUZZY_SEARCH_ROLE_INFO_CS = 2095,			// ����ģ����ѯ���н�ɫ
		MT_FUZZY_SEARCH_ROLE_INFO_SC = 2096,			// ģ����ѯ���н�ɫ��Ϣ
		MT_GIVE_FLOWER_NOTICE = 2097,					// �ͻ�֪ͨ

		MT_SVIP_CLIENT_CUSTOM_DATA_SC = 2098,			// SVIP�ͻ����Զ��������·�
		MT_SVIP_CLIENT_CUSTOM_DATA_CS = 2099,			// SVIP�ͻ����Զ�����������

		//����ϵͳ
		MT_SMART_MOUNTS_SYSTEM_REQ_CS = 2100,			// ����ϵͳ����
		MT_SMART_MOUNTS_SYSTEM_ALL_INFO_SC = 2101,		// ����ϵͳ��Ϣ�·�
		MT_SMART_MOUNTS_SYSTEM_SINGLE_INFO_SC = 2102,	// ����ϵͳ������Ϣ�·�
		MT_SMART_MOUNTS_SYSTEM_ATTR_LIST_SC = 2103,		// ������������б��·�
		MT_SMART_MOUNTS_SYSTEM_DOMINANCE_INFO_SC = 2104,// ����ͳ��������Ϣ�·�
		MT_SMART_MOUNTS_SYSTEM_SHOW_INFO_SC = 2105,		// ���ﵯ������չʾ��Ϣ�·�
		MT_SMART_MOUNTS_SYSTEM_REFINE_SKILL_SC = 2106,	// ����ϴ�����ܽ���·�
		MT_SMART_MOUNTS_SYSTEM_OTHER_INFO_SC = 2107,	// ����������Ϣ�·�
		MT_SMART_MOUNTS_SYSTEM_RELATED_INFO_SC = 2108,	// ���������Ϣ�·�
		MT_SMART_MOUNTS_SYSTEM_SKILL_REQ_CS = 2109,		// ����ϵͳ��������

		MT_SHITU_OPERA_MAKE_RELATION_REQ_CS = 2110,		// ʦͽϵͳ   ��ʦ����ͽ ����
		MT_SHITU_OPERA_MAKE_RELATION_AKC_CS = 2111,		// ʦͽϵͳ   �ظ���ʦ����ͽ ����
		MT_SHITU_OPERA_BREAK_RELATION_REQ_CS = 2112,	// ʦͽϵͳ   ��ҵ���ʦ ����
		MT_SHITU_OPERA_MAKE_RELATION_ROUTE_SC = 2113,	// ʦͽϵͳ   ��ʦ����ͽ ת��
		MT_SHITU_OPERA_RELATION_DATA_SC = 2114,			// ʦͽϵͳ   ʦͽ��Ϣ
		MT_SHITU_OPERA_BREAK_RELATION_ROUTE_SC = 2115,	// ʦͽϵͳ   ��ҵ���ʦ ת��
		MT_SHITU_OPERA_BREAK_RELATION_AKC_CS = 2116,	// ʦͽϵͳ   �ظ� ��ҵ���ʦ 
		MT_SHITU_OPERA_REQ_CS = 2117,					// ʦͽϵͳ   �������
		MT_SHITU_FB_SCENE_INFO_SC = 2118,				// ʦͽ������Ϣ
		MT_SHITU_COMMON_INFO_SC = 2119,				// ʦͽͨ����Ϣ
		MT_SHITU_INVITATION_NOTICE_SC = 2120,		//ʦͽ�����������
		MT_SHITU_FINISH_NOTICE_SC = 2121,		//ʦͽ��������֪ͨ
		MT_SHITU_BATTLE_RESULT_SC = 2122,		//ʦͽ����ս�����֪ͨ
		
		MT_SHITU_PREFERENCES_INFO_SC = 2124,		//ʦͽϵͳ ƫ��������Ϣ
		MT_SHITU_SEEKING_REQ_CS = 2125,					// ʦͽϵͳ ����
		MT_SHITU_SEEKING_INFO_SC = 2126,				// ʦͽϵͳ ��Ϣ

		MT_SERVER_FIRST_KILL_REQ_CS = 2127,			// ȫ����ɱ  ��Ϣ����
		MT_SERVER_FIRST_KILL_AKC_SC = 2128,			// ȫ����ɱ  ��Ϣ�·�

		//--------------------------����--------------------------------------
		MT_CHANNEL_CHAT_CS = 2130,						// ����Ƶ������
		MT_SINGLE_CHAT_CS = 2131,						// ����˽��
		MT_CHANNEL_CHAT_SC = 2132,						// Ƶ�����췵��
		MT_SINGLE_CHAT_SC = 2133,						// ˽�ķ���
		MT_SINGLE_CHAT_USET_NOT_EXIST_SC = 2134,		// ˽�� ��ʾ�û�������
		MT_CHAT_ITEM_INFO_REQ_CS = 2135,				// ��������Ʒ��Ϣ����
		MT_CHAT_ITEM_INFO_RET_SC = 2136,				// ��������Ʒ��ϸ��Ϣ����
		MT_REALTIME_COMMENT_CS = 2137,					// ��Ļ��������
		MT_REALTIME_COMMENT_SC = 2138,					// ��Ļ�ذ�
		MT_REPLY_WHETHER_FORBIDEN_SC = 2139,			// ��������Ƿ񱻽���

		MT_COURAGE_CHALLENGE_REQ_CS = 2140,				// ������ս ����
		MT_COURAGE_CHALLENGE_ALL_INFO_SC = 2141,			// ������ս ������Ϣ
		MT_COURAGE_CHALLENGE_FIRST_KILL_INFO_SC = 2142,	// ������ս ��ɱ��Ϣ
		MT_COURAGE_CHALLENGE_SINGLE_INFO_SC = 2143,			// ������ս ������Ϣ
		MT_COURAGE_CHALLENGE_SCORE_SINGLE_INFO_SC = 2144,	// ������ս ���ֵ�����Ϣ

		MT_MONSTER_WAVE_INFO_SC = 2145,					// ħ����Ϣ
		MT_MONSTER_WAVE_FINISH_INFO_SC = 2146,			// ħ�������Ϣ

		MT_CHAT_RECORD_LIST_SC = 2147,					// ��������¼
		MT_CHAT_RECORD_SET_CS = 2148,					// ��������¼����
		MT_CHAT_SIMPLE_RECORD_LIST_SC = 2149,			// ��ݷ��Լ�¼
		MT_CHAT_SIMPLE_RECORD_SET_CS = 2150,			// ��ݷ��Լ�¼����
		MT_DELETE_MUTE_PLAYER_CHAT_RECORD_SC = 2151,	// ɾ���漰ĳ��ҵ������¼
		MT_COURAGE_CHALLENGE_PET_HELPER_NOTICE_SC = 2152,	// ������ս����ħ֮����������ս֪ͨ
		MT_USER_CHAT_RECORD_CS = 2153,					// ��ѯ�û��������¼

		MT_MEDAL_REQ_CS = 2160,							// ѫ������
		MT_MEDAL_INFO_SC = 2161,						// ѫ����Ϣ
		MT_MEDAL_FB_SCENE_INFO_SC = 2162,				// ѫ�¸�����Ϣ
		MT_MEDAL_FB_KILL_BOSS_SC = 2163,				// ѫ�¸�����ɱboss
		MT_MEDAL_DECOMPOSE_NOTICE_SC = 2164,			// ѫ�·ֽ�֪ͨ
		MT_MEDAL_TASK_PLAY_VIDEO_CS = 2165,				// ѫ�����񲥷Ŷ���
		MT_MEDAL_LOTTREY_INFO_SC = 2166,				// ѫ�³齱�·�
		MT_MEDAL_TAKE_OFF_SC = 2167,					// ѫ������
		MT_MEDAL_UPGRADE_NOTICE = 2168,					// ѫ�½���
		MT_MEDAL_SPECIAL_EXCHANGE = 2169,				// ���ѫ���û�֪ͨ
		MT_MEDAL_ONE_KEY_GRADE_CS = 2170,				// ����һ������
		MT_MEDAL_UP_GRADE_SPECIAL = 2171,				// �����������

		MT_AUTO_CATCH_PET_CS = 2175,					// �Զ�ץ����������
		MT_AUTO_CATCH_PET_SC = 2176,					// �Զ�ץ��������Ϣ

		MT_ROLE_GIFT_GATHER_DREAM_LIST_SC = 2177,		// ��ҽ��켯Ը������Ϣ
		MT_ROLE_CAN_PUBLISH_GATHER_DREAM_LIST_SC = 2178,// ��ҿɷ����ļ�Ը��Ϣ�б�
		MT_ROLE_GAHTER_DREAM_RECORD_INFO_LIST_SC = 2179,// ��Ҽ�Ը��¼�б��·�

		/////////////////////////////////////////////////////////////////// �������� ///////////////////////////////////////////////////////////////////
		MT_SCHOOL_TASK_ALL_INFO_SC = 2180,
		MT_SCHOOL_TASK_SINGLE_INFO_SC = 2181,
		MT_SCHOOL_TASK_REQ_CS = 2182,
		MT_BOUNTY_TASK_TURN_TABLE_FETCH_REWARD_CS = 2183,
		MT_BOUNTY_TASK_TURN_TABLE_INFO_SC = 2184,
		MT_BOUNTY_TASK_INFO_SC = 2185,

		////////////////////////////////////////////////////////////////// ���Ф��о� //////////////////////////////////////////////////////////////////
		MT_RESEARCH_TASK_OPERATOR_CS = 2190,			// �����о���������
		MT_RESEARCH_TASK_ALL_INFO_SC = 2191,			// �����о�������Ϣ
		MT_RESEARCH_TASK_AUTO_START_CS = 2192,			// �����о��Զ���ʼ

		/////////////////////////////////////////////////////////////////// ����BOSS ������boss��///////////////////////////////////////////////////////////////////
		MT_WROLD_BOSS_REQ_CS = 2200,					// ����BOSS ��������
		MT_WROLD_BOSS_INFO_SC = 2201,					// ����BOSS ��Ϣ
		MT_WROLD_BOSS_RESULT_SC = 2202,					// ����BOSS ���������������Ϣ
		MT_WROLD_BOSS_POS_INFO_SC = 2203,				// ����BOSS λ����Ϣ
		MT_WROLD_BOSS_BATTLE_INFO_SC = 2204,				// ����BOSS ÿ��ս������һЩ������Ϣ
		MT_WROLD_BOSS_ROLE_INFO_SC = 2205,				// ����BOSS �����Ϣ
		MT_WROLD_BOSS_RANK_INFO_SC = 2206,				// ����BOSS ���а���Ϣ

		//-------------------------------- ��ͼ -----------------------------------------
		MT_TREASURE_MAP_REQ_CS = 2210,					// ��ͼ����
		MT_TREASURE_MAP_INFO_SC = 2211,					// ��ͼ��Ϣ
		MT_TREASURE_MAP_ITEM_INFO_SC = 2212,			// �ر�ͼ��Ϣ
		MT_TREASURE_MAP_ITEM_OPEN_RESULT_SC = 2213,		// �ر�ͼ�򿪻����Ʒ��Ϣ
		MT_TREASURE_MAP_BATCH_USED_RESULT_SC = 2214,	// �߼��ر�ͼ����ʹ�û����Ʒ��Ϣ

		//-------------------------------- ���� -----------------------------------------
		MT_ESCORT_ALL_INFO_SC = 2220,					// ������Ϣ
		MT_ESCORT_REQ_CS = 2221,						// ���ڲ���
		MT_ESCORT_RESULT_NOTIC_SC = 2222,				// ���ڽ��֪ͨ
		MT_ESCORT_STATUS_CHANGE_NOTICE_AREA_SC = 2223,	// ����״̬�ı�֪ͨ

		////////////////////////////////////// ��ͼ���� //////////////////////////////////////
		MT_MAP_UNLOCK_ENTIRE_ACTIVE_FLAG_LISTS_SC = 2230,	// ��ͼ���� ���н�������б���Ϣ
		MT_MAP_UNLOCK_FIRST_ENTER_SCENE_NOTICE_SC = 2231,	// ��ͼ���� �״ν����������֪ͨ
		MT_MAP_UNLOCK_REMOVE_UNLOCK_ANIMATIONS_CS = 2232,	// ��ͼ���� �Ƴ���ͼ�״ν�������
		MT_MAP_UNLOCK_FIRST_ENTER_FB_TYPE_CS = 2233,		// ��ͼ���� �Ƿ��һ�ν��븱������

		////////////////////////////////////// ��ս���� //////////////////////////////////////
		MT_CHALLENGE_FB_ROLE_INFOR = 2240,					// ��ս���� - ������Ϣ
		MT_CHALLENGE_FB_AWARD_LIST = 2241,					// ��ս���� - ��������

		////////////////////////////////////// �������� /////////////////////////////////////
		MT_MYSTERIOUS_TRIAL_CS = 2250,						// �������� - ����(����)
		MT_MYSTERIOUS_TRIAL_INFO_SC = 2251,					// �������� - ��Ϣ�·�
		MT_MYSTERIOUS_TRIAL_SINGLE_INFO_SC = 2252,			// �������� - ������Ϣ�·�
		MT_MYSTERIOUS_TRIAL_FINALLY_REWARD_SC = 2253,		// �������� - ���ս�����Ϣ�·�

		//////////////////////////////////// �������� - ˿��֮· ////////////////////////////
		MT_TRANSACTION_TASK_CS = 2255,						// �������� - ˿��֮·����
		MT_TRANSACTION_TASK_INFO_SC = 2256,					// �������� - ˿��֮·��Ϣ�·�

		////////////////////////////////////// ��Ծϵͳ ////////////////////////////////////
		MT_ACTIVE_SYSTEM_CS = 2257,							// ��Ծϵͳ - ��Ϣ����
		MT_ACTIVE_SYSTEM_INFO_SC = 2258,					// ��Ծϵͳ - ��Ϣ�·�

		////////////////////////////////////// һ����� ////////////////////////////////////
		MT_ONE_KEY_FINISH_SC = 2259,

		////////////////////////////////////// �������� ////////////////////////////////////
		MT_POKER_REWARD_CS = 2260,

		MT_ACTIVE_SYSTEM_SINGLE_INFO_SC = 2264,				// ��Ծϵͳ - ���������·�
		MT_ACTIVE_SYSTEM_EXTRA_REWARD_FETCH_CS = 2265,		// ��Ծϵͳ - ��ȡ����
		MT_ACTIVE_SYSTEM_SPECIAL_ACTIVE_INFO_SC = 2266,		// �����Ծ��Ϣ�·�

		///////////////////////////////////// �ķ� /////////////////////////////////////
		MT_HEART_SKILL_REQ_CS = 2270,						// �ķ�����
		MT_HEART_SKILL_ALL_INFO_SC = 2271,					// �ķ�������Ϣ�·�	
		MT_HEART_SKILL_SINGLE_INFO_SC = 2272,				// �ķ�������Ϣ�·�
		MT_HEART_SKILL_OTHER_INFO_SC = 2273,				// �ķ�������Ϣ�·�
		MT_HEART_SKILL_DISPLACE_REQ_CS = 2274,				// �ķ��û��ϳ�����

		///////////////////////////////////// �¿��ܿ� ////////////////////////////////////
		MT_MONTH_CARD_REQ_CS = 2280,						// �¿��ܿ�����
		MT_MONTH_CARD_INFO_SC = 2281,						// �¿��ܿ���Ϣ�·�

		////////////////////////////////// �������� ///////////////////////////////////////
		MT_PRESTIGE_TASK_INFO_SC = 2290,					// ����������Ϣ�·�
		MT_PRESTIGE_TASK_SELECT_VILLAGE_CS = 2291,			// ѡ���ׯ
		MT_PRESTIGE_TASK_FINAL_NOTICE = 2292,				// �����������֪ͨ
		MT_PRESTIGE_TASK_REQ_CS = 2293,						// ������������

		////////////////////////////////// ������ ///////////////////////////////////////////
		MT_TRADE_MARKET_CS_BEGIN = 2300,

		MT_ADD_ITEM_ON_SHELVE_CS = 2301,					// �ϼ�
		MT_REMOVE_ITEM_ON_SHELVE_CS = 2302,					// �¼�
		MT_CASH_OUT_CS = 2303,								// ����
		MT_TRADE_MARKET_USER_SHELVE_DATA_CS = 2304,			// ���˽�������������
		MT_TRADE_MARKET_SEARCH_BY_ITEM_ID_CS = 2305,		// ����ƷID������������Ʒ����
		MT_TRADE_MARKET_SEARCH_BY_ITEM_SUB_TYPE_CS = 2306,	// ����Ʒ����������������Ʒ����
		MT_TRADE_MARKET_BUY_ITEM_CS = 2307,					// ����
		MT_RECENT_TRANSACTION_RECORDS_CS = 2308,			// ��ѯ������׼�¼
		MT_TRADE_MARKET_GET_FOCUS_LIST_CS = 2309,			// ��ȡ��ע�б�
		MT_TRADE_MARKET_ADD_FOCUS_CS = 2310,				// ��ӹ�ע
		MT_TRADE_MARKET_REMOVE_FOCUS_CS = 2311,				// ȡ����ע
		MT_TRADE_MARKET_GET_FOCUS_ITEM_INFO_CS = 2312,		// ��ȡ��ע��Ʒ����������
		MT_TRADE_MARKET_ADVERTISE_CS = 2313,				// �����
		MT_TRADE_MARKET_SEARCH_ADVERTISE_CS = 2314,			// ���ҹ�����Ʒ
		MT_TRADE_MARKET_GET_SELLING_LIST_CS = 2315,			// ��ѯĳ��Ʒ�����������б�
		MT_TRADE_MARKET_ROLE_GET_TRADE_RECORD_CS = 2316,	// ��ȡ��ҽ��׼�¼���󣨹��򡢳��ۣ�
		MT_TRADE_MARKET_REDO_ADD_ITEM_ON_SHELVE_CS = 2317,	// �����ϼ�
		MT_TRADE_MARKET_SEARCH_PET_FOR_QUICK_BUY_CS = 2318,	// �����ݹ�������
		MT_TRADE_MARKET_GET_MERCHANDISE_NUM_CS = 2319,		// ������Ʒ����
		MT_TRADE_MARKET_SEARCH_MEDAL_BY_SPE_TYPE_AND_COLOR_CS = 2320,	// ����ɫ����Ч���Ͳ�������
		MT_TRADE_MARKET_QUERY_ITEM_RECOMMEND_PRICE = 2321,	// �����Ƽ��۸�

		MT_TRADE_MARKET_CS_END = 2350,

		MT_TRADE_MARKET_SC_BEGIN = MT_TRADE_MARKET_CS_END,
		MT_TRADE_MARKET_USER_SHELVE_DATA_SC = 2351,			// ���˽��������ݷ���
		MT_TRADE_MARKET_SEARCH_RESULT_SC = 2352,			// �������ݷ���
		MT_RECENT_TRANSACTION_RECORDS_SC = 2353,			// ����������׼�¼
		MT_TRADE_MARKET_GET_FOCUS_LIST_SC = 2354,			// ���ع�ע�б�
		MT_TRADE_MARKET_GET_FOCUS_ITEM_INFO_SC = 2355,		// ���ع�ע��Ʒ����������
		MT_TRADE_MARKET_GET_SELLING_LIST_SC = 2356,			// ��ѯĳ��Ʒ�����������б���
		MT_TRADE_MARKET_ROLE_GET_TRADE_RECORD_SC = 2357,	// ��ȡ��ҽ��׼�¼���أ����򡢳��ۣ�
		MT_TRADE_MARKET_SEARCH_PET_FOR_QUICK_BUY_SC = 2358,	// �����ݹ�����������
		MT_TRADE_MARKET_REFRESH_NOTICE = 2359, 				// ˢ��֪ͨ
		MT_TRADE_MARKET_GET_MERCHANDISE_NUM_SC = 2360,		// ��Ʒ�����·�
		MT_TRADE_MARKET_CREDIT_INFO_SC = 2361,				// ����������Ϣ�·�
		MT_TRADE_MARKET_QUERY_ITEM_RECOMMEND_PRICE_RESP = 2362,	// ������Ʒ�Ƽ��۸񷵻� 

		MT_TRADE_MARKET_SC_END = 2400,

		//--------------------------����--------------------------------------
		MT_FABAO_SEND_INFO_SC = 2450,				// �·�������Ϣ
		MT_FABAO_REQ_CS = 2451,						// ���󷨱�����
		MT_FABAO_SEND_SIGNL_INFO_SC = 2452,			// �·����������޸���Ϣ
		MT_FABAO_SEND_EQUIP_BAR_INFO_SC = 2453,		// �·�����װ�����޸���Ϣ
		MT_FABAO_DRAW_INFO_SC = 2454,		// �����齱��Ϣ
		MT_FABAO_DRAW_REWARD_INFO_SC = 2455,		// �����齱������Ϣ

		//--------------------------ˮ������--------------------------------------
		MT_CRYSTAL_FB_ALL_NPC_SC = 2459,			// ˮ����������NPC��Ϣ
		MT_CRYSTAL_FB_INFO_SC = 2460,				// ˮ��������Ϣ
		
		MT_CRYSTAL_FB_REQ_CS = 2462,				// ˮ����������
		MT_CRYSTAL_FB_USE_ITEM_INFO_SC = 2463,		// ˮ������ʹ����Ʒ��Ϣ
		MT_CRYSTAL_FB_OBJ_INFO_SC = 2464,			// ˮ������ʹ����������Ϣ

		//--------------------------���׸���---------------------------------------
		MT_ADVANCE_FB_INFO_SC = 2465,				//���׸�������������Ϣ
		MT_ADVANCE_FB_TASK_INFO_SC = 2466,			//���׸���������Ϣ

		//--------------------------����þ�(����)--------------------------------------
		MT_FANTASY_FB_SCENE_INFO_SC = 2467,			//����þ�����������Ϣ
		MT_FANTASY_FB_INFO_SC = 2468,				//����þ�������Ϣ
		MT_FANTASY_FB_REWARD_INFO_SC = 2469,		//����þ������ݴ潱����Ϣ(��ʱ����)

		//--------------------------Ұ��boss--------------------------------------
		MT_WILD_BOSS_NOTICE_SC = 2470,				// Ұ��boss����

		MT_ADVANCE_FB_DROP_BACK_SC = 2471,			//���׸���������ʱ����

		MT_WILD_BOSS_INFO_SC = 2472,				// Ұ��boss�·�
		MT_WILD_BOSS_REQ_CS = 2473,					// Ұ��boss����
		MT_WILD_BOSS_POSI_INFO_SC = 2474,			// Ұ��bossλ��
		MT_WILD_BOSS_POKER_SC = 2475,				// Ұ��boss�����·�
		MT_WILD_BOSS_POKER_CS = 2476,				// Ұ��boss��������
		MT_WILD_BOSS_SINGLE_SC = 2477,				// Ұ��boss�����·�

		MT_CRYSTAL_FB_FAST_PASS_CS = 2478,			// ˮ����������ɨ��
		MT_CRYSTAL_FB_ENERGY_INFO_SC = 2479,		// ˮ������������Ϣ

		//--------------------------buff--------------------------------------
		MT_BUFF_INFO_SC = 2480,
		//------------------------------------ shop ------------------------------------
		MT_SHOP_ALL_INFO_SC = 2490,
		MT_SHOP_SINGLE_INFO_SC = 2491,
		MT_SHOP_BUY_REQ_CS = 2492,
		MT_SHOP_FAST_BUY_CS = 2493,					// ���ٹ���
		MT_SHOP_GOLD_SHOP_BUY_CS = 2494,			// Ԫ���̳�
		MT_SHOP_GOLD_SHOP_ALL_INFO_SC = 2495,		// Ԫ���̳�
		MT_SHOP_GOLD_SHOP_SINGLE_INFO_SC = 2496,	// Ԫ���̳�
		MT_EXCHANGE_REQ_CS = 2497,			// �һ�
		MT_EXCHANGE_ALL_INFO_SC = 2498,		// �һ�
		MT_EXCHANGE_SINGLE_INFO_SC = 2499,	// �һ�

		//--------------------------treasure hunt--------------------------------------
		MT_XUN_BAO_REQ_CS = 2500,			// Ѱ��
		MT_XUN_BAO_RET_SC = 2501,			// Ѱ�����
		MT_XUN_BAO_OPEN_INFO_SC = 2502,			// Ѱ��������Ϣ
		MT_XUN_BAO_DRAW_TIMES_INFO_SC = 2503,			// Ѱ���齱������Ϣ
		MT_XUN_BAO_EXCHANGE_CS = 2504,			// Ѱ���һ�
		MT_XUN_BAO_BELSS_CS = 2505,				//Ѱ��������
		MT_XUN_BAO_BELSS_INFO_SC = 2506,		//Ѱ������Ϣ�·�

		// �ֻ�����
		MT_LUN_HUI_WANG_CHUAN_REQ_CS = 2507,	//�ֻ���������
		MT_LUN_HUI_WANG_CHUAN_INFO_SC = 2508,	//�ֻ������·�

		//--------------------------�ɼ�����--------------------------------------
		MT_GATHER_FB_INFO_SC = 2510,				// �ɼ�������Ϣ
		MT_GATHER_FB_GATHER_LIST_SC = 2513,			// �ɼ������б���Ϣ
		
		//--------------------------�´��ع�--------------------------------------
		MT_BRAVE_GROUND_REQ_CS = 2520,				// �´��ع�����
		MT_BRAVE_GROUND_RANK_SC = 2521,				// �´��ع����а���Ϣ
		MT_BRAVE_GROUND_SCENE_SC = 2522,			// �´��ع�������Ϣ
		MT_BRAVE_GROUND_REWARD_INFO_SC = 2523,		// �´��ع��㽱����Ϣ
		MT_BRAVE_GROUND_INFO_SC = 2524,				// �´��ع���Ϣ
		MT_BRAVE_GROUND_ROUTE_SC = 2525,			// �´��ع�ת��
		MT_BRAVE_GROUND_NOTIFY_SC =2526,			// �´��ع�֪ͨ
		MT_BRAVE_GROUND_USE_ITEM_INFO_SC = 2527,			// �´��ع�ʹ����Ʒ��Ϣ
		MT_BRAVE_GROUND_TEAM_INFO_SC = 2528,			// �´��ع����������Ϣ
		MT_BRAVE_GROUND_ROLE_INFO_SC = 2529,			// �´��ع������Ϣ
		MT_BRAVE_GROUND_COMMON_INFO_SC = 2530,			// �´��ع������һЩ��Ϣ

		//------------------------------------ shop 2 ------------------------------------
		MT_CANCEL_NOTIFY_REQUEST_CS = 2540,			// ȡ�����պ����ʾ����
		MT_PURCHASE_NOTIFY_INFOR_SC = 2541,			// ���պ����ʾ�·�

		//------------------------------------ �г� --------------------------------------
		MT_MARKET_REQ_CS = 2545,						// �г���������
		MT_MARKET_ALL_INFO_SC = 2546,					// �г�������Ϣ�·�
		MT_MARKET_SINGLE_INFO_SC = 2547,				// �г�������Ϣ�·�

		////////////////////////////////////////////////////////////////// ���  //////////////////////////////////////////////////////////////////
		MT_MARRY_REQ_CS = 2551,							// �������
		MT_MARRY_REQ_ROUTE_SC = 2552,					// �����Ϣת�����Է�
		MT_MARRY_REQ_INFO_SC = 2553,					// �����Ϣ
		MT_DIVORCE_REQ_ROUTE_SC = 2554,					// �������ת��
		MT_DIVORCE_PROPOSE_NOTICE_SC = 2555,			// ֪ͨ����������ѷ��͵��Է�
		MT_MARRIAGE_SEEKING_REQ_CS = 2556,				// ��ѯ����������ϸ��Ϣ����
		MT_MARRIAGE_SEEKING_INFO_SC = 2557,				// �������Խ����Ϣ
		MT_MARRIAGE_TASK_ROUTE_SC = 2558,				// ��Ե�ճ���������ת��
		MT_MARRIAGE_TASK_QUESTION_INFO_SC = 2559,		// ��Ե�ճ�������Ϣ
		MT_MARRIAGE_TASK_QUESTION_RESULT_SC = 2560,		// ��Ե�ճ����������
		MT_MARRIAGE_QUERY_OTHER_INFO_RET = 2561,		// ��ѯ���˽����Ϣ����
		MT_MARRIAGE_DUET_PUZZLE_INFO = 2562,			// ˫��ƴͼ��Ϣ
		MT_MARRIAGE_DUET_TAST_LOGOUT_NOTICE = 2563,		// ˫����Ϸ����֪ͨ

		//--------------------------������--------------------------------------
		MT_COLOSSEUM_REQ_CS = 2600,					//����
		MT_COLOSSEUM_ROLE_INFO_SC = 2601,			//������Ϣ
		MT_COLOSSEUM_TOP_INFO_SC = 2602,			//ǰ4��Ϣ
		MT_COLOSSEUM_TARGET_INFO_SC = 2603,			//������Ϣ
		MT_COLOSSEUM_RANK_INFO_SC = 2604,			//���а���Ϣ
		MT_COLOSSEUM_REPORT_INFO_SC = 2605,			//ս����Ϣ
		MT_COLOSSEUM_RANK_REWARD_SC = 2606,			//ÿ�ս�����ȡ��Ϣ
		MT_COLOSSEUM_PVPS_RESULT_SC = 2607,			//ս�����
		MT_COLOSSEUM_OPEN_STATUS_SC = 2608,			//�״̬���֪ͨ
		MT_COLOSSEUM_INPOOL_LIST_SC = 2609,			//����������
		MT_COLOSSEUM_NEW_REPORT_SC = 2610,			//��ս��֪ͨ
		MT_COLOSSEUM_NEW_DAY_REWARD_SC = 2611,		//��ÿ�ս���֪ͨ
		MT_COLOSSEUM_NEW_REPORT_DETAIL_SC = 2612,	//��ս������
		MT_COLOSSEUM_CLIENT_REFRESH_TARGET_NOTICE_SC = 2613, //֪ͨ�ͻ������������Ϣ
		MT_COLOSSEUM_ACHIEVEMENT_INFO_SC = 2614,	//�������ɾ���Ϣ�·�
		MT_COLOSSEUM_ACHIEVEMENT_REQ_CS = 2615,		//�������ɾ�����
		MT_COLOSSEUM_DETAL_INFO_SC = 2616,			//��������ϸ��Ϣ
		MT_COLOSSEUM_HISTORY_RANK_CHANGE_NOTICE = 2617, //��������ʷ�����仯֪ͨ
		MT_COLOSSEUM_SWEEP_RESULT_INFO = 2618,		// ������ɨ���ɹ�/ʧ��֪ͨ
		MT_COLOSSEUM_BATTLE_REWARD_NOTICE = 2619,	// ��������ս������֪ͨ

		//---------------------�ؾ�����
		MT_RA_MI_JING_QI_WEN_OBJ_INFO_SC = 2620,	// �ؾ�����obj��Ϣ

		//---------------------��ڤ����
		MT_NETHER_WORLD_PURGATORY_INFO_SC = 2630,	//��Ϣ�·�
		MT_NETHER_WORLD_PURGATORY_REQ_CS = 2631,	//�������
		MT_NETHER_WORLD_PURGATORY_SIGLE_INFO_SC = 2632,//������Ϣ�·�
		MT_NETHER_WORLD_PURGATORY_MI_LING_INFO_SC = 2633,//������Ϣ�·�
		MT_NETHER_WORLD_PURGATORY_SIGLE_CAPHTER_SC = 2634,//���½��·�

		//--------------------------��ϯ��ѡ--------------------------------------
		MT_CHIEF_ELECTION_REQ_CS = 2650,			// ��ϯ��ѡ ����
		MT_CHIEF_ELECTION_NOTICE_SC = 2651,			// ��ϯ��ѡ ֪ͨ
		MT_CHIEF_ELECTION_MATCH_INFO_SC = 2652,		// ��ϯ��ѡ ������Ϣ
		MT_CHIEF_ELECTION_ROLE_INFO_SC = 2653,		// ��ϯ��ѡ ��ɫ��Ϣ
		MT_CHIEF_ELECTION_WIN_ROLE_INFO_SC = 2654,	// ��ϯ��ѡ ʤ����Ϣ
		MT_CHIEF_ELECTION_JOIN_TOP_INFO_SC = 2655,	// ��ϯ��ѡ ��������Ϣ
		MT_CHIEF_ELECTION_FIGHTER_FINISH_NOTICE_SC = 2656,	// ��ϯ��ѡ ������ս�����
		MT_CHIEF_ELECTION_CHAMPION_SC = 2657,	// ��ϯ��ѡ �ھ�֪ͨ
		MT_CHIEF_ELECTION_SEA_ROLE_INFO_SC = 2658,	// 
		MT_CHIEF_ELECTION_SEA_SCENE_INFO_SC = 2659,	// 
		MT_CHIEF_ELECTION_STATE_INFO_SC = 2660,	// 
		MT_CHIEF_ELECTION_RANK_SC = 2661,	// 

		//--------------------------��ϯ�۷�ս--------------------------------------
		MT_CHIEF_PEAK_BATTLE_REQ_CS = 2700,			// ��ϯ�۷�ս ����
		MT_CHIEF_PEAK_BATTLE_NOTICE_SC = 2701,		// ��ϯ�۷�ս ֪ͨ
		MT_CHIEF_PEAK_BATTLE_MATCH_INFO_SC = 2702,	// ��ϯ�۷�ս ������Ϣ
		MT_CHIEF_PEAK_BATTLE_ROLE_INFO_SC = 2703,	// ��ϯ�۷�ս ��ɫ��Ϣ
		MT_CHIEF_PEAK_BATTLE_WINNER_INFO_SC = 2704,	// ��ϯ�۷�ս ʤ����Ϣ
		MT_CHIEF_PEAK_BATTLE_FIGHTER_FINISH_NOTICE_SC = 2705,	// ��ϯ�۷�ս ������ս�����
		MT_CHIEF_PEAK_BATTLE_CHAMPION_SC = 2706,	// ��ϯ�۷�ս �ھ�֪ͨ

		//--------------------------������ҫս--------------------------------------
		MT_GUILD_HONOR_REQ_CS = 2750,				// ������ҫս ����
		MT_GUILD_HONOR_ROLE_INFO_SC = 2751,			// ������ҫս ��ɫ��Ϣ
		MT_GUILD_HONOR_ROLE_CHANGE_SC = 2752,		// ������ҫս ��ɫ�ı�������Ϣ
		MT_GUILD_HONOR_ADD_ROLE_SC = 2754,		// ������ҫս ���볡��
		MT_GUILDHONOR_ZONE_BATTLE_RESULT_INFO_SC = 2755,		
		MT_GUILD_HONOR_ALL_ZONE_BATTLE_RESULT_INFO_SC = 2756,
		MT_GUILD_HONOR_ALL_ZONE_ROLE_NUM_INFO_SC = 2757,
		MT_GUILD_HONOR_REQ_2_CS = 2758,				// ������ҫս ����2
		MT_GUILD_HONOR_PROCESSINFO_SC = 2759,
		MT_GUILD_HONOR_SCORE_RANK_SC = 2760,
		MT_GUILD_HONOR_SCORE_RANK_INFO_SC = 2761,		
		MT_GUILD_HONOR_MATCH_INFO_SC = 2762,
		MT_GUILD_HONOR_GUESS_INFO_SC = 2763,
		MT_GUILD_HONOR_MATCH_RECORD_SC = 2764,
		MT_GUILD_HONOR_TOP_INFO_SC = 2765,
		MT_GUILD_HONOR_ROLE_BATTLE_STATE_SC = 2766,

		//--------------------------��̨վ--------------------------------------
		MT_PLATFORM_BATTLE_REQ_CS = 2780,			// ��̨վ ����
		MT_PLATFORM_BATTLE_ROLE_INFO_SC = 2781,	// ��̨վ ��ɫ�ϰ���Ϣ
		MT_PLATFORM_BATTLE_RANK_INFO_SC = 2782,	// ��̨վ ��̨�����Ϣ
		MT_PLATFORM_BATTLE_RANK_FIRST_INFO_REQ_CS = 2783,	// ��̨վ ��̨�����Ϣ
		MT_PLATFORM_BATTLE_RANK_FIRST_INFO_SC = 2784,	// ��̨վ ��̨�����Ϣ
		MT_PLATFORM_BATTLE_SWITCH_PLATFORM_CS = 2785,// �л���̨
		MT_PLATFORM_BATTLE_RECORD_CS = 2786,	// ��̨ս��
		MT_PLATFORM_BATTLE_RECORD_NOTICE_CS = 2787,	
		MT_PLATFORM_BATTLE_REQ_2_CS = 2788,
		MT_PLATFORM_BATTLE_ACHIEVEMENT_INFO_SC = 2789,
		MT_PLATFORM_BATTLE_SETTLE_NOTICE_SC = 2790,
		MT_PLATFORM_BATTLE_LAST_RANK_INFO_SC= 2791,

		//--------------------------���------------------------------------------
		MT_CROSS_START_REQ_CS = 2800,				// ��ʼ�������
		MT_CROSS_START_ACK_SC = 2801,				// ��ʼ�������
		MT_BACK_TO_ORIGIN_SERVER_CS = 2802,			// ���ر�������
		MT_BACK_TO_ORIGIN_SERVER_SC = 2803,			// ���ر�������

		MT_ROLE_INFO_NOTICE_REQ_CS = 2850,
		MT_ROLE_INFO_NOTICE_SC = 2851,		
		
		/////////////////////////// �۷�ȼ� //////////////////////////////////////	
		MT_ROLE_TOP_LEVEL_REQ_CS = 2860,		// �۷�ȼ�����
		MT_ROLE_TOP_LEVEL_ALL_INFO_SC = 2861,	// �۷�ȼ�������Ϣ�·�
		MT_ROLE_TOP_LEVEL_NOTICE_SC = 2862,		// �۷�ȼ�֪ͨ
		MT_ROLE_TOP_LEVEL_AOI_NOTICE_SC = 2863,	// �۷�ȼ�֪ͨ�ܱ����

		//--------------------------��������--------------------------------------
		MT_GUIDE_FB_INFO_SC = 2900,				// ����������Ϣ

		//--------------------------���龳(���ϸ���)------------------------------
		MT_MATERIAL_REQ_CS = 2910,				// ���龳��������
		MT_MATERIAL_SCENE_INFO_SC = 2911,		// ���龳������Ϣ
		MT_MATERIAL_ALL_INFO_SC = 2912,			// ���龳��ɫ������Ϣ
		MT_MATERIAL_SINGLE_INFO_SC = 2913,		// ���龳�����ؿ���Ϣ
		MT_MATERIAL_OTHER_INFO_SC = 2914,		// ���龳������Ϣ�·�

		//
		MT_SALARY_BATTLE_STAT_INFO_SC = 2920,	// ս��ǰͳ�Ʊ���ս������ֵ��Ϣ

		//--------------------------����ϵͳ--------------------------------------
		MT_FORMATION_SIMPLE_INFO_CS = 2950,		// ����ϵͳ������Ϣ����
		MT_FORMATION_SIMPLE_INFO_SC = 2951,		// ����ϵͳ������Ϣ����
		MT_FORMATION_DETAIL_INFO_CS = 2952,		// ����ϵͳ��ϸ��Ϣ����
		MT_FORMATION_DETAIL_INFO_SC = 2953,		// ����ϵͳ��ϸ��Ϣ����
		MT_FORMATION_RENAME_CS = 2954,			// ����ϵͳ����������
		MT_FORMATION_RENAME_SC = 2955,			// ����ϵͳ����������
		MT_FORMATION_SAVE_CS = 2956,			// ����ϵͳ����Ķ�����
		MT_FORMATION_DELETE_CS = 2957,			// ����ϵͳɾ������

		MT_PET_HELPER_FORMATION_SET_CS = 2970,	// ������ս������������
		MT_PET_HELPER_FORMATION_INFO_SC = 2971,	// ������ս������Ϣ����
		MT_PET_HELPER_FORMATION_QUERY_CS = 2972,// ������ս���ݲ�ѯ

		//--------------------------����--------------------------------------
		MT_RAND_ACTIVITY_OP_CS = 3000,							// ������������
		MT_RAND_ACTIVITY_TIME_LIMIT_STORE_INFO = 3001,			// ����-��ʱ�̵�
		MT_RAND_ACTIVITY_TURN_TABLE_INFO = 3003,				// ����-����ת��
		MT_RAND_ACTIVITY_TURN_TABLE_REWARD = 3004,				// ����-����ת��-����
		MT_RAND_ACTIVITY_DISCOUNT_STORE_INFO = 3005,			// ����-�ۿ��̵�
		MT_RAND_ACTIVITY_CONSUME_CAROUSEL_INFO = 3008,			// ����-����ת��
		MT_RAND_ACTIVITY_CONSUME_CAROUSEL_REWARD = 3009,		// ����-����ת��-����
		MT_RAND_ACTIVITY_CONSUME_CAROUSEL_SHOW = 3010,			// ����-����ת��-��ϡ��ʾ
		MT_FIRST_CHONGZHI_ACTIVITY_YIXIANER = 3011,				// �׳�-ҽ�ɶ�
		MT_FIRST_CHONGZHI_ACTIVITY_REQ_CS = 3012,				// �׳�����
		MT_RAND_ACTIVITY_HERITAGE_TREASURE_INFO_CS = 3013,		// ���й屦��Ϣ
		MT_RAND_ACTIVITY_SEVEN_COLOR_PICTURE = 3014,			// �߲�ͼ����Ϣ�·�
		MT_RAND_ACTIVITY_EXPEL_MONSTER_INFO_SC = 3015,			// ���������������·�
		MT_RAND_ACTIVITY_SALE_SHOP_INFO_SC = 3016,				// �����̵���Ϣ�·�
		MT_RAND_ACTIVITY_HAPPY_UP_INFO_SC = 3017,				// ����������Ϣ�·�
		MT_RAND_ACTIVITY_ANIMAL_RACE_INFO_SC = 3018,			// ���޾���������Ϣ�·�
		MT_RAND_ACTIVITY_ANIMAL_RACE_ROLE_SC = 3019,			// ���޾��������Ϣ�·�
		MT_RAND_ACTIVITY_LUCKY_BAG_INFO_SC = 3020,				// ������Ϣ
		MT_RAND_ACTIVITY_SEVEN_COLOR_REQ_CS = 3021,				// �߲�ͼ���ͻ�����������
		MT_RAND_ACTIVITY_SEVEN_COLOR_TYPE_CS = 3022,			// �߲�ͼ�����͵���������Ϣ
		MT_RAND_ACTIVITY_TIAN_JIANG_HAO_LI_INFO_CS = 3023,		// �콵������Ϣ
		MT_RAND_ACTIVITY_JOY_SEEKS_FAVORS_SC = 3024,			// ����Ѱ��ƥ��
		MT_RAND_ACTIVITY_JOY_SEEKS_FAVORS_MATH_SUCC_SC = 3025,	// ����Ѱ�赹��ʱ
		MT_RAND_ACTIVITY_JOY_SEEKS_FAVORS_MATH_REQ_CS = 3026,	// ����Ѱ������ʼս��
		MT_RAND_ACTIVITY_JIAN_DAN_QIN_XIN_INFO_SC = 3027,		// ����������Ϣ�·�
		MT_RAND_ACTIVITY_LINKAGE_GIFT_INFO_SC = 3028,			// ����������Ϣ�·�
		MT_RAND_ACTIVITY_COLLECT_WORD_INFO_SC = 3029,			// ���ֻ��Ϣ
		MT_RAND_ACTIVITY_DISCOUNT_CARNIVAL_INFO_SC = 3030,		// �ۿۿ���Ϣ
		MT_RAND_ACTIVITY_LUCKY_DISCOUNT_INFO_SC = 3031,			// �����ۿ���Ϣ
		MT_RAND_ACTIVITY_LUCKY_TURNTABLE_INFO_SC = 3032,		// ����ת����Ϣ�·�
		MT_RAND_ACTIVITY_LUCKY_TURNTABLE_REWARD_SC = 3033,		// ����ת�� - ����
		MT_RAND_ACTIVITY_ADVENTURE_GIFT_INFO_SC = 3035,			// ���������Ϣ
		MT_RAND_ACTIVITY_QUAN_MIN_DISCOUNT_INFO_SC = 3036,		// ȫ���Ź���Ϣ
		MT_RAND_ACTIVITY_ANIMAL_RACE_ANIMATION_INFO_SC = 3037,	// ���޾���������Ϣ�·�
		MT_RAND_ACTIVITY_TU_JIAN_TREASURE_INFO_SC = 3038,		// ͼ���ᱦ��Ϣ
		MT_RAND_ACTIVITY_GOD_TOKEN_INFO_SC = 3039,				// ����ս����Ϣ
		MT_RAND_ACTIVITY_ELEMENT_CARDS_INFO_SC = 3040,			// Ԫ�ط�����Ϣ
		MT_RAND_ACTIVITY_SIGN_IN_CONTINUITY_INFO_SC = 3041,		// ���Ǻ�����Ϣ
		MT_RAND_ACTIVITY_GIVE_PET_DRAW_INFO_SC = 3042,			// ��30���飨��Ѱ�����飩��Ϣ
		MT_RAND_ACTIVITY_MAKE_UP_REWARD_REQ_CS = 3045,			// ��Ӫ�-������������
		MT_RAND_ACTIVITY_BONUS_OP_CS = 3050,					// �������⽱��ͨ������
		MT_RAND_ACTIVITY_BONUS_INFO_SC = 3051,					// �������⽱����Ϣ�·�

		MT_RAND_ACTIVITY_CI_JIU_YING_XIN_INFO_SC = 3052,		// ����-�Ǿ�ӭ���·�
		MT_RAND_ACTIVITY_NIU_XING_DA_YUN_INFO_SC = 3053,		// ����-ţ�д����·�
		MT_RAND_ACTIVITY_SPRING_FESTIVAL_STORE_INFO_SC = 3054,	// ����-������·�
		MT_RAND_ACTIVITY_MONPOLY_GIFT_INFO_SC = 3055,			// ����-ţ�˵�ͷ�·�
		MT_RAND_ACTIVITY_MONPOLY_GIFT_EXCHANGE_INFO_SC = 3056,	// ����-ţ�˵�ͷ�һ��·�
		MT_RAND_ACTIVITY_MONPOLY_GIFT_QUESTION_INFO_SC = 3057,	// ����-ţ�˵�ͷ�����·�
		MT_RAND_ACTIVITY_COW_EXCHANGE_INFO_SC = 3058,			// ����-ţ�˶һ��·�
		MT_RAND_ACTIVITY_COW_STORE_INFO_SC = 3059,				// ����-ţ���̳��·�
		MT_RAND_ACTIVITY_COW_LUCKY_EXCHANGE = 3060,				// ����-ţ���ޱ�
		MT_RAND_ACTIVITY_LITTLE_YEAR_GIFT = 3061,				// ����-С������
		MT_RAND_ACTIVITY_PET_LOOT = 3062,						// ����-����ᱦ
		MT_RAND_ACTIVITY_TRADEMARKET_RETURN_GOLD_INFO_SC = 3063,// ����-�����з���
		MT_RAND_ACTIVITY_TRADEMARKET_RETURN_GOLD_HOT_ITEM_LIST = 3064, // ����-�����з���-�����б�
		MT_RAND_ACTIVITY_HAPPY_LOOK_INFO_SC = 3065,				// ����-�����ַ���
		MT_RAND_ACTIVITY_SIGN_HAPPY_INFO_SC = 3066,				// ����-��¼�ͺ���
		MT_RAND_ACTIVITY_WAN_LING_GONG_YING_INFO_SC = 3067,		// ����-���鹲ӯ

		MT_RAND_ACTIVITY_GOOD_GIFT_TURN_INFO_SC = 3070,			// ����-����ת����Ϣ�·�
		MT_RAND_ACTIVITY_GOOD_GIFT_TURN_REWARD_SC = 3071,		// ����-����ת����Ϣ�·�
		MT_RAND_ACTIVITY_HELP_OTHERS_INFO_SC = 3072,			// ����-�嵨������Ϣ�·�
		MT_RAND_ACTIVITY_SECRET_TREASURE_INFO = 3073,			// ����-�ػ��ر���Ϣ�·�
		MT_RAND_ACTIVITY_LAO_DONG_HAO_LI_INFO_SC = 3074,		// ����-�Ͷ�������Ϣ�·�
		MT_RAND_ACTIVITY_SUMMER_LI_LIAN_ALL_INFO_SC = 3075,		// ����-����������Ϣ�·�
		MT_RAND_ACTIVITY_COLORFUL_LANTERN_ALL_INFO_SC = 3076,	// ����-��ʻ�����Ϣ�·�
		MT_RAND_ACTIVITY_COLORFUL_LANTERN_GAME_INFO_SC = 3077,	// ����-��ʻ�����Ϸ��Ϣ�·�
		MT_RAND_ACTIVITY_COLORFUL_LANTERN_LIGHT_INFO_SC = 3078,	// ����-��ʻ��ƻ�����Ϣ�·�
		MT_RAND_ACTIVITY_SUMMER_STORE_INFO_SC = 3079,			// ����-�����̵���Ϣ�·�
		MT_RAND_ACTIVITY_LUCKY_SIGN_INFO_SC = 3080,				// ����-���˻�ǩ��Ϣ�·�
		MT_RAND_ACTIVITY_SUPER_OFF_SHOP_SC = 3081,				// ����-�����ۿ��̳���Ϣ�·�
		MT_RAND_ACTIVITY_SUMMER_STORE_SINGLE_INFO_SC = 3082,	// ����-�����̵굥����Ϣ�·�
		MT_RAND_ACTIVITY_SUPER_OFF_SHOP_SINGLE_INFO_SC = 3083,	// ����-�����ۿ��̳ǵ�����Ϣ�·�
		MT_RAND_ACTIVITY_YUAN_QI_TURNING_INFO_SC = 3084,		// ����-Ԫ��������Ϣ�·�
		MT_RAND_ACTIVITY_COOL_BREEZE_INFO_SC = 3085,			// ����-������ˬ��Ϣ�·�
		MT_RAND_ACTIVITY_COOL_BREEZE_SINGLE_INFO_SC = 3086,		// ����-������ˬ������Ϣ�·�
		MT_RAND_ACTIVITY_LOTUS_MI_BAO_INFO_SC = 3087,			// ����-�����ٱ���Ϣ�·�
		MT_RAND_ACTIVITY_MI_BAO_FANG_DIAN_INFO_SC = 3088,		// ����-�ٱ�������Ϣ�·�
		MT_RAND_ACTIVITY_MI_BAO_FANG_DIAN_SINGLE_INFO_SC = 3089,// ����-�ٱ����굥����Ϣ�·�
		MT_RAND_ACTIVITY_DUI_HUAN_LING_YU_CS = 3090,			// ����-�һ�����
		MT_RAND_ACTIVITY_HELP_OTHERS_INFO_2_SC = 3091,			// ����-�嵨����2  ����԰һ���£���Ϣ�·�
		MT_RAND_ACTIVITY_SHAN_HAI_QUESTION = 3092,				// ����-ɽ���ʴ���Ϣ�·�

		MT_RAND_ACTIVITY_HORCRUX_TIANCHENG_ROLE_INFO = 3093,	// ����-������� ��ɫ��Ϣ
		MT_RAND_ACTIVITY_HORCRUX_TIANCHENG_POOL_INFO = 3094,	// ����-������� ������Ϣ
		MT_RAND_ACTIVITY_HORCRUX_TIANCHENG_DRAW = 3095,			// ����-������� �齱����
		MT_RAND_ACTIVITY_INTEGRAL_DRAW_ROLE_INFO = 3096,		// ����-����齱 ��ɫ��Ϣ
		MT_RAND_ACTIVITY_INTEGRAL_DRAW_GAME_INFO = 3097,		// ����-����齱 ��Ϸ��Ϣ
		MT_RAND_ACTIVITY_INTEGRAL_DRAW_RESULT = 3098,			// ����-����齱 �齱����

		MT_RAND_ACTIVITY_SHAN_HAI_QUESTION_STATUS = 3099,		// ����-ɽ���ʴ����״̬��Ϣ�·�

		MT_RAND_ACTIVITY_PAI_MAI_HANG_NOTICE_INFO_SC = 3100,	// ����-������֪ͨ��Ϣ�·�
		MT_RAND_ACTIVITY_PAI_MAI_HANG_SINGLE_NOTICE_SC = 3101,	// ����-�����е���֪ͨ��Ϣ�·�
		MT_RAND_ACTIVITY_PAI_MAI_HANG_USER_INFO_SC = 3102,		// ����-�����н�ɫ��Ϣ�·�
		MT_RAND_ACTIVITY_PAI_MAI_HANG_USER_SINGLE_INFO_SC = 3103,// ����-�����н�ɫ������Ϣ�·�
		MT_RAND_ACTIVITY_PAI_MAI_HANG_USER_RECORD_INFO_SC = 3104,//	����-�����н�ɫ���ļ�¼�·�
		MT_RAND_ACTIVITY_BACK_REWARD_INFO_SC = 3105,			// ����-�ع������Ϣ�·�
		MT_RAND_ACTIVITY_BACK_REWARD_REQ_CS = 3106,				// ����-�ع��������

		MT_RAND_ACTIVITY_SERVER_COMPETITION_ROLE_ALL_INFO = 3107,// ����-ȫ����ƴ��ɫ������Ϣ�·�
		MT_RAND_ACTIVITY_SERVER_COMPETITION_SINGLE_INFO = 3108,	 // ����-ȫ����ƴ��ɫ������Ϣ�·�
		MT_RAND_ACTIVITY_SERVER_COMPETITION_ACTIVITY_INFO = 3109,// ����-ȫ����ƴ���Ϣ�·�

		MT_RAND_ACTIVITY_SHAN_HAI_MEET_INFO = 3110,				 // ����-ɽ�������Ϣ�·�
		MT_RAND_ACTIVITY_FATE_ENTANGLEMENT_INFO_SC = 3111,			 // ����-ɽ��Ե����Ϣ�·�

		MT_RAND_ACTIVITY_ZHAOCAIJINBAO_ROLE_INFO_SC = 3112,		// ����-�вƽ��� ��ɫ��Ϣ
		MT_RAND_ACTIVITY_ZHAOCAIJINBAO_DRAW_RESULT_SC = 3113,	// ����-�вƽ��� �齱����

		MT_RAND_ACTIVITY_YUN_ZE_TERRITORY_INFO_SC = 3114,		 // ����-���󻭾���Ϣ�·�
		MT_RAND_ACTIVITY_YUN_ZE_BLOOMING_RECORD_INFO_SC = 3115,	 // ����-���󷱻�¼��Ϣ�·�
		MT_RAND_ACTIVITY_MO_ZU_ZAI_NA_LI_SC = 3116,				 // ����-ħ����������Ϣ�·�
		MT_RAND_ACTIVITY_CHAOZHI_FENXIANG = 3117,				// ����-��ֵ����
		MT_RAND_ACTIVITY_YUN_ZE_SLOT_MACHINE_INFO_SC = 3118,	 // ����-����ͬ������Ϣ�·�
		MT_RAND_ACTIVITY_YUN_ZE_SLOT_MACHINE_RESULT_INFO_SC = 3119,	// ����-����ͬ����ҡ�������Ϣ�·�
		MT_RAND_ACTIVITY_YUN_ZE_ER_SAN_SHI_INFO_SC = 3120,		 // ����-�����������Ϣ�·�
		MT_RAND_ACTIVITY_SHAN_HAI_ZHENG_FENG_ALL_INFO_SC = 3121, // ����-ɽ��������Ϣ�·�
		MT_RAND_ACTIVITY_SHAN_HAI_MI_BAO_INFO_SC = 3122,		 // �������-ɽ���ٱ�
		MT_RAND_ACTIVITY_SHANHAI_HUADENG_ALL_INFO_SC = 3123,	// ����-ɽ��������Ϣ�·�
		MT_RAND_ACTIVITY_SHANHAI_HUADENG_GAME_INFO_SC = 3124,	// ����-ɽ��������Ϸ��Ϣ�·�
		MT_RAND_ACTIVITY_SHANHAI_HUADENG_LIGHT_INFO_SC = 3125,	// ����-ɽ�����ƻ�����Ϣ�·�
		MT_RAND_ACTIVITY_YUN_ZE_ER_SAN_SHI_TURNTABLE_INFO_SC = 3126,	// ����-���������ת����Ϣ�·�
		
		MT_RAND_ACTIVITY_JIXING_GAOZHAO_ROLE_INFO_SC = 3127,	// ����-���Ǹ��� ��ɫ��Ϣ
		MT_RAND_ACTIVITY_JIXING_GAOZHAO_POOL_INFO_SC = 3128,	// ����-���Ǹ��� ������Ϣ
		MT_RAND_ACTIVITY_JIXING_GAOZHAO_DRAW_SC = 3129,			// ����-���Ǹ��� �齱����
		MT_RAND_ACTIVITY_JIXING_GAOZHAO_RECORD_LIST_INFO_SC = 3130,// ����-���Ǹ��� ��¼�б�
		MT_RAND_ACTIVITY_JIXING_GAOZHAO_RECORD_INFO_SC = 3131,	// ����-���Ǹ��� ��Ӽ�¼

		MT_RAND_ACTIVITY_YUN_ZE_WISH_INFO_SC = 3132,			// ����-������Ը  ��Ϣ�·�
		MT_RAND_ACTIVITY_YUN_ZE_HAO_LI_INFO_SC = 3133,			// ����-�������  ��Ϣ�·�
		MT_RAND_ACTIVITY_YUN_ZE_CUTE_PET_INFO_SC = 3134,		// ����-�ȳ�ݵ�  ��Ϣ�·�
		MT_RAND_ACTIVITY_YUN_ZE_CATCH_ELVES_GET_INFO_SC = 3135,	// ����-�����ץ�� ץ����Ϣ�·�
		MT_RAND_ACTIVITY_YUN_ZE_CUTE_PET_CHALLENGE_INFO_SC = 3136,	//����-�ȳ�ݵ���ս��Ϣ�·�

		MT_RAND_ACTIVITY_SERVER_COMPETITION_OTHER_INFO_SC = 3137,// ����-ȫ����ƴ������Ϣ�·�

		MT_RAND_ACTIVITY_TIGER_FISH_EVERY_YEAR_INFO_SC = 3138,	// ����-�������� ��Ϣ�·�
		MT_RAND_ACTIVITY_YINHU_WENSHI_INFO_SC = 3139,			// ����-��������
		MT_RAND_ACTIVITY_TIGER_FIND_SPRING_INFO_SC = 3140,		// ����-����̽�� ��Ϣ�·�
		MT_RAND_ACTIVITY_TIGER_TEASE_NEWYEAR_INFO_SC = 3141,	// ����-�������´�
		MT_RAND_ACTIVITY_CINIU_YINGXINHU_INFO_SC = 3142,		// ����-��ţӭ�»�
		MT_RAND_ACTIVITY_YIN_HU_BAI_SUI_INFO_SC = 3143,			// ����-��������
		MT_RAND_ACTIVITY_HU_XING_DA_YUN_INFO_SC = 3144,			// ����-���д���
		MT_RAND_ACTIVITY_HU_XING_DA_YUN_USER_LIST_SC = 3145,	// ����-���д��� ���������б�
		MT_RAND_ACTIVITY_TIGER_LUCKY_MONEY_INFO_SC = 3146,		// ����-����ѹ��
		MT_RAND_ACTIVITY_TIGERAUSPICIOUS_SNOW_INFO_SC = 3147,	// ����-��ѩ���� ��Ϣ�·�
		
		MT_RAND_ACTIVITY_YUAN_QI_SPEED_INFO_SC = 3148,			// ����-Ԫ������
		MT_RAND_ACTIVITY_YUAN_QI_SPEED_PLAY_INFO_SC = 3149,		// ����-Ԫ��������Ϸ״̬

		MT_RAND_ACTIVITY_JINGJI_YUEKA_ALL_INFO_SC = 3150,		// ����-�����¿�������Ϣ�·�
		MT_RAND_ACTIVITY_JINGJI_YUEKA_NOTICE_SC = 3151,			// ����-�����¿����֪ͨ  
		MT_RAND_ACTIVITY_JINGJI_YUEKA_REFRESH_NOTICE_SC = 3152,	// ����-�����¿�����ˢ���·�   
		MT_RAND_ACTIVITY_JINGJI_YUEKA_SINGLE_TYPE_INFO_SC = 3153,//����-�����¿�����������Ϣ�·�   
		MT_RAND_ACTIVITY_JINGJI_YUEKA_TASK_LIST_INFO_SC = 3154, //����-�����¿������б���Ϣ�·�     

		MT_RAND_ACTIVITY_RONG_LIAN_YOU_LI_INFO_SC = 3155,		// ����-��������������Ϣ�·�
		MT_RAND_ACTIVITY_QIYUAN_KONGMINGDENG_SC = 3156,			// ����-��Ը������
		MT_RAND_ACTIVITY_XUNMENG_HUDIE_INFO_SC = 3157,			// ����-Ѱ�κ���
		MT_RAND_ACTIVITY_YUAN_QI_HELP_INFO_SC = 3158,			// ����-Ԫ�������·�
		MT_RAND_ACTIVITY_BO_ZHONG_YOU_LI_INFO_SC = 3159,		// ����-��������
		MT_RAND_ACTIVITY_GOD_BEAST_ADVENT_INFO_SC = 3160,		// ����-��������
		MT_RAND_ACTIVITY_YUAN_QI_FAN_PAI_INFO_SC = 3161,		// ����-Ԫ������
		MT_RAND_ACTIVITY_GOD_BEAST_ADVENT_OPEN_INFO_SC = 3162,	// ����-������������״̬��Ϣ
		MT_RAND_ACTIVITY_GOD_BEAST_ADVENT_BUY_INFO_SC = 3163,	// ����-��������ȫ���޹�����������Ϣ
		MT_RAND_ACTIVITY_WANLING_ZHULI_INFO_SC = 3164,			// ����-��������
		MT_RAND_ACTIVITY_BAO_ZI_LAI_LE_INFO_SC = 3165,			// ����-��������
		MT_RAND_ACTIVITY_YUAN_QI_JISHIBU_INFO_SC = 3166,		// ����-Ԫ�����²�
		MT_RAND_ACTIVITY_CHENG_FENG_PO_LANG_OPEN_INFO_SC = 3167,// ����-�˷����˿�����Ϣ
		MT_RAND_ACTIVITY_CHENG_FENG_PO_LANG_INFO_SC = 3168,		// ����-�˷�����
		MT_RAND_ACTIVITY_GOD_BEAST_ADVENT_NOTIFY_INFO_SC = 3169,// ����-���������øжȱ䶯֪ͨ
		MT_RAND_ACTIVITY_MIAO_BI_SHENG_HUA_INFO_SC = 3170,		//  ����-�������
		MT_RAND_ACTIVITY_ZHAOCAIJINBAO_RECORD_LIST_INFO_SC = 3171,	// ����-�вƽ��� �齱��¼��Ϣ�·�

		MT_RAND_ACTIVITY_ZONG_XIANG_MAN_CHENG_INFO = 3180,		//����-��������
		MT_RAND_ACTIVITY_SALTY_SWEET_BATTLE_INFO_SC = 3181,		// ����-����֮��
		MT_RAND_ACTIVITY_SALTY_SWEET_BATTLE_ROLE_INFO_SC = 3182,// ����-����֮�������·�
		MT_RAND_ACTIVITY_DUAN_YANG_XIANG_NANG_INFO = 3183,		// ����-��������
		MT_RAND_ACTIVITY_CANG_LONG_CI_FU_INFO = 3184,			// ����-�����͸�
		MT_RAND_ACTIVITY_LUCKY_BLIND_BOX_INFO_SC = 3185,		// ����-����ä��
		MT_RAND_ACTIVITY_LAN_TANG_CHU_MU_INFO = 3186,			// ����-��������
	
		MT_RAND_ACTIVITY_CHONG_JI_FAN_LI_INFO_SC = 3187,		// ����-�輼������Ϣ�·�
		MT_RAND_ACTIVITY_XIN_FU_QIANG_HUA_INFO_SC = 3188,		// ����-�·�ǿ��
		
		MT_RAND_ACTIVITY_QI_XI_TE_HUI_INFO = 3189,				// ����-��Ϧ�ػ���Ϣ
		MT_RAND_ACTIVITY_QI_XI_TE_HUI_SINGLE_INFO = 3190,		// ����-��Ϧ�ػݵ�����Ϣ
		MT_RAND_ACTIVITY_QI_XI_TE_HUI_OTHER_INFO = 3191,		// ����-��Ϧ�ػ�������Ϣ
		MT_RAND_ACTIVITY_PET_EXCHANGE_SIGNLE_INFO_SC = 3192,	// ����-����һ�������Ϣ�·�

		/////////////////////////////////////////////////////////////////// ����BOSS2 ������boss��///////////////////////////////////////////////////////////////////
		MT_WROLD_BOSS_2_RANK_INFO_SC = 3198,				// ����BOSS2 ���а���Ϣ
		MT_WROLD_BOSS_2_ROLE_INFO_SC = 3199,				// ����BOSS2 �����Ϣ
		MT_WROLD_BOSS_2_REQ_CS = 3200,					// ����BOSS2 ��������
		MT_WROLD_BOSS_2_INFO_SC = 3201,					// ����BOSS2 ��Ϣ
		MT_WROLD_BOSS_2_RESULT_SC = 3202,					// ����BOSS2 ���������������Ϣ
		MT_WROLD_BOSS_2_POS_INFO_SC = 3203,				// ����BOSS2 λ����Ϣ
		MT_WROLD_BOSS_2_BATTLE_INFO_SC = 3204,				// ����BOSS2 ÿ��ս������һЩ������Ϣ		

		/////////////////////////////////////////////////�������//////////////////////////////////////////
		MT_WORLD_QUESTION_INFO_SC = 3205,				//��������·�
		MT_WORLD_QUESTION_REQ_CS = 3206,				//�����������

		//////////////////////////////����BOSS3(������)///////////////////////////////////////
		MT_WORLD_BOSS_3_REQ_CS = 3210,					// ����BOSS3 ��������
		MT_WORLD_BOSS_3_INFO_SC = 3211,					// ����BOSS3 ��Ϣ
		MT_WORLD_BOSS_3_RESULT_SC = 3212,				// ����BOSS3 ���������������Ϣ
		MT_WORLD_BOSS_3_POS_INFO_SC = 3213,				// ����BOSS3 λ����Ϣ
		MT_WORLD_BOSS_3_BATTLE_INFO_SC = 3214,			// ����BOSS3 ÿ��ս������һЩ������Ϣ
		MT_WORLD_BOSS_3_ROLE_INFO_SC = 3215,			// ����BOSS3 �����Ϣ
		MT_WORLD_BOSS_3_RANK_INFO_SC = 3216,			// ����BOSS3 ���а���Ϣ
	
		//////////////////////////////// ƥ�� ///////////////////////////
		MT_SCENE_MATCH_START_SC = 3230,
		MT_SCENE_MATCH_INFO_UPDATE_SC = 3231,
		MT_SCENE_MATCH_STOP_SC = 3232,
		MT_SCENE_MATCH_MATCH_ENEMY_SC = 3233,

		MT_TASK_CHOSED_REQ_CS = 3234,
		MT_TASK_CHOSED_INFO_SC = 3235,

		MT_TASK_CHAIN_TURN_TABLE_INFO_SC = 3236,			// ������ת����Ϣ�·�

		/////////////////////////////// ����Ⱥ��Э�� //////////////////////////////////
		MT_CHAT_GROUP_START_SC = 3240,
		MT_CHAT_GROUP_ROLE_GROUP_INFO_SC = 3241,			// ����Ⱥ���������Ϣ
		MT_CHAT_GROUP_ALL_INFO_SC = 3242,					// ����Ⱥ�����ϸ��Ϣ
		MT_CHAT_GROUP_ROLE_INVITE_ENTER_SC = 3243,			// ��������Ⱥ����Ϣ
		MT_CHAT_GROUP_ONE_GROUP_INFO_SC = 3244,				// ����Ⱥ��������Ϣ

		MT_CHAT_GROUP_START_CS = 3260,
		MT_CHAT_GROUP_ROLE_INVITE_ENTER_CS = 3261,			// �����������Ⱥ��
		MT_CHAT_GROUP_ROLE_ENTER_GROUP_CS = 3262,			// ������������Ⱥ�� (�ܾ�ֱ�Ӻ�����Ϣ
		MT_CHAT_GROUP_CREATE_GROUP_CS = 3263,				// ����Ⱥ��
		MT_CHAT_GROUP_KICK_MEMBER_CS = 3264,				// �Ƴ���ҳ�Ⱥ��
		MT_CHAT_GROUP_DISSMISS_CS = 3265,					// ��ɢȺ��
		MT_CHAT_GROUP_LEAVE_CS = 3266,						// �뿪Ⱥ��
		MT_CHAT_GROUP_GET_GROUP_LIST_CS = 3267,				// ��ȡȺ���б�
		MT_CHAT_GROUP_GET_GROUP_INFO_CS = 3268,				// ��ȡȺ����ϸ��Ϣ���Լ���Ա��Ϣ
		MT_CHAT_GROUP_CHANGE_NOTICE_CS = 3269,				// �޸�Ⱥ������

		MT_TEMP_CHAT_GROUP_SIMPLE_INFO_SC = 3270,			// ��ʱȺ����Ϣ
		MT_TEMP_CHAT_GROUP_LEAVE_SC = 3271,					// �뿪��ʱȺ��
		MT_CHAT_GROUP_MSG_NOTICE_SC = 3272,					// Ⱥ��֪ͨ��Ϣ��ʾ
		MT_TEMP_CHAT_GROUP_BECOME_OFFICIAL_CS = 3273,		// ��ʱȺ��ת��
		MT_TEMP_CHAT_GROUP_BECOME_OFFICIAL_NOTICE_SC = 3274,// ��ʱȺ��ת���ɹ�֪ͨ
		MT_CHAT_GROUP_HANDOVER_TO_OTHERS_CS = 3275,			// Ⱥ���ƽ�Ⱥ��
		
		//-----------------------�ý�ս��----------------------------
		MT_HUAN_JIE_ZHAN_CHANG_REQ_CS = 3290,				//�ý�ս������
		MT_HUAN_JIE_ZHAN_CHANG_TEAM_INFO_SC = 3291,				//�ý�ս��������Ϣ
		MT_HUAN_JIE_ZHAN_CHANG_ERR_RETURN_SC = 3292,				
		MT_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_SC = 3301,		//�ý�ս��ƥ��״̬
		MT_HUAN_JIE_ZHAN_CHANG_RESULT_SC = 3302,			//�ý�ս��ս�����
		MT_HUAN_JIE_ZHAN_CHANG_OPPONENT_INFO_SC = 3303,		//�ý�ս��������Ϣ
		MT_HUAN_JIE_ZHAN_CHANG_ROLE_INFO_SC = 3304,		//�ý�ս����Ϣ
		MT_HUAN_JIE_ZHAN_CHANG_RANK_LIST_INFO_SC = 3305,		//�ý�ս�����а�
		MT_HUAN_JIE_ZHAN_CHANG_ROLE_RANK_LIST_REQ_CS = 3306,		//
		MT_HUAN_JIE_ZHAN_CHANG_ROLE_RANK_LIST_SC = 3307,		//
		MT_HUAN_JIE_ZHAN_CHANG_TIAN_TI_RECORD_SC = 3308,		//
		MT_HUAN_JIE_ZHAN_CHANG_PROF_WIN_LOSE_INFO_SC = 3309,		//
			
		//////////////////////////////////�ý�ս�� - ����ģ��///////////////////////////////////////////
		MT_HUASHEN_ROLE_INFO_SC = 3310,						// ���������Ϣ�·�
		MT_HUASHEN_ROLE_ATTR_LIST_SC = 3311,				// ���������б��·�
		MT_HUASHEN_MEDAL_REQ_CS = 3312,						// ����ѫ������
		MT_HUASHEN_MEDAL_INFO_SC = 3313,					// ����ѫ�´�����Ϣ�·�
		MT_HUASHEN_EQUIPMENT_OPERA_REQ_CS = 3314,			// ����װ����������
		MT_HUASHEN_EQUIPMENT_INFO_SC = 3315,				// ����װ����Ϣ�·�
		MT_HUASHEN_EQUIPMENT_GRID_CHANGE_SC = 3316,			// ����װ���ı�֪ͨ
		MT_HUASHEN_JEWELRY_HANDLE_REQ_CS = 3317,			// �������β�������
		MT_HUASHEN_JEWELRY_ALL_FORM_SC = 3318,				// ��������װ����Ϣ�·�
		MT_HUASHEN_JEWELRY_INFO_CHANGE_SC = 3319,			// �������θı�֪ͨ
		MT_HUASHEN_ATTR_PLAN_POINT_CS = 3320,				// ����Ǳ�ܲ�������
		MT_HUASHEN_ROLE_ATTR_PLAN_SC = 3321,				// ����ӵ���Ϣ�·�
		MT_HUASHEN_PET_REQ_CS = 3322,						// ��������������
		MT_HUASHEN_PET_ASSIGN_ATTR_POINT_CS = 3323,			// �����������������Ե�
		MT_HUASHEN_SEND_PET_ALL_INFO_SC = 3324,				// �������г�����Ϣ�·�
		MT_HUASHEN_PET_SINGLE_INFO_SC = 3325,				// ������������Ϣ�·�
		MT_HAUSHEN_PET_OTHER_INFO_SC = 3326,				// �������������Ϣ�·�
		MT_HUASHEN_PET_ATTRLIST_INFO_SC = 3327,				// ���������������б��·�
		MT_HUASHEN_INFO_REQ_CS = 3328,						// ����ģ����Ϣ�����·�
		MT_HUASHEN_KNAPSACK_ALL_INFO_SC = 3329,				// ������������Ϣ�·�
		MT_HUASHEN_KNAPSACK_SINGLE_ITEM_CHANEGE_SC = 3330,	// ��������Ʒ�ı��·�
		MT_HUASHEN_SET_BATTLE_POSITION_CS = 3331,			// ����������������վλ
		MT_HUASHEN_BATTLE_POSITION_SC = 3332,				// ����վλ��Ϣ�·�
		MT_HUASHEN_SEASON_INFO_SC = 3333,					// ����������Ϣ�·�
		MT_HUASHEN_CHANGE_CRYSTAL_NOTICE_SC = 3334,			// ����Ԫ��ˮ����Ϣ����·�
		MT_HUASHEN_ATTR_PLAN_LIST_INFO_SC = 3335,			// �������ӵ���Ϣ�·�
		MT_HUASHEN_ATTR_PLAN_CHANGE_NAME_CS = 3336,			// ��������������
		MT_HUASHEN_MEDAL_TAKE_OFF_SC = 3337,				// ����ѫ�����º��ѫ��λ���·�

		//////////////////////////////////����(����)�̵�///////////////////////////////////
		MT_MYSTERY_STORE_OP_CS = 3350,						// ����(����)�̵��������
		MT_MYSTERY_STORE_INFO_SC = 3351,					// ����(����)�̵���Ϣ

		/////////////////////////////////��������////////////////////////////////////////
		MT_GUILD_INBORN_REQ_CS = 3360,						// ����������������
		MT_GUILD_INBORN_ALL_INFO_SC = 3361,					// ��������������Ϣ�·�
		MT_GUILD_INBORN_SINGLE_INFO_SC = 3362,				// ��������������Ϣ�·�
		MT_GUILD_INBORN_SINGLE_TYPE_INFO_SC = 3363,			// ��������������Ϣ�·�

		//////////////////////////////////ɽ������///////////////////////////////////
		MT_SHAN_HAI_BOSS_OP_CS = 3370,						// ɽ��������������
		MT_SHAN_HAI_BOSS_NPC_INFO_SC = 3371,				// ɽ������NPC��Ϣ
		MT_SHAN_HAI_BOSS_ROLE_INFO_SC = 3372,				// ɽ�����������Ϣ
		//MT_SHAN_HAI_BOSS_POKER_INFO_SC = 3373,				// ɽ������������Ϣ
		MT_SHAN_HAI_BOSS_SINGLE_INFO_SC = 3374,				// ɽ������������Ϣ

		MT_RAND_ACTIVITY_CAI_SHEN_JU_BAO_INFO_SC = 3380,				// ����۱���Ϣ
		MT_RAND_ACTIVITY_DING_ZHI_BAO_XIANG_INFO_SC = 3390,				// ���Ʊ���
		MT_RAND_ACTIVITY_ZHUAN_SHU_LI_BAO_INFO_SC = 3400,				// ר�����
		MT_RAND_ACTIVITY_ZHUAN_SHU_LI_BAO_DAY_INFO_SC = 3401,			// ר�����-ÿ�������Ϣ�·�
		MT_RAND_ACTIVITY_JING_YAN_BIAO_SHENG_INFO_SC = 3410,			// ���������Ϣ
		MT_RAND_ACTIVITY_XIAN_SHI_BAO_HE_SC = 3420,						// ��ʱ����
		MT_RAND_ACTIVITY_XIAN_SHI_BAO_DAI_SC = 3430,					// ��ʱ����
		MT_RAND_ACTIVITY_SHAN_HAI_BAO_XIANG_INFO_SC = 3431,				// ɽ������ ��Ϣ
		MT_RAND_ACTIVITY_SHAN_HAI_BAO_HE_INFO_SC = 3432,				// ɽ������ ��Ϣ
		MT_RAND_ACTIVITY_SHAN_HAI_BAO_DAI_INFO_SC = 3433,				// ɽ������ ��Ϣ

		MT_WEN_XIANG_LOU_FB_REQ_CS = 3474,					// ����¥������������
		MT_WEN_XIANG_LOU_NPC_INFO_SC = 3475,				// ����¥������������
		MT_WEN_XIANG_LOU_SCENE_INFO_SC = 3476,				// ����¥������������

		MT_RAND_ACTIVITY_DING_ZHI_BAO_XIANG_2_INFO_SC = 3477,				// ���Ʊ���2
		MT_RAND_ACTIVITY_XIAN_SHI_BAO_HE_2_SC = 3478,				// ��ʱ����2
		MT_RAND_ACTIVITY_XIAN_SHI_BAO_DAI_2_SC = 3479,				// ��ʱ����2
	
		//////////////////////////////////��������////////////////////////////////////
		MT_ANECDOTE_TASK_INFO_SC = 3490,					// ��������������Ϣ�ı�֪ͨ
		MT_ANECDOTE_TASK_INFO_LIST_SC = 3491,				// ����������Ϣ�б�
		MT_ANECDOTE_TASK_BATTLE_FAIL_SC = 3492,				// ����ս��ʧ��֪ͨ
		MT_ANECDOTE_TASK_COMPLETE_TASK_NOTICE_SC = 3493,	// ���һ������֪ͨ
		MT_ANECDOTE_TASK_OPERATE_REQ_CS = 3494,				// ������������
		MT_ANECDOTE_TASK_SPECIAL_INFO_SC = 3495,			// �����������������·�
		MT_ANECDOTE_TASK_SPECIAL_SET_CS = 3496,				// ��������������������
	
		MT_NORMAL_TASK_FINISH_ANEDOTE_GAME_CS = 3497,		// ��ͨ�����������С��Ϸ

		MT_RESEARCH_TASK_ROUND_INFO_SC = 3498,				// �����ħ�����·�

		MT_ANECDOTE_BIG_EVENT_INFO_SC = 3499,				// ���Ŵ��¼�

		//////////////////////////////////�µ�������////////////////////////////////
		MT_NEW_TASK_ACTION_REQ_CS = 3500,						// �µ�������Ϊ����
		MT_NEW_TASK_DU_CHUANG_ZEI_YING_SCENE_INFO_SC = 3501,	// ������Ӫ������Ϣ�·�
		MT_NEW_TASK_DU_CHUANG_ZEI_YING_NOTICE_SC = 3502,		// ������Ӫ֪ͨѰ·

		MT_NEW_TASK_CATCH_FOX_REQ_CS = 3503,					// ץ�ú���
		MT_NEW_TASK_GIVE_NPC_ITEM_CS = 3504,					// �µ������ύ��Ʒ��NPC����
		MT_NEW_TASK_PARTNER_SOAR_INFO_SC = 3505,				// ���������������Ϣ

		MT_NEW_TASK_ACTION_END = 3550,								// Ԥ��50��Э��

		//////////////////////////////////���ƹ�����////////////////////////////////
		MT_DRAW_REQ_CS = 3560,								// ��������
		
		MT_ROLE_DRAW_INFO_SC = 3561,						// ��ɫ����ѡ����Ϣ
		MT_DRAW_POKER_INFO_SC = 3565,						// ����������Ϣ


		////////////////////////////////�ȼ�����////////////////////////////////////////

		//�ɵĵȼ���������
		MT_ROLE_LEVEL_COMPLEMENT_SC = 3562,					//�ȼ������·�
		MT_ROLE_LEVEL_COMPLEMENT_CS = 3563,					//�ȼ���������

		//---�µȼ�����
		MT_ROLE_NEW_LEVEL_COMPLEMENT_REQ_CS = 3566,			//�µȼ���������
		MT_ROLE_NEW_LEVEL_COMPLEMENT_INFO_SC = 3567,		//�µȼ�������ɫ��Ϣ�·�
		MT_NEW_LEVEL_COMPLEMENT_SPEICAL_INFO_SC = 3568,		//�µȼ�����������Ϣ�·�

		/////////////////////////////////��Ԩ��¼//////////////////////////////////////
		MT_DREAM_NOTES_CHALLENGE_CS = 3570,					// ��Ԩ��¼����
		MT_DREAM_NOTES_CHALLENGE_ROLE_INFO_SC = 3571,		// ��Ԩ��¼��ɫ��Ϣ
		MT_DREAM_NOTES_CHALLENGE_SCENE_INFO_SC = 3572,		// ��Ԩ��¼������Ϣ
		MT_DREAM_NOTES_CHALLENGE_NOTICE_SC = 3573,			// ��Ԩ��¼֪ͨ

		/////////////////////////////////���޽���//////////////////////////////////////
		MT_SHEN_SHOU_JIANG_LIN_INFO_SC = 3580,		// ���޽�����Ϣ
		MT_SHEN_SHOU_JIANG_LIN_DRWA_RESULT_SC = 3581,		// ���޽��ٳ齱���
		MT_SHEN_SHOU_JIANG_LIN_BUY_INFO_SC = 3582,		// ���޽��ٹ�����Ϣ

		/////////////////////////////////У������//////////////////////////////////////
		MT_SCHOOL_EXERCISE_INFO_SC = 3590,					// У��������Ϣ
		MT_SCHOOL_EXERCISE_CHALLENGE_CS = 3591,				// У����ս����
		MT_SCHOOL_EXERCISE_CHALLENGE_SUCC_SC = 3592,		// У����ս�ɹ�֪ͨ
		MT_SCHOOL_EXERCISE_FIRST_OPEN_UI = 3593,			// �״δ�У��UI

		MT_WORKSHOP_EQUIP_DECOMPOSE_CS = 3600,				// װ���ֽ�

		////////////////////////////////////���յǷ�//////////////////////////////////////////
		MT_RA_CHUN_RI_DENG_FENG_INFO_SC = 3611,						// ���յǷ�

		MT_MI_JING_QI_WEN_SHOP_INFO_SC = 3612,						// �ؾ������̵�
		MT_MI_JING_QI_WEN_QUESTION_INFO_SC = 3614,					// �ؾ����Ŵ�����Ϣ
		MT_MI_JING_QI_WEN_QUESTION_REQ_CS = 3615,					// �ؾ����Żش���Ŀ
		MT_RA_FORTUNE_MAZE_BOX_NUM_SC = 3616,						// �����Թ���������
		MT_RAND_ACTIVITY_FORTUNE_MAZE_SC = 3617,					// ���� - �����Թ�
		MT_RAND_ACTIVITY_FORTUNE_MAZE_PIN_TU_MAP_SC = 3618,			// ���� - �����Թ���ͼ
		MT_MI_JING_QI_WEN_BUY_ITEM_REQ_CS = 3619,					// �ؾ����Ź������

		MT_CLOSE_WINDOS_RET_SC = 3620,								// �رմ����·�
		MT_CLOSE_WINDOS_REQ_CS = 3621,								// �رմ�������

		//////////////////////////////////�ż��ᱦ����
		MT_RELIC_LOOTING_TIME_SC = 3622,							// �ż��ᱦʱ�������·�
		MT_RELIC_LOOTING_SKILL_SC = 3623,							// ������Ϣ�·�
		MT_RELIC_LOOTING_MATH_CS = 3624,							// �ż��ᱦ����ƥ��
		MT_RELIC_LOOTING_TEAM_SC = 3625,							// �ż��ᱦ������Ϣ
		MT_RELIC_LOOTING_PLAYER_REQ_CS = 3626,						// �ż��ᱦ��Ҳ���
		MT_RELIC_LOOTING_OBJ_INFO_SC = 3627,						// �ż��ᱦobj
		MT_RELIC_LOOTING_MEMBER_POS_SC = 3628,						// �ż��ᱦ����λ��
		MT_RELIC_LOOTING_MEMBER_TS_SC = 3629,						// �ż��ᱦ���Ͷ���

		////////////////////////////////////��Ϻ���//////////////////////////////////////////
		MT_RA_ZU_HE_HE_LI_INFO_SC = 3630,						// ��Ϻ���


		//////////////////////////�ż��ᱦ����
		MT_RELIC_LOOTING_MEMBER_MARK_SC = 3631,						// ����·�
		MT_RELIC_LOOTING_FB_FINISH_SC = 3632,						// ��������·�
		MT_RELIC_LOOTING_RANK_LIST_SC = 3633,						// ���а��·�
		MT_RELIC_LOOTING_ACHIEVEMENT_SC = 3634,						// �ɾ��·�
		MT_RELIC_LOOTING_SPECIAL_EFFECT_SC = 3635,					// ���״̬�·�

		//----------------
		MT_WOLRD_STATUS_GUILD_FIGHT_TOP_INFO_SC=3650,

		MT_WOLRD_STATUS_END = 3659,

		////////////////////////////// ����ؾ� ////////////////////////////////
		MT_PET_MONOPOLY_INFO_SC = 3660,						// ����ؾ���Ϣ�·�
		MT_PET_MONOPOLY_OPERATE_CS = 3661,					// ����ؾ���������
		MT_PET_MONOPOLY_EXPECT_PET_SET_SC = 3662,			// ����ؾ�������
		MT_PET_MONOPOLY_ROLL_DICE_RESULT_SC = 3663,			// ����ؾ�Ͷ���ӽ������
		MT_PET_MONOPOLY_CUR_GRID_CHANGE_NOTICE_SC = 3664,	// ����ؾ���ǰ������ݱ��֪ͨ
		MT_PET_MONOPOLY_REWARD_RECORD_SC = 3665,			// ����ؾ��鿴����
		MT_PET_MONOPOLY_ENTER_NEXT_LEVEL_SC = 3666,			// ����ؾ�������һ��֪ͨ
		MT_PET_MONOPOLY_TURN_TABLE_SC = 3667,				// ����ؾ�ת����Ϣ�·�
		MT_PET_MONOPOLY_TURN_TABLE_RESULT_SC = 3668,		// ����ؾ�ת�̽���·�
		MT_PET_MONOPOLY_SPECIAL_ITEM = 3669,				// ����ؾ��������������Ϣ�·�
		MT_PET_MONOPOLY_SPECIAL_ITEM_USE = 3670,			// ����ؾ���Ը���滻����λ��

		////////////////////////////// �ƶ˾��� ////////////////////////////////
		MT_CLOUD_ARENA_OPERATE_CS = 3675,					// �ƶ˾�����������
		MT_CLOUD_ARENA_SELF_INFO_SC = 3676,					// �ƶ˾���������Ϣ����
		MT_CLOUD_ARENA_TARGET_INFO_SC = 3677,				// �ƶ˾���Ŀ����Ϣ����
		MT_CLOUD_ARENA_NEW_FIGHT_RECORD_NOTICE_SC = 3678,	// �ƶ˾�����ս��֪ͨ
		MT_CLOUD_ARENA_FIGHT_RECORD_LIST_SC = 3679,			// �ƶ˾���ս���б���
		MT_CLOUD_ARENA_RANK_LIST_SC = 3680,					// �ƶ˾������а񷵻�
		MT_CLOUD_ARENA_SEASON_INITIAL_SCORE_NOTICE = 3681,	// �ƶ˾���֪ͨ��ʼ����
		MT_CLOUD_ARENA_NOTICE_DAN_CHANGE = 3682,			// �ƶ˾�����λ���֪ͨ
		MT_CLOUD_ARENA_ACHIEVEMENT_INFO = 3683,				// �ƶ˾����ɾ���Ϣ����
		MT_CLOUD_ARENA_ACHIEVEMENT_UPDATE_NOTICE = 3684,	// �ƶ˾����ɾ͸ı�֪ͨ
		MT_CLOUD_ARENA_ROLE_SEASON_HIGHEST_DATA_SC = 3685,	// �ƶ˾������������������·�
		MT_CLOUD_ARENA_REAL_TIME_INFO = 3686,				// �ƶ˾���ʵʱ������������
		MT_CLOUD_ARENA_REAL_TIME_INFO_REQ = 3687,			// �ƶ˾���ʵʱ������������
		MT_CLOUD_ARENA_SET_FORMATION_REQ = 3688,			// �ƶ˾�����������
		MT_CLOUD_ARENA_FORMATION_INFO_SC = 3689,			// �ƶ˾���������Ϣ�·�

		////////////////////////////// �ƶ����� ////////////////////////////////
		MT_MOVE_ASSIST_REQ_SC = 3690,						// �ƶ�Э������
		MT_MOVE_ASSIST_RET_CS = 3691,						// �ƶ�Э������

		// ��ɫ��������  ���� 3692  ~ 3694
		MT_ROLE_JOIN_ACTIVITY_REWARD_REQ_CS = 3692,			// ��ɫ�������� �ͻ�������
		MT_ROLE_JOIN_ACTIVITY_REWARD_INFO_SC = 3693,		// ��ɫ�������� ��Ϣ
		MT_ROLE_JOIN_ACTIVITY_ONE_CLICK_FINISH_INFO_SC = 3694,		// һ����ɻ

		////////////////////////////////////�������//////////////////////////////////////////
		MT_RA_LING_PAI_TIAN_CI_INFO_SC = 3810,						// �������

		////////////////////////////////////�����ķ�//////////////////////////////////////////
		MT_RA_LING_PAI_TIAN_CI_2_INFO_SC = 3811,						// �����ķ�(�������2)

		///////////////////////////////////��������//////////////////////////////////////////
		MT_RA_LING_PAI_TIAN_CI_3_INFO_SC = 3812,						// ��������

		//
		MT_RA_FASHION_SHOP_INFO_SC = 3820,						// ʱװ�̵�

		//
		MT_MI_YI_INFO_SC = 3830,						// ��ҽ��ı��Ϣ
		MT_MIYI_POKER_SC = 3831,						// ��ҽ��ı������Ϣ
		MT_MIYI_POKER_REQ_CS = 3832,					// ��ҽ��ı��������
		MT_MIYI_POKER_INFO_SC = 3833,					// ��ҽ��ı��Ϣ�·�
		MT_MIYI_SCENE_INFO_SC = 3834,					// ��ҽ��ı������Ϣ�·�
		MT_MIYI_KILL_PASS_INFO_SC = 3835,				// ֪ͨ�ͻ��˴˴�ͨ���ǲ�����ͨ��Ϣ
		MT_MIYI_REQ_CS = 3836,				// ��ҽ����
	
		//����
		MT_ZHU_LING_REQ_CS = 3850,						
		MT_ZHU_LING_CHANG_INFO_SC = 3851,						
		MT_ZHU_LING_DECOMPOSE_INFO_SC = 3852,					
		MT_ZHU_LING_DECOMPOSE_REQ_CS = 3853,
		MT_ZHU_LING_UP_LEVEL_REQ_CS = 3854,
		MT_ZHU_LING_PUT_ON_SUCC_NOTICE_SC = 3855,

		MT_SET_AUTO_ADD_POINT_FLAG_CS = 3880,  //�����Զ��ӵ�
		
		//////////////////////////////////////////////////////////////////////////////
		MT_RA_ONE_YUAN_GOU_INFO_SC = 3890,						// һԪ��

		///////////////////////////////// ������� //////////////////////////////////////
		MT_LING_YU_JI_JIN_INFO_SC = 3900,		// ���������Ϣ
		
		
		// ���ɻ�Ծ�����������   ����  3910 ~ 3916 
		MT_GUILD_ACTIVE_PERSON_RANK_CS = 3910,					//  ��ȡ ���ɻ�Ծ�� ����������Ϣ 
		MT_GUILD_ACTIVE_INFO_SC = 3911,							//  ���ɻ�Ծ����Ϣ[���ɻ�Ծֵ]
		MT_GUILD_ACTIVE_INFO_MEMBER_SC = 3912,					//  ���ɻ�Ծ����Ϣ[������ȡ��������Ϣ]
		MT_GUILD_ACTIVE_PERSON_RANK_SC= 3913,					//  ���ɻ�Ծ�� ����������Ϣ 
		MT_GUILD_ACTIVE_GET_GIFT_CS = 3914,						//  ���ɻ�Ծ��  ��ȡ���


		///////////////////////////////// �Զ���ָ�� /////////////////////////////////////
		MT_CUSTOM_INSTRUCTION_INFO_SC = 3920,					//  �Զ���ָ���·�
		MT_CUSTOM_INSTRUCTION_ADD_CS = 3921,					//  �����Զ���ָ��
		MT_CUSTOM_INSTRUCTION_REMOVE_CS = 3922,					//  ȥ���Զ���ָ��
		MT_CUSTOM_INSTRUCTION_BATCH_REMOVE_CS = 3923,			//  ��������Զ���ָ��

		///////////////////////////////// �ܳ� //////////////////////////////////////
		MT_WEEK_ACTIVE_DEGREE_INFO_SC = 3925,	//�ܳ� - �����Ϣ֪ͨ

		///////////////////////////////// ��ӡ֮�� //////////////////////////////////////
		MT_PETY_GOD_FB_REQ_CS = 3930,				// ��ӡ֮�� �û�����

		MT_PETY_GOD_FB_ROLE_INFO_SC = 3931,			// ��ӡ֮�� ��ɫ��Ϣ
		MT_PETY_GOD_FB_SCENE_INFO_SC = 3932,		// ��ӡ֮�� ����������Ϣ

		///////////////////////////////// �ɽ� //////////////////////////////////////
		MT_DU_JIE_REQ_CS = 3935,					// �ɽ� �û�����

		MT_DU_JIE_ROLE_INFO = 3936,					// �ɽ� ��ɫ��Ϣ
		MT_DU_JIE_FB_INFO = 3937,					// �ɽ� ������Ϣ

		//////////////////////////////////�ٹ�ҹ��///////////////////////////////////
		MT_HUNDRED_GHOST_INFO_SC = 3941,					// �ٹ�ҹ��NPC��Ϣ
		MT_HUNDRED_GHOST_POKER_INFO_SC = 3942,				// �ٹ�ҹ�з�����Ϣ

		//////////////////////////////��������//////////////
		MT_BIG_DIPPER_REQ_CS = 3950,			//����
		MT_BIG_DIPPER_INFO_SC = 3951,			//�·�
		MT_BIG_DIPPER_MY_INFO_SC = 3952,		//�ҵ�ͨ����Ϣ
		
		//////////////////////////////����֮��
		MT_FALLEN_GOD_INFO_SC = 3953,			//����֮����Ϣ
		MT_FALLEN_GOD_REQ_CS = 3954,			//����֮������

		///////////////////////////////// ��Դ���� //////////////////////////////////////
		MT_FETCH_RESOURCE_DOWNLOAD_REWARD_CS = 3955,			// ��ȡ��Դ���ؽ���
		MT_FETCH_RESOURCE_DOWNLOAD_REWARD_SC = 3956,			// ��Դ���ؽ�����Ǳ��

		///////////////////////////////// ��թƭ���� //////////////////////////////////////
		MT_FANG_FRADU_REWARD_REQ_CS = 3957,			// ��թƭ�����ش�
		MT_FANG_FRADU_REWARD_INFO_SC = 3958,			// ��թƭ������Ǳ��

		//////////////////////////////// ���Ե� //////////////////////////////////////////
		MT_ROLE_ATTR_PELLET_REQ_CS = 3960,			// ���Ե���������
		MT_ROLE_ATTR_PELLET_ALL_INFO_SC = 3961,		// ���Ե�������Ϣ�·�
		MT_ROLE_ATTR_PELLET_SINGLE_INFO_SC = 3962,	// ���Ե�������Ϣ�·�

		///////////////////////////////// ��Ծת�� //////////////////////////////////////
		MT_ACTIVE_TURNTABLE_REQ_CS = 3965,			// ��Ծת������
		MT_ACTIVE_TURNTABLE_INFO_SC = 3966,			// ��Ծת��������Ϣ
		MT_ACTIVE_TURNTABLE_REWARD_SC = 3967,		// ��Ծת�̳齱����

		///////////////////////////////// �������� //////////////////////////////////////
		MT_PINGJIA_REWARD_REQ_CS = 3968,			// ������������
		MT_PINGJIA_REWARD_INFO_SC = 3969,			// ����������Ϣ
		//
		MT_CAPACITY_TARGET_REQ_CS = 3975,		// ϵͳ��ս����
		MT_CAPACITY_TARGET_INFO_SC = 3976,		// ϵͳ��ս��Ϣ

		///////////////////////////////// ��� //////////////////////////////////////
		MT_HONG_BAO_CLIENT_REQ_CS = 4000,			// �ͻ�������
		MT_HONG_BAO_SEND_LIST_SC = 4001,			// ��� - �ɷ����б���Ϣ
		MT_HONG_BAO_GET_LIST_SC = 4002,				// ��� - ����ȡ�б���Ϣ
		MT_HONG_BAO_BASE_INFO_SC = 4003,			// ��� - ��ϸ��Ϣ
		MT_HONG_BAO_SEND_LIST_CHANGE_SC = 4004,		// ��� - �ɷ����б���Ϣ���
		MT_HONG_BAO_GET_LIST_CHANGE_SC = 4005,		// ��� - ����ȡ�б���Ϣ���
		MT_HONG_BAO_BASE_INFO_CHANGE_SC = 4006,		// ��� - ������Ϣ������� [�߻�Ҫ��������ȡ��Ա����Ҫ��ʵˢ��] 

		///////////////////////////////// ���--Ԫ����Լ //////////////////////////////////////
		MT_PARTNER_PACT_CHANGE_LIST_SC = 4010,		// ���--Ԫ����Լ �ֿ��б���
		MT_PARTNER_PACT_BASE_ATTR_SC = 4011,		// ���--Ԫ����Լ ��Լ�� ����֪ͨ
		MT_PARTNER_PACT_PACTINFO_SC = 4012,			// ���--Ԫ����Լ ��Լ�ı�

		//////////////////////////////// ���� 4015 ~ 4018  ///////////////////////////////////////
		MT_TIAN_MING_INFO_SC = 4015,				// ���� ��Ϣ

		///////////////////////////////// ��ص��� //////////////////////////////////////
		MT_TIAN_DI_DAO_HEN_REQ_CS = 4040,		//
		MT_TIAN_DI_DAO_HEN_ROLE_INFO_SC = 4041,		//
		MT_TIAN_DI_DAO_HEN_OPEN_INFO_SC = 4042,		//


		MT_GIVE_GIFT_REQ_CS = 4050,
		MT_GIVE_GIFT_ROLE_INFO_SC = 4051,
		MT_GIVE_GIFT_INFO_SC = 4052,

		MT_RAND_ACTIVITY_ZHEN_PIN_SHANG_PU_INFO_SC = 4080,			// ��Ʒ������Ϣ
		MT_RAND_ACTIVITY_ZHEN_PIN_SHANG_PU_OPEN_DAY_SC = 4081,			// ��Ʒ������Ϣ

		///////////////////////////////// ��ţ��ϲ [4090 ~ 4093] //////////////////////////////////////
		MT_COW_REPORT_GOOD_NEWS_CLIENT_REQ_CS = 4090,				// �ͻ��˲�������
		MT_COW_REPORT_GOOD_NEWS_REWARD_LIST_INFO_SC = 4091,			// �н���Ա����
		MT_COW_REPORT_GOOD_NEWS_INFO_SC = 4092,						// ��ţ��ϲ��ϵͳ��Ϣ
		MT_COW_REPORT_GOOD_NEWS_ROLE_BUY_INFO = 4093,				// ��ҵĹ�����Ϣ

		///////////////////////////////// �����ɸ�(��ţ��ϲ����) [4094 ~ 4097] ////////////////////////
		MT_TIGER_BRINGS_BLESSING_CLIENT_REQ_CS = 4094,				// �ͻ��˲�������
		MT_TIGER_BRINGS_BLESSING_REWARD_LIST_INFO_SC = 4095,		// �н���Ա����
		MT_TIGER_BRINGS_BLESSING_INFO_SC = 4096,					// �����ɸ���ϵͳ��Ϣ
		MT_TIGER_BRINGS_BLESSING_ROLE_BUY_INFO = 4097,				// ��ҵĹ�����Ϣ


		///////////////////////////////// ���� //////////////////////////////////////
		MT_DUEL_BATTLE_INFO_SC = 4100,				// ����ս����Ϣ
		MT_DUEL_KILL_VALUE_AOI_NOTICE_SC = 4101,	// ɱ¾ֵ�ƺ�֪ͨ�ܱ����

		///////////////////////////////// ������ //////////////////////////////////////
		MT_YIN_YANG_EYE_OP_REQ_CS = 4110,			// �����۲�������
		MT_YIN_YANG_EYE_INFO_SC = 4111,				// ��������Ϣ

		////////////////////////////////  �޾�֮��////////////////////////////////////
		MT_ENDLESS_TOWER_INFO_SC = 4120,				// �޾�֮���·�
		MT_ENDLESS_TOWER_REQ_CS = 4121,					// �޾�֮������

		/////////////////////////////  ����
		MT_FLY_UP_SYSTEM_INFO_SC = 4130,				// �����·�
		MT_FLY_UP_SYSTEM_REQ_CS = 4131,				// ��������
		

		///////////////////////////		�����׾�
		MT_MIRAGE_TERRITORY_REQ_CS = 4132,			// ����
		MT_MIRAGE_TERRITORY_INFO_SC = 4133,			// �·�
		MT_MIRAGE_TERRITORY_SET_PARTNER_REQ_CS = 4134,//������
		MT_MIRAGE_TERRITORY_PARTNER_INFO_SC = 4135,//�������·�

		MT_FLY_UP_SYSTEM_ADV_INFO_SC = 4136,		// ����ǿ���·�
		MT_FLY_UP_SKILL_SIGLE_INFO_SC = 4137,		// ����������Ϣ�·�

		///////////////////////////////// С��Ϸ ////////////////////////////////////
		MT_MINI_GAME_INFO_SC = 4145,				// С��Ϸ��Ϣ�·�
		MT_MINI_GAME_FINISH_STAGE_CS = 4146,		// С��Ϸ�������
		MT_MINI_GAME_CLAIM_PHASE_REWARD_CS = 4147,	// С��Ϸ��ȡ�׶ν�������
		MT_MINI_GAME_START_BATTLE = 4148,			// С��Ϸ����ս��
		MT_MINI_GAME_SET_MINI_GAME_ID = 4149,		// �ɿͻ��˸�֪С��Ϸid

		///////////////////////////////// ���� //////////////////////////////////////
		MT_ACCESSORIES_REQ_CS = 4150,				// ��������
		MT_ACCESSORIES_INFO_SC = 4151,				// ������Ϣ�·�
		MT_ACCESSORIES_SIGNLE_SC = 4152,			// ���ε�����Ϣ�仯
		MT_ACCESSORIES_VOC_COMPOSE_CS = 4153,		// ���μ̳д���
		MT_ACCESSORIES_SPECAIL_EQUIP_SC = 4154,		// ����-�����·�

		///////////////////////////////// ְҵ���� - �ӽ� 4155 - 4158 ///////////////////////////
		MT_ADVANCE_CAREER_SUB_INFO_SC = 4156,		// ְҵ�����ӽ���Ϣ


		//�ؾ�����2
		MT_RA_MI_JING_QI_WEN_2_OBJ_INFO_SC = 4170,						// �ؾ�����2�̵�
		MT_MI_JING_QI_WEN_2_SHOP_INFO_SC = 4171,						// �ؾ�����2�̵�
		MT_MI_JING_QI_WEN_2_QUESTION_INFO_SC = 4172,					// �ؾ�����2������Ϣ
		MT_MI_JING_QI_WEN_2_QUESTION_REQ_CS = 4173,						// �ؾ�����2�ش���Ŀ
		MT_MI_JING_QI_WEN_2_BUY_ITEM_REQ_CS = 4174,						// �ؾ�����2�������


		//////���� ɽ���۱�
		MT_SHAN_HAI_JU_BAO_ROLE_INFO_SC = 4177,			//��ɫ��Ϣ
		MT_SHAN_HAI_JU_BAO_DRAW_RET_SC = 4178,			//�齱����


		///////////////////////////////// �������ܶ� [4180 ~ 4183 ] //////////////////////////////////////
		MT_FRIEND_INTIMACY_LIST_INFO_SC = 4180,							// �������ܶ���Ϣ�б�
		MT_FRIEND_INTIMACY_INFO_CHANGE_SC = 4181,						// �������ܶ���Ϣ���

		//////���� ɽ������
		MT_SHAN_HAI_MET_ROLE_INFO_SC = 4185,			//��ɫ��Ϣ
		MT_SHAN_HAI_MET_TASK_PARAM_SC = 4186,			//������Ϣ
		MT_SHAN_HAI_MET_TASK_CHANGE_PARAM_SC = 4187,	//����ı�

		//////���� ��ս����
		MT_ZAI_ZHAN_YUN_ZE_INFO_SC = 4190,			//��ɫ��Ϣ

		//////���� ����ӱ�
		MT_COLORFUL_DOUBLE_ROLE_INFO_SC = 4195,			//��ɫ��Ϣ
		MT_COLORFUL_DOUBLE_TASK_PARAM_SC = 4196,		//������Ϣ
		MT_COLORFUL_DOUBLE_TASK_CHANGE_PARAM_SC = 4197,	//����ı�

		//����
		MT_JIE_YI_REQ_CS = 4200,			
		MT_JIE_YI_CREATE_CS = 4201,			
		MT_JIE_YI_CREATE_NAME_CS = 4202,
		MT_JIE_YI_CREATE_INFO_SC = 4203,
		MT_JIE_YI_CREATE_NAME_UPDATE_SC = 4204,
		MT_JIE_YI_CREATE_CONFIRM_SC = 4205,
		MT_JIE_YI_CREATE_CANCEL_SC = 4206,
		MT_JIE_YI_ZU_INFO_SC = 4207,
		MT_JIE_YI_ZU_MEMBER_NAME_CS = 4208,
		MT_JIE_YI_ZU_TEXT_CS = 4209,
		MT_JIE_YI_ZU_NAME_CS = 4210,
		MT_JIE_YI_ZU_ROUTE_SC = 4211,
		MT_JIE_YI_ZU_ROLE_XUAN_YAN_LIST_SC = 4212,
		MT_JIE_YI_ZU_XUAN_YAN_LIST_SC = 4213,
		MT_JIE_YI_ZU_XUAN_YAN_REQ_CS = 4214,
		MT_JIE_YI_ZU_XUAN_YAN_REQ_LIST_CS = 4215,
		MT_JIE_YI_ZU_KICK_NOTICE_SC = 4216,
		MT_JIE_YI_ZU_ROLE_ONLINE_STATUS_SC = 4217,

		//////���黰��
		MT_WAN_LING_FB_REQ_CS = 4230,			//�û�����
		MT_WAN_LING_FB_ROLE_INFO_SC = 4231,		//��ɫ��Ϣ
		MT_WAN_LING_FB_INFO_SC = 4232,			//������Ϣ

		//////��Ե-����
		MT_WEDDING_FB_REQ_CS = 4240,			//��Ҳ���
		MT_WEDDING_FB_SCENE_INFO_SC = 4241,		//������Ϣ
		MT_WEDDING_FB_ROLE_SC = 4242,			//������ɫ��Ϣ
		MT_WEDDING_FB_SYNC_SC = 4243,			//������Ϣͬ��
		MT_WEDDING_SHOW_INFO_SC = 4244,			//����--չʾ��Ϣ(���뺯)

		MT_WEDDING_REGIST_LIST_SC = 4245,		//����--ԤԼ�б�
		MT_WEDDING_INVITE_LIST_SC = 4246,		//����--�����б�
		MT_WEDDING_APPLY_LIST_SC = 4247,		//����--�����б�
		MT_WEDDING_CHANGE_SC = 4248,			//����--֪ͨ

		MT_WEDDING_BASE_INFO_SC = 4249,			//����--������Ϣ
		MT_WEDDING_STATUS_SYNC_SC = 4250,		//����--�����㲥

		MT_WEDDING_FB_SUB_ACT_XUN_QING_MI_AI_SC = 4251,	//����--С��Ϸ Ѱ���ٰ� ��ɫ��Ϣ
		MT_WEDDING_FB_SUB_ACT_XIN_XIN_XIANG_YIN_SC = 4252,	//����--С��Ϸ ������ӡ ��ɫ��Ϣ
		MT_WEDDING_FB_SUB_ACT_BAO_WEI_XIU_QIU_SC = 4253,	//����--С��Ϸ �������� ��ɫ��Ϣ
		MT_WEDDING_FB_SUB_ACT_ZAO_SHENG_GUI_ZI_SC = 4254,	//����--С��Ϸ �������� ��ɫ��Ϣ



		// ��Ե
		MT_QING_YUAN_REQ_CS = 4260,			//�û�����
		MT_QING_YUAN_INVITE_ROUTE_SC = 4261,
		MT_QING_YUAN_INFO_SC = 4262,
		MT_QING_YUAN_LEAVE_ROUTE_SC = 4263,
		MT_QING_YUAN_INVITE_RET_SC = 4264,
		MT_QING_YUAN_LEAVE_NOTICE_SC = 4265,
		MT_QING_YUAN_CREATE_NOTICE_SC = 4266,

		//�������
		MT_PET_FEI_SHENG_TASK_INFO_SC = 4300,

		MT_PET_FEI_SHENG_END = 4330,

		// ��ʱ�۳� 4331 ~4333
		MT_XIAN_SHI_LEI_CHONG_INFO_SC = 4331,			// ��ʱ�۳���Ϣ

		//��ĩ���
		MT_WEEKEND_GIFT_INFO = 4335,			//��ɫ��Ϣ

		//��ֵ����
		MT_CHAO_ZHI_GIFT_INFO = 4340,			//��ɫ��Ϣ

		// ��Ϊ 4345 ~4348
		MT_XIU_WEI_REQ_CS = 4345,					// ��Ϊ �ͻ�������
		MT_XIU_WEI_INFO_SC = 4346,					// ��Ϊ��Ϣ
		MT_XIU_WEI_INFO_CHANGE_SC = 4347,			// ��Ϊ��Ϣ���
		MT_XIU_WEI_DAN_YAO_LIMIT_INFO_SC = 4348,	// ��Ϊ��ҩ��Ϣ

		//�����ر�
		MT_TREASURE_TASK_REQ_CS = 4349,			// �����ر�����
		MT_TREASURE_TASK_INFO_SC = 4350,		// �����ر���ɫ��Ϣ	

		// �۽�����
		MT_WORLD_ARENA_INFO_SC = 4400,					//!< ������Ϣ
		MT_WORLD_ARENA_SIGNUP_INFO_SC = 4401,			//!< ���Ǳ�����Ϣ
		MT_WORLD_ARENA_OPERATE_CS = 4402,				//!< ��Ҳ���
		MT_WORLD_ARENA_SCHEDULE_INFO_SC = 4403,			//!< ������Ϣ
		MT_WORLD_ARENA_USER_BASE_INFO_SC = 4404,		//!< ��һ�����Ϣ
		MT_WORLD_ARENA_GROUP_RANK_LIST_SC = 4405,		//!< �������а��·�
		MT_WORLD_ARENA_COUNT_DOWN_SC_TO_FIGHT = 4406,	//!< ����ʱ��ʼս�� 
		MT_WORLD_ARENA_START_ENTER_SCENE_NOTICE = 4407,	//!< ��ʼ����֪ͨ
		MT_WORLD_ARENA_SHOWUP_INFO = 4408,				//!< ������Ϣ�·�

			//��������¼
		MT_PET_FEI_SHENG_REWARD_INFO_SC = 4420,

		//��Եʱװ
		MT_RA_QING_YUAN_SHI_ZHUANG_INFO_SC = 4425,	// ����-��Եʱװ ��ɫ��Ϣ
		//��Ե�۳�
		MT_RA_QING_YUAN_LEI_CHONG_INFO_SC = 4426,	// ����-��Եʱװ ��ɫ��Ϣ

		MT_PET_BATCH_COMPOSE_CS = 4430,	// ���������ϳ�
		MT_PET_BATCH_COMPOSE_RET_SC = 4431,	// ���������ϳ�
		MT_PET_BATCH_COMPOSE_AND_DECOMPOSE_CS = 4432,
		MT_PET_DECOMPOSE_DECOMPOSE_CS = 4433,

		MT_TASK_SKIP_CS = 4434,//ͨ����������

		//Ѳ��
		MT_XUN_YOU_CAR_STATUS_INFO_SYNC = 4450,	//���� ״̬��Ϣͬ��

		//����������
		MT_PEAK_TOKEN_ROLE_REQ_CS = 4455,			//��Ҳ���


		MT_PEAK_TOKEN_ROLE_JOIN_STATUS_SC = 4457,	//��ɫ����״̬֪ͨ
		MT_PEAK_TOKEN_REWARD_INFO_SC = 4458,		//����֪ͨ
		MT_PEAK_TOKEN_ROLE_INFO_SC = 4459,			//�����Ϣ

		//����ͳ��
		MT_INCOME_ROLE_REQ_CS = 4460,			//��Ҳ���
		MT_INCOME_ROLE_INFO_SC = 4461,			//�����Ϣ

		//���ǿ��
		MT_PARTNER_INTENSIFY_CS = 4465,			//���ǿ��
		MT_PARTNER_INTENSIFY_INFO_SC = 4466,	//���ǿ���ɹ�����

		//ʦͽϵͳ �Ż� ʦͽ���� 
		MT_SHI_TU_CHAUN_GONG_FB_REQ = 4470,			//ʦͽ���� ��ɫ����

		MT_SHI_TU_CHAUN_GONG_GAME_INFO = 4471,		//ʦͽ���� ��Ϸ��Ϣ֪ͨ
		MT_SHI_TU_CHAUN_GONG_SCENE_INFO = 4472,		//ʦͽ���� ������Ϣ
		MT_SHI_TU_SYNC_INFO = 4473,					//ʦͽ���� ������Ϣ

		// Ӣ�ۻ���
		MT_WORLD_TEAM_ARENA_REQ_CS = 4600,							// Ӣ�ۻ�������
		MT_WORLD_TEAM_ARENA_ALL_SIGN_UP_TEAM_SC = 4601,				// ���б���ս����Ϣ�·�
		MT_WORLD_TEAM_ARENA_SIGN_UP_REQ_CS = 4602,					// Ӣ�ۻ��䱨������
		MT_WORLD_TEAM_ARENA_PRE_TEAM_SIGN_UP_INFO_SC = 4603,		// Ӣ�ۻ���Ԥ��������Ϣ�·�
		MT_WORLD_TEAM_ARENA_NOTICE_INFO_SC = 4604,					// Ӣ�ۻ���֪ͨ��Ϣ�·�
		MT_WORLD_TEAM_ARENA_TEAM_SIGN_UP_DETAILED_INFO_SC = 4605,	// Ӣ�ۻ���ս����ϸ��Ϣ�·�
		MT_WORLD_TEAM_ARENA_ACTIVITY_INFO_SC = 4606,				// Ӣ�ۻ�����Ϣ�·�
		MT_WORLD_TEAM_ARENA_SCENE_OP_REQ_CS = 4607,					// Ӣ�ۻ��䳡����������
		MT_WORLD_TEAM_ARENA_ALL_RANK_INFO_SC = 4608,				// Ӣ�ۻ������а���Ϣ
		MT_WORLD_TEAM_ARENA_MY_RANK_INFO_SC = 4609,					// Ӣ�ۻ�������ս�����а���Ϣ
		MT_WORLD_TEAM_ARENA_SPECIAL_INFO_SC = 4610,					// Ӣ�ۻ���������Ϣ�·�
		MT_WORLD_TEAM_ARENA_TEAM_SPECIAL_INFO_SC = 4611,			// Ӣ�ۻ������������Ϣ�·�
		MT_WORLD_TEAM_ARENA_FIGHT_INFO_SC = 4612,					// Ӣ�ۻ��䷢��ս����Ϣ�·�
		MT_WORLD_TEAM_ARENA_FINALLY_RANK_REQ_CS = 4613,				// Ӣ�ۻ������հ�����
		MT_WORLD_TEAM_ARENA_FINALLY_RANK_INFO_SC = 4614,			// Ӣ�ۻ������հ���Ϣ�·�
		MT_WORLD_TEAM_ARENA_OTHER_INFO_SC = 4615,					// Ӣ�ۻ���������Ϣ�·�

		//��Եʢ��
		MT_QING_YUAN_SHENG_HUI_REQ_CS = 4650,		//��Եʢ�� ����
		MT_QING_YUAN_SHENG_SCENE_INFO_SC = 4651,	//��Եʢ�� ������Ϣ
		MT_QING_YUAN_SHENG_ROLE_INFO_SC = 4652,		//��Եʢ�� ��ɫ��Ϣ
		MT_QING_YUAN_SHENG_HUI_QUESTION_RESULT_SC= 4653,		
		MT_QING_YUAN_SHENG_HUI_QUESTION_ROUTE_SC = 4654,		
		MT_QING_YUAN_SHENG_HUI_QUESTION_INFO_SC = 4655,
		MT_QING_YUAN_SHENG_HUI_OPPONENT_INFO_SC = 4656,				
		MT_QING_YUAN_SHENG_HUI_MATCH_OP_ROUTE_SC = 4657,
		MT_QING_YUAN_SHENG_HUI_INFO_SC = 4658,
		MT_QING_YUAN_SHENG_HUI_QI_YUAN_NOTICE_SC = 4659,

		MT_QING_YUAN_SHENG_HUI_REQ_2_CS = 4680,		//��Եʢ�� ����2
		
		///////////////////////////////ɽ������ҫ����///////////////////////////////
		//��Ե�Զ���
		MT_QINGYUAN_DUIDUIPENG_REQ_CS = 4660,						// ��Ե�Զ��� ������Ϣ
		MT_QINGYUAN_DUIDUIPENG_START_SC = 4661,						// ��Ե�Զ��� ������ʼ֪ͨ
		MT_QINGYUAN_DUIDUIPENG_ROLE_FORM_SC = 4662,					// ��Ե�Զ��� ��һû���Ϣ
		MT_QINGYUAN_DUIDUIPENG_FB_INFO_SC = 4663,					// ��Ե�Զ��� ����������Ϣ
		MT_QINGYUAN_DUIDUIPENG_FINISH_SC = 4664,					// ��Ե�Զ��� ��������֪ͨ
		MT_QINGYUAN_DUIDUIPENG_RANK_INFO_SC = 4665,					// ��Ե�Զ��� ���а���Ϣ
		MT_QINGYUAN_DUIDUIPENG_DAY_TIMES_SC = 4666,					// ��Ե�Զ��� ÿ�մ���
	
		// ɽ���ƻ�		
		MT_LANTERN_RIDDIE_REQ_CS = 4670,						//ɽ���ƻ� �ͻ�������
		MT_LANTERN_RIDDIE_NPC_INFO_SC = 4671,					//ɽ���ƻ� NPC����
		MT_LANTERN_RIDDIE_TIMEOUT_INFO_SC = 4672,				//ɽ���ƻ� ���ⳬʱʱ���
		MT_LANTERN_RIDDIE_ANSWER_RESULT_INFO_SC = 4673,			//ɽ���ƻ� ��������Ϣ
		MT_LANTERN_RIDDIE_BASE_INFO_SC = 4674,					//ɽ���ƻ� ��¼ʱ�·�
		MT_LANTERN_RIDDIE_PRAISE_INFO_SC = 4675,				//ɽ���ƻ� ���޺͵�����Ϣ�·�
		MT_LANTERN_RIDDIE_NOTIFY_INFO_SC = 4676,				//ɽ���ƻ� Ư��
		MT_LANTERN_RIDDIE_USE_CARD_INFO_SC = 4677,				//ɽ���ƻ� ʹ�õ���
		MT_LANTERN_RIDDIE_LAST_RESULT_INFO_SC = 4678,			//ɽ���ƻ� ���ս���

		// ��ԵС��
		MT_LOVERS_MARKET_REQ_CS = 4685,							// ��ԵС�� ������Ϣ
		MT_LOVERS_MARKET_INFO_SC = 4686,						// ��ԵС�� ��Ʒ��Ϣ

		// Եǣͬ�Ľ�
		MT_CONCENTRIC_KNOT_REQ_CS = 4690,						// Եǣͬ�Ľ� ������Ϣ
		MT_CONCENTRIC_KNOT_AGREE_SC = 4691,						// Եǣͬ�Ľ� ׼��֪ͨ
		MT_CONCENTRIC_KNOT_START_SC = 4692,						// Եǣͬ�Ľ� ��ʼ��Ϸ����
		MT_CONCENTRIC_KNOT_OPEN_CARD_SC = 4693,					// Եǣͬ�Ľ� ����֪ͨ
		MT_CONCENTRIC_KNOT_END_GAME_SC = 4694,					// Եǣͬ�Ľ� ��������
		ME_CONCENTRIC_KNOT_INFO_SC = 4695,						// Եǣͬ�Ľ� ��ɫ��Ϣ
		//����
		MT_ZHOU_QI_INFO_SC = 4700,					// ������Ϣ

		//����ȥ�Ķ�
		MT_CA_WHERE_IS_THE_FAIRY_OPERA_REQ_CS = 4730,		// ����ȥ�Ķ���������
		MT_CA_ROLE_WHERE_IS_THE_FAIRY_INFO_SC = 4731,		// ����ȥ�Ķ������Ϣ�·�
		MT_CA_WHERE_IS_THE_FAIRY_INFO_SC = 4732,			// ����ȥ�Ķ�������Ϣ�·�

		//�칬Ѱ��
		MT_CA_TIAN_GONG_XUN_BAO_OP_REQ_CS = 4735,			//�칬Ѱ����������
		MT_CA_TIAN_GONG_XUN_BAO_INFO_SC = 4736,				//�칬Ѱ����Ϣ�·�

		MT_WORLD_STATUS_HIDDEN_INFO_SC = 4750,			//����״̬���ط���Ϣ

		MT_TU_ZI_KUAI_PAO_REQ_CS = 4770,				//���ӿ�������
		MT_TU_ZI_KUAI_PAO_SCENE_INFO_SC = 4771,				//���ӿ��ܳ�����Ϣ
		MT_TU_ZI_KUAI_PAO_INFO_SC = 4772,				//���ӿ��ܳ�����Ϣ
		MT_TU_ZI_KUAI_PAO_REST_SC = 4773,				//���ӿ��ܳ�����Ϣ
		MT_TU_ZI_KUAI_PAO_ENCOURAGE_RESULT_SC = 4774,			//���ӿ��ܳ�����Ϣ
		MT_TU_ZI_KUAI_PAO_RUN_SC = 4775,			//
		MT_TU_ZI_KUAI_PAO_ROLE_INFO_SC = 4776,			//
		MT_TU_ZI_KUAI_PAO_FINISH_NOTICE_SC = 4777,			//
		MT_TU_ZI_KUAI_PAO_POS_INFO_SC = 4778,			//

		MT_JIN_YI_TIAN_XIANG_SC = 4790,			//����������Ϣ
		MT_YUAN_QI_YAN_HUO_INFO_SC = 4800,			//Ԫ���̻���Ϣ

		// ����-���ٳ�Ƥ��
		MT_RA_SANBAI_CHOU_INFO_SC = 4805,			// ����-���ٳ�Ƥ�� ��ɫ��Ϣ
		MT_RA_SANBAI_CHOU_DRAW_SC = 4806,			// ����-���ٳ�Ƥ�� �齱����

		// ʥ��
		MT_HOLY_EQUIP_REQ_CS = 4830,					// ʥ�� �ͻ�������
		MT_HOLY_EQUIP_LEVEL_UP_REQ_CS = 4831,			// ʥ�� ��������
		MT_HOLY_EQUIP_HOLY_BAG_ALL_INFO_SC = 4832,		// ʥ�� ʥ������ȫ����Ϣ�·�
		MT_HOLY_EQUIP_FUSHI_BAG_ALL_INFO_SC = 4833,		// ʥ�� ��ʯ����ȫ����Ϣ�·�
		MT_HOLY_EQUIP_HOLY_BAG_SIGNLE_INFO_SC = 4834,	// ʥ�� ʥ������������Ϣ�·�
		MT_HOLY_EQUIP_LEVEL_UP_SONCUME_INFO_SC = 4835,	// ʥ�� ʥ��ʥ��/��ʯ�ϳ������б��·�
		MT_HOLY_EQUIP_FIRST_GAIN_INFO_SC = 4836,		// ʥ�� ��һ�λ��ʱ����ָ��
		MT_HOLY_EQUIP_FOLLOW_INDEX_INFO_SC = 4837,		// ʥ�� ������Ϣ

		// ���µ�һ����[���] �ھ���¼�ֲ� 4962 ~ 4967
		MT_GET_UTA_CHAMPION_RECORD_CS = 4962 ,		// �ͻ��˻�ȡ �ھ���¼�ֲ�[���]
		MT_GET_UTA_CHAMPION_RECORD_ACK_SC = 4963,	// ���� �ھ���¼�ֲ�[���] ��Ϣ


		// ���µ�һ����[���] ���� 4968 ~ 4973
		MT_UTA_GUESS_CLIENT_REQ_CS = 4968,			// UTA���� �ͻ��˲���
		MT_UTA_GUESS_MATCH_INFO_SC = 4969,			// UTA���� ĳһ�������ľ�����Ϣ
		MT_UTA_GUESS_BET_RECORD_SC = 4970,			// UTA���� ��Ҳ�����ע���б�
		MT_UTA_GUESS_HISTORY_EARNING_SC = 4971,		// UTA���� �ۼ������·�
		MT_UTA_GUESS_HAS_BET_REWARD_SC = 4972,		// UTA���� �п���ȡ����֪ͨ

		////////////////////////////// ���µ�һ������ ///////////////////////////////
		MT_UTA_SC_BEGIN = 5000,
		MT_UTA_BASE_INFO_SC = 5001,
		MT_UTA_TEAM_LIST_SC = 5002,
		MT_UTA_MY_TEAM_QUALIFICATION_SCHEDULES_SC = 5003,
		MT_UTA_TEAM_DETAIL_INFO_SC = 5004,	
		MT_UTA_MY_TEAM_BASE_INFO_SC = 5005,
		MT_UTA_TEAM_SCORE_RANK_LIST_SC = 5006,
		MT_UTA_MAIN_SCHEDULES_SC = 5007,
		MT_UTA_FINAL_RANK_LIST_SC = 5008,
		MT_UTA_OVERVIEW_SCHEDULES_SC = 5009,
		MT_UTA_QUALIFICATION_SCHEDULES_SC = 5010,
		MT_UTA_SCENE_INFO_SC = 5011,
		MT_UTA_BATTLE_VS_INFO = 5012,
		MT_UTA_PREPARE_ENTER_SCENE_SC = 5013,
		MT_UTA_BATTLE_RESULT_SHOW_SC = 5014,

		MT_UTA_CS_BEGIN = 5050,
		MT_UTA_TEAM_LIST_CS = 5051,		// �����������
		MT_UTA_TEAM_DETAIL_CS = 5052,	// ����ĳ��������ϸ��Ϣ
		MT_UTA_MY_TEAM_QUALIFICATION_SCHEDULES_CS = 5053, // �����ҵĶ����ʸ�������	
		MT_UTA_FETCH_WIN_BOX_CS = 5054,	// ������ȡʤ������
		MT_UTA_TEAM_QUALIFICATION_RANK_LIST_CS = 5055,	// �����ʸ������а�
		MT_UTA_MAIN_SCHEDULES_CS = 5056,// ������������
		MT_UTA_FINAL_RANK_LIST_CS = 5057,	//�������հ�
		MT_UTA_QUALIFICATION_SCHEDULES_CS = 5058,	//�����ʸ�������
		MT_UTA_MAIN_SCHEDULES_OVERVIEW_CS = 5059,	//����������������
		MT_UTA_ENTER_SCENE_CS = 5060,	// ������볡��
		MT_UTA_WATCH_VIDEO_CS = 5061,	// �鿴ս��¼��
		MT_UTA_START_OB_CS = 5062,		// ��ʼ��ս

		//�������
		MT_HONGMENG_TIANJIE_REQ_CS = 5100,				// ������� ����
		MT_HONGMENG_TIANJIE_SCENE_INFO_SC = 5101,		// ������� ������Ϣ
		MT_HONGMENG_TIANJIE_ROLE_INFO_SC = 5102,		// ������� ������Ϣ
		MT_HONGMENG_TIANJIE_WORLD_INFO_SC = 5103,		// ������� ȫ��BOSS ��Ϣ
		MT_HONGMENG_TIANJIE_TALK_INFO_SC = 5104,		// ������� �����Ի���Ϣת��
		MT_HONGMENG_TIANJIE_REWARD_INFO_SC = 5105,		// ������� ������¼
		MT_HONGMENG_TIANJIE_PASS_TEAM_SC = 5106,		// ������� ��ͨ������Ϣ

		//�컯װ��
		MT_ZAO_HUA_EQUIPMENT_REQ_CS = 5120,				// �컯װ�� ����
		MT_ZAO_HUA_EQUIPMENT_REFINE_SC = 5121,				// �컯װ��������Ϣ
		MT_ZAO_HUA_YUAN_HUN_ZHU_REQ_CS = 5122,				// �컯װ��Ԫ��������
		MT_ZAO_HUA_YUAN_HUN_ZHU_UP_LEVEL_CS = 5123,			// �컯װ��Ԫ��������
		MT_ZAO_HUA_LONG_ZHU_INFO_SC = 5124,			// �컯װ��������Ϣ
		MT_ZAO_HUA_LONG_ZHU_SLOT_INFO_SC = 5125,			// �컯װ����������Ϣ
	

		MT_DUAN_YANG_ZI_XUAN_INFO_SC = 5130,		// ������ѡ
		MT_DUAN_YANG_FU_LI_INFO_SC = 5131,			// ��������

		MT_JIN_LONG_JU_BAO_INFO_SC = 5135,		// �����۱���Ϣ
		MT_JIN_LONG_JU_BAO_DRAW_RET_SC = 5136,		// �����۱��齱����

		MT_SERVER_COMPETITION_ALL_RANK_INFO_SC = 5137,		// �·���ƴȫ�����а���Ϣ

		// �ƾٿ���
		MT_KE_JU_EXAM_REQ_CS = 5140,						//�ƾٿ��� �ͻ�������
		MT_KE_JU_EXAM_NPC_INFO_SC = 5141,					//�ƾٿ��� NPC����
		MT_KE_JU_EXAM_TIMEOUT_INFO_SC = 5142,				//�ƾٿ��� ���ⳬʱʱ���
		MT_KE_JU_EXAM_ANSWER_RESULT_INFO_SC = 5143,			//�ƾٿ��� ��������Ϣ
		MT_KE_JU_EXAM_BASE_INFO_SC = 5144,					//�ƾٿ��� ��¼ʱ�·�
		MT_KE_JU_EXAM_PRAISE_INFO_SC = 5145,				//�ƾٿ��� ���޺͵�����Ϣ�·�
		MT_KE_JU_EXAM_NOTIFY_INFO_SC = 5146,				//�ƾٿ��� Ư��
		MT_KE_JU_EXAM_USE_CARD_INFO_SC = 5147,				//�ƾٿ��� ʹ�õ���
		MT_KE_JU_EXAM_LAST_RESULT_INFO_SC = 5148,			//�ƾٿ��� ���ս���

		//��Ҫ��ѧ
		MT_GOTO_SCHOOL_REQ_CS = 5150,				// ��Ҫ��ѧ����
		MT_GOTO_SCHOOL_INFO_SC = 5151,				// ��Ҫ��ѧ������Ϣ

		MT_ZHOU_QI_SHOP_REQ_CS = 5160,				// �����̵�����
		MT_ZHOU_QI_SHOP_INFO_SC = 5161,				// �����̵���Ϣ

		
		MT_PARTNER_PET_RANK_REQ_CS = 5162,		// ���/����ǰʮ���а�����
		MT_PET_TOP_RANK_INFO_SC = 5163,			// ����ǰʮ��Ϣ�·�
		MT_PARTNER_TOP_RANK_INFO_SC = 5164,		// ���ǰʮ��Ϣ�·�

		MT_PET_STRENGTHEN_BUFF_INFO = 5170,				// ���ﹲ����Ϣ

		MT_SHEN_SHOU_EN_ZE_INFO_SC = 5180,				// ���޶�����Ϣ
		MT_SHEN_SHOU_EN_ZE_DRAW_RESULT_SC = 5181,		// ���޶���齱����
		MT_SHEN_SHOU_EN_ZE_RECORD_INFO_SC = 5182,		// ���޶���齱��¼
	
		MT_ELEMENT_ELF_REWARDS_SC = 5185,				// Ԫ�ؾ��齱��֪ͨ

		// ����
		MT_FU_YU_SINGLE_INFO_SC = 5186,			// �Ѵ�����������Ϣ�·�
		MT_FU_YU_ALL_INFO_SC = 5187,			// �Ѵ�������ȫ����Ϣ�·�
		MT_FU_YU_OPERATE_REQ_CS = 5188,			// ��������

		MT_RAND_ACTIVITY_XIAN_SHI_BAO_HE_3_SC = 5190,				// ��ʱ����3
		MT_SHI_TU_OP_RET_SC = 5191,									// ʦͽ����֪ͨ
		MT_RAND_ACTIVITY_XIAN_SHI_BAO_HE_4_SC = 5200,				// ��ʱ����4
		MT_RAND_ACTIVITY_DING_ZHI_BAO_XIANG_3_INFO_SC = 5210,		// ���Ʊ���3

		MT_RAND_ACTIVITY_XIN_FU_LING_CHONG_INFO = 5220,				// �·������Ϣ�·�
																			
		// ����-����ҫ��
		MT_RAND_ACTIVITY_TIAN_YI_YAO_SHI_INFO_SC = 5221,
		MT_RAND_ACTIVITY_HUN_QI_FAN_LI_INFO_SC = 5222,		// ����-��������	

		MT_RAND_ACTIVITY_SHEN_CI_HUA_FU_INFO_SC = 5225,				// ��ͻ�����Ϣ
		MT_RAND_ACTIVITY_SHEN_CI_HUA_FU_DRAW_RET_SC = 5226,			// ��ͻ�����Ϣ
		MT_PET_LIAN_YAO_COMMON_INFO_SC = 5230,					
		MT_PET_ADD_EXP_NOTICE_SC = 5231,			

		//���Ӣ��¼
		MT_KUA_FU_YING_XIONG_LU_REQ_CS = 5310,						// ���Ӣ��¼����
		MT_KUA_FU_YING_XIONG_LU_ACTIVITY_INFO_SC = 5311,			// ���Ӣ��¼���Ϣ�·�
		MT_KUA_FU_YING_XIONG_LU_ROLE_ALL_INFO_SC = 5312,			// ���Ӣ��¼��ɫ������Ϣ�·�
		MT_KUA_FU_YING_XIONG_LU_ROLE_SINGLE_RANK_INFO_SC = 5313,	// ���Ӣ��¼��ɫ��������Ϣ�·�
		MT_KUA_FU_YING_XIONG_LU_ROLE_SINGLE_TASK_INFO_SC = 5314,	// ���Ӣ��¼��ɫ����������Ϣ�·�
		MT_KUA_FU_YING_XIONG_LU_ROLE_RANK_VALUE_SC = 5315,			// ���Ӣ��¼��ɫ����ֵ�б���Ϣ�·�

		// ��ͼϵͳ
		MT_STAR_CHART_REQUEST_CS = 5320,					// ��ͼϵͳ����
		MT_STAR_CHART_INFO_SC = 5321,						// ��ͼϵͳ��Ϣ�·�
		MT_STAR_CHART_INSTALL_SKILL_NOTICE_SC = 5322,		// ��ͼװ������֪ͨ

		//߱��֮��
		MT_NEW_COURAGE_CHALLENGE_REQ_CS = 5330,					// ߱��֮������
		MT_NEW_COURAGE_CHALLENGE_LEVEL_LIST_INFO_SC = 5331,		// ߱��֮���ؿ��б���Ϣ
		MT_NEW_COURAGE_CHALLENGE_LEVEL_SIGNLE_INFO_SC = 5332,	// ߱��֮���ؿ�������Ϣ
		MT_NEW_COURAGE_CHALLENGE_REWARD_INFO_SC = 5333,			// ߱��֮��������Ϣ


		// ��Ϧ����
		MT_RA_YUE_XI_DENG_MI_REQ_CS = 5340,					// ����-��Ϧ���� ����
		MT_RA_YUE_XI_DENG_MI_INFO_SC = 5341,				// ����-��Ϧ���� ���Ϣ�·�
		// ��Ϧ�۳�
		MT_RA_YUE_XI_LEI_CHONG_INFO_SC = 5342,				// ����-��Ϧ�۳� ���Ϣ�·�
		// ��Ϧ���ղ���
		MT_RA_YUE_XI_DENG_MI_ANSWER_RESULT_SC = 5343,		// ����-��Ϧ���� �������·�
		
		//���� - ���ʷ���
		MT_RA_DAN_BI_FAN_LI_INFO_SC = 5345,					// ���ʷ�����Ϣ�·�
		MT_RA_DAN_BI_FAN_LI_SINGLE_INFO_SC = 5346,			// ���ʷ���������Ϣ�·�

		MT_RAND_ACTIVITY_ZHUI_YUE_SHANG_DIAN_INFO_SC = 5350,				// ׷���̵�
		MT_RAND_ACTIVITY_ZHUI_YUE_SHANG_DIAN_DRAW_RET_SC = 5351,			// ׷���̵�
		MT_RAND_ACTIVITY_ZHUI_YUE_SHANG_DRAW_RECORD_SC = 5352,			// ׷���̵�

		MT_GUA_JI_EXP_INFO_SC = 5355,				// ÿ�չһ�����

		//����ϵͳ
		MT_HUAN_SHOU_REQ_CS = 5360,					//��������
		MT_HUAN_SHOU_ALL_INFO_SC = 5361,			//���л�����Ϣ�·�
		MT_HUAN_SHOU_SINGLE_INFO_SC = 5362,			//����������Ϣ�·�
		MT_HUAN_SHOU_NOTICE_INFO_SC = 5363,			//������Ϣ֪ͨ�·�
		MT_HUAN_SHOU_DECOMPOSE_REQ_CS = 5364,		//�������Ƿֽ�����
		MT_HUAN_SHOU_OTHER_INFO_SC = 5365,			//����������Ϣ�·�
		MT_HUAN_SHOU_ANNOUNCEMENT_INFO_SC = 5366,	//���޹�����Ϣ�·�

		MT_HUAN_SHOU_BAG_ALL_INFO_SC = 5370,		//���ޱ���������Ϣ�·�
		MT_HUAN_SHOU_BAG_SINGLE_INFO_SC = 5371,		//���ޱ���������Ϣ�·�


		//
		MT_ROLE_GUILD_TASK_REQ_CS = 5400,			// ������������
		MT_ROLE_GUILD_TASK_INFO_SC = 5401,			// ��������
		MT_ROLE_GUILD_CHANGE_SC = 5402,			// ��������

		// ʵ����֤����
		MT_REAL_NAME_REWARD_RECEIVE_CS = 5410,		// ʵ����֤������ȡ����
		MT_REAL_NAME_REWARD_RECEIVE_INFO_SC = 5411,	// ʵ����֤������ȡ��Ϣ�·�
	};
}

#endif