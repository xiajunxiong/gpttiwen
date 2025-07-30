#ifndef __SURFACE_CONFIG_HPP__
#define __SURFACE_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/surfacedef.hpp"
#include "other/attributelist.hpp"
#include "servercommon/struct/itemlistparam.h"
#include <map>
#include <vector>


struct SurfaceCfg
{
	SurfaceCfg()
	{
		id = 0;
		type = 0;
		suit_id = 0;
		decompose_item = 0;
		decompose_num = 0;
		duration = 0;
	}

	int id;
	int type;
	int suit_id;
	int decompose_item;
	int decompose_num;
	int duration;
};

struct SurfaceSuitCfg
{
	SurfaceSuitCfg()
	{
		suit_id = 0;
		suit_num = 0;
		memset(attrs, 0, sizeof(attrs));
	}

	int suit_id;
	int suit_num;
	Attribute attrs[BATTLE_ATTR_MAX];
};

struct SurfaceExchangeCfg
{
	SurfaceExchangeCfg()
	{
		seq = 0;
		consume_item_id = 0;
		consume_num = 0;
		dealline_time = 0;
	}

	int seq;
	int consume_item_id;
	int consume_num;
	int dealline_time;
	ItemConfigData reward;
};

struct SurfaceAttrCfg
{
	SurfaceAttrCfg()
	{
		memset(attrs, 0, sizeof(attrs));
	}

	Attribute attrs[BATTLE_ATTR_MAX];
};

struct SurfaceAttrValueCfg
{
	SurfaceAttrValueCfg()
	{}
	
	SurfaceAttrCfg prof_attr[PROFESSION_TYPE_NUM];		//对应职业,0为新手
};

class SurfaceConfig : public ILogicConfig
{
public:
	SurfaceConfig();
	virtual ~SurfaceConfig();
	virtual bool Init(const std::string &configname, std::string *err);

	const SurfaceCfg* GetSurfaceCfg(int id);
	const SurfaceSuitCfg* GetSurfaceSuitCfg(int id);
	const SurfaceExchangeCfg * GetSurfaceExchangeCfg(int seq);
	const SurfaceAttrCfg * GetSurfaceAttrCfgByIdAndProf(int id, int prof);

private:
	int InitSurfaceListCfg(TiXmlElement *RootElement);
	int InitSuitListCfg(TiXmlElement *RootElement);
	int InitExchangeCfg(TiXmlElement * RootElement);
	int InitSuitValueCfg(TiXmlElement *RootElement);

	std::map<int, SurfaceCfg> m_surface_map;
	std::map<int, SurfaceSuitCfg> m_surface_suit_map;
	std::vector<SurfaceExchangeCfg> m_exchange_cfg;
	std::map<int, SurfaceAttrValueCfg> m_surface_attr_value_map;	//以时装ID为key
};

#endif // !__SURFACE_CONFIG_HPP__
