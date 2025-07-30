#ifndef __KUA_FU_YING_XING_LU_CONFIG_HPP__
#define __KUA_FU_YING_XING_LU_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/struct/global/kuafuyingxiongludef.hpp"
#include "gamedef.h"

struct KuaFuYingXiongLuOtherCfg
{
	KuaFuYingXiongLuOtherCfg() : is_can_open_activity(false)
	{}

	bool is_can_open_activity;	//是否可以开启活动
};

struct KuaFuYingXiongLuTimeCfg
{
	KuaFuYingXiongLuTimeCfg() : start_time(0), continuou_times(0)
	{}

	int start_time;				//第几天开启
	int continuou_times;		//持续天数
};

struct KuaFuYingXiongLuRankCfg
{
	KuaFuYingXiongLuRankCfg() : rank_type(0), seq(0)
	{}

	int rank_type;
	int seq;
	std::vector<ItemConfigData> reward_list;
};

struct KuaFuYingXiongLuTaskCfg
{
	KuaFuYingXiongLuTaskCfg() : rank_type(0), seq(0), task_type(0), param1(0), param2(0)
	{}

	int rank_type;
	int seq;
	int task_type;
	int param1;
	int param2;
	std::vector<ItemConfigData> reward_list;
};

union KuaFuYingXiongLuKey
{
	int param[2];
	long long unique_key;
};
class KuaFuYingXiongLuConfig : public ILogicConfig
{
public:
	KuaFuYingXiongLuConfig();
	virtual ~KuaFuYingXiongLuConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const KuaFuYingXiongLuOtherCfg & GetOtherCfg() const { return m_other_cfg; }
	const KuaFuYingXiongLuTimeCfg * GetTimeCfg(int rank_type)const;

	void GetRankVec(int section_day, int rank_type, std::vector<KuaFuYingXiongLuRankCfg> & rank_vec) const;

	const KuaFuYingXiongLuTaskCfg * GetTaskCfg(int section_day, int rank_type, int seq) const;
	const std::vector<KuaFuYingXiongLuTaskCfg> * GetTaskVec(int section_day, int rank_type)const;

private:
	int InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitTimeCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitRankCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitTaskCfg(TiXmlElement * RootElement, std::string& errormsg);

	long long GetSectionKey(int section_start, int section_end);
	void GetInfoByKey(long long _info_key, int * info_param1, int * info_param2)const;

	KuaFuYingXiongLuOtherCfg m_other_cfg;
	KuaFuYingXiongLuTimeCfg m_time_cfg_list[KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX];		//时间配置
	std::map<long long , std::vector<KuaFuYingXiongLuRankCfg> > m_rank_cfg_list[KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX];	//排行配置 数组下标:rank_type key:GetSectionKey vector:序号
	std::map<long long , std::vector<KuaFuYingXiongLuTaskCfg> > m_task_cfg_list[KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX];	//任务配置 数组下标:rank_type key:GetSectionKey vector:序号
};


#endif	//__KUA_FU_YING_XING_LU_CONFIG_HPP__