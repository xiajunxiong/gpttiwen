#include "normalpublicnotice.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "servercommon/serverdef.h"
#include "servercommon/serverconfig/crossconfig.hpp"

NormalPublicNotice::NormalPublicNotice() : m_curr_normal_notice_index(0), 
	m_next_normal_notice_time(0), m_normal_notice_count(0)
{

}

NormalPublicNotice::~NormalPublicNotice()
{

}

void NormalPublicNotice::Init(const UniqueServerID& usid)
{
	m_usid = usid;
}

void NormalPublicNotice::AddNormalNotice(const PublicNoticeList::NoticeAttr &notice_attr)
{
	if (m_normal_notice_count >= 0 && m_normal_notice_count < PublicNoticeList::MAX_PUBLICNOTICE_NUM)
	{
		m_normal_noticelist[m_normal_notice_count++] = notice_attr;
	}
}

void NormalPublicNotice::UpdateNormalNotice(unsigned int now_second)
{
	if (now_second > m_next_normal_notice_time && m_normal_notice_count > 0 && m_normal_notice_count <= PublicNoticeList::MAX_PUBLICNOTICE_NUM)
	{
		for (; m_curr_normal_notice_index < m_normal_notice_count; m_curr_normal_notice_index++)
		{
			const PublicNoticeList::NoticeAttr &notice_attr = m_normal_noticelist[m_curr_normal_notice_index];
			if (now_second > notice_attr.begin_time && now_second < notice_attr.end_time)
			{
				++m_curr_normal_notice_index;
				m_next_normal_notice_time = static_cast<unsigned int>(now_second + notice_attr.send_interval);

				if (!CrossConfig::Instance().IsHiddenServer())
				{
					const char *notice_str = notice_attr.notice.c_str();
					int notice_len = static_cast<int>(notice_attr.notice.length());
					World::GetInstWorld()->GetSceneManager()->SystemMsgAll(notice_str, notice_len, GetMsgType(notice_attr.type),
						notice_attr.color, 0);
				}
				else
				{
					const char *notice_str = notice_attr.notice.c_str();
					int notice_len = static_cast<int>(notice_attr.notice.length());
					World::GetInstWorld()->GetSceneManager()->SystemMsgFilter(notice_str, notice_len, GetMsgType(notice_attr.type),
						notice_attr.color, 0, 0, 0, UniqueServerFilter, &m_usid);
				}

				break;
			}
		}

		if (m_curr_normal_notice_index >= m_normal_notice_count)
		{
			m_normal_notice_count = 0; m_curr_normal_notice_index = 0;
		}
	}
}

void NormalPublicNotice::ResetNormalNotice()
{
	m_normal_notice_count = 0; m_curr_normal_notice_index = 0;
}

