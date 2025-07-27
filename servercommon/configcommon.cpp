#include "servercommon/struct/battle/battle_def.hpp"
#include "configcommon.h"

#include <map>

int GetAttrIndexByName(const std::string name)
{
	static std::map<std::string, int> m;
	if (m.size() == 0)
	{
		m["maxhp"] = BATTLE_ATTR_MAX_HP;
		m["maxmp"] = BATTLE_ATTR_MAX_MP;
		m["gongji"] = BATTLE_ATTR_ATTACK;
		m["fangyu"] = BATTLE_ATTR_DEFENSE;
		m["minjie"] = BATTLE_ATTR_AGILITY;
		m["jingshen"] = BATTLE_ATTR_MENTAL;
		m["huifu"] = BATTLE_ATTR_RECOVERY;
		m["bisha"] = BATTLE_ATTR_CRITICAL;
		m["mingzhong"] = BATTLE_ATTR_HIT;
		m["shanduo"] = BATTLE_ATTR_DODGE;
		m["fanji"] = BATTLE_ATTR_COUNTER_ATTACK;
		m["mogong"] = BATTLE_ATTR_MAGIC_ATTACK;
		m["mokang"] = BATTLE_ATTR_MAGIC_DEFENSE;
		m["bisha_dmg"] = BATTLE_ATTR_CRITICAL_INC_VALUE;
		m["jianren"] = BATTLE_ATTR_CRITICAL_DEC_VALUE;
		m["def_zhongdu"] = BATTLE_ATTR_ANTI_POISON;
		m["def_hunshui"] = BATTLE_ATTR_ANTI_FREEZE;
		m["def_shihua"] = BATTLE_ATTR_ANTI_PETRIFY;
		m["def_hunluan"] = BATTLE_ATTR_ANTI_CHAOS;
		m["def_jiuzui"] = BATTLE_ATTR_ANTI_DRUNK;
	}

	std::map<std::string, int>::iterator iter = m.find(name);

	if (iter == m.end())
	{
		assert(0);
		return 0;
	}

	return iter->second;
}

bool StringSeperate(const std::string& str, const char split, std::vector<std::string>& res)
{
	if (str == "") return false;

	//在字符串末尾也加入分隔符，方便截取最后一段
	std::string strs = str + split;
	size_t pos = strs.find(split);

	// 若找不到内容则字符串搜索函数返回 npos
	while (pos != strs.npos)
	{
		std::string temp = strs.substr(0, pos);
		res.push_back(temp);
		//去掉已分割的字符串,在剩下的字符串中进行分割
		strs = strs.substr(pos + 1, strs.size());
		pos = strs.find(split);
	}

	return true;
}

bool GetSubNodeIntArrayValueBySeperator(TiXmlElement* element, const std::string& name, std::vector<int>& retValues, const char seperator)
{
	if (!element) return false;
	TiXmlElement *TmpElement = element->FirstChildElement(name.c_str());
	std::string nodeValueStr;
	if (!GetNodeValue(TmpElement, nodeValueStr))
	{
		return false;
	}

	std::vector<std::string> nodeValueStrVec;
	if (!StringSeperate(nodeValueStr, seperator, nodeValueStrVec))
	{
		return false;
	}

	retValues.clear();
	retValues.reserve(nodeValueStrVec.size());

	for (size_t i = 0; i < nodeValueStrVec.size(); ++i)
	{
		int tempValue = 0;
		if (!StringToBaseType(nodeValueStrVec[i], tempValue))
		{
			return false;
		}

		retValues.push_back(tempValue);
	}

	return true;
}