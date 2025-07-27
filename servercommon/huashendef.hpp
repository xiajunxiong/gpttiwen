#ifndef __HUASHEN_DEF_HPP__
#define __HUASHEN_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/fabaodef.hpp"
#include "servercommon/attributedef.hpp"
#include "servercommon/struct/skilllistparam.hpp"
#include "servercommon/traindef.hpp"
#include "servercommon/commondata.hpp"
//#include "other/attributelist.hpp"

#pragma pack(push, 4)

static const int MAX_HUASHEN_ATTR_PLAN_NUM = 3;
static const int MAX_HUASHEN_DELETE_ITEM_NUM = 1;		//ͬ��ʱ�����Ƴ��������ڵ��ڸ�ֵ,���Զ�������

struct HuaShenParam
{
	HuaShenParam() { this->Reset(); }

	void Reset()
	{
		season_reset = 0;
		plan_reset_flag = 0;
		max_level = 0;
		memset(reserve_char, 0, sizeof(reserve_char));
		is_finish_ding_ji = 0;
		is_rule_change_flag = 0;
		is_reset_grid_flag = 0;
		is_finish_guide = 0;
		reserve_long_long = 0;
		pet_train.Reset();
		for (int i = 0; i < TARIN_TYPE_MAX; i++)
		{
			train_list[i].Reset();
		}
		season_reset_first_dijing = 0;
		use_new_pet_status_array = 0;
		memset(pet_status_index_OBSOLETE, -1, sizeof(pet_status_index_OBSOLETE));
		memset(new_pet_status_index, -1, sizeof(new_pet_status_index));
		capability = 0;
		max_capability = 0;
		battle_position = 0;
		cur_plan_index = 0;
		for (int i = 0; i < MAX_HUASHEN_ATTR_PLAN_NUM; i++)
		{
			attr_plan_list[i].Reset();
		}
		rank_score = 0;
		//memset(m_fabao_equipment_bar, FABAO_EQUIP_BAR_NO, sizeof(m_fabao_equipment_bar));
	}
	//��������
	void SeasonReset(bool is_reset_dingji_flag)
	{
		season_reset = 0;
		max_level = 0;
		memset(reserve_char, 0, sizeof(reserve_char));
		is_finish_ding_ji = 0;
		is_finish_guide = 0;
		reserve_long_long = 0;
		pet_train.Reset();
		for (int i = 0; i < TARIN_TYPE_MAX; i++)
		{
			train_list[i].Reset();
		}
		season_reset_first_dijing = is_reset_dingji_flag ? 0 : season_reset_first_dijing;
		//use_new_pet_status_array = 0; �������þͲ���������������
		memset(pet_status_index_OBSOLETE, -1, sizeof(pet_status_index_OBSOLETE));
		memset(new_pet_status_index, -1, sizeof(new_pet_status_index));
		capability = 0;
		max_capability = 0;
		battle_position = 0;
		cur_plan_index = 0;
		for (int i = 0; i < MAX_HUASHEN_ATTR_PLAN_NUM; i++)
		{
			attr_plan_list[i].Reset();
		}
		rank_score = 0;
	}

	char season_reset;											// 0:��������δ����  1��������������(һ������ֻ������һ��)
	char plan_reset_flag;										// �Ϻ��������Ե�  0:δ���� 1������
	short max_level;											// ��������ѡ���ս��
	char reserve_char[76];										// ������,�Ա������ݲ�����
	char is_finish_ding_ji;										// �Ƿ�����˶�����	0:δ���
	char is_rule_change_flag;									// ����仯��������  5:������
	char is_reset_grid_flag;									// ����ظ�����  1:������
	char is_finish_guide;										// �Ƿ���ɻ���ָ�� 0 : δ��� 1:�����
	long long reserve_long_long;								// ������

	TrainData pet_train;										//������������
	TrainData train_list[TARIN_TYPE_MAX];						//������������

	char season_reset_first_dijing;								// 0:�������û�δ���� 1:���������ѽ���(��������ʾ��
	char use_new_pet_status_array;								//ת�Ƴ���״̬������
	short pet_status_index_OBSOLETE[PET_STATUS_MAX_OBSOLETE];	//����״̬(�ѷ�����

	int capability;												//����ǰս��
	int max_capability;											//������ʷ���ս��	(�˴�ֻ���ṩһ������ս������λ��)

	int battle_position;										//����ս��վλ��0������   1��ǰ�ţ�

	int cur_plan_index;											// ��ǰ������� ��Χ[0,3)
	AttributePlan attr_plan_list[MAX_HUASHEN_ATTR_PLAN_NUM];	// ����Ǳ�ܷ��� �̶�����
	int rank_score;												// ��λ��
	
	short new_pet_status_index[NEW_PET_STATUS_MAX];
};
UNSTD_STATIC_CHECK(8 == NEW_PET_STATUS_MAX);
UNSTD_STATIC_CHECK(5 == PET_STATUS_MAX_OBSOLETE);

typedef char HuaShenParamHex[sizeof(HuaShenParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(HuaShenParamHex) < 2048);

#pragma pack(pop)

#endif