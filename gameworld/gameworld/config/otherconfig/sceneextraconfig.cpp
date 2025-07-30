#include "sceneextraconfig.hpp"
#include "servercommon/servercommon.h"

SceneExtraConfig::SceneExtraConfig()
{
	memset(m_extra_cfg_list, 0, sizeof(m_extra_cfg_list));
}

SceneExtraConfig::~SceneExtraConfig()
{
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_extra_cfg_list); ++i)
	{
		if (NULL != m_extra_cfg_list[i])
		{
			delete m_extra_cfg_list[i];
			m_extra_cfg_list[i] = NULL;
		}
	}
}

bool SceneExtraConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("teshu_scene", InitSceneExtraCfg);

	return true;
}

bool SceneExtraConfig::IsShowOtherRole(int scene_id)
{
	const ExtraSceneCfg* cfg = this->GetExtraSceneCfg(scene_id);
	if (NULL == cfg) return true; // 找不到就返回true 

	return cfg->is_show_other_role;
}

const ExtraSceneCfg* SceneExtraConfig::GetExtraSceneCfg(int scene_id)
{
	if (scene_id < 0 || scene_id >= ARRAY_ITEM_COUNT(m_extra_cfg_list))
	{
		return NULL;
	}

	return m_extra_cfg_list[scene_id];
}

int SceneExtraConfig::InitSceneExtraCfg(TiXmlElement *RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int scene_id = 0;
		if (!GetSubNodeValue(dataElement, "scene_id", scene_id) || scene_id < 0 || scene_id >= ARRAY_ITEM_COUNT(m_extra_cfg_list))
		{
			return -1;
		}

		int is_show = 0;
		if (!GetSubNodeValue(dataElement, "is_show", is_show))
		{
			return -2;
		}

		ExtraSceneCfg* cfg = new ExtraSceneCfg();
		cfg->scene_id = scene_id;
		cfg->is_show_other_role = is_show > 0;

		m_extra_cfg_list[cfg->scene_id] = cfg;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

