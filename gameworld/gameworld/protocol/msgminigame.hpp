#ifndef __MSG_MINI_GAME_HPP__
#define __MSG_MINI_GAME_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/bitmapfunction.hpp"
#include "servercommon/minigamedef.hpp"

#pragma pack(push)
#pragma pack(4)

namespace Protocol
{
	class SCMiniGameInfo		//!< ���С��Ϸ��Ϣ  4145
	{
	public:
		SCMiniGameInfo();
		MessageHeader header;

		int finish_stage_num;	//!< ��ɵĹؿ���
		BitMap<MINI_GAME_PHASE_REWARD_MAX_SEQ> phase_reward_flag; //!< �׶ν�����ǣ������ƣ�	

		/* MINI_GAME_PHASE_REWARD_MAX_SEQ = 32 */
	};

	class CSMiniGameFinishNextStage	//!< ��������һ��С��Ϸ  4146
	{
	public:
		CSMiniGameFinishNextStage();
		MessageHeader header;
	};

	class CSMiniGameClaimPhaseReward	//!< С��Ϸ�׶ν�����ȡ  4147
	{
	public:
		CSMiniGameClaimPhaseReward();
		MessageHeader header;

		int phase_reward_seq;			//!< ��ȡ�Ľ׶ν�����seq
	};

	class CSMiniGameStartBattle			//!< 4148 С��Ϸ����ս��
	{
	public:
		CSMiniGameStartBattle();
		MessageHeader header;

		int seq;						//!< �ؿ�seq
		char has_pet;					//!< �Ƿ��г���
		char has_partner;				//!< �Ƿ��л��
		char has_skill;					//!< �Ƿ��м���
		char reserve_ch;				//!< ������
	};

	class CSMiniGameSetMiniGameId		//!< 4149 �ͻ��˸�֪С��Ϸid
	{
	public:
		CSMiniGameSetMiniGameId();
		MessageHeader header;

		int mini_game_id;
	};
}


#pragma pack(pop)

#endif