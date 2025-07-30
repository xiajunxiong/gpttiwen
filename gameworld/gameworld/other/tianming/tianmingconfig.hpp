#ifndef __TIAN_MING_CONFIG_HPP__
#define __TIAN_MING_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/tianmingparam.h"

struct TianMingCfg
{
	TianMingCfg()
	{
		need_exp = 0;
		att_type = 0;
		att_per = 0;
		att_per_tol = 0;
	}

	int need_exp;			// 升级到该等级所需的经验
	int att_type;			// 加成的系统类型
	int att_per;			// 该等级的加成 , 非总值 (应该用不上 ,先读着)
	int att_per_tol;		// 总加成 , 指该系统类型的该属性加成到该等级时的总加成值
};

// -----------------------------------------------------------------------------------
class TianMingConfig : public ILogicConfig
{
public:
	TianMingConfig();
	virtual ~TianMingConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const TianMingCfg * GetTianMingCfg(int _level);

private:
	int InitTianMingCfg(TiXmlElement * RootElement);

	TianMingCfg  m_tian_ming_cfg_arr[MAX_TIAN_MING_LEVEL_COUNT]; 

};

#endif	// __TIAN_MING_CONFIG_HPP__
