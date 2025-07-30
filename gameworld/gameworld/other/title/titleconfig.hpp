#ifndef __TITLE_CONFIG_HPP__
#define __TITLE_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/titledef.hpp"
#include "other/attributelist.hpp"
#include <map>
#include <vector>
#include <set>


struct TitleCfg
{
	TitleCfg()
	{
		source_type = 0;
		title_id = 0;
		time = 0;
		memset(attrs, 0, sizeof(attrs));
	}

	int source_type;
	int title_id;
	int time;		// 持续的秒数
	Attribute attrs[BATTLE_ATTR_MAX];
};

enum AUTO_TITLE_TYPE
{
	AUTO_TITLE_TYPE_INVALID = 0,
	AUTO_TITLE_TYPE_1 = 1,				//达到指定好友人数				参数1：好友人数
	AUTO_TITLE_TYPE_2 = 2,				//通关试炼之地层数				参数1：层数      参数2: 该层数的boss序号   通关的话参数2采用BOSS最大数量加1
	AUTO_TITLE_TYPE_3 = 3,				//参加单服首届竞技对抗赛		参数1：季军
	AUTO_TITLE_TYPE_4 = 4,				//封妖成功次数					参数1：封妖次数
	AUTO_TITLE_TYPE_5 = 5,				//宠物参与战斗次数				参数1：宠物战斗次数
	AUTO_TITLE_TYPE_6 = 6,				//一天之内在商城使用元宝数量	参数1：使用元宝数量
	AUTO_TITLE_TYPE_7 = 7,				//累计冲值元宝数量				参数1：充值元宝数量
	AUTO_TITLE_TYPE_8 = 8,				//开启3种宝箱累计到达指定次数	参数1：开启宝箱次数
	AUTO_TITLE_TYPE_9 = 9,				//累计寻宝次数					参数1：寻宝次数
	AUTO_TITLE_TYPE_10 = 10,			//到达指定的技能等级			参数1：技能ID  参数2：技能等级

	AUTO_TITLE_TYPE_MAX,
};

struct AutoTitleCfg
{
	AutoTitleCfg()
	{
		cond_type = AUTO_TITLE_TYPE_INVALID;
		title_id = 0;
		cond_param1 = 0;
		cond_param2 = 0;
		cond_param3 = 0;
		cond_param4 = 0;
	}
	int cond_type;
	int title_id;
	int cond_param1;
	int cond_param2;
	int cond_param3;
	int cond_param4;
};

class TitleConfig : public ILogicConfig
{
public:
	TitleConfig();
	virtual ~TitleConfig();
	virtual bool Init(const std::string &configname, std::string *err);

	const TitleCfg* GetTitleCfg(int seq);
	const AutoTitleCfg* GetAutoTitleCfg(int seq);
	const AutoTitleCfg* CheckAutoTitleFirst(int *seq,int conditon_type,int param1 = 0,int param2 = 0,int param3 = 0,int param4 = 0);//检查返回第一个称号的seq
	std::set<int> GetNotSaveTitle();
	static bool IsNeedSaveTitleType(int title_type);
private:
	int InitTitleListCfg(TiXmlElement *RootElement);
	int InitAutoTitleListCfg(TiXmlElement *RootElement);

	std::map<int, TitleCfg> m_title_map;
	std::map<int, AutoTitleCfg> m_auto_title_map;
	std::set<int> m_not_save_title_id;
};

#endif // !__COLLECTION_CONFIG_HPP__
