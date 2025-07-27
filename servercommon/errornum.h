#ifndef __ERRORNUM_H__
#define __ERRORNUM_H__

#include "servercommon/servercommon.h"

namespace errornum
{
	enum ERROR_NUM_DEF
	{
		////////////////////////////////////// ����������� //////////////////////////////////////
		EN_KNAPSACK_USE_NOT_DIRECT = 100,				//����Ʒ����ֱ��ʹ��
		EN_KNAPSACK_IS_LOCK,							//�����Ѿ���ס
		EN_KNAPSACK_USE_ITEM_ERROR,						//ʹ����Ʒ����
		EN_KNAPSACK_USE_ITEM_NUM_NOT_ENOUGH,			//ʹ����Ʒ��������
		EN_KNAPSACK_LOAD_ERROR,							//��ȡ��Ʒ����
		EN_KNAPSACK_CANNOT_DISCARD,						//��Ʒ���ܶ���
		EN_KNAPSACK_MOVE_ERROR,							//��Ʒ�ƶ�����
		EN_KNAPSACK_SPLIT_CANNOT,						//��Ʒ���ܲ��
		EN_KNAPSACK_SPLIT_NUM_ERROR,	                //��Ʒ�����������
		EN_KNAPSACK_PUT_ERROR,							//��Ʒ�������
		EN_KNAPSACK_NO_SPACE,							//�����ռ䲻��
		EN_TEMPORARY_KNAPSACK_NO_SPACE,					//��ʱ�����ռ䲻��
		EN_KNAPSACK_EQUIP_TAKEOFF_ERROR,				//װ�����´���
		EN_KNAPSACK_USE_XUANYUN_STATE,					//ѣ��״̬������ʹ����Ʒ
		EN_KNAPSACK_STORAGE_EXTEND_MAX,					//�����ֿ���չ�Ѵ����ֵ
		EN_ITEM_IS_INVALID,								//��Ʒ��ʧЧ
		EN_ITEM_PROF_LIMIT,								//ְҵ������
		EN_ITEM_LEVEL_LIMIT,							//��ǰ�ȼ�����
		EN_ITEM_CANNOT_MULTI_USE,						//����Ʒ��������ʹ��
		EN_CHESTSHOP_NOT_ENOUGH_TEMP_SPACE,				//���䱳���ռ䲻��
		EN_ITEM_COLDDOWN,								//��Ʒ��ȴ��
		EN_KNAPSACK_USE_SCENE_LIMIT,					//��ǰ����������ʹ�ø���Ʒ
		EN_ITEM_TAOFA_TASK_NOT_ACCEPTABLE,				//�޷����ܸ��ַ�����
		EN_ITEM_TASK_LIST_FULL,							//�޷�ʹ���ַ��� �����б���
		EN_ITEM_USE_COUNT_LIMIT,						//ʹ�ô����ﵽ����
		EN_ITEM_TAOFA_TASK_HAS_TASK,					//�ü�����ַ���������δ��� �����ظ���ȡ��
		EN_ITEM_EVIL_NEED_NOT_USE,						//������ʹ����������
		EN_ITEM_SEX_LIMIT,								//�Ա𲻷���
		EN_KNAPSACK_CANNOT_DISCARD_INLAY,				//����Ƕ��ʯ���ܶ���
		EN_KNAPSACK_USE_DEAD,							//�������޷�ʹ����Ʒ
		EN_SUBLOCK_IS_LOCKED,							//������δ�⿪
		EN_SUBLOCK_ERROR_TIMES_LIMIT,					//�ѵ��������ÿ���������������
		EN_ITEM_CALL_MONSTER_IN_FB,						//�����ڲ����ٻ�����
		EN_SUBLOCK_PASSWORD_ILLEGAL,					//����������Ƿ�
		EN_ITEM_HAVE_BIGGER_BUFF,						//�������и�ǿ���״̬
		EN_EXTEND_KNAPSACK_TIME_LIMIT,					//����ʱ�䲻���Կ�����ǰ����
		EN_ITEM_UNUSABLE_ON_FLY,						//����״̬�²���ʹ�ø���Ʒ
		EN_ITEM_TIME_NO_IN_STORAGE,						//ʱЧ����Ʒ���ɷ���ֿ�
		EN_STOP_FLY_AREA_LIMIT,							//������������ֹͣ����
		EN_PET_COUNT_LIMIT,								//�޷�ӵ�и���ĳ���
		EN_PET_LEVEL_LIMIT,								//�Ѵﵽ���ȼ�
		EN_TARGET_NOT_ONLINE,							//�Է�������
		EN_NOT_MEET_JOIN_CONDITIONS,					//����Է�Ҫ��ĵȼ�����
		EN_HAS_BEEN_SEND_JOIN_REQ,						//�ѷ��͹��������
		EN_NOT_ELIGIBLE,								//��������ȡ����
		EN_STUFF_NOT_ENOUGH,							//������Ʒ����
		EN_KNAPSACK_EXTEND_TIMES_LIMIT,					//��չ�����Ѵ�����
		EN_KNAPSACK_ORDER_TOO_FREQUENT,					//����������Ƶ�������Ժ�����
		EN_ITEM_IS_BIND,								//��Ʒ�Ѱ�
		EN_ITEM_CAN_NOT_TRADE,							//��Ʒ���ɽ���
		EN_ITEM_AMOUNT_NOT_ENOUGH,						//��Ʒ��������
		EN_ITEM_NOT_ELIGIBLE,							//������Ʒ����
		EN_PET_COUNT_LIMIT2,							//���ﱳ���ռ䲻�㣬���ڳ��ռ������
		EN_KNAPSACK_CANNOT_DISCOMPOSE,					//��Ʒ���ֽܷ�
		EN_SURFACE_ALREADY_HAD,							//��ӵ�и�ʱװ�������ʱװ�ɽ��зֽ�
		EN_SURFACE_DECOMPOSE_ERROR,						//ʱװ�ֽ�ʧ��
		EN_KNAPSACK_NO_ITEM,							//���ı���û���������
		EN_KNAPSACK_CONVERT_ITEM,						//��Ǹ,��Ʒ�һ�ʧ��,��鿴�ʼ�
		EN_KNAPSACK_CONVERT_ALREADY_OWNED,				//�������ظ�ʱװ,�µĻ��Զ�ת��Ϊ��Ӧ��ʱװ��������
		EN_PET_BAG_EXTEND_LIMIT,						//���ﱳ�������Ѵﵽ����
		EN_PET_NOT_TAKE_EQUIPMENT,						//����û�д���װ��
		EN_EQUIPMENT_PUT_LEVEL_LIMIT,					//�ȼ����㣬�޷�����
		EN_PET_NO_EQUIPMENT,							//����δ����װ��
		EN_OPERATE_NEED_IN_CROSS,						//��Ҫ�ڿ������
		EN_CROSS_ROLE_CANNOT_INVITE,					//��Ŀǰ����ԭ�����޷����봦�ڿ��״̬�µ����
		EN_KNAPSACK_PARTNER_SKIN_CONVERT,				//�������ظ����Ƥ�����µĻ��Զ�ת��Ϊ���Ƥ����Ƭ
		EN_KNAPSACK_PET_SKIN_CONVERT,					//�������ظ�����Ƥ�����µĻ��Զ�ת��Ϊ����Ƥ����Ƭ
		EN_KNAPSACK_ITEM_ENOUGH_COMMIT_ERROR,			//��Ʒ���㣬�޷��ύ
		EN_PET_NO_SOUL_WEAPON_ERR,						//����δ��������
		EN_PET_SOUL_WEAPON_PARAM_ERR,					//�������´���
		EN_PET_SOUL_WEAPON_LEVEL_LIMIT_ERR,				//�����ȼ�����
		EN_PET_SOUL_BELONG_ERR,							//�ó����޷��������ר������
		EN_KNAPSACK_ITEM_NOT_ELIGIBLE,					//��Ʒ������
		EN_MARRY_NOT_DISAGREE,					//�Է���ͬ��
		EN_MARRY_REPLY_TIME_OUT,					//�����ѹ���
		EN_HAS_QING_YUAN,					//һ��������Ե
		EN_HAS_QING_YUAN_INTIMACY_NOT_ENGOUH ,		//���ܶȲ���
		EN_QING_YUAN_REPLY_TIME_OUT,		//�����ѹ���
		EN_QING_YUAN_CANT_GIVE,					//�Է��������δ���ڣ��޷���������
		EN_QING_YUAN_GIFT_ALREADY_FETCH,					//�����Ѿ���ȡ��
		EN_QING_YUAN_DISAGREE_DISMISS,					//�Է��Ѿܾ��������������
		EN_QING_YUAN_AGREE_DISMISS,					//�Է�ͬ�����������
		EN_QING_YUAN_CANT_SINGLE_LEAVE,					//�Է�����ʱ�����㣬���ܵ��������
		EN_QING_YUAN_NULL,								//���ȵ޽���Ե, 

		EN_HUAN_SHOU_BAG_NOT_SPACE,						//���ޱ����ռ䲻��

		////////////////////////////////////// ������������ //////////////////////////////////////
		EN_ROLE_LEVEL_TOO_LOW = 200,					//����ȼ�����
		EN_ROLE_LEVEL_OVERTOP,							//����ȼ�����
		EN_ROLE_OCCUP_NOT_FIT,							//����ְҵ������Ҫ��
		EN_ROLE_ATTR_POINT_LIMIT,						//���ܳ�����ǰ�����ʵ��һ��
		
		////////////////////////////////////// ������������ //////////////////////////////////////
		EN_TASK_PREDECESSOR_NEED = 250,					//���������ǰ������
		EN_TASK_MISSION_COMPLETE = 251,					//�����Ѿ���ɣ���ѡ���ύ����
		EN_TASK_TIMER_LIMIT_OVER = 252,					//����ʱ���ѹ�����ѡ���������
		EN_TASK_NOLONGER_EXISTED = 253,					//�����Ѳ����ڣ���ѡ���������
		EN_TASK_IS_NOT_COMPLETED = 254,					//������δ��ɣ�������������
		EN_TASK_ALREADY_FINISHED = 255,					//������������ɹ���
		EN_TASK_ALREADY_ACCEPTED = 256,					//���Ѿ��������������
		EN_TASK_CONDI_MUST_ALONE = 257,					//���������뵥�����
		EN_TASK_CONDI_NEED_MORES = 258,					//���������Ҫ�����Ա
		EN_TASK_LIST_ALREADY_MAX = 259,					//��ǰ����̫�࣬�����ٽ�������
		EN_TASK_CAN_NOT_BE_ABORT = 260,					//���������ڵ�ǰ����·���
		EN_TASK_TIME_ALREADY_MAX = 261,					//��������Ѵ�����
		EN_TASK_NOT_ACCEPTEND = 262,					//������δ��ȡ
		EN_ESCORT_TASK_ALREADY_ACCEPTED = 263,			//�������������ʦ�������������µ�����
		EN_TASK_LUO_SHU_QI_WEN_NOT_FINISH = 264,		//�����ƺ����������أ��ټ��һ�°ɣ�
		EN_TASK_PARTNER_SOAR_TASK_FINISH = 265,			//��������ɹ�������������������
		EN_TASK_PARTNER_SOAR_TASK_IN_PROGRESS = 266,	//��ǰ���ڽ��л���������
		EN_TASK_NOT_GIVE_UP_TASK = 267,					//��Ǹ,�������ܷ���
		EN_TASK_PARTNER_SOAR_NOT_ACCPET_BY_ROLE_NOT_SOAR = 268,	//��ɫ��δ���з���,��ʱ�޷���ȡ����������
		EN_TASK_ACCEPT_OPEN_SERVER_LIMIT = 269,			//������δ�ɽ�ȡ�������ĵȴ�

		///////////////////////////////////////// ���ģ�� /////////////////////////////////////////
		EN_PARTNER_STATUS_IS_LOCKED = 270,				//�����黹û����Ŷ
		EN_PARTNER_STATUS_IS_UNLOCK,					//�������Ѿ�������
		EN_PARTNER_SYNCFIGHT_LIMITS,					//����ս���Ѵ�����
		EN_PARTNER_GRADE_ALREAY_MAX,					//�������Ѿ�������
		EN_PARTNER_ATTR_BREAKTH_UNFINISHED,				//�����������ͻ����δ���
		EN_PARTNER_UPGRADE_TASK_UNFINISHED,				//�����������������δ���
		EN_PARTNER_SKILLS_IS_LOCKED,					//������ܻ�û�н���
		EN_PARTNER_LEVEL_TOO_LOW,						//���ȼ�����
		EN_PARTNER_EQUIP_UNLOCK_TOO_LOW,				//��������ȼ������޷�װ��
		EN_PARTNER_BIOGRAPHY_UPGRADE_LV_NOT_WORTH,		//��Ǹ,�û��Ƚײ���,�޷��鿴�˴���

		///////////////////////////////////////// ����ģ�� /////////////////////////////////////////
		EN_PET_LEVEL_DEFICIENCY = 280,					//����ȼ�����
		EN_PET_INDEX_ERROR,								//������Ŵ���
		EN_PET_SKILL_INDEX_ERROR,						//���＼�ܲ۲���
		EN_PET_SKILL_NEW_LOWER_THAN_OLD,				//��ѧ����߼��ļ���
		EN_PET_IS_FIGHT,								//�ó��ﴦ�ڳ�ս״̬������ʧ��
		EN_PET_IS_STANDBY,								//�ó��ﴦ�ڴ���״̬������ʧ��
		EN_STANDBY_AND_FIGHT_PET_NOT_EXCEED_5,			//�����ͳ�ս���ﲻ�ܳ���5ֻ
		EN_PET_IS_LOCK,									//�ó���������������ʧ��
		EN_PET_IS_BIND,									//�ó����Ѱ󶨣�����ʧ��
		EN_PET_CAN_NOT_COMPREHEND_GIFT,					//�ó����Ѿ�û�м��ܿ�������
		EN_PET_COMPOSE_FAILD_NEED_PET_ITEM_NOT_ENOUGH,	//����ר����Ƭ����

		///////////////////////////////////////// NPCģ�� /////////////////////////////////////////
		EN_NPC_ROLE_IS_FAR_FROM_NPC,					//����Ŀ��̫Զ
		EN_NPC_ROLE_IS_BUSY_CURRENT,					//æµ��...

		///////////////////////////////////////// ����ģ�� /////////////////////////////////////////
		EN_WORKSHOP_RECIPE_ALREADY_ACTIVE = 300,		//����䷽�Ѿ��������Ŷ
		EN_WORKSHOP_RECIPE_ACTIVE_SUCCEED = 301,		//�����䷽����ɹ�
		EN_WORKSHOP_GEM_MOUNT_ALREADY_MAX = 302,		//��ʯ��Ƕ�Ѵ�����
		EN_WORKSHOP_GEM_PARAM_OCCUR_ERROR = 303,		//��ʯ�������ִ���
		EN_WORKSHOP_RECIPE_IS_NOT_ACTIVED = 304,		//Ŀ���䷽��δ����
		EN_WORKSHOP_ITEM_COMPOUND_FAILURE = 305,		//Ŀ����Ʒ�ϳ�ʧ��
		EN_WORKSHOP_EQUIPMENT_CAN_T_CRAFT = 306,		//��װ����֧��ǿ������
		EN_WORKSHOP_WITHOUT_LEGAL_RESULTS = 307,		//����ʧ�ܣ�û��ƥ��ĳɹ���
		EN_WORKSHOP_REFORGE_RESULTS_EXIST = 308,		//�����ϴ��ضͽ��
		EN_WORKSHOP_GEM_ALREADY_MAX_LEVEL = 309,		//�����ʯ�Ѿ�����߼��ı�ʯ�ˣ�

		///////////////////////////////////////// �̵�ģ�� /////////////////////////////////////////
		EN_SHOP_ITEM_NOT_SUPPORT_FAST_BUY = 310,		//�˵��߲�֧�ֿ��ٹ���
		EN_SHOP_ITEM_NOT_CROSS_CONVERT_BUY = 311,		//����޷��һ����뷵��ԭ��		

		///////////////////////////////////////// ��ֵϵͳ /////////////////////////////////////////
		EN_FACE_SCORE_RES_ALREADY_BUY = 320,			//�Ѿ�ӵ���������ֵ��Դ�������ظ�����
		EN_FACE_SCORE_CURRENCY_INSUFF = 321,			//��ֵ�Ҳ���
		EN_FACE_SCORE_RES_IS_INACTIVE = 322,			//�����ֵ��Դ��û�м������ʧ��
		EN_FACE_SCORE_NOT_REPEAT_WEAR = 323,			//�����ظ�����
		EN_FACE_SCORE_RES_NOT_ACTIVE  = 324,			//���Ѽ������ֵ���ߣ�����ʧ��

