#ifndef __MSG_TASK_H__
#define __MSG_TASK_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/taskdef.h"
#include "servercommon/taskchaindef.h"
#include "servercommon/anecdotetaskdef.hpp"
#include "servercommon/prestigetaskdef.hpp"
#include "gamedef.h"
#include "servercommon/treasuretaskdef.h"

#pragma pack(push, 4)

namespace Protocol
{
	class CSTaskInfoSingleHandle
	{
	public:
		CSTaskInfoSingleHandle();		//!< 1720 - ���������������
		MessageHeader header;

		enum TASK_OP_TYPE
		{
			TASK_ACCEPT = 0,		//!< ��������
			TASK_GIVEUP = 1,		//!< ��������
			TASK_COMMIT = 2,		//!< �ύ����
			TASK_DEMAND = 3,		//!< ��ѯ����״̬		0 - δ���У�	1 - ����ɣ�	2 - ������
			TASK_PATROL = 4,		//!< Ѳ������			0 - �����·�    1 - ��ȡ����    2-�����ύ
			TASK_SKIP_BATTLE = 5,	//!< ��������ս��
		};

		UInt16 op_type;			//!< ushort ��������

		TaskID task_id;			//!< ushort ����ID
	};
	
	class SCTaskDetailedInfoList
	{
	public:
		SCTaskDetailedInfoList();			//!< 1721 - �ɽӺ��ѽӵ���Ϣ�ܱ�
		MessageHeader header;

		struct TaskProCon
		{
			TaskInfoPro task_con;
			int list_type;			//!< �����б����� - 0���ѽ��б� | 1���ɽ��б�
		};

		int list_count;		//!< ��������
		TaskProCon task_list_info[MAX_TASK_LIST_LEN_NUM];
	};

	class SCTaskDeleteNotifyList
	{
	public:
		SCTaskDeleteNotifyList();			//!< 1722 - ���ܱ����Ƴ�������Ϣ
		MessageHeader header;

		struct TaskRemoveCon
		{
			short remove_reason;		//!< �Ƴ�ԭ�� 0 - ����ԭ�� 1 - �������
			unsigned short task_id;
		};

		int list_count;
		TaskRemoveCon info_list[MAX_TASK_LIST_LEN_NUM];
	};

	class CSTalkWithAppointedNPC
	{
	public:
		CSTalkWithAppointedNPC();		//!< 1723 - ������ָ���� NPC ��̸
		MessageHeader header;

		TaskID task_id;		//!< ushort ����ID
		UInt16 npc_id;		//!< ushort NPC ID
	};
	
	class CSGiveNPCTaskNeedItems
	{
	public:
		CSGiveNPCTaskNeedItems();		//!< 1724 - �ύ����(�б�)
		MessageHeader header;

		TaskID task_id;		//!< ushort ����ID
		UInt16 list_count;	//!< ushort �ύ�ĵ�������

		ItemContainer commit_list[MAX_ITEM_NUM_IN_GROUP];	//!< ���߱�
	};

	class SCTaskCompleteTaskList
	{
	public:
		SCTaskCompleteTaskList();		//!< 1725 - ����ɵ������б�
		MessageHeader header;

		int list_count;			//!< �б���
		TaskID task_list[MAX_TASK_RECORD_NUM];	//!< ����ID
	};

	class SCTaskCanAbandonedList
	{
	public:
		SCTaskCanAbandonedList();		//!< 1726 - �����ܷ񱻷���	0 - ���ܣ� 1 - ����
		MessageHeader header;

		int list_num;		//!< �б���
		ItemContainer can_abandon_list[MAX_ACCEPTED_TASK_NUM];	//!< ��Ϣ�б�
	};

	class SCTaskSingleStatusInfo
	{
	public:
		SCTaskSingleStatusInfo();		//!< 1727 - ��������ǰ״̬   0 - δ���У� 1 - ����ɣ� 2 - ������
		MessageHeader header;

		ItemContainer info_con;		//!< ״̬����
	};

	class CSTaskChainOp				//!<1728	����������
	{
	public:
		CSTaskChainOp();
		MessageHeader header;

		enum OP_TYEP
		{
			ACCEPT = 0,			//!< ��ȡ
			GIVEUP = 1,			//!< ����
			DELIVERY = 2,		//!< ����	column|index
			TALK = 3,			//!< ��̸
			SKIP = 4,			//!< ����
			DELIVERY_PET = 5,	//!< �Ͻ����� pet_index
			FETCH_REWARD = 6,	//!< ��ȡת�̽���
			WAIT_ITEM = 7,		//!< ��ȡ���͵���
		};

		int type;
		int param1;
		int param2;
	};

	class SCTaskChainInfo		//!<1729	��������Ϣ
	{
	public:
		SCTaskChainInfo();
		MessageHeader header;

		int pass_count;								//!< ����˶��ٴ�
		int task_type;								//!< ��ǰ��������
		int task_param[TASK_CHAIN_PARAM_COUNT];		//!< ����Ĳ���
		int can_skip;								//!< �ܷ�����
		int price_type;								//!< ��������
		int price;									//!< ���ѽ��
		int task_count;								//!< �׶�
		short is_new;								//!< �Ƿ���ʾΪ������
		short completion_times;						//!< ��ҵ�ǰ���������ܻ���
		long long task_limit_timestamp;				//!< ����ʱ�����ƴ�
		short task_pass_count;						//!< ���׶ε��������
		short cur_task_max_count;					//!< ���׶ε������
		int accept_task_level;						//!< ÿ�ν�ȡ����ĵȼ�
	};
	UNSTD_STATIC_CHECK(10 == TASK_CHAIN_PARAM_COUNT);

	class CSSetupTaskTraceStatus
	{
	public:
		CSSetupTaskTraceStatus();		//!< 2008 - ��������׷��״̬
		MessageHeader header;

		int list_num;
		unsigned int param_list[TASK_TRACE_MAX_NUM];
	};

	class SCTaskTraceInformation
	{
	public:
		SCTaskTraceInformation();		//!< 2009 - �·�����׷������
		MessageHeader header;

		int list_num;
		unsigned int param_list[TASK_TRACE_MAX_NUM];
	};

	class SCTaskChainMonsterInfo
	{
	public:
		SCTaskChainMonsterInfo();		//!< 2016 - ������ - ˢ��֪ͨ
		MessageHeader header;

		int monster_id;
		int monster_group_id;
		int scene_id;
		int pos_x;
		int pos_y;
	};

	class CSTaskGameReq				//!< 2026 - ����С��Ϸ
	{
	public:
		CSTaskGameReq();
		MessageHeader header;
		int task_game_type;
		int param1;
		int param2;
	};

	class CSTaskCommitMoneyReq				//!< 2029 - �ύ����
	{
	public:
		CSTaskCommitMoneyReq();
		MessageHeader header;
		
		int task_id;
	};

	
	class CSTaskSkipReq				//!< 4434 - ��������-ͨ��
	{
	public:
		CSTaskSkipReq();
		MessageHeader header;

		int task_id;
	};

	class SCPatrolTaskInfo
	{
	public:
		SCPatrolTaskInfo();		//!< 2020 - Ѳ������
		MessageHeader header;

		char today_flag;				//�Ƿ���ȡ��Ѳ������
		char today_reward;				// �Ƿ��ȡ��Ѳ��������
		short task_progress;			//�������
		short task_status;				//��ȡ״̬  0-������ȡ 1-����ȡ(��δ��ȡ) 2-����ȡ
		unsigned int task_coin;			//��ȡͭ��
	};

	class SCBountyTaskTurnTableRewardInfo	//!< 2184 �ͽ�����ת�̽�����Ϣ
	{
	public:
		SCBountyTaskTurnTableRewardInfo();
		MessageHeader header;

		short reward_idx;		//!< ת��ָ��Ľ��� ֵΪ-1ʱ��Ч
		short group_id;			//!< ������ID
	};

	class CSBountyTaskTurnTableRewardFetch  //!< 2183 ��ȡ�ͽ�����ת�̽���
	{
	public:
		CSBountyTaskTurnTableRewardFetch();
		MessageHeader header;
	};

	class SCBountyTaskInfo		// 2185 �ͽ����������Ϣ
	{
	public:
		SCBountyTaskInfo();
		MessageHeader header;

		short cur_finish_task_num;	//!< ��ǰ�������
		short today_task_num;		//!< �����ܹ�Ҫ����������
		short has_finish_first_task;//!< �Ƿ�����˵�һ������
		short reserve_sh;			//!< ������
	};

	class CSTaskChosedReq		// 3234 ����ѡ���¼����
	{
	public:
		enum TASK_CHOSED_REQ_TYPE
		{
			TASK_CHOSED_REQ_TYPE_INFO = 0,	//!< �����·�
			TASK_CHOSED_REQ_TYPE_SET = 1,	//!< ����
		};
		CSTaskChosedReq();
		MessageHeader header;

		unsigned short req_type;
		unsigned short param1;
		unsigned short param2;
		unsigned short param3;
	};

	class SCTaskChosedInfo		// 3235 ����ѡ���¼�·�
	{
	public:
		SCTaskChosedInfo();
		MessageHeader header;

		int info_count;
		TaskChosedInfo chosed_list[MAX_TASK_CHOSED_RECORD_NUM];
	};

	class SCTaskChainTurnTableInfo	//!< 3236 ��������Ϣת���·�
	{
	public:
		SCTaskChainTurnTableInfo();
		MessageHeader header;

		short turn_table_index;				// ת������Ľ����±�
		short turn_table_group_id;			// ת�̽�����
	};

	struct AnecdoteTaskMsgInfo
	{
		short anecdote_task_type;	//!< ������������ \see ANECDOTE_TASK_TYPE
		unsigned short cur_task_id;	//!< ��ǰ����ID
		short has_finish;			//!< �Ƿ�����ɸ�������������
		short accept_status;		//!< ��ǰ����Ľ���״̬ 0:�ɽ��� 1:�ѽ���
		int progress;				//!< �������
	};

	class SCAnecdoteTaskInfo	//!< 3490 ��������������Ϣ�·� ���ı�����ʱ�·���
	{
	public:
		SCAnecdoteTaskInfo();
		MessageHeader header;

		AnecdoteTaskMsgInfo task_info;
	};

	class SCAnecdoteTaskInfoList	//!< 3491 ���������б�  ����ʱ�·�
	{
	public:
		SCAnecdoteTaskInfoList();
		MessageHeader header;

		int count;
		AnecdoteTaskMsgInfo task_info_list[ANECDOTE_TASK_TYPE_MAX];
	};

	class SCAnecdoteBigEventInfo		//!< 3499 ���Ŵ��¼���Ϣ�·�
	{
	public:
		SCAnecdoteBigEventInfo();
		MessageHeader header;

		char reward_version;				//!< �����汾��
		char is_big_event_open_now;			//!< ���¼��Ƿ�����
		short anecdote_finish_num;			//!< �����������
		unsigned int end_timestamp;			//!< ���¼�����ʱ���
		unsigned int reward_fetch_status;	//!< ���¼�������ȡ��ǣ���������ʽ��
	};

	class SCAnecdoteTaskBattleFail		//!< 3492 ��������ս��ʧ��
	{
	public:
		SCAnecdoteTaskBattleFail();
		MessageHeader header;

		short anecdote_task_type;
		unsigned short task_id;
	};

	class CSAnecdoteTaskOperate			//!< 3494 ������������
	{
	public:
		CSAnecdoteTaskOperate();
		MessageHeader header;

		enum OPERATE_TYPE
		{
			OPERATE_TYPE_ACCEPT_TASK = 0, //!< ��������  p1 ����ID
			OPERATE_TYPE_COMMIT_TASK = 1, //!< �ύ����  p1 ����ID
			OPERATE_TYPE_CLAIM_REWARD = 2,//!< ��ȡ������ p1 ������������ \see ANECDOTE_TASK_TYPE
			OPERATE_TYPE_FETCH_BIG_EVENT_REWARD = 3, //!< ��ȡ���¼�����  p1 ����seq
		};

		short operate_type;		//!< ��������
		unsigned short param1;	//!< ����1
	};

	class SCAnecdoteTaskCompleteNotice	//!< 3493 �����������֪ͨ
	{
	public:
		SCAnecdoteTaskCompleteNotice();
		MessageHeader header;

		unsigned short task_id;
		short anecdote_task_type;
	};

	class SCAnecdoteTaskSpecialInfo	//!< 3495 ��������-���������·�
	{
	public:
		SCAnecdoteTaskSpecialInfo();
		MessageHeader header;

		int special_value[ANECDOTE_TASK_TYPE_MAX];
	};

	class CSAnecdoteTaskSpecialReq	//!< 3496 ��������-������������
	{
	public:
		CSAnecdoteTaskSpecialReq();
		MessageHeader header;

		int req_type;//��������
		int req_value;
	};

	class CSNewTaskAction		//!< 3500 �µ�������Ϊ����
	{
	public:
		CSNewTaskAction();
		MessageHeader header;

		short action_type;		//!< ��Ϊ���� \see NEW_TASK_ACTION 
		short reserve_sh;		//!< ������
		int param1;
		int param2;
		int param3;
		int param4;
	};

	class CSNewTaskGiveNPCTaskNeedItems
	{
	public:
		CSNewTaskGiveNPCTaskNeedItems();		//!< 3504 - �ύ����(�б�)
		MessageHeader header;

		TaskID task_id;		//!< ushort ����ID
		UInt16 list_count;	//!< ushort �ύ�ĵ�������

		ItemContainer commit_list[MAX_ITEM_NUM_IN_GROUP];	//!< ���߱�
	};
	
	class SCNewTaskPartnerSoarInfo		 //!< 3505 ���������������Ϣ
	{
	public:
		SCNewTaskPartnerSoarInfo();
		MessageHeader header;

		char is_cur_accept_partner_soar_task;	// �Ƿ��ǵ����ȡ�Ļ��������� 1:���� 0:��ʼ״̬ 2:���ǵ���
		char reserve_ch;					
		short reserve_sh;			
	};

	class CSNormalTaskFinishAnedoteGame	//!< 3497 ��ͨ�����������С��Ϸ������������
	{
	public:
		CSNormalTaskFinishAnedoteGame();
		MessageHeader header;

		TaskID task_id;
		short anedote_type;
	};

	class SCResearchTaskNoticInfo	//!< 3498 �����ħ�������
	{
	public:
		SCResearchTaskNoticInfo();
		MessageHeader header;
		const static int MAX_REWARD_LIST = 10;//���10��
		struct Item_info
		{
			Item_info() : item_id(0), num(0), is_bind(0) {}
			ItemID item_id;
			short num;
			int is_bind;
		};
		long long add_exp;
		
		int reward_num;
		Item_info reward_list[MAX_REWARD_LIST];
	};

	class SCPrestigeTaskInfo	//!< 2290 �������������·�
	{
	public:
		struct TaskData
		{
			UInt16 task_id;
			short star;
		};

		SCPrestigeTaskInfo();
		MessageHeader header;

		short village_id;				//!< ����ѡ��Ĵ�ׯID (0��ʾδѡ��)
		short is_accept_task;			//!< �Ƿ��ȡ����
		short today_finish_task_num;	//!< ��������ɵ�������
		short today_task_num;			//!< �����������
		TaskData today_task_list[MAX_PRESTIGE_TASK_NUM_PER_DAY]; //!< ���������б� (���鰴today_task_num��ȡ��
	};

	class CSPrestigeTaskSelectVillage	//!< 2291 ѡ�����������ׯ
	{
	public:
		CSPrestigeTaskSelectVillage();
		MessageHeader header;

		int village_id;					//!< ѡ��Ĵ�ׯid
	};

	class SCPrestigeTaskFinalNotice		//!< 2292 �����������֪ͨ
	{
	public:
		SCPrestigeTaskFinalNotice();
		MessageHeader header;

		int village_id;					//!< ѡ��Ĵ�ׯid
		int total_add_prestige;			//!< �ܼƻ������
	};

	class CSTreasureTaskReq					// 4349
	{
	public:
		CSTreasureTaskReq();
		MessageHeader header;

		short type;
		short param;
	};

	class SCTreasureTaskInfo				// 4350
	{
	public:
		SCTreasureTaskInfo();
		MessageHeader header;

		short level;					// ������ʱ�ĵȼ�
		short order_id;					// ��ǰ����ID
		short route;					// ·��
		short pos_id;					// �����ID

		short pool[TREASURE_TASK_POOL_NNM];		// ���� 8
	};

	class CSPrestigeTaskReq	//!< 2293 ������������
	{
	public:
		CSPrestigeTaskReq();
		MessageHeader header;

		short type;
		short param;
	};

	class SCTaskWaitingList		// 1352 �ȴ������б�
	{
	public:
		SCTaskWaitingList();
		MessageHeader header;

		struct WaitingTask
		{
			TaskID task_id;		// ����id
			short reserve_sh;	// ������
			unsigned int wait_until_timestamp; // ���񿪷ŵ�ʱ���
		};

		int count;
		WaitingTask task_list[100];
	};

	class CSSpecialAnecdoteReq		// 1354 ������������ʼ
	{
	public:
		CSSpecialAnecdoteReq();
		MessageHeader header;

		int task_type;
		int task_seq;
	};

	class SCSpecialAnecdoteList		// 1355 �������������б��·�
	{
	public:
		SCSpecialAnecdoteList();
		MessageHeader header;

		SpecialTaskInfo task_list[MAX_SPECIAL_ANECDOTE_TASK_TYPE];
	};

	class SCSpecialAnecdoteSingleList		// 1356 �������������·� - ����
	{
	public:
		SCSpecialAnecdoteSingleList();
		MessageHeader header;
		int task_type;
		SpecialTaskInfo task_info;
	};
}

#pragma pack(pop)
#endif