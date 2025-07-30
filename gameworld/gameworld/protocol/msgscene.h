#ifndef MSGSCENE_H
#define MSGSCENE_H

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/taskdef.h"
#include "servercommon/reliclootingdef.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/weddingdef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	/*
	�����˳�����
	*/

	class SCEnterScene					//!< ���볡�� 1100
	{
	public:
		SCEnterScene();
		MessageHeader	header;

		int				sceneid;		//!< ����ID
		ObjID			obj_id;			//!< �����ڳ����е�id
		short			is_first;		//!< �Ƿ��һ�ν���˳���
		short			pos_x;			//!< �����ڳ����е�λ��X
		short			pos_y;			//!< �����ڳ����е�λ��Y
		int				scene_key;		//!< ����key
	};

	class SCLeaveScene					//!< �뿪����֪ͨ 1101 \note ��δʹ��
	{
	public:
		SCLeaveScene();
		MessageHeader	header;
		int				sceneid;		//!< �뿪�ĳ���ID
	};

	class SCVisibleObjEnterRole			//!< ������ҽ�����Ұ 1102
	{
	public:
		SCVisibleObjEnterRole();
		MessageHeader	header;

		int				role_id;		//!< ���ID
		GameName		role_name;		//!< �����

		ObjID			obj_id;			//!< ��Ҷ���ID
		short			level;			//!< ��ҵȼ�
		int				top_level;		//!< �۷�ȼ�
		int				kill_value;		//!< ɱ¾ֵ 

		Appearance		appearance;		//!< ���

		int				move_speed;		//!< �ٶ�
		short			pos_x;			//!< �ڳ����е�λ��X
		short			pos_y;			//!< �ڳ����е�λ��Y
		Scalar			distance;		//!< ������� (���ڵ�ǰ������Ҫ�ߵľ��룩
		Dir				dir;			//!< ���� [-PI,-PI]
		int				scene_id;		//!< ��ʶ�����ĸ�������obj

		char			behavior_state;	//!< ��Ϊ״̬ \see RoleStatusManager::ROLE_STATUS
		char			is_leader;		//!< �Ƿ�ӳ�[0:�� 1:��]
		char			is_in_team;		//!< �Ƿ��ڶ���[0:�� 1:��]
		char			move_state;		//!< ������-> �ƶ�״̬[0-127],���� RoleStatusManager::ROLE_MOVE_STATUS
		int				team_id;		//!< ����ID
		char			team_is_red;	//!< �Ƿ��������
		char			guild_post;		//!< ������-> ����ְλ, �޼���: -1
		short			sh_reserve;		//!< ������

		enum ATTACH_TYPE
		{
			ATTACH_TYPE_ESCORT = 0,		//!< �ڳ���seq
			ATTACH_TYPE_GUILD = 1,		//!< ����ļ����������
			ATTACH_TYPE_TITLE = 2,		//!< ͷ���ƺ�
			ATTACH_TYPE_FACESCORE = 3,	//!< ��ֵͷ����ԴID
			ATTACH_TYPE_FOLLOW_PET = 4,	//!< �������
			ATTACH_TYPE_JIE_YI_ZU_NAME = 5,	//!< ����������
			ATTACH_TYPE_MAX_NUM
		};
		UNSTD_STATIC_CHECK(ATTACH_TYPE_MAX_NUM < 31);

		const static int ATTACH_DATA_LENGTH = 2048; // attach�����ܳ��ȼ�����һ�����ܳ����������

		int				attach_mask;
		char			attach_data[ATTACH_DATA_LENGTH];
	};

	class SCUpdateFollowList	//!< 1117 ���¸����б� (�����ӳ�ʱ�·���
	{
	public:
		SCUpdateFollowList();
		MessageHeader header;

		int follow_team_id;
		ObjID follow_objid_list[MAX_TEAM_MEMBER_NUM];
		short reserve_sh;

		UNSTD_STATIC_CHECK(MAX_TEAM_MEMBER_NUM % 2 != 0);
	};

	class SCAddFollow			//!< 1118 ���Ӹ����� ������ӻ���ʱ�·���
	{
	public:
		SCAddFollow();
		MessageHeader header;

		int	follow_team_id;		//!< ����Ķ���ID
		ObjID add_objid;		//!< ����objid�б�
		short reserve_sh;		//!< ������
	};

	class SCRemoveFollow		//!< 1119 ɾ�������� ����ӻ�����ʱ�·���
	{
	public:
		SCRemoveFollow();
		MessageHeader header;

		int follow_team_id;		//!< ����Ķ���ID
		ObjID remove_objid;		//!< ɾ���ĸ�����objid
		short is_remove_all;	//!< ��ֵΪ1����ɢ�����������
	};

	

	class SCChangeAppearanceRole			//!< ��۸ı� 1112
	{
	public:
		SCChangeAppearanceRole();
		MessageHeader	header;

		int obj_id;
		Appearance appearance;				//!< ���
	};

	class SCVisibleObjEnterNPCMonster	//!< 1109 NPC���������Ұ
	{
	public:
		SCVisibleObjEnterNPCMonster();
		MessageHeader header;

		int npc_id;						//!< npc_id
		int client_show_id;				//!< �ͻ�����ʾģ��id
		int monster_group_id;			//!< ������ID
		int battle_mode;				//!< ս��ģʽ  \see BATTLE_MODE
		int npc_seq;					//!< npc_seq   \note ���ڱ�ʶ����˵�NPC

		short obj_id;					//!< ����ID
		short reserve_sh;				//!< ������

		int				speed;			//!< �ٶ�
		short			pos_x;			//!< �ڳ����е�λ��X
		short			pos_y;			//!< �ڳ����е�λ��Y
		Scalar			distance;		//!< ������� (���ڵ�ǰ������Ҫ�ߵľ��룩
		Dir				dir;			//!< ���� [-PI,-PI]
		int				scene_id;		//!< ��ʶ�����ĸ�������obj
	};

	class SCVisibleObjEnterNPCGathers	//!< 1110 NPC�ɼ��������Ұ
	{
	public:
		SCVisibleObjEnterNPCGathers();
		MessageHeader header;

		int npc_id;					//!< ������ʾ���
		int npc_seq;				//!< ���ڱ�ʶ��������ר�в���
		short posi_x;				//!< �ڳ����е�λ��X
		short posi_y;				//!< �ڳ����е�λ��y

		int scene_id;				//!< ����ID

		ObjID obj_id;				//!< ����ID
		short gather_times;			//!< �ɼ�����
	};


	class SCVisibleObjEnterSpecialTransport	//!< 1113 �ر��߼������Ž�����Ұ
	{
	public:
		SCVisibleObjEnterSpecialTransport();
		MessageHeader header;

		int scene_id;				//!< ����ID
		ObjID obj_id;				//!< ����ID
		short posi_x;				//!< �ڳ����е�λ��X
		short posi_y;				//!< �ڳ����е�λ��y
		short reserved;				//!< ������
		
		int param1;
		int param2;
	};

	class SCVisibleObjEnterTreasureChest	//!< 1114 ���������Ұ
	{
	public:
		SCVisibleObjEnterTreasureChest();
		MessageHeader header;

		int scene_id;				//!< ����ID
		ObjID obj_id;				//!< ����ID
		short posi_x;				//!< �ڳ����е�λ��X
		short posi_y;				//!< �ڳ����е�λ��y

		short is_open;				//!< �Ƿ����
		int treasure_chest_id;		//!< ����id
	};

	class SCVisibleObjEnterNPCFuncNpc		//!< 1115 ������NPC������Ұ
	{
	public:
		enum FUN_NPC_TYPE
		{
			FUN_NPC_TYPE_NORMAL = 0,//!< Ĭ��
			FUN_NPC_TYPE_CHOSED = 1,//!< ����npc
		};
		SCVisibleObjEnterNPCFuncNpc();
		MessageHeader header;

		int scene_id;				//!< ����ID
		ObjID obj_id;				//!< ����ID
		short fun_type;				//!< ��������
		short posi_x;				//!< �ڳ����е�λ��X
		short posi_y;				//!< �ڳ����е�λ��y

		int npc_id;					//!< ������ʾ���

		int info_type;
		int param1;
		int param2;
		int param3;
		Dir dir;					//!< ���� [-PI,-PI]
	};

	class SCVisibleObjEnterXunYouCar	//!< 1128 Ѳ�λ���������Ұ
	{
	public:
		SCVisibleObjEnterXunYouCar();
		MessageHeader header;

		ObjID			obj_id;			//!< ����ID
		short			is_stop;		//!< ���ڵȴ�״̬

		int				car_type;		//!< Ѳ�λ�������
		
		int				speed;			//!< �ٶ�
		short			pos_x;			//!< �ڳ����е�λ��X
		short			pos_y;			//!< �ڳ����е�λ��Y
		Scalar			distance;		//!< ������� (���ڵ�ǰ������Ҫ�ߵľ��룩
		Dir				dir;			//!< ���� [-PI,-PI]
		int				scene_id;		//!< ��ʶ�����ĸ�������obj

		//��������Ϣ
		struct HostInfo
		{
			HostInfo()
			{
				uid = 0;

				memset(role_name, 0, sizeof(role_name));
				avatar_type = 0;

				obj_id = 0;
				hole_sh = 0;
			}
			int uid;
			GameName role_name;
			int avatar_type;		//!< ��ɫģ��[1,6]

			ObjID obj_id;			//!< ����ID
			short hole_sh;
		};

		HostInfo host_info_[WeddingFB::HOST_NUM];	//��������Ϣ[2]
	};

	class SCVisibleObjEnterPeakTokenObj	//!< 1129 ���������� ���� ������Ұ
	{
	public:
		SCVisibleObjEnterPeakTokenObj();
		MessageHeader header;

		ObjID			obj_id;			//!< ����ID
		short			reserved;		//!< ������

		int				speed;			//!< �ٶ�
		short			pos_x;			//!< �ڳ����е�λ��X
		short			pos_y;			//!< �ڳ����е�λ��Y
		Dir				dir;			//!< ���� [-PI,-PI]
		int				scene_id;		//!< ��ʶ�����ĸ�������obj

		int				creat_uid;		//!< ������uid
		GameName		creat_name;		//!< ����������
		Appearance		creat_appearance;	//!< ���������

		long long		act_id;			//!< �id
		unsigned int	act_begin_time;	//!< ���ʼʱ���
		unsigned int	act_end_time;	//!< �����ʱ���

		int	role_fetch_time;			//!< ��ɫ�콱����
	};

	class SCVisibleObjEnterTuZiKuaiPaoTuzi	//!< 1138 ���ӿ�������
	{
	public:
		SCVisibleObjEnterTuZiKuaiPaoTuzi();
		MessageHeader header;

		ObjID			obj_id;			//!< ����ID
		short			reserved;		//!< ������

		int				speed;			//!< �ٶ�
		short			pos_x;			//!< �ڳ����е�λ��X
		short			pos_y;			//!< �ڳ����е�λ��Y
		Scalar			distance;		//!< ������� (���ڵ�ǰ������Ҫ�ߵľ��룩
		Dir				dir;			//!< ���� [-PI,-PI]
		int				scene_id;		//!< ��ʶ�����ĸ�������obj
		int tu_zi_type;
		int role_id;
		unsigned int time;			//����ʱ��
		int track_type;
		int track_point_index;
	};

	class SCVisibleObjLeave			//!< �����뿪��Ұ 1103
	{
	public:
		SCVisibleObjLeave();
		MessageHeader	header;		
		ObjID			obj_id;		//!< �����ڳ����е�ID
		unsigned short	reserved;	//!< ������
	};

	/*
	�ƶ�
	*/
	class CSObjMove					//!< �����ƶ� 1150
	{
	public:
		CSObjMove();
		MessageHeader	header;
		short			pos_x;			//!< ��ҽ�ɫ�ڿͻ����еĵ�ǰλ��X
		short			pos_y;			//!< ��ҽ�ɫ�ڿͻ����еĵ�ǰλ��Y
		short			target_pos_x;	//!< Ŀ��λ��X
		short			target_pos_y;	//!< Ŀ��λ��Y
		int				cur_scene_id;	//!< �ͻ��˵�ǰ���ڳ���ID
	};

	class SCObjMove					//!< �ƶ����� 1104	\note ���һ����Χ�ڵ�ÿ������·����ͻ�����Ҫ�жϺ����ĸ��������ƶ�
	{
	public:
		SCObjMove();
		MessageHeader	header;
		
		ObjID			obj_id;		//!< ����ID
		short			reserve_sh;	//!< ������
		short			pos_x;		//!< ����ǰ��λ��X
		short			pos_y;		//!< ����ǰ��λ��Y
		Dir				dir;		//!< ����[-PI,PI]
		Scalar			distance;	//!< �������
		
	};

	class SCResetPost				//!< ǿ������λ�� 1105
	{
	public:
		enum
		{
			RESET_TYPE_DEFAULT,		//!< Ĭ��ԭ�� 0
			RESET_TYPE_SLOW,		
			RESET_TYPE_JUMP,
		};

		SCResetPost();
		MessageHeader	header;
		short			x;			//!< ǿ�赽��ǰ����λ��X
		short			y;			//!< ǿ�赽��ǰ����λ��Y
		int				scene_id;	//!< ��ǰ����ID
	};

	class CSFlyByShoe				//!< ʹ�÷�����ѥ���� 1151
	{
	public:
		CSFlyByShoe();
		MessageHeader	header;

		int				scene_id;	//!< ����������ID
		short			pos_x;		//!< ������λ��X
		short			pos_y;		//!< ������λ��Y
	};

	class SCChangeSceneNotice		//!< �л�����֪ͨ  1106
	{
	public:
		SCChangeSceneNotice();
		MessageHeader	header;

		int				change_scene_reason;
		int				target_scene_id;
	};

	class CSEnterFB			//!< 1107 ������븱��
	{
	public:
		CSEnterFB();
		MessageHeader header;

		short fb_type;		//!< ��������  \see FB_TYPE
		short reserve;		//!< ������
		int param_1;		//!< ����1
		int param_2;		//!< ����2
	};

	class CSLeaveFB			//!< 1108 �����뿪����
	{
	public:
		CSLeaveFB();		
		MessageHeader header;
	};

	class SCSceneObjList		//!< 1116 ������objλ���б�
	{
	public:
		SCSceneObjList();
		MessageHeader header;

		struct ObjInfo
		{
			short pos_x;		//!< λ��x
			short pos_y;		//!< λ��y
			ObjID obj_id;		//!< obj_id
			short obj_type;		//!< obj���� \see Obj::_ObjType
		};

		enum NOTIFY_REASON
		{
			NOTIFY_REASON_NORMAL = 0,	//!< 0 �����·���ͨ���ڽ��볡��ʱ��
			NOTIFY_REASON_ADD = 1,		//!< 1 ����objʱ�·�
			NOTIFY_REASON_DELETE = 2,	//!< 2 ɾ��objʱ�·�
		};

		static const int MAX_OBJ_NUM = 256;

		short notify_reason;	//!< 0:normal   1:add   2:remove
		short obj_num;			//!< obj����

		ObjInfo obj_list[MAX_OBJ_NUM];	//!< objλ���б�
	};

	class SCJoinTeamNotifyArea	//!< 1120 ������飬֪ͨ��������
	{
	public:
		SCJoinTeamNotifyArea();
		MessageHeader header;

		ObjID obj_id;			//!< ���������ҵ�obj
		short is_red;			//!< �Ƿ��������
		short team_id;			//!< ����ID
	};

	class SCLeaveTeamNotifyArea	//!< 1121 �뿪����  ֪ͨ��������
	{
	public:
		SCLeaveTeamNotifyArea();
		MessageHeader header;

		ObjID obj_id;			//!< �뿪������ҵ�obj
		short reserve_sh;		//!< ������
	};

	class CSRolePatrol			//!< 2017 ��ʼѲ��/ֹͣѲ��
	{
	public:
		CSRolePatrol();
		MessageHeader header;

		int is_start_or_stop;	//!< 0:ֹͣѲ��  1:��ʼѲ��
	};

	class CSRoleTaskPathFinding	//!< 2019 �������Ѱ·֪ͨ
	{
	public:
		CSRoleTaskPathFinding();
		MessageHeader header;

		int task_id;			//!< Ѱ·����ID
		short is_stop;			//!< �Ƿ�ֹͣ  [0:��ʼѰ·  1:ֹͣѰ·]
		short reserve;			//!< ������
	};

	class SCRoleSceneEffect						//!< 1122����buffЧ�����õ���Ч
	{
	public:
		enum ROLE_SCENE_EFFECT_TYPE
		{
			ROLE_SCENE_EFFECT_TYPE_0 = 0,//!< ��Ѫ
			ROLE_SCENE_EFFECT_TYPE_1 = 1,//!< ����
			ROLE_SCENE_EFFECT_TYPE_2 = 2,//!< ��Ѫ����
			ROLE_SCENE_EFFECT_TYPE_3 = 3,//!< ����buff
			ROLE_SCENE_EFFECT_TYPE_4 = 4,//!< Ч��buff
		};
		SCRoleSceneEffect();
		MessageHeader		header;
		ObjID buffer_obj;
		short resever_sh;
		int effect_type;
	};

	class SCSpecialAppearanceChangeNotice	//!< 1123  ��������ı�֪ͨ  
	{
	public:
		SCSpecialAppearanceChangeNotice();
		MessageHeader		header;

		ObjID obj_id;						//!< �ı��objid
		short special_appearance_type;		//!< �����������
		int special_appearance_param;		//!< ������۲���
	};

	class CSRoleTalkToNpc						//!< 1124 ��NPC�Ի�ͨ��
	{
	public:
		CSRoleTalkToNpc();
		MessageHeader		header;
	};

	class CSEscapeFromCurrentPosition			//!< 1152 �ͻ������뿨��
	{
	public:
		CSEscapeFromCurrentPosition();
		MessageHeader		header;
	};

	class CSAutoEscapeFromCurrentPosition		//!< 1153 �ͻ����Զ����뿨��
	{
	public:
		CSAutoEscapeFromCurrentPosition();
		MessageHeader header;
	};

	class CSGetCurPos	//!< 1154 �����˿ͻ��˲�ѯ�Լ���λ��
	{
	public:
		CSGetCurPos();
		MessageHeader header;
	};

	class SCOjbSpeedChangeNotice			//!< 1155 obj���ٸı�֪ͨ
	{
	public:
		SCOjbSpeedChangeNotice();
		MessageHeader		header;

		int obj_id;
		int move_speed;
	};
	
	class SCGetCurPos
	{
	public:
		SCGetCurPos();
		MessageHeader header;

		int scene_id;
		short pos_x;
		short pos_y;
	};

	class CSWenXiangLouReq
	{
	public:
		CSWenXiangLouReq();		//3374
		MessageHeader header;

		enum REQ_TYPE
		{
			REQ_TYPE_TALK = 0,//��̸				param1:npc_seq
			REQ_TYPE_FIND_ITEM = 1,//���ҵ���		param1:item_seq
			REQ_TYPE_IDENTITY = 2,//ָ��			param1:npc_seq
			REQ_TYPE_FIGHT = 3,	//�������ս��		�޲���
			REQ_TYPE_NPC_INFO = 4,//����NPC��Ϣ
			REQ_TYPE_SCENE_INFO = 5,//���󳡾���Ϣ
		};

		int req_type;
		int param1;
	};

	class SCWenXiangLouNpcInfo
	{
	public:
		SCWenXiangLouNpcInfo();		//3375
		MessageHeader header;

		char load_status;								// ���н׶�
		char reserve_ch;
		short reserve_sh;
		char npc_sucspicion_list[MAX_NPC_NUM_WXL];		// npc����ֵ
		char npc_talk_list[MAX_NPC_NUM_WXL];			// npc�Ի���¼
		char npc_identity_list[MAX_NPC_NUM_WXL];		// npcָ�ϼ�¼
		char npc_check_list[MAX_NPC_NUM_WXL];			// npc��֤��¼����פ��
	};

	class SCWenXiangLouReqSceneInfo
	{
	public:
		SCWenXiangLouReqSceneInfo();
		MessageHeader header;

		char load_status;		// ���н׶�
		char role_enter_posi;	//�Ƿ񴥷������ض��ص�
		char has_find_identity;	//�Ƿ�ָ����ȷ
		char has_finish_fb;		//�Ƿ���ɸ���

		char wenxianglou_has_enter;
		char reserve_ch;
		short reserve_sh;
		//��ʱ���߱���                                                                                                                                                                       
		int reward_count;
		int reward_list[MAX_SPECIAL_ITEM_NUM];//���⸱�����߰� -- ��������20��
	};

	class SCRAMiJingQiWenShopInfo
	{
	public:
		SCRAMiJingQiWenShopInfo();		//3612 �̵���Ϣ
		struct ItemInfo
		{
			ItemInfo() :item_id(0), num(0), is_bind(0), price_type(PRICE_TYPE_0), price(0), buy_times(0), discount(0) {}
			enum PRICE_TYPE
			{
				PRICE_TYPE_0 = 0,//Ԫ��
				PRICE_TYPE_2 = 2,//ͭǮ
			};
			ItemID item_id;
			short num;
			int is_bind;
			int price_type;
			int price;
			int buy_times;
			int m_buy_times;
			int buy_item_idx;//�����͵��±�
			int discount;
		};
		const static int MAX_SHOP_ITEM_NUM = 15;
		MessageHeader header;
		int shop_num;
		ItemInfo shop_list[MAX_SHOP_ITEM_NUM];
	};

	struct MiJingQuestion
	{
		MiJingQuestion() : question_id(0), is_ture(0), answer_index(0) {}
		int question_id;//��Ŀ
		int is_ture;	// �Ƿ���ȷ   0��δ���⣬1����ȷ��2������
		int answer_index;//�𰸱�� - չʾ�ӳ������ѡ��
	};

	class SCRAMiJingQiWenQuestionInfo
	{
	public:
		SCRAMiJingQiWenQuestionInfo();		//3614 ��Ŀ�·�
		
		MessageHeader header;

		MiJingQuestion question_info;
	};

	class CSRAMiJingQiWenQuestionReq
	{
	public:
		CSRAMiJingQiWenQuestionReq();		//3615 ����

		MessageHeader header;

		int answer_id;
		int npc_obj_id;
	};

	class SCRAFortuneMazeBoxNum
	{
	public:
		SCRAFortuneMazeBoxNum();		//3516

		MessageHeader header;

		int box_num;
	};

	class CSRAMiJingQiWenBuyItemReq
	{
	public:
		CSRAMiJingQiWenBuyItemReq();		//3619 ����

		MessageHeader header;

		int item_idx;
		int npc_obj_id;
	};

	class SCCloseWindosRet
	{
	public:
		SCCloseWindosRet();		//3620 ���ڿ��Ʒ���

		MessageHeader header;

		int reason;
	};

	class CSCloseWindosReq
	{
	public:
		CSCloseWindosReq();		//3621 �رմ���

		MessageHeader header;

		int reason;
	};

	class SCRelicLootingTampInfo
	{
	public:
		SCRelicLootingTampInfo();		//!< 3622 ��Ϸ״̬
		enum RELIC_LOOTING_TYPE
		{
			RELIC_LOOTING_TYPE_ROLE_LOGIN_STATUS = 0,	//!< �ȴ������˼���״̬
			RELIC_LOOTING_TYPE_READY_START = 1,			//!< ׼����ʼ
			RELIC_LOOTING_TYPE_BEGAIN = 2,				//!< ��Ϸ��ʽ��ʼ
		};

		MessageHeader header;

		int reason_type;
		unsigned int tamp;//!< ��ʱ���������
	};

	class SCRelicLootingFbSkillInfo
	{
	public:
		SCRelicLootingFbSkillInfo();		//!< 3623  ������Ϣ
		MessageHeader header;
		int stop_reason;
		PlayerSkillList m_skill;
	};

	class CSRelicLootingMatchReq				//!< 3624  �ż��ᱦ ����
	{
	public:
		CSRelicLootingMatchReq();
		MessageHeader header;

		enum TYPE
		{
			TYPE_CANCEL = 0,			//!<ȡ��ƥ��
			TYPE_MATCH = 1,				//!<����ƥ��
			TYPE_RANK = 2,				//!<��ѯ����
			TYPE_REWARD = 3,			//!<�ɾͽ��� param seq
			TYPE_REWARD_INFO = 4,		//!<�ɾ��·�
			TYPE_SET_SHOW = 5,			//!<�����������
		};

		int is_match;
		int param;
	};

	class SCRelicLootingTeamInfoReq				//!< 3625  �ż��ᱦ��Ӫ��Ϣ
	{
	public:
		SCRelicLootingTeamInfoReq();
		MessageHeader header;
		const static int MAX_TEAM_INFO_MEMBER_NUM = 3;//�����Ӫ����
		enum TEAM_TYPE
		{
			TEAM_TYPE_DEFENDER = 0,			//!<����
			TEAM_TYPE_ATTACKER = 1,			//!<����
		};
		struct TeamInfo
		{
			TeamInfo() : team_type(0), uid(0), gongxian(0), stop_reason(0), on_line(0) {}
			int team_type;
			int uid;
			int gongxian;
			int stop_reason;
			int on_line;
			GameName play_name;
		};

		TeamInfo defend_team_list[MAX_TEAM_INFO_MEMBER_NUM];
		TeamInfo attack_team_list[MAX_TEAM_INFO_MEMBER_NUM];
	};

	class CSRelicLootingPlayerReq				//!< 3626  �ż��ᱦ��Ҳ���
	{
	public:
		CSRelicLootingPlayerReq();
		MessageHeader header;
		enum MSG_TYPE
		{
			MSG_TYPE_GIVE_UP_SKILL = 1,			//!< ������Ʒ����
			MSG_TYPE_GET_SKILL = 2,				//!< ��ȡ��Ʒ���� param1 ��Ӧ����Ʒobjid
			MSG_TYPE_USE_SKILL = 3,				//!< ʹ�ü���
			MSG_TYPE_TS = 4,					//!< ���ܴ��� param1���봫�͵����uid
			MSG_TYPE_MARK = 5,					//!< ��ǵ�ͼ param1 ������ͣ�param2 pos_x param3 pos_y
			MSG_TYPE_GIVE_UP_SKILL_STATUS = 6,	//!< �жϼ���
		};

		int type;
		int param1;
		int param2;
		int param3;
		int param4;
	};

	class SCRelicLootingObjInfo				//!< 3627  �ż��ᱦ��ͼ��Ϣ
	{
	public:
		SCRelicLootingObjInfo();
		MessageHeader header;
		const static int MAX_OBJ_NUM = 32;//���obj����
		enum OBJ_TYPE
		{
			OBJ_TYPE_BOX = 0,			//!<����
			OBJ_TYPE_SKILL = 1,			//!<����	param ����id
			OBJ_TYPE_TRAP = 2,			//!<����
			OBJ_TYPE_DOLL = 3,			//!<��������
			OBJ_TYPE_ROBOT = 4,			//!<������
		};
		enum OBJ_STATUS
		{
			OBJ_STATUS_NORMAL = 0,		//!< ����
			OBJ_STATUS_NO = 1,			//!< ����������
		};
		struct Objinfo
		{
			Objinfo() : obj_type(0), status(0), param(0), param2(0), obj_pos(0, 0) {}
			int obj_type;
			int status;
			int param;
			int param2;
			Posi obj_pos;
		};
		int obj_count;
		Objinfo obj_list[MAX_OBJ_NUM];
	};

	class SCRelicLootingMemberPosInfo				//!< 3628  �ż��ᱦ��Աλ��
	{
	public:
		SCRelicLootingMemberPosInfo();
		const static int MAX_TEAM_INFO_MEMBER_NUM = 3;//�����Ӫ����
		MessageHeader header;
		struct MemberPos
		{
			MemberPos() {}
			int uid;
			Posi pos;
		};
		int member_num;
		MemberPos member_list[MAX_TEAM_INFO_MEMBER_NUM];
	};

	class SCRelicLootingMemberTransmitInfo				//!< 3629 �ż��ᱦѯ�ʴ���
	{
	public:
		SCRelicLootingMemberTransmitInfo();
		MessageHeader header;
		int uid;
		unsigned int end_tamp;
		GameName player_name;
	};

	class SCRelicLootingMarkInfo				//!< 3631 �ż��ᱦ����·�
	{
	public:
		SCRelicLootingMarkInfo();
		MessageHeader header;
		int marker_uid;
		int mark_type;
		Posi mark_pos;
	};

	class SCRelicLootingFBFinishInfo				//!< 3632 �ż��ᱦ����·�
	{
	public:
		SCRelicLootingFBFinishInfo();
		MessageHeader header;
		const static int MAX_TEAM_INFO_MEMBER_NUM = 3;//�����Ӫ����
		enum TEAM_TYPE
		{
			TEAM_TYPE_DEFENDER = 0,			//!<����
			TEAM_TYPE_ATTACKER = 1,			//!<����
		};
		enum RELIC_LOOTING_WIN_TYPE
		{
			RELIC_LOOTING_WIN_TYPE_DRAW = 0,//��ʱ
			RELIC_LOOTING_WIN_TYPE_WIN_A = 1,//����ʤ
			RELIC_LOOTING_WIN_TYPE_WIN_D = 2,//�ر�ʤ
		};
		struct TeamInfo
		{
			TeamInfo() : team_type(0), uid(0), gongxian(0), add_score(0), on_line(0)
			{
				memset(play_name, 0, sizeof(GameName));
			}
			int team_type;
			int uid;
			int gongxian;
			int add_score;
			int on_line;
			GameName play_name;
		};
		int win_type;
		TeamInfo defend_team_list[MAX_TEAM_INFO_MEMBER_NUM];
		TeamInfo attack_team_list[MAX_TEAM_INFO_MEMBER_NUM];
	};

	class SCRelicLootingRankInfo				//!< 3633 �ż��ᱦ���а�
	{
	public:
		SCRelicLootingRankInfo();
		MessageHeader header;
		const static int MAX_RANK_NUM = 100;//��������·�
		
		int m_pos;//�ҵ�����
		int rank_num;
		RelicLootRankData rank_list[MAX_RANK_NUM];
	};

	class SCRelicLootingAchievementInfo				//!< 3634 �ż��ᱦ�ɾͽ�����Ϣ
	{
	public:
		SCRelicLootingAchievementInfo();
		MessageHeader header;

		int param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_MAX];
		int reward_status[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_MAX];//�Ƿ��Ѿ���ȡ[0-1]
		int show_flag;
	};

	class SCRelicLootingEffectInfo				//!< 3635 �ż��ᱦ���״̬�·�
	{
	public:
		SCRelicLootingEffectInfo();
		MessageHeader header;

		int uid;
		int special_effect_type;
	};


	class SCRAMiJingQiWenShopInfo2
	{
	public:
		SCRAMiJingQiWenShopInfo2();		//4171 �̵���Ϣ
		struct ItemInfo
		{
			ItemInfo() :item_id(0), num(0), is_bind(0), price_type(PRICE_TYPE_0), price(0), buy_times(0), discount(0) {}
			enum PRICE_TYPE
			{
				PRICE_TYPE_0 = 0,//Ԫ��
				PRICE_TYPE_2 = 2,//ͭǮ
			};
			ItemID item_id;
			short num;
			int is_bind;
			int price_type;
			int price;
			int buy_times;
			int m_buy_times;
			int buy_item_idx;//�����͵��±�
			int discount;
		};
		const static int MAX_SHOP_ITEM_NUM = 15;
		MessageHeader header;
		int shop_num;
		ItemInfo shop_list[MAX_SHOP_ITEM_NUM];
	};

	struct MiJingQuestion2
	{
		MiJingQuestion2() : question_id(0), is_ture(0), answer_index(0) {}
		int question_id;//��Ŀ
		int is_ture;	// �Ƿ���ȷ   0��δ���⣬1����ȷ��2������
		int answer_index;//�𰸱�� - չʾ�ӳ������ѡ��
	};

	class SCRAMiJingQiWenQuestionInfo2
	{
	public:
		SCRAMiJingQiWenQuestionInfo2();		//4172 ��Ŀ�·�

		MessageHeader header;

		MiJingQuestion2 question_info;
	};

	class CSRAMiJingQiWenQuestionReq2
	{
	public:
		CSRAMiJingQiWenQuestionReq2();		//4173 ����

		MessageHeader header;

		int answer_id;
		int npc_obj_id;
	};

	class CSRAMiJingQiWenBuyItemReq2
	{
	public:
		CSRAMiJingQiWenBuyItemReq2();		//4174 ����

		MessageHeader header;

		int item_idx;
		int npc_obj_id;
	};
}

#pragma pack(pop)

#endif

