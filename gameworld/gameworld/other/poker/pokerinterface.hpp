#ifndef __POKER_MANAGER_HPP__
#define __POKER_MANAGER_HPP__

#include "obj/character/role.h"
#include "servercommon/internalprotocal/battleprotocol.h"

/************************************************************************/
/*								��������                                */
/************************************************************************/

const int LOAD_DATA_MAX = 255;		// ��Ҫ�洢���������

enum GetPokerReason
{
	POKER_REASON_BY_SELF = 0,			// �����������
	POKER_ERASON_BY_SYSTEM = 1,			// ϵͳ��æ����
};

enum GameMode
{
	GAME_MODE_DEFAULT,						// Ĭ�� (�ø���ֻ��һ��һ��ģʽ)
	GAME_MODE_WILD_BOSS,					//!< Ұ��boss
	GAME_MODE_SHANHAI,						//!< ɽ������
};

enum LoadData
{
	//<! Զ��ħڣ���� !>
	LOAD_DATA_MIYI_CONFIG_MiYiLevelCfg				= 0,
	LOAD_DATA_MIYI_CONFIG_MiYiMonsterCfg,

	//<!  ͨ�ø���  !>
	// Ұ��boss
	LOAD_DATA_WILDBOSS_CONFIG_WildBossOtherCfg		= 0,
	LOAD_DATA_WILDBOSS_VARIABLE_team_id,
	LOAD_DATA_WILDBOSS_INFO_TeamWildBossPokerInfo,
	LOAD_DATA_WILDBOSS_INFO_NPCMonster,
	// ɽ������
	LOAD_DATA_SHANHAI_INFO_NPCMonster,
	LOAD_DATA_SHANHAI_INFO_TeamShanHaiBossPoker,

};

class PokerInterface
{
public:
	PokerInterface() {}
	virtual ~PokerInterface() {}

	/**
	 *	@brief					���� [�������]
	 *	
	 *	@param role_uid			��ɫid
	 *	@param poker_index		��ɫ������ [1,5]
	 *	@param reson			����ԭ��  
	 *	@param game_mode		���ݲ�ͬģʽ����ͬ�߼�
	 *
	 *	#return	void
	 */
	virtual void GetPoker(int role_uid, int poker_index,int reson = POKER_REASON_BY_SELF,int game_mode = GAME_MODE_DEFAULT) {}

	/**
	*	@brief					��õĽ����߼� [ս��������ɫѭ��ʱ ����,�߼��ɰ������潱���ͷ��ƽ���]
	*	
	*	@param ack				ս���ṹ
	*	@param cur_role			��ǰ��ɫ
	*	@load_data_arr			���ݵ����ݽṹ
	*	@param game_mode		���ݲ�ͬģʽ����ͬ�߼�
	*	
	*	#return	void
	*/
	virtual void FightFinishPoker(battlegameprotocol::BattleGameFightResultAck * ack,int index, Role *role, const void *load_data_arr[LOAD_DATA_MAX],int game_mode = GAME_MODE_DEFAULT) {}

private:
	
};

#endif // !__POKER_MANAGER_HPP__
