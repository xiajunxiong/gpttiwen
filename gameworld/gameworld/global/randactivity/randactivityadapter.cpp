#include "randactivityadapter.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "randactivitymanager.hpp"
#include "hiddenrandactivityshadow.hpp"

RandActivityAdapter* RandActivityAdapter::Instance()
{
	static RandActivityAdapter ins;
	return &ins;
}

RandActivityManager* RandActivityAdapter::GetRAManager(const UniqueServerID& usid)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return RandActivityManager::Instance();
	}
	else
	{
		return HiddenRandActivityShadow::Instance()->GetRAManager(usid);
	}
}

RandActivityAdapter::RandActivityAdapter()
{

}

RandActivityAdapter::~RandActivityAdapter()
{

}