		/////////////////////////////////////////// ���� ///////////////////////////////////////////
		EN_GUILD_INVILD_NAME = 400,						//�������Ѵ���
		EN_GUILD_APPLICANT_HAS_GUILD = 401,				//��������ӵ�м���
		EN_GUILD_IMPEACHMENT_CANNOT = 402,				//��ǰ�޷������峤
		EN_GUILD_INSUFFICIENT_AUTHORITY= 403,			//Ȩ�޲���
		EN_GUILD_POST_REACHED_MAXINUM = 404,			//ְλ�����Ѵ����
		EN_GUILD_GONGXIAN_NOT_ENOUGH = 405,				//���ײ���
		EN_GUILD_BOSS_LIMIT_TODAY = 406,				//��������ս������������
		EN_GUILD_NOT_IN_GUILD = 407,					//����û�м����κμ��壬���ȼ�����������~
		EN_GUILD_APPLY_SUCC = 408,						//����ɹ����������������
		EN_GUILD_APPLY_REPEAT = 409,					//�����Ѿ�������ü�����Ŷ
		EN_GUILD_EXP_NOT_ENOUTH = 410,					//�����ʽ���
		EN_GUILD_APPLY_LIMIT_NUM = 411,					//������ļ�������̫�࣬�����ĵȴ�
		EN_GUILD_IS_SAME_AS_YOURS = 412,				//�Է���������ͬһ�����У�����ʧ��
		EN_GUILD_OPPOSITE_IS_NOT_IN_GUILD = 413,		//�Է�û�м����κμ��壬����ʧ��
		EN_GUILD_OPPOSITE_HAVE_GUILD_YET = 414,			//�Է����м��壬����ʧ��
		EN_GUILD_YOU_HAVE_GUILD = 415,					//���Ѿ�ӵ�м��壬����ʧ��
		EN_GUILD_BOSS_FINISH = 416,						//����������ȫ����ɱ
		EN_GUILD_PET_TRAINING_ALREADY = 417,			//��������Ѿ���ѵ������
		EN_GUILD_CAN_NOT_KICKOUT_SELF = 418,			//���ܰ��Լ��߳�����Ŷ~
		EN_GUILD_CAN_NOT_APPOINT_SELF = 419,			//���ܸ��Լ�����ְ��Ŷ~
		EN_GUILD_LEVEL_ALREADY_MAX = 420,				//����ȼ��Ѵ�����
		EN_GUILD_BOSS_CANT_SELECT_CHAPTER = 421,		//ֻ��ѡ����ͨ�ص��½�
		EN_GUILD_CAN_NOT_IMPEACH_YOURSELF = 422,		//���ܵ����Լ�Ŷ~
		EN_GUILD_MAX_MEMBER_CAN_NOT_JOIN = 423,			//���������������޷�����
		EN_GUILD_CREATE_GOLD_NOT_ENOUGH = 424,			//����Ԫ������,�벹�������
		EN_GUILD_CANT_INVITE_OTHER_FREQUENTLY,			//��Ǹ��10�����ڲ�����Ƶ������ͬһ��ɫ
		EN_GUILD_GUILD_FIGHT_OPEN_NOT_QUIT_GUILD,		//��Ǹ,������Ҷ��ڼ���ʱ�޷��˳�����
		EN_GUILD_BANNER_SENSITIVE_WORDS,				//��Ǹ���������������ݺ��������ַ���������
		EN_GUILD_BANNER_EMPTY,							//����������������������
		EN_GUILD_HAS_SET_BANNER_TODAY,					//���������ù���գ�����������
		EN_GUILD_BANNER_IS_USED,						//��Ǹ��������Ѿ���ռ��
		EN_GUILD_FIGHT_NOT_SAME_GUILD_WITH_LEADER,		//��ӳ���ͬ���壬�޷����뵽������
		EN_GUILD_GATHER_DREAM_NOT_CAN_PRAY,				//��Ǹ,����Ʒ�ʵĳ��ﲻ����Ը
		EN_GUILD_GATHER_DREAM_NOT_REPEATABLE,			//��Ǹ,�������ѷ�������Ը,�����ظ�����
		EN_GUILD_GATHER_DREAM_NOT_ROLE,					//��Ǹ,����Ҳ�δ������Ը��Ϣ
		EN_GUILD_GATHER_DREAM_NOT_GIFT,					//��Ǹ,������Ѽ��뼯Ը
		EN_GUILD_GATHER_DREAM_NOT_CAN_GIFT,				//��Ǹ,�����������͹������
		EN_GUILD_GATHER_DREAM_NOT_HAVE_GIFT_NUM,		//��Ǹ,������ļ�Ը��������
		EN_GUILD_GATHER_DREAM_NOT_FETCH,				//��Ǹ,��ǰ���޿�����Ƭ
		EN_GUILD_GATHER_DREAM_GIFT_MY,					//��Ǹ,�㲻�����͸��Լ�
		EN_GUILD_NAME_EMPTY,							//�������������������
		EN_GUILD_GATHER_DREAM_NOT_ENOUGH,				//����,����ǰ���е���Ƭ��������Ŷ~
		EN_GUILD_FREQUENTLY_QUILD,						//������Ƶ���˳����壬���Ժ�����
		EN_GUILD_MAX_BUILD_LEVEL,						//�Ѵﵽ������ߵȼ�,�޷�������
		EN_GUILD_LIMIT_BUILD_LEVEL,						//��������ȼ�����,������������
		EN_GUILD_BUILDING_IS_BEING_BUILD,				//��ǰ�н����������������Ժ����ԡ�
		EN_GUILD_NOT_BUILDING_PERMISSIONS,				//�˵ȼ�����£��������峤�����峤��������
		EN_GUILD_LACK_STUDY_LEVEL,						//�鷿�ȼ�����,�������������鷿�ȼ�
		EN_GUILD_MEMBER_EXCEED,							//��Ա��������,�޷����
		EN_GUILD_OTHER_BUILD_BUILDING,					//�н������ڽ�����,��ǰ���ɲ��
		EN_GUILD_NOT_GUILD_CAN_NOT_USE,					//��ǰδ�������,�޷�ʹ��
		EN_GUILD_CAN_NOT_REPEAT_OPEN_SPEED_UP,			//�����ظ��������ٽ���
		EN_GUILD_SPEED_UP_LIMIT_COUNT,					//���տ������ٴ���������
		EN_GUILD_JOIN_SPEED_UP_BUILDING_LACK_COIN,		//ͭ�Ҳ��㣬�޷����ٽ���
		EN_GUILD_CANNOT_REPEAT_SPEED_UP,				//���Ѿ��������Լ�����������������һЩ����ɣ�
		EN_GUILD_SPEED_UP_FINISH,						//���ٽ�����ɣ�
		EN_GUILD_SPEED_UP_SUCCESS,						//���彨����ٳɹ�
		EN_GUILD_LAST_BUILDING_TIMES,					//���ս�����������꣬����������
		EN_GUILD_LEVEL_NOT_ENOUGH,						//����ȼ�����


		//////////////////////////////////////// ���� ////////////////////////////////////////////
		EN_SCENE_TRANSPORT_TOO_FAR = 500,				//���봫�͵�̫Զ
		EN_SCENE_TRANSPORT_LIMIT_BY_ROLE_STATUS,		//��ɫ���ڲ��ܴ��͵�״̬
		EN_FB_NOT_EXIST,								//����������
		EN_FB_ENTER_FB_ERROR,							//���븱������
		EN_SCENE_TRANSPORT_FB_LIMIT,					//������Ӵ������ƶ�������
		EN_CURR_LEVEL_NOT_ENTER_SCENE,					//��ǰ�ȼ��޷�����
		EN_SCENE_UNLOCK_YET,							//�ó���̫��Σ�գ���ȼ���Щ����
		EN_MEMBER_CAN_NOT_ENTER_SCENE,					//�������в��ܽ���Ŀ�곡���Ķ�Ա
		EN_SPECIAL_CAN_NOT_MOVE_ON,						//�ܵ�����Ч��Ӱ�죬�����ƶ�
		EN_SCENE_FORBID_ENTER_TEAM,						//�˳�����Է�������������������Ҵ�����������
		EN_TARGET_SCENE_NO_SAME,						//�Է��������ڲ�ͬ�ĳ���������ʧ��
		EN_YOU_ARE_IN_FB_NOW,							//���ڸ����У�����ʧ��
		EN_THIS_SCENE_FORBID_LEAVE_GUILD,				//�˳�������������뿪����
		EN_TARGET_SCENE_FORBID_LEAVE_GUILD,				//�Է������������������뿪����
		EN_MEMBER_LEVEL_NOT_ENTER_SCENE,				//�������ж�Ա�ȼ����ͣ��޷�����ó���
		EN_ALREADY_IN_GUILD,							//�����м��壬����ʧ��
		EN_SCENE_EXCEED_MAX_LEVEL_LIMIT,				//��Ǹ,���ĵȼ�����,�޷�����ó���
		EN_SCENE_NOT_CAN_FAST_PASS_FB,					//��ǰ����������ʹ�ÿ���ɨ��,���˳��ó���������

		//////////////////////////////////////// ��� ////////////////////////////////////////////
		EN_ROLE_NOT_IN_TEAM = 600,						//�����ڶ����У�����ʧ��
		EN_ROLE_IN_TEAM,								//���ڶ����У�����ʧ��
		EN_ROLE_NOT_LEADER,								//�����Ƕӳ�������ʧ��
		EN_TEAM_IS_FULL,								//������������
		EN_TARGET_HAVE_TEAM,							//�Է����ж���
		EN_HAS_DISMISS_MEMBER_IN_TEAM,					//�������������ң�����ʧ��
		EN_FOLLOW_MEMBER_NOT_ENOUGH,					//����ǳ�ǿ���Ҽ�������������ս��
		EN_NOW_STATUS_UN_JOIN_TEAM,						//��ǰ״̬�޷��������
		EN_MEMBER_LEVEL_NOT_ENOUGH,						//����������ҵȼ�����
		EN_MEMBER_LEVEL_TOO_HIGH,						//����������ҵȼ�����
		EN_IN_TEAM_CAN_NOT,								//����в���ִ�иò���
		EN_MEMBER_GUILD_NOT_UNIFORM,					//����������Ҽ����������ļ��壬����ʧ�ܣ�
		EN_MEMBER_GUILD_NOT_EXIST,						//�����������û�м����κμ��壬����ʧ�ܣ�
		EN_TARGET_IS_NOT_LEADER,						//�Է����Ƕӳ�������ʧ��
		EN_MATCH_CROSS_TEAM_NOT_EXIST,					//ƥ����鲻���ڻ��ѽ�ɢ���뷵��ԭ������
		EN_CROSS_TEAM_NOT_AVALIABLE,					//������ƥ���ݲ����ã���Ϊ�������������ƥ��
		EN_TEAM_LEADER_CANT_CHANGE_POSITION,			//�ӳ�վλ�޷��ı�
		EN_TEAM_EMPTY_POSITION_CANT_SWITCH,				//�޷����λ�û���վλ
		EN_MEMBER_FUN_OPEN_LIMIT,						//�������������δ�����������
		EN_ROLE_DONT_WANT_TO_JOIN,						//�Է��ܾ��������������
		EN_TEAM_DISMISS,								//�����ѽ�ɢ
		EN_TEAM_CAN_NOT_APPLY_JOIN,						//�ö��鲻�����������
		EN_TEAM_NOT_EXIST,								//���鲻����
		EN_TEAM_LEVEL_LIMIT,							//�����϶���ȼ�����
		EN_CAN_NOT_INVITE_YOURSELF,						//���������Լ����Ŷ~
		EN_IN_BATTLE_CANNOT_TEMPORARILY_PART,			//ս��״̬���޷�����
		EN_LEVEL_LIMIT_CANNOT_GO_TO_LEADER_SCENE,		//�ȼ������޷�����ӳ����ڵ�ͼ
		EN_TEAM_MEMBER_LACK_OF_TICKET,					//����������ȱ����Ʊ
		EN_TEAM_MEMBER_MUST_TEMP_APART,					//�޷����͵��ӳ�λ�ã���Ϊ���������
		EN_TEAM_LEADER_CANNOT_LEAVE_BATTLE,				//�ӳ��޷��˳���ս
		EN_TEAM_VOTE_ALREADY_VOTE,						//����ͶƱ�������ظ�ͶƱ
		EN_TEAM_VOTE_HAS_NO_VOTE,						//��Ǹ����û��ͶƱȨ
		EN_TEAM_VOTE_NO,								//δ��ö�����Աͬ�⣬����ʧ��
		EN_TEAM_VOTE_ROLE_IS_LEADER,					//�����Ƕӳ������跢��ͶƱ
		EN_TEAM_VOTE_IS_ON,								//���о�ѡ�ڽ��У����Ժ�
		EN_TEAM_VOTE_IS_OFF,							//ͶƱ�ѽ���
		EN_MEMBER_NOT_ENOUGH_LEVEL,						//����������ҵȼ�δ�����������
		EN_ARRESTING_THIEVES_IS_SOLO_NOT_TEAM,			//��������ֻ�ܵ��˲��룬���״̬�޷�����ս��
		EN_TEAM_MEMBER_STEP_OUT,						//��ǰ�ж�Ա�Ӷ���������
		EN_YOU_HAVE_BEEN_KICKED,						//���ѱ��Ƴ�����
		EN_SORRY_OTHER_SIDE_TEAM_DISMISS,				//��Ǹ,�Է������ѽ�ɢ
		EN_SORRY_OTHER_SIDE_TEAM_FULL,					//��Ǹ,�Է���������Ա
		EN_TEAM_CAN_NOT_CHANGE_LEADER,					//��Ǹ,�����޷������ӳ�
		EN_TEAM_MEMBER_NOT_ENGOUH,						//��Ǹ,������������
		EN_TEAM_MEMBER_NOT_TRANSPORT,					//�������ж�Ա���ڲ���������ʧ��
		EN_TEAM_MEMBER_NOT_BE_ALLOW_ENTER_BATTLE,		//�ж�Ա�����Ͻ�������
		EN_TEAM_MEMBER_LIMIT_PLAY_TIMES,				//�ж�Ա�����������
		EN_CAN_NOT_TEMP_APART_IN_FB,					//�ڸ����в�������
		EN_TEAM_MEMBER_LIMIT_LEVEL_NOT_MEET,			//����������Ҳ��������Ŀ��ȼ�Ҫ�󣬲���ʧ��
		EN_SORRY_OTHER_SIDE_TEAM_NOT_AVALIABLE,			//��Ǹ���Է������ݲ��ɽ���
		EN_TEAM_LEVEL_MORE_TEN,							//��Ա�ȼ�����10�����޷�������ս
		EN_CAN_NOT_NVITATE,								//��ǰ״̬�޷������������
		EN_TARGET_TEAM_NOW_NOT_AVALIABLE,				//Ŀ������ѽ��븱����������ʱ�޷����룬���Ժ�����
		EN_TEAM_TOO_MANY_PEOPLE,						//��������̫���ˣ����ݾ��ߵģ�
		EN_TEAM_LEVEL_LOW_LIMIT,						//������͵ȼ�����С�ڵ������ĵȼ�
		EN_MEMBER_NOT_ONLINE_LEADER_NO_CHANGE,			//����Ҵ�������״̬���޷���Ϊ�ӳ�
		EN_CROSS_MEMBER_LEAVE_TEAM,						//�������뿪����
		EN_ENTER_SCENE_NOT_TEAM,						//��Ǹ���ó����޷���ӽ���
		EN_TEAM_IN_HUAN_JIE_MATCHING_CANN_ADD,			// �������ڻý�ƥ���У��޷�����
		EN_ROLE_IN_HUAN_JIE_MATCHING,					// ���ڻý�ƥ���У��޷�����
		EN_TEAM_MEMBER_LEVL_NOT_ENOUGH,					// ����������ҵȼ�����
		EN_TEAM_NOT_EXIST_2,								//�����ѽ�ɢ
		EN_TEAM_INVITE_CROSS_USER_SERVER_NOT_ALLOW,		// ����Ҳ����ڱ������뵽�����������
		EN_TEAM_INVITE_CROSS_USER_IN_BATTLE,			// ���������ս���У����Ժ�����
		EN_TEAM_CAN_NOT_JOIN_SHAN_HAI_DENG_HUI,			// �������ڽ��еƻ������,��ʱ���ɼ���
		EN_NETHER_TODAY_TIMES_LIMIT,					// ������ս�����Ѿ�ʹ�����

		/////////////////////////////////////// �ʼ�ϵͳ ��� ///////////////////////////////////////
		EN_HAS_NO_SPACING = 700,						//�����ռ䲻��
		EN_ITEM_IS_NOT_EXIST,							//������Ʒ������
		EN_ITEM_NOT_FETCH_ATTACHMENT,					//�������и�����û����ȡŶ
		EN_ONEKEY_FETCH_CD_LIMIT,						//һ����ȡ̫Ƶ�������Ժ�����
		EN_KNASACK_NO_SPACE_SEND_MAIL,					//��Ǹ,���ı����ռ䲻��,������ͨ���ʼ�����

		///////////////////////////////////////// ս����� /////////////////////////////////////////
		EN_MONSTER_IS_LOCK = 800,						//��ǰ�Ĺ�������ս����...
		EN_ROLE_NOT_SUIT_FOR_BATTLE = 801,				//��ɫ��ǰ�޷�ս�������Ժ�����
		EN_MEMBER_NOT_SUIT_FOR_BATTLE = 802,			//�ж�Ա��ǰ�޷�ս�������Ժ�����
		EN_CUR_SCENE_NO_MINE_MONSTER = 803,				//��ǰ����û�а���
		EN_SERIALIZE_ROLE_BATTLE_DATA_FAILED = 804,		//���л����ս������ʧ��
		EN_UNSERIALIZE_ROLE_BATTLE_DATA_FAILED = 805,	//�����л����ս������ʧ��
		EN_ENEMY_ROLE_CAN_NOT_BATLLE = 806,				//�Է���ǰ�޷�ս�������Ժ�����
		EN_QIE_CUO_REQ_ALREADY_SEND = 807,				//�����ѷ��ͣ������ĵȴ��ظ�
		EN_TARGET_NOT_IN_BATTLE = 808,					//Ŀ�겻��ս���У���սʧ��
		EN_TARGET_IS_IN_BATLLE = 809,					//�Է�����ս���У�����ʧ��
		EN_SAME_TARGET_TWO_COMMAND_LIMIT = 810,			//ͬһĿ�����ֻ�ܱ�ָ������ָ��
		EN_OBSERVER_WATCH_USER_NOT_FOUND = 811,			//�Ҳ���Ҫ��ս����
		EN_TARGET_IS_OB_ING = 812,						//Ŀ�����ڹ�ս
		EN_ROLE_IS_OB_ING = 813,						//�����ڹ�ս������ʧ��
		EN_CAN_NOT_LEAVE_OB_ON_THIS_MODE = 814,         //���޷��뿪��ս����ȴ�ս������
		EN_ROLE_NOT_OB_ING = 815,                       //����ǰ���ڹ�ս״̬������ʧ��
		EN_BATTLE_NOT_FOUND = 816,						//�Ҳ�������ս�������Ϣ������ʧ��
		EN_HONOR_FIGHT_FORBID_LEAVE_OB = 817,			//��δ�μ�ս������ս��������
		EN_IN_PREPARING_CROSS_STATUS_OB_LIMIT = 818,	//��ǰ���ڿ��׼��״̬���޷���ս
		EN_MP_NOT_ENOUGH_PERFORM_SKILL_FAIL = 819,		//���������޷�ʩ�ż���
		EN_HP_NOT_ENOUGH_PERFORM_SKILL_FAIL = 820,		//Ѫ�������޷�ʩ�ż���
		EN_COND_NOT_MEET_PERFORM_SKILL_FAIL = 821,		//���������޷�ʩ�ż���
		EN_NOT_ALLOW_USE_MEDICINE = 822,				//�޷�ʹ��ҩƷ
		EN_NOT_ALLOW_MODIFY_ANIMATION_SPEED = 823,		//��Ǹ����ǰս���޷�����2����
		EN_SKILL_REMAIN_CAST_TIMES_NOT_ENOUGH = 824,	//����ʣ���ͷŴ�������
		EN_ONLY_LEADER_CAN_MODIFY_ANIMATION_SPEED = 825,//2����ս��ֻ�жӳ����ܿ���Ŷ

