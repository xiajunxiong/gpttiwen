#ifndef __MYSTERIOUS_STRIAL_DEF_HPP__
#define __MYSTERIOUS_STRIAL_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"


//-----------------------------��������(ģ�鹦��)------------------------------------
static const int RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM = 7;								//��������
static const int RA_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM = 5;							//һ���������
static const int RA_NEW_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM = 7;						//��һ���������
static const int RA_MYSTERIOUS_TRIAL_FINALLY_TASK_MAX_NUM = 5;						//������������
static const int RA_MYSTERIOUS_TRIAL_FINALLY_MAX_NUM = 32;							//�����������ʶ
static const int RA_MYSTERIOUS_TRIAL_OPEN_DAY_MAX_NUM = 7;							//����������
static const int RA_MYSTERIOUS_TRAIL_CMD_BUY_START_NUM = 10;						//ֱ����ʼ + day(start_time)	 �����������������
UNSTD_STATIC_CHECK(RA_MYSTERIOUS_TRIAL_OPEN_DAY_MAX_NUM <= RA_MYSTERIOUS_TRAIL_CMD_BUY_START_NUM);		

enum MysteriousStrialCondition
{
	 MSC_CON_LOGIN_DAY    = 0,				// �� �ۼƵ�½����	|   �ۻ�   | ָ������		| ��
	 MSC_CON_ROLE_LEVEL   = 1,				// �� ��ҵ�ǰ�ȼ�	| ֱ���ж� | ָ���ȼ�		| ��
	 MSC_CON_EQPM_FORG    = 2,				// �� ���װ������	|   �ۻ�   | ָ������		| ���ڵ���Ʒ��(0:������Ʒ��) | ���ڵ��ڵȼ�(0�����Ƶȼ�) |��
	 MSC_CON_PROF_PRO     = 3,				// �� ���ְҵ����	| ֱ���ж� | ָ������		| ��
	 MSC_CON_PET_INTEN	  = 4,				// �� ���ǿ������	| ֱ���ж� | ָ������		| ���ڵ���Ʒ��(0:������Ʒ��) | ��
	 MSC_CON_COUR_CHAG    = 5,				// �� ��ħ֮������| ֱ���ж� | ָ������		| ָ���ȼ� |��
	 MSC_CON_KILL_BOSS    = 6,				// �� ��ɱBOSS����| ���μ��� | ָ������		| ��(�޶�Ұ��)
	 MSC_CON_COMP_TREA    = 7,				// �� �μӾ�������	|   �ۻ�   | ָ������		| ��
	 MSC_CON_COMPLETE_FB  = 8,				// �� ������⸱��	|   �ۻ�   | ָ������		| ��
	 MSC_CON_PARTNER_NUM  = 9,				// �� �����������	| ֱ���ж� | ָ������		| ��
	 MSC_CON_PET_CAPABIL  = 10,				// �� ��ҳ���ս��	| ֱ���ж� | ָ��ս��		| ��
	 MSC_CON_GEMS_MOSAIC  = 11,				// �� ��ұ�ʯ��Ƕ| ֱ���ж� | ָ������		| �ﵽƷ�� |��
	 MSC_CON_ONLINE_TIME  = 12,				// �� �������ʱ��|   �ۻ�   | ָ��ʱ��		| ��
	 MSC_CON_PARTNER_PRO  = 13,				// �� ��һ�����| ֱ���ж� | ָ������		| ָ������ |��
	 MSC_CON_JOIN_FAMILY  = 14,				// �� ��Ҽ������	| ֱ���ж� | ָ������		| ��
	 MSC_CON_TEMP_COMP_FB = 15,				// �� �����ɸ���	|   �ۻ�   | ָ������		| ��
	 MSC_CON_PET_USE_BOOK = 16,				// �� �����ü�����	|   �ۻ�   | ָ������		| ���ڵ��ڳ���Ʒ��(0:������Ʒ��) | ���ڵ�����ȼ�(0:������Ʒ��) |��
	 MSC_CON_TRIAL_BOSS   = 17,				// �� ����֮������	|   �ۻ�   | ָ������		| ��
	 MSC_CON_TRA_MAR_PUR  = 18,				// �� �����������	|   �ۻ�   | ָ������		| ��
	 MSC_CON_KILL_YW_BOSS = 19,				// �� ��ɱҰ��BOSS|   �ۻ�   | ָ������	| ��(�޶�Ұ��)
	 MSC_CON_USER_CAPABIL = 20,				// �� �����������	| ֱ���ж� | ָ��ս��		| ��
	 MSC_CON_ACT_COLLECT  = 21,				// �� ��Ҽ���ͼ��	|   �ۻ�   | ָ������		| ��
	 MSC_CON_SHOP_CONSUME = 22,				// �� ����̻�����	|   �ۻ�   | ָ������		| ��
	 MSC_CON_JOIN_XIYOUFB = 23,				// �� ����ϡ�и���	|   �ۻ�   | ָ������		| ��

