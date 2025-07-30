#ifndef __FACE_SCORE_CONFIG_HPP__
#define __FACE_SCORE_CONFIG_HPP__

#include "gamedef.h"
#include "servercommon/configcommon.h"
#include "servercommon/facescoredef.h"
#include "servercommon/struct/battle/battle_def.hpp"

#include <map>

struct FaceScoreOthers
{
	FaceScoreOthers() : gold_exchange_rate (0) {}

	int gold_exchange_rate;		// 元宝兑换颜值币的比例：表示 1 元宝可兑换颜值币的数量；
};

struct FaceAttrAddInfo
{
	FaceAttrAddInfo() : need_score(0), max_hp_add(0), max_mp_add(0)
	{
		memset(base_add, 0, sizeof(base_add));
		memset(amen_add, 0, sizeof(amen_add));
		memset(resi_add, 0, sizeof(resi_add));
	}

	int need_score;				// 到达这个属性加成所需的颜值分
	int max_hp_add;				// 血上限加成
	int max_mp_add;				// 魔上限加成
	int base_add[BATTLE_BASE_ATTR_NUM];		// 攻、防、敏、回、精；
	int amen_add[BATTLE_AMEN_ATTR_NUM];		// 修正值
	int resi_add[BATTLE_RESI_ATTR_NUM];		// 抵抗值
};

struct FaceAttrAddInfoProGroup		//颜值系统等级属性职业组
{
	FaceAttrAddInfoProGroup()
	{}
	
	FaceAttrAddInfo node_cfg[PROFESSION_TYPE_NUM];		//下标0为新手
};

struct FaceResourceInfo
{
	FaceResourceInfo() : type(0), exp_add(0), title_id(0)
	{}

	int type;				// 类型 \see FACE_SCORE_TYPE
	int exp_add;			// 给予的颜值经验
	int title_id;			// 当type不是称号时, 为0
	long time;				// 持续时间 单位 s
};

class FaceScoreConfig : public ILogicConfig
{
public:
	FaceScoreConfig();
	virtual ~FaceScoreConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	int GetMaxLevel() { return m_face_score_attr_add_map.rbegin()->first; }
	int GetSeqByTitleID(int title_id);
	int GetUplevelNeedExp(int level, int job);
	void InitLevelAndExpByTotalExp(int * out_level, int * out_exp, int job);

	const FaceAttrAddInfo * GetAttrAddCfgByLevel(int level, int job);
	const FaceResourceInfo * GetResInfoCfg(int seq);

private:
	int InitFacesScoresOthersCfg(TiXmlElement * RootElement);
	int InitLevelAttrAdditionCfg(TiXmlElement * RootElement);
	int InitFaceItemsBuyPriceCfg(TiXmlElement * RootElement);

	FaceScoreOthers m_face_others;
	std::map<int, FaceAttrAddInfoProGroup> m_face_score_attr_add_map;
	std::map<int, FaceResourceInfo> m_face_res_info_map;

	// 预处理
	std::map<int, int> m_title_to_res_map;			// 称号ID 转换 资源ID 映射：键 - 称号ID；值 - 资源ID
};

#endif