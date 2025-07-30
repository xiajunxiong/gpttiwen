#ifndef __SPECIAL_LOGIC_CHALLENGE_FB_HPP__
#define __SPECIAL_LOGIC_CHALLENGE_FB_HPP__

#include "scene/speciallogic.hpp"
#include "obj/character/npc_monster.h"

#include <map>

class SpecialLogicChallengeFb : public SpecialLogic
{
public:
	SpecialLogicChallengeFb(Scene * scene);
	virtual ~SpecialLogicChallengeFb();

	virtual bool CanDestroy() { return true; }
	virtual bool IsForceEncounterMineMonster();
	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_CHALLENGE_FB; }
	virtual void OnRoleEnterScene(Role *role);
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);
	virtual bool CanUseItem(Role *role, ItemID item_id, int num);
	virtual void OnUseItem(Role *role, ItemID item_id, int num);
	virtual bool IsAutoRecover() { return false; }

	bool InitChallengeScene(int fb_id, int chapter);

private:
	void OnFinishChallFb();			// �����ս����

	NPCMonster * CreateAMonster(int npc_id, const Posi & posi);
	void OnKillAMonster(int seq_id);		// ����һ������
	bool CheckIsRightOrder(int seq_id, bool & is_last);
	void ResurgenceAllMonster();

	int m_fb_id;			// ����ID
	int m_chapter;			// �½�
	int m_play_type;		// �����淨
	int m_count_param;		// ��������(�淨 - ��������)��0 - Boss��SeqId��1 - ��ɱָ�������������2 - ��ɱ˳��

	struct SpChallengeMonsterCon
	{
		SpChallengeMonsterCon(int _npc_id = 0, const Posi & _posi = Posi(0, 0)) : npc_id(_npc_id), posi(_posi.x, _posi.y), already_defeat(false) {}

		int npc_id;
		Posi posi;
		bool already_defeat;
	};

	std::map<int, SpChallengeMonsterCon> m_monster_list;		// ���ɵ������б��� - NpcId
	std::map<int, std::map<int, int> > m_item_use_times_map;	// ���ʹ�õ����͵����������� - ���ID���������ͣ�ֵ - ����ʹ�ô���
};

#endif