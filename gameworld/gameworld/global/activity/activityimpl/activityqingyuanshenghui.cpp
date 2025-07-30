#include "world.h"
#include "scene/scenemanager.hpp"

#include "scene/speciallogicimpl/speciallogicqingyuanshenghui.hpp"
#include "activityqingyuanshenghui.hpp"

ActivityQingYuanShengHui::ActivityQingYuanShengHui(ActivityManager * activity_manager) :Activity(activity_manager, ACTIVITY_TYPE_QING_YUAN_SHENG_HUI)
{
}

ActivityQingYuanShengHui::~ActivityQingYuanShengHui()
{
}

void ActivityQingYuanShengHui::OnActivityStatusChange(int from_status, int to_status)
{
	Activity::OnActivityStatusChange(from_status, to_status);

	switch (to_status)
	{
		case ACTIVITY_STATUS_CLOSE:
			{
				//todo gfp
				SpecialLogicQingYuanShengHui * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicQingYuanShengHui(323, COMMON_FB_KEY);
				if (special_lobgic != NULL)
				{
					special_lobgic->OnActivityClose();
				}
			}
			break;
		case ACTIVITY_STATUS_STANDY:
			{
			}
			break;

		case ACTIVITY_STATUS_OPEN:
			{

				SpecialLogicQingYuanShengHui * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicQingYuanShengHui(323, COMMON_FB_KEY);
				if (special_lobgic != NULL)
				{
					special_lobgic->OnActivityBegin();
				}
			}
			break;
	}
}

void ActivityQingYuanShengHui::Update(unsigned long interval, time_t now_second)
{
	Activity::Update(interval, now_second);
}
