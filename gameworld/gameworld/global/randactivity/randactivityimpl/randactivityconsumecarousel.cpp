#include "randactivityconsumecarousel.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "internalcomm.h"
#include "global/randactivity/randactivitymanager.hpp"
#include "servercommon/serverconfig/localconfig.hpp"

RandActivityConsumeCarousel::RandActivityConsumeCarousel(RandActivityManager *activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL) 
{
}

RandActivityConsumeCarousel::~RandActivityConsumeCarousel()
{
}

void RandActivityConsumeCarousel::Init(const ActivityData & data)
{
	RandActivity::Init(data);
	const RandActivityData * ra_data = &data.rand_activity_data;

	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(ra_data->data));
	m_param = *(RAShowInfoParam *)ra_data->data;
	if (m_param.count > RA_SHOW_INFO_MAX_NUM || m_param.count < 0)
	{
		m_param.Reset();			//处理由于修改存储数据顺序可能造成的错乱
		this->SetDirty();
	}
}

void RandActivityConsumeCarousel::GetInitParam(ActivityData & data)
{
	RandActivity::GetInitParam(data);

	RandActivityData * ra_data = &data.rand_activity_data;
	UNSTD_STATIC_CHECK(sizeof(m_param)<= sizeof(ra_data->data));
	
	*(RAShowInfoParam *)ra_data->data = m_param;
}

void RandActivityConsumeCarousel::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);

	if (ACTIVITY_STATUS_CLOSE == to_status)
	{
		memset(m_param.m_ra_show_data_list, 0 , sizeof(m_param.m_ra_show_data_list));
		m_param.count = 0;
		this->SetDirty();
	}
}

void RandActivityConsumeCarousel::OnAddShowListInfo(RAShowInfoParam::RAShowInfoDataParam * show_data, bool is_sync_from_cross)
{
	if(m_param.count > RA_SHOW_INFO_MAX_NUM) 
		m_param.count = RA_SHOW_INFO_MAX_NUM;
	if(m_param.count == RA_SHOW_INFO_MAX_NUM)
	{ 
		int i = 0;
		for (; i < RA_SHOW_INFO_MAX_NUM-1; i++)
		{
			m_param.m_ra_show_data_list[i] = m_param.m_ra_show_data_list[i+1];
		}
		m_param.m_ra_show_data_list[i] = *show_data;
	}
	if (m_param.count < RA_SHOW_INFO_MAX_NUM)
	{
		m_param.m_ra_show_data_list[m_param.count] = *show_data;
		m_param.count++;
	}
	this->SetDirty();

	if (!is_sync_from_cross)
	{
		if (!CrossConfig::Instance().IsHiddenServer())
		{
			crossgameprotocal::GameHiddenConsumeCarouselAddNewRecord msg;
			msg.plat_type = LocalConfig::Instance().GetPlatType();
			msg.server_id = LocalConfig::Instance().GetMergeServerId();
			msg.item_id = show_data->item_id;
			msg.item_num = show_data->num;
			F_STRNCPY(msg.role_name, show_data->role_name, sizeof(msg.role_name));
			InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
		}
		else
		{
			crossgameprotocal::HiddenGameConsumeCarouselAddNewRecord msg;
			msg.item_id = show_data->item_id;
			msg.item_num = show_data->num;
			F_STRNCPY(msg.role_name, show_data->role_name, sizeof(msg.role_name));
			const UniqueServerID& usid = m_rand_activity_manager->GetUniqueServerID();
			InternalComm::Instance().SendToGameThroughCross(usid.plat_type, usid.server_id, (const char*)&msg, sizeof(msg));
		}
	}	
}

void RandActivityConsumeCarousel::GetRAShowInfoData(RAShowInfoParam::RAShowInfoDataParam* ra_show_data_list,int len)
{
	memcpy(ra_show_data_list,m_param.m_ra_show_data_list,sizeof(RAShowInfoParam::RAShowInfoDataParam) * len);
}

void RandActivityConsumeCarousel::SyncActivityDataFromCross(CrossRandActivityDataParam &param)
{
	RandActivity::SyncActivityDataFromCross(param);
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(param.data));

	//m_param = *(RAShowInfoParam *)param.data; // 每条记录现在单独同步
}

bool RandActivityConsumeCarousel::CanSyncActivityDataToCross()
{
	return true;
}