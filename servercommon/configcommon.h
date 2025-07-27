#ifndef CONFIGCOMMON_H
#define CONFIGCOMMON_H

#include "servercommon/tinyxml/tinyxml2.h"
typedef tinyxml2::XMLElement TiXmlElement;
typedef tinyxml2::XMLText TiXmlText;

#include "servercommon/tinyxml/tinyxml_adapter.hpp"
typedef XmlDocumentAdapter TiXmlDocument;

#include <string>
#include <sstream>
#include <vector>

// 将字符串转成基础数据类型,非基础类型需显示定义
template<class T>
bool StringToBaseType(const std::string& str, T& val)
{
	std::istringstream stream;
	stream.str(str);
	if (stream >> val)
	{
		return true;
	}
	return false;
}

template<class T>
bool StringToBaseType(const char* str, T&val)
{
	std::istringstream stream;
	stream.str(str);
	if (stream>>val)
	{
		return true;
	}
	return false;
}

template< >
inline bool StringToBaseType(const std::string& str, std::string& val)
{
	val = str;
	return true;
}

template< >
inline bool StringToBaseType(const char* str, int& val)
{
	return tinyxml2::XMLUtil::ToInt(str, &val);
}

template< >
inline bool StringToBaseType(const char* str, double& val)
{
	return tinyxml2::XMLUtil::ToDouble(str, &val);
}

template< >
inline bool StringToBaseType(const char* str, float& val)
{
	return tinyxml2::XMLUtil::ToFloat(str, &val);
}

template< >
inline bool StringToBaseType(const char* str, unsigned int& val)
{
	return tinyxml2::XMLUtil::ToUnsigned(str, &val);
}

template< >
inline bool StringToBaseType(const char* str, bool& val)
{
	return tinyxml2::XMLUtil::ToBool(str, &val);
}

template< >
inline bool StringToBaseType(const char* str, int64_t& val)
{
	return tinyxml2::XMLUtil::ToInt64(str, &val);
}

template< >
inline bool StringToBaseType(const char* str, uint64_t& val)
{
	return tinyxml2::XMLUtil::ToUnsigned64(str, &val);
}

// 将基础数据类型转成字符串,非基础类型需显示定义
template<class T>
bool BaseTypeToString(const T& src, std::string& tar)
{
	std::ostringstream stream;
	if (stream<<src)
	{
		tar = stream.str();
		return true;
	}
	return false;
}

template<class T>
inline bool BaseTypeToBuffer(const T& src, char* buffer, int buffer_size)
{
	std::ostringstream stream;
	if (stream << src)
	{
		strncpy(buffer, stream.str().c_str(), buffer_size);
		return true;
	}
	return false;
}

template< >
inline bool BaseTypeToBuffer(const int& src, char* buffer, int buffer_size)
{
	tinyxml2::XMLUtil::ToStr(src, buffer, buffer_size);
	return true;
}

template< >
inline bool BaseTypeToBuffer(const double& src, char* buffer, int buffer_size)
{
	tinyxml2::XMLUtil::ToStr(src, buffer, buffer_size);
	return true;
}

template< >
inline bool BaseTypeToBuffer(const float& src, char* buffer, int buffer_size)
{
	tinyxml2::XMLUtil::ToStr(src, buffer, buffer_size);
	return true;
}

template< >
inline bool BaseTypeToBuffer(const unsigned int& src, char* buffer, int buffer_size)
{
	tinyxml2::XMLUtil::ToStr(src, buffer, buffer_size);
	return true;
}

template< >
inline bool BaseTypeToBuffer(const bool& src, char* buffer, int buffer_size)
{
	tinyxml2::XMLUtil::ToStr(src, buffer, buffer_size);
	return true;
}

template< >
inline bool BaseTypeToBuffer(const int64_t& src, char* buffer, int buffer_size)
{
	tinyxml2::XMLUtil::ToStr(src, buffer, buffer_size);
	return true;
}

template< >
inline bool BaseTypeToBuffer(const uint64_t& src, char* buffer, int buffer_size)
{
	tinyxml2::XMLUtil::ToStr(src, buffer, buffer_size);
	return true;
}


// 读写element的子节点name的值，不适用多个同名子节点
// <element>
//		……
//		<name>s_val</name>
//		……
// </element>

// 读写如下节点element的值,不支持形如<element/>的空值
// <element>s_val</element>
template<class T>
static bool GetNodeValue(TiXmlElement* element, T& val)
{
	if (!element || !element->FirstChild() || !StringToBaseType(element->FirstChild()->Value(), val))
	{
		return false;
	}
	return true;
}

