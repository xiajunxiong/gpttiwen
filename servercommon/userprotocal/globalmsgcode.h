#ifndef __GLOBALMSGCODE_H__
#define __GLOBALMSGCODE_H__

namespace Protocol
{
	enum
	{
		GLOBAL_SERVER_SC_BEGIN = 9000,
		GLOBAL_SERVER_CS_BEGIN = 9050,

		TEAM_SC_BEGIN = 9100,
		TEAM_CS_BEGIN = 9150,

		GLOBAL_ROLE_INFO_SC_BEGIN = 9300,
		GLOBAL_ROLE_INFO_CS_BEGIN = 9350,

		GLOBAL_MAIL_SC_BEGIN = 9500,
		GLOBAL_MAIL_CS_BEGIN = 9550,

		GLOBAL_PUBLICSALE_SC_BEGIN = 9600,
		GLOBAL_PUBLICSALE_CS_BEGIN = 9650,

		GLOBAL_GUILD_SC_BEGIN = 9800,
		GLOBAL_GUILD_CS_BEGIN = 9850,

		GLOBAL_GUILD_UNION_SC_BEGIN = 9900,
		GLOBAL_GUILD_UNION_CS_BEGIN = 9950,

		GLOBAL_RANK_SC_BEGIN = 10000,
		GLOBAL_RANK_CS_BEGIN = 10050,

		GLOBAL_OTHER_SC_BEGIN = 10100,
		GLOBAL_OTHER_CS_BEGIN = 10150,

		GLOBAL_CAMP_SC_BEGIN = 10200,
		GLOBAL_CAMP_CS_BEGIN = 10250,

		GLOBAL_ACTIVITY_SC_BEGIN = 10300,
		GLOBAL_ACTIVITY_CS_BEGIN = 10350,
	};

	enum _MsgTypeGlobal
	{
		//////////////////////////////////////////////// ȫ�ַ�������� /////////////////////////////////////////////////////////
		MT_ENTER_GLOBAL_SERVER_ACK_SC = 9000,									// �����½ȫ�ַ������ظ�
		MT_TIME_ACK_SC = 9001,													// ������ʱ�䷵��
		MT_WALLOW_NOTICE_SC = 9002,												// �û������Է���
		MT_DISCONNECT_NOTICE_SC = 9003,											// ֪ͨ�ͻ��˶Ͽ�������
		MT_TIME_DATE_SC = 9004,													// �����������·�
		MT_CONNECT_CROSS_INFO_SC = 9005,										// ���������Ϣ�·�

		MT_ENTER_GLOBAL_SERVER_REQ_CS = 9050,									// �����½ȫ�ַ�����
		MT_TIME_REQ_CS = 9051,													// ���������ʱ��

		//////////////////////////////////////////////// ������ /////////////////////////////////////////////////////////
		MT_TEAM_INFO_SC = 9100,													// ������Ϣ
		MT_OUT_OF_TEAM_SC = 9101,												// �����Ѿ���ɢ
		MT_INVITE_USER_TRANSMIT_SC = 9102,										// ֪ͨ������
		MT_REQ_JOIN_TEAM_SC = 9103,												// ֪ͨ�ӳ���������������
		MT_TEAM_LIST_ACK_SC = 9104,												// ��������б�ظ�
		MT_JOIN_TEAM_SC = 9105,													// ֪ͨ��Ҽ����˶���
		MT_TEAM_LEADER_INFO = 9106,												// �ӳ���Ϣ������������׷��ӳ�
		MT_TEAM_LEADER_CALLTOGETHER = 9107,										// �ӳ������ټ�
		MT_TEAM_ENTER_ROUTE_SC = 9108,											// ���븱������ת��
		MT_TEAM_ENTER_INFO_SC = 9109,											// ��Աȷ����Ϣ
		MT_TEAM_POS_INFO_SC = 9110,												// ���վλ��Ϣ
		MT_TEAM_LEADER_CHANGE_NOTICE_AREA_SC = 9111,							// �ӳ��ı��֪ͨ�������
		MT_CHIVALROUS_TEAM_LIST_SC = 9112,										// ����ֵ�����б�

		MT_CREATE_TEAM_CS = 9150,												// ��������
		MT_INVITE_USER_CS = 9151,												// ����������
		MT_INVITE_JOIN_TEAM_RET_CS = 9152,										// ���������
		MT_REQ_JOIN_TEAM_RET_CS = 9153,										// �ӳ��ظ����������߼������
		MT_REQ_JOIN_TEAM_CS = 9154,											// ����������ĳ����
		MT_DISMISS_TEAM_CS = 9155,												// �ӳ���ɢ����
		MT_KICK_OUT_OF_TEAM_CS = 9156,											// �ӳ�����Ա�߳�����
		MT_CHANGE_TEAM_LEADER_CS = 9157,										// ���ӳ�
		MT_EXIT_TEAM_CS = 9158,												// ����˳�����
		MT_TEAM_LIST_REQ_CS = 9159,											// �������ڳ����Ķ����б�
		MT_CHANGE_MUST_CHECK_CS = 9160,										// �ı��������Ƿ���Ҫ�ӳ�ͨ��
		MT_CHANGE_ASSIGN_MODE_CS = 9161,										// �ı�������ģʽ
		MT_CHANGE_MEMBER_CAN_INVITE_CS = 9162,									// �ı������ͨ��Ա�Ƿ������
		MT_CHANGE_TEAM_LIMIT_CS = 9163,										// �޸Ķ����������
		MT_AUTO_HAVE_TEAM_CS = 9164,											// �Զ����
		MT_UNFULL_TEAM_LIST_BY_TYPE_REQ_CS = 9165,								// ���ݶ������ͻ�ȡδ���Ķ����б�
		MT_SET_TEAM_FLAG_CS = 9166,											// ���������Զ���ʼ���
		MT_TEAM_OPERATE_CS = 9167,												// �������
		MT_TEAM_MATCH_INFO_SC = 9169,											// �Ƿ���ƥ����
		MT_TEAM_MATCH_REQ_CS = 9170,											// ƥ��
		MT_TEAM_TEMPORARILY_PART_CS = 9171,										// ����
		MT_TEAM_INVITATION_REQ_CS = 9172,										// �������
		MT_TEAM_INVITATION_NOTIC_SC = 9173,										// ֪ͨ�յ����������
		MT_TEAM_INVITATION_RET_CS = 9174,										// �������ظ�
		MT_TEAM_RECRUIT_CS = 9175,												// ��ļ
		MT_TEAM_ENTER_FB_RET_CS = 9176,											// �ظ����븱������
		MT_TEAM_SWITCH_TO_CROSS = 9177,											// �л������״̬
		MT_TEAM_CHANNEL_INFO_SC = 9178,											// ���Ƶ����Ϣ
		MT_TEAM_SWITCH_POSITION_CS = 9179,										// �����л�վλ����
		MT_TEAM_MEMBER_APPEARANCE_CHANGE_SC = 9180,								// ��Ա��۸ı�֪ͨ														
		MT_TEAM_REMOVE_APPLY_SC = 9181,											// ȥ������֪ͨ
		MT_TEAM_APPLY_LIST_SC = 9182,											// �����б��·�					
		MT_TEAM_VOTE_START_SC = 9183,											// ��ʾ��ʼͶƱ
		MT_TEAM_LAUNCH_LEADER_VOTE_CS = 9184,									// �����Ϊ�ӳ�
		MT_TEAM_ROLE_VOTE_CS = 9185,											// ͶƱ
		MT_TEAM_SYNC_MEMBER_HPMP_SC = 9186,										// ͬ����Ա��HP��MP
		MT_TEAM_TEAM_LEADER_NPC_REQ_CS = 9187,									// �ӳ�����NPC����
		MT_TEAM_TEAM_LEADER_NPC_INFO_SC = 9188,									// �ӳ�����NPC�·�
		MT_TEAM_SYNC_MEMBER_PET_HPMP_SC = 9189,									// ͬ����Ա�����HP��MP
		MT_TEAM_MEMBER_ADD_HP_MP_NOTICE_SC = 9190,								// ��Ա������Ѫ����֪ͨ
		MT_TEAM_SIMPLE_INFO_CHANGE_NOTICE_SC = 9191,
		MT_TEAM_CHANGE_MEMBER_BASE_INFO_NOTICE_SC = 9192,
		MT_TEAM_LEADER_SUMMON_MEMBER_CS = 9193,
		MT_TEAM_LEADER_SUMMON_MEMBER_NOTICE_SC = 9194,
		MT_TEAM_MEMBER_REPLY_SUMMON_CS = 9195,
		MT_TEAM_MEMBER_INCOMPATIBLE_NOTICE_SC = 9196,
		MT_TEAM_LEADER_BATTLE_PARTNER_LIST_CS = 9197,
		MT_TEAM_LEADER_BATTLE_PARTNER_LIST_SC = 9198,
		MT_TEAM_LEADER_CHECK_MEMBER_ITEM_CS = 9199,
		MT_TEAM_LEADER_CHECK_MEMBER_ITEM_RET_SC = 9200,
		MT_CHIVALROUS_TEAM_LIST_REQ_CS = 9201,									// ����ֵ�����б�
		MT_REQ_JOIN_TEAM_INFO_CS = 9202,
		MT_TEAM_IS_RED_NAME_INFO_CS = 9203,										// �Ƿ����������Ϣ
		MT_TEAM_LEADER_EJECT_WINDOWS_CS = 9204,
		MT_TEAM_LEADER_EJECT_WINDOWS_SC = 9205,
		MT_TEAM_ROLE_RECRUIT_INFO_SC = 9206,
		MT_TEAM_ROLE_RECRUIT_ANSWER_CS = 9207,

		//////////////////////////////////////////////// ������� /////////////////////////////////////////////////////////

		//////////////////////////////////////////////// �ʼ���� /////////////////////////////////////////////////////////
		MT_GLOBAL_MAIL_SEND_RESULT_SC = 9500,									// ��ȡ��������
		MT_GLOBAL_MAIL_DELETE_SC = 9501,										// ɾ���ʼ�����
		MT_GLOBAL_MAIL_LOCK_SC = 9502,											// ���ʼ�����
		MT_GLOBAL_MAIL_UNLOCK_SC = 9503,										// �����ʼ�����
		MT_GLOBAL_MAIL_LIST_SC = 9504,											// �ʼ��б���
		MT_GLOBAL_MAIL_DETAIL_SC = 9505,										// �ʼ���ϸ��Ϣ
		MT_GLOBAL_MAIL_ATTACHMENT_SC = 9506,									// ��ȡ�ʼ���������
		MT_GLOBAL_MAIL_NEW_MAIL_NOTICE_SC = 9507,								// ���ʼ�֪ͨ
		MT_GLOBAL_MAIL_HAS_UNREAD_MAIL_SC = 9508,								// δ��ȡ�ʼ�֪ͨ

		MT_MAIL_SEND_CS = 9550,									// �����ʼ�
		MT_MAIL_DELETE_CS = 9551,								// ɾ���ʼ�
		MT_MAIL_GETLIST_CS = 9552,								// ��ȡ�ʼ��б�
		MT_MAIL_READ_CS = 9553,									// ��ȡ�ʼ�
		MT_MAIL_ATTACHMENT_CS = 9554,							// ��ȡ����
		MT_MAIL_CLEAN_CS = 9555,								// ����ʼ�
		MT_MAIL_ONE_KEY_FETCH_ATTACHMENT = 9556,				//һ����ȡ�ʼ�

		//////////////////////////////////////////////// �����г� /////////////////////////////////////////////////////////
		MT_PUBLICSALE_SEARCH_CS = 9650,											// ����

		MT_PUBLICSALE_ADD_ACK_SC = 9600,										// ������Ʒ�ϼܷ���
		MT_PUBLICSALE_DEL_ACK_SC = 9601,										// ������Ʒ�¼ܷ���
		MT_PUBLICSALE_BUY_ACK_SC = 9602,										// ����������Ʒ����
		MT_PUBLICSALE_GET_ITEM_LIST_SC = 9603,									// ��ȡ�Լ�������Ʒ�б�
		MT_PUBLICSALE_SEARCH_SC = 9604,											// �������� 
		MT_PUBLICSALE_BUY_FAILED_REFRESH_SC = 9605,								// ����ʧ�ܺ�ˢ��������Ʒ��Ϣ

		MT_PUBLICSALE_SEND_ALL_RECORD_SC = 9606,								// ����ȫ�����۽��׼�¼
		MT_PUBLICSALE_SEND_RECORD_CHANGE_SC = 9607,								// ���ͼ��۽��׼�¼�仯

		MT_PUBLICSALE_SEND_NUM_OF_SEARCH_TYPE_SC = 9608,							// ���͸��� search type ��ȡ������Ʒ����

		//////////////////////////////////////////////// ������� /////////////////////////////////////////////////////////
		MT_CREATE_GUILD_CS = 9800,												// ��������
		MT_QUIT_GUILD_REQ_CS = 9801,											// �˳�����
		MT_QUIT_GUILD_SC = 9802,												// ֪ͨ����Ѿ�û�м���
		MT_GUILD_BASE_INFO_SC = 9803,											// ���������Ϣ
		MT_GUILD_MEMBER_INFO_SC = 9804,											// �����Ա��Ϣ
		MT_GUILD_APPLY_JOIN_REQ_CS = 9805,										// �������
		MT_GUILD_APPLY_JOIN_ACK_CS = 9806,										// ��������
		MT_GUILD_EXPEL_OUT_REQ_CS = 9807,										// �߳�����
		MT_GUILD_SET_NOTICE_REQ_CS = 9808,										// ���Ĺ���
		MT_GET_GUILD_LIST_CS = 9809,											// ��������б�
		MT_SEND_GUILD_LIST_SC = 9810,											// �·������б�
		MT_SEND_GUILD_NOTICE_SC = 9811,											// �·����幫��
		MT_GUILD_PET_TRAINING_UP_CS = 9812,										// ǿ������ѵ��
		MT_GUILD_PET_TRAINING_CS = 9813,										// ����ѵ��
		MT_GET_GUILD_TRAINING_INFO_CS = 9814,									// ����ѵ����Ϣ
		MT_GUILD_SEND_TRAINING_INFO_SC = 9815,									// ����ѵ����Ϣ
		MT_SEND_GUILD_APPLY_LIST_SC = 9816,										// �·������б�
		MT_GUILD_CREATE_RET_SC = 9819,											// �������巵��
		MT_GUILD_ABDICATION_REQ_CS = 9820,										// ת���峤
		MT_GUILD_IMPEACHMENT_REQ_CS = 9821,										// ����
		MT_GUILD_APPOINTMENT_REQ_CS = 9822,										// ����
		MT_GUILD_SHOP_BUY_CS = 9823,											// �̵깺��
		MT_GUILD_SHOP_INFO_SC = 9824,											// �̵���Ϣ
		MT_GUILD_PET_TRAINING_EXP_SC = 9825,									// ����ѵ�����㾭����ʾ
		MT_GUILD_RENAME_CS = 9826,												// �������

		MT_GUILD_INVITATION_REQ_CS = 9829,										// ��������
		MT_GUILD_INVITATION_NOTIC_SC = 9830,									// ֪ͨ�յ��˼�������
		MT_GUILD_BOSS_INFO_SC = 9831,											// ��������Boss��Ϣ����
		MT_GUILD_BOSS_REQ_CS = 9832,											// ��������Boss��Ϣ����
		MT_GUILD_LEVEL_UP_CS = 9833,											// ������������
		MT_PLAYER_APPLY_LIST_SC = 9834,											// ����������Ϣ
		MT_GUILD_INVITATION_REPLY_CS = 9835,									// ��������ظ�
		MT_GUILD_BOSS_RANK_INFO_SC = 9836,										// �����������а�
		MT_GUILD_NEED_APPROVE_INFO_SC = 9837,									// �����Ƿ���Ҫ��׼������Ϣ
		MT_GUILD_SET_NEED_APPROVE_INFO_CS = 9838,								// ���ù����Ƿ���Ҫ��׼������Ϣ
		MT_GUILD_EVENT_INFO_CS = 9839,											// �����¼��·�
		MT_GUILD_RESET_BANNER_CS = 9840,										// �����޸����
		MT_GUILD_BOSS_CHAPTER_GUILD_RANK_SC = 9841,								// ��������֮�����������������
		MT_GUILD_GATHER_DREAM_INFO_LIST_SC = 9842,								// ���弯Ը��Ϣ�б��·�
		MT_GUILD_GATHER_DREAM_SIGNLE_INFO_SC = 9843,							// ���弯Ը������Ϣ�·�
		MT_GUILD_QUIT_INFO_SC = 9844,											// �˳�������Ϣ
		MT_GUILD_RECOMMEND_INFO_SC = 9845,										// �Ƽ�����
		MT_GUILD_HALLOFFAME_REQ_CS = 9846,										// �����ò���
		MT_GUILD_HALLOFFAME_INFO_SC = 9847,										// ���������������·�
		MT_GUILD_QIFU_INFO_SC = 9848,											// ����Ϣ�·�

		MT_GUILD_CONSTRUCT_BUILDING_INFO_SC = 9849,								// �����������赥����Ϣ�·�
		MT_GUILD_CONSTRUCT_BUILDING_SEQ_CS = 9850,								// ����������������
		MT_GUILD_CONSTRUCT_BUILDING_FINISH_INFO_SC = 9851,						// ���彨������,����ͻ����жϺ��
		MT_GUILD_CONSTRUCT_USE_BUILD_MATERIAL_INFO_SC = 9852,					// ���彨��ͨ��Ư�ֵ���
		MT_GUILD_CONSTRUCT_BUILDING_ALL_INFO_SC = 9853,							// ������������ȫ����Ϣ�·�
		MT_GUILD_CONSTRUCT_SPEED_UP_INFO_SC = 9854,								// ���彨�������Ϣ�·�
		MT_GUILD_CONSTRUCT_BUILDING_FINISH_TIMESTAMP = 9855,					// ���彨�����ʱ���
		MT_GUILD_CONSTRUCT_OPEN_SUP_INFO_SC = 9856,								// ������ٿ�����Ϣ
		MT_GUILD_MEMBER_INFO_CHANGE_SC = 9857,								// 

		//////////////////////////////////////////////// ���а���� /////////////////////////////////////////////////////////
		MT_RANK_GET_PERSONRANK_LIST_ACK_SC = 10000,
		MT_RANK_GET_GUILDRANK_LIST_ACK_SC = 10001,
		MT_RANK_GET_TEAMRANK_LIST_ACK_SC = 10002,
		MT_RANK_GET_PERSONRANK_TOP_USER_SC = 10003,
		MT_RANK_GET_WORLD_LEVEL_ACK_SC = 10004,
		MT_RANK_GET_MINGRENRANK_INFO_SC = 10005,			//���˰���Ϣ����
		MT_RANK_GET_FENGYUNRANK_INFO_SC = 10006,			//���ư���Ϣ����
		MT_RANK_GET_MINGRENRANK_OPEN_INFO_SC = 10007,		//���ư���Ϣ����
		MT_RANK_GET_PETRANK_LIST_ACK_SC = 10008,			// �����
		MT_RANK_GET_PET_RANK_DETAILED_INFO_SC = 10009,		// ������ϸ��Ϣ�·�
		
		
		MT_RANK_GET_PERSONRANK_LIST_CS = 10050,
		MT_RANK_GET_GUILDRANK_LIST_CS = 10051,
		MT_RANK_GET_TEAMRANK_LIST_CS = 10052,
		MT_RANK_GET_PERSONRANK_TOP_USER_CS = 10053,
		MT_RANK_GET_MINGRENRANK_LIST_CS = 10054,			//�������˰���Ϣ
		MT_RANK_GET_FENGYUNRANK_LIST_CS = 10055,			//������ư���Ϣ
		MT_RANK_GET_PETRANK_LIST_CS = 10056,				//��������
		MT_RANK_GET_REFRESH_TIMESTAMP_CS = 10057,			//�������а�ˢ��ʱ��
		MT_RANK_GET_REFRESH_TIMESTAMP_SC = 10058,			//�·����а�ˢ��ʱ��	
		MT_RANK_GET_PETRANK_DETAILED_INFO_CS = 10059,		//����������а�ĳһ��������Ϣ

		MT_RANK_GET_COURAGE_CHALLENGE_REQ_CS = 10060,		//��ħ֮��������Ϣ����
		MT_RANK_GET_COURAGE_CHALLENGE_SC = 10061,			//��ħ֮��������Ϣ�·�

		MT_RANK_GET_CHIVALROUS_REQ_CS = 10062,				//����ֵ���а�����
		MT_RANK_GET_CHIVALROUS_RESP_SC = 10063,				//����ֵ���а񷵻�

		//////////////////////////////////////////////// ���� /////////////////////////////////////////////////////////
		RESERVE_MT_SHOP_LIMIT_ITEM_LIST_SC = 10100,								// ����
		MT_WORLDBOSS_INFO_SC = 10101,											// �·�����boss��Ϣ
		MT_WORLDBOSS_BORN_SC = 10102,											// ����boss����
		MT_JINGYING_BOSS_BORN_NOTICE_SC = 10103,								// ��Ӣboss����֪ͨ

		RESERVE_MT_GET_SHOP_LIMIT_ITEM_LIST_CS = 10150,							// ����
		MT_GET_WORLDBOSS_INFO_CS = 10151,										// ��������boss��Ϣ

		//////////////////////////////////////////////// ��Ӫ /////////////////////////////////////////////////////////
		MT_CAMP_INFO_SC = GLOBAL_CAMP_SC_BEGIN,									// ��Ӫ��Ϣ����

		MT_CAMP_INFO_CS = GLOBAL_CAMP_CS_BEGIN,									// ��ȡ��Ӫ��Ϣ
	};
}

#endif // __GLOBALMSGCODE_H__


