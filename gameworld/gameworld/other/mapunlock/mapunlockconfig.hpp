#ifndef __MAP_UNLOCK_CONFIG_HPP__
#define __MAP_UNLOCK_CONFIG_HPP__

#include "gamedef.h"
#include "servercommon/configcommon.h"
#include "servercommon/mapunlockdef.h"

#include <set>
#include <map>
#include <vector>

enum UNLOCK_TYPE		// 解锁条件
{
	MAP_UNLOCK_TYPE_INVALID 	= 0,		// 无效的类型	参数1
	MAP_UNLOCK_TYPE_TASK		= 1,		// 完成任务		任务ID
	MAP_UNLOCK_TYPE_LEVEL		= 2,		// 达到等级		需求等级
	MAP_UNLOCK_TYPE_PASS_FB		= 3,		// 通关副本		副本类型
	MAP_UNLOCK_TYPE_BIG_DIPPER	= 4,		// 通关北斗七星 星君序号
	MAP_UNLOCK_TYPE_FEISHEN		= 5,		// 角色飞升		飞升阶段

	MAP_UNLOCK_TYPE_MAX
};

struct UnlockSceneInfo		// - 键：场景ID
{
	UnlockSceneInfo() : active_id(0), irregular_id(0), condition_id(0) {}

	int active_id;
	int irregular_id;
	int condition_id;
};

struct UnlockCondition		// 解锁条件 - 键：解锁类型
{
	UnlockCondition() : unlock_type(0), unlock_param(0) {}

	int unlock_type;		// 解锁类型
	int unlock_param;		// 条件参数
};

class MapUnlockConfig : public ILogicConfig
{
public:
	MapUnlockConfig();
	virtual ~MapUnlockConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const UnlockCondition * GetUnlockCondition(int condition_id);

	void GetConditionIDListByIrregularID(int ir_id, std::set<int> & out_list);
	void GetUnlockInfoBySceneID(int scene_id, std::vector<UnlockSceneInfo> & out_list);
	bool IsCanEnterScene(int scene_id, int role_level);
	int GetActiveID(int scene_id);
	int GetIrregularID(int scene_id);
	int GetMaxIrregularID() { return m_irregular_id_to_condition_id.rbegin()->first; }

private:
	int InitWorldAreasInfo(TiXmlElement * RootElement);
	int InitSceneUnlockCon(TiXmlElement * RootElement);

	std::map<int, int> m_scene_id_to_active_id;							// 场景ID 对应 激活ID 唯一映射表；
	std::map<int, int> m_scene_id_to_irregular;							// 场景ID 对应 点亮ID 唯一映射表；
	std::map<int, std::vector<UnlockSceneInfo> > m_unlock_scene_map;	// 场景ID 对应 基本信息 映射表；
	std::map<int, UnlockCondition> m_unlock_condi_map;					// 条件ID 对应 条件内容 映射表；
	std::map<int, std::set<int> > m_irregular_id_to_condition_id;		// 不规则ID 对应 条件ID 映射表；
};

#endif