template<class T>
static bool SetNodeValue(TiXmlElement* element, T& val)
{
	static const int BUFFER_SIZE = 128;
	static char buffer[BUFFER_SIZE];
	buffer[BUFFER_SIZE - 1] = 0;

	if (!element || !element->FirstChild() || !BaseTypeToBuffer(val, buffer, BUFFER_SIZE))
	{
		return false;
	}

	buffer[BUFFER_SIZE - 1] = 0;
	element->FirstChild()->SetValue(buffer);
	return true;
}

template<class T>
static bool GetSubNodeValue(TiXmlElement *element, const char* name, T& val)
{
	if (NULL == element || NULL == name) return false;
	TiXmlElement *TmpElement = element->FirstChildElement(name);
	return GetNodeValue(TmpElement, val);
}

template<class T>
static bool SetSubNodeValue(TiXmlElement *element, const char* name, T& val)
{
	if (NULL == element || NULL == name) return false;
	TiXmlElement *TmpElement = element->FirstChildElement(name);
	return SetNodeValue(TmpElement, val);
}

template<class T>
static bool GetSubNodeValue(TiXmlElement *element, const std::string& name, T& val)
{
	if (!element) return false;
	TiXmlElement *TmpElement = element->FirstChildElement(name.c_str());
	return GetNodeValue(TmpElement, val);
}

template<class T>
static bool SetSubNodeValue(TiXmlElement *element, const std::string& name, T& val)
{
	if (!element) return false;
	TiXmlElement *TmpElement = element->FirstChildElement(name.c_str());
	return SetNodeValue(TmpElement, val);
}

bool StringSeperate(const std::string& str, const char split, std::vector<std::string>& res);


bool GetSubNodeIntArrayValueBySeperator(TiXmlElement* element, const std::string& name, std::vector<int>& retValues, const char seperator);

// 对应至少一个子节点的xml结构
// <Element>
//		<node_0>value</node_0>
//		<node_1>value</node_1>
//		<node_2>value</node_2>
//		……
//		<Element>……</Element>
// </Element>
class ConfigElementBase
{
public:
	ConfigElementBase():m_element(0) {}
	virtual ~ConfigElementBase() {}

	void SetElement(TiXmlElement *element) { m_element = element; }

	// 供子类列表中获取偏移地址
	virtual ConfigElementBase* offset() = 0;
	// 将子节点读到相应成员数据中
	virtual bool read() = 0;
	// 将成员数据写入文件
	virtual bool write() = 0;

protected:
	// 获取子节点的值,不适用有多个同名子节点的情况
	template<class T>
	bool GetValue(const char *name, T* value)
	{
		if (m_element == 0) return false;
		return GetSubNodeValue(m_element, name, *value);
	}
	bool GetValue(const char *name, ConfigElementBase* value)
	{
		if (m_element == 0) return false;
		value->SetElement(m_element->FirstChildElement(name));
		return value->read();
	}

	// 获取子节点的值,适用有多个同名子节点的情况,与数组对应
	template<class T>
	bool GetListValue(const char *name, T* list_value, int& list_len, int list_max_len)
	{
		list_len = 0;
		if (m_element == 0)
		{
			return false;
		}
		TiXmlElement *tmp_element = m_element->FirstChildElement(name);
		for ( ;list_len < list_max_len && tmp_element != 0; ++list_len, ++list_value, tmp_element = tmp_element->NextSiblingElement(name))
		{
			if (!GetNodeValue(tmp_element, *list_value)) return false;
		}
		return true;
	}
	bool GetListValue(const char *name, ConfigElementBase* list_value, int& list_len, int list_max_len)
	{
		list_len = 0;
		if (m_element == 0)
		{
			return false;
		}
		TiXmlElement *tmp_element = m_element->FirstChildElement(name);
		for ( ;list_len < list_max_len && tmp_element != 0; ++list_len)
		{
			list_value->SetElement(tmp_element);
			if (!list_value->read())return false;
			list_value = list_value->offset();
			tmp_element = tmp_element->NextSiblingElement(name);
		}
		return true;
	}

	// 设置子节点的值,不适用有多个同名子节点的情况
	template<class T>
	bool SetValue(const char *name, T* value)
	{
		if (m_element == 0) return false;

		TiXmlElement * tmp_element = m_element->FirstChildElement(name);
		if (!tmp_element)
		{
			tmp_element = m_element->InsertNewChildElement(name);
		}
		if (!tmp_element->FirstChild())
		{
			std::string tmp_value;
			BaseTypeToString(*value, tmp_value);
			tmp_element->InsertNewText(tmp_value.c_str());
			return true;
		}
		return SetSubNodeValue(m_element, name, *value);
	}
	bool SetValue(const char *name, ConfigElementBase* value)
	{
		if (m_element == 0) return false;
		TiXmlElement * tmp_element = m_element->FirstChildElement(name);
		if (!tmp_element)
		{
			m_element->InsertNewChildElement(name);
			tmp_element = m_element->FirstChildElement(name);
		}
		value->SetElement(tmp_element);
		return value->write();
	}

