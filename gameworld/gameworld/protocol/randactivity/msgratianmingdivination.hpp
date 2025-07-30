#ifndef __MSG_RA_TIAN_MING_DIVINATION_HPP__
#define __MSG_RA_TIAN_MING_DIVINATION_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ratianmingdivinationparam.hpp"

//////////////////////////////////////////// 2077  ����  ��������  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_TIANMING_DIVINATION_OPERA_TYPE
	{
		RA_TIANMING_DIVINATION_OPERA_TYPE_QUERY_INFO = 0,			// ������Ϣ
		RA_TIANMING_DIVINATION_OPERA_TYPE_ADD_LOT_TIMES = 1,		// ��ǩ��ע
		RA_TIANMING_DIVINATION_OPERA_TYPE_RESET_ADD_LOT_TIMES = 2,	// ������ǩ��ע����
		RA_TIANMING_DIVINATION_OPERA_TYPE_START_CHOU = 3,			// ��ʼ��ǩ
		RA_TIANMING_DIVINATION_OPERA_TYPE_REWARD_POOL_INFO = 4,		// �·���ʷ����

		RA_TIANMING_DIVINATION_OPERA_TYPE_MAX
	};

	class SCRATianMingDivinationInfo							// ����������Ϣ 1235
	{
		struct HistoryItem
		{
			char seq;											// �����±�
			char add_lot;										// �񽱼�ע����
		};

	public:
		SCRATianMingDivinationInfo();
		MessageHeader header;

		int free_chou_times;															// ����������Ѳ��Դ���
		char add_lots_list[RA_TIANMING_LOT_COUNT];										// ��ǩ��ע
		short reserve_sh;
		unsigned int ra_tianming_today_chou_tims;										// ���ճ��˼���
	};

	class SCTianMingDivinationActivityStartChouResult			// �������Ի񽱽�� 
	{
	public:
		SCTianMingDivinationActivityStartChouResult();
		MessageHeader header;

		short reward_index;										// ����Ʒ�±�
		short reserve_sh;
	};

	class SCTianMingRewardPoolInfo								// �������Ի񽱳��б�
	{
	public:
		const static int MAX_POOL_NUM = 40;
		SCTianMingRewardPoolInfo();
		MessageHeader header;

		int pool_num;
		TianMingDivinationInfo pool_info[MAX_POOL_NUM];
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_TIAN_MING_DIVINATION_HPP__