		////////////////////////////////////////// ������ //////////////////////////////////////////
		EN_TRADE_MARKET_SHELVE_FULL = 850,				//������������������
		EN_TRADE_MARKET_ADD_ON_SHELVE_FAILED,			//�ϼ�ʧ�ܣ����Ժ�����
		EN_TRADE_MARKET_REMOVE_ITEM_NOT_FOUND,			//δ�ҵ���Ҫ�¼ܵĻ�Ʒ
		EN_TRADE_MARKET_CASH_OUT_FIRST,					//��������
		EN_TRADE_MARKET_CASH_OUT_ITEM_NOT_FOUND,		//δ�ҵ������ֵĻ�Ʒ
		EN_TRADE_MARKET_ITEM_CAN_NOT_CASH_OUT,			//�û�Ʒ�޷�����
		EN_TRADE_MARKET_ITEM_CASH_OUT_FAILED,			//����ʧ�ܣ����Ժ�����
		EN_TRADE_MARKET_SELLER_NOT_FOUND,				//δ�ҵ������ң����Ժ�����
		EN_TRADE_MARKET_ITEM_NOT_FOUND,					//�����Ʒ�Ѿ��¼���Ŷ������ѡ��������Ʒ
		EN_TRADE_MARKET_ITEM_ANNOUNCING,				//��Ʒ���ڹ�ʾ�ڣ������ڴ�
		EN_TRADE_MARKET_ITEM_NOT_ENOUGH,				//��Ʒ�������㣬��ȷ������
		EN_TRADE_MARKET_ITEM_NOT_FOUND_FOR_AUTO_BUY,	//δ�ҵ�����Ļ�Ʒ����Ʒ������
		EN_TRADE_MARKET_UNFOCUS_FAILED,					//ȡ����עʧ�ܣ����Ժ�����
		EN_TRADE_MARKET_FOCUS_ITEM_NOT_FOUND,			//û���ҵ�Ҫ��ע����Ʒ����ˢ������
		EN_TRADE_MARKET_ITEM_CAN_NOT_FOCUS_NOW,			//��Ʒ���¼�
		EN_TRADE_MARKET_ITEM_FOCUS_FAILED,				//��עʧ�ܣ����Ժ�����
		EN_TRADE_MARKET_ITEM_SEARCH_TOO_FREQUENTLY,		//��������Ƶ�������Ժ�����
		EN_TRADE_MARKET_FOCUS_ITEM_OFFSHELVE,			//�û�Ʒ���������ѱ��¼�
		EN_TRADE_MARKET_FOCUS_ITEM_ON_SALE,				//�û�Ʒ���ϼܣ��뾡��ǰ������
		EN_TRADE_MARKET_NOT_ANOUNCE_OR_SALE,			//�û�Ʒ��δ��ʾ����
		EN_TRADE_MARKET_PRICE_TOO_LOW_TO_SALE,			//���ɵ�����Ʒ���׵׼�
		EN_TRADE_MARKET_CANNOT_FLIP_PAGE,				//̯λ���ޣ�������Ʒ�����Ŷ�
		EN_TRADE_MARKET_ITEM_TYPE_CANNOT_FOCUS,			//������װ�������βſ��Թ�ע
		EN_TRADE_MARKET_REDO_ADD_ITEM_ON_SHELVE_FAILED,	//�����ϼ�ʧ��
		EN_TRADE_MARKET_ITEM_NOT_OFF_SHELVE,			//��Ʒ�����¼�״̬
		EN_TRADE_MARKET_ITEM_SOLD_OUT,					//��Ʒ������
		EN_TRADE_MARKET_CROSS_CANNOT_OPERATE,			//��ǰ���ڿ��״̬���޷����е�ǰ����
		EN_TRADE_MARKET_EQUIP_MUST_BE_TREASURE,			//ֻ����Ʒ�����ڽ������н���
		EN_TRADE_MARKET_AUTOBUY_EXCEED_EXPECT_PRICE,	//��Ʒ��������۸񳬹�Ԥ�ڣ���������
		EN_TRADE_MARKET_ITEM_NUM_NOT_DEVIDED_EVENLY,	//�������۵���������������Ʒ�ĵ�λ����
		EN_TRADE_MARKET_ITEM_NUM_PER_GROUP_CHANGED,		//��Ʒ���ó��ֱ仯������ȡ����Ʒ�����²����ϼ�
		EN_TRADE_MARKET_PET_BIND_CAN_NOT_TRADE,			//��Ǹ���Ѱ󶨵ĳ����޷��ϼܽ���
		EN_TRADE_MARKET_PRICE_TOO_HIGH_TO_SALE,			//���ɸ�����Ʒ������߼�
		EN_TRADE_MARKET_NEIWAN_CANNOT_SALE,				//�޷����г��ۣ�������������ϵ�ͷ���Ա��
		EN_TRADE_MARKET_NEIWAN_CANNOT_BUY,				//�޷���ɹ���������������ϵ�ͷ���Ա��
		EN_TRADE_MARKET_PRICE_NOT_IN_RANGE,				//�ۼ۲�������Χ�ڣ����¼�����
		EN_CROSS_TRADE_MARKET_NOT_AVALIABLE_NOW,		//�������ݲ����ã����Ժ�����
		EN_TRADE_MARKET_PET_PRICE_NOT_IN_RANGE,			//�����ۼ۲��ںϷ���Χ��
		EN_ITEM_CANNOT_TRADE,							//����Ʒ���ɽ���
		EN_TRADE_MARKET_ITEM_CAN_NOT_BUY,				//����Ʒ���ɹ���

		///////////////////////////////////////// ���� /////////////////////////////////////////
		EN_WORLD_BOSS_ALREADY_DEAD = 900,					//���������ѱ�����
		EN_ACTIVITY_ISNT_IN_STANDY = 901,					//�������׼��״̬������ʧ��
		EN_COLOSSEUM_CAN_NOT_FIGHT = 902,					//���ѽ�����񱦿⣬��������ս
		EN_GAME_SEASON_ALREADY_END = 903,					//�����ѽ��㣬ս����Ч
		EN_WORLD_BOSS_2_ALREADY_DEAD = 904,					//���������ѱ�����
		EN_COLOSSEUM_NOT_IN_RANK_LIST = 905,				//�����ڱ���ս���б���
		EN_RAND_ACTIVITY_NOT_ENOUGH = 906,					//���Ĵ�������
		EN_RAND_ACTIVITY_BOUGHT = 907,						//��Ǹ,���ѹ�����¶�Ͷ��
		EN_RAND_ACTIVITY_NOT_BOUGHT = 908,					//��Ǹ,����δ�����¶�Ͷ��,�޷�ǩ��
		EN_RAND_ACTIVITY_EXPIRED = 909,						//��Ǹ,�����¶�Ͷ���ѹ���
		EN_RAND_ACTIVITY_MAKE_NOT_ENOUGH = 910,				//��Ǹ,���Ĳ�ǩ��������
		EN_RAND_ACTIVITY_PIGMENT_NOT_ENOUGH = 911,			//��Ǹ,���Ļ������ϲ���
		EN_RAND_ACTIVITY_NOT_OPEN_ITEM_NOT_USE = 912,		//��Ǹ,���δ����,�õ�����ʱ�޷�ʹ��
		EN_RAND_ACTIVITY_PICTURE_REACHED_THE_LIMIT = 913,	//��Ǹ,��ͼ����ʹ���Ѵ�����
		EN_RAND_ACTIVITY_ANIMAL_RACE_ITEM_NOT_ENOUGH = 914,	//��Ǹ,���ĵ��߲���,Ѻעʧ��
		EN_RAND_ACTIVITY_ANIMAL_RACE_BET_LIMIT = 915,		//��Ǹ,����Ѻע�����Ѵ�����,Ѻעʧ��
		EN_RAND_ACTIVITY_ANIMAL_RACE_BET_TIME = 916,		//��Ǹ,��ǰʱ�䲻�ٽ���Ѻע
		EN_RAND_ACTIVITY_MAX_RECEIVE_NUM = 917,				//����û����ȡ����
		EN_RAND_ACTIVITY_LAYER_NOT_OPEN = 918,				//��Ǹ,��ǰ�ؿ���δ����
		EN_ACTIVITY_SIGN_UP_FAILD = 919,					//��Ǹ,�������ѱ����μӸû
		EN_ACTIVITY_WAITING_FOR_COUNTDOWN = 920,			//���Եȣ�����ʱ�����󽫽���ս���׶�
		EN_RAND_ACTIVITY_HAVE_DISCOUNT = 921,				//��Ǹ,���ѳ�ȡ���ۿ�
		EN_RAND_ACTIVITY_BUY_NOT_ENOUGH = 922,				//��Ǹ,�����޹������
		EN_ACTIVITY_CROSS_SERVER_NOT_OPEN = 923,			//��Ǹ,�����������δ����,�������������
		EN_ACTIVITY_OPEN_NOT_ENTER_NOTICE = 924,			//��Ǹ,�û����ʽ����,���ڻ׼��ʱǰ��
		EN_ACTIVITY_IS_STANDY_NOT_ENTER = 925,				//�����׼���׶Σ�������ʽ���������Ժ�
		EN_RAND_ACTIVITY_BUY_NOT_ENOUGH_TODAY = 926,		//��Ǹ,���������޹������
		EN_RAND_ACTIVITY_BUY_PASS_ENOUGH = 927,				//��Ǹ,�����޹�����
		EN_RAND_ACTIVITY_GOD_TOKEN_IS_ACTIVE = 928,			//�߼�ս���Ѿ�����
		EN_RAND_ACTIVITY_GOD_TOKEN_LEVEL_NOT_ENOUGH = 929,	//ս��ȼ����㣬�����ÿ�������������
		EN_ACTIVITY_HUANJIEZHANCHANG_HUASHEN_SYN_SUCC = 930,		//��������ͬ���ɹ�
		EN_RAND_ACTIVITY_COLLECT_WORD_NOT_FETCH = 931,		//��Ǹ,����������ȡ����
		EN_RAND_ACTIVITY_GOD_TOKEN_PLEASE_ACTIVE = 932,		//��δ����߼�ս��
		EN_ACTIVITY_ENTER_TIME_LATER_NOT_ENTER = 933,		//��Ǹ���볡�׶��ѽ����������´β���
		EN_ACTIVITY_ARENA_IS_COLSE = 934,					//�������ѹر�			
		EN_RAND_ACTIVITY_NOT_FINISH_BATTLE_TASK = 935,		//�����ƺ���δ������������~
		EN_RAND_ACTIVITY_MAKE_UP_REWARD_FINISHED = 936,		//��Ǹ�����Ѽ�������ͼ���������ظ�����
		EN_RAND_ACTIVITY_MAKE_UP_CUR_TIME_NOT_USE = 937,	//��Ǹ����ǰʱ����ʱ�޷�ʹ�øù���
		EN_RAND_ACTIVITY_SUMMER_LI_LIAN_LEVEL_LOCK = 938,			//����ͨ����һ������
		EN_RAND_ACTIVITY_SUMMER_LI_LIAN_LEVEL_TODAY_LOCK = 939,		//�����������δ����
		EN_RAND_ACTIVITY_COLORFUL_LANTERN_OFFLINE_LOGIN = 940,		//���������ڼ�����Ϸʱ���ѵ�
		EN_RAND_ACTIVITY_LUCKY_SIGN_REWARD_FETCHED = 941,			//��Ǹ,���ѽ��й�ÿ��ռ��ǩ��
		EN_RAND_ACTIVITY_COOL_BREEZE_NOT_REFRESH = 942,				//���������������ˢ��һ��Ŷ~
		EN_RAND_ACTIVITY_LOTUS_MI_BAO_LIGHT_UP = 943,				//�Ѿ���������~
		EN_RAND_ACTIVITY_NOT_IN_ACTIVE_TIME = 944,					//���ڻʱ����
		EN_RAND_ACTIVITY_LACK_ITEM = 945,							//��Ǹ��������Ʒ����
		EN_RAND_ACTIVITY_UNFINISHED_TASK = 946,						//����δȫ������Ŷ
		EN_RAND_ACTIVITY_GAME_IN_PROGRESS = 947,					//��ǰ��Ϸ������
		EN_RAND_ACTIVITY_CAN_NOT_REPEAT_LOCK = 948,					//�޷��ظ�����
		EN_RAND_ACTIVITY_LOCK_UPPER_LIMIT = 949,					//���������������������޷�������
		EN_RAND_ACTIVITY_PLEASE_LOCK_ALL_REWARD = 950,				//���������������㣬�޷���Ը
		EN_RAND_ACTIVITY_PET_CHALLENGE_SUCCESS = 951,				//���ȳ��Ѿ���ս�ɹ���������ս�����ȳ��
		EN_RAND_ACTIVITY_NO_RECEIVE_FISHERMAN_TASK = 952,			//����ǰ�������ȡ����
		EN_RAND_ACTIVITY_PLEASE_SPECIFIED_TIME_RECEIVE = 953,		//����ָ��ʱ����ȡ~
		EN_RAND_ACTIVITY_FIND_SPRING_TASK_FINISH = 954,				//̽����ɣ�����ܴ󣬳�ȥ����Ŷ~
		EN_RAND_ACTIVITY_BO_ZHONG_YOU_LI_PLANT_LIMIT = 955,			//��ǰ������,���ɼ�����ֲ
		EN_RAND_ACTIVITY_BO_ZHONG_YOU_LI_NO_PLANTS = 956,			//��ǰû����ֲֲ���ֲ����ȫ������
		EN_RAND_ACTIVITY_BO_ZHONG_YOU_LI_PLANTS_IMMATURE = 957,		//��ֲ�ﻹδ����,�����ջ�
		EN_RAND_ACTIVITY_CHENG_FENG_PO_LANG_ANSWER_LIMIT = 958,		//��ǰ�������������,�´�������~
		
		EN_RAND_ACTIVITY_REWARD_ALL_RECEIVE_ALREADY,				//����������ȫ����ȡ~
		EN_RAND_ACTIVITY_PAI_MAI_HANG_END_OF_AUCTION,				//��Ǹ,����Ʒ�����Ѿ�����
		EN_RAND_ACTIVITY_PAI_MAI_HANG_NOT_BID_TIME,					//��Ǹ,��ǰ���Ǿ���ʱ��
		EN_RAND_ACTIVITY_PAI_MAI_HANG_NOT_BID_OF_HIGH,				//��Ǹ,����ǰ������Ϊ��߼�
		EN_RAND_ACTIVITY_PAI_MAI_HANG_BID_PRICE_CHANGE,				//�۸��ѱ���������³���
		EN_RAND_ACTIVITY_PAI_MAI_HANG_BID_NOT_ENOUGH,				//��Ǹ,���ĳ��۲���
		EN_RAND_ACTIVITY_PAI_MAI_HANG_DEPOSIT_NOT_ENOUGH,			//�Ĵ�Ԫ�����㣬��ֹͣ�Զ�����
		EN_RAND_ACTIVITY_PAI_MAI_DEPOSIT_NEED_AUTO,					//�״μĴ���Ҫѡ�Զ�����
		EN_RAND_ACTIVITY_PAI_MAI_HANG_BID_GOLD_NOT_ENOUGH,			//��Ǹ,�Ĵ�Ԫ������
		EN_RAND_ACTIVITY_PAI_MAI_HANG_RETRIEVE_FAILD,				//��Ǹ,��ѡȡ��Ԫ���������ǰʣ��Ĵ�
		EN_RAND_ACTIVITY_JINGJI_YUEKA_LEVEL_NOT_ENOUGH,				//��ȼ�����,���������ȼ�
		EN_RAND_ACTIVITY_JINGJI_YUEKA_NOT_ACTIVE,					//����δ������������ȼ���
		EN_RAND_ACTIVITY_JINGJI_YUEKA_LEVEL_LIMIT,					//�Ѵ������ȼ�
		EN_RAND_ACTIVITY_JINGJI_YUEKA_TASK_FINISH_NOT_REFRESH,		//��������ɣ�����ˢ��
		EN_RAND_ACTIVITY_JINGJI_YUEKA_ACTIVED,						//���Ѽ������,�����ظ����򼤻�

		///////////////////////////////////// ��Ծϵͳ //////////////////////////////////////////////
		EN_ACTIVE_SYSTEM_NOT_ENOUGH = 990,				//��Ǹ,���Ļ�Ծ�Ȳ���,�޷���ȡ����
		EN_ACTIVE_SYSTEM_REWARD_TOO_MUCH = 991,			//��Ǹ,�������߳�����Χ

		/////////////////////////////////////////// ���� ///////////////////////////////////////////
		EN_WST_COUNT_LIMIT = 1000,						//���մ����������
		EN_WST_NO_BUY_EXP_COUNT,						//û�й������
		EN_SKILL_FULL,									//���ܲ�����
		EN_SKILL_NOT_LEARN,								//������δѧϰ
		EN_SKILL_CANNOT_LEARN,							//���ܲ�����
		EN_SKILL_LEVEL_LIMIT,							//���ܵȼ��ѵ�����
		EN_SKILL_HAS_LEARNED,							//������ѧϰ
		EN_FIND_COUNT_NOT_ENOUGH,						//�һش�������
		EN_FIND_REWARD_ERROR,							//û���ҵ�����
		EN_NO_LEGITIMATE_TARGET,						//����û�������������ʩ��״̬
		EN_SELF_HAS_AMULET,						//���ѱ�����״̬��Ӱ��
		EN_USE_AMULET_SUCC,						// ���ѳɹ�ʹ�ø�״̬��
		EN_REQ_ALREADY_TIMEOUT,							//�����ѳ�ʱ
		EN_NEED_SKILL_LEVEL_NOT_ENOUGH,					//����Ҫ���ܵȼ�����
		EN_REWARD_HAS_NOT_GET,							//���н���û����ȡ������ʧ��
		EN_BOSS_ALREADY_DEFEAT,							//�����ѱ�����
		EN_WORLD_BOSS_NOT_ENOUGH_TEAM_MEMBER,			//����ħ������ǿ��������3�����Ӧս
		EN_WORLD_BOSS_2_NOT_ENOUGH_TEAM_MEMBER,			//�Ϲž�������ǿ��������3�����Ӧս
		EN_WORLD_BOSS_3_NOT_ENOUGH_TEAM_MEMBER,			//�Ϲ��������ǿ��������3�����Ӧս
		EN_PLATFORM_BATTLE_MAX_JOIN_NUM,				//�ѵ�������������
		EN_PLATFORM_BATTLE_CANT_CHALLENGE_RANK,			//������ս���Լ�������
		EN_MEDAL_YOU_HAVE_CERTIFICATE,					//����ѫ��֤����δ�һ�������ʧ��
		EN_MEDAL_MEMBER_HAVE_CERTIFICATE,				//�ж�Ա��ѫ��֤����δ�һ�������ʧ��
		EN_MONSTER_WAVE_CANT_PLAY,					//�����ս���ѽ���������������
		EN_CHAT_ITEM_INFO_NOT_EXIST,						//��Ʒ��Ϣ��ȡʧ��
		EN_TEAM_MEMBER_NOT_ENOUGH_ENTER_TIMES,				//�������г�Ա����ս��������
		EN_COURAGE_CHALLENGE_STAR_ENOUGH,					//��Ǹ�������������㣬��ȡʧ��
		EN_SHAN_HAI_BOSS_NOT_ENOUGH_TEAM_MEMBER,			//Boss����ǿ��������5�����Ӧս
		EN_SHAN_HAI_BOSS_NOT_CHALLENGE_COUNT,				//�����ظ���ս������´�ˢ��
		EN_MEDAL_SPECIAL_EXCHANGE_NOT_SPECIAL,				//��Ǹ,��ѡ���߲����������
		EN_LACK_AOE_AND_TREAT_PARTNER,						//��ǰȱ��Ⱥ�������ƻ�飬��ע�����ݴ���
		EN_LACK_AOE_PARTNER,								//��ǰȱ��Ⱥ����飬��ע�����ݴ���
		EN_LACK_TREAT_PARTNER,								//��ǰȱ�����ƻ�飬��ע�����ݴ���
		EN_NOT_LANTERN_IRDDIE_ANSWER_REWARD_COUNT,			//���մ��⽱������������
		EN_NOT_LANTERN_IRDDIE_USED_SPECIAL_CARD,			//ÿ��ֻ��ʹ��һ�����⿨
		EN_NOT_LANTERN_IRDDIE_ERROR_ANSWER,					//�ô��ѱ��ų�~