	// 设置子节点的值,适用有多个同名子节点的情况,与数组对应
	template<class T>
	bool SetListValue(const char *name, T* list_value, int list_len)
	{
		if (m_element == 0 || list_len < 0)
		{
			return false;
		}
		TiXmlElement *tmp_element = m_element->FirstChildElement(name);
		for(int i=0;i<list_len;++i,++list_value)
		{
			if (!tmp_element)
			{
				tmp_element = m_element->InsertNewChildElement(name);
			}
			if (!tmp_element->FirstChild())
			{
				std::string tmp_value;
				BaseTypeToString(*list_value, tmp_value);
				tmp_element->InsertNewText(tmp_value.c_str());
			}
			else
			{
				if (!SetNodeValue(tmp_element, *list_value)) return false;
			}
			tmp_element = tmp_element->NextSiblingElement(name);
		}
		while(tmp_element)
		{
			TiXmlElement *to_remove = tmp_element;
			tmp_element = tmp_element->NextSiblingElement(name);
			m_element->DeleteChild(to_remove);
		};
		return true;
	}
	bool SetListValue(const char *name, ConfigElementBase* list_value, int list_len)
	{
		if (m_element == 0 || list_len < 0)
		{
			return false;
		}
		TiXmlElement *tmp_element = m_element->FirstChildElement(name);
		for(int i=0;i<list_len;++i)
		{
			if (!tmp_element)
			{
				tmp_element = m_element->InsertNewChildElement(name);
			}

			list_value->SetElement(tmp_element);
			if (!list_value->write()) return false;

			list_value= list_value->offset();
			tmp_element = tmp_element->NextSiblingElement(name);
		}
		while(tmp_element)
		{
			TiXmlElement *to_remove = tmp_element;
			tmp_element = tmp_element->NextSiblingElement(name);
			m_element->DeleteChild(to_remove);
		};
		return true;
	}


	// 至少有一个节点的xml结构
	TiXmlElement *m_element;

private:
};

struct ItemPairCon		// 道具 ID <-> 数量 组
{
	ItemPairCon(unsigned short _item_id = 0, short _item_num = 0) : item_id(_item_id), item_num(_item_num) {}

	unsigned short item_id;
	short item_num;
};

struct ParamSection
{
	ParamSection(int _val_min = 0, int _val_max = 0) : seq(0), section_min(_val_min), section_max(_val_max) {}

	int seq;
	int section_min;
	int section_max;

	bool operator != (const ParamSection & cobj) const
	{
		return this->section_min != cobj.section_min || this->section_max != cobj.section_max || this->seq != cobj.seq;
	}

	bool operator == (const ParamSection & cobj) const
	{
		return this->section_min == cobj.section_min && this->section_max == cobj.section_max && this->seq == cobj.seq;
	}

	bool operator < (const ParamSection & cobj) const
	{
		if (this->section_min != cobj.section_min)
		{
			return this->section_min < cobj.section_min;
		}
		return this->section_max < cobj.section_max;
	}
};

class ILogicConfig
{
public:
	ILogicConfig() {}
	virtual ~ILogicConfig() {}

	virtual bool Init(const std::string &configname, std::string *err) = 0;
};

int GetAttrIndexByName(const std::string name);		//通过配置表字段名获取属性index，不存在返回0

namespace globalconfig
{

#define CONFIG_INIT(config_struct, config_name) \
	CurrElement = RootElement->FirstChildElement(config_name);\
	if (!config_struct.InitConfig(configname, err, CurrElement, config_name)) return false;

#define START_CHECK_NODE \
	char errinfo[512] = {0};\
	char nodeinfo[64] = {0};\
	F_STRNCPY(nodeinfo, config_name, sizeof(nodeinfo));\
	if (NULL == node)\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in [config->%s].\ncannot find node.", file_name.c_str(), config_name);\
		*err = errinfo;\
		return false;\
	}\
	TiXmlElement *sub_node = node->FirstChildElement("data");\
	if (NULL == sub_node)\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in [config->%s] cannot find node.", file_name.c_str(), config_name);\
		*err = errinfo;\
		return false;\
	}


#define GET_NODE_DATA(mem, str, can_be_zero)\
	if (!GetSubNodeValue(sub_node, str, mem) || mem < 0)\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in [config->%s].%s", file_name.c_str(), nodeinfo, str);\
		*err = errinfo;\
		return false;\
	}\
	if (!can_be_zero && 0 == mem)\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in [config->%s].%s", file_name.c_str(), nodeinfo, str);\
		*err = errinfo;\
		return false;\
	}

#define GET_UNSIGNED_NODE_DATA(mem, str, can_be_zero)\
	if (!GetSubNodeValue(sub_node, str, mem))\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in [config->%s].%s", file_name.c_str(), nodeinfo, str);\
		*err = errinfo;\
		return false;\
	}\
	if (!can_be_zero && 0 == mem)\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in [config->%s].%s", file_name.c_str(), nodeinfo, str);\
		*err = errinfo;\
		return false;\
	}

#define CHECK_SENTENCE(sentence, str)\
	if (sentence)\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in [config->%s] %s", file_name.c_str(), nodeinfo, str);\
		*err = errinfo;\
		return false;\
	}\


#define GET_NEXT_NODE \
	sub_node = sub_node->NextSiblingElement("data");\
	if (NULL == sub_node) break;

#define PRE_LOAD_CONFIG \
	char errinfo[1024] = {0};\
	TiXmlDocument document;\
	if (configname == "" || !document.LoadFile(configname.c_str()))\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "Load config [%s] Error.%s", configname.c_str(), document.ErrorStr());\
		*err = errinfo;\
		return false;\
	}\
	TiXmlElement *RootElement = document.RootElement();\
	if (NULL == RootElement)\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "Load config [%s] Error.cannot find root node.", configname.c_str());\
		*err = errinfo;\
		return false;\
	}\
	int iRet = 0;


#define LOAD_CONFIG(element_name, init_func)\
	{\
		TiXmlElement *root_element = RootElement->FirstChildElement(element_name);\
		if (NULL == root_element)\
		{\
			*err = configname + ": no [" element_name "].";\
			return false;\
		}\
		iRet = this->init_func(root_element);\
		if (iRet)\
		{\
			SNPRINTF(errinfo, sizeof(errinfo), "%s: %s failed %d", configname.c_str(), #init_func, iRet);\
			*err = errinfo;\
			return false;\
		}\
	}

#define LOAD_CONFIG_2(element_name, init_func)\
	{\
		TiXmlElement *root_element = RootElement->FirstChildElement(element_name);\
		if (NULL == root_element)\
		{\
			*err = configname + ": no [" element_name "].";\
			return false;\
		}\
		std::string error_msg;\
		iRet = this->init_func(root_element, error_msg);\
		if (iRet)\
		{\
			SNPRINTF(errinfo, sizeof(errinfo), "%s: %s failed %d, errormsg[%s]", configname.c_str(), #init_func, iRet, error_msg.c_str());\
			*err = errinfo;\
			return false;\
		}\
	}

#define PRE_LOAD_MULTI_LIST\
	char errinfo[1024] = { 0 };\
	std::string configname;\
	int iRet = 0;\
	TiXmlDocument m_document;\

#define READ_NEW_LIST(list_name)\
	configname = list_name;\
	if (configname == "" || !m_document.LoadFile((dir + configname).c_str()))\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: Load Config Error, %s.", (dir + configname).c_str(), m_document.ErrorStr());\
		*err = errinfo;\
		return false;\
	}\
	TiXmlElement *RootElement = m_document.RootElement();\
	if (NULL == RootElement)\
	{\
		*err = configname + ": xml root node error.";\
		return false;\
	}
}

#define LOAD_ITEM_NODE(REWARD_LIST_STR,REWARD_STR,LIST,NUM) \
{\
	NUM = 0;\
	TiXmlElement *reward_list_element = data_element->FirstChildElement(REWARD_LIST_STR);\
	if (NULL != reward_list_element) \
	{\
		TiXmlElement *reward_element = reward_list_element->FirstChildElement(REWARD_STR);\
		while (NULL != reward_element)\
		{\
			ItemConfigData item_data;\
			if (!item_data.ReadConfig(reward_element))\
			{\
				return -400001;\
			}\
			LIST[NUM] = item_data;\
			if(++NUM > (sizeof(LIST) / sizeof(LIST[0])))\
			{\
				return -500001;\
			}\
			reward_element = reward_element->NextSiblingElement();\
		}\
	}\
}

#define LOAD_ITEM_VETOR_NODE(REWARD_LIST_STR,REWARD_STR,VECTOR_NODE) \
{\
	TiXmlElement *reward_list_element = data_element->FirstChildElement(REWARD_LIST_STR);\
	if (NULL != reward_list_element) \
	{\
		TiXmlElement *reward_element = reward_list_element->FirstChildElement(REWARD_STR);\
		while (NULL != reward_element)\
		{\
			ItemConfigData item_data;\
			if (!item_data.ReadConfig(reward_element))\
			{\
				return -400001;\
			}\
			VECTOR_NODE.push_back(item_data);\
			reward_element = reward_element->NextSiblingElement();\
		}\
	}\
}

#endif