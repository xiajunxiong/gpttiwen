#ifndef __COURSE_OF_GROWTH_CONFIG_HPP__
#define __COURSE_OF_GROWTH_CONFIG_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"

#include <map>
#include <vector>

static const int MAX_COG_AWARD_NUM = 8;						// ���ʼ���������һ��
static const int GROWTH_TASK_NUM_PER_TYPE_STAGE = 50;	// �ɳ�֮· - ���׶��� (�׶δ�1��ʼ)
static const int GROWTH_TASK_NUM_PER_TYPE_STAR = 5;		// �ɳ�֮· - ����Ǽ��� (�Ǽ���1��ʼ)
static const int GROWTH_TASK_NUM_PER_TYPE_SEQ = 9;			// �ɳ�֮· - ����������

struct COGAwardCon
{
	COGAwardCon() : reward_exp(0), reward_coin(0), reward_num(0) {}

	int reward_exp;
	int reward_coin;
	int reward_num;
	ItemConfigData reward_list[MAX_COG_AWARD_NUM];
};

struct COGTaskCon
{
	COGTaskCon() : cog_id(0), prog(0), star(0), seq(0), condtion(0), 
		param1(0), param2(0) {}

	int cog_id;			// �������
	int prog;			// �׶εȼ�
	int star;			// �Ǽ��ȼ�
	int seq;			// ��������

	int condtion;		// ��������
	int param1;			// ����1
	int param2;			// ����2

	COGAwardCon award_con;	// ��������
};

struct CourseEntryCfg
{
	CourseEntryCfg(): seq(0), condition_type(0), param1(0), param2(0), course_reward(0), version(0)
	{}

	int seq;
	int condition_type;
	int param1;
	int param2;
	int course_reward;
	int version;		
};

struct CourseLevelRewardCfg
{
	CourseLevelRewardCfg() : need_fraction(0), exp(0), coin(0), version(0)
	{}

	int need_fraction;
	int exp;
	int coin;
	int version;
	std::vector<ItemConfigData> reward_list;
};

class CourseOfGrowthConfig : public ILogicConfig
{
public:
	CourseOfGrowthConfig();
	virtual ~CourseOfGrowthConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const COGTaskCon * GetCOGTaskInfo(int index);
	const COGTaskCon * GetCOGTaskInfo(int prog, int star, int sort);
	const COGAwardCon * GetStarAward(int prog, int star);
	int GetBossGroupID(int prog, int star);
	int GetMaxCogNum() { return max_cog_task_num; }

	void SetNextProgAndStar(UInt16 * out_prog, UInt16 * out_star);

	const std::map<int, CourseEntryCfg> & GetCourseEntryCfg() { return m_course_entry_map; }
	int GetMaxCourseSeq();
	const std::vector<CourseEntryCfg> * GetCourseEntryCfg(int condition_type);
	const CourseLevelRewardCfg * GetCourseLevelRewardCfg(int level);
	const std::map<int, CourseLevelRewardCfg> & GetCourseLevelRewardCfg() { return m_course_level_reward_cfg; }
	void CalcLevelByCourseFraction(int course_fraction, int * out_level, int * out_cur_fraction);

private:
	int InitCOGTasksList(TiXmlElement * RootElement, std::string& errormsg);
	int InitCOGStarAward(TiXmlElement * RootElement);
	int InitCOGBossGroup(TiXmlElement * RootElement);
	int InitCourseEntry(TiXmlElement * RootElement, std::string& errormsg);
	int InitCourseLevel(TiXmlElement * RootElement, std::string& errormsg);

	bool CheckCondition(int seq, int condition, int param1, int param2, std::string& errormsg);

	std::map<int, COGTaskCon> m_cog_task_list;									// ���������б� - ����Ψһindex

	// ����ӳ���б� - �����׶Ρ��Ǽ������
	int max_cog_task_num;
	COGTaskCon m_cog_task_map[GROWTH_TASK_NUM_PER_TYPE_STAGE + 1][GROWTH_TASK_NUM_PER_TYPE_STAR + 1][GROWTH_TASK_NUM_PER_TYPE_SEQ];

	std::map<int, std::map<int, int> > m_boss_group_map;						// ������ս���� - �����׶Ρ��Ǽ�

	std::map<int, std::map<int, COGAwardCon> > m_cog_promote_award;				// ���ǽ����б� - �����׼����Ǽ�

	std::map<int, CourseEntryCfg> m_course_entry_map;							// �������� key:seq
	std::map<int, std::vector<CourseEntryCfg> > m_course_entry_list;			// �������� key1:condition_type
	std::map<int, CourseLevelRewardCfg> m_course_level_reward_cfg;				// ���̽������� key: level
};

#endif // __COURSE_OF_GROWTH_CONFIG_HPP__