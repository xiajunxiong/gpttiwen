#include "doorconfig.h"
#include "servercommon/configcommon.h"
//#include "scene/sceneregister/sceneregister.hpp"
#include "scene/scene.h"

#include "checkresourcecenter.hpp"

bool ConfigSceneDoor::Load(const std::string &configname, std::string &err)
{
	TiXmlDocument m_document;
	char errinfo[1024];

	if (configname == "" || !m_document.LoadFile(configname) )
	{
		sprintf(errinfo,"Load door config [%s] Error.%s", configname.c_str(),m_document.ErrorDesc());
		err = errinfo;
		return false;
	}

	TiXmlElement *RootElement = m_document.RootElement();

	if (0 == RootElement)
	{
		sprintf(errinfo,"Load door config [%s] Error.cannot find root node.", configname.c_str());
		err = errinfo;
		return false;
	}

	int tmpdoorid = 0;
	if (!GetSubNodeValue(RootElement, "id", tmpdoorid ))
	{
		err = configname + ": xml node error in node [door->id] ";
		return false;
	}
	if (tmpdoorid!=door_id)
	{
		sprintf(errinfo,"door id are not same in [%s] and scene config xml.", configname.c_str());
		err = errinfo;
		return false;
	}

	if (!GetSubNodeValue(RootElement, "level", level) || 0 > level)
	{
		err = configname + ": xml node error in node [door->level] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "type", type))// || type != Scene::SCENE_TYPE_COMMON_SCENE)
	{
		err = configname + ": xml node error in node [door->type] ";
		return false;
	}

	TiXmlElement *subElement = RootElement->FirstChildElement("targetscene");
	if (!GetSubNodeValue(subElement, "sceneid", targetscene.sceneid))
	{
		err = configname + ": xml node error in node [door->targetscene->targetid] ";
		return false;
	}
	CheckResourceCenter::Instance().GetSceneCheck()->Add(targetscene.sceneid, __FUNCTION__);

	if (!GetSubNodeValue(subElement, "x", targetscene.x))
	{
		err = configname + ": xml node error in node [door->targetscene->x] ";
		return false;
	}
	if (!GetSubNodeValue(subElement, "y", targetscene.y))
	{
		err = configname + ": xml node error in node [door->targetscene->y] ";
		return false;
	}

	//if (!SceneRegister::Instance().AddSceneLevelLimit(targetscene.sceneid, level))
	//{
	//	err = configname + ": xml node error door->level disaccord";
	//	return false;
	//}

	return true;
}




