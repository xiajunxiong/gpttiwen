#include "rmiroleclient.h"
#include "servercommon/darmi/darole.h"
#include "servercommon/serverclmempool.h"
#include "gamelog.h"
#include "servercommon/struct/rolequeryparam.hpp"
#include "servercommon/servercommon.h"
#include "engineadapter.h"
#include "servercommon/struct/skilllistparam.hpp"
#include "servercommon/struct/roleinitparam.h"
#include "servercommon/struct/roleotherinitparam.hpp"
#include "servercommon/struct/global/maillistparam.hpp"
#include "servercommon/struct/rolefirstkillparam.hpp"
#include "servercommon/rolecommonsavedef.h"
#include "servercommon/uniqueidgenerator.hpp"

USE_DATAACCESS_VAR();
USE_DATAACCESS_ROLE_VAR();

static char TLV_BUFF[30 * 1024 * 1024];

enum RMI_INIT_ERROR_TYPE
{
	RMI_INIT_ERROR_REQUEST,
	RMI_INIT_ERROR_RESPONSE,
	RMI_INIT_ERROR_EXCEPTION,
	RMI_INIT_ERROR_TIMEOUT,
};

static void RMIInitError(int err, const char *msg, bool stop = true) 
{
	printf("RMI Init Error[type: %d, msg: %s]\n", err, NULL == msg ? "" : msg);

	if (stop)
	{
		EngineAdapter::Instance().StopGame();
	}
}

// 获取Role初始化信息

bool RMIRoleClient::RoleInitAsyn(int uid, rmi::RMIBackObject *backobj, const char * pname)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(uid);
	if (!ret)
	{
		delete backobj;
		return false;
	}
	ret = s.Push(pname);
	if (ret)
	{
		ret = m_session.rmi_module->Call(m_session, dataaccessrmi::ROLE, dataaccessrmi::role::ROLE_INIT, s, backobj, true, 5000);
	}

	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIRoleClient::RoleQueryAsyn(int uid, rmi::RMIBackObject *backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(uid);
	if (ret)
	{
		ret = m_session.rmi_module->Call(m_session, dataaccessrmi::ROLE, dataaccessrmi::role::ROLE_QUERY, s, backobj, false, 5000);
	}

	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIRoleClient::RoleReadDataAsyn(int uid, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(uid);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::ROLE, dataaccessrmi::role::ROLE_READ_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIRoleClient::RoleSaveDataAsyn(int uid, const CommonDataParam & common_param, RMIRoleSaveDataObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(uid);
	if (!ret)
	{
		delete backobj;
		return false;
	}
	TLVSerializer common_data;
	common_data.Reset((void*)&common_param, sizeof(common_param));
	common_data.MoveCurPos(sizeof(common_param));
	ret = s.Push(common_data);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::ROLE, dataaccessrmi::role::ROLE_SAVE_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIRoleInitBackObject::__response(TLVUnserializer &in_param)
{
	int ret;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	static RoleInitParam p;
	static RoleOtherInitParam other_p;
	static ItemListParam item_list_param;
	static SkillListParam skill_list_param;
	static MailListParam mail_list_param;
	static RoleFirstKillParamList first_kill_list;
	static RandActivityRoleParamList rand_activity_role_list;
	static RoleCommonSaveDataParam role_common_save_list;
	static RoleCourageChallengeLevelParamList role_courage_challenge_list;
	static GodPrintItemListParam god_print_item_list;
	static NewCourageChallengeLevelParamList new_courage_challenge_list;

	long long last_save_time = 0;

	if (ret == 0)
	{
		if (!p.Unserialize(in_param)
			|| !other_p.Unserialize(in_param)
			|| !item_list_param.Unserialize(in_param)
			|| !skill_list_param.Unserialize(in_param)
			|| !mail_list_param.Unserialize(in_param)
			|| !first_kill_list.Unserialize(in_param)
			|| !rand_activity_role_list.Unserialize(in_param)
			|| !role_common_save_list.Unserialize(in_param)
			|| !role_courage_challenge_list.Unserialize(in_param)
			|| !god_print_item_list.Unserialize(in_param)
			|| !new_courage_challenge_list.Unserialize(in_param)
			|| !in_param.Pop(&last_save_time)
			)
		{
			return false;
		}
	}

	/*
	for (int i = 0; i < mail_list_param.count; ++i)
	{
		if (mail_list_param.mail_list[i].mail_param.contentparam.item_list[i].item_id == 9003)
		{
			printf("param=");
			for (int k = 0; k < 10; ++k)
			{
				printf("%c", mail_list_param.mail_list[i].mail_param.contentparam.item_list[i].param_data[k]);
			}
			printf("\n");
		}
	}
	*/

	RoleInitRet(ret, p, other_p, item_list_param, skill_list_param,mail_list_param, first_kill_list, rand_activity_role_list, role_common_save_list, 
		role_courage_challenge_list, god_print_item_list, new_courage_challenge_list, last_save_time);

	return true;
}

void RMIRoleInitBackObject::RoleInitRet(int ret, const RoleInitParam &p, 
										const RoleOtherInitParam &other_p,
										const ItemListParam& item_list_param,
										const SkillListParam& skill_list_param,
										const MailListParam& maillist_param,
										const RoleFirstKillParamList& first_kill_list,
										const RandActivityRoleParamList & rand_activity_role_list,
										const RoleCommonSaveDataParam & role_common_save_list,
										const RoleCourageChallengeLevelParamList & role_courage_challenge_list,
										const GodPrintItemListParam & god_print_item_list,
										const NewCourageChallengeLevelParamList & new_courage_challenge_list,
										long long last_save_time)
{
	printf("RoleInitRet:%d\n", ret);
}

void RMIRoleInitBackObject::__free()
{
	delete this;
}

// 保存Role信息
bool RMIRoleClient::RoleSaveAsyn(int uid,
	const RoleInitParam &p,
	const RoleOtherInitParam &other_p,
	const ItemListParam& item_list_param,
	const SkillListParam &skill_list_param,
	const MailListParam& maillist_param,
	const RoleFirstKillParamList& first_kill_list,
	const RandActivityRoleParamList & rand_activity_role_list,
	const RoleCommonSaveDataParam & role_common_save_list,
	const RoleCourageChallengeLevelParamList & role_courage_challenge_list,
	const GodPrintItemListParam & god_print_item_list,
	const NewCourageChallengeLevelParamList & new_courage_challenge_list,
	RMIRoleSaveBackObject *backobj,
	bool is_logout)
{
	// 添加参数时一定要加上相应的静态断言
	UNSTD_STATIC_CHECK((
		sizeof(RoleInitParam) +
		sizeof(RoleOtherInitParam) + 
		sizeof(ItemListParam) + 
		sizeof(SkillListParam) +
		sizeof(MailListParam) + 
		sizeof(RoleFirstKillParamList) +
		sizeof(RandActivityRoleParamList) +
		sizeof(RoleCommonSaveDataParam) + 
		sizeof(RoleCourageChallengeLevelParamList) +
		sizeof(GodPrintItemListParam) +
		sizeof(NewCourageChallengeLevelParamList) +
		sizeof(is_logout)) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(uid);
	if (!ret) 
	{
		delete backobj;
		return false;
	}

	ret = p.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}
	
	ret = other_p.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = item_list_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = skill_list_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = maillist_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = first_kill_list.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = rand_activity_role_list.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = role_common_save_list.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = role_courage_challenge_list.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = god_print_item_list.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = new_courage_challenge_list.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = s.Push(is_logout);
	if (!ret) 
	{
		delete backobj;
		return false;
	}

#ifdef _DEBUG
	long long serial_id = 0;
	UniqueIDGenerator::Instance()->GenUniqueID(&serial_id);
	ret = s.Push(serial_id);
	if (!ret)
	{
		delete backobj;
		return false;
	}
	gamelog::g_log_world.printf(LL_MAINTANCE, "RMIRoleClient::RoleSaveAsyn role[%d,%s] is_logout[%s] serial[%lld] gold[%lld]", 
		p.role_id, p.role_name, is_logout ? "T" : "F", serial_id, p.gold);
#endif

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::ROLE, dataaccessrmi::role::ROLE_SAVE, s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

RMIRoleSaveBackObject::RMIRoleSaveBackObject()
{
}

RMIRoleSaveBackObject::~RMIRoleSaveBackObject()
{
}

void RMIRoleSaveBackObject::SetData(const TLVSerializer &data)
{
}

bool RMIRoleSaveBackObject::__response(TLVUnserializer &int_param)
{
	int ret;
	if (!int_param.Pop(&ret))
	{
		return false;
	}

	RoleSaveRet(ret);

	return true;
}

void RMIRoleSaveBackObject::__free()
{
	delete this;
}

void RMIRoleSaveBackObject::RoleSaveRet(int ret)
{
	printf("RoleSaveRet:%d\n", ret);
}

void RMIRoleSaveBackObject::__exception(int error)
{
	Error(log_user_id, GAME_ERROR_EXCEPT, 0);
}

void RMIRoleSaveBackObject::__timeout()
{
	Error(log_user_id, GAME_ERROR_TIMEOUT, 0);
}

void RMIRoleSaveBackObject::Error(const UserID &user_id, int reason, int ret)
{
	gamelog::g_log_save_error.printf(LL_ERROR, "[RMIRoleSaveBackObject::Error Unserialize_Error][role_id %d][reason:%d][ret:%d]", 
		UidToInt(user_id), reason, ret);
}

// 保存在线状态
bool RMIRoleClient::RoleSaveStatusAsyn(int uid, bool is_logout, bool is_cross, bool is_micro_pc, PlatName pname, rmi::RMIBackObject *backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(uid) && s.Push(is_logout) && s.Push(is_cross) && s.Push(is_micro_pc) && s.Push((const char*)pname);
	if (!ret) 
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::ROLE, dataaccessrmi::role::ROLE_SAVE_STATUS, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIRoleClient::QueryRetrievePetAsyn(int uid, int pet_id, long long item_unique_id, rmi::RMIBackObject *backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(uid) && s.Push(pet_id) && s.Push(item_unique_id);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::ROLE, dataaccessrmi::role::QUERY_RETRIEVE_PET, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIRoleClient::RoleMuteAsyn(int uid, unsigned int forbid_time, int forbid_talk_type, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(uid) && s.Push(forbid_time) && s.Push(forbid_talk_type);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::ROLE, dataaccessrmi::role::ROLE_MUTE, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIRoleClient::RoleCmdToRoleSetAuthorityAsyn(int uid, int authority_type, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(uid) && s.Push(authority_type);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::ROLE, dataaccessrmi::role::ROLE_SET_AUTHORITY, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

RMIRoleSaveStatusBackObject::RMIRoleSaveStatusBackObject()
{
}

RMIRoleSaveStatusBackObject::~RMIRoleSaveStatusBackObject()
{
}

bool RMIRoleSaveStatusBackObject::__response(TLVUnserializer &int_param)
{
	int ret;
	if (!int_param.Pop(&ret))
	{
		return false;
	}

	RoleSaveStatusRet(ret);

	return true;
}

void RMIRoleSaveStatusBackObject::__free()
{
	delete this;
}

void RMIRoleSaveStatusBackObject::RoleSaveStatusRet(int ret)
{
	printf("RoleSaveStatusRet:%d\n", ret);
}

void RMIRoleSaveStatusBackObject::__exception(int error)
{
	Error(log_user_id, GAME_ERROR_EXCEPT, 0);
}

void RMIRoleSaveStatusBackObject::__timeout()
{
	Error(log_user_id, GAME_ERROR_TIMEOUT, 0);
}

void RMIRoleSaveStatusBackObject::Error(const UserID &user_id, int reason, int ret)
{
	gamelog::g_log_save_error.printf(LL_ERROR, "[RMIRoleSaveStatusBackObject::Error Unserialize_Error][role_id %d][reason:%d][ret:%d]", 
		UidToInt(user_id), reason, ret);
}

bool RMIRoleQueryBackObject::__response(TLVUnserializer &in_param)
{
	int ret;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	static RoleQueryParam p;
	static QueryItemListParam item_list_param;
	static GodPrintItemListParam god_print_item_list_param;

	if (ret == 0)
	{
		if (!p.Unserialize(in_param)
			|| !item_list_param.Unserialize(in_param)
			|| !god_print_item_list_param.Unserialize(in_param))
		{
			return false;
		}
	}

	RoleQueryRet(ret, p, item_list_param, god_print_item_list_param);

	return true;
}

void RMIRoleQueryBackObject::__free()
{
	delete this;
}

void RMIRoleQueryBackObject::RoleQueryRet(int ret, const RoleQueryParam &p, const QueryItemListParam &item_list_param, const GodPrintItemListParam &god_print_item_list_param)
{
	printf("RoleQueryRet:%d, uid:%d\n", ret, p.role_id);
}

void RMIRoleQueryBackObject::__timeout()
{
	printf("RMIRoleQueryBackObject::__timeout\n");
}

bool RMIRoleReadDataObject::__response(TLVUnserializer & in_param)
{
	int ret;
	if (!in_param.Pop(&ret))
	{
		return false;
	}
	static RoleInitParam p;
	static RoleOtherInitParam other_p;
	static ItemListParam item_list_param;
	static SkillListParam skill_list_param;
	static MailListParam mail_list_param;
	static RoleFirstKillParamList first_kill_list;
	static RandActivityRoleParamList rand_activity_role_list;
	static RoleCommonSaveDataParam role_common_save_list;
	static RoleCourageChallengeLevelParamList role_courage_challenge_list;
	static GodPrintItemListParam god_print_item_list;
	static NewCourageChallengeLevelParamList new_courage_challenge_list;

	long long last_save_time = 0;

	if (ret == 0)
	{
		if (!p.Unserialize(in_param)
			|| !other_p.Unserialize(in_param)
			|| !item_list_param.Unserialize(in_param)
			|| !skill_list_param.Unserialize(in_param)
			|| !mail_list_param.Unserialize(in_param)
			|| !first_kill_list.Unserialize(in_param)
			|| !rand_activity_role_list.Unserialize(in_param)
			|| !role_common_save_list.Unserialize(in_param)
			|| !role_courage_challenge_list.Unserialize(in_param)
			|| !god_print_item_list.Unserialize(in_param)
			|| !new_courage_challenge_list.Unserialize(in_param)
			|| !in_param.Pop(&last_save_time)
			)
		{
			return false;
		}
	}

	RoleReadDataRet(ret, p, other_p, item_list_param, skill_list_param,  mail_list_param, first_kill_list, rand_activity_role_list, 
		role_common_save_list, role_courage_challenge_list, god_print_item_list, new_courage_challenge_list, last_save_time);

	return true;
}

void RMIRoleReadDataObject::__free()
{
	delete this;
}

void RMIRoleReadDataObject::RoleReadDataRet(int ret, const RoleInitParam & p, 
											const RoleOtherInitParam & other_p, 
											const ItemListParam & item_list_param, 
											const SkillListParam & skill_list_param, 
											const MailListParam & maillist_param, 
											const RoleFirstKillParamList & first_kill_list, 
											const RandActivityRoleParamList & rand_activity_role_list, 
											const RoleCommonSaveDataParam & role_common_save_list, 
											const RoleCourageChallengeLevelParamList & role_courage_challenge_List,
											const GodPrintItemListParam & god_print_item_list,
											const NewCourageChallengeLevelParamList & new_courage_challenge_List,
											long long last_save_time)
{
	printf("%s:%d\n", __FUNCTION__, ret);
}

bool RMIRoleSaveDataObject::__response(TLVUnserializer & in_param)
{
	int ret;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	RoleSaveDataRet(ret);

	return true;
}

void RMIRoleSaveDataObject::__free()
{
	delete this;
}

void RMIRoleSaveDataObject::RoleSaveDataRet(int ret)
{
	printf("%s : %d!", __FUNCTION__, ret);
}

bool RMIQueryRetrievePetBackObject::__response(TLVUnserializer &in_param)
{
	int ret;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	static ItemListParam item_list_param;

	if (ret == 0)
	{
		if (!item_list_param.Unserialize(in_param))
		{
			return false;
		}
	}

	QueryRetrievePetRet(ret, item_list_param);

	return true;
}

void RMIQueryRetrievePetBackObject::__timeout()
{
	gamelog::g_log_world.printf(LL_WARNING, "RMIQueryRetrievePetBackObject::__timeout");
}

void RMIQueryRetrievePetBackObject::__free()
{
	delete this;
}

void RMIQueryRetrievePetBackObject::__exception(int error)
{
	gamelog::g_log_world.printf(LL_WARNING, "RMIQueryRetrievePetBackObject::__exception error[%d]", error);
}

void RMIQueryRetrievePetBackObject::QueryRetrievePetRet(int ret, const ItemListParam &item_list_param)
{
	gamelog::g_log_world.printf(LL_INFO, "RMIQueryRetrievePetBackObject::QueryRetrievePetRet");
}

bool RMIRoleMuteBackObject::__response(TLVUnserializer & in_param)
{
	int ret = 0;
	long long forbid_time = 0;
	int forbid_talk_type = 0;
	if (!in_param.Pop(&ret) || !in_param.Pop(&forbid_time) || !in_param.Pop(&forbid_talk_type))
	{
		return false;
	}

	RoleMuteRet(ret, (unsigned int)forbid_time, forbid_talk_type);

	return true;
}

void RMIRoleMuteBackObject::__timeout()
{
	printf("%s\n", __FUNCTION__);
}

void RMIRoleMuteBackObject::__free()
{
	delete this;
}

void RMIRoleMuteBackObject::RoleMuteRet(int ret, unsigned int forbid_time, int forbid_talk_type)
{
	printf("%s : %d time:%u type:%d!", __FUNCTION__, ret, forbid_time, forbid_talk_type);
}

bool RMIRoleCmdSetAuthorityBackObject::__response(TLVUnserializer & in_param)
{
	int ret = 0, authority_type = 0;

	if (!in_param.Pop(&ret) || !in_param.Pop(&authority_type))
	{
		return false;
	}

	RoleCmdSetAuthorityRet(ret, authority_type);

	return true;
}

void RMIRoleCmdSetAuthorityBackObject::__timeout()
{
	printf("%s\n", __FUNCTION__);
}

void RMIRoleCmdSetAuthorityBackObject::__free()
{
	delete this;
}

void RMIRoleCmdSetAuthorityBackObject::RoleCmdSetAuthorityRet(int ret, int authority_type)
{
	printf("%s : %d type:%d!", __FUNCTION__, ret, authority_type);
}
