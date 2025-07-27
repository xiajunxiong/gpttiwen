#ifndef __TINYXML_ADAPTER_HPP__
#define __TINYXML_ADAPTER_HPP__

#include "tinyxml2.h"
#include <string>

class XmlDocumentInterface
{
public:
	virtual bool LoadFile(const std::string& str) = 0;
	virtual const char* ErrorStr() = 0;
	virtual const char* ErrorDesc() = 0;
	virtual TiXmlElement* FirstChildElement(const char * name) = 0;
};

class XmlDocumentAdapter : public XmlDocumentInterface
{
public:
	XmlDocumentAdapter() {}
	virtual ~XmlDocumentAdapter() {}

	virtual bool LoadFile(const std::string& str)
	{
		int error = m_document.LoadFile(str.c_str());
		if (tinyxml2::XML_SUCCESS == error)
		{
			return true;
		}
		return false;
	}

	virtual bool SaveFile(const std::string& str)
	{
		int error = m_document.SaveFile(str.c_str());
		if (tinyxml2::XML_SUCCESS == error)
		{
			return true;
		}
		return false;
	}

	virtual const char* ErrorStr()
	{
		return m_document.ErrorStr();
	}

	virtual const char* ErrorDesc()
	{
		return m_document.ErrorStr();
	}

	tinyxml2::XMLElement* RootElement()
	{
		return m_document.RootElement();
	}

	virtual TiXmlElement* FirstChildElement(const char * name)
	{
		return m_document.FirstChildElement(name);
	}

private:
	tinyxml2::XMLDocument m_document;
};

#endif