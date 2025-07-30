#include "mapconfig.h"
#include "servercommon/configcommon.h"

bool ConfigMap::Load(const std::string& configname)
{
	TiXmlDocument document;

	if (configname == "" || !document.LoadFile(configname))
	{
		lasterr = "Load MapConfig Error:" + (std::string)document.ErrorDesc();
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();
	if (!RootElement)
	{
		lasterr = configname + ": xml node error in root";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "id", id))
	{
		lasterr = configname + ": xml node error in node [id] ";
		return false;
	}
	
	if (!GetSubNodeValue(RootElement, "width", width))
	{
		lasterr = configname + ": xml node error in node [width] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "height", height))
	{
		lasterr = configname + ": xml node error in node [height] ";
		return false;
	}

	if (width <= 0 || height <= 0)
	{
		lasterr = configname + ": config error in node [height] and [width], low than 0 is not allowed.";
		return false;
	}

	// 处理mask，不可为空
	if (!GetSubNodeValue(RootElement, "mask", mask ))
	{
		lasterr = configname + ": xml node error in node [mask] ";
		return false;
	}

	if (width*height != (int)mask.length())
	{
		lasterr = configname + ": xml node error: [width * height] not equal to [mask] ";
		return false;
	}

	return true;
}

