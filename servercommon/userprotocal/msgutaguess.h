#ifndef __MSG_UTA_GUESS_H__
#define __MSG_UTA_GUESS_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/userprotocal/msgcode.h"
#include "servercommon/commondata.hpp"

#include "servercommon/struct/bigcross/utaguess/utaguessparam.hpp"


#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum UTA_GUESS_CLIENT_REQ_TYPE
	{
		UTA_GUESS_CLIENT_REQ_TYPE_GET_INFO = 0,		//!< ��ȡ�������µ���Ϣ param1: match_idx�������
		UTA_GUESS_CLIENT_REQ_TYPE_BET = 1,			//!< ������ע 
													//!< param1: match_idx������� 
													//!< param2: team_idx�������
													//!< param3: ��ע�Ľ��

		UTA_GUESS_CLIENT_REQ_TYPE_GET_RECORD = 2,	//!< ��ȡ�Լ���ע���ı�����¼
		UTA_GUESS_CLIENT_REQ_TYPE_GET_BET_REWARD = 3,	//!< ��ȡ���½��� 
														//!< param1:[-1:һ����ȡ ,��-1Ϊmatch_idx������� ]
														//!< param2:team_idx�������[param1��-1ʱ����]
		
		UTA_GUESS_CLIENT_REQ_TYPE_GET_HISTORY_EARNING = 4, //!< ��ȡ�ۼ�����
	};

	struct CSUTAGuessClienReq				// 4968  ���µ�һ����[���]���� �ͻ��˲���
	{
		CSUTAGuessClienReq() :header(MT_UTA_GUESS_CLIENT_REQ_CS) {}
		MessageHeader header;

		int req_type;						// ��������
		long long param_1;
		long long param_2;
		int param_3;
	};

	struct SCUTAGuessMatchInfo			// 4969 ĳһ�������ľ�����Ϣ
	{
		struct TeamInfo
		{
			TeamInfo():team_seq(-1), plat_id(-1), server_id(-1), total_bet_num(0), guess_status(0), winning(0),
				win_rate(0), bet_coin_num(0)
			{
				memset(prof, 0, sizeof(prof));
			}

			long long team_seq;								//!< ��ע�Ķ������
			int plat_id;									//!< ����ƽ̨ID
			int server_id;									//!< ����������ID
			int total_bet_num;								//!< ������ܵı���ע�Ľ��
			int guess_status;								//!< ����״̬  [0:������ע , 1:ֹͣ��ע , 2:����[��ʤ] , 3����[ʧ��]]
			int prof[MAX_TEAM_MEMBER_NUM];					//!< �����ְҵ��Ϣ
			int winning;									//!< ��ʤ����
			int win_rate;									//!< ʤ��
			int bet_coin_num;								//!< ��ҶԱ���˫������ע��Ϣ
		};

		SCUTAGuessMatchInfo() :header(MT_UTA_GUESS_MATCH_INFO_SC) {}
		MessageHeader header;

		long long match_idx;
		TeamInfo team_info[MAX_MATCH_TEAM_NUM];		// ����˫���������ע��Ϣ				
	};

	struct SCUTAGuessBetRecord				// 4970 ��Ҳ�����ע���б�
	{
		struct TeamInfo
		{
			TeamInfo() :team_idx(0), bet_coin_num(0), is_get(0), rate(0), leader_headshot_id(0), leader_avatar_type(0)
			{
				memset(team_name, 0, sizeof(team_name));
			}

			long long team_idx;								//!< ����ID
			GameName team_name;								//!< ������
			int bet_coin_num;								//!< ��ע�Ľ��
			int is_get;										//!< �Ƿ�����ȡ
			int rate;										//!< ����
			short leader_headshot_id;						//!< �ӳ�ͷ��
			short leader_avatar_type;						//!< �ӳ�ģ������
		};

		struct MatchInfo
		{
			MatchInfo():match_idx(0), type(0), fight_result(0), round_idx(0), is_win_group(0) {}

			long long match_idx;		//!< ����ID	
			char type;					//!< ��������  0:�ʸ���  1:����
			char fight_result;			//!< ������� 0:δ��  1:����ʤ   2:�ط�ʤ  3: �ڴ� , 4:ƽ��
			char round_idx;				//!< �ִ� ����typeһ��ȷ����ʼʱ�䣩
			char is_win_group;			//!< �Ƿ�ʤ���飨������ʱ��Ч��
		
			TeamInfo team_info[MAX_MATCH_TEAM_NUM];
		};

		SCUTAGuessBetRecord() :header(MT_UTA_GUESS_BET_RECORD_SC) {}
		MessageHeader header;

		int is_add;				// �Ƿ������[�����ע�ı����ر��,��ȡʱ���ܻ��յ�����Э��]
		int count;
		MatchInfo match_info[MAX_ONCE_UTA_GUESS_COUNT];
	};

	struct SCUTAGuessHistoryEarning	//!< 4971 �����ۼ�����
	{
		SCUTAGuessHistoryEarning() : header(MT_UTA_GUESS_HISTORY_EARNING_SC) {}
		MessageHeader header;

		int history_earning;	//!< �ۼ�����
	};

	struct SCUTAGuessHasBetRewardNotice //!< 4972 �п���ȡ���½���֪ͨ
	{
		SCUTAGuessHasBetRewardNotice() : header(MT_UTA_GUESS_HAS_BET_REWARD_SC) {}
		MessageHeader header;
	};
}

#pragma pack(pop)

#endif // __MSG_UTA_GUESS_H__

