#ifndef __MSG_FUNC_GUIDE_H__
#define __MSG_FUNC_GUIDE_H__

#include "gamedef.h"
#include "servercommon/serverdef.h"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/funcguidedef.h"

#pragma pack(push, 4)

namespace Protocol
{
	class CSFuncGuideOperator
	{
	public:
		CSFuncGuideOperator();		//!< 1830 - �������� ��������
		MessageHeader header;

		enum FuncGuideOpType
		{
			FUNC_GUIDE_CHOOSE_PET = 0,		// ѡ�����ֳ��� param��ѡ��� seq
			FUNC_GUIDE_CATCH_PET_BATTLE = 1,// ����ץ��ս��
			FUNC_GUIDE_FINISH_CLOUD_ARENA_GUIDE = 2, // ����ƶ˾���ָ��
			FUNC_GUIDE_FINISH_LUNHUI_WANGCHUAN_GUIDE = 3, // ����ֻ�����ָ��
		};

		int op_type;
		int param;
	};

	class SCFuncGuideFlagInfo
	{
	public:
		SCFuncGuideFlagInfo();		//!< 1831 - �������� �¼����
		MessageHeader header;

		BitMap<FUNC_GUIDE_FLAG_MAX> flag_info;	// �¼���� - ��ӦλΪ 1 ��ʾ���¼��Ѵ�����
	};

	class SCPetDeadSummonGuideInfo		//!< 1833 �������������ٻ�ָ����ÿ�������Ϣ�·�
	{
	public:
		SCPetDeadSummonGuideInfo();
		MessageHeader header;

		short guide_trigger_times[PET_DEAD_SUMMON_GUIDE_RECORD_DAYS];	// ÿ��Ĵ������������鳤��8
	};

	class CSPetDeadSummonGuideTrigger	//!< 1832 �������������ٻ�ָ��
	{
	public:
		CSPetDeadSummonGuideTrigger();
		MessageHeader header;
	};
}

#pragma pack(pop)
#endif