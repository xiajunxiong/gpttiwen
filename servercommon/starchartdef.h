#ifndef __STAR_CHART_DEF__
#define __STAR_CHART_DEF__

#include "servercommon/servercommon.h"

enum STAR_CHART_STATE
{
	STAR_CHART_STATE_SKILL_LOCK = -1,		// ��ͼ��������״̬
	STAR_CHART_STATE_LOCK = 0,				// ��ͼ��λ����״̬
	STAR_CHART_STATE_SKILL_UNLOCK = STAR_CHART_STATE_LOCK,	// ��ͼ���ܽ���״̬
};

enum STAR_CHART_TYPE
{
	STAR_CHART_TYPE_ROLE = 0,		// ��ɫ��ͼ
	STAR_CHART_TYPE_PET = 1,		// ������ͼ
	STAR_CHART_TYPE_PARTNER = 2,	// �����ͼ

	STAR_CHART_TYPE_MAX,
};

enum SUPER_GALAXIESCFG_TYPE
{
	SUPER_GALAXIESCFG_TYPE_ATTR = 0,		// �������ԣ�����1Ϊ�������ͣ�����2Ϊ����ֵ
	SUPER_GALAXIESCFG_TYPE_PASSIVE = 1,		// �������ܣ�����1Ϊ����id������2Ϊ�ȼ�

	SUPER_GALAXIESCFG_TYPE_MAX,
};

static const int MAX_STAR_CHART_POINT = 20;			// ��ͼ�ڵ���(19 + 1), +1Ϊ���ֽڶ���
static const int MAX_STAR_CHART_STAGE = 5;			// ��ͼ����
static const int MAX_STAR_CHART_SKILL = 3;			// ��ͼ������
static const int MAX_SUPER_GALAXIES_POINT = 168;	// ����ϵ�ڵ���(166 + 2), +2Ϊ���ֽڶ���

static const int MAX_TAR_CHART_TYPE = 3;			// ��ͼ�������
UNSTD_STATIC_CHECK(STAR_CHART_TYPE_MAX == MAX_TAR_CHART_TYPE)

static const int STAR_CHART_RESET_POINT_RATIO = 1000;	// ���÷���ͭ��ǧ�ֱ�

#pragma pack(push, 4)
struct StarChartParam
{
	StarChartParam() { this->Reset(); }
	void Reset()
	{
		memset(super_galaxies, STAR_CHART_STATE_LOCK, sizeof(super_galaxies));
		memset(star_chart, STAR_CHART_STATE_LOCK, sizeof(star_chart));
		memset(star_chart_skill, STAR_CHART_STATE_SKILL_LOCK, sizeof(star_chart_skill));
	}

	char super_galaxies[MAX_SUPER_GALAXIES_POINT];										// ����ϵ
	char star_chart[MAX_TAR_CHART_TYPE][MAX_STAR_CHART_STAGE][MAX_STAR_CHART_POINT];	// ��ͼ
	int  star_chart_skill[MAX_STAR_CHART_SKILL][MAX_TAR_CHART_TYPE];					// ��ͼ����, ��ŵ��Ǽ����ڳ���ϵ�������
};
typedef char StarChartParamHex[sizeof(StarChartParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(StarChartParamHex) < 1024);

#pragma pack(pop)
#endif