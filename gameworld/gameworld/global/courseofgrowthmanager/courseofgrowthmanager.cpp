#include "courseofgrowthmanager.hpp"
#include "gamelog.h"
#include "common/bintohex.h"
#include "internalcomm.h"

#include "global/usercache/usercache.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "protocol/msgcourseofgrowth.h"

#include "config/logicconfigmanager.hpp"
#include "other/courseofgrowth/courseofgrowthconfig.hpp"

#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"

CourseOfGrowthManager & CourseOfGrowthManager::Instance()
{
	static CourseOfGrowthManager manager;
	return manager;
}

bool CourseOfGrowthManager::LoadAllUser(TMysqlStatement & stMysqlStatement)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return true;
	}
	char szErrMsg[1024];
	int iRet = 0;

	int iErrno;
	stMysqlStatement.iQueryType = SQL_TYPE_SELECT;
	strncpy(stMysqlStatement.szSQLString,
		"SELECT A.`role_id`, A.`role_course_data` FROM `role_attr_detail10` as A WHERE A.`role_id` > 0",
		sizeof(stMysqlStatement.szSQLString) - 1);//query

	iRet = CMysqlUtility::ExecSelect(&stMysqlStatement, &iErrno, szErrMsg, ARRAY_LENGTH(szErrMsg));
	if (0 != iRet)
	{
		CMysqlUtility::FreeResult(&stMysqlStatement);

		gamelog::g_log_world.printf(LL_MAINTANCE, "CourseOfGrowthManager::LoadAllUser select failed with :%d errmsg:%s",
			iErrno, szErrMsg);

		return false;
	}

	int iFetchRows = stMysqlStatement.iResNum;

	enum field
	{
		field_uid = 0,
		field_role_course_data,
	};
	int max_seq = LOGIC_CONFIG->GetCourseOfGrowthConfig()->GetMaxCourseSeq();
	std::set<int> uid_list;
	for (int i = 0; i < iFetchRows; ++i)
	{
		iRet = CMysqlUtility::FetchRow(&stMysqlStatement, szErrMsg);
		if (0 != iRet)
		{
			gamelog::g_log_world.printf(LL_MAINTANCE, "CourseOfGrowthManager::LoadAllUser when fetched data has error :%s,index:%d",
				szErrMsg, i);

			break;
		}
		int role_uid = 0;
		if (stMysqlStatement.stRow[field_uid])
		{
			role_uid = atoi(stMysqlStatement.stRow[field_uid]);
		}
		if (role_uid <= 0 || uid_list.end() != uid_list.find(role_uid))
		{
			continue;
		}
		RoleCourseParam course_param;

		if (stMysqlStatement.stRow[field_role_course_data] && stMysqlStatement.alFieldsLength[field_role_course_data] > 0)
		{
			int app_len = stMysqlStatement.alFieldsLength[field_role_course_data];
			const char* app_str = stMysqlStatement.stRow[field_role_course_data];
			HexToBin(app_str, app_len, (char *)&course_param);
		}
		for (int k = 0; k < max_seq && k < MAX_COURSE_ENTRY_LIMIT_NUM && k < ARRAY_LENGTH(m_course_finish_count_list); ++k)
		{
			if(!course_param.finish_record_flag.IsBitSet(k)) continue;

			m_course_finish_count_list[k]++;
		}
	}

	CMysqlUtility::FreeResult(&stMysqlStatement);

	if (0 != iRet)
	{
		return false;
	}

	gamelog::g_log_world.printf(LL_MAINTANCE, "CourseOfGrowthManager::LoadAllUser total user count:%d",
		(int)uid_list.size());
	return true;
}

void CourseOfGrowthManager::OnRoleLogin(Role * role)
{
	if(NULL == role) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameServerCourseInfoReq hgscir;
		hgscir.role_id = role->GetUID();

		InternalComm::Instance().SendToGameThroughCross2((const char *)&hgscir, sizeof(hgscir), IntToUid(hgscir.role_id).db_index);
		return;
	}
	this->OnSendAllInfo(role);
}

void CourseOfGrowthManager::OnCourseFinishChange(Role * role, int seq, bool is_add)
{
	if(NULL == role || seq < 0 || seq >= ARRAY_LENGTH(m_course_finish_count_list)) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameServerCourseChangeReq hgsccr;
		hgsccr.role_id = role->GetUID();
		hgsccr.seq = seq;
		hgsccr.is_add = is_add ? 1 : 0;

		InternalComm::Instance().SendToGameThroughCross2((const char *)&hgsccr, sizeof(hgsccr), IntToUid(hgsccr.role_id).db_index);
		return;
	}

	if (is_add)
	{
		m_course_finish_count_list[seq] += 1;
	}
	else
	{
		m_course_finish_count_list[seq] -= 1;
		if (m_course_finish_count_list[seq] < 0)
		{
			m_course_finish_count_list[seq] = 0;
		}
	}
	this->OnSendSingleInfo(role, seq);
}

void CourseOfGrowthManager::OnCourseFinishChangeFromHidden(int role_uid, int seq, bool is_add)
{
	if(CrossConfig::Instance().IsHiddenServer() || seq < 0 || seq >= ARRAY_LENGTH(m_course_finish_count_list)) return;

	if (is_add)
	{
		m_course_finish_count_list[seq] += 1;
	}
	else
	{
		m_course_finish_count_list[seq] -= 1;
		if (m_course_finish_count_list[seq] < 0)
		{
			m_course_finish_count_list[seq] = 0;
		}
	}
	this->OnSendSingleInfoToHidden(role_uid, seq);
}

void CourseOfGrowthManager::OnSendAllInfo(Role * role)
{
	if(NULL == role) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{	
		crossgameprotocal::HiddenGameServerCourseInfoReq hgscir;
		hgscir.role_id = role->GetUID();

		InternalComm::Instance().SendToGameThroughCross2((const char *)&hgscir, sizeof(hgscir), IntToUid(hgscir.role_id).db_index);
		return;
	}

	Protocol::SCServerCourseAllInfo info;
	info.server_role_count = UserCacheManager::Instance().CountOfUser();
	info.count = LOGIC_CONFIG->GetCourseOfGrowthConfig()->GetMaxCourseSeq();
	if (info.count > ARRAY_LENGTH(m_course_finish_count_list))
	{
		info.count = ARRAY_LENGTH(m_course_finish_count_list);
	}
	if (info.count > 0)
	{
		memcpy(info.course_finish_count_list, m_course_finish_count_list, sizeof(info.course_finish_count_list[0]) * info.count);
	}
	else
	{
		info.count = 0;
	}

	int length = sizeof(info) - (ARRAY_LENGTH(info.course_finish_count_list) - info.count) * sizeof(info.course_finish_count_list[0]);
	role->GetRoleModuleManager()->NetSend(&info, length);
}

void CourseOfGrowthManager::OnSendSingleInfo(Role * role, int seq)
{
	if(NULL == role || seq < 0 || seq >= ARRAY_LENGTH(m_course_finish_count_list)) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	Protocol::SCServerCourseSingleInfo info;
	info.server_role_count = UserCacheManager::Instance().CountOfUser();
	info.seq = seq;
	info.course_finish_count = m_course_finish_count_list[seq];

	role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
}

void CourseOfGrowthManager::OnSendAllInfoToHidden(int role_uid)
{
	if(CrossConfig::Instance().IsHiddenServer()) return;

	Protocol::SCServerCourseAllInfo info;
	info.server_role_count = UserCacheManager::Instance().CountOfUser();
	info.count = LOGIC_CONFIG->GetCourseOfGrowthConfig()->GetMaxCourseSeq();
	if (info.count > ARRAY_LENGTH(m_course_finish_count_list))
	{
		info.count = ARRAY_LENGTH(m_course_finish_count_list);
	}
	if (info.count > 0)
	{
		memcpy(info.course_finish_count_list, m_course_finish_count_list, sizeof(info.course_finish_count_list[0]) * info.count);
	}
	else
	{
		info.count = 0;
	}

	int length = sizeof(info) - (ARRAY_LENGTH(info.course_finish_count_list) - info.count) * sizeof(info.course_finish_count_list[0]);
	InternalComm::Instance().SendToHiddenUserThroughCross(role_uid, (const char *)&info, length);
}

void CourseOfGrowthManager::OnSendSingleInfoToHidden(int role_uid, int seq)
{
	if(CrossConfig::Instance().IsHiddenServer()) return;
	if(seq < 0 || seq >= ARRAY_LENGTH(m_course_finish_count_list)) return;

	Protocol::SCServerCourseSingleInfo info;
	info.server_role_count = UserCacheManager::Instance().CountOfUser();
	info.seq = seq;
	info.course_finish_count = m_course_finish_count_list[seq];

	InternalComm::Instance().SendToHiddenUserThroughCross(role_uid, (const char *)&info, sizeof(info));
}

CourseOfGrowthManager::CourseOfGrowthManager()
{
	memset(m_course_finish_count_list, 0, sizeof(m_course_finish_count_list));
}

CourseOfGrowthManager::~CourseOfGrowthManager()
{
}
