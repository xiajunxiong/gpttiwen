#ifndef __XIU_WEI_CONFIG_HPP__
#define __XIU_WEI_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/xiuweiparam.hpp"

struct XiuWeiOtherCfg			// 修为 - 其他
{
	XiuWeiOtherCfg():cost1(0), cost1_exp(0) , cost2(0) , cost2_exp(0) ,
		use_times(0) , normal_use_times(0), need_feisheng(0){}

	int cost1;					// 普通修为丹ID
	int cost1_exp;				// 普通修为丹经验增加
	int cost2;					// 高级修为丹ID
	int cost2_exp;				// 高级修为丹经验增加
	int use_times;				// 高级修为丹每日使用次数限制
	int normal_use_times;		// 低级修为丹每日使用次数限制
	int need_feisheng;			// 开启需要飞升阶段
};

struct XiuWeiAttrCfg
{
	XiuWeiAttrCfg() :xiuwei_exp(0), dam_add(0), xiaoguo_add(0), dedam_def(0),
		xiaoguo_def(0), hp_add(0), sp_id(0), open_level(0){}

	int xiuwei_exp;				// 修为升级所需经验
	int dam_add;				// 伤害加成 [千分比]
	int xiaoguo_add;			// 效果命中 [千分比]
	int dedam_def;				// 伤害减免 [千分比]
	int xiaoguo_def;			// 效果抵抗 [千分比]
	int hp_add;					// 生命加成 [固定值]
	int sp_id;

	int open_level;				// 解锁等级
};

class XiuWeiConfig : public ILogicConfig
{
public:
	XiuWeiConfig();
	virtual ~XiuWeiConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const XiuWeiOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const XiuWeiAttrCfg * GetAttrCfg(int _xiu_wei_type, int _level);
	int GetXiuWeiTypeMaxLevel(int _xiu_wei_type);

private:
	int InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitAttrCfg(TiXmlElement * RootElement, std::string& errormsg);

	XiuWeiOtherCfg	m_other_cfg;
	std::map< int, std::map<int, XiuWeiAttrCfg > > m_attr_cfg_map;	//<修为类型 < 等级 ,配置 >>
};

#endif	// __XIU_WEI_CONFIG_HPP__
