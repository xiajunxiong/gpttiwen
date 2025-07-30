#ifndef __MSG_RA_SERVER_COMPETITION_HPP__
#define __MSG_RA_SERVER_COMPETITION_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raservercompetitionparam.hpp"

#pragma pack(push, 4)

static const int MAX_SERVER_COMPETITION_RANK_USER_COUNT = 10;	// ÿ�����а��������

namespace Protocol
{
	// ��Э��3000
	enum RA_SERVER_COMPETITION_REQ_TYPE
	{
		RA_SERVER_COMPETITION_REQ_TYPE_ROLE_ALL_INFO = 0,	// �����·���ɫ��Ϣ
		RA_SERVER_COMPETITION_REQ_TYPE_FETCH_REWARD = 1,	// ������ȡ���� P1:rank_type P2:seq
		RA_SERVER_COMPETITION_REQ_TYPE_ACTIVITY_INFO = 2,	// ������Ϣ�·�
		RA_SERVER_COMPETITION_REQ_TYPE_FETCH_DAY_GIFT = 3,	// ������ȡÿ�����
		RA_SERVER_COMPETITION_REQ_TYPE_ALL_RANK_INFO = 4,	// �������а���Ϣ
	};

	struct RAServerCompetitionSingleRoleInfo
	{
		RAServerCompetitionSingleRoleInfo()
		{}

		int cur_rank_type_value;													// �ð񵥵�ǰ��ֵ
		int max_rank_type_value;													// ��ڼ�ð񵥴ﵽ�������ֵ
		BitMap<MAX_SERVER_COMPETITION_RANK_TYPE_SEQ_NUM> reward_fetch_flag;			// ������ȡ��ʶ
	};

	class SCRAServerCompetitionRoleAllInfo		//!< 3107 ����-ȫ����ƴ��ɫ������Ϣ�·�
	{
	public:
		SCRAServerCompetitionRoleAllInfo();
		MessageHeader header;

		char is_fetch_day_gift_flag;		// �Ƿ���ȡÿ����� 0:δ��ȡ 1:����ȡ
		char reserve_ch;
		short reserve_sh;
		int count;
		RAServerCompetitionSingleRoleInfo rank_info_list[MAX_SERVER_COMPETITION_RANK_TYPE_NUM];
	};

	class SCRAServerCompetitionSingleRoleInfo	//!< 3108 ����-ȫ����ƴ��ɫ������Ϣ�·�
	{
	public:
		SCRAServerCompetitionSingleRoleInfo();
		MessageHeader header;

		int rank_type;
		RAServerCompetitionSingleRoleInfo rank_info;
	};

	class SCRAServerCompetitionActivityInfo		//!< 3109 ����-ȫ����ƴ���Ϣ�·�
	{
	public:
		SCRAServerCompetitionActivityInfo();
		MessageHeader header;

		int count;
		unsigned int rank_type_end_timestamp_list[MAX_SERVER_COMPETITION_RANK_TYPE_NUM];		//�������а����ʱ���
	};

	class SCRAServerCompetitionOtherInfo	//!< 3137 ����-ȫ����ƴ������Ϣ�·�
	{
	public:
		SCRAServerCompetitionOtherInfo();
		MessageHeader header;

		char is_fetch_day_gift_flag;		// �Ƿ���ȡÿ����� 0:δ��ȡ 1:����ȡ
		char reserve_ch;
		short reserve_sh;
	};

	struct SCRAServerCompetitionRankType
	{
		SCRAServerCompetitionRankType():uid(0), param(0)
		{}

		struct Top
		{
			Top()
			{
				memset(name, 0, sizeof(GameName));
				profession = 0;
				level = 0;
				top_level = 0;
				capability = 0;
				headshot_id = 0;
				avatar_type = 0;
			}

			GameName name;
			short profession;
			short level;
			int top_level;
			int capability;
			short headshot_id;
			short avatar_type;
		};

		int uid;
		int param;			// ����,��ħ֮�����ǵ�
		Top appearance_info;
	};

	class SCRAServerCompetitionAllRankInfo		//!<5137 ����-�·���ƴȫ�����а���Ϣ
	{
	public:
		SCRAServerCompetitionAllRankInfo();
		MessageHeader header;

		SCRAServerCompetitionRankType rank_info[SEVER_COMPETITION_RANK_TYPE_MAX][MAX_SERVER_COMPETITION_RANK_USER_COUNT];
	};

}

#pragma pack(pop)

#endif	//__MSG_RA_SERVER_COMPETITION_HPP__