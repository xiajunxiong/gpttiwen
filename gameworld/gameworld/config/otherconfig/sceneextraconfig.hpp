#ifndef __SCENE_EXTRA_CONFIG_HPP__
#define __SCENE_EXTRA_CONFIG_HPP__

#include "servercommon/configcommon.h"

static const int MAX_SCENE_ID = 3000;

struct ExtraSceneCfg
{
	ExtraSceneCfg() : scene_id(0), is_show_other_role(false) {}

	int scene_id;
	bool is_show_other_role;
};

class SceneExtraConfig : public ILogicConfig
{
public:
	SceneExtraConfig();
	virtual ~SceneExtraConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	bool IsShowOtherRole(int scene_id);
	const ExtraSceneCfg* GetExtraSceneCfg(int scene_id);
private:
	int InitSceneExtraCfg(TiXmlElement *RootElement);

	ExtraSceneCfg* m_extra_cfg_list[MAX_SCENE_ID];
};

#endif