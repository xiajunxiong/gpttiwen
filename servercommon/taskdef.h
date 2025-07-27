#ifndef __TASK_DEF_H__
#define __TASK_DEF_H__

#include "engineadapter.h"

#pragma pack(push, 4)

static const int MAX_ACCEPTED_TASK_NUM = 350;		// ���ͬʱӵ�е���������
static const int MAX_ACCEPTED_TASK_NUM_ALLOW = 300; // �����������ߣ���������ɽ�����
static const int MAX_TASK_LIST_LEN_NUM = 2048;		// ���η��͵������б�ĳ���
static const int MAX_ITEM_NUM_IN_GROUP = 10;		// ÿ�������������ӵ�еĵ�������
static const int TASK_EXPIRE_CHECK_INTERVAL = 3;	// ����ʱ�����
static const int TASK_TRACE_MAX_NUM = 350;			// �������׷������
UNSTD_STATIC_CHECK(TASK_TRACE_MAX_NUM >= MAX_ACCEPTED_TASK_NUM);

static const int MAX_TASK_RECORD_NUM = 4990;
static const int MAX_TASK_RECORD_LEN = sizeof(short) + sizeof(short) * MAX_TASK_RECORD_NUM;
static const int MAX_TASK_CHOSED_RECORD_NUM = 512;

static const int TASK_NEED_HAVE_PROPERTY_LIST_REQUIRE_NUM = 1;

enum TASK_TYPE		// ��������
{
	TASK_TYPE_MAIN_QUEST = 0,				// ��������
	TASK_TYPE_SIDE_QUEST = 1,				// ֧������
	TASK_TYPE_CHALLENGES = 2,				// ��ս����
	TASK_TYPE_STAGE_TASK = 3,				// ��������
	TASK_TYPE_MEDAL_TASK = 4,				// ѫ������
	TASK_TYPE_TRMAP_TASK = 5,				// ��ͼ����
	TASK_TYPE_PROFS_TASK = 6,				// ��������(ְҵ���� / ѧԺ����)
	TASK_TYPE_CHAIN_TASK = 7,				// �ܳ����� - ������
	TASK_TYPE_GUILD_TASK = 8,				// ��������
	TASK_TYPE_RESEARCHES = 9,				// ��ħ����(���е��о�)
	TASK_TYPE_ADVENTURES = 10,				// ��������
	TASK_TYPE_SPECIAL_QUEST = 11,			// �������񣨸��߻��õģ��Ƽ�����
	TASK_TYPE_BOUNTY_TASK = 12,				// �ͽ�����
	TASK_TYPE_TRANSACTION_TASK = 13,		 //�������� - ˿��֮·
	TASK_TYPE_ANECDOTE_TASK = 14,			// ��������
	TASK_TYPE_PRESTIGE_TASK = 15,			// ��������
	TASK_TYPE_FLY_UP_TASK = 16,				// ��ɫ��������
	TASK_TYPE_PARTNER_SOAR_TASK = 17,		// ����������
	TASK_TYPE_PARTNER_SOAR_FRONT_TASK = 18,	// ������ǰ������
	TASK_TYPE_PET_FEI_SHENG_TASK = 19,		// �����������
	TASK_TYPE_FLY_UP_FRONT_TASK = 20,		// ��ɫ����ǰ������
	TASK_TYPE_DIANHUA_TASK = 21,	// �㻯����
	TASK_TYPE_STAGE_TASK_1 = 22,	//��������1
	TASK_TYPE_SERIAL_FEEDER_TASK = 23,		// ����ʽ֧������
	TASK_TYPE_SERIAL_FEEDER_TASK2 = 24,		// ����ʽ֧������2
	TASK_TYPE_SERIAL_FEEDER_TASK_HOLY = 25,	// ʥ������֧������
	TASK_TYPE_TEAM_CHALLENGES = 26,			// �����ս����
	TASK_TYPE_ROLE_GUILD_TASK = 27,		// ��ɫ��������

	TASK_TYPE_MAX
};

enum TASK_TEAM_LIMIT	// �������
{
	TASK_TEAM_UNCONFINED = 0,		// û������
	TASK_TEAM_MUST_ALONE = 1,		// ���뵥�����
	TASK_TEAM_AT_LEAST_2 = 2,		// ���� 2 �����
	TASK_TEAM_AT_LEAST_3 = 3,		// ���� 3 �����
	TASK_TEAM_AT_LEAST_4 = 4,		// ���� 4 �����
	TASK_TEAM_AT_LEAST_5 = 5,		// ���� 5 �����

