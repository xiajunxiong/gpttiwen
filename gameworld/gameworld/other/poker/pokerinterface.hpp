#ifndef __POKER_MANAGER_HPP__
#define __POKER_MANAGER_HPP__

#include "obj/character/role.h"
#include "servercommon/internalprotocal/battleprotocol.h"

/************************************************************************/
/*								副本翻牌                                */
/************************************************************************/

const int LOAD_DATA_MAX = 255;		// 需要存储处理的数据

enum GetPokerReason
{
	POKER_REASON_BY_SELF = 0,			// 玩家主动翻牌
	POKER_ERASON_BY_SYSTEM = 1,			// 系统帮忙翻牌
};

enum GameMode
{
	GAME_MODE_DEFAULT,						// 默认 (该副本只有一种一种模式)
	GAME_MODE_WILD_BOSS,					//!< 野外boss
	GAME_MODE_SHANHAI,						//!< 山海降妖
};

enum LoadData
{
	//<! 远征魔冢副本 !>
	LOAD_DATA_MIYI_CONFIG_MiYiLevelCfg				= 0,
	LOAD_DATA_MIYI_CONFIG_MiYiMonsterCfg,

	//<!  通用副本  !>
	// 野外boss
	LOAD_DATA_WILDBOSS_CONFIG_WildBossOtherCfg		= 0,
	LOAD_DATA_WILDBOSS_VARIABLE_team_id,
	LOAD_DATA_WILDBOSS_INFO_TeamWildBossPokerInfo,
	LOAD_DATA_WILDBOSS_INFO_NPCMonster,
	// 山海降妖
	LOAD_DATA_SHANHAI_INFO_NPCMonster,
	LOAD_DATA_SHANHAI_INFO_TeamShanHaiBossPoker,

};

class PokerInterface
{
public:
	PokerInterface() {}
	virtual ~PokerInterface() {}

	/**
	 *	@brief					翻牌 [请求调用]
	 *	
	 *	@param role_uid			角色id
	 *	@param poker_index		角色翻的牌 [1,5]
	 *	@param reson			翻牌原因  
	 *	@param game_mode		根据不同模式处理不同逻辑
	 *
	 *	#return	void
	 */
	virtual void GetPoker(int role_uid, int poker_index,int reson = POKER_REASON_BY_SELF,int game_mode = GAME_MODE_DEFAULT) {}

	/**
	*	@brief					获得的奖励逻辑 [战斗后进入角色循环时 调用,逻辑可包括常规奖励和翻牌奖励]
	*	
	*	@param ack				战斗结构
	*	@param cur_role			当前角色
	*	@load_data_arr			传递的数据结构
	*	@param game_mode		根据不同模式处理不同逻辑
	*	
	*	#return	void
	*/
	virtual void FightFinishPoker(battlegameprotocol::BattleGameFightResultAck * ack,int index, Role *role, const void *load_data_arr[LOAD_DATA_MAX],int game_mode = GAME_MODE_DEFAULT) {}

private:
	
};

#endif // !__POKER_MANAGER_HPP__