		//////////////////////////////////////// ��� ///////////////////////////////////////////
		EN_MARRY_TARGET_MARRIED,						//�Է��Ѿ����
		EN_MARRY_MUST_DIFFERENT_SEX_IN_TEAM,			//������һ��һŮ��Ӳſ�����
		EN_BABY_REFUSE_YOUR_REQ,						//�Է��ܾ�����
		EN_LOVER_BABY_NUM_MAX,							//�Է�����Ů��������
		EN_LOVER_BABY_CREATING,							//�Է�����������
		EN_LOVER_LEVEL_NOT_ENOUG,						//���˵ȼ�����
		EN_MARRIAGE_SEEKING_SUCCESS,					//���Գɹ�
		EN_MARRIAGE_SEEKING_MARRIED,					//���Ѿ���飬���ܽ�������
		EN_BABY_NUM_MAX,								//��Ů��������
		EN_LOVER_CAN_NOT_START_TASK,					// �Է���������
		EN_CAN_NOT_START_TASK,							// ��������
		EN_NOT_ONLY_LOVER_IN_TEAM,						// ֻ�ܺͰ�����ӿ�ʼ
		EN_CAN_NOT_AUTO_FB,								// �Ѿ����й�ɨ������������Ҫ��
			
		//////////////////////////////////////  ���ѡ������� ���    ////////////////////////////////////
		EN_FRIEND_NUM_MAX,								//���������Ѵﵽ����
		EN_FRIEND_EXIST,								//�Է��������ĺ���
		EN_CAN_NOT_MAKE_FRIEND_WITH_SELF,				//���ܼ��Լ�Ϊ����Ŷ~
		EN_ROLE_NOT_ONLINE,								//�Է�������
		EN_FRIEND_OTHER_REJECT_REQ,						//�Է��ܾ������ĺ�������
		EN_FRIEND_OTHER_FULL,							//�Է���������
		EN_FRIEND_CAN_ADD_IN_HIDEEN,							//����ﲻ�ܼӺ���
		EN_FRIEND_BLESS_NOT_FRIEND,						//�Է��������ĺ���
		EN_FRIEND_BLESS_THANK_BLESS_TIMES_ZERO,			//�������Ϊ�㣬���ܼ���������
		EN_FRIEND_REMOVE_LOVER_LIMIT,					//���ܺͷ��޽�����ѹ�ϵ
		EN_ADD_BLACK_LOVER_LIMIT,						//���ܽ����˼��������
		EN_IN_YOUR_BLACKLISTS,							//�Է������ĺ�������
		EN_BLACKLISTS_NUM_MAX,							//�������Ѵ�����
		EN_YOU_ARE_IN_BLACKLISTS,						//���ڶԷ��ĺ�������
		EN_ADD_BLACK_SHITU_LIMIT,						//���ܽ�ʦͽ���������
		EN_FRIEND_REMOVE_SHITU,							//���ܺ�ʦͽ������ѹ�ϵ
		EN_GIVE_FLOWER_TO_NO_FRIEND,					//���Ǻ��Ѳ����ͻ�
		EN_SINGLE_CHAT_ROLE_TO_BLACK,					//���Ѿ����Է�����, �޷�˽��
		EN_SINGLE_CHAT_BLACK_TO_ROLE,					//���Ѿ����Է�����, �޷�˽��
		EN_BLESS_ROLE_TO_BLACK,							//���Ѿ����Է�����, �޷�ף��
		EN_BLESS_BLACK_TO_ROLE,							//���Ѿ����Է�����, �޷�ף��
		EN_NOT_ENOUGH_FRIEND_INTIMACY,					//���ܶȲ���
		EN_TARGET_ROLE_IS_PROPOSE,						//�Է����������
		EN_FRIEND_NOT_REFRESH,							//��������̫���ˣ����������������Կ�Ŷ
		EN_FRIEND_NOT_HAS_OTHER_FRIEND,					//�����������Ƽ��ĺ��ѣ��������Եȵ�������Ŷ~

		EN_CAREER_ADVANCE_PRE_PROF_LIMIT,				//ǰ��ְҵ����ȷ
		EN_CARRER_ADVANCE_UNKNOWN_PROF,					//�Ҳ�����ְҵ
		EN_CAREER_ADVANCE_PRE_TASK_LIMIT,				//ǰ������δ���
		EN_CAREER_ADVANCE_TITLE_LIMIT,					//ǰ�óƺ�δӵ��
		EN_CAREER_ADVANCE_CAPABILITY_NOT_WORTH,			//Ŀ������δ���

		///////////////////////////////////////// ʦͽ /////////////////////////////////////////
		EN_SHITU_TARGET_NOT_FRIEND,						//��Ǹ�����ܺͷǺ��ѵ޽�ʦͽ��ϵ
		EN_SHITU_TARGET_APPRENTICE_FULL,				//�Է��Ѿ����㹻���ͽ����
		EN_SHITU_APPRENTICE_FULL,						//���Ѿ����㹻���ͽ����
		EN_SHITU_ALREADY_HAS_MASTER_FULL,					//����ʦ������
		EN_SHITU_TARGET_ALREADY_SHITU,					//�Է��Ѿ������޽���ʦͽ��ϵ
		EN_SHITU_TARGET_ALREADY_HAS_MASTER,				//�Է�ʦ���Ѿ�����
		EN_SHITU_TARGET_REJECT_REQ,						//��Ǹ���Է��ܾ����������
		EN_SHITU_ROLE_LEVEL_NOT_SATISFY,				//�Է�δ�ﵽʦ���ȼ�

		EN_CANNOT_USE_ITEM_IN_TEAM,						//����в���ʹ��
		EN_ESCORT_NOT_CAN_ENTER_FB,						//�����в��ܽ��븱��
		EN_ESCORT_NOT_CAN_JOIN_TEAM,					//�����в��ܼ������
		EN_ESCORT_OVERTIME,								//����δ�ܰ�ʱ���ڳ��˵����ھ������ջ��ڳ�

		///////////////////////////////////////// ���� /////////////////////////////////////////
		EN_FABAO_NO_THIS_FABAO,							//û���������
		EN_FABAO_HAS_ACTIVE_FABAO,						//�����Ѽ���
		EN_FABAO_NO_ACTIVE_FABAO,						//����δ����
		EN_FABAO_LEVEL_MAX_ERR,							//��������Ѿ�������߼�
		EN_FABAO_WEEK_UPLEVEL_ERR,						//���ܷ������������ﵽ����
		EN_FABAO_LEVEL_ERR,								//���Ƚ��׷���,���ܼ�������
		EN_FABAO_JIE_MAX_ERR,							//�����Ѿ��ﵽ��߽�
		EN_FABAO_LEVEL_NEED_ERR,						//����δ�ﵽ���׵ȼ�
		EN_FABAO_PASSIVE_NEED_ERR,						//�Ǳ�����������װ��������װ����
		EN_FABAO_NO_EQUIP_ERR,							//�÷���װ����δװ������
		EN_FABAO_DECOMPOSE_DRESSED_ERR,					//װ���еķ����޷��ֽ⣬�����·����ڳ���
		EN_FABAO_DECOMPOSE_NO_DATA_ERR,					//�÷����޷��ֽ�
		EN_FABAO_RAND_ATTR_ERR,							//���Ƚ��׷���,���ܼ���ϴ��

		EN_FB_STUFF_NOT_ENOUGH,							//����ȱ�ٸ������ϣ����ռ�����֮��������ս
		EN_FB_ROLE_LEVEL_NOT_ENOUTH,					//�����ȼ����㣬��ͨ��֧�����񣬻���ɾͣ��һ��ȷ�ʽ�����ȼ�

		EN_FABAO_PRE_FABAO_NO_ACTIVE_ERR,				//ǰ�÷���δ����

		EN_CHAT_FREQUENTLY,			//����̫Ƶ��
		EN_CHAT_CAN_NOT_CHAT_WITH_YOURSELF,				//���ܺ��Լ�����Ŷ~

		EN_TEAM_FIGHT_BOX_ALREADY_FETCH,				//�����ѱ���ȡ�������ظ���ȡ
		EN_BRAVE_GROUND_OVER_USE_NPC_RECOVER_TIMES,			//�ָ�Ч���Ѿ�ʹ����
		EN_BRAVE_GROUND_USE_NPC_RECOVER,			//�ָ��ɹ�
		EN_TODAY_CHALLENGE_TIMES_NOT_ENOUGH,			//������ս�������㣬����������������ս
		EN_GATHER_TIME_NOT_ENOUGH,						//�ɼ�ʱ�䲻��ɼ�ʧ��

		EN_PET_IN_PLATFORM_BATTLE,			//�������������У�����ʧ��
		EN_PET_USE_EXP_PILL,						//����������ȼ��޷�������ɫ�ȼ�Ŷ��
		EN_ATTR_HAS_FULL_ERR,						//״̬����������Ҫ�ٳ�ҩ��
		EN_TEAM_AVG_LEVEL_NOT_ENOUGH,					//����ƽ���ȼ�����
		EN_TEAM_AVG_LEVEL_TOO_HIGH,						//����ƽ���ȼ�����
		EN_TASK_QIYU_TIMES_MAX,							//����������̫������������̫��Ͳ������ˣ���������������

		EN_SCHOOL_TASK_DAILY_NUM_LIMIT,					//�����ȡ���������ѵ�����
		EN_GUILD_INVITE_NO_AUTHORITY,					//��Ҫ���ϼ�����ְλ�ſ���������
		EN_EQUIPMENT_VOC_CHANGE_LEVEL_LIMIT,			//�߼�װ�����������������ͼ�װ����
		EN_EQUIPMENT_VOC_NOT_SAME_VOC_ERR,				//һ��װ���޷�ͬʱӵ��2����ͬ������

		EN_CONVERT_SHOP_BUY_TIMES_LIMIT,				//����������Ʒ�����ɶһ��������޷��һ�
		EN_CONVERT_SHOP_RESOURCE_NOT_ENOUGH,			//������Դ���㣬�޷��һ�
		EN_CONVERT_SHOP_BUY_ITEM_NOT_SAME,				//�һ�����Ʒ�����ò�ͬ�����ص���Ϸ������������
		EN_MARKET_BUY_MY_SELLER,						//����Ʒ���Լ��ϼ���Ʒ
		EN_MARKET_SALE_EQUIP_HAS_STONE,					//���б�ʯ��װ���޷��ϼܽ�����
		EN_ACTIVITY_SKY_DROP_GATHER_ERR,				//���״̬�޷��������䣬���ɢ���鵥�˿���
		EN_ACTIVITY_CHIEF_PEAK_BATTLE_BET_NOT_IN_CROSS,	// ֻ���ڿ������ע

		EN_BOSS_CHAPTER_BOSS_NOT_FOUND,					// û���ҵ��ù�����
		EN_BOSS_CHAPTER_BOSS_NOT_KILLED,				// ��δ��ܸù�����
		EN_BOSS_CHAPTER_REWARD_CLAIMED,					// ��������ȡ
		EN_BOSS_CHAPTER_CHAPTER_BOSS_NOT_ALL_KILLED,	// ��δ��ܸ��½����й�����
		EN_BOSS_CHAPTER_BOSS_KILLED,					// �ù������ѱ����
		EN_BOSS_CHAPTER_TASK_NOT_ACCEPTED,				// ����δ�ӣ�������ս�ù�����
		EN_TASK_CHAIN_PET_ERR,							// �Ͻ�����ʧ��
		EN_TEAM_NOT_READY_ERR,							// �������������δ׼����

		EN_MEDAL_FB_OVER_MEMBER,							// �����ȶ����޷����ض���ͬ��
		EN_MEDAL_FB_LESS_MEMBER,							// ������Ϊ���գ�����3���������
		EN_SPECIAL_ADVANCE_FB_PASS_ERR,					// ��Ʒ�ռ����㣬�޷�����������
		
		EN_CAN_NOT_REPLACE_PET_SKILL,					// �и�ǿ���Ԫ�ط�������
		EN_REPEAT_PET_SKILL,					// �Ѿ�����ͬ������
		EN_RESTIGE_ERR_ITEM_LEVEL,						// ��ǰ�����а��������Ͼ���װ���ȼ���װ��������ʧ��
		EN_RESTIGE_ERR_ITEM_QUALITY,					// ��ǰ�����а��������Ͼ���װ��Ʒ�ʵ�װ��������ʧ��
		EN_RESTIGE_ERR_NOT_ENOUGH,						// ��ǰ���������Ѵ�����,����ʧ��

		EN_ITEM_REQUIRE_COLOSSEUM,						// ����Ʒ��Ҫ���ߵľ���������
		EN_PRESTIGE_NOT_ENOUGH,							// ����Ʒ��Ҫ���ߵ�����
		EN_GATHER_POWER_FULLY,							// ��������

		EN_KNAPSACK_ITEM_COLUMN_NO_SPACE,               // ������Ʒ���ռ䲻�㣬����������
		EN_KNAPSACK_SUPPLY_COLUMN_NO_SPACE,             // �����������ռ䲻�㣬����������
		EN_KNAPSACK_TASK_MATERIAL_COLUMN_NO_SPACE,      // �����������ռ䲻�㣬����������
		EN_KNAPSACK_FRAGMENT_COLUMN_NO_SPACE,           // ������Ƭ���ռ䲻�㣬����������
		EN_KNAPSACK_ILLUSTRATION_COLUMN_NO_SPACE,       // ����ͼ�����ռ䲻�㣬����������
		EN_KNAPSACK_STORAGE_COLUMN_NO_SPACE,            // �ֿ�ռ䲻�㣬��������
		EN_KNAPSACK_TEMPERARY_COLUMN_NO_SPACE,          // ��ʱ�����ռ䲻�㣬��������

		EN_BATTLE_MODE_CANNOT_GIVEUP,                   // ��ս��ģʽ�޷�����
		EN_BATTLE_ONLY_LEADER_CAN_GIVEUP,               // ֻ�жӳ����ܷ���
		EN_BATTLE_AFTER_ROUND3_CAN_GIVEUP,              // ��3�غϺ���Է���
		EN_BATTLE_PVP_CANNOT_GIVEUP,					// PVP���޷�����
		EN_BATTLE_ONLY_TEAMUP_CAN_GIVEUP,				// ֻ���������²��ܷ���
		EN_EQUIPMENT_VOC_LOCK_ERR,						// �����������޷�����
		EN_ROLE_CHAT_GROUP_MAX,							// ����Ⱥ�������ѵ�������
		EN_ROLE_CHAT_GROUP_CREATE_FAIL,					// ��������Ⱥ��ʧ��
		EN_CRATER_LEAVE_CHAT_GROUP_FAIL,				// Ⱥ�鴴�����޷��뿪����Ⱥ�飬��ʹ�ý�ɢȺ��
		EN_ROLE_CHAT_GROUP_DISMISS_RIGHT,				// Ȩ�޲��㣬�����߷��ɽ�ɢȺ��
		EN_BE_INVITE_MEMBER_WAS_IN_GROUP,				// ����������Ѿ����뱾Ⱥ
		EN_ROLE_CHAT_GROUP_KICK_RIGHT,					// Ȩ�޲��㣬�����߷����߳���Ա
		EN_ROLE_CHAT_GROUP_INVALID,						// �����Ⱥ�鲻����

		EN_BATTLE_VIDEO_NOT_FOUND,						// ���޿ɲ��ŵ�¼��
		EN_RAND_ACTIVITY_EXCHANGE_UNLOCK,				// ����δ�����޷��һ�
		EN_RAND_ACTIVITY_EXCHANGE_ITEM_ERR,				// �һ����ϲ���
		EN_RAND_ACTIVITY_EXCHANGE_TIMES_ERR,			// �һ���������
		EN_RA_TIAN_MING_DIVINATION_MAX_ADD_LOT,			// ����ǩ��ע����
		EN_EQUIPMENT_HAS_STONE_ERR,						// ����װ����ж�±�ʯ���ٽ��о���
		EN_EXHANGE_ITEM_TIME_HAS_PASSED,				// ��Ʒ�һ�ʱ���ѹ�
		EN_TEAM_AUTO_JOIN_FAIL,							// �ж�Ա���ӿ��ʧ��
		EN_COLLECTION_INCLUDED_FAIL,					// ��δ��¼�޷�����
		EN_COLLECTION_INCLUDED_AGAIN,					// �Ѿ���¼����޷��ٴ���¼
		EN_TARGET_FIGHT_PROTECT,						// �Է�����30��ս�������У��޷���ս
		EN_MY_FIGHT_PROTECT,							// ��������30��ս�������У��޷���ս
		EN_ADV_PROF_ERR_PRESTIGE_LIMIT,					// �����ȼ�����
		EN_RA_SPORTS_HISTORY_ERR,						// ��ʷ����δ�ﵽ���޷���ȡ
		EN_MAZE_RACER_FIERCE_BATTLE,					// �ܼ�ս��Ӱ�죬��������ս��
		EN_ROLE_CHAT_GROUP_FULL,						// ��Ǹ���Է�Ⱥ����������
		EN_ROLE_CHAT_GROUP_INVITE_ERR,					// �ǿ��Ⱥ�鲻�����ڿ���������
		EN_PET_CAN_NOT_SALE_EQUIP,						// ж�³���װ����ɽ���
		EN_FUNC_PREVIEW_ALREADY_CLAIMED,				// Ԥ�潱������ȡ
		EN_PET_HSA_EQUIPMENT_ERR,						// ���ﴩ�������в�����Ч
		EN_TREASURE_CHEST_IS_OPEN,						// �����ѱ�����
		EN_TASK_CHAIN_COMMIT_PET_ERROR,					// ��Ǹ���ó��ﲻ��1������ύʧ��
		EN_EXPELMONSTER_NO_REWARD_TIMES,				// ���ս����Ѵ�����
		EN_CHAT_GROUP_KICK_OUT_ERR,						// �߳����˲���ͬһ��Ⱥ��
		EN_RAND_ACTIVITY_LOGIN_COUNT_ERR,				// ��½��������
		EN_CAN_NOT_REPLACE_PET_SKILL_LOCKED,			// Ҫ�滻�ļ����Ѿ�����
		EN_FORMATION_NOT_AVAILABLE,						// ��������δ����
		EN_CUR_FORMATION_CANNOT_DELETE,					// ��ǰ����ʹ�ø����ݣ��޷�ɾ��
		EN_HEADSHOT_ADVANCE_TIMES_LIMIT,				// ��δ������״�������		
		EN_HEADSHOT_SHIZHUANG_LIMIT,					// ��δ������Ӧʱװ
		EN_TASK_CHAIN_WAIT_LIMIT,						// ��Ʒ��δ׼���ã������ĵȴ�
		EN_TEAMMANAGER_INV_ERR_DIF_GUILD,				// ��Ǹ����ͶԷ��ļ��岻һ��
		EN_CHIEF_ELECTION_SEA_CANT_ENTER,				// ����ʱ���ѹ�
		EN_MONEY_LUCK_COIN_NOT_ENOUGH,					// ����ֵ���㣬�޷����жһ�
		EN_NO_EMPTY_SLOT_FOR_NEW_FORMATION,				// ���޿�λ����������
		EN_PET_IS_TRAINNING_OPER_FAIL,					// ��������ѵ���У�����ʧ��
		EN_GUILD_FIGHT_MEMBER_GOTO_LEADER_SCENE_FAIL,	// ��ѿ������޷����͵��ӳ����ڳ���	
		EN_JOIN_TEAM_ACTIVITY_LIMIT,					// ��ѿ������޷�����ö���
		EN_SHAN_HAI_LIAN_HUA_ERROR,						// ��Ҫ���ĸ��������Ƭ1Ƭ
		EN_ACTIVITY_OPEN_OPER_FAIL,						// ��ѿ���������ʧ��
		EN_TEAMMANAGER_INV_ERR_NO_GUILD,				// ��Ŀǰ��δ������壬����ʧ��
		EN_TEAMMANAGER_INV_ERR_TAR_NO_GUILD,			// ��Ǹ���Է�Ŀǰ��δ�������
		EN_ROLE_CHAT_GROUP_IN,							// ���Ѿ��Ǹ�Ⱥ�ĵĳ�Ա��
		EN_ESCORT_CANNOT_TEAM_MATCH,					// �������ڣ��޷����ж���ƥ��
		EN_ESCORT_CANNOT_FIGHT,							// �����в��ɽ���ս��
		EN_CAN_NOT_REPLACE_PET_SKILL_LOCKED_2,			// ֻ��ѧϰһ��Ԫ�ر�������
		EN_SHAN_HAI_KEY_NOT_ENOUGH,						// ��Ǹ����ȱ����սԿ��
		EN_CHAT_INV_TIME_LIMIT,							// ��Ǹ��1������ֻ������һ��
		EN_TEAM_CREATE_SCENE_ERR,						// ������ֻ�ܴ����Լ�����Ҷ�ΪĿ��Ķ���
		EN_FB_MANAGER_ENTER_TEAM_TYPE_ERR,				// ���������븱�����Ͳ�����
		EN_CHAT_GROUP_INV_FULL_ERR,						// ��Ǹ��Ⱥ����������
		EN_CAN_NOT_MAKE_SHI_TI,							// ֻ�ܺͱ�����������ʦͽ
		EN_FUZZY_SEARCH_NOT_FOUND,						// ������ǳƻ�ID������
		EN_CHAT_ITEM_INFO_LOST,							// ������Ϣ�ѹ���
		EN_ACTIVITY_TEAM_FIGHT_ENEMY_ALL_OFFLINE,		// ��з�ս����Աȫ�����ߣ������Զ���ʤ
		EN_GUILD_HONOR_BATTLE_CANT_ENTER,			// �����볡ʱ�䲻�ܽ���
		EN_ROLE_CRYSTAL_FB_ENERGY_NOT_ENOUGH,			// ��������
		EN_CHAT_GROUP_CROSS_CANNOT_OPERATE,				// ��ǰ���ڿ��״̬���޷����е�ǰ����
		EN_CHAT_GROUP_POWER_LIMIT,						// Ȩ�޲���
		EN_ANECDOTE_COMMIT_ITEM_MISMATCH,				// �ⲻ������������ƷŶ
		EN_ANECDOTE_ALREADY_SUCC,						// �������ѳɹ���ɣ��޷��ٴγ���
		EN_ANECDOTE_TASK_NOT_ACCEPTED,					// ��������δ��ȡ������ʧ��
		EN_ANECDOTE_TASK_NOT_FINISH,					// ��δ��������޷���ȡ����
		EN_ANECDOTE_TASK_ALREADY_CLAIM_REWARD,			// ���Ž�������ȡ������ʧ��

		EN_DRAW_MANAGER_HAS_BEEN_SELECT,				// �ý����Ѿ���ѡ��
		EN_DRAW_MANAGER_SELECTING,						// �ж�Ա��δѡ���������Ժ�����
		EN_NEW_FORMATION_LIMIT_WITH_LEADER,				// �ӳ��޷����������� 
		EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT,		// �����������
		EN_RA_COW_EXCHANGE_JIFEN_LIMIT,					// δ�ﵽ�һ����ֶ�
		EN_RA_COW_LUCKY_EXCHANGE_JIFEN_LIMIT,			// ���ֲ���
		EN_RA_LITTLE_YEAR_GIFT_FAIL_NPC,				// �Ҵ�����
		EN_RA_COW_HAS_REWARD_ERR,						// ���Ѿ��ù�������
		EN_RA_COW_LUCKY_EXCHANGE_IND_LIMIT,				// ���ܶȲ���

		EN_NOW_ESCORTING_NO_MORE_ACCEPT_ESCORT_TASK,	// ��ǰ�������ڣ��޷���ȡ������

		EN_GIVE_SECRET_KEY_HAS_GET_REWARD,				// �����Ѿ���ȡ����
		EN_GIVE_SECRET_KEY_PARAM_NOT_ADV,				// ����δ��ɣ��޷���ȡ
		EN_GIVE_SECRET_KEY_NO_SPACE,					// ������������������
		EN_USE_PET_EXP_ITEM_ERR_WITHOUT_BREAK_LEVEL,	//��Ǹ����Ҫ���г���ͻ�ƺ󷽿�ʹ��
		EN_GUILD_PET_TRAINING_ERR_WITHOUT_BREAK_LEVEL,	//��Ǹ����Ҫ���г���ͻ�ƺ󷽿�ѵ��
		EN_CUMULATIVE_RECHARGE_HAS_GET_REWARD,			// �����Ѿ���ȡ����
		EN_CUMULATIVE_RECHARGE_ACC_PRICE_ERR,			// �ۼƳ�ֵ����
		EN_LEVELCOMPLEMENT_EXP_NOT_ENOUGH_ERR,			// ��Ǹ����ǰ�洢���鲻��
		EN_PARTNER_UP_LEVEL_OVER_ROLE_LEVEL,			// ��Ǹ�����ȼ�����������ɫ��ǰ�ȼ�
		EN_PET_LOOT_REWARD_HAS_FATCH_ERR,				// �����Ѿ���ȡ����
		EN_PET_LOOT_TASK_NOT_ALLOW,						// ����δ����޷���ȡ
		EN_DREAM_NOTES_CHALLENGE_NOT_CHALLENGE,			// ����ͨ��ǰ�ùؿ�����������
		EN_DREAM_NOTES_CHALLENGE_NOT_TIMES,				// �������ɨ������������ 
		EN_DREAM_NOTES_CHALLENGE_NOT_FAST_PASS,			// ͨ�ر��غ�������ؿ���ɨ��
		EN_USE_CRYSTAL_FB_ENERGY_ITEM_UPPER_LIMIT,		// �����Ѵ�����
		EN_CRYSTAL_FB_ENERGY_UPPER_LIMIT,				// �ؾ�����ֵ����
		EN_ULTIMATE_MAX_ERR,							// �Ѿ��ﵽ���ֵ
		EN_WORKSHOP_CAN_USE_THIS_EQUIP,					// ���װ���޷������������
		EN_GOD_STRONGTHEN_NULL,							// ���װ���޷�����
		EN_DREAM_NOTES_CHALLENGE_BUY_TIMES_LIMIT,		// ��Ǹ�������ն��⹺�����������
		EN_DREAM_NOTES_CHALLENGE_BUY_ERROR,				// ��Ǹ�����Ļ��Ҳ��㣬����ʧ��
		EN_WORKSHOP_HAS_BE_CREATED_ERR,					// ��װ���Ѿ���������޷��ٴ�����
		EN_GOD_UP_GREADE_NULL,							// ���װ���޷�ǿ��
		EN_NO_INS_NUM,									// �޴������Լ���
		EN_ROLE_CHAT_GROUP_ALREADY_IN,					// ��Ǹ�������ڸ�Ⱥ����
		EN_ORIGIN_SERVER_TRADE_MARKET_BUY_NOT_AVAILABLE,// ԭ������������Ʒ������ʱ�رգ��뵽����в���
		EN_ORIGIN_SERVER_TRADE_MARKET_UPSHELVE_NOT_AVAILABLE,// ԭ���ϼܽ�������Ʒ������ʱ�رգ��뵽����в���
		EN_SCHOOL_EXERCISE_UNLOCK_PREVIOUS_LIMIT,		// ͨ����һ�ѶȺ����ս
		EN_ALREADY_CATCH_PET_BEFORE,					// ��ǰ��ץȡ���������ʧ��
		EN_NO_CHECK_ITEM_NO_KNAPSACK,					// ��������������ʧ��
		EN_MIJINGQIWEN_BUY_NO_INDEX,					// ����ʧ�ܣ��޴˵���
		EN_MIJINGQIWEN_BUY_TIME_LIMIT,					// �����������
		EN_MIJINGQIWEN_ANSWER_QUESTION_ERR,				// �������
		EN_NEED_UPDATE_CFG_VER,							// �ͻ��˲������£�������������
		EN_CROSS_NOT_ALLOW_COLOSSEUM_FIGHT,				// ��Ǹ�����о�������ս��Ҫ����ԭ��
		EN_CROSS_NOT_ALLOW_COLOSSEUM_EVERY_REWARD,		// ��Ǹ��������ÿ�ս�����Ҫ����ԭ����ȡ
		EN_NEED_BOTH_IN_CROSS_TO_ADD_FRIEND,			// ��˫��ͬʱ������������ӿ�����Ϊ����
		EN_RETURN_ORIGIN_SERVER_IN_BATTLE_LIMIT,		// ��ǰ����ս���У��޷�����ԭ��������ս������������
		EN_COLOSSEUM_ACHIEVEMENT_HAS_GET,				// �ɾͽ����Ѿ���ȡ����
		EN_COLOSSEUM_ACHIEVEMENT_NO_COMPLETE,			// �ɾ�δ���
		EN_REALM_BREAK_PERFECT_SPIRITUAL_PRACTIVCE,		// ������Բ��
		EN_REALM_BREAK_BREAK_EXP_FULL_TUNA,				// ������ͻ��,Ȼ���������� 
		EN_REALM_BREAK_BREAK_EXP_FULL_USE_ITEM,			// ������ͻ��,Ȼ������ʹ��������
		EN_REALM_BREAK_BREAK_EXP_NOT_ENOUGH,			// ��Ǹ,��������ֵ����,�޷�����ͻ��
		EN_REALM_BREAK_SCENE_TYPE_FAILD_NOT_BREAK,		// ��ǰ�����޷�ͻ��,���˳��ó���֮���ٴγ���
		EN_REALM_BREAK_FETCH_EROOR,						// ��Ŭ������֮��,������ȡ
		EN_REALM_BREAK_TUNA_NUM_NOT_ENOUGH,				// ÿ��0��ˢ��֮�������ɴ���
		EN_PET_MONOPOLY_GRID_NO_REWARD,					// ���޽�����Ʒ����ȡ
		EN_PET_MONOPOLY_GRID_NO_PET_TO_CATCH,			// ���޳����ץ
		EN_MATERIAL_PASS_TIME_NOT_ENOUGH,				// ����ʣ����ս����������
		EN_MATERIAL_LEVEL_NOT_PASS_BUY_ERROR,			// ��δͨ�ر��ؿ�,�޷����������ս����
		EN_MATERIAL_LEVEL_BUY_NOT_ENOUGH,				// �ùؿ����ն��⹺������ѻ�ȡ
		EN_ROLE_ADD_EXP_EXCEED_LEVEL_LIMIT,				// �����ѳ����ȼ�����
		EN_PARTNER_PAGE_COMPOSE_ITEM_NOT_ENOUGH,		// �������㣬����������������ϳɰ�
		EN_GUILD_INBORN_PREPOSEL_SKILL_NOT_LEARN,		// ǰ�ü���δѧϰ���޷�ѧϰ�ü���
		EN_QIONG_LOU_YU_YAN_NOT_YAN_HUI,				// ��ǰ�������ʱ��
		EN_QIONG_LOU_YU_YAN_NO_EAT_TIMES,				// ���Ѿ��Ա������ٳԾ�Ҫ�Գ���~
		EN_QIONG_LOU_YU_YAN_NO_PREPARATION_TIMES,		// ���ڲ��ǳﱸ���ʱ�䣬�ύʧ��
		EN_QIONG_LOU_YU_YAN_NO_JOIN,					// ��δ����ﱸ�޷��μ����
		EN_QIONG_LOU_YU_YAN_CANNT_REWARD,				// ��δ����ﱸ���޷���ȡŶ
		EN_SHOP_BUY_ITEM_FAILED_BY_SOAR_COUNT_NOT_ENOUGH,// ����Ʒ��Ҫ��ɫ������ſɹ���


		////////////////////////////////////// ����ϵͳ /////////////////////////////////////////////
		EN_SMART_MOUNTS_SYSTEM_ACTIVE_ITEM_NOT_ENOUGH,	// �������߲���
		EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED,			// ��Ǹ,��������δ����,����ʧ��
		EN_SMART_MOUNTS_SYSTEM_SKILL_POINT_NOT_ENOUGH,	// ���ܵ㲻��
		EN_SMART_MOUNTS_SYSTEM_ENERGY_FULL,				// ��Ǹ,������ľ���ֵ����
		EN_SMART_MOUNTS_SYSTEM_PET_DOMINATION,			// �ó����ѱ�ͳ��
		EN_SMART_MOUNTS_SYSTEM_NOT_FINISH_BREACH,		// ��Ǹ,�����ﻹδ����ͻ��,�޷���������
		EN_SMART_MOUNTS_SYSTEM_LEVEL_FULL,				// ��Ǹ,������ȼ��Ѵ�����,�޷���������
		EN_SMART_MOUNTS_SYSTEM_REVEAL_LEVEL_FULL,		// ��Ǹ,������㻯�ȼ��Ѵ�����,�޷���������
		EN_SMART_MOUNTS_SYSTEM_ALREADY_BRAECH,			// ��������ͻ��,�����ظ�����
		EN_SMART_MOUNTS_SYSTEM_BREACH_FAILD,			// ͻ��ʧ��
		EN_SMART_MOUNTS_SYSTEM_UNLOCKED,				// ��Ǹ,�������ѽ���,�����ظ�����
		EN_SMART_MOUNTS_SYSTEM_LOCK,					// ��Ǹ,�ü��ܻ�δ����,����ʧ��
		EN_SMART_MOUNTS_SYSTEM_SKILL_LEVEL_FULL,		// ��Ǹ,�ü��ܵȼ��Ѵ�����,�޷���������
		EN_SMART_MOUNTS_SYSTEM_NOT_REFINE,				// û�п�ϴ���ļ��ܸ���
		EN_SMART_MOUNTS_SYSTEM_NO_REFINING,				// ��Ǹ,����δ����ϴ��,����ʧ��
		EN_SMART_MOUNTS_SYSTEM_SKILL_LEVEL_MIN,			// ��ǰ�ȼ��޷�����
		EN_SMART_MOUNTS_SYSTEM_UNLOCK_COND_NOT_MEET,	// ��������δ���
		EN_SMART_MOUNTS_SYSTEM_NATURE_LIMIT,			// �����Ѵ����ֵ
		EN_SMART_MOUNTS_SYSTEM_JEWELRY_NOT_ACTIVE,		// ��Ǹ,������û�д�����Ʒ
		EN_SMART_MOUNTS_SYSTEM_EQUIP_ACTIVED,			// ��Ǹ,��װ�����ѱ�����
		EN_SMART_MOUNTS_SYSTEM_EQUIP_NOT_ACTIVE,		// ��Ǹ,��װ������δ����
		EN_SMART_MOUNTS_SYSTEM_EQUIP_MAX_LEVEL,			// ��Ǹ,��װ��ǿ���ȼ�����
		EN_SMART_MOUNTS_SYSTEM_JEWELRY_MAX_LEVEL,		// ��Ǹ,����Ʒ�ȼ�����
		EN_SMART_MOUNTS_SYSTEM_HOLY_ACTIVED,			// ��Ǹ,�������ʥ���ѱ�����
		EN_SMART_MOUNTS_SYSTEM_HOLY_NOT_ACTIVE,			// ��Ǹ,�������ʥ����δ����
		EN_SMART_MOUNTS_SYSTEM_HOLY_MAX_LEVEL,			// ��Ǹ,�������ʥ��ǿ���ȼ�����
		EN_SMART_MOUNTS_SYSTEM_SKILL_UP_FAILED,			// ��ǰ�鼼����ȼ�����

		EN_LUNHUIWANGCHUAN_REWARD_HAS_GET,				// �����Ѿ���ȡ����
		EN_LUNHUIWANGCHUAN_HAS_PASS_GET,				// �Ѿ�ͨ�ع���
		EN_LUNHUIWANGCHUAN_NOT_PASS,					// δͨ�أ��޷���ȡ����

		EN_PET_MONOPOLY_NOT_ON_ENDING_POINT,			// ��ǰ�����յ㣬����ʧ��
		EN_PET_MONOPOLY_CUR_GRID_HAS_REWARD,			// ��ǰ���ӻ��н���Ŷ
		EN_PET_MONOPOLY_CUR_GRID_NO_TURNTABLE,			// ��ǰ����û��ת��Ŷ
		EN_PET_MONOPOLY_PET_NOT_EXPECTABLE,				// �ó��ﲻ�ڿ����б���
		EN_PET_MONOPOLY_ON_ENDING_POINT,				// ��ǰ�����յ㣬����ʧ��
		EN_PET_MONOPOLY_LACK_SPECIAL_ITEM,				// ������������
		EN_PET_MONOPOLY_NO_REWARD_GRID,					// ����û�н������滻��
			
		EN_MATERIAL_BOX_REAWRD_FETCHED,					// �ý�������ȡ,�����Ŭ��Ŷ!
		EN_MATERIAL_INTEGRAL_NOT_ENOUGH,				// ��Ǹ,���Ļ��ֲ���,��ȡʧ��
		EN_MATERIAL_CONDITION_NOT_ALL_FINISH,			// �������ɨ��������������ؿ���ɨ��
		
		EN_CRYSTAL_BUY_ENERGY_TIMES_NOT_ENOUGH,			// ʣ�๺���������	
		EN_CRYSTAL_BUY_ENERGY_REACH_LIMIT,				// �����Ѵﵽ�����ˣ���������ȥʹ�ú����������

		EN_CLOUD_ARENA_ONLY_IN_CROSS,					// ��Ǹ��񷶥֮��ֻ���ڿ���в���
		EN_PLAY_ACTIVITY_NOW_ERR,						// �����ڻ�У����Ժ����ԡ�
		EN_RELICLOOTING_CANNT_MOVE,						// ���׼���У���ʱ�޷��ƶ�
		EN_PET_MONOPOLY_TURNTABLE_GEN_FAILED,			// ת�̽�������ʧ�ܣ����Ժ�����
		EN_CLOUD_ARENA_MAX_REMAIN_FIGHT_TIMES,			// ��ս��������������Ҫ����
		EN_CLOUD_ARENA_FIGHT_TIMES_NOT_ENOUGH,			// ��ս�������㣬�޷�������ս
		EN_CLOUD_ARENA_MANUAL_REFRESH_CD_LIMIT,			// �ֶ�ˢ�¶��ֹ�����ȴ�У����Ժ�����
		EN_CLOUD_ARENA_ROLE_LEVEL_NOT_ENOUGH,			// �ù��ܽ���45����������������Ŭ����~
		EN_CLOUD_ARENA_NO_ADD_EXP_TIMES,				// �Ӿ������������
		EN_CLOUD_ARENA_RANKING_NOT_REACH,				// ��δ����ö�λ
		EN_CLOUD_ARENA_RANKING_REWARD_HAS_FETCHED,		// �ö�λ��������ȡ
		EN_CLOUD_ARENA_SETTLEMENT_LIMIT,				// ��ǰ�������������ڣ����ܽ�����սŶ~
		EN_CLOUD_ARENA_ALREADY_FETCH_ACHIVEMENT,		// �Ѿ���ȡ����ǰ�ɾͽ���
		EN_CLOUD_ARENA_ACHIEVEMENT_NOT_FETCHABLE,		// ��δ��ɸóɾͣ��޷���ȡ����
		EN_CLOUD_ARENA_ALREADY_FETCH_DAY_REWARD,		// �����Ѿ���ȡÿ�ս���
		EN_CLOUD_ARENA_DAY_REWARD_NOT_FETCHABLE,		// �ݲ�����ȡÿ�ս���
		EN_CLOUD_ARENA_BUY_FIGHT_TIMES_DAY_LIMIT,		// ����ɹ�����ս��������������

		EN_RELICLOOTING_BE_CATCH,						// �㱻ץס�ˣ��޷�����������ж���Ԯ��
		EN_RELICLOOTING_BE_TRAP,						// �㱻��ס�ˣ��޷�����
		EN_RELICLOOTING_MATH_MEMBER_TOO_MUTCH,			// ƥ����Ա̫����
		EN_RELICLOOTING_PARAM_NOT_ADV,					// ����δ��ɣ��޷���ȡ
		EN_RELICLOOTING_REWARD_HAS_GET,					// �����Ѿ���ȡ����

		EN_ITEM_REQUIRE_CLOUD_ARENA_DAN,				// ����Ʒ��Ҫ���ߵ�񷶥֮����λ
		EN_ITEM_REQUIRE_CLOUD_ARENA_RANK,				// ����Ʒ��Ҫ���ߵ�񷶥֮������

		EN_CRYSTAL_FB_NORMAL_TIMES_LIMIT,				// �ùؿ�����ʣ���������

		EN_HUAN_JIE_ZHAN_CHANG_CANNT_FIGHT,				// �н�ɫ�����쳣��ϵͳ���½���ƥ��
		EN_HUAN_JIE_ZHAN_TIAN_TI_MATCH_TOO_MANY_ROLE,	// �����������࣬�޷�����ƥ��
		EN_PLATFORM_BATTLE_IN_OTHER_PLATFORM,			// Ҫ��ս��������Ѿ���������̨
		EN_GUILD_HONOR_BATTLE_CANT_ENTER_IS_BYE,		// ����ս���ֿ�
		EN_TEAM_INVITE_LEVEL_LIMIT,						// ��ҵȼ����㣬�޷��������
		EN_MAP_EXPLORATION_HAS_CHECK,					// �����Ѿ�̽������
		EN_TODAY_HAS_BEEN_POKER_GET,					// ����ͨ�ش����Ѵ����ޣ��޷�������ȡ���ƽ���
		EN_ROLE_HEART_SKILL_NOT_REPEAT_WEAR,			// ��Ǹ,�ü�������ʹ���˸ü���
		EN_ROLE_HEART_SKILL_LEVEL_NOT_ENOUGH_UNLOCK,	// ��Ǹ,�������ĵȼ�����,�ü���������ʧ��
		EN_ROLE_HEART_SKILL_GRID_NOT_UNLOCK,			// ��Ǹ,�ü�������δ����,װ��ʧ��
		EN_ROLE_HEART_SKILL_UNLOCK,						// ��Ǹ,�ü������ѽ���
		EN_PET_HELPER_NUM_OVER_LIMIT,					// ��Ǹ��������ս������������
		EN_PET_SOUL_REFINE_ITEM_ERR,					// ����ʧ�ܣ����߲�ͬ
		EN_ROLE_TOP_LEVEL_NOT_ENOUGH_FETCH_FAILD,		// ʵ�����㣬��ʱ�޷���ȡ
		EN_KNAPSACK_GOD_PRINT_COLUMN_NO_SPACE,          // ��ӡ�����ռ䲻�㣬��������
		EN_BATTLE_DIFFICULT_MUST_MANUAL,				// ����ս���ѶȽϴ����Զ���������Ϊ�ֶ�����
		EN_CHIVALROUS_ACHIEVE_ALL_FETCHED,				// ����ֵ�ɾ���ȫ����ȡ���
		EN_HISTORY_CHIVALROUS_NOT_ENOUGH,				// �ۼ�����ֵ���㣬��ȡʧ��
		EN_CHIVALROUS_REWARD_ALREADY_FETCHED,			// �ý�������ȡ
		EN_SINGLE_CHAT_IS_LIMIT,						// ��ǰ�ȼ����ͣ�˽�������Ѵ�����
		EN_WORKSHOP_ACTVIE_VOC_MAX_NUM,					// ��������Ѿ��ﵽ�������
		EN_GOD_EQUIP_NO_LEVEL,							// ��װ�ȼ�����
		EN_CLOUD_ARENA_SHOP_ERROR,						// 񷶥�̵���Ʒ�������Ժ�����
		EN_PET_STRENGTHEN_PROTECT_CANNT_USE,						// ��ǰǿ������ʹ��ǿ����������
		EN_PET_STRENGTHEN_PROTECT_ITEM_NOT_ENOUGH,				// ����ǿ���������߲���
		EN_HAS_ORDER,				// ������һ������δ��ɣ���ȴ�2���Ӻ�����
		EN_GUILD_ACTIVE_DAY_ACTIVE_LIMIT,				// �����ջ�Ծ�Ȳ���,�޷���ȡ�����
		EN_INSTRUCTION_FULL,							// �Զ���ָ����������
		EN_MAZE_RACER_ALREADY_GET_REWARD,				// ����ͨ���޷�������ý���
		EN_MAZE_RACER_NEW_PASS_TIME,				// ��ϲ������ͨ�ش������Լ����λ����Ѽ�¼
		EN_NOT_PASS_CRYSTAL_FB,							//δͨ���޷���ȡ
		EN_PASS_CRYSTAL_FB_REWARD_HAS_GET,				//ͨ�ؽ����Ѿ���ȡ��
		EN_SHOP_LOCK_NOT_CAN_BUY_ITEM,					//δ���������ɹ���

		EN_GOD_PRINT_EXP_NOT_ENOUGH,					// ���鲻��
		EN_PET_WEAR_GOD_PRINT,							// ���ﴩ����ӡ��
		EN_PET_NO_WEAR_GOD_PRINT,						// ����û�д�����ӡ
		EN_PET_GOD_PRINT_HAS_LOCK,						// ������ӡ������
		EN_FB_NOT_PASS,									// ��һ�ؿ�δͨ��
		EN_NETHER_TASK_IS_NOT_COMPLETED,				// ������δ���
		EN_NETHER_TASK_REWARD_HAS_GET,					// �����Ѿ���ȡ����
		EN_NETHER_TASK_NOT_OPEN,						// �ؿ���δ����
		EN_CRYSTAL_PASS_LEVEL_NOT_HAS_CONSUME,			// ����û�����ķ�ӡʯ������ͨ���޷���ȡ���ƽ���


		EN_HONG_BAO_GET_LIMIT_FOR_HIDDEN,				// ��Ǹ����ȡ���������Ҫ����ԭ����ȡ
		EN_HONG_BAO_SEED_LIMIT_FOR_HIDDEN,				// ��Ǹ�����ͺ����Ҫ����ԭ������
		EN_HONG_BAO_INVALID_TIMESTAMP,					// �������ˣ�����ѹ���
		EN_HONG_BAO_OVER,								// �������ˣ�����ѱ�����
		EN_HONG_BAO_QUIT_GUILD_GET_LIMIT,				// ���ձ�����µ��������Ϣ��������ȡ����ɡ�
		EN_HONG_BAO_QUIT_GUILD_SEED_LIMIT,				// ���ձ�����µ��������Ϣ������������ɡ�
		EN_HONG_BAO_TYPE_NOT_TIMES,						// �����ͺ������ , �޷����ͺ��
		EN_HONG_BAO_GET_LIMIT,							// ���Ѿ���ȡ���ú���� , �����ظ���ȡ

		EN_PET_GOD_FB_NOT_PASS,							// ��ǰ�ؿ�δ����������ͨ����һ��
		EN_KNAPSACK_PET_SOUL_EQUIP_COLUMN_NO_SPACE,		// ��ӡ�����ռ䲻��
		EN_NETHER_WORLD_PURGATORY_MILING_NO_TIMES,		// ��ǰ�����������㣬�޷�����
		EN_PET_WEAR_SOUL_EQUIP,							// ���ﴩ��������
		EN_EQUIP_HAS_STONE_ERR,							// ��װ������Ƕ��ʯ�޷��ֽ�
		EN_EQUIP_HAS_ZHULING_ERR,						// ��װ���������޷��ֽ�
		EN_JEWERLY_HAS_STONE_ERR,						// ����������Ƕ��ʯ�޷��ֽ�
		EN_JEWERLY_HAS_ZHULING_ERR,						// �������������޷��ֽ�
		EN_ACTIVITY_BIG_DIPPER_NO_DATA,					// δ֪�Ǿ���۾���Ϣ
		EN_ACTIVITY_BIG_DIPPER_NO_MONSTER,				// ���Ǿ���������ս
		EN_ACTIVITY_BIG_DIPPER_NO_BOSS,					// �˵۾���������ս
		EN_ACTIVITY_BIG_DIPPER_NO_PASS_LEVEL,			// δ�ﵽ�����׶�
		EN_ACTIVITY_BIG_DIPPER_HAS_REWARD,				// �����Ѿ���ȡ����
		EN_ACTIVITY_BIG_DIPPER_NO_PASS,					// ��δͨ��
		EN_TODAY_VILLAGE_ALREADY_SELECT,				// ������ѡ���˴�ׯ���޷��ٴ�ѡ��
		EN_VILLAGE_NOT_FOUND,							// δ�ҵ��ô�ׯ
		EN_VILLAGE_NO_TASK_FOUND,						// �ô�ׯδ��������
		EN_PARTNER_GIFT_TREE_LEVEL_LIMIT,				// ǰ���츳δ����
		EN_PRESTIGE_TASK_ACCEPT_FAILED,					// ��ȡ��������ʧ��
		EN_BRAVE_GROUND_WORLD_LEVEL_NOT_OPEN,			// ��ǰ����ȼ��´��ع�δ����
		EN_FANG_FRAUD_ERROR,							// ����������ѡ����ȷ������
		EN_ROLE_ATTR_PELLET_USE_LIMIT,					// ���Ե��Ѵﵽ�������Ե�ʹ�����ޣ�ʹ��ʧ��

		EN_MAZE_SUMMER_NOT_UNLOCK,						// �õ�ͼδ�����޷�ǰ��
		EN_MAZE_SUMMER_DOING_MAP,						// ������ս���޷��ƶ�
		EN_MAZE_SUMMER_REWARD_HAS_GET,					// �����Ѿ���ȡ����

		EN_PARTNER_PACT_CANT_SELF,						// �޷���Լ�������Ԫ��
		EN_PARTNER_PACT_CANT_SAME,						// ����Լ�û��Ԫ�������ظ���Լ
		EN_PARTNER_PACT_MAX,							// �û��Ԫ���ѱ���Լ2�Σ�������Լ������
		EN_PARTNER_BE_PACT_UP,							// ��Ԫ����Լ������Զ�Ӧ����

		EN_TITLE_HAS_KILL_VALUE,						// ����ǰ����ս��ֵ����ʱ�޷�����ƺ�
		EN_TITLE_KILL_VALUE_TITLE_CHANGE,				// ����ǰ����ս��ֵ����ǿ���滻�ƺ�
		EN_DUEL_ITEM_NOT_ENOUGH,						// �������
		EN_DUEL_NOT_HAS_KILL_VALUE,						// ��ǰû��ս��ֵ���޷�ʹ�����¼
		EN_DUEL_SCENE_CAN_NOT_DUEL,						// ��ǰ�����޷���������ȥ��Ұ�ⳡ��
		EN_DUEL_MINE_CAN_NOT_DUEL,						// ��ǰ�ȼ����㣬�޷�����
		EN_DUEL_TARGET_CAN_NOT_DUEL,					// �Է��ȼ����㣬�޷�����
		EN_DUEL_TEAM_IS_RED_NAME,						// ��ǰ������ں�����ң������⵽�������׷ɱ!
		EN_DUEL_TEAM_NOT_ALLOW_RED_JOIN,				// �ö���ܾ�����������
		EN_DUEL_TEAM_HAS_RED,							// �ö����Ѿ����ں�����ң��޸�ʧ��
		EN_RA_HAPPY_LOOK_NOT_FETCH_SUCC,				// ����ȡ���ı����ٽ�����һ����սŶ~
		EN_RA_HAPPY_LOOK_NOT_FETCH_FAIL,				// ����ȡ��������ٽ�����һ����սŶ~
		EN_GUILD_HONOR_BATTLE_CANT_ENTER_2,				// ���ǲ�����Ա���ܽ���
		EN_PET_NOT_OP_BY_BENEDICTION,					// ��ǰ��������Ϊף�����裬����ʧ��
		EN_PET_CANCEL_HUA_SHEN_PET_STATUS,				// ��Ϊ���������ṩף��Ч�����޷�չʾ���ﻯ��
		EN_YIN_YANG_EYE_PLEASE_OPEN,					// ��������δ����
		EN_KNAPSACK_ACCESSORIES_COLUMN_NO_SPACE,		// ���α����ռ䲻��
		EN_MEDAL_EXCHANGE_NOT_SAME_SP,					// �޷�ת��ͬ������
		EN_BREAK_SOUL_FIGHT_OLD_NOT_PASS,				// ǰ�ùؿ�δͨ��
		EN_COMPLETE_ANECDOTE_NUM_LIMIT,					// �������������������
		EN_BIG_EVENT_NOT_OPEN,							// ���ڴ��¼����Ž׶�
		EN_BIG_EVENT_REWARD_ALREADY_FETCHED,			// �ý�������ȡ
		EN_COURSE_LEVEL_NOT_ENOUGH_FETCH_FAILED,		// ���̵ȼ�����,�޷���ȡ
		EN_RA_SHAN_HAI_MEET_NOT_BUY,					// ɽ�����δ����
		EN_RA_SHAN_HAI_MEET_CAN_NOT_POKER,				// δ����ɽ����������ɽ��з���
		EN_RA_SHAN_HAI_MEET_NOT_POKER_ALL,				// ���ȷ���������
		EN_DAY_REWARD_DAY_FOOD_FETCH_NOT_TIME,			// ��δ��ʱ��,�޷���ȡ
		EN_DAY_REWARD_DAY_FOOD_NOT_MAKE_FETCH,			// ��Ǹ,�ý������ɲ���
		EN_DAY_REWARD_DAY_FOOD_FETCH_TIME_OUT,			// �����ѽ���
		EN_TARGET_HAS_JIE_YIE_ZU,						// �Է��Ѿ��н���
		EN_TARGET_JIE_YI_OFFLINE_SHORT,					// �Է����߲������������߳�
		EN_TARGET_JIE_YI_ONLINE,						// �Է������У������߳�
		EN_JIE_YI_QIN_MI_DU_BU_ZU,						// �ӳ��Ͷ�Ա�����ܶȲ���
		EN_NOW_ESCORTING_NO_START_CHANGE,				// ���������У�������ս�����Ժ�����Ŷ~
		EN_GATHER_FAILED_BY_SLOW,						// ��ѽ����������һ��ժ����
		EN_GATHER_POWER_NOT_ENOUGH,						// ��Ǹ,���Ļ���ֵ����
		EN_GATHER_FAIL_BY_DUEL,							// ��ժʧ��
		EN_PARTNER_SOAR_FAIL_BY_LEVEL_NOT_ENOUGH,		// ��Ǹ,�û��ȼ�����,����ʧ��
		EN_PARTNER_SOAR_ARMS_LEVEL_UP_FAIL_BY_NOT_SOAR,	// ��Ǹ,�û�黹δ����,����������δ����
		EN_PARTNER_SOAR_FAIL_BY_SOAR_TASK_NOT_FINISH,	// ��Ǹ,�û�����������δ���,����ʧ��
		EN_PARTNER_SOAR_ARMS_LEVEL_UP_FAIL_LEVEL_LIMIT,	// ��Ǹ,�û����������ȼ��Ѵ�����,ǿ��ʧ��
		EN_PARTNER_SOAR_FAIL_BY_SCORE_NOT_ENOUGH,		// ��Ǹ,�û������δ���,����ʧ��
		EN_PARTNER_SOAR_FAIL_BY_QUALITY_LIMIT,			// ��Ǹ,�û�����Ʒ������,����ʧ��
		EN_PARTNER_UP_GRADE_FAIL_BY_MAX_LIMIT,			// ��Ǹ,�û�����������ǰ����,����ʧ��
		EN_PARTNER_SOAR_FAIL_BY_UPGRADE_LEVEL_LIMIT,	// ��Ǹ,�û����׽�������,����ʧ��
		EN_ROLE_FLY_UP_STAG_NOT_ENOUGH,					// ��Ǹ,���Ľ�ɫ��ǰ�����׶λ������Խ����ùؿ�
		EN_HAS_DIANHUASHI_ERR,							// ��Ǹ,�õ��߱��㻯���޷��ϼ�
		EN_FLY_UP_LEVEL_ERR,							// ��Ҫ��ɸ߽׷����ſɴ���
		EN_PARTNER_SOAR_ACTIVATION_FAILED,				// ��Ǹ,�û�����Ʒ������,����ʧ��

		// ��ţ��ϲ,�����ɸ�
		EN_COW_REPORT_GOOD_NEWS_CAN_NOT_BUY,			// ��Ǹ,�Ѵ���ɹ���ʱ��
		EN_COW_REPORT_GOOD_NEWS_BUY_TIMES_LIMIT,		// ��Ǹ,���Ѿ��������,�����ظ�����
		EN_COW_REPORT_GOOD_NEWS_IS_NOT_SHOW_TIME,		// ��Ǹ,���ڲ��ǻ�����չʾ��
		EN_RA_ZHIGOU_NOT_BUY,							// Ȩ��δ����޷���ȡ����
		
		EN_WAN_LING_FB_BATTLE_MAX,						// ����ͭ�ҽ�����ȡ���Ѵﵽ����
		EN_WAN_LING_FB_PASS_MAX,						// ����ͨ�ؽ�����ȡ���Ѵﵽ����
		EN_CLOUD_ARENA_MANUAL_REFRESH_TIMES_LIMIT,		// �����ֶ�ˢ�´���������

		EN_ACTIVITY_SILK_ROADS_TRADING_LOG_ALL_SHOW,	// ����˿��֮·����Ϣ��ȫ����ʾ���
		EN_ACTIVITY_SILK_ROADS_SUPPLY_NOT_ENOUGH,		// ��Ǹ,���Ĳ�������,�޷�ǰ����һ������
		EN_ACTIVITY_SILK_ROADS_SALE_COMMODITY_FAILED,	// ��Ǹ,���Ļ��ﲻ��,����ʧ��
		EN_ACTIVITY_SILK_ROADS_BUY_TRADING_LOG_LIMIT,	// ��Ǹ,�ó���������Ϣ���Դ�̽
		EN_ACTIVITY_SILK_ROADS_MONEY_NOT_ENOUGH,		// ��Ǹ,���Ļ��Ҳ���
		EN_ACTIVITY_SILK_ROADS_WAREHOUSE_EXP_LIMIT,		// ��Ǹ,�ֿ������Ѵ�����
		EN_ACTIVITY_SILK_ROADS_BUY_SUPPLY_LIMIT,		// ��Ǹ,������������Ѵ�����
		EN_ACTIVITY_SILK_ROADS_FINISH_NUM_LIMIT,		// ��Ǹ,������ɴ����Ѵ�����
		EN_ACTIVITY_SILK_ROADS_NOT_ADVANCE_END,			// ��Ǹ,�������㹻�Ĳ�����,������ǰ����
		EN_ACTIVITY_SILK_ROADS_ACTIVITY_END,			// ˿��֮·��ѽ����������ѷ��������䣬�����
	
		EN_PET_FEI_SHENG_TIMES_NOT_ENOUGH,				// ��������ȼ�����
		EN_RA_CHAO_ZHI_GIFT,							// ����δ���ֵ�������ȼ���
		EN_WORLD_ARENA_ALREADY_SIGNED_UP,				// ����ɱ��������������������ʼ�����ؽ���֪ͨ

		EN_STAR_UP_ERR_NOT_SAME_STAR_LEVEL,				// �Ǽ���һ��
		EN_STAR_UP_ERR_NOT_SAME_LEVEL,					// �ȼ���һ��
		EN_STAR_UP_ERR_NOT_SAME_QUALITY,				// Ʒ�ʲ�һ��
		EN_STAR_UP_ERR_NOT_SAME_PROF,					// ְҵ��ͬ
		EN_STAR_UP_ERR_NOT_SAME_ITEM_TYPE,				// ����ͬ��װ��

		EN_FLY_UP_ROLE_CAPBILITY_ERR,					// ������ֲ���
		EN_PET_TWO_MUTANT_NEED_FEI_SHENG,				// ���α�����Ҫ�������

		EN_NEW_LIFE_SKILL_LEVEL_LIMIT_BY_GUILD_LEVEL,	//��ǰ���ܵȼ��Ѵ��������ܵȼ�����
		EN_NEW_LIFE_PROF_LEVEL_LIMIT_BY_GUILD_LEVEL,	//��ǰְҵ�ȼ��Ѵ��������ܵȼ�����
		
		EN_FRIEND_BY_CROSS_NOT_CONNECTED ,				// ���ѹ�����ʱ�޷�ʹ�� , ���Ժ�����

		EN_EQUIP_JEWELRY_UP_STAR_LEVEL_ERR,				//װ���������ʯ�޷�����
		EN_NOT_LEADER_CANNT_MATH_REQ_ERR,				//��ȴ��ӳ���ʼƥ��
		EN_PET_NEED_FEI_SHENG,							//��Ҫ�������
		EN_NEED_ROLE_FEI_SHENG,							//��ɫ����������
		EN_TEAM_NEED_ROLE_FEI_SHENG,					//����������ҷ���������
		EN_NEED_PET_STR_LEVEL,							//����ǿ���ȼ�������
		EN_NEED_PET_CAPBILITY_LEVEL,					//����ս���ȼ�������
		EN_NOT_IN_PET_LIST,								//���ﲻ���б��У��޷���ɷ�������
		EN_PET_TASK_NOT_MATCH,							//��������׶κ�����ƥ�䣬�޷���ɷ�������
		EN_NEED_ROLE_FLY_UP,							//��ɽ�ɫ����������ս
		EN_FUNOPEN_SERVER_OPEN_ERR,						//�淨��δ���������������ĵȴ�
		EN_SHI_TU_BREAK_NOT_ENOUGH_DAY,					//ʦͽ��ϵ����30��

		EN_RA_YINHU_WENSHI_NOT_BUY,						// ����δ������������Ŷ~
		EN_RA_ACTIVITY_DAY_NOT,							// ������������
		EN_RA_CINIU_YINGXINGHU_NOT_AWAKEN,				// δ�ﵽ���ѽ׶�
		EN_RA_CINIU_YINGXINGHU_AWAKEN,					// �ѻ���
		EN_RA_HU_XING_DA_YUN_NOT_SERVER,				// �������ĺ��Ѳ���ͬһ������
		EN_RA_HU_XING_DA_YUN_NOT_INVITED,				// ��������ͬ����
		EN_RA_HU_XING_DA_YUN_INVITED_TIMES,				// ����ĺ��Ѵ�������
		EN_RA_HU_XING_DA_YUN_TASK_END,					// �����Ѿ����յ�
		EN_RA_YIN_HU_BAI_SUI_NOT_BUY,					// ����δ������������Ŷ~

		EN_WEDDING_REGIST_FULL,							//������ԤԼ,������Ч
		EN_WEDDING_REGIST_SUCC,							//�ɹ�ԤԼ����
		EN_WEDDING_REGIST_OFF,							//����ԤԼ��ȡ��
		EN_WEDDING_REGIST_TOO_NEAR,						//����ԤԼʱ�����
		EN_WEDDING_REGIST_OVER,							//��ʱ���ѱ�ԤԼ
		EN_WEDDING_NULL,								//������ʧЧ
		EN_WEDDING_INVITE_SUCC,							//�ɹ�����Է��μӻ���
		EN_WEDDING_BE_INVITED,							//�Ѿ�������������
		EN_WEDDING_INVITED_LIST_FULL,					//�����б�����
		EN_WEDDING_APPLY_SEND,							//������������ѷ���
		EN_WEDDING_APPLY_WAIT,							//�����������û���, �ȴ������ߴ���
		EN_WEDDING_WAIT_LINK,							//���紦��Ԥ�Ƚ׶�, ���Ժ�
		EN_WEDDING_MEAL_MAX,							//�òʹ����Ѵ�����
		EN_WEDDING_MEAL_FAST,							//�ò͹���Ƶ��, ���Ժ�
		EN_WEDDING_SUB_ACT_FINISH,						//��ǰ����ȫ����ɣ���ȴ���һ������
		EN_WEDDING_XUN_YOU_BUY_SUCC,					//�ѹ��򻨳�����
		EN_WEDDING_XUN_YOU_HAS_BUY,						//�޷��ظ�����
		EN_NEW_LIFE_SKILL_SURFACE_COUNT_NOT_ENOUGH,		//��Ǹ,���Ļû��㲻��,�û�ʧ��
		EN_NEW_LIFE_SKILL_PERFORM_SKILL_ERROR,			//��Ǹ,���Ļû��㲻��,�޷��ͷŸü���
		EN_NEW_LIFE_SKILL_NOT_HAS_CHANGE_CARD,			//��Ǹ,��û��ʹ�ûû���
		EN_FALLEN_GOD_MEMBER_NOT_PASS_PRE_SN,			//�ж�Աδ�߱���ս�ʸ��޷����Ѹû���
		EN_MINI_GAME_REWARD_ALREADY_FETCHED,			//��������ȡ
		EN_MINI_GAME_PHASE_REWARD_CLEAR_STAGE_NUM_LIMIT,//��δͨ��ָ���ؿ�
		EN_RELICLOOTING_HAS_ITEM_SKILL,					//��ǰ��������ʹ�õĵ��ߣ���ʱ�޷�ʰȡ

		EN_WORLD_ARENA_NOT_IN_TIME_TO_ENTER_SCENE,		//��ǰ���ڽ���ʱ����
		EN_WORLD_ARENA_NOT_IN_BATTLE_STATUS,			//��ǰ����ս���׶�
		EN_WORLD_ARENA_UNABLE_ENTER_SCENE,				//�����������޷�����������ϵ�ͷ�
		EN_WORLD_ARENA_ENTER_SCENE_FAILED,				//����ʧ�ܣ����Ժ�����
		EN_WORLD_ARENA_WIN_NUM_NOT_ENOUGH,				//ʤ�����㣬�޷���ȡ�ý���
		EN_WORLD_ARENA_WIN_BOX_ALREADY_FETCHED,			//�ý�������ȡ���޷��ظ���ȡ
		EN_WORLD_ARENA_IN_SIGNUP_STATUS_LIMIT,			//���ڱ����׶Σ�����ʧ��
		EN_WORLD_ARENA_HAS_FETCH_SIGNUP_REWARD,			//������������ȡ���޷��ظ���ȡ
		EN_WORLD_ARENA_DAILY_REWARD_FETCH_FAIL,			//ÿ�ս���������ȡ������ȡ

		EN_RELICLOOTING_TEAM_TYPE_ERR,					//��Ǹ������ǰ�Ķ���Ŀ�����淨Ŀ�겻һ��
		EN_RELICLOOTING_TEAM_ADD_ERR,					//����ƥ�����޷�����
		EN_WORLD_QUESTION_ANS_ERR,						//�ش���������Ŭ���ɣ���

		EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_NAME,				//��Ǹ,ս�����ƴ��������ַ�
		EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_MEMBER,			//��Ǹ,��Ҫ���5�˶���ſɽ���ս�ӱ���
		EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_TIME,				//��Ǹ,��ǰ��ʱ�޷�����
		EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_MEMBER_SIGN_UP,	//��Ǹ,�������г�Ա�Ѽ���ս��
		EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_MONEY_NOT_ENOUGH,	//��Ǹ,�ӳ�δӵ���㹻�ı�����
		EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_MEMBER_CHANGE,	//��Ǹ,��Ա��Ϣ�����ı�,ս�ӱ���ʧ��
		EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_NAME_REPEAT,		//��Ǹ��������ս�������ظ�
		EN_WORLD_TEAM_ARENA_ENTER_SCENE_FAILED_BY_NOT_HAS_TEAM,	//��Ǹ������Ҫ�������󷽿ɽ�������
		EN_WORLD_TEAM_ARENA_FAILED_TEAM_NOT_SAME_TEAM,			//��Ǹ�������ڱ����Ϊͬһս�ӳ�Ա
		EN_WORLD_TEAM_ARENA_NOT_HAS_TEAM,						//��Ǹ�����ѱ�����ս�ӿɽ��в���
		EN_GUILD_QIFU_NOT_OPEN,									//��������δ����
		EN_GUILD_QIFU_VALUE_NOT_ENOUGH,							//������δ����
		EN_TASK_CANNT_SKIP,										//�������޷�����
		EN_ENTRY_COIN_USE_TIMES_NOT_ENOUGH,						//����ͭ��������������
		EN_PARTNER_FEI_SHENG_TIMES_NOT_ENOUGH,					//�������ȼ�����
		EN_PET_BADGE_TAKE_OFF_FAIL,								//�����������ʧ��
		EN_YAO_XING_SHI_LIAN_PASS_ERR,					// �ɾ�δ���
		EN_RELICLOOTING_IN_MATCH_ERR,					// ����ƥ���У��޷�����
		EN_DONATE_ERR_HAS_ZHU_LING_ERR,							//����װ����ж�������پ���


		EN_SHI_TU_MILE_FETCH_LIMIT,						//��Ǹ������ʦͽ��̽�����ȡ�����Ѵﵽ����


		EN_PET_TALENT_NOT_REPEAT_ACTIVED,						//���츳���Ѽ���
		EN_PET_TALENT_NOT_YET_ACTIVE,							//���츳����δ����
		EN_PET_TALENT_NOT_RESET_NOT_TRADE,						//�����츳δ���ã��޷����н���

		EN_SHITU_CHUAN_GONG_TEAM_FULL,					//ʦͽ����, ��Ҫʦͽ���˲���
		EN_SHITU_CHUAN_GONG_TEAM_LEADER,				//ʦͽ����, ��Ҫ���Ӵ��ӽ���
		EN_LEVEL_COMPLEMENT_BUY_EXP_COUNT_NOT_ENOUGH,	//����������չ����������
		EN_LEVEL_COMPLEMENT_NOT_HAS_EXP_BUY_FAILED,		//��ǰ���������Ѿ�ȫ������ˣ����տ��Թ���Ŷ

		EN_QINGYUAN_DUIDUIPNEG_PEIDUI_NOT,				//��֤ʧ�ܣ����ٽ�����

		EN_YAO_XING_SHI_LIAN_CAP_NOT_ENOUGH,			// ս������
		EN_TEAM_MEMBEI_NOT_CANENTER_ITEM,				// ��ǰ��������Ե�ͬ�Ľ�
		EN_QING_YUAN_SHENG_HUI_QUESTION_FINISH,			// �˻��ڴ����Ѿ�����
		EN_QINGYUAN_DUIDUIPENG_LEAVE_SCENE,				// ������������뿪��������Ϸʧ��
		EN_SHITU_CHUAN_GONG_BREAK_OFF,					// �Է��˳������������ж�
		EN_RA_QIYUAN_KONGMINGDENG_NOT_ITEM,				// ���ֲ���
		EN_RA_XUNMENG_HUDIE_NOT_NPC,					// �����Ѿ�������ץ����
		EN_RA_ACTIVITY_NOT_TIMES,						// ���������
		EN_ZAO_HUA_KNAPSACK_NO_SPACE,					// �컯�����ռ䲻��
		EN_ZAO_HUA_NO_WEAR_EQUIP,						// δ����װ��
		EN_ZAO_HUA_WEAR_ERR_SUIT,						// �컯װ��δ������װ
		EN_ZAO_HUA_APP_HAS_ACTIVE,						// ����Ѿ��������
		EN_ZAO_HUA_APP_NOT_ACTIVE,						// �����δ����
		EN_ZAO_HUA_SUIT_LEVEL_LIMIT,					// ��װ�ȼ�����

		EN_UTA_TEAM_NOT_FOUND,							//δ�ҵ��ö�����Ϣ
		EN_UTA_MY_TEAM_DONT_HAVE_QUALIFICATION,			//��Ǹ������ս��δ����ʸ���̭������
		EN_UTA_NO_SCHEDULE_YET,							//��Ǹ����δ���ʸ���̭������

		EN_UTA_GUESS_BET_REPETITION_ERROR ,				//�ö������Ѿ���ע����
		EN_UTA_WIN_BOX_FETCH_LIMIT,						//�ñ����ݲ�����Ҫ�󣬻�����ȡ
		EN_UTA_NO_MAIN_SCHEDULE_YET,					//��Ǹ����δ�н���������
		EN_UTA_NO_ROUND_MAIN_SCHEDULE_YET,				//��Ǹ���˽׶�������δ����
		EN_UTA_NO_FINAL_LIST_YET,						//��Ǹ�����հ���δ����
		EN_UTA_ONCE_LOTTERY_LIMIT,						//�Ѵﵥ����ע����

		EN_UTA_NOT_IN_ENTER_SCENE_TIME,					//��ǰ���ڿɽ���ʱ����
		EN_UTA_SCENE_NOT_FOUND,							//��Ǹ���Ҳ�������ս������
		EN_UTA_ENTER_SCENE_FAILED,						//����ʧ�ܣ����Ժ�����
		EN_UTA_TEAM_ENTER_SCENE_FAILED,					//���Ķ������ʧ�ܣ�������Ա���ã�ȷ����Ա�����ڿɽ���״̬

		EN_UTA_SCHEDULE_ALREADY_HAS_RESULT,				//����ս�����н���������ظ�ս��
		EN_UTA_OB_DATA_EMPTY,							//��δ�б���ս�������ݻش������޷���ս
		EN_UTA_OB_FIGHT_ALREADY_END,					//����ս���ѽ��������Ժ��Թۿ�¼��
		EN_UTA_OB_IN_TEAM_LIMIT,						//��ǰ�ڶ����У����˶ӻ����������
		EN_BIGCROSS_GM_OPER_SUCC,						//����GM��������ɹ�
		EN_BIGCROSS_GM_OPER_FAIL,						//����GM�������ʧ��ʧ��ʧ��

		EN_UTA_GUESS_BET_ERROR,							//��ǰ�׶β�������ע
		EN_UTA_LAST_SEASON_OVERVIEW_NOT_AVALIABLE,		//�����Ͻ���������

		EN_ROLE_HUNDRED_GOD_BOSS_FINISH_NOT_FETCH,		//�ù��������
		EN_ROLE_HUNDRED_GOD_BOSS_END,					//�ù����ѽ���
		EN_ROLE_HUNDRED_GOD_BOSS_TARGET_NOT_FINISH,		//��Ŀ��δ���

		EN_AUTHORITY_TYPE_TEST_DAY_BUY_LIMIT,			//���ս����Ѵ�����

		EN_RA_GOD_BEAST_ADVENT_UNLOCK_REWARD,			// �øж�δ�ﵽ,�����Զһ�
		EN_RA_GOD_BEAST_ADVENT_EXCHANGE_LIMIT,			// �ﵽ�ɶһ�����,���ɶһ�
		EN_RA_LOGIN_DAY_NOT,							// ��¼��������
		EN_RA_YUANQI_JISHIBU_NOT_BUY_JINJIE,			// ���׽���δ����
		EN_TRADEMARKET_UPSHELVE_TIME_LIMIT,				// ����Ʒʣ����Чʱ�䲻�㣬���ɳ���

		EN_HOLY_EQUIP_BAG_NO_SPACE,						//ʥ�������ռ䲻��
		EN_FU_SHI_BAG_NO_SPACE,							//��ʯ�����ռ䲻��
		EN_HOLY_EQUIP_STANDBY_MAX,						//��ս�����ʥ��λ������
		EN_HOLY_EQUIP_SKILL_HOLE_LOCK,					//���ܿ�λδ����
		EN_HOLY_EQUIP_FORE_SKILL_HOLE_UNLOCK,			//���Ƚ���ǰ��ļ��ܿ�λ
		EN_HOLY_EQUIP_NO_SKILLS_TO_SAVE,				//û�пɱ���ļ���
		EN_HOLY_EQUIP_HAVE_SAME_TYPE_FUSHI,				//������Ƕͬ���ͷ�ʯ
		EN_HOLY_EQUIP_FUSHI_LEVEL_NOT_SAME,				//�����ȼ���ͬ�ķ�ʯ
		EN_HOLY_EQUIP_FUSHI_LEVEL_ERROR,				//��ʯ�ȼ�����������
		EN_HOLY_EQUIP_CAN_NOT_ADD_FINGHT_STANDBY,		//��ս�����״̬ʥ��������Ϊ����
		EN_HONGMENG_TIANJIE_NOT_REWARD_TIMES,			//��ǰ�޽���������������ս���޷���ý���
		EN_GOTO_COMMON_SCENE_NOT,						//���뿪�������ٳ���
		EN_HOLY_EQUIP_LEVEL_NOT_ENOUGH,					//ʥ���ȼ�����
		EN_HOLY_EQUIP_PLEASE_UNLOCK_LOWER_LEVEL_HOLE,	//���Ƚ������͵ȼ��ķ�ʯ��
		EN_HOLY_EQUIP_CAN_NOT_BLESS_PET,				//ֻ�г�ս�����ʥ���ſ���ʥ�ӳ���
		EN_HOLY_EQUIP_BLESS_PET_ONE_ON_ONE,				//һ��ʥ��ֻ��ʥ��һֻ����
		EN_HOLY_EQUIP_NOT_REPEATABLE_STUDY,				//��ʥ����ӵ�д˼��ܣ��޷��ٴ�ѧϰ
		EN_HOLY_EQUIP_HAS_NO_SAVE_SKILL,				//��ʥ������δ����ļ���
		EN_HOLY_EQUIP_QUALITY_TOO_CHEAP,				//ʥ��Ʒ�ʲ���

		EN_JING_YING_SHI_LIAN_TEAM_HAVE_ROBOT,			//��ǰ�����д�����ս��Ա���޷�������ؾ�

		EN_RA_CHONGZHI_NOT_ENOUGH,						// ���ճ�ֵδ�ﵽҪ��
		EN_RA_SANBAI_CHOU_NOT_DRAW,						// ��ǰ���ͳ齱��������

		EN_RA_LAN_TANG_CHU_MU_CAN_NOT_ADD_HERBS,		//��ǰ���ɼ���ҩ��

		EN_NEW_SHOP_ITEM_NOT_SHOW,						//��ǰ��Ʒ���ڲ�����״̬
		EN_KE_JU_EXAM_CAN_NOT_ANSWER,					//ֻ��ָ����ҿɴ���
		EN_KE_JU_EXAM_CAN_NOT_SEEK_HELP,				//������ָ���������,��������
		EN_KE_JU_EXAM_CAN_NOT_SUGGEST,					//����ָ���������,���ɽ���
		EN_TEAM_CAN_NOT_JOIN_KE_JU_EXAM,				// �������ڽ��пƾٴ�����,��ʱ���ɼ���
		EN_TARGET_OFFLINE_TIME_NOT_ENOUTH,				// �Է�����ʱ��������
		EN_GET_ROLE_INFO_FAIL,				// ��ȡ�Է���Ϣʧ�ܣ����Ժ�����
		EN_JIE_YI_TARGET_OFFLINE_TIME_NOT_ENOUTH,			//  �Է�����ʱ��������

		EN_RA_CHARISMATIC_TANABATA_FOWLS_ERR,			//�������ͻ�Ӧ�õ����Ͱ�
		EN_RA_CHARISMATIC_TANABATA_LOVE_VALUE_LIMIT,	//��Ľֵ����
		EN_RA_CHARISMATIC_TANABATA_MEILI_VALUE_LIMIT,	//����ֵ����

		EN_FU_YU_ALREADY_MAX_LEVEL,						//�����Ѵﵽ����,���ɼ���ǿ��
		EN_FU_YU_NOT_PROTECT_ITEM,						//δӵ���������, �޷�����
		EN_EQUIP_JEWELRY_UP_EQUIP_LEVEL_LIMIT,			//��װ���ȼ����㣬�޷�ǿ��
		EN_BATTLE_CANNOT_SURRENDER,						//��ǰս���޷�Ͷ��
		EN_BATTLE_TOO_SOON_TO_SURRENDER,				//����������Ͷ����̫��
		EN_BATTLE_ONLY_LEADER_CAN_SURRENDER,            //ֻ�жӳ�����Ͷ��
		EN_RA_HUN_QI_FAN_LI_LOTTERY_COUNT_NOT_ENOUGH,	//�齱��������,������ȡ

		EN_KUA_FU_YING_XIONG_LU_FETCH_FAIL,				//��Ǹ,�ð�δ����,������ȡ
		EN_MONTH_CARD_CAN_USE_FUNCTION,					//�ܿ����¿���Ȩ��ʹ�øù���
		EN_DIANHUASHI_ERR_NOT_SAME_STONE,				//��ʹ��ͬ�ȼ��㻯ʯ

		EN_STAR_CHART_SKILL_HOLE_UNLOCK,				// �ü��ܿ�δ����
		EN_STAR_CHART_SKILL_LOCK,						// �ü���δ����
		EN_STAR_CHART_SKILL_TYPE_MISMATCH,				// �������Ͳ�ƥ��
		EN_STAR_CHART_SKILL_REPEAT,						// �ü����ѱ�װ��,�����ظ�װ��
		EN_STAR_CHART_SKILL_HOLE_UNLOCKED,				// ��ϲ�������µļ��ܿ�λ
		EN_STAR_CHART_POINT_CONDITION_NOT_MET,			// ����������,���ɽ���
		EN_STAR_CHART_NOT_ALL_LIGHT_UP,					// ���ȵ���ǰ�����ͼ~

		EN_FASHION_COLLECTION_MAX_SPECIALIZE_LEVEL,		// ר���Ѵ����ȼ�, ���ɼ�������
		EN_FASHION_COLLECTION_STAR_NOT_ENOUGH,			// ������������,�޷�����
		EN_RA_YUE_XI_DENG_MI_ANSWER_QUESTION_ERR,		//��Ǹ,�ش����~

		EN_FU_YU_LEVEL_NOT_ENOUGH,						//�����������
		EN_FU_YU_SP_UNSAME,								//������Ч��һ��,��������
		EN_FU_YU_CAN_NOT_SWITCH,						//��ǰ���񲻿�ת��
		EN_FU_YU_SAME_SP_CAN_NOT_SWITCH,				//ת����Ч��ԭ��Ч��ͬ
		
		EN_HUAN_SHOU_NOT_ACTIVE,						//��Ǹ,�û�����δ����,����ʧ��
		EN_HUAN_SHOU_STR_LEVEL_LIMIT,					//��Ǹ,�û����Ѵ��������,����ʧ��
		EN_HUAN_SHOU_SKILL_NOT_ENOUGH,					//��Ǹ,�û����ѽ���������������,���ɼ���
		EN_HUAN_SHOU_PP_LEVEL_LIMIT,					//��Ǹ,�õ�λ�ȼ��Ѵ�����,����ʧ��
		EN_HUAN_SHOU_PP_NOT_LEARN_BY_ADJOIN_POINT_LEVEL,//ǰ�õ�λ��δ����ָ���ȼ�,����ʧ��
		EN_HUAN_SHOU_LIAN_DAN_LIMIT,					//���������Ѵ����ޣ����Ժ�����

		EN_NORMAL_ERROR_NUM_MAX,		
		////////////////////////////////////////// �̶�ֵ //////////////////////////////////////////
		EN_GET_ACCOUNT_GOLD_TOO_FAST = 100000,			//���˺���ȡԪ�����ʱ�䲻��
		EN_COIN_NOT_ENOUGH = 100001,					//ͭ�Ҳ��㣬��ͨ����ħ֮������ս������淨���ͭ��
		EN_GOLD_NOT_ENOUGH = 100002,					//��Ԫ�����㣬��ǰ����ֵ��
		EN_BIND_GOLD_NOT_ENOUGH = 100003,				//��Ԫ������
		EN_MONEY_IS_LOCK = 100004,						//��Ǯ�Ѿ�����
		EN_ROLE_ZHENQI_NOT_ENOUGH = 100005,				//�ɻ겻��
		EN_XIANNV_EXP_DAN_LIMIT = 100006,				//��Ů���鵤����
		EN_CONVERTSHOP_BATTLE_FIELD_HONOR_LESS = 100007,//ս����������
		EN_ROLE_LEVEL_NOT_ENOUTH = 100008,				//��ɫ�ȼ����㣬��������
		EN_ROLE_CAPABILITY_NOT_ENOUTH = 100009,			//��ɫս������15���޷������μ���һ�а�
		EN_REWARD_ALREADY_FETCHED = 100010,				//�ý�������ȡ
		EN_ACTIVITY_NOT_OPEN = 100011,					//���δ����
		EN_INPUT_PARAM_ERROR = 100012,					//��������
		EN_ACTIVITY_IN_STANBY_STATUS = 100013,			//�����׼���У����λ����׼�����룡
		EN_CROSS_NOT_CONNECTED = 100014,				//�����δ��ͨ�����Ժ�����
		EN_MARKET_COIN_BIND_NOT_ENOUGH = 100015,		//���ɱҲ���
		EN_UNKNOWN_ERROR = 100016,						//δ֪����,���Ժ�����
		EN_STORAGE_SPACE_NOT_ENOUGH = 100017,			//�ֿ�ռ䲻��
		EN_ROLE_IN_BATTLE = 100018,						//��ɫ����ս���У�����ʧ��
		EN_BATTLE_SERVER_NOT_CONNECT = 100019,			//ս���������������Ժ�����
		EN_ROLE_NOT_IN_BATTLE = 100020,					//��ɫ����ս���У�����ʧ��
		EN_CONDITIONS_NOT_REACHED = 100021,				//����δ���
		EN_TODAY_USE_COUNT_LIMIT = 100022,				//����ʹ�ô�������
		EN_SHOP_BUY_LIMIT = 100023,						//�Ѵﹺ������
		EN_MONSTER_GROUP_NOT_EXIST = 100024,			//�����鲻����
		EN_SCENE_TARGET_POS_NOT_VALIDATE = 100027,		//Ŀ��λ�ò�����
		EN_ROLE_NAME_REPEAT = 100028,					//�ǳ��ѱ�ʹ��
		EN_IMMORTAL_COIN_NOT_ENOUGH = 100029,			//�ɱҲ���
		EN_SOMEONE_IN_BATTLE = 100030,					//�н�ɫ����ս����
		EN_TODAY_JOIN_COUNT_LIMIT = 100031,				//���մ�������
		EN_CHIVALROUS_NOT_ENOUGH = 100032,				//����ֵ����
		EN_ROLE_CAN_NOT_CROSS_NOW = 100033,				//��ǰ�����޷������״̬�����Ժ�����
		EN_CROSS_FAILED = 100034,						//������ʧ��
		EN_OPERATE_FORBID_IN_CROSS = 100035,			//�ò����ڿ����Ч�����ԭ������
		EN_OPERATE_TOO_FREQUENT = 100036,				//��������Ƶ�������Ժ�����
		EN_FUN_OPEN_ROLE_LEVEL_LIMIT = 100037,			//��ɫ�ȼ����㣬�޷������ù���
		EN_FUN_OPEN_ROLE_TASK_LIMIT = 100038,			//δ������������޷������ù���
		EN_HAVE_NOT_REACH_ACTIVE_CONDITION = 10039,		//��δ��ɼ�������
		EN_YOUR_CLICK_IS_TOO_OFTEN = 10040,				//����������̫���ˣ��������ٵ��~
		EN_ACTIVITY_IS_END = 10041,						//��ѽ���
		EN_OTHER_MONEY_NOT_ENOUGH = 10042,				//������Դ���㣬����ʧ��
		EN_TEAM_MEMBER_NOT_GUILD_MEMBER = 10043,		//�������г�Ա����ͬһ�����У�����ʧ��
		EN_TEAM_MEMBER_LEVEL_LIMIT = 10044,				//�������г�Ա�����ϵȼ����ƣ�����ʧ��
		EN_AVATAR_COLOR_NOT_MATCH = 10045,				//��ɫ���ɫģ�ͻ�ְҵ��ƥ��
		EN_START_CROSS_ERROR_PLAT_TYPE = 10046,			//����������ʹ���,�޷�������
		EN_START_CROSS_ERROR_PARAM_LEN = 10047,			//����汾�����޷�������
		EN_LOGIN_ACCOUNT_KEY_ERROR = 10048,				//��¼Token�����������µ�¼
		EN_RETURN_ORIGIN_SERVER_FAIL = 10049,			//���޷�����ԭ�������Ժ�����
		EN_REQ_PARAM_NOT_LONG_ENOUGH = 10050,			//����������Ȳ���
		EN_DEBUG_RAND_NAME_EXIST = 10051,				//��������ظ����������ǲ��Դ��룬������ʾ����ʽ����
		EN_YOUR_ELEMENT_CRYTAL_COUNT_DONE = 10052,		//��������������ʣ��ʹ�ô���
		EN_MIYI_HAS_BEEN_POKER_GET = 10053,				//���ս��������Ѵ����ޣ��޷�������ȡ���ƽ���
		EN_CREDIT_NUM_NOT_ENOUGH = 10054,				//��������ֵ����
		EN_DB_SESSION_LOST = 10055,						//�����ݲ����ã����Ժ�����
		EN_SILVER_COIN_NOT_ENOUGH = 10056,				//������
		EN_TARGET_HONG_BAO_INEXISTENCE = 10057,			//Ŀ���������ڣ�����ʧ��
		EN_CFG_VER_CHANGE = 10058,						//���ñ�������ص���Ϸ������������
		EN_LEVEL_MAX_LIMIT = 10059,						//�ȼ��Ѵﵽ����
		EN_ASSIST_REWARD = 10060,						//������Ϊ�ֵľٶ������������ʹͣ�
		EN_ACTIVITY_NOT_FLY_SHOE = 10061,				//��ڼ�,�޷��뿪����
		EN_CURR_LEVEL_LIMIT = 10062,					//��ǰ�ȼ��Ѵﵽ����
		EN_HIDDEN_REQ = 10063,							//��ǰ���������
		EN_OPPONENT_IN_BATTLE = 100064,					//��������ս���У�ս������ʧ��
		EN_CHAT_SERVER_LOST = 10063,					//���������ά����,���Ժ�����
		EN_BIGCROSS_SERVER_LOST = 10064,				//����ά����,���Ժ�����
		EN_LOTTERY_NUM_NOT_ENOUGH = 10065,				//���±Ҳ���
		EN_SERVER_TOO_HOT_TO_CREATE_NEW_ROLE = 10066,	//������̫��������ѡ�����µķ�����Ŷ

		EN_ROLE_IN_BATTLE_1 = 100101,					//��ɫ����ս���У�����ʧ��(���ǲ��Դ���1)
		EN_ROLE_IN_BATTLE_2 = 100102,					//��ɫ����ս���У�����ʧ��(���ǲ��Դ���2)
		EN_ROLE_IN_BATTLE_3 = 100103,					//��ɫ����ս���У�����ʧ��(���ǲ��Դ���3)
		EN_ROLE_IN_BATTLE_4 = 100104,					//��ɫ����ս���У�����ʧ��(���ǲ��Դ���4)
		EN_ROLE_IN_BATTLE_5 = 100105,					//��ɫ����ս���У�����ʧ��(���ǲ��Դ���5)
		EN_ROLE_IN_BATTLE_6 = 100106,					//��ɫ����ս���У�����ʧ��(���ǲ��Դ���6)
		EN_ROLE_IN_BATTLE_7 = 100107,					//��ɫ����ս���У�����ʧ��(���ǲ��Դ���7)
		EN_ROLE_IN_BATTLE_8 = 100108,					//��ɫ����ս���У�����ʧ��(���ǲ��Դ���8)
		EN_ROLE_IN_BATTLE_9 = 100109,					//��ɫ����ս���У�����ʧ��(���ǲ��Դ���9 ����ʾ�������������)
		EN_ROLE_IN_BATTLE_10 = 100110,					//��ɫ����ս���У�����ʧ��(���ǲ��Դ���10)
		EN_ROLE_IN_BATTLE_11 = 100111,					//��ɫ����ս���У�����ʧ��(���ǲ��Դ���11)	
		
		EN_ROLE_BATTLE_INFO_NOT_FOUND = 100200,			//����ս����Ϣδ�ҵ������ǲ��Դ��룩
		EN_BATTLE_LOCAL_INFO_NOT_FOUND = 100201,		//����ս����Ϣδ�ҵ������ǲ��Դ��룩
		EN_ROLE_BATTLE_ID_AQUIRE_FAILED = 100202,		//��ȡ����ս��ʵ��IDʧ�ܣ����ǲ��Դ��룩
		EN_BATTLE_ID_NOT_EQUAL = 100203,				//ս��ʵ��ID����ͬ�����ǲ��Դ��룩
		EN_BATTLE_INSTANCE_AQUIRE_FAILED = 100204,		//��ȡս��ʵ��ʧ�ܣ����ǲ��Դ��룩
		EN_BATTLE_INSTANCE_ROLE_INFO_NOT_FOUND = 100205,//ս��ʵ�����Ҳ������ǵ���Ϣ�����ǲ��Դ��룩
		EN_MAX,
	};
	UNSTD_STATIC_CHECK(EN_MAX < 200000);
}

#endif