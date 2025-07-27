#ifndef __BATTLE_MSG_CODE_H__
#define __BATTLE_MSG_CODE_H__

namespace Protocol
{
	// �˴�������������Battle�Ľ���Э��

	enum
	{
		BATTLE_SERVER_MSG_BEGIN = 11000,
		BATTLE_SERVER_MSG_END = 12000,
	};

	enum _MsgTypeBattle
	{
		MT_BATTLE_START_SC = 11001,				// ս����ʼ֪ͨ
		MT_BATTLE_RESULT_SC = 11002,			// ս�����ս��
		MT_BATTLE_INFO_SC = 11003,				// ��ս˫����Ϣ
		MT_BATTLE_ROUND_RESULT_SC = 11004,		// ���غϽ��
		MT_BATTLE_ROUND_PHASE_SC = 11005,		// �׶�
		MT_BATTLE_ROLE_INFO_SC = 11006,			// ��ɫ����
		MT_BATTLE_SET_AUTO_RESP_SC = 11007,		// �Զ����÷���
		MT_BATTLE_FORCE_FINISH_FIGHT_SC = 11008,// ǿ�ƽ���ս������or֪ͨ
		MT_BATTLE_SET_AUTO_MOVE_SC = 11009,		// �Զ��ͷ���Ϊ���ûذ�
		MT_BATTLE_DEMON_CAVE_WAVE_SC = 11010,	// ħ�ߵ�ǰ����
		MT_BATTLE_ROLE_OPERATE_SC = 11011,		// �����ж��ķ��ؽ��
		MT_BATTLE_ROLE_OB_SWITCH_SC = 11012,	// OB����
		MT_BATTLE_SPECIAL_START_SC = 11013,		// ����ս����Ϣ����
		MT_BATTLE_SPECIAL_ONE_MAP_CHANGE_SC = 11014,// ����ս��-�����ͼ�ı�
		MT_BATTLE_SPECIAL_FINISH_SC = 11015,	// ����Ѱ����
		MT_BATTLE_SPECIAL_JOY_REQ_CS = 11016,	// ����Ѱ���������
		MT_BATTLE_CHARACTER_BUFF_LIST_SC = 11017,	// ��ɫbuff�б�
		MT_BATTLE_REPORT_SC = 11018,			// ս��
		MT_BATTLE_CHARACTER_SPECIAL_EFFECT_LIST_SC = 11019,	// ��ɫ��Ч�б�
		MT_BATTLE_CHARACTER_HALO_LIST_SC = 11020,	// ��ɫ�⻷�б�
		MT_BATTLE_HALLOW_LIST_SC = 11021,			// ʥ���б�

		MT_BATTLE_START_CS = 11051,
		MT_BATTLE_DEMO_START_CS = 11052,
		MT_BATTLE_ANIMATION_FINISH_CS = 11053,	// ����������
		MT_BATTLE_ROLE_OPERATE_CS = 11054,		// ��ɫս������
		MT_BATTLE_FORCE_FINISH_FIGHT_CS = 11055,// ǿ�ƽ���ս��(������)
		MT_BATTLE_COUNTDOWN_FINISH_CS = 11056,	// ����ʱ����
		MT_BATTLE_FIGHT_MONSTER_CS = 11057,		// �����ĳ������
		MT_BATTLE_MODE_FIGHT_CS = 11058,		// ��ս��ģʽ��ʼս��
		MT_BATTLE_FIGHT_OTHERS_CS = 11059,		// ǿ�� PK �������(ǿP������)

		MT_BATTLE_QIE_CUO_REQ_CS = 11060,		// ��������������д�
		MT_BATTLE_QIE_CUO_REQ_SC = 11061,		// ����������������д�
		MT_BATTLE_QIE_CUO_REP_CS = 11062,		// �д�����ظ�

		MT_BATTLE_SET_STRATEGY_CS = 11071,		// ս��ָ������
		MT_BATTLE_CLEAR_STRATEGY_CS = 11072,	// ���ս��ָ��
		MT_BATTLE_STRATEGY_INFO_SC = 11073,		// ս��ָ����Ϣ
		MT_SYNC_WORLD_BOSS_CUR_HP_SC = 11074,	// ͬ������boss��ǰѪ��
		MT_BATTLE_ROLE_FINISH_INPUT_NOTICE_SC = 11075, // �����������
		MT_BATTLE_ROLE_START_INPUT_SC = 11076,	// ���ǿ�ʼ����

		MT_BATTLE_LEAVE_OB_CS = 11077,			// �뿪��ս���������ɹ�ս����ͨ�Ĺ�ս��
		MT_BATTLE_END_FOR_AUDIENCE_SC = 11078,	// ����ս������

		MT_BATTLE_PAUSE_CS = 11079,				// ������ͣ
		MT_BATTLE_PAUSE_SC = 11080,				// ������ͣ�������

		MT_BATTLE_PREPARE_FINISH_CS = 11081,	// ս��׼���������
		MT_BATTLE_GIVEUP_CS = 11082,            // ս������

		MT_BATTLE_REPORT_SIMPLE_INFO_CS = 11083,// ����ս��������Ϣ
		MT_BATTLE_REPORT_SIMPLE_INFO_SC = 11084,// ����ս��������Ϣ

		MT_BATTLE_REPORT_DETAIL_INFO_CS = 11085,// ����ս����ϸ��Ϣ���ۿ�¼��
		MT_BATTLE_REPORT_STOP_WATCHING_CS = 11086, // ս����ͣ�ۿ�����
		MT_BATTLE_REPORT_STOP_WATCHING_SC = 11087, // ս����ͣ�ؿ�����
		MT_BATTLE_AUTO_INFO_LIST_SC = 11088,	   // �Զ���Ϣ�б�
		MT_BATTLE_AUTO_INFO_CHANGE_NOTICE_SC = 11089,	// �Զ���Ϣ�ı�֪ͨ
		MT_BATTLE_START_NPC_DEMO_FIGHT_CS = 11090,	// ��ʼ������������ս��
		MT_BATTLE_ROLE_IN_BATTLE_NOTICE_SC = 11091,	// ֪ͨ�����ս���У����Ҫ���������ս����
		MT_BATTLE_ROLE_REJOIN_BATTLE_SCENE_CS = 11092, // ���½���ս����������
		MT_BATTLE_STATISTIC_RESULT_SC = 11093,  // ս��ͳ�������·�
		MT_BATTLE_CHIEF_ELECT_FIELD_BUFF_SHOW = 11094, // ��ϯ��ѡս��BUFF��ʾ
		MT_BATTLE_AI_SKILL_RESULT = 11095,		// ����ʩ�����
		MT_BATTLE_SET_CUSTOM_INSTRUCTION_CS = 11096, // ս���������Զ���ָ��
		MT_BATTLE_REPORT_PARSE_FAILED = 11097,	// ս������ʧ���ϱ�
		MT_BATTLE_SKIP_CS = 11098,				// ����ս��
		MT_BATTLE_FAST_FORWARD_CS = 11099,		// ��ս
		MT_BATTLE_FAST_FORWARD_SC = 11100,		// ��ս����·�
		MT_BATTLE_GUILD_BOSS_DAMAGE_SC = 11101,	// ս���·�����BOSS���˺�
		MT_BATTLE_SURRENDER_CS = 11102,         // Ͷ��

		MT_BATTLE_WATCH_BATTLE_CS = 11200,		// �����ս
		MT_BATTLE_CHAT_CS = 11201,				// ����ս�����죨��Ļ��
		MT_BATTLE_CHAT_SC = 11202,				// ս�����췵��(��Ļ��

		MT_BATTLE_UNDO_CS = 11203,				// ����ս��ָ��
		MT_BATTLE_ANIMATION_SPEED_MODIFY_CS = 11204, // �ۿ��ٶ��޸�����
		MT_BATTLE_ANIMATION_SPEED_MODIFY_SC = 11205, // �ۿ��ٶ��޸ķ���
		
		MT_BATTLE_MOVE_BOARDCAST_SC = 11206,	// ս��ָ��㲥
	};

	static const int BATTLE_MSG_NUM = BATTLE_SERVER_MSG_END - BATTLE_SERVER_MSG_BEGIN;
}

#endif