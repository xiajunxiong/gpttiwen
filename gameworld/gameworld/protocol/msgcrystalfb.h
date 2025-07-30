#ifndef __MSG_CRYSTAL_FB_H__
#define __MSG_CRYSTAL_FB_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/rolecrystalfbdef.h"
#include "servercommon/commondata.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCCrystalFbInfo						//!< ˮ��������Ϣ		2460
	{
	public:
		SCCrystalFbInfo();
		MessageHeader		header;

		struct CrystalFBNormalRecordPro		//ÿ����ͨ�ؾ���¼
		{
			CrystalFBNormalRecordPro(): fb_seq(-1), challenge_times(0), buy_times(0), reserve_sh(0)
			{}

			short fb_seq;
			short challenge_times;			//������ս����
			short buy_times;				//���չ������ս����
			short reserve_sh;
		};

		unsigned char xi_you_fb_play_times[CRYSTAL_FB_XI_YOU_MAX_LEVEL];	//��С 52��ϡ�и�����Ĵ���
	
		int normal_record_num;													//!< normal_fb_record_list�����鳤��
		CrystalFBNormalRecordPro normal_fb_record_list[CRYSTAL_FB_NORMAL_NUM];	//!< ÿ����ͨ�ؾ��ļ�¼
	};

	class SCCrystalFbUseItemInfo						//!< ˮ�������������ʹ�õ�����Ϣ 2463
	{
	public:
		SCCrystalFbUseItemInfo();
		MessageHeader		header;

		struct UseItemInfo
		{
			UseItemInfo()
			{
				use_type = 0;
				use_times = 0;
			}

			int use_type;
			int use_times;
		};

		int count;
		UseItemInfo use_info[CRYSTAL_FB_XI_LIMIT_ITEM_MAX];		//��С100
	};

	enum CRYSTAL_FB_REQ_TYPE
	{
		CRYSTAL_FB_REQ_TYPE_NEXT_LAYER = 0,			//ˮ��������һ��
		CRYSTAL_FB_REQ_TYPE_EXIT_SCENE = 1,			//����þ���ȡ�꽱���󵹼�ʱ�������ߵ��ȷ���˳����� param1: 0:��Ա���(��Ա��,����) 1:�ӳ����(ȫԱ��) 2:����ʱ����(ȫԱ��)
		CRYSTAL_FB_REQ_TYPE_BUY_ENERGY = 2,			//��ɫ����Ԫ����������	
		CRYSTAL_FB_REQ_TYPE_BUY_TIMES  = 3,			//������ˮ���������� param1:fb_seq
		CRYSTAL_FB_REQ_TYPE_GET_REWARD = 4,			//������ȡ��ͨ���� param1 : fb_seq
	};

	class CSCrystalFbReq						//!< 2462 ˮ������ / ����þ�����
	{
	public:
		CSCrystalFbReq();
		MessageHeader		header;

		int req_type;
		int param1;							
	};

	class SCCrystalFbObjInfo						//!< 2464 ˮ����������������Ϣ
	{
	public:
		const static int MAX_OBJ_LIST_COUNT = 100;//!< ����·���������
		enum CRYSTAL_FB_OBJ_TYPE
		{
			CRYSTAL_FB_OBJ_TYPE_NOT = 0,		//!< δ֪��Ʒ
			CRYSTAL_FB_OBJ_TYPE_MONSTER = 1,	//!< ����
			CRYSTAL_FB_OBJ_TYPE_NPC = 2,		//!< npc
			CRYSTAL_FB_OBJ_TYPE_BOSS = 3,		//!< Boss
			CRYSTAL_FB_OBJ_TYPE_GOUHUO = 4,		//!< ����
			CRYSTAL_FB_OBJ_TYPE_COIB = 5,		//!< ͭ��
			CRYSTAL_FB_OBJ_TYPE_GATHER0 = 6,	//!< �ɼ��� - ľ��
			CRYSTAL_FB_OBJ_TYPE_GATHER1 = 7,	//!< �ɼ��� - ��ʯ
			CRYSTAL_FB_OBJ_TYPE_GATHER2 = 8,	//!< �ɼ��� - ��
			CRYSTAL_FB_OBJ_TYPE_BOX = 9,		//!< ����
			CRYSTAL_FB_OBJ_TYPE_TSD = 10,		//!< ��˵��
		};

		struct CrystalFbObj
		{
			CrystalFbObj():obj_type(0), npc_id(0),pos_x(0),pos_y(0){}
			int obj_type;
			int npc_id;
			short pos_x;
			short pos_y;
		};
		SCCrystalFbObjInfo();
		MessageHeader		header;

		int level_seq;
		int layer;				//!<��ǰ����
		int scene_id;

		int box_num;			//!<�������
		int m_box_num_max;		//!<��������

		int monster_num;		//!<���ָ���
		int m_monster_num_max;	//!<������ָ���

		int npc_num;			//!<�Ѿ���������NPC����
		int m_npc_num_max;		//!<���NPC����

		int gouhuo_num;			//!<��������
		int m_gouhuo_num_max;	//!<�����������

		int coin_num;			//!<ͭ������
		int m_coin_num_max;		//!<���ͭ������

		int gather_num;			//!<�ɼ�������
		int m_gather_num_max;	//!<���ɼ�������

		int obj_count;
		CrystalFbObj obj_list[MAX_OBJ_LIST_COUNT];
	};

	class SCAdvanceFbObjInfo						//!< 2465 ���׸�������������Ϣ
	{
	public:
		const static int MAX_OBJ_LIST_COUNT = 100;//!< ����·���������

		enum SPECIALLOGIC_OBJ_TYPE
		{
			SPECIALLOGIC_OBJ_TYPE_NO = -1,		//!< ��
			SPECIALLOGIC_OBJ_TYPE_BOSS = 0,		//!< boss
			SPECIALLOGIC_OBJ_TYPE_TRANSPROT = 1,//!< ������
			SPECIALLOGIC_OBJ_TYPE_MAX,
		};

		struct AdvanceFbObj
		{
			AdvanceFbObj() :obj_type(SPECIALLOGIC_OBJ_TYPE_NO), obj_id(-1), obj_pos_x(0),obj_pos_y(0) {}
			short obj_type;
			ObjID obj_id;
			short obj_pos_x;
			short obj_pos_y;
		};
		SCAdvanceFbObjInfo();
		MessageHeader		header;

		int obj_count;
		AdvanceFbObj obj_list[MAX_OBJ_LIST_COUNT];
	};

	class SCAdvanceFbTaskInfo						//!< 2466 ���׸�������������Ϣ
	{
	public:
		SCAdvanceFbTaskInfo();
		MessageHeader		header;

		int task_id;		//����id
		int progress;		//����
	};

	class SCFantasyFbSceneInfo						//!< ����þ�����������Ϣ 2467
	{
	public:
		SCFantasyFbSceneInfo();
		MessageHeader		header;

		int level_seq;
		int layer;				//!<��ǰ����

		int kill_boss_flag;		//BOSS��ɱ��־  0:δ��ɱ  1:�ѻ�ɱ

		int box_num;			//!<�������
		int m_box_num_max;		//!<��������

		int gather_num;			//!<�ɼ�������
		int m_gather_num_max;	//!<���ɼ�������
	};

	class SCFantasyFbObjInfo						//!< 2468 ����þ���������������Ϣ
	{
	public:
		const static int MAX_OBJ_LIST_COUNT = 100;//!< ����·���������
		enum FANTASY_FB_OBJ_TYPE
		{
			FANTASY_FB_OBJ_TYPE_NOT = 0,		//!< δ֪��Ʒ
			FANTASY_FB_OBJ_TYPE_MONSTER = 1,	//!< ����
			FANTASY_FB_OBJ_TYPE_NPC = 2,		//!< npc
			FANTASY_FB_OBJ_TYPE_BOSS = 3,		//!< Boss
			FANTASY_FB_OBJ_TYPE_GOUHUO = 4,		//!< ����
			FANTASY_FB_OBJ_TYPE_COIB = 5,		//!< ͭ��
			FANTASY_FB_OBJ_TYPE_GATHER0 = 6,	//!< �ɼ��� - ľ��
			FANTASY_FB_OBJ_TYPE_GATHER1 = 7,	//!< �ɼ��� - ��ʯ
			FANTASY_FB_OBJ_TYPE_GATHER2 = 8,	//!< �ɼ��� - ��
			FANTASY_FB_OBJ_TYPE_BOX = 9,		//!< ����
			FANTASY_FB_OBJ_TYPE_TSD = 10,		//!< ��˵��
		};

		struct FantasyFbObj
		{
			FantasyFbObj() :obj_type(0), npc_id(0), pos_x(0), pos_y(0) {}
			int obj_type;
			int npc_id;
			short pos_x;
			short pos_y;
		};
		SCFantasyFbObjInfo();
		MessageHeader		header;

		int obj_count;
		FantasyFbObj obj_list[MAX_OBJ_LIST_COUNT];
	};

	class SCFantasyFBRewardInfo						//2469 ����þ������ݴ潱����Ϣ�·�
	{
	public:
		SCFantasyFBRewardInfo();
		MessageHeader header;

		static const int MAX_ITEM_DATA_NUM = 100;
		struct ItemData
		{
			ItemData():item_id(0), is_bind(0), num(0) {}

			ItemID item_id;
			short is_bind;
			int num;
		};

		enum FANTASY_FB_TYPE
		{
			FANTASY_FB_TYPE_INFO = 0,				//�·���ҽ��������Ϣ (������Ϣ������)
			FANTASY_FB_TYPE_REWARD = 1,				//�·�����ݴ潱����Ϣ
		};

		int op_type;
		BitMap<LAYER_MAX_NUM> fantasy_finish_flag;			//�����Ƿ�����ȡ����    (������λ���� 1-4�� 0-31λʹ��1-4)							
		int reward_count;
		ItemData reward_list[MAX_ITEM_DATA_NUM];
	};
	
	class SCADVdropRewardInfo						//2471 ���׸�����������б��·�
	{
	public:
		SCADVdropRewardInfo();
		MessageHeader header;

		static const int MAX_DROP_DATA_NUM = 10;
		struct ItemData
		{
			ItemData() :item_id(0), is_bind(0), num(0) {}

			ItemID item_id;
			short is_bind;
			int num;
		};

		int reward_count;
		ItemData reward_list[MAX_DROP_DATA_NUM];
	};

	class SCCrystalFbEnergyInfo						//!< ˮ������������Ϣ		2479
	{
	public:
		SCCrystalFbEnergyInfo();
		MessageHeader		header;

		int role_enter_energy;						//!< ��������
		short buy_times;							//!< ����Ԫ��������������
		short reserve_sh;
		unsigned int next_energy_reply_tamp;		//!< ��һ�������ָ�ʱ��� -- 0������ָ�����
		unsigned int frist_pass_flag[CRYSTAL_FB_FIRST_FLAG];//!< �Ƿ��Ѿ�ͨ�ع��������ƣ�CRYSTAL_FB_FIRST_FLAG == 5
		unsigned int frist_pass_reward[CRYSTAL_FB_FIRST_FLAG];//!< �Ƿ���ȡ����ͨ����(������)
		int new_jing_ying_play_times;//��Ӣͨ�ش���
		int use_xi_you_reward_play_times;//ϡ��ͨ�ش���
		int new_xi_you_play_times;//ϡ���ܴ���
	};

	class CSCrystalFbFastPassInfo						//!< ˮ������ɨ��		2478
	{
	public:
		CSCrystalFbFastPassInfo();
		MessageHeader		header;

		int fb_seq;
		int times;
	};

	class CSCrystalFbAllNpcInfo						//!< ˮ������ɨ��		2459
	{
	public:
		CSCrystalFbAllNpcInfo();

		const static int MAX_OBJ_LIST_COUNT = 100;//!< ����·���������
		enum CRYSTAL_FB_OBJ_TYPE
		{
			CRYSTAL_FB_OBJ_TYPE_NOT = 0,		//!< δ֪��Ʒ
			CRYSTAL_FB_OBJ_TYPE_MONSTER = 1,	//!< ����
			CRYSTAL_FB_OBJ_TYPE_NPC = 2,		//!< npc
			CRYSTAL_FB_OBJ_TYPE_BOSS = 3,		//!< Boss
			CRYSTAL_FB_OBJ_TYPE_GOUHUO = 4,		//!< ����
			CRYSTAL_FB_OBJ_TYPE_COIB = 5,		//!< ͭ��
			CRYSTAL_FB_OBJ_TYPE_GATHER0 = 6,	//!< �ɼ��� - ľ��
			CRYSTAL_FB_OBJ_TYPE_GATHER1 = 7,	//!< �ɼ��� - ��ʯ
			CRYSTAL_FB_OBJ_TYPE_GATHER2 = 8,	//!< �ɼ��� - ��
			CRYSTAL_FB_OBJ_TYPE_BOX = 9,		//!< ����
			CRYSTAL_FB_OBJ_TYPE_TSD = 10,		//!< ��˵��
		};

		struct CrystalFbAllObjInfo
		{
			CrystalFbAllObjInfo() : objid(-1), obj_type(0), npc_id(0), pos_x(0), pos_y(0) {}
			int objid;
			int obj_type;
			int npc_id;
			short pos_x;
			short pos_y;
		};

		MessageHeader		header;

		int obj_count;
		CrystalFbAllObjInfo obj_list[MAX_OBJ_LIST_COUNT];
	};

	class SCMiJingQiWenFbObjInfo						//!< 2620 ˮ����������������Ϣ
	{
	public:
		const static int MAX_OBJ_LIST_COUNT = 100;//!< ����·���������
		enum MI_JING_QI_WEN_FB_OBJ_TYPE
		{
			MI_JING_QI_WEN_FB_OBJ_TYPE_NOT = 0,		//!< δ֪��Ʒ
			MI_JING_QI_WEN_FB_OBJ_TYPE_MONSTER = 1,	//!< ����
			MI_JING_QI_WEN_FB_OBJ_TYPE_NPC = 2,		//!< npc
			MI_JING_QI_WEN_FB_OBJ_TYPE_BOSS = 3,		//!< Boss
			MI_JING_QI_WEN_FB_OBJ_TYPE_GOUHUO = 4,		//!< ����
			MI_JING_QI_WEN_FB_OBJ_TYPE_COIB = 5,		//!< ͭ��
			MI_JING_QI_WEN_FB_OBJ_TYPE_GATHER0 = 6,	//!< �ɼ��� - ľ��
			MI_JING_QI_WEN_FB_OBJ_TYPE_GATHER1 = 7,	//!< �ɼ��� - ��ʯ
			MI_JING_QI_WEN_FB_OBJ_TYPE_GATHER2 = 8,	//!< �ɼ��� - ��
			MI_JING_QI_WEN_FB_OBJ_TYPE_BOX = 9,		//!< ����
			MI_JING_QI_WEN_FB_OBJ_TYPE_TSD = 10,		//!< ��˵��
			MI_JING_QI_WEN_FB_OBJ_TYPE_QUESTION = 11,	//!< ����
		};

		struct MiJingQiWenFbObj
		{
			MiJingQiWenFbObj() :obj_type(0), npc_id(0), pos_x(0), pos_y(0) {}
			int obj_type;
			int npc_id;
			short pos_x;
			short pos_y;
		};
		SCMiJingQiWenFbObjInfo();
		MessageHeader		header;

		int level_seq;
		int layer;				//!<��ǰ����
		int scene_id;

		int box_num;			//!<�������
		int m_box_num_max;		//!<��������

		int monster_num;		//!<���ָ���
		int m_monster_num_max;	//!<������ָ���

		int npc_num;			//!<�Ѿ���������NPC����
		int m_npc_num_max;		//!<���NPC����

		int gouhuo_num;			//!<��������
		int m_gouhuo_num_max;	//!<�����������

		int coin_num;			//!<ͭ������
		int m_coin_num_max;		//!<���ͭ������

		int gather_num;			//!<�ɼ�������
		int m_gather_num_max;	//!<���ɼ�������

		int obj_count;
		MiJingQiWenFbObj obj_list[MAX_OBJ_LIST_COUNT];
	};

	class SCMiJingQiWenFbObjInfo2						//!< 4170 ˮ����������������Ϣ
	{
	public:
		const static int MAX_OBJ_LIST_COUNT = 100;//!< ����·���������
		enum MI_JING_QI_WEN_FB_OBJ_TYPE
		{
			MI_JING_QI_WEN_FB_OBJ_TYPE_NOT = 0,		//!< δ֪��Ʒ
			MI_JING_QI_WEN_FB_OBJ_TYPE_MONSTER = 1,	//!< ����
			MI_JING_QI_WEN_FB_OBJ_TYPE_NPC = 2,		//!< npc
			MI_JING_QI_WEN_FB_OBJ_TYPE_BOSS = 3,		//!< Boss
			MI_JING_QI_WEN_FB_OBJ_TYPE_GOUHUO = 4,		//!< ����
			MI_JING_QI_WEN_FB_OBJ_TYPE_COIB = 5,		//!< ͭ��
			MI_JING_QI_WEN_FB_OBJ_TYPE_GATHER0 = 6,	//!< �ɼ��� - ľ��
			MI_JING_QI_WEN_FB_OBJ_TYPE_GATHER1 = 7,	//!< �ɼ��� - ��ʯ
			MI_JING_QI_WEN_FB_OBJ_TYPE_GATHER2 = 8,	//!< �ɼ��� - ��
			MI_JING_QI_WEN_FB_OBJ_TYPE_BOX = 9,		//!< ����
			MI_JING_QI_WEN_FB_OBJ_TYPE_TSD = 10,		//!< ��˵��
			MI_JING_QI_WEN_FB_OBJ_TYPE_QUESTION = 11,	//!< ����
		};

		struct MiJingQiWenFbObj
		{
			MiJingQiWenFbObj() :obj_type(0), npc_id(0), pos_x(0), pos_y(0) {}
			int obj_type;
			int npc_id;
			short pos_x;
			short pos_y;
		};
		SCMiJingQiWenFbObjInfo2();
		MessageHeader		header;

		int level_seq;
		int layer;				//!<��ǰ����
		int scene_id;

		int box_num;			//!<�������
		int m_box_num_max;		//!<��������

		int monster_num;		//!<���ָ���
		int m_monster_num_max;	//!<������ָ���

		int npc_num;			//!<�Ѿ���������NPC����
		int m_npc_num_max;		//!<���NPC����

		int gouhuo_num;			//!<��������
		int m_gouhuo_num_max;	//!<�����������

		int coin_num;			//!<ͭ������
		int m_coin_num_max;		//!<���ͭ������

		int gather_num;			//!<�ɼ�������
		int m_gather_num_max;	//!<���ɼ�������

		int obj_count;
		MiJingQiWenFbObj obj_list[MAX_OBJ_LIST_COUNT];
	};
}

#pragma pack(pop)

#endif

