#ifndef __NORMAL_PUBLIC_NOTICE_HPP__
#define __NORMAL_PUBLIC_NOTICE_HPP__

#include "servercommon/struct/system/publicnoticelist.h"
#include "servercommon/serverdef.h"

class NormalPublicNotice
{
public:
	NormalPublicNotice();
	~NormalPublicNotice();

	void Init(const UniqueServerID& usid);

	bool NormalNoticeIsEmpty() { return m_normal_notice_count <= 0; }
	void AddNormalNotice(const PublicNoticeList::NoticeAttr &notice_attr);
	void UpdateNormalNotice(unsigned int now_second);
	void ResetNormalNotice();
private:
	UniqueServerID m_usid;

	int m_curr_normal_notice_index;				// 当前的公告索引
	unsigned int m_next_normal_notice_time;		// 下次播放公告时间
	int m_normal_notice_count;					// 正常公告的条数
	PublicNoticeList::NoticeAttr m_normal_noticelist[PublicNoticeList::MAX_PUBLICNOTICE_NUM];	// 正常公告一轮数据
};


#endif