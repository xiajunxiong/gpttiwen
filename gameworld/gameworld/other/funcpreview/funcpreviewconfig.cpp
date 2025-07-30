#include "funcpreviewconfig.hpp"

FuncPreviewConfig::FuncPreviewConfig() : m_func_preview_cfg_num(0)
{

}

FuncPreviewConfig::~FuncPreviewConfig()
{

}

bool FuncPreviewConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("notice", InitFuncPreviewCfg);

	return true;
}

const FuncPreviewCfg* FuncPreviewConfig::GetFuncPreviewCfg(int notice_id)
{
	if (notice_id < 0 || notice_id >= m_func_preview_cfg_num)
	{
		return NULL;
	}

	if (m_func_preview_cfg_list[notice_id].not_use == 1)
	{
		return NULL;
	}

	return &m_func_preview_cfg_list[notice_id];
}

int FuncPreviewConfig::InitFuncPreviewCfg(TiXmlElement *RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	bool notice_flag[MAX_FUNC_PREVIEW_NUM] = { 0 };

	m_func_preview_cfg_num = 0;
	while (dataElement)
	{
		int notice_id = 0;
		if (!GetSubNodeValue(dataElement, "notice_id", notice_id) 
			|| notice_id < 0 || notice_id >= MAX_FUNC_PREVIEW_NUM)
		{
			return -1;
		}
		if (notice_flag[notice_id])
		{
			return -2; // 重复了
		}
		notice_flag[notice_id] = true;

		FuncPreviewCfg& cfg = m_func_preview_cfg_list[notice_id];
		cfg.notice_id = notice_id;

		if (!GetSubNodeValue(dataElement, "start_level", cfg.start_level)
			|| cfg.start_level < 0 || cfg.start_level >= MAX_ROLE_LEVEL)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "end_level", cfg.end_level)
			|| cfg.end_level <= cfg.start_level || cfg.end_level > MAX_ROLE_LEVEL)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "reset", cfg.version) || cfg.version < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "not_use", cfg.not_use) || cfg.not_use < 0 || cfg.not_use > 1)
		{
			return -99;
		}

		TiXmlElement* reward_item_list = dataElement->FirstChildElement("reward_item_list");
		if (NULL == reward_item_list)
		{
			return -7;
		}

		TiXmlElement * reward_item = reward_item_list->FirstChildElement("reward_item");
		while (NULL != reward_item)
		{
			if (cfg.reward_num >= FuncPreviewCfg::REWARD_NUM) return -8;

			if (!cfg.reward_list[cfg.reward_num++].ReadConfig(reward_item))
			{
				return -8;
			}

			reward_item = reward_item->NextSiblingElement();
		}

		dataElement = dataElement->NextSiblingElement();

		m_func_preview_cfg_num = GetMax(m_func_preview_cfg_num, notice_id + 1);
	}


	for (int i = 0; i < m_func_preview_cfg_num; ++i)
	{
		if (!notice_flag[i])
		{
			return -100; // 有某个id空出来了，不连续
		}
	}

	return 0;
}