	 MSC_CON_COURAGE_CHALLENGE = 24,		// �� ��ħ֮������	| ֱ���ж� | ָ������		| ��
	 MSC_CON_COLOSSEUM = 25,				// �� ����������	| ֱ���ж� | ָ������		| ��
	 MSC_CON_CATCH_PET = 26,				// �� ���һֻ����	| ֱ���ж� | ָ��Ʒ��		| ��
	 MSC_CON_ROLE_SKILL_LEVEL = 27,			// �� ��ɫ��������	| ֱ���ж� | ָ���ȼ�		| ��
	 MSC_CON_GET_MEDAL = 28,				// �� ��������	| ֱ���ж� | ָ������		| ָ��Ʒ��		| ��
	 MSC_CON_PARTNER_FETTERS = 29,			// �� ��������	| ֱ���ж� | ָ������		| ��
	 MSC_CON_MI_YI = 30,					// �� ͨ��Զ��ħڣ	| ֱ���ж� | �̶�1		| ָ������ |��
	 MSC_CON_SPECIAL_PET_INTEN = 31,		// �� �������ǿ��	| ֱ���ж� | ָ��Ʒ��		| �ﵽǿ���ȼ�	| ��
	 MSC_CON_PRESTIG_LEVEL = 32,			// �� ��ׯ����	| ֱ���ж� | ָ���ȼ�		| ��
	 MSC_CON_PET_SKILL_NUM = 33,			// �� ���＼������	| ֱ���ж� | ָ������		| ��
	 MSC_CON_JOIN_XIYOUFB_LEVEL = 34,		// �� ϡ�и���ָ���ؿ�	|  ֱ�ӵ���  | ָ������		| ��
	 MSC_CON_GET_FA_BAO = 35,				// �� ��ɫ��÷���| ֱ���ж� | ָ������    | ָ���ȼ�  | ָ��Ʒ��(0:����) | �� 
	 MSC_CON_UNLOCK_SMART_MOUNTS = 36,		// �� �������������| ֱ���ж� | ָ������(����) | 0:���� 1:���� | ��
	 MSC_CON_ACT_COLLECT_QUAILTY = 37,		// �� ����Ʒ��ͼ��| ֱ���ж� |��ָ�������� | ָ��Ʒ��(0:����) | ��
	 MSC_CON_DREAM_NOTES_CHALLENGE = 38,	// �� ͨ����Ԩ��¼| ֱ���ж� |  ָ���ؿ�   | ��
	 MSC_CON_LUN_HUI_WANG_CHUAN = 39,		// �� ͨ���ֻ�����| ֱ���ж� |  ָ���ؿ�   | ��
	 MSC_CON_CLOUD_ARENA = 40,				// �� 񷶥֮����λ| ֱ���ж� |  ָ����λ   | ��
	 MSC_CON_KILL_SHAN_HAI_BOSS = 41,		// �� ��ɱɽ������|  �ۻ�    |  ָ������  |	 ɽ������(�Ѷ� 0:����) |��
	 MSC_CON_BRAVE_GROUD_PASS = 42,			// �� ͨ���´��ع�| ֱ���ж� |  ָ������   | ��
	 MSC_CON_HUAN_JIE_ZHAN_CHANG = 43,		// �� �ý�ս����λ| ֱ���ж� |  ָ����λ   | ��
	 MSC_CON_PARTNER_EQUIPMENT = 44,		// �� ���һ������	| ֱ���ж� |  ָ������   | ��
	 MSC_CON_PARTNER_QUALITY = 45,			// �� ������    | ֱ���ж� |��ָ�������� | �����Ѵ��� |��
	 MSC_CON_NTHER_WORLD_PURGATORY = 46,	// �� ͨ��ĳ����  |  �ۻ�	 |	ָ������	 |��ָ�����ǣ�-1:�����ƣ��� |��
	 MSC_CON_NTHER_WORLD_PURGATORY_ALL = 47,// �� ͨ�ر�������|  �ۻ�	 |	ָ������  |	�Ǿ����ͣ�0:����,1:�Ǿ�,2:�۾��� |��
	 MSC_CON_SPECIAL_PET_ABANDON = 48,		// �� �������	|  �ۻ�	 |  ָ������  | ָ��Ʒ��(0:����)	| ��
	 MSC_CON_PET_REFINED = 49,				// �� ����ϴ������|  �ۻ�	 |  ָ������ | ���ڵ���Ʒ��(0:������Ʒ��) |��
	 MSC_CON_PARTNER_BREAK = 50,			// �� ����������|  �ۻ�	 |  ָ������ | ���ڵ���Ʒ��(0:������Ʒ��) |��
	 MSC_CON_FAST_PASS_DREAM_NOTES = 51,	// �� ɨ����Ԩ��¼|  �ۻ�	 |  ָ������ | ��
	 MSC_CON_EQUIP_FULL_SET_STR = 52,		// �� ȫ��װ��ǿ��| ֱ���ж�|  ָ���ȼ� | ��
	 MSC_CON_PAR_LIMIT_ACTIVITY = 53,		// �� ������ʱ�|  �ۻ�	 |  ָ������ | ��  
	 MSC_CON_PET_INTEN_NUMBER	= 54,		// �� ǿ���������	|  �ۻ�	 |  ָ������ | ���ڵ���Ʒ��(0:������Ʒ��) | ��
	 MSC_CON_PET_MONOPOLY		= 55,		// �� ���޵�����	| ֱ���ж�|  ָ������ | ��
	 NSC_CON_REALM_BREAK_FIGHT	= 56,		// �� ����֮���ؿ�| ֱ���ж�|  ָ���ؿ� | ��
	 MSC_CON_MEDAL_UP_GRADE		= 57,		// �� ��ɫѫ�½���|  �ۻ�	 |  ָ������ | ���ڵ���Ʒ��(0:������Ʒ��) | ��	
	 MSC_CON_MARKET_SHOPPING	= 58,		// �� �г�������	|  �ۻ�	 |  ָ������ | ��
	 MSC_CON_PASS_MI_YI_BOSS	= 59,		// �� ͨ��Զ������| ֱ���ж�|�� �̶�1   | ָ������ | ָ��������� | ��
	 MSC_CON_PAR_CLOUD_ARENA	= 60,		// �� ����񷶥֮��|  �ۻ�	 |  ָ������ | �� 
	 MSC_CON_PET_LIAN_YAO		= 61,		// �� ��������	|  �ۻ�	 |  ָ������ | ���ڵ���Ʒ��(0:�����Ƴ���) | ��

	 MSC_CON_MAX,	
}; 
UNSTD_STATIC_CHECK(MSC_CON_MAX == 62);

enum MYSTERIOUS_TRIAL_TYPE
{
	MYSTERIOUS_TRIAL_TYPE_YUSHEN = 0,			// ��������
	MYSTERIOUS_TRIAL_TYPE_BA_SHE = 1,			// ��������

	MYSTERIOUS_TRIAL_TYPE_MAX
};

#pragma pack(push, 4)

struct ActivityFlagSchedule
{
	void Reset()
	{
		schedule = 0;
		reserve_sh = 0;
		flag = 0;
	}
	unsigned short flag;	// ��־     0:δ���   1:�����δ��ȡ   2:����ȡ	
	short reserve_sh;		//������				
	int schedule;			// ����
};

struct MysteriousTrialParam
{ 
	MysteriousTrialParam(){this->Reset();}
	void Reset()
	{
		open_day = 0;
		task = 0;
		for(int i = 0; i<RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM; i++)
			for(int j =0;j<RA_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM;j++)
				mysterious_trial_info[i][j].Reset();
		activity_flag.Reset();
		type = MYSTERIOUS_TRIAL_TYPE_YUSHEN;
		change_reset_flag = 0;
		transfer = 0;
		for (int i = 0; i < RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM; i++)
			for (int j = 0; j < RA_NEW_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM; j++)
				new_mysterious_trial_info[i][j].Reset();
		active_open_day_id = 0;
	}

	void ActivityExpiredReset()
	{
		open_day = 0;
		task = 0;
		for (int i = 0; i < RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM; i++)
			for (int j = 0;j < RA_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM;j++)
				mysterious_trial_info[i][j].Reset();
		activity_flag.Reset();
		for (int i = 0; i < RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM; i++)
			for (int j = 0; j < RA_NEW_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM; j++)
				new_mysterious_trial_info[i][j].Reset();
		active_open_day_id = 0;
	}

	int open_day;					//�����������
	int task;						//����ɵ��������(��������������)
	ActivityFlagSchedule mysterious_trial_info[RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM][RA_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM];					//����Ļ��Ϣ
	BitMap<RA_MYSTERIOUS_TRIAL_FINALLY_MAX_NUM> activity_flag;																			//�����������Ϣ
	short type;						//��ǰ�׶� MYSTERIOUS_TRIAL_TYPE	
	char change_reset_flag;			//�������ݳ����޸����� 0:δ���� 1:������
	char transfer;					//����ת��
	ActivityFlagSchedule new_mysterious_trial_info[RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM][RA_NEW_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM];					//����Ļ��Ϣ
	unsigned int active_open_day_id;
};
typedef char MysteriousTrialParamHex[sizeof(MysteriousTrialParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MysteriousTrialParamHex) < 2048);

#pragma pack(pop)

#endif
