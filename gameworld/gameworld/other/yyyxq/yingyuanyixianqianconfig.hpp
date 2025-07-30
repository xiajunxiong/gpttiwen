#ifndef __YIN_YUAN_YI_XIAN_QIAN_CONFIG_HPP__
#define __YIN_YUAN_YI_XIAN_QIAN_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "gamedef.h"
#include "npc/npcpool.h"

#include <map>
#include <vector>

struct YYYXQOtherConfig
{
	YYYXQOtherConfig() : yjcnpc_ID(0), scene_ID(0), x(0), y(0), dir_y(0), npc_id_1(0){}
	int yjcnpc_ID;
	int scene_ID;
	int x;
	int y;
	int dir_y;
	int npc_id_1;
};

class YYYXQConfig : public ILogicConfig
{
public:
	YYYXQConfig();
	virtual ~YYYXQConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const YYYXQOtherConfig * GetOtherCfg() { return &m_other_cfg; }
private:
	int InitOhterCfg(TiXmlElement *RootElement);

	YYYXQOtherConfig m_other_cfg;
};

#endif