#ifndef __MSG_STAR_CHART__
#define __MSG_STAR_CHART__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/commondata.hpp"
#include "servercommon/starchartdef.h"

#pragma pack(push, 4)

namespace Protocol
{

class CSStarChartRequest
{
public:
	CSStarChartRequest();		//!<5320 ��ͼϵͳ����
	MessageHeader header;

	enum OP_TYPE
	{
		OP_TYPE_STAR_CHART_LEVEL_UP = 0,			// ��ͼ��λ����, param1-��ͼ����(0-2) param2-��ͼ��λ����, param3-��ͼ����
		OP_TYPE_SUPER_GALAXIES_LEVEL_UP = 1,		// ����ϵ��λ����, param1-����ϵ��λ����
// 		OP_TYPE_STAR_CHART_RESET = 2,				// ������ͼ, param1-��ͼ����	(�߻�: ȡ��С��ͼ�����ù���)
		OP_TYPE_SUPER_GALAXIES_RESET = 3,			// ���ó���ϵ
// 		OP_TYPE_STAR_CHART_UNLOCK_SKILL = 4,		// �������ܿ�λ, param1-��ͼ����(0-2) param2-���ܿ�λ����	(�߻�Ҫ�Զ�����, ��������)
		OP_TYPE_STAR_CHART_INSTALL_SKILL = 5,		// װ������, param1-��ͼ����(0-2) param2-���ܿ�λ���� param3-�����ڳ���ϵ������
		OP_TYPE_STAR_CHART_ONE_KEY_UPGRADE = 6,		// ��ͼ��λһ������, param1-��ͼ����(0-2) param2-��ͼ��λ����, param3-��ͼ����
		OP_TYPE_SUPER_GALAXIES_ONE_KEY_UPGRADE = 7,	// ����ϵ��λһ������, param1-����ϵ��λ����
	};

	int op_type;
	int param1;
	int param2;
	int param3;
};

class SCStarChartAllInfo		//!<5321 ��ͼϵͳ��Ϣ�·�
{
public:
	SCStarChartAllInfo();
	MessageHeader header;

	char super_galaxies[MAX_SUPER_GALAXIES_POINT];										// ����ϵ
	char star_chart[MAX_TAR_CHART_TYPE][MAX_STAR_CHART_STAGE][MAX_STAR_CHART_POINT];	// ��ͼ
	int  star_chart_skill[MAX_STAR_CHART_SKILL][MAX_TAR_CHART_TYPE];					// ��ͼ����, ��ŵ��Ǽ����ڳ���ϵ�������
};

class SCStarChartInstallSkillNotice		//!<5322 ��ͼװ������֪ͨ
{
public:
	enum NOTICE_TYPE	// ֪ͨ����
	{
		NOTICE_TYPE_USED = 0,	// װ������
		NOTICE_TYPE_ACTIVE = 1,	// �����
	};

public:
	SCStarChartInstallSkillNotice();
	MessageHeader header;

	int notice_type;	// SCStarChartInstallSkillNotice::NOTICE_TYPE
	int skill_seq;		// װ���ļ����ڳ���ϵ������
};


};

#pragma pack(pop)
#endif