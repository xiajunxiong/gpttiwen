#ifndef __LUN_HUI_WANG_CHUAN_DEF_H__
#define __LUN_HUI_WANG_CHUAN_DEF_H__

#include "servercommon/bitmapfunction.hpp"

const static int MAX_LUN_HUI_WANG_CHUAN_INDEX = 32;		//���ؿ����

#pragma pack(push, 4)
struct LunHuiWangChuanParam
{
	/*************** **********************/
	LunHuiWangChuanParam() { this->Reset(); }

	void Reset()
	{
		battle_tamp = 0;
		fight_seq = 0;
		fight_level = 0;
		fight_refresh_tamp = 0;

		set_seq = 0;
		start_level = 0;
		battle_monster_id = 0;
		previous_refresh_tamp = 0;
		pass_list.Reset();
		reward_list.Reset();
		memset(reserve_ll, 0, sizeof(reserve_ll));
		pass_times = 0;
	}

	void Refresh()
	{
		set_seq = 0;
		start_level = 0;
		battle_monster_id = 0;
		previous_refresh_tamp = 0;
		pass_list.Reset();
		reward_list.Reset();
	}

	void OnFightFinish()
	{
		battle_tamp = 0;
		fight_seq = 0;
		fight_level = 0;
		//battle_monster_id = 0;
		fight_refresh_tamp = 0;
	}

	unsigned int battle_tamp;	//��¼ս����ʼʱ���
	int fight_seq;				//����ս��seq
	int fight_level;
	unsigned int fight_refresh_tamp;//ս��ʱ��ˢ��ʱ���

	int start_level;			// ��¼�ȼ�
	int set_seq;				// ����ս��seq��ֵ�仯Ҫ����void LunHuiWangChuan::OnPassSeqChange()
	int battle_monster_id;		// ս��������
	unsigned int previous_refresh_tamp;

	BitMap<MAX_LUN_HUI_WANG_CHUAN_INDEX> pass_list;

	BitMap<MAX_LUN_HUI_WANG_CHUAN_INDEX> reward_list;

	long long reserve_ll[3];
	int pass_times;				// ����ͨ�ش���
};

typedef char LunHuiWangChuanParamHex[sizeof(LunHuiWangChuanParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(LunHuiWangChuanParamHex) < 256);


#pragma pack(pop)
#endif