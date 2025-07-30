#include "xiuweiconfig.hpp"

#include "item/itempool.h"
#include "checkresourcecenter.hpp"

XiuWeiConfig::XiuWeiConfig()
{

}

XiuWeiConfig::~XiuWeiConfig()
{

}

bool XiuWeiConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("other", InitOtherCfg);
	LOAD_CONFIG_2("xiuwei_att", InitAttrCfg);

	return true;
}

const XiuWeiAttrCfg * XiuWeiConfig::GetAttrCfg(int _xiu_wei_type, int _level)
{
	std::map< int, std::map<int, XiuWeiAttrCfg > >::const_iterator it = m_attr_cfg_map.find(_xiu_wei_type);
	if (m_attr_cfg_map.end() != it)
	{
		std::map<int, XiuWeiAttrCfg >::const_iterator it_sub = it->second.find(_level);
		if (it->second.end() != it_sub)
		{
			return &it_sub->second;
		}
	}

	return NULL;
}

int XiuWeiConfig::GetXiuWeiTypeMaxLevel(int _xiu_wei_type)
{
	std::map< int, std::map<int, XiuWeiAttrCfg > >::const_iterator it = m_attr_cfg_map.find(_xiu_wei_type);
	if (m_attr_cfg_map.end() != it)
	{
		return it->second.size();
	}

	return 0;
}

int XiuWeiConfig::InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "cost1", m_other_cfg.cost1) || NULL == ITEMPOOL->GetItem(m_other_cfg.cost1))
	{
		errormsg = STRING_SPRINTF("[cost1=%d] error, NULL == ITEMPOOL->GetItem(%d)", m_other_cfg.cost1, m_other_cfg.cost1);
		return -1;
	}

	if (!GetSubNodeValue(root_element, "cost1_exp", m_other_cfg.cost1_exp) || m_other_cfg.cost1_exp <= 0)
	{
		errormsg = STRING_SPRINTF("[cost1_exp=%d] error, cost1_exp <= 0", m_other_cfg.cost1_exp);
		return -2;
	}

	if (!GetSubNodeValue(root_element, "cost2", m_other_cfg.cost2) || NULL == ITEMPOOL->GetItem(m_other_cfg.cost2))
	{
		errormsg = STRING_SPRINTF("[cost2=%d] error, NULL == ITEMPOOL->GetItem(%d)", m_other_cfg.cost2, m_other_cfg.cost2);
		return -3;
	}

	if (!GetSubNodeValue(root_element, "cost2_exp", m_other_cfg.cost2_exp) ||m_other_cfg.cost2_exp <= 0)
	{
		errormsg = STRING_SPRINTF("[cost2_exp=%d] error, cost2_exp <= 0", m_other_cfg.cost2_exp);
		return -4;
	}

	if (!GetSubNodeValue(root_element, "use_times", m_other_cfg.use_times) || m_other_cfg.use_times <= 0)
	{
		errormsg = STRING_SPRINTF("[use_times=%d] error, use_times <= 0", m_other_cfg.use_times);
		return -5;
	}

	if (!GetSubNodeValue(root_element, "use_times1", m_other_cfg.normal_use_times) || m_other_cfg.normal_use_times <= 0)
	{
		errormsg = STRING_SPRINTF("[use_times1=%d] error, use_times1 <= 0", m_other_cfg.normal_use_times);
		return -6;
	}

	if (!GetSubNodeValue(root_element, "need_feisheng", m_other_cfg.need_feisheng) || m_other_cfg.need_feisheng < 0)
	{
		errormsg = STRING_SPRINTF("[need_feisheng=%d] error, need_feisheng < 0", m_other_cfg.need_feisheng);
		return -7;
	}

	return 0;
}

int XiuWeiConfig::InitAttrCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int last_type = -1, last_level = -1;
	while (NULL != root_element)
	{
		int type = 0 , level = 0;
		if (!GetSubNodeValue(root_element, "xiuwei_type", type) || type < 0)
		{
			errormsg = STRING_SPRINTF("[xiuwei_type] error , node no exist or xiuwei_type < 0");
			return -1;
		}

		if (!GetSubNodeValue(root_element, "xiuwei_level", level) || level <= 0)
		{
			errormsg = STRING_SPRINTF("[xiuwei_level] error , node no exist or xiuwei_level <= 0");
			return -2;
		}

		if (-1 != last_type && type != last_type)
		{
			last_level = -1;
		}

		if (-1 == last_level && 1 != level)
		{
			errormsg = STRING_SPRINTF("[xiuwei_level] error , The starting level must be 1");
			return -201;
		}

		if (-1 != last_level && last_level + 1 != level)
		{
			errormsg = STRING_SPRINTF("[xiuwei_level] error , xiuwei_level need to be consecutively numbered");
			return -202;
		}

		XiuWeiAttrCfg & temp_cfg = m_attr_cfg_map[type][level];

		if (!GetSubNodeValue(root_element, "xiuwei_exp", temp_cfg.xiuwei_exp) || temp_cfg.xiuwei_exp <= 0)
		{
			errormsg = STRING_SPRINTF("[xiuwei_exp=%d] error, node no exist or  xiuwei_exp <= 0 ", temp_cfg.xiuwei_exp);
			return -3;
		}

		if (!GetSubNodeValue(root_element, "dam_add", temp_cfg.dam_add) || temp_cfg.dam_add < 0)
		{
			errormsg = STRING_SPRINTF("[dam_add=%d] error,  node no exist or  dam_add < 0 ", temp_cfg.dam_add);
			return -4;
		}

		if (!GetSubNodeValue(root_element, "xiaoguo_add", temp_cfg.xiaoguo_add) || temp_cfg.xiaoguo_add < 0)
		{
			errormsg = STRING_SPRINTF("[xiaoguo_add=%d] error,  node no exist or  xiaoguo_add < 0 ", temp_cfg.xiaoguo_add);
			return -5;
		}

		if (!GetSubNodeValue(root_element, "dedam_def", temp_cfg.dedam_def) || temp_cfg.dedam_def < 0)
		{
			errormsg = STRING_SPRINTF("[dedam_def=%d] error,  node no exist or  dedam_def < 0 ", temp_cfg.dedam_def);
			return -6;
		}

		if (!GetSubNodeValue(root_element, "xiaoguo_def", temp_cfg.xiaoguo_def) || temp_cfg.xiaoguo_def < 0)
		{
			errormsg = STRING_SPRINTF("[xiaoguo_def=%d] error,  node no exist or  xiaoguo_def < 0 ", temp_cfg.xiaoguo_def);
			return -7;
		}

		if (!GetSubNodeValue(root_element, "hp_add", temp_cfg.hp_add) || temp_cfg.hp_add < 0)
		{
			errormsg = STRING_SPRINTF("[hp_add=%d] error,  node no exist or  hp_add < 0 ", temp_cfg.hp_add);
			return -8;
		}
		
		if (!GetSubNodeValue(root_element, "open_level", temp_cfg.open_level) || temp_cfg.open_level < 0 || temp_cfg.open_level > MAX_ROLE_LEVEL)
		{
			errormsg = STRING_SPRINTF("[open_level=%d] error,  node no exist or  open_level < 0 || open_level > [MAX_ROLE_LEVEL]%d", temp_cfg.hp_add , MAX_ROLE_LEVEL);
			return -9;
		}

		if (!GetSubNodeValue(root_element, "sp_id", temp_cfg.sp_id) || temp_cfg.sp_id < 0)
		{
			errormsg = STRING_SPRINTF("[sp_id=%d] error,  node no exist or  sp_id < 0 ", temp_cfg.sp_id);
			return -10;
		}
		if (temp_cfg.sp_id > 0)
		{
			CheckResourceCenter::Instance().GetSpecialEffectCheck()->AddSpecial(temp_cfg.sp_id, __FUNCTION__);
		}

		last_level = level;
		last_type = type;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

