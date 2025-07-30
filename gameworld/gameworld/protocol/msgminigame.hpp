#ifndef __MSG_MINI_GAME_HPP__
#define __MSG_MINI_GAME_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/bitmapfunction.hpp"
#include "servercommon/minigamedef.hpp"

#pragma pack(push)
#pragma pack(4)

namespace Protocol
{
	class SCMiniGameInfo		//!< 玩家小游戏信息  4145
	{
	public:
		SCMiniGameInfo();
		MessageHeader header;

		int finish_stage_num;	//!< 完成的关卡数
		BitMap<MINI_GAME_PHASE_REWARD_MAX_SEQ> phase_reward_flag; //!< 阶段奖励标记（二进制）	

		/* MINI_GAME_PHASE_REWARD_MAX_SEQ = 32 */
	};

	class CSMiniGameFinishNextStage	//!< 玩家完成下一关小游戏  4146
	{
	public:
		CSMiniGameFinishNextStage();
		MessageHeader header;
	};

	class CSMiniGameClaimPhaseReward	//!< 小游戏阶段奖励领取  4147
	{
	public:
		CSMiniGameClaimPhaseReward();
		MessageHeader header;

		int phase_reward_seq;			//!< 领取的阶段奖励的seq
	};

	class CSMiniGameStartBattle			//!< 4148 小游戏进入战斗
	{
	public:
		CSMiniGameStartBattle();
		MessageHeader header;

		int seq;						//!< 关卡seq
		char has_pet;					//!< 是否有宠物
		char has_partner;				//!< 是否有伙伴
		char has_skill;					//!< 是否有技能
		char reserve_ch;				//!< 保留字
	};

	class CSMiniGameSetMiniGameId		//!< 4149 客户端告知小游戏id
	{
	public:
		CSMiniGameSetMiniGameId();
		MessageHeader header;

		int mini_game_id;
	};
}


#pragma pack(pop)

#endif