	TASK_TEAM_TYPE_MAX				// ��������������ֵ
};

enum TASK_CONDITION		// �������
{
	TASK_NEED_TO_DO_NOTHING = 0,		// ʲô������
	TASK_NEED_TALK_WITH_NPC = 1,		// �� NPC ��̸
	TASK_NEED_KILL_MONGROUP = 2,		// ����������е��������
	TASK_NEED_PASS_TRANSCRI = 3,		// ͨ�ظ�������
	TASK_NEED_INTO_SCENARIO = 4,		// ���볡��ID
	TASK_NEED_LOAD_PROGRESS = 5,		// ���ؽ�����(�����ڲɼ�������ɶ����Ҫ�������Ĳ���)
	TASK_NEED_GIVE_PROGROUP = 6,		// ���������
	TASK_NEED_USES_PROGROUP = 7,		// ʹ�õ�����
	TASK_NEED_BUY_PROPERTY = 8,			// �������
	TASK_NEED_PLAY_CUTSCENE = 9,		// ���ž��鶯��
	TASK_NEED_KILL_MONSTERS = 10,		// �����ֹ���
	TASK_NEED_GIVE_PROPERTY = 11,		// �ύ���ֵ���
	TASK_NEED_USES_PROPERTY = 12,		// ʹ�õ��ֵ���
	TASK_NEED_KILL_IN_ORDER = 13,		// ��˳���ɱ����
	TASK_NEED_HAVE_PROPERTY = 14,		// ӵ�е���
	TASK_NEED_BEAT_MONGROUP = 15,		// սʤĳ���ؿ� / ����ĳ�����
	TASK_NEED_MEET_REQUIRES = 16,		// ��������
	TASK_NEED_TRAP_CREATURE = 17,		// ��׽����
	TASK_NEED_GAIN_TASKPROP = 18,		// ɱ���ռ��������
	TASK_NEED_ENTER_FB = 19,			// ���븱������
	TASK_NEED_ESCORT = 20,				// ����
	TASK_NEED_COMMIT_PROPERTY = 21,		// �ύ��Ʒ���ύ����ʱ������
	TASK_NEED_PASS_DESIGNATED_MEDAL_FB = 22,	// ͨ��ָ��ѫ�¸��� p1:ָ������seq
	TASK_NEED_HAVE_PROPERTY_LIST = 23,	// ӵ�е�����(���������������)
	TASK_NEED_MEET_ANECDOTE_REQUIRES = 24, // ��������NPC������ p1:��������
	TASK_NEED_PASS_CRYSTAL_FB_SEQ = 25,	// ͨ��ָ��ˮ������ p1:ָ��ˮ������seq p2:ָ������
	TASK_NEED_ACCEPT_ESCORT = 26,		// ����
	TASK_NEED_COURAGE_CHALLENGE = 27,	// ͨ����ħ֮��
	TASK_NEED_CREATE_ROLE_DAYS = 28,	// �������� 
	TASK_NEED_MATERIAL = 29,			// ���龳 p1:chapter_id(��1��ʼ) p2:level(��1��ʼ)
	TASK_NEED_MI_YI = 30,				// Զ��ħڣ p1:Զ����һ�� p2:������
	TASK_NEED_WAIT_UNTIL_NEXT_DAY = 31,	// �ȴ�ֱ���ڶ��� 
	TASK_NEED_PARTNER_LEVEL_ACHIVE = 32,// ���ﵽָ���ȼ� p1 partner_id p2 level
	TASK_NEED_VILLAGE_LEVEL_ACHIVE = 33,// ָ����ׯ�����ﵽָ���ȼ� p1 village_id p2 level
	TASK_NEED_COLLECTION_TU_JIAN_ACTIVE_NUM = 34,	// �ռ�ͼ��������� param2 ����
	TASK_NEED_ONE_FA_BAO_UP_LEVEL = 35,		// ������ 1 �������ټ�  param2 ����
	TASK_NEED_OPEN_YIN_YANG_EYE = 36,		// ���������� 1:�� 0:��
	TASK_NEED_PLAY_GAME = 37,				// ����Ϸ p1:��Ϸ���� p2(ʤ������)
	TASK_NEED_PASS_WANLING_FB = 38,		// ͨ�����黭�� p1:����ID   p2:����
	TASK_NEED_CAREER_ADVANCE = 39,		// ְҵ����  p1:���  p2:С��
	TASK_NEED_COMMIT_MONEY = 40,			// �ύ���� param1 �������� param2 ����
	TASK_NEED_DREAM_NOTES_CHALLENGE = 41,	// ͨ����Ԩ��¼ param1:����
	TASK_NEED_PARTNER_STRENGTHEN = 42,		 // ָ�����ǿ�� P1:���ID P2:ǿ������
	TASK_NEED_PARTNER_BREAK = 43,			 // ָ�������� P1:���ID P2:��������
	TASK_NEED_ROLE_BEAT_MONGROUP = 44,		 // ��Ҹ��˹�   p1 ����id 
	TASK_NEED_COMMIT_PET = 45,		 //�ύ���� p1 �ǳ���id

	TASK_NEED_MAX
};

enum TASK_GAME_PLAY_TYPE//TASK_NEED_PLAY_GAME P1 ö��
{
	TASK_GAME_PLAY_TYPE_0 = 0,//Ԫ�ؿ���
	TASK_GAME_PLAY_TYPE_1 = 1,//�ƻ���̳ p2�ƻ�����
	TASK_GAME_PLAY_TYPE_2 = 2,//�޲��� p2�޸�����
	TASK_GAME_PLAY_TYPE_3 = 3,//���� p2���д���
	TASK_GAME_PLAY_TYPE_MAX,
};

enum TASK_MEET_REQUIRES_TYPE	// 16 ��ö�ٵ� ����1
{
	TASK_MEET_PROF_PROMOTE = 0,		// ְҵ���� - ����2 ���׵ȼ�
	TASK_MEET_HAVE_PARTNER = 1,		// ӵ�л�� - ����2 ���ID
	TASK_MEET_LEARN_SKILLS = 2,		// ѧϰ����
	TASK_MEET_JOIN_BATTLES = 3,		// ����ս�� - ����2 ����(��)ID
	TASK_MEET_OPEN_A_CHEST = 4,		// �򿪱��� - ����2 ����ID
	TASK_MEET_OWN_CREATURE = 5,		// ӵ�г��� - ����2 ����ID
	TASK_MEET_PLAY_ONCE_COLOSSEUM = 6,	// �μ�һ�ι��񱦿�
	TASK_MEET_TRANSACTION_TASK = 7,		//˿��֮·
	TASK_MEET_MEDAL_TASK_PLAY_VIDEO = 8, // ѫ��������ܼ�
	TASK_MEET_PET_RELEASE = 9,		// �������
	TASK_MEET_PET_COMPOSE = 10,		// ����ϳ�
	TASK_MEET_DREAM_NOTES_CHALLENGE = 11,		// ͨ����Ԩ��¼ param2:Ŀ�����
	TASK_MEET_BOUNTY_TASK = 12,					// �ͽ����� param2:��ɴ���
	TASK_MEET_RESEARCH_TASK = 13,				// �����ħ param2����ɴ���
	TASK_MEET_PET_MONOPOLY_CATCH_PET = 14,	// ��赺���Xֻ���� param2:ֻ��
	TASK_MEET_LUN_HUI_WANG_CHUAN = 15,		// �ֻ�����ͨ���ڼ��� param2:����
	TASK_MEET_GOD_PRINT_WISH = 16,	// ������� param 2 ����
	TASK_MEET_SKILL_LEVEL = 17,		// ���Ǽ��ܵȼ�  param2:�ȼ�
	TASK_MEET_JOIN_GUILD = 18,		// ����һ������
	TASK_MEET_TREASURE_TASK = 19,	// �����ر� param2 ����
	TASK_MEET_BOUNTY_LAST_TASK = 20,// ���N���ͽ����� param2 ����
	TASK_MEET_TREASURE_LAST_TASK = 21,	// ���N�־����ر� param2 ����
	TASK_MEET_BOUNTY_TASK2 = 22,	// ���2*N���ͽ����� param2 ����(NҪС��һ���������/2)
	TASK_MEET_WANLING_FB_TIMES = 23,// �������N�����黭��  param2 ����		
	TASK_MEET_KILL_WILD_BOSS = 24,	// ����Ұ������ param2 ����
	TASK_MEET_REALM_BREAK_FIGHT = 25, // ����֮��ͨ�� P2:�ڼ��� 

	TASK_MEET_TYPE_MAX
};

enum TASK_GIVE_UP_TYPE
{
	GIVEUP_TYPE_CAPTAIN_CAN = 0,	// �����Ϊ���ӳ���ʱ���ܲ��ܷ�������
	GIVEUP_TYPE_CAPTAIN_ALL = 1,	// �����Ϊ���ӳ���ʱ�����������Ƿ񲨼�ȫ��
	GIVEUP_TYPE_MEMBERS_CAN = 2,	// �����Ϊ����Ա��ʱ���ܲ��ܷ�������Ĭ�ϲ��Შ��ȫ�ӣ�
	GIVEUP_TYPE_ONESELF_CAN = 3,	// �����Ϊ�����ˡ�ʱ���ܲ��ܷ�������������û����ӵ����

	GIVEUP_TYPE_MAX
};

enum TASK_REWARD_ALLOT_TYPE		// ��������������	AT -> According to������
{
	ALLOT_TYPE_COMMON = 0,		// ֻҪ������񣬽���һ�Ź̶�
	ALLOT_TYPE_ATPROF = 1,		// ����ְҵ��ͬ������Ҳ������ͬ
	ALLOT_TYPE_ATLEVE = 2,		// ���ݵȼ���ͬ������Ҳ������ͬ
	ALLOT_TYPE_PET_CHOICE = 3,	// ������ʷѡ��ĳ��ｱ��seq

	ALLOT_TYPE_MAX
};

enum TASK_STATE_TYPE
{
	TASK_STATE_UNDERWAY = 0,	// ������
	TASK_STATE_FINISHED = 1,	// ���ύ

	TASK_STATE_MAX
};

enum SPECIAL_ENUM
{
	TASK_IS_SHARE_NONE = 0,			// �����Ƿ�ȫ��ͬʱ���� - ��
	TASK_IS_SHARE_TRUE = 1,			// �����Ƿ�ȫ��ͬʱ���� - ��

	TASK_GIVEUP_TYPE_NONE = 0,		// �������� - �߼���
	TASK_GIVEUP_TYPE_TRUE = 1,		// �������� - �߼���

	TASK_REMOVE_REASON_OTHERS = 0,	// �����Ƴ�ԭ�� - ����ԭ��
	TASK_REMOVE_REASON_FINISH = 1,	// �����Ƴ�ԭ�� - �������

	TASK_STATE_HAVE_NO_CONDUCTED = 0,	// ����״̬ - δ����
	TASK_STATE_ALREADY_COMPLETED = 1,	// ����״̬ - �����
	TASK_STATE_STILLS_IN_PROCESS = 2,	// ����״̬ - ������

	TASK_LIST_TYPE_ACCEPTED = 0,		// �����б� - �ѽ���
	TASK_LIST_TYPE_ACCEPTABLE = 1,		// �����б� - �ɽ���
};

enum TASK_CHECK_OPERA_TYPE
{
	TASK_CHECK_OPERA_TYPE_CLEAR = 0,		//��ո��������� param ��������
	TASK_CHECK_OPERA_TYPE_HAS_TASK = 1,		//���Ҹ��������� param ��������

	TASK_CHECK_OPERA_TYPE_MAX,
};

struct ItemContainer	// ������ - ����״̬ ����
{
	ItemContainer(unsigned short p_item_id = 0, unsigned short p_com_num = 0)
	{
		item_id = p_item_id;
		com_num = p_com_num;
	}

	unsigned short item_id;		// ushort ����ID			����ID
	unsigned short com_num;		// ushort �ύ������			����״̬

	bool operator < (const ItemContainer & cobj) const
	{
		if (this->item_id < cobj.item_id) return true;
		else if (this->item_id == cobj.item_id && this->com_num < cobj.com_num) return true;
		
		return false;
	}
};

struct TaskInfoPro		// ������Ϣ - Э���ʽ
{
	TaskInfoPro() { this->Reset(); }

	void Reset()
	{
		task_id = 0;
		state_type = 0;
		prograss = 0;
		expire_time = 0;
	}

	unsigned short task_id;			// ����ID
	char state_type;				// ״̬���� \see TASK_STATE_TYPE
	unsigned char prograss;			// ����

	unsigned int expire_time;		// ������ʱ��
};

struct TaskInfo		// ������Ϣ - ����ʽ
{
	TaskInfo(unsigned short new_task_id = 0, unsigned char cur_versions = 0, unsigned int p_expire_time = 0) { this->Reset(new_task_id, cur_versions, p_expire_time); }

	void Reset(unsigned short new_task_id = 0, unsigned char cur_versions = 0, unsigned int p_expire_time = 0)
	{
		task_id = new_task_id;
		prog_num = 0;
		versions = cur_versions;
		expire_time = p_expire_time;
	}

	unsigned short task_id;			// ����ID
	unsigned char prog_num;			// ������ֵ(����ɱ�ֵ���������ȡ�ĵ���������)
	unsigned char versions;			// ����汾

	unsigned int expire_time;		// �����ڵ�ʱ��� timestamp
};

enum PARTNER_SOAR_TASK_STATUS
{
	PARTNER_SOAR_TASK_STATUS_BEGIN = 0,
	PARTNER_SOAR_TASK_STATUS_ACCEPT = 1,	//�����������ȡ��
	PARTNER_SOAR_TASK_STATUS_FINISH = 2,	//�������������

	PARTNER_SOAR_TASK_STATUS_MAX
};

static const int PARTNER_UPGRADE_TASK_PRESTIGE_VILLAGE_ID = 2;	//���
static const int PARTNER_UPGRADE_TASK_ITEM_ID = 26722;			//¹Ң��Ƭ

struct TaskParam
{
	TaskParam() { this->Reset(); }

	void Reset()
	{
		task_count = 0;
		
		for (int t_cur = 0; t_cur < MAX_ACCEPTED_TASK_NUM; ++t_cur)
		{
			task_info_list[t_cur].Reset();
		}

		today_adv_tri_times = 0;
		adv_task_id = 0;
		adv_task_timestamp = 0;

		is_cur_accept_partner_soar_task = 0;
		partner_soar_task_status = PARTNER_SOAR_TASK_STATUS_BEGIN;
		cur_partner_soar_task_id = 0;

		is_partner_upgrade_task_buy_flag = 0;
		is_old_user_partner_upgrade_task_buy_flag = 0;
		reserve_sh = 0;
	}

	const TaskInfo * GetTaskByID(unsigned short task_id)
	{
		for (int t_cur = 0; t_cur < task_count; ++t_cur)
		{
			if (task_info_list[t_cur].task_id == task_id)
			{
				return & task_info_list[t_cur];
			}
		}

		return NULL;
	}

	bool InsertTask(const TaskInfo & p_task_info)
	{
		if (task_count >= MAX_ACCEPTED_TASK_NUM) return false;
		for (int t_cur = 0; t_cur < task_count; ++t_cur)
		{
			if (p_task_info.task_id == task_info_list[t_cur].task_id) return false;
		}

		task_info_list[task_count++] = p_task_info;
		return true;
	}

	void RemoveTask(const unsigned short task_id)		// �����һ�����񸲸��������
	{
		if (task_count <= 0 || task_count > MAX_ACCEPTED_TASK_NUM) return;
		for (int t_cur = 0; t_cur < task_count; ++t_cur)
		{
			if (task_info_list[t_cur].task_id != task_id) continue;
			
			task_info_list[t_cur] = task_info_list[task_count - 1];
			--task_count;
			return;
		}
	}

	void RemoveTaskByIdx(int index)
	{
		if (index < 0 || index >= task_count || index >= MAX_ACCEPTED_TASK_NUM || task_count <= 0 || task_count > MAX_ACCEPTED_TASK_NUM) return;

		task_info_list[index] = task_info_list[task_count - 1];
		--task_count;
	}

	int task_count;
	TaskInfo task_info_list[MAX_ACCEPTED_TASK_NUM];

	unsigned short today_adv_tri_times;		// �����������񴥷��Ĵ���
	unsigned short adv_task_id;				// ��ǰ��ǵ���������ID
	unsigned int adv_task_timestamp;		// ��������ʱʱ���

	char is_cur_accept_partner_soar_task;		// �Ƿ��ǵ����ȡ�Ļ��������� 1:���� 0:��ʼ״̬ 2:���ǵ���
	char partner_soar_task_status;				// ����������״̬
	unsigned short cur_partner_soar_task_id;	// ��ǰ��ȡ�ķ�������ID

	bool is_partner_upgrade_task_buy_flag;				//�����������ʶ(�����ָ����������̵�,ָ������ID)
	char is_old_user_partner_upgrade_task_buy_flag;		//����Ҹ�����ǰ�����жϽ�����ʶ
	short reserve_sh;
};
typedef char TaskParamHex[sizeof(TaskParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TaskParamHex) < 6400);

struct TaskTraceParam		// ����׷������
{
	TaskTraceParam() { this->Reset(); }

	void Reset()
	{
		list_len = 0;
		memset(trace_list, 0, sizeof(trace_list));
	}

	int list_len;
	unsigned int trace_list[TASK_TRACE_MAX_NUM];
};
typedef char TaskTraceParamHex[sizeof(TaskTraceParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TaskTraceParamHex) < 3072);

struct TaskChosedInfo
{
	TaskChosedInfo() {}
	unsigned short index;
	unsigned short param;
};

struct TaskChosedParam		// ����ѡ���¼
{
	TaskChosedParam() { this->Reset(); }

	void Reset()
	{
		chosed_count = 0;
		memset(chosed_list, 0, sizeof(chosed_list));
	}

	int chosed_count;
	TaskChosedInfo chosed_list[MAX_TASK_CHOSED_RECORD_NUM];
};
typedef char TaskChosedParamHex[sizeof(TaskChosedParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TaskChosedParamHex) < 4500);


enum NEW_TASK_ACTION
{
	NEW_TASK_ACTION_TALK_TO_NPC = 0,	// ��NPC�Ի� param1 npc_seq
	NEW_TASK_ACTION_COMMIT_ITEM = 1,	// �ύ��Ʒ param1:item_id  param2:num  param3:task_id
	NEW_TASK_ACTION_ANECDOTE_SUCC = 2,	// ������ս�ɹ� param1 anecdote_type
};

static const int MAX_SPECIAL_ITEM_NUM = 20;			//���⸱�����߱������洢����20
static const int MIN_SUSPICION = 50;				//���ָ�����ɶ�
static const int MAX_NPC_NUM_WXL = 9;				//NPC����

struct TaskWenXiangLouFbParam
{
	TaskWenXiangLouFbParam() { this->Reset(); }

	void Reset()
	{
		role_enter_posi = 0;
		has_find_identity = 0;
		has_finish_fb = 0;
		lyjz_gahter_flag = 0;

		wenxianglou_has_enter = 0;
		reserver_ch = 0;
		yinyuanyixianqian_flag = 0;
		load_status = 0;

		memset(npc_sucspicion_list, 0, sizeof(npc_sucspicion_list));
		memset(npc_talk_list, 0, sizeof(npc_talk_list));
		memset(npc_identity_list, 0, sizeof(npc_identity_list));
		memset(npc_check_list, 0, sizeof(npc_check_list));

		reward_count = 0;
		memset(reward_list, 0, sizeof(reward_list));
	}

	char role_enter_posi;
	char has_find_identity;//�Ƿ�ָ����ȷ
	char has_finish_fb;		//�Ƿ���ɸ���

	char lyjz_gahter_flag;//��ҩ�����ɼ����

	char wenxianglou_has_enter;
	char reserver_ch;
	char yinyuanyixianqian_flag;
	//---------------------------------����¥-------------------------------------
	char load_status;				// ���н׶�

	char npc_sucspicion_list[MAX_NPC_NUM_WXL];		// npc����ֵ
	char npc_talk_list[MAX_NPC_NUM_WXL];			// npc�Ի���¼
	char npc_identity_list[MAX_NPC_NUM_WXL];		// npcָ�ϼ�¼
	char npc_check_list[MAX_NPC_NUM_WXL];			// npc��֤��¼����פ��

	//��ʱ���߱���                                                                                                                                                                       
	int reward_count;
	int reward_list[MAX_SPECIAL_ITEM_NUM];//���⸱�����߰� -- ��������20��
	 //---------------------------------����¥-------------------------------------
};
typedef char TaskWenXiangLouFbParamHex[sizeof(TaskWenXiangLouFbParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TaskWenXiangLouFbParamHex) < 450);

#pragma pack(pop)
#endif