#include "rmiglobal.hpp"
#include "servercommon/darmi/daglobal.hpp"
#include "servercommon/servercommon.h"
#include "engineadapter.h"
#include "gamelog.h"
#include "rmi/rmibase.h"
#include "servercommon/struct/global/worldarenaparam.hpp"
#include "servercommon/offlinebattedatadef.hpp"

USE_DATAACCESS_VAR();

static char TLV_BUFF[10 * 1024 * 1024];

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

//static void RMISaveError(int err, const char *msg)
//{
//	printf("RMI Save Error[type: %d, msg: %s]\n", err, NULL == msg ? "" : msg);
//}

int RMIGlobalClient::OnRMIBackManagerTry(const RMICallInfo & rmi_call_info, rmi::RMIBackObject * backobj,
										const char* module, const char* method)
{
	if (NULL == module
		|| NULL == method)
	{
		delete backobj;
		return -100;
	}
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	if (0 != s.Size())
	{
		delete backobj;
		return -1;
	}

	if (s.MaxSize() <= rmi_call_info.tlv_buff.size())
	{
		delete backobj;
		return -2;
	}
	if (!rmi_call_info.tlv_buff.empty())
	{
		memcpy(TLV_BUFF, &rmi_call_info.tlv_buff[0], rmi_call_info.tlv_buff.size());
	}

	s.MoveCurPos(rmi_call_info.tlv_buff.size());

	bool ret = false;
	ret = m_session.rmi_module->Call(m_session, module, method, s, backobj, true, 10000);

	if (!ret)
	{
		delete backobj;
		return -3;
	}

	if (backobj != NULL)
	{
		RMIBackManager::Instance().AddCallInfo(rmi_call_info.module, rmi_call_info.method, 
								s, backobj->__get_id(), RMI_CALL_INFO_DEF_OUT_TIMT,
								rmi_call_info.try_times + 1);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool RMIGlobalClient::InitWorldStatusAsyn(long long id_from, rmi::RMIBackObject *backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitWorldStatusAsyn");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_WORLDSTATUS, s, backobj, true, 10000); 
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitWorldStatusAsyn");
		return false;
	}

	ON_RMI_CALL(GLOBAL, INIT_WORLDSTATUS, s, backobj);

	return ret;
}

bool RMIInitWorldStatusBackObject::__response(TLVUnserializer &in_param)
{
	ON_RMI_BACK(
		return false;
	);

	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitWorldStatusAsyn");
		return false;
	}

	static WorldStatusParam worldstatus_param;

	if (ret == 0)
	{
		if (!worldstatus_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitWorldStatusAsyn");
			return false;
		}
	}

	this->InitWorldStatusRet(ret, worldstatus_param);

	return true;
}

void RMIInitWorldStatusBackObject::InitWorldStatusRet(int ret, const WorldStatusParam &worldstatus_param)
{
	printf("RMIInitWorldStatusBackObject::InitWorldStatusRet:%d\n", ret);
}

void RMIInitWorldStatusBackObject::__exception(int error)
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "InitWorldStatusAsyn");
}

void RMIInitWorldStatusBackObject::__timeout()
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, "InitWorldStatusAsyn");
}

void RMIInitWorldStatusBackObject::__free()
{
	delete this;
}

void RMIInitWorldStatusBackObject::InitError()
{
	printf("RMIInitWorldStatusBackObject InitError\n");
}

bool RMIGlobalClient::SaveWorldStatusAsyn(const WorldStatusParam &worldstatus_param, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK(sizeof(WorldStatusParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = worldstatus_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}
	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_WORLDSTATUS, s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMISaveWorldStatusBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveWorldStatusRet(ret);

	return true;
}

void RMISaveWorldStatusBackObject::SaveWorldStatusRet(int ret)
{
	printf("SaveWorldStatusRet:%d\n", ret);
}

void RMISaveWorldStatusBackObject::__free()
{
	delete this;
}

void RMISaveWorldStatusBackObject::__exception(int error)
{
	printf("RMISaveWorldStatusBackObject __exception: %d !!!\n", error);
}

void RMISaveWorldStatusBackObject::__timeout()
{
	printf("RMISaveWorldStatusBackObject __timeout !!!\n");
}

bool RMIGlobalClient::HeartbeatCheckAsyn(rmi::RMIBackObject *backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::HEARTBEAT_CHECK, s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return true;
}

bool RMIGlobalClient::InitOfflineRegisterAsyn(long long id_from, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitOfflineRegisterAsyn");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_OFFLINE_REGISTER, s, backobj, true, 100000);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitOfflineRegisterAsyn");
		return false;
	}

	ON_RMI_CALL(GLOBAL, INIT_OFFLINE_REGISTER, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveOfflineRegisterAsyn(const OfflineRegisterParam & offline_reg_param, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK(sizeof(OfflineRegisterParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = offline_reg_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_OFFLINE_REGISTER, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitBabyAsyn(long long id_from, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitBabyAsyn");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_BABY_DATA, s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitBabyAsyn");
		return false;
	}

	ON_RMI_CALL(GLOBAL, INIT_BABY_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveBabyAsyn(const AllBabyDataParam & all_baby_data_param, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = all_baby_data_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_BABY_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::CreateGuildAsyn(int create_db_index, const GuildBaseParam & guild_base, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK(sizeof(GuildBaseParam) * 2 < sizeof(TLV_BUFF));
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(create_db_index);
	if (!ret)
	{
		delete backobj;
		return false;
	}
	ret = guild_base.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::CREATE_GUILD,
									 s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMICreateGuildBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	GuildID guild_id = 0;

	if (in_param.Pop(&ret) && in_param.Pop(&guild_id))
	{
		this->CreateGuildRet(ret, guild_id);
		return true;
	}

	return false;
}

void RMICreateGuildBackObject::CreateGuildRet(int ret, GuildID guild_id)
{
	printf("CreateGuildRet:%d guild_id:%d\n", ret, guild_id);
}

void RMICreateGuildBackObject::__free()
{
	delete this;
}


bool RMIGlobalClient::DismissGuildAsyn(GuildID guild_id, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK(sizeof(GuildID) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(guild_id);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::DESTROY_GUILD,
									 s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitGuildAsyn(GuildID guild_id_from, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK(sizeof(GuildID) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(guild_id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitGuildAsyn");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_GUILD,
									 s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitGuildAsyn");
		return false;
	}

	ON_RMI_CALL(GLOBAL, INIT_GUILD, s, backobj);

	return ret;
}

bool RMIInitGuildBackObject::__response(TLVUnserializer &in_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitGuildAsyn_0");
		return false;
	}

	static AllGuildParam all_guild_list;
	all_guild_list.count = 0;

	static AllGuildGathersDreamParam guild_dream_list;
	guild_dream_list.count = 0;

	if (ret == 0)
	{
		if (!all_guild_list.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitGuildAsyn_1");
			return false;
		}
		if (!guild_dream_list.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitGuildAsyn_2");
			return false;
		}
	}

	this->InitGuildRet(ret, all_guild_list, guild_dream_list);

	return true;
}

void RMIInitGuildBackObject::InitGuildRet(int ret, const AllGuildParam &all_guild_list, const AllGuildGathersDreamParam &guild_dream_list)
{
	printf("RMIInitGuildBackObject::InitGuildRet:%d\n", ret);
}

void RMIInitGuildBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "InitGuildAsyn");
}

void RMIInitGuildBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, "InitGuildAsyn");
}

void RMIInitGuildBackObject::__free()
{
	delete this;
}

bool RMISaveGuildBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveGuildRet(ret);

	return true;
}

void RMISaveGuildBackObject::SaveGuildRet(int ret)
{
	printf("SaveGuildRet:%d\n", ret);
}

void RMISaveGuildBackObject::__free()
{
	delete this;
}

void RMISaveGuildBackObject::__exception(int error)
{
	printf("SaveGuildRet __exception: %d !!!\n", error);
}

void RMISaveGuildBackObject::__timeout()
{
	printf("SaveGuildRet __timeout !!!\n");
}

bool RMIGlobalClient::SaveGuildAsyn(GuildID guild_id, AllGuildParam & all_guild_param, AllGuildGathersDreamParam &guild_dream_list, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK((sizeof(GuildID) + sizeof(AllGuildParam)) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(guild_id);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = all_guild_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = guild_dream_list.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_GUILD,
									 s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIDismissGuildBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;

	if (in_param.Pop(&ret))
	{
		this->DismissGuildRet(ret);
		return true;
	}

	return false;
}

void RMIDismissGuildBackObject::DismissGuildRet(int ret)
{
	printf("DismissGuildRet: %d\n", ret);
}

void RMIDismissGuildBackObject::__free()
{
	delete this;
}


bool RMIHeartbeatCheckBackObject::__response(TLVUnserializer &in_param)
{
	this->HeartbeatCheckRet();

	return true;
}

void RMIHeartbeatCheckBackObject::__exception(int error)
{
	printf("RMIHeartbeatCheckBackObject __exception: %d !!!\n", error);
}

void RMIHeartbeatCheckBackObject::__timeout()
{
	printf("RMIHeartbeatCheckBackObject __timeout !!!\n");
}

void RMIHeartbeatCheckBackObject::__free()
{
	delete this;
}

void RMIHeartbeatCheckBackObject::HeartbeatCheckRet()
{
	printf("RMIHeartbeatCheckBackObject::HeartbeatCheckRet\n");
}

bool RMIInitOfflineRegisterBackObject::__response(TLVUnserializer &in_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitOfflineRegisterAsyn");
		return false;
	}

	static OfflineRegisterParam offline_reg_param;
	offline_reg_param.count = 0;

	if (ret == 0)
	{
		if (!offline_reg_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitOfflineRegisterAsyn");
			return false;
		}
	}

	this->InitOfflineRegisterRet(ret, offline_reg_param);

	return true;
}

void RMIInitOfflineRegisterBackObject::InitOfflineRegisterRet(int ret, const OfflineRegisterParam &offline_reg_param)
{
	printf("RMIInitEscortBackObject::InitOfflineRegisterRet:%d\n", ret);
}

void RMIInitOfflineRegisterBackObject::__exception(int error)
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "InitOfflineRegisterAsyn");
}

void RMIInitOfflineRegisterBackObject::__timeout()
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, "InitOfflineRegisterAsyn");
}

void RMIInitOfflineRegisterBackObject::__free()
{
	delete this;
}

void RMIInitOfflineRegisterBackObject::InitError()
{
	printf("RMIInitOfflineRegisterBackObject InitError\n");
}

bool RMISaveOfflineRegisterBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveOfflineRegisterRet(ret);

	return true;
}

void RMISaveOfflineRegisterBackObject::SaveOfflineRegisterRet(int ret)
{
	printf("RMISaveOfflineRegisterBackObject:%d\n", ret);
}

void RMISaveOfflineRegisterBackObject::__free()
{
	delete this;
}

void RMISaveOfflineRegisterBackObject::__exception(int error)
{
	printf("RMISaveOfflineRegisterBackObject __exception: %d !!!\n", error);
}

void RMISaveOfflineRegisterBackObject::__timeout()
{
	printf("RMISaveOfflineRegisterBackObject __timeout !!!\n");
}

//-----------------------------------------------------------baby data-----------------------------------------------------
bool RMIInitBabyDataBackObject::__response(TLVUnserializer & in_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, "RMIInitBabyDataBackObject::__response :1");
		return false;
	}

	static AllBabyDataParam all_baby_data_param;
	all_baby_data_param.count = 0;

	if (ret == 0)
	{
		if (!all_baby_data_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, "RMIInitBabyDataBackObject::__response:2");
			return false;
		}
	}

	this->InitBabyDataRet(ret, all_baby_data_param);

	return true;
}

void RMIInitBabyDataBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "RMIInitBabyDataBackObject::__exception");
}

void RMIInitBabyDataBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, "RMIInitBabyDataBackObject::__timeout");
}

void RMIInitBabyDataBackObject::__free()
{
	delete this;
}

void RMIInitBabyDataBackObject::InitBabyDataRet(int ret, const AllBabyDataParam & all_baby_param)
{
	printf("RMIInitBabyDataBackObject::InitBabyDataRet:%d\n", ret);
}

bool RMISaveBabyDataBackObject::__response(TLVUnserializer & in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveBabyDataRet(ret);

	return true;
}

void RMISaveBabyDataBackObject::__exception(int error)
{
	printf("SaveBabyDataRet __exception: %d !!!\n", error);
}

void RMISaveBabyDataBackObject::__timeout()
{
	printf("SaveBabyDataRet __timeout !!!\n");
}

void RMISaveBabyDataBackObject::__free()
{
	delete this;
}

void RMISaveBabyDataBackObject::SaveBabyDataRet(int ret)
{
	printf("SaveBabyDataRet:%d\n", ret);
}

//-----------------------------------------------------------mail data-----------------------------------------------------

bool RMIGlobalClient::InitMailsAsyn(int uid, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK((sizeof(int) + sizeof(int)) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(uid);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	if (ret)
	{
		ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_MAILS, s, backobj, true, 5000);
	}

	if (!ret)
	{
		delete backobj;
	}

	return ret;
}

bool RMIInitMailsBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;

	if (!in_param.Pop(&ret))
	{
		return false;
	}

	static MailListParam mail_param;
	mail_param.count = 0;

	if (0 == ret)
	{
		if (!mail_param.Unserialize(in_param))
		{
			return false;
		}
	}

	this->InitMailsRet(ret, mail_param);

	return true;
}

void RMIInitMailsBackObject::InitMailsRet(int ret, const MailListParam &mail_param)
{
	printf("InitMailsRet:%d\n", ret);
}

void RMIInitMailsBackObject::__free()
{
	delete this;
}

bool RMIGlobalClient::SaveMailsAsyn(int uid, const MailListParam &mail, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK((sizeof(int) + sizeof(int) + sizeof(MailListParam)) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(uid);
	if (!ret)
	{
		delete backobj;
		return false;
	}
	ret = mail.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_MAILS,
									 s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}


bool RMIGlobalClient::GetSystemMailsAsyn(long long id_from, int uid, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK(sizeof(long long) + (sizeof(int) + sizeof(int)) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from) && s.Push(uid);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	if (ret)
	{
		ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::GET_SYSTEM_MAILS,
										 s, backobj, false, 5000);
	}

	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}
bool RMIGlobalClient::RemoveSystemMailsAsyn(int uid, const UserSystemMailListParam &mail_param, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK((sizeof(int) + sizeof(int) + sizeof(UserSystemMailListParam)) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(mail_param.count);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	for (int i = 0; i < mail_param.count && ARRAY_LENGTH(mail_param.mail_list); ++i)
	{
		ret = s.Push(mail_param.mail_list[i].system_mail_id);
		if (!ret)
		{
			delete backobj;
			return false;
		}
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::REMOVE_SYSTEM_MAILS, s, backobj, false, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitActivityDataAsyn(long long id_from, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_ACTIVITY_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ON_RMI_CALL(GLOBAL, INIT_ACTIVITY_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveActivityDataAsyn(const ActivityDataListParam & list_param, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK(sizeof(ActivityDataListParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = list_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_ACTIVITY_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::SaveTradeMarketDataAsync(const TradeMarketData& data, rmi::RMIBackObject* backobj)
{
	UNSTD_STATIC_CHECK(sizeof(TradeMarketData) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = data.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_TRADE_MARKET_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitTradeMarketDataAsync(long long id_from, bool is_system_shelve, rmi::RMIBackObject* backobj)
{
	UNSTD_STATIC_CHECK(sizeof(long long) < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from) && s.Push(is_system_shelve);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_TRADE_MARKET_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ON_RMI_CALL(GLOBAL, INIT_TRADE_MARKET_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveTradeMarketUserRecordAsync(const UserTradeRecordData& data, rmi::RMIBackObject* backobj)
{
	UNSTD_STATIC_CHECK(sizeof(UserTradeRecordData) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = data.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_TRADE_MARKET_USER_RECORD, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitTradeMarketUserRecordAsync(long long id_from, rmi::RMIBackObject* backobj)
{
	UNSTD_STATIC_CHECK(sizeof(long long) < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_TRADE_MARKET_USER_RECORD, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ON_RMI_CALL(GLOBAL, INIT_TRADE_MARKET_USER_RECORD, s, backobj);

	return ret;
}

bool RMIGlobalClient::InitTradeMarketSystemRecordAsync(long long id_from, rmi::RMIBackObject* backobj)
{
	UNSTD_STATIC_CHECK(sizeof(long long) < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_TRADE_MARKET_SYSTEM_RECORD, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_TRADE_MARKET_SYSTEM_RECORD, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveTradeMarketSystemRecordAsync(const SystemTradeRecord& data, rmi::RMIBackObject* backobj)
{
	UNSTD_STATIC_CHECK(sizeof(SystemTradeRecord) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = data.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_TRADE_MARKET_SYSTEM_RECORD, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMISaveMailBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;

	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveMailsRet(ret);

	return true;
}

void RMISaveMailBackObject::SaveMailsRet(int ret)
{
	printf("SaveMailsRet:%d\n", ret);
}

void RMISaveMailBackObject::__free()
{
	delete this;
}


bool RMIGetSystemMailBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	static UserSystemMailListParam mail_param;
	mail_param.count = 0;

	if (0 == ret)
	{
		if (!mail_param.Unserialize(in_param))
		{
			return false;
		}
	}

	this->GetSystemMailsRet(ret, mail_param);

	return true;
}

void RMIGetSystemMailBackObject::GetSystemMailsRet(int ret, const UserSystemMailListParam &mail_param)
{
	printf("RMIGetSystemMailBackObject:%d\n", ret);
}

void RMIGetSystemMailBackObject::__free()
{
	delete this;
}

void RMIGetSystemMailBackObject::__timeout()
{
	printf("RMIGetSystemMailBackObject: __timeout\n");
}


bool RMIRemoveSystemMailBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->RemoveSystemMailsRet(ret);

	return true;
}

void RMIRemoveSystemMailBackObject::RemoveSystemMailsRet(int ret)
{
	printf("RemoveSystemMailsRet:%d\n", ret);
}

void RMIRemoveSystemMailBackObject::__free()
{
	delete this;
}

//-----------------------------RMIInitActivityDataBackObject--------------------------------
bool RMIInitActivityDataBackObject::__response(TLVUnserializer &out_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!out_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, "RMIInitActivityDataBackObject_1");
		return false;
	}

	static ActivityDataListParam list_param;
	list_param.count = 0;
	if (0 == ret)
	{
		if (!list_param.Unserialize(out_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, "RMIInitActivityDataBackObject_2");
			return false;
		}
	}

	this->InitActivityDataRet(ret, list_param);

	return true;
}

void RMIInitActivityDataBackObject::__exception(int error)
{
	printf("RMIInitActivityDataBackObject::__exception error[%d]\n", error);
}

void RMIInitActivityDataBackObject::__timeout()
{
	printf("RMIInitActivityDataBackObject::__timeout\n");
}

void RMIInitActivityDataBackObject::__free()
{
	delete this;
}

void RMIInitActivityDataBackObject::InitActivityDataRet(int ret, const ActivityDataListParam& list_param)
{
	printf("RMIInitActivityDataBackObject::InitActivityDataRet ret[%d]\n", ret);
}

//--------------------RMISaveActivityDataBackObject----------------------------------
bool RMISaveActivityDataBackObject::__response(TLVUnserializer &out_param)
{
	int ret = -1;
	if (!out_param.Pop(&ret))
	{
		return false;
	}

	this->SaveActivityDataRet(ret);

	return true;
}

void RMISaveActivityDataBackObject::__exception(int error)
{
	printf("RMISaveActivityDataBackObject::__exception error[%d]\n", error);
}

void RMISaveActivityDataBackObject::__timeout()
{
	printf("RMISaveActivityDataBackObject::__timeout\n");
}

void RMISaveActivityDataBackObject::__free()
{
	delete this;
}

void RMISaveActivityDataBackObject::SaveActivityDataRet(int ret)
{
	printf("RMISaveActivityDataBackObject::SaveActivityDataRet ret[%d]\n", ret);
}


bool RMIGlobalClient::InitPersonRankAsyn(int _rank_type, rmi::RMIBackObject *backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(_rank_type);
	if (ret)
	{
		ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_PERSON_RANK,
			s, backobj, true, 5000);
	}

	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitPersonRankAsyn");
		return false;
	}

	return ret;
}

bool RMIInitPersonRankBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitPersonRankAsyn");
		return false;
	}

	static PersonRankParam person_rank_param;
	person_rank_param.count = 0;

	if (0 == ret)
	{
		if (!person_rank_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitPersonRankAsyn");
			return false;
		}
	}

	this->InitPersonRankRet(ret, person_rank_param);

	return true;
}

void RMIInitPersonRankBackObject::InitPersonRankRet(int ret, const PersonRankParam &person_rank_param)
{
	printf("InitPersonRankRet:%d\n", ret);
}

void RMIInitPersonRankBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "InitPersonRankAsyn");
}

void RMIInitPersonRankBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, "InitPersonRankAsyn");
}

void RMIInitPersonRankBackObject::__free()
{
	delete this;
}

bool RMIGlobalClient::SavePersonRankAsyn(const PersonRankParam &person_rank_param, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK(sizeof(PersonRankParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = person_rank_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_PERSON_RANK,
									 s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}
bool RMISavePersonRankBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SavePersonRankRet(ret);

	return true;
}

void RMISavePersonRankBackObject::SavePersonRankRet(int ret)
{
	printf("SavePersonRankRet:%d\n", ret);
}

void RMISavePersonRankBackObject::__exception(int error)
{
	printf("RMISavePersonRankBackObject SaveError error[%d]\n", error);
}

void RMISavePersonRankBackObject::__timeout()
{
}

void RMISavePersonRankBackObject::__free()
{
	delete this;
}

bool RMIGlobalClient::InitTeamRankAsyn(rmi::RMIBackObject *backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_TEAM_RANK,
										  s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitTeamRankAsyn");
		return false;
	}

	return ret;
}

bool RMIInitTeamRankBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitTeamRankAsyn");
		return false;
	}

	static TeamRankParam team_rank_param;
	team_rank_param.count = 0;

	if (0 == ret)
	{
		if (!team_rank_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitTeamRankAsyn");
			return false;
		}
	}

	this->InitTeamRankRet(ret, team_rank_param);

	return true;
}

void RMIInitTeamRankBackObject::InitTeamRankRet(int ret, const TeamRankParam &team_rank_param)
{
	printf("InitTeamRankRet:%d\n", ret);
}

void RMIInitTeamRankBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "InitTeamRankAsyn");
}

void RMIInitTeamRankBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, "InitTeamRankAsyn");
}

void RMIInitTeamRankBackObject::__free()
{
	delete this;
}

bool RMIGlobalClient::SaveTeamRankAsyn(const TeamRankParam &team_rank_param, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK(sizeof(TeamRankParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = team_rank_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_TEAM_RANK,
									 s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitPetRankAsyn(int _rank_type , rmi::RMIBackObject *backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(_rank_type);
	if (ret)
	{
		ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_PET_RANK,
			s, backobj, true, 5000);
	}

	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitPetRankAsyn");
		return false;
	}

	return ret;
}

bool RMIInitPetRankBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitPetRankAsyn");
		return false;
	}

	static PetRankParam pet_rank_param;
	pet_rank_param.count = 0;

	if (0 == ret)
	{
		if (!pet_rank_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitPetRankAsyn");
			return false;
		}
	}

	this->InitPetRankRet(ret, pet_rank_param);

	return true;
}

void RMIInitPetRankBackObject::InitPetRankRet(int ret, const PetRankParam &pet_rank_param)
{
	printf("InitPetRankRet:%d\n", ret);
}

void RMIInitPetRankBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "InitPetRankAsyn");
}

void RMIInitPetRankBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, "InitPetRankAsyn");
}

void RMIInitPetRankBackObject::__free()
{
	delete this;
}

bool RMIGlobalClient::SavePetRankAsyn(const PetRankParam &pet_rank_param, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK(sizeof(PetRankParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = pet_rank_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_PET_RANK,
		s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitPartnerRankAsyn(int _rank_type, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(_rank_type);
	if (ret)
	{
		ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_PARTNER_RANK,
			s, backobj, true, 5000);
	}

	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitPartnerRankAsyn");
		return false;
	}

	return ret;
}

bool RMIGlobalClient::SavePartnerRankAsyn(const PartnerRankParam & partner_rank_param, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK(sizeof(PartnerRankParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = partner_rank_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_PARTNER_RANK,
		s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMISavePetRankBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SavePetRankRet(ret);

	return true;
}

void RMISavePetRankBackObject::SavePetRankRet(int ret)
{
	printf("SavePetRankRet:%d\n", ret);
}

void RMISavePetRankBackObject::__exception(int error)
{
	printf("RMISavePetRankBackObject SaveError\n");
}

void RMISavePetRankBackObject::__timeout()
{
}

void RMISavePetRankBackObject::__free()
{
	delete this;
}


bool RMIGlobalClient::InitChallengeUserAsyn(long long next_id_from, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(next_id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitChallengeUserAsyn");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_CHALLENGE_USER, s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitChallengeUserAsyn");
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_CHALLENGE_USER, s, backobj);

	return ret;
}
bool RMIGlobalClient::SaveChallengeUserAsyn(const ChallengeUserListParam & challenge_user_param, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK(sizeof(ChallengeUserListParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = challenge_user_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_CHALLENGE_USER, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}


bool RMIGlobalClient::InitActivityRoleDataAsyn(long long id_from, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitActivityRoleDataAsyn:1");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_ACTIVITY_ROLE_DATA, s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitActivityRoleDataAsyn:2");
		return false;
	}

	ON_RMI_CALL(GLOBAL, INIT_ACTIVITY_ROLE_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveActivityRoleDataAsyn(const ActivityRoleDataParam & activity_role_data_param, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK(sizeof(ActivityRoleDataParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = activity_role_data_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_ACTIVITY_ROLE_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitCommonDataAsyn(long long id_from, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, __FUNCTION__);
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_COMMON_DATA, s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, __FUNCTION__);
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_COMMON_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveCommonDataAsyn(const CommonSaveDataParam & data_param, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK(sizeof(CommonSaveDataParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = data_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_COMMON_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitPlatformBattleAsyn(long long id_from, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitPlatformBattleAsyn:1");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_PLATFORM_BATTLE_DATA, s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitPlatformBattleAsyn:2");
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_PLATFORM_BATTLE_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SavePlatformBattleAsyn(const PlatformBattleParam & data_param, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK(sizeof(PlatformBattleParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = data_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_PLATFORM_BATTLE_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitServerFirstKillAsyn(long long id_from, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitServerFirstKillAsyn:1");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_SERVER_FIRST_KILL_DATA,
		s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitServerFirstKillAsyn:2");
		return false;
	}

	ON_RMI_CALL(GLOBAL, INIT_SERVER_FIRST_KILL_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveServerFirstKillAsyn(const ServerFirstKillParam & server_first_kill_param, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK(sizeof(ServerFirstKillParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = server_first_kill_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_SERVER_FIRST_KILL_DATA, 
		s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMISaveTeamRankBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveTeamRankRet(ret);

	return true;
}

void RMISaveTeamRankBackObject::SaveTeamRankRet(int ret)
{
	printf("SaveTeamRankRet:%d\n", ret);
}

void RMISaveTeamRankBackObject::__exception(int error)
{
	printf("RMISaveTeamRankBackObject SaveError\n");
}

void RMISaveTeamRankBackObject::__timeout()
{
}

void RMISaveTeamRankBackObject::__free()
{
	delete this;
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool RMIInitChallengeUserBackObject::__response(TLVUnserializer &in_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, "RMIInitChallengeUserBackObject_1");
		return false;
	}

	static ChallengeUserListParam challenge_user_param;
	challenge_user_param.count = 0;

	if (ret == 0)
	{
		if (!challenge_user_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, "RMIInitChallengeUserBackObject_2");
			return false;
		}
	}

	this->InitChallengeUserRet(ret, challenge_user_param);

	return true;
}

void RMIInitChallengeUserBackObject::InitChallengeUserRet(int ret, const ChallengeUserListParam &challenge_user_param)
{
	printf("RMIInitChallengeUserBackObject::InitChallengeUserRet:%d\n", ret);
}

void RMIInitChallengeUserBackObject::__exception(int error)
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "RMIInitChallengeUserBackObject::__exception error[%d]", error);
}

void RMIInitChallengeUserBackObject::__timeout()
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, "RMIInitChallengeUserBackObject::__timeout");
}

void RMIInitChallengeUserBackObject::__free()
{
	delete this;
}

void RMIInitChallengeUserBackObject::InitError()
{
	printf("RMIInitChallengeUserBackObject InitError\n");
}

bool RMISaveChallengeUserBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveChallengeUserRet(ret);

	return true;
}

void RMISaveChallengeUserBackObject::SaveChallengeUserRet(int ret)
{
	printf("RMISaveChallengeUserBackObject:%d\n", ret);
}

void RMISaveChallengeUserBackObject::__free()
{
	delete this;
}

void RMISaveChallengeUserBackObject::__exception(int error)
{
	printf("RMISaveChallengeUserBackObject __exception: %d !!!\n", error);
}

void RMISaveChallengeUserBackObject::__timeout()
{
	printf("RMISaveChallengeUserBackObject __timeout !!!\n");
}




bool RMIInitActivityRoleDataBackObject::__response(TLVUnserializer & in_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
		return false;
	}

	ActivityRoleDataParam rank_param;
	rank_param.count = 0;

	if (0 == ret)
	{
		if (!rank_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
			return false;
		}
	}

	this->InitActivityRoleDataRet(ret, rank_param);

	return true;
}

void RMIInitActivityRoleDataBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "InitActivityRoleDataAsyn");
}

void RMIInitActivityRoleDataBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, "InitActivityRoleDataAsyn");
}

void RMIInitActivityRoleDataBackObject::__free()
{
	delete this;
}

void RMIInitActivityRoleDataBackObject::InitActivityRoleDataRet(int ret, const ActivityRoleDataParam & role_data_param)
{
	printf("InitActivityRoleDataRet:%d\n", ret);
}

bool RMISaveActivityRoleDataBackObject::__response(TLVUnserializer & in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveActivityRoleDataRet(ret);

	return true;
}

void RMISaveActivityRoleDataBackObject::__exception(int error)
{
	printf("%s error[%d]\n", __FUNCTION__, error);
}

void RMISaveActivityRoleDataBackObject::__timeout()
{
	printf("%s \n", __FUNCTION__);
}

void RMISaveActivityRoleDataBackObject::__free()
{
	delete this;
}

void RMISaveActivityRoleDataBackObject::SaveActivityRoleDataRet(int ret)
{
	printf("%s ret[%d]\n", __FUNCTION__, ret);
}

bool RMIInitPlatformBattleBackObject::__response(TLVUnserializer & in_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
		return false;
	}

	PlatformBattleParam rank_param;
	rank_param.count = 0;

	if (0 == ret)
	{
		if (!rank_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
			return false;
		}
	}

	this->InitPlatformBattleRet(ret, rank_param);

	return true;
}

void RMIInitPlatformBattleBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "InitPlatformBattleAsyn");
}

void RMIInitPlatformBattleBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, "InitPlatformBattleAsyn");
}

void RMIInitPlatformBattleBackObject::__free()
{
	delete this;
}

void RMIInitPlatformBattleBackObject::InitPlatformBattleRet(int ret, const PlatformBattleParam & param)
{
	printf("InitPlatformBattleRet:%d\n", ret);
}

bool RMISavePlatformBattleBackObject::__response(TLVUnserializer & in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SavePlatformBattleRet(ret);

	return true;
}

void RMISavePlatformBattleBackObject::__exception(int error)
{
	printf("%s error[%d]\n", __FUNCTION__, error);
}

void RMISavePlatformBattleBackObject::__timeout()
{
	printf("%s \n", __FUNCTION__);
}

void RMISavePlatformBattleBackObject::__free()
{
	delete this;
}

void RMISavePlatformBattleBackObject::SavePlatformBattleRet(int ret)
{
	printf("%s ret[%d]\n", __FUNCTION__, ret);
}

bool RMIInitOriginUserCacheBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	if (ret < 0)
	{
		return false;
	}

	long long from_index_to = 0;
	if (!in_param.Pop(&from_index_to))
	{
		return false;
	}

	int count = 0;
	if (!in_param.Pop(&count))
	{
		return false;
	}

	if (count == 0)
	{
		return true;
	}

	TLVUnserializer usercache_node_data;
	if (!in_param.Pop(&usercache_node_data))
	{
		return false;
	}

	static UserCacheParam usercache_node[MAX_INIT_USERCACHE_COUNT];															
	memcpy(&usercache_node, usercache_node_data.Ptr(), usercache_node_data.Size());

	this->InitOriginUserCacheRet(from_index_to, count, usercache_node);

	return true;
}

void RMIInitOriginUserCacheBackObject::__exception(int error)
{
	printf("%s error[%d]\n", __FUNCTION__, error);
}

void RMIInitOriginUserCacheBackObject::__timeout()
{
	printf("%s \n", __FUNCTION__);
}

void RMIInitOriginUserCacheBackObject::__free()
{
	delete this;
}

void RMIInitOriginUserCacheBackObject::InitOriginUserCacheRet(long long from_id, int count, UserCacheParam user_cache_node[MAX_INIT_USERCACHE_COUNT])
{
	printf("%s count[%d]\n", __FUNCTION__, count);
}

bool RMIGlobalClient::InitAnimalRaceAsyn(long long id_from, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitAnimalRaceAsyn:1");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_ANIMAL_RACE_DATA,
		s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitAnimalRaceAsyn:2");
		return false;
	}

	ON_RMI_CALL(GLOBAL, INIT_ANIMAL_RACE_DATA, s, backobj);


	return ret;
}

bool RMIGlobalClient::SaveAnimalRaceAsyn(const AnimalRaceParam & animal_race_param, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK(sizeof(AnimalRaceParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = animal_race_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_ANIMAL_RACE_DATA,
		s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitOriginUserCacheAsyn(long long id_from, rmi::RMIBackObject *backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitOriginUserCacheAsyn");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_ORIGIN_USERCACHE_DATA, s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitWorldStatusAsyn");
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitBattleVideoDataAsyn(long long id_from, rmi::RMIBackObject *backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitBattleVideoDataAsyn_1");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL,
		dataaccessrmi::global::INIT_BATTLE_VIDEO_DATA, s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitBattleVideoDataAsyn_2");
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_BATTLE_VIDEO_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveBattleVideoDataAsyn(const BattleVideoListParam& list_param, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK(sizeof(BattleVideoListParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = list_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, 
		dataaccessrmi::global::SAVE_BATTLE_VIDEO_DATA, s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}



bool RMIGlobalClient::InitShiTuSeekingDataAsyn(long long id_from, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitShiTuSeekingDataAsyn_1");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL,
		dataaccessrmi::global::INIT_SHI_TU_SEEKING_DATA, s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitShiTuSeekingDataAsyn_2");
		return false;
	}

	ON_RMI_CALL(GLOBAL, INIT_SHI_TU_SEEKING_DATA, s, backobj);


	return ret;
}

bool RMIGlobalClient::SaveShiTuSeekingDataAsyn(const ShiTuSeekingParam & list_param, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK(sizeof(ShiTuSeekingParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = list_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL,
		dataaccessrmi::global::SAVE_SHI_TU_SEEKING_DATA, s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitQuanMinDiscountDataAsyn(long long id_from, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitQuanMinDiscountDataAsyn_1");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL,
		dataaccessrmi::global::INIT_QUAN_MIN_DISCOUNT_DATA, s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitQuanMinDiscountDataAsyn_2");
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_QUAN_MIN_DISCOUNT_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveQuanMinDiscountDataAsyn(const QuanMinDiscountParam & list_param, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK(sizeof(QuanMinDiscountParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = list_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL,
		dataaccessrmi::global::SAVE_QUAN_MIN_DISCOUNT_DATA, s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitCourageChallengeRankAsyn(long long id_from, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitCourageChallengeRankAsyn:1");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_COURAGE_CHALLENGE_RANK_DATA,
		s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitCourageChallengeRankAsyn:2");
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_COURAGE_CHALLENGE_RANK_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveCourageChallengeRankAsyn(const CourageChallengeRankParam & courage_challenge_rank_param, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK(sizeof(CourageChallengeRankParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = courage_challenge_rank_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_COURAGE_CHALLENGE_RANK_DATA,
		s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitGatherDreamRecordDataAsyn(long long id_from, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitGatherDreamRecordDataAsyn:1");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_GATHER_DREAM_RECORD_DATA,
		s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitGatherDreamRecordDataAsyn:2");
		return false;
	}

	ON_RMI_CALL(GLOBAL, INIT_GATHER_DREAM_RECORD_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveGatherDreamRecordDataAsyn(const GatherDreamRecordParam & gather_dream_record_param, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK(sizeof(GatherDreamRecordParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = gather_dream_record_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_GATHER_DREAM_RECORD_DATA,
		s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIInitServerFirstKillBackObject::__response(TLVUnserializer & in_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
		return false;
	}

	static ServerFirstKillParam server_first_kill_param;

	if (0 == ret)
	{
		if (!server_first_kill_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
			return false;
		}
	}

	this->InitServerFirstKillRet(ret, server_first_kill_param);

	return true;
}

void RMIInitServerFirstKillBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, __FUNCTION__);
}

void RMIInitServerFirstKillBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, __FUNCTION__);
}

void RMIInitServerFirstKillBackObject::__free()
{
	delete this;
}

void RMIInitServerFirstKillBackObject::InitServerFirstKillRet(int ret, const ServerFirstKillParam & param)
{
	printf("InitServerFirstKillRet:%d\n", ret);
}

bool RMISaveSeverFirstKillBackObject::__response(TLVUnserializer & in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveServerFirstKillRet(ret);

	return true;
}

void RMISaveSeverFirstKillBackObject::__exception(int error)
{
	printf("%s error[%d]\n", __FUNCTION__, error);
}

void RMISaveSeverFirstKillBackObject::__timeout()
{
	printf("%s \n", __FUNCTION__);
}

void RMISaveSeverFirstKillBackObject::__free()
{
	delete this;
}

void RMISaveSeverFirstKillBackObject::SaveServerFirstKillRet(int ret)
{
	printf("%s ret[%d]\n", __FUNCTION__, ret);
}

bool RMISaveChatGroupBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveChatGroupRet(ret);
	return true;
}

void RMISaveChatGroupBackObject::__exception(int error)
{
	printf("%s error[%d]\n", __FUNCTION__, error);
}

void RMISaveChatGroupBackObject::__timeout()
{
	printf("%s \n", __FUNCTION__);
}

void RMISaveChatGroupBackObject::__free()
{
	delete this;
}

void RMISaveChatGroupBackObject::SaveChatGroupRet(int ret)
{
	if (ret < 0)
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
		return;
	}
}

bool RMIInitChatGroupBackObject::__response(TLVUnserializer &in_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	if (ret < 0)
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
		return false;
	}

	static ChatGroupParam param;
	long long last_id = 0;
	bool pop_ret = in_param.Pop(&last_id) &&
		param.Unserialize(in_param);

	this->InitChatGroupRet(last_id, param.count, param.info_list);

	return pop_ret;
}

void RMIInitChatGroupBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, __FUNCTION__);
}

void RMIInitChatGroupBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, __FUNCTION__);
}

void RMIInitChatGroupBackObject::__free()
{
	delete this;
}

void RMIInitChatGroupBackObject::InitChatGroupRet(long long last_id, int count, ChatGroupInfo param_list[ONCE_SEND_CHAT_GROUP_COUNT])
{
	if (count < 0)
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
		return;
	}
}

bool RMISaveCrossChatGroupBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveChatGroupRet(ret);
	return true;
}

void RMISaveCrossChatGroupBackObject::__exception(int error)
{
	printf("%s error[%d]\n", __FUNCTION__, error);
}

void RMISaveCrossChatGroupBackObject::__timeout()
{
	printf("%s \n", __FUNCTION__);
}

void RMISaveCrossChatGroupBackObject::__free()
{
	delete this;
}

void RMISaveCrossChatGroupBackObject::SaveChatGroupRet(int ret)
{
	if (ret < 0)
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
		return;
	}
}

bool RMIInitCrossChatGroupBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	if (ret < 0)
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
		return false;
	}

	static ChatGroupParam param;
	long long last_id = 0;
	bool pop_ret = in_param.Pop(&last_id) &&
		param.Unserialize(in_param);

	this->InitChatGroupRet(last_id, param.count, param.info_list);

	return pop_ret;
}

void RMIInitCrossChatGroupBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, __FUNCTION__);
}

void RMIInitCrossChatGroupBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, __FUNCTION__);
}

void RMIInitCrossChatGroupBackObject::__free()
{
	delete this;
}

void RMIInitCrossChatGroupBackObject::InitChatGroupRet(long long last_id, int count, ChatGroupInfo param_list[ONCE_SEND_CHAT_GROUP_COUNT])
{
	if (count < 0)
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
		return;
	}
}

bool RMIGlobalClient::InitChatGroupAsyn(long long id_from, rmi::RMIBackObject *backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitChatGroupAsyn");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_CHAT_GROUP_DATA, s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitChatGroupAsyn");
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_CHAT_GROUP_DATA, s, backobj);


	return ret;
}

bool RMIGlobalClient::SaveChatGroupAsyn(const ChatGroupParam & chat_group_param, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK(sizeof(ChatGroupParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = chat_group_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_CHAT_GROUP_DATA,
		s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitCrossChatGroupAsyn(long long id_from, rmi::RMIBackObject *backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitChatGroupAsyn");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_CROSS_CHAT_GROUP_DATA, s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitChatGroupAsyn");
		return false;
	}

	return ret;
}

bool RMIGlobalClient::SaveCrossChatGroupAsyn(const ChatGroupParam & chat_group_param, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK(sizeof(ChatGroupParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = chat_group_param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_CROSS_CHAT_GROUP_DATA,
		s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitTradeMarketSystemRecycleRuleAsync(long long id_from, rmi::RMIBackObject* backobj)
{
	UNSTD_STATIC_CHECK(sizeof(long long) < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_TRADE_MARKET_SYSTEM_RECYCLE_RULE, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_TRADE_MARKET_SYSTEM_RECYCLE_RULE, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveTradeMarketSystemRecycleRuleAsync(const SystemRecycleRuleList& data, rmi::RMIBackObject* backobj)
{
	UNSTD_STATIC_CHECK(sizeof(SystemRecycleRuleList) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = data.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_TRADE_MARKET_SYSTEM_RECYCLE_RULE, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitSystemAutoUpshelveDataAsync(long long id_from, rmi::RMIBackObject* backobj)
{
	UNSTD_STATIC_CHECK(sizeof(long long) < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_SYSTEM_AUTO_UPSHELVE_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_SYSTEM_AUTO_UPSHELVE_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveSystemAutoUpshelveDataAsync(const AutoUpshelveDataList& data_list, rmi::RMIBackObject* backobj)
{
	UNSTD_STATIC_CHECK(sizeof(AutoUpshelveDataList) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = data_list.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_SYSTEM_AUTO_UPSHELVE_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitCloudArenaDataAsyn(rmi::RMIBackObject* backobj)
{
	UNSTD_STATIC_CHECK(sizeof(long long) < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_CLOUD_ARENA_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_CLOUD_ARENA_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveCloudArenaDataAsyn(const CloudArenaParam& param, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK(sizeof(CloudArenaParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_CLOUD_ARENA_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitCloudArenaUserDataAsyn(long long id_from, rmi::RMIBackObject* backobj)
{
	UNSTD_STATIC_CHECK(sizeof(long long) < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	if (!s.Push(id_from))
	{
		return false;
	}

	bool ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_CLOUD_ARENA_USER_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_CLOUD_ARENA_USER_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveCloudArenaUserDataAsyn(const CloudArenaUserListParam& param, rmi::RMIBackObject* backobj)
{
	UNSTD_STATIC_CHECK(sizeof(CloudArenaUserListParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_CLOUD_ARENA_USER_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIInitBattleVideoDataBackObject::__response(TLVUnserializer &out_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!out_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
		return false;
	}

	static BattleVideoListParam list_param;
	list_param.count = 0;

	if (0 == ret)
	{
		if (!list_param.Unserialize(out_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
			return false;
		}
	}

	this->InitBattleVideoDataRet(ret, list_param);

	return true;
}

void RMIInitBattleVideoDataBackObject::__exception(int error)
{
	char error_msg[100] = { 0 };
	SNPRINTF(error_msg, sizeof(error_msg), "%s, error[%d]", __FUNCTION__, error);
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, error_msg);
}

void RMIInitBattleVideoDataBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, __FUNCTION__);
}

void RMIInitBattleVideoDataBackObject::__free()
{
	delete this;
}

void RMIInitBattleVideoDataBackObject::InitBattleVideoDataRet(int ret, const BattleVideoListParam& list_param)
{
	printf("InitBattleVideoDataRet ret[%d]\n", ret);
}

bool RMISaveBattleVideoDataBackObject::__response(TLVUnserializer &out_param)
{
	int ret = -1;
	if (!out_param.Pop(&ret))
	{
		return false;
	}

	this->SaveBattleVideoDataRet(ret);

	return true;
}

void RMISaveBattleVideoDataBackObject::__exception(int error)
{
	printf("RMISaveBattleVideoDataBackObject::__exception error[%d]\n", error);
}

void RMISaveBattleVideoDataBackObject::__timeout()
{
	printf("RMISaveBattleVideoDataBackObject::__timeout\n");
}

void RMISaveBattleVideoDataBackObject::__free()
{
	delete this;
}

void RMISaveBattleVideoDataBackObject::SaveBattleVideoDataRet(int ret)
{
	printf("RMISaveBattleVideoDataBackObject::SaveBattleVideoDataRet ret[%d]\n", ret);
}

bool RMIInitAnimalRaceBackObject::__response(TLVUnserializer & in_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
		return false;
	}

	static AnimalRaceParam animal_race_param;

	if (0 == ret)
	{
		if (!animal_race_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
			return false;
		}
	}

	this->InitAnimalRaceRet(ret, animal_race_param);

	return true;
}

void RMIInitAnimalRaceBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, __FUNCTION__);
}

void RMIInitAnimalRaceBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, __FUNCTION__);
}

void RMIInitAnimalRaceBackObject::__free()
{
	delete this;
}

void RMIInitAnimalRaceBackObject::InitAnimalRaceRet(int ret, const AnimalRaceParam & param)
{
	printf("InitAnimalRaceRet:%d\n", ret);
}

bool RMISaveAnimalRaceBackObject::__response(TLVUnserializer & in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveAnimalRaceRet(ret);

	return true;
}

void RMISaveAnimalRaceBackObject::__exception(int error)
{
	printf("%s error[%d]\n", __FUNCTION__, error);
}

void RMISaveAnimalRaceBackObject::__timeout()
{
	printf("%s \n", __FUNCTION__);
}

void RMISaveAnimalRaceBackObject::__free()
{
	delete this;
}

void RMISaveAnimalRaceBackObject::SaveAnimalRaceRet(int ret)
{
	printf("%s ret[%d]\n", __FUNCTION__, ret);
}


bool RMISaveShiTuSeekingDataBackObject::__response(TLVUnserializer & out_param)
{
	int ret = -1;
	if (!out_param.Pop(&ret))
	{
		return false;
	}

	this->SaveShiTuSeekingDataRet(ret);

	return true;;
}

void RMISaveShiTuSeekingDataBackObject::__exception(int error)
{
	printf("RMISaveShiTuSeekingDataBackObject::__exception error[%d]\n", error);
}

void RMISaveShiTuSeekingDataBackObject::__timeout()
{
	printf("RMISaveShiTuSeekingDataBackObject::__timeout\n");
}

void RMISaveShiTuSeekingDataBackObject::__free()
{
	delete this;
}

void RMISaveShiTuSeekingDataBackObject::SaveShiTuSeekingDataRet(int ret)
{
	printf("RMISaveShiTuSeekingDataBackObject::SaveShiTuSeekingDataRet ret[%d]\n", ret);
}

bool RMIInitShiTuSeekingDataBackObject::__response(TLVUnserializer & out_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!out_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
		return false;
	}

	static ShiTuSeekingParam list_param;

	if (0 == ret)
	{
		if (!list_param.Unserialize(out_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
			return false;
		}
	}

	this->InitShiTuSeekingDataRet(ret, list_param);
	
	return true;
}

void RMIInitShiTuSeekingDataBackObject::__exception(int error)
{
	char error_msg[100] = { 0 };
	SNPRINTF(error_msg, sizeof(error_msg), "%s, error[%d]", __FUNCTION__, error);
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, error_msg);
}

void RMIInitShiTuSeekingDataBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, __FUNCTION__);
}

void RMIInitShiTuSeekingDataBackObject::__free()
{
	delete this;
}

void RMIInitShiTuSeekingDataBackObject::InitShiTuSeekingDataRet(int ret, const ShiTuSeekingParam & list_param)
{
	printf("InitShiTuSeekingDataRet ret[%d]\n", ret);
}

bool RMIInitQuanMinDiscountDataBackObject::__response(TLVUnserializer & out_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!out_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitQuanMinDiscountDataAsyn");
		return false;
	}

	static QuanMinDiscountParam qm_discount_param;
	qm_discount_param.count = 0;

	if (ret == 0)
	{
		if (!qm_discount_param.Unserialize(out_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitQuanMinDiscountDataAsyn");
			return false;
		}
	}

	this->InitQuanMinDiscountDataRet(ret, qm_discount_param);

	return true;
}

void RMIInitQuanMinDiscountDataBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "InitQuanMinDiscountDataAsyn");
}

void RMIInitQuanMinDiscountDataBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, __FUNCTION__);
}

void RMIInitQuanMinDiscountDataBackObject::__free()
{
	delete this;
}

void RMIInitQuanMinDiscountDataBackObject::InitQuanMinDiscountDataRet(int ret, const QuanMinDiscountParam & list_param)
{
	printf("InitQuanMinDiscountDataRet ret[%d]\n", ret);
}

bool RMISaveQuanMinDiscountDataBackObject::__response(TLVUnserializer & out_param)
{
	int ret = -1;
	if (!out_param.Pop(&ret))
	{
		return false;
	}

	this->SaveQuanMinDiscountDataRet(ret);

	return true;
}

void RMISaveQuanMinDiscountDataBackObject::__exception(int error)
{
	printf("RMISaveQuanMinDiscountDataBackObject::__exception error[%d]\n", error);
}

void RMISaveQuanMinDiscountDataBackObject::__timeout()
{
	printf("RMISaveQuanMinDiscountDataBackObject::__timeout\n");
}

void RMISaveQuanMinDiscountDataBackObject::__free()
{
	delete this;
}

void RMISaveQuanMinDiscountDataBackObject::SaveQuanMinDiscountDataRet(int ret)
{
	printf("RMISaveQuanMinDiscountDataBackObject::SaveQuanMinDiscountDataRet:[%d]\n", ret);
}

bool RMIInitCourageChallengeRankDataBackObject::__response(TLVUnserializer & in_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
		return false;
	}

	static CourageChallengeRankParam courage_challenge_rank_param;

	if (0 == ret)
	{
		if (!courage_challenge_rank_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
			return false;
		}
	}

	this->InitCourageChallengeRankDataRet(ret, courage_challenge_rank_param);

	return true;
}

void RMIInitCourageChallengeRankDataBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, __FUNCTION__);
}

void RMIInitCourageChallengeRankDataBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, __FUNCTION__);
}

void RMIInitCourageChallengeRankDataBackObject::__free()
{
	delete this;
}

void RMIInitCourageChallengeRankDataBackObject::InitCourageChallengeRankDataRet(int ret, const CourageChallengeRankParam & list_param)
{
	printf("%s:%d\n", __FUNCTION__, ret);
}

bool RMISaveCourageChallengeRankDataBackObject::__response(TLVUnserializer & in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveCourageChallengeRankDataRet(ret);

	return true;
}

void RMISaveCourageChallengeRankDataBackObject::__exception(int error)
{
	printf("%s error[%d]\n", __FUNCTION__, error);
}

void RMISaveCourageChallengeRankDataBackObject::__timeout()
{
	printf("%s \n", __FUNCTION__);
}

void RMISaveCourageChallengeRankDataBackObject::__free()
{
	delete this;
}

void RMISaveCourageChallengeRankDataBackObject::SaveCourageChallengeRankDataRet(int ret)
{
	printf("%s:%d\n", __FUNCTION__, ret);
}

bool RMIInitCommonDataBackObject::__response(TLVUnserializer & in_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
		return false;
	}

	CommonSaveDataParam param;
	param.count = 0;

	if (0 == ret)
	{
		if (!param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
			return false;
		}
	}

	this->InitDataRet(ret, param);

	return true;
}

void RMIInitCommonDataBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "InitCommonDataAsyn");
}

void RMIInitCommonDataBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, "InitCommonDataAsyn");
}

void RMIInitCommonDataBackObject::__free()
{
	delete this;
}

void RMIInitCommonDataBackObject::InitDataRet(int ret, const CommonSaveDataParam & data_param)
{
	printf("InitCommonDataAsyn:%d\n", ret);
}

bool RMISaveCommonDataBackObject::__response(TLVUnserializer & in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveDataRet(ret);

	return true;
}

void RMISaveCommonDataBackObject::__exception(int error)
{
	printf("%s error[%d]\n", __FUNCTION__, error);
}

void RMISaveCommonDataBackObject::__timeout()
{
	printf("%s \n", __FUNCTION__);
}

void RMISaveCommonDataBackObject::__free()
{
	delete this;
}

void RMISaveCommonDataBackObject::SaveDataRet(int ret)
{
	printf("%s ret[%d]\n", __FUNCTION__, ret);
}

bool RMISaveGatherDreamRecordDataBackObject::__response(TLVUnserializer & in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveGatherDreamRecordDataRet(ret);

	return true;
}

void RMISaveGatherDreamRecordDataBackObject::__exception(int error)
{
	printf("%s error[%d]\n", __FUNCTION__, error);
}

void RMISaveGatherDreamRecordDataBackObject::__timeout()
{
	printf("%s \n", __FUNCTION__);
}

void RMISaveGatherDreamRecordDataBackObject::__free()
{
	delete this;
}

void RMISaveGatherDreamRecordDataBackObject::SaveGatherDreamRecordDataRet(int ret)
{
	printf("%s:%d\n", __FUNCTION__, ret);
}

bool RMIInitGatherDreamRecordDataBackObject::__response(TLVUnserializer & in_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
		return false;
	}

	static GatherDreamRecordParam gather_dream_record_param;

	if (0 == ret)
	{
		if (!gather_dream_record_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
			return false;
		}
	}

	this->InitGatherDreamRecordDataRet(ret, gather_dream_record_param);

	return true;
}

void RMIInitGatherDreamRecordDataBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, __FUNCTION__);
}

void RMIInitGatherDreamRecordDataBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, __FUNCTION__);
}

void RMIInitGatherDreamRecordDataBackObject::__free()
{
	delete this;
}

void RMIInitGatherDreamRecordDataBackObject::InitGatherDreamRecordDataRet(int ret, const GatherDreamRecordParam & list_param)
{
	printf("%s:%d\n", __FUNCTION__, ret);
}

bool RMISaveTradeMarketDataBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}
	this->SaveRet(ret);
	return true;
}

void RMISaveTradeMarketDataBackObject::__exception(int error)
{
	printf("RMISaveTradeMarketDataBackObject::__exception error[%d]\n", error);
}

void RMISaveTradeMarketDataBackObject::__timeout()
{
	printf("RMISaveTradeMarketDataBackObject::__timeout\n");
}

void RMISaveTradeMarketDataBackObject::__free()
{
	delete this;
}

void RMISaveTradeMarketDataBackObject::SaveRet(int ret)
{
	printf("RMISaveTradeMarketDataBackObject::SaveRet ret[%d]\n", ret);
}

bool RMIInitTradeMarketDataBackObject::__response(TLVUnserializer &in_param)
{
	ON_RMI_BACK(return false;);

	int result = -1;
	if (!in_param.Pop(&result))
	{
		return false;
	}

	static TradeMarketData data;
	data.count = 0;

	if (0 == result)
	{
		if (!data.UnSerialize(in_param))
		{
			return false;
		}
	}

	this->InitRet(result, data);
	return true;
}

void RMIInitTradeMarketDataBackObject::__exception(int error)
{
	printf("RMIInitTradeMarketDataBackObject::__exception error[%d]\n", error);
}

void RMIInitTradeMarketDataBackObject::__timeout()
{
	printf("RMIInitTradeMarketDataBackObject::__timeout\n");
}

void RMIInitTradeMarketDataBackObject::__free()
{
	delete this;
}

void RMIInitTradeMarketDataBackObject::InitRet(int result, const TradeMarketData& data)
{
	printf("RMIInitTradeMarketDataBackObject::InitRet result[%d]\n", result);
}

bool RMISaveUserTradeRecordBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}
	this->SaveRet(ret);
	return true;
}

void RMISaveUserTradeRecordBackObject::__exception(int error)
{
	printf("RMISaveTradeMarketUserRecordBackObject::__exception error[%d]", error);
}

void RMISaveUserTradeRecordBackObject::__timeout()
{
	printf("RMISaveTradeMarketUserRecordBackObject::__timeout\n");
}

void RMISaveUserTradeRecordBackObject::__free()
{
	delete this;
}

void RMISaveUserTradeRecordBackObject::SaveRet(int ret)
{
	printf("RMISaveTradeMarketUserRecordBackObject::SaveRet ret[%d]\n", ret);
}

bool RMIInitUserTradeRecordBackObject::__response(TLVUnserializer &in_param)
{
	ON_RMI_BACK(return false;);

	int result = -1;
	if (!in_param.Pop(&result))
	{
		return false;
	}

	static UserTradeRecordData data;
	data.count = 0;

	if (0 == result)
	{
		if (!data.UnSerialize(in_param))
		{
			return false;
		}
	}

	this->InitRet(result, data);
	return true;
}

void RMIInitUserTradeRecordBackObject::__exception(int error)
{
	printf("RMIInitUserTradeRecordBackObject::__exception error[%d]\n", error);
}

void RMIInitUserTradeRecordBackObject::__timeout()
{
	printf("RMIInitUserTradeRecordBackObject::__timeout\n");
}

void RMIInitUserTradeRecordBackObject::__free()
{
	delete this;
}

void RMIInitUserTradeRecordBackObject::InitRet(int ret, const UserTradeRecordData& data)
{
	printf("RMIInitUserTradeRecordBackObject::InitRet ret[%d] count[%d]\n", ret, data.count);
}

bool RMISaveSystemTradeRecordBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}
	this->SaveRet(ret);
	return true;
}

void RMISaveSystemTradeRecordBackObject::__exception(int error)
{
	printf("RMISaveSystemTradeRecordBackObject::__exception error[%d]\n", error);
}

void RMISaveSystemTradeRecordBackObject::__timeout()
{
	printf("RMISaveSystemTradeRecordBackObject::__timeout\n");
}

void RMISaveSystemTradeRecordBackObject::__free()
{
	delete this;
}

void RMISaveSystemTradeRecordBackObject::SaveRet(int ret)
{
	printf("RMISaveSystemTradeRecordBackObject::SaveRet ret[%d]\n", ret);
}

bool RMIInitSystemTradeRecordBackObject::__response(TLVUnserializer &in_param)
{
	ON_RMI_BACK(return false;);

	int result = -1;
	if (!in_param.Pop(&result))
	{
		return false;
	}

	static SystemTradeRecord data;
	data.count = 0;

	if (0 == result)
	{
		if (!data.UnSerialize(in_param))
		{
			return false;
		}
	}

	this->InitRet(result, data);
	return true;
}

void RMIInitSystemTradeRecordBackObject::__exception(int error)
{
	printf("RMIInitSystemTradeRecordBackObject::__exception error[%d]\n", error);
}

void RMIInitSystemTradeRecordBackObject::__timeout()
{
	printf("RMIInitSystemTradeRecordBackObject::__timeout\n");
}

void RMIInitSystemTradeRecordBackObject::__free()
{
	delete this;
}

void RMIInitSystemTradeRecordBackObject::InitRet(int ret, const SystemTradeRecord& data)
{
	printf("RMIInitSystemTradeRecordBackObject::InitRet ret[%d] count[%d]\n", ret, data.count);
}
bool RMISaveSystemRecycleRuleBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}
	this->SaveRet(ret);
	return true;
}

void RMISaveSystemRecycleRuleBackObject::__exception(int error)
{
	printf("RMISaveSystemRecycleRuleBackObject exception[%d]\n", error);
}

void RMISaveSystemRecycleRuleBackObject::__timeout()
{
	printf("RMISaveSystemRecycleRuleBackObject timeout\n");
}

void RMISaveSystemRecycleRuleBackObject::__free()
{
	delete this;
}

void RMISaveSystemRecycleRuleBackObject::SaveRet(int ret)
{
	printf("RMISaveSystemRecycleRuleBackObject::SaveRet ret[%d]\n", ret);
}

bool RMIInitSystemRecycleRuleBackObject::__response(TLVUnserializer &in_param)
{
	ON_RMI_BACK(return false;);

	int result = -1;
	if (!in_param.Pop(&result))
	{
		return false;
	}

	static SystemRecycleRuleList data;
	data.count = 0;

	if (0 == result)
	{
		if (!data.UnSerialize(in_param))
		{
			return false;
		}
	}

	this->InitRet(result, data);
	return true;
}

void RMIInitSystemRecycleRuleBackObject::__exception(int error)
{
	printf("RMIInitSystemRecycleRuleBackObject exception[%d]\n", error);
}

void RMIInitSystemRecycleRuleBackObject::__timeout()
{
	printf("RMIInitSystemRecycleRuleBackObject timeout\n");
}

void RMIInitSystemRecycleRuleBackObject::__free()
{
	delete this;
}

void RMIInitSystemRecycleRuleBackObject::InitRet(int ret, const SystemRecycleRuleList& list)
{
	printf("RMIInitSystemRecycleRuleBackObject::InitRet ret[%d]\n", ret);
}

bool RMISaveSystemAutoUpshelveBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}
	this->SaveRet(ret);
	return true;
}

void RMISaveSystemAutoUpshelveBackObject::__exception(int error)
{
	printf("RMISaveSystemAutoUpshelveBackObject::__exception error[%d]\n", error);
}

void RMISaveSystemAutoUpshelveBackObject::__timeout()
{
	printf("RMISaveSystemAutoUpshelveBackObject::__timeout\n");
}

void RMISaveSystemAutoUpshelveBackObject::__free()
{
	delete this;
}

void RMISaveSystemAutoUpshelveBackObject::SaveRet(int ret)
{
	printf("RMISaveSystemAutoUpshelveBackObject::SaveRet ret[%d]\n", ret);
}

bool RMIInitSystemAutoUpshelveBackObject::__response(TLVUnserializer &in_param)
{
	ON_RMI_BACK(return false;);

	int result = -1;
	if (!in_param.Pop(&result))
	{
		return false;
	}

	static AutoUpshelveDataList data;
	data.count = 0;

	if (0 == result)
	{
		if (!data.UnSerialize(in_param))
		{
			return false;
		}
	}

	this->InitRet(result, data);
	return true;
}

void RMIInitSystemAutoUpshelveBackObject::__exception(int error)
{
	printf("RMIInitSystemAutoUpshelveBackObject::__exception error[%d]\n", error);
}

void RMIInitSystemAutoUpshelveBackObject::__timeout()
{
	printf("RMIInitSystemAutoUpshelveBackObject::__timeout\n");
}

void RMIInitSystemAutoUpshelveBackObject::__free()
{
	delete this;
}

void RMIInitSystemAutoUpshelveBackObject::InitRet(int ret, const AutoUpshelveDataList& list)
{
	printf("RMIInitSystemAutoUpshelveBackObject::InitRet ret[%d]\n", ret);
}

bool RMIInitCloudArenaDataBackObject::__response(TLVUnserializer &in_param)
{
	ON_RMI_BACK(return false;);

	int result = -1;
	if (!in_param.Pop(&result))
	{
		return false;
	}

	static CloudArenaParam data;
	if (0 == result)
	{
		if (!data.UnSerialize(in_param))
		{
			return false;
		}
	}
	
	this->InitRet(result, data);
	
	return true;
}

void RMIInitCloudArenaDataBackObject::__exception(int error)
{
	printf("RMIInitCloudArenaDataBackObject::__exception error[%d]\n", error);
}

void RMIInitCloudArenaDataBackObject::__timeout()
{
	printf("RMIInitCloudArenaDataBackObject::__timeout\n");
}

void RMIInitCloudArenaDataBackObject::__free()
{
	delete this;
}

void RMIInitCloudArenaDataBackObject::InitRet(int result, const CloudArenaParam& data)
{
	printf("RMIInitCloudArenaDataBackObject::InitRet ret[%d]\n", result);
}

bool RMISaveCloudArenaDataBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveRet(ret);

	return true;
}

void RMISaveCloudArenaDataBackObject::__exception(int error)
{
	printf("RMISaveCloudArenaDataBackObject::__exception error[%d]\n", error);
}

void RMISaveCloudArenaDataBackObject::__timeout()
{
	printf("RMISaveCloudArenaDataBackObject::__timeout\n");
}

void RMISaveCloudArenaDataBackObject::__free()
{
	delete this;
}

void RMISaveCloudArenaDataBackObject::SaveRet(int ret)
{
	printf("RMISaveCloudArenaDataBackObject::SaveRet ret[%d]", ret);
}

bool RMIInitCloudArenaUserDataBackObject::__response(TLVUnserializer &in_param)
{
	ON_RMI_BACK(return false;);

	int result = -1;
	if (!in_param.Pop(&result))
	{
		return false;
	}

	static CloudArenaUserListParam data;
	if (0 == result)
	{
		if (!data.Unserialize(in_param))
		{
			return false;
		}
	}

	this->InitRet(result, data);

	return true;
}

void RMIInitCloudArenaUserDataBackObject::__exception(int error)
{
	printf("RMIInitCloudArenaUserDataBackObject::__exception error[%d]\n", error);
}

void RMIInitCloudArenaUserDataBackObject::__timeout()
{
	printf("RMIInitCloudArenaUserDataBackObject::__timeout\n");
}

void RMIInitCloudArenaUserDataBackObject::__free()
{
	delete this;
}

void RMIInitCloudArenaUserDataBackObject::InitRet(int result, const CloudArenaUserListParam& data)
{
	printf("RMIInitCloudArenaUserDataBackObject::InitRet result[%d]\n", result);
}

bool RMISaveCloudArenaUserDataBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveRet(ret);

	return true;
}

void RMISaveCloudArenaUserDataBackObject::__exception(int error)
{
	printf("RMISaveCloudArenaUserDataBackObject::__exception error:%d\n", error);
}

void RMISaveCloudArenaUserDataBackObject::__timeout()
{
	printf("RMISaveCloudArenaUserDataBackObject::__timeout\n");
}

void RMISaveCloudArenaUserDataBackObject::__free()
{
	delete this;
}

void RMISaveCloudArenaUserDataBackObject::SaveRet(int ret)
{
	printf("RMISaveCloudArenaUserDataBackObject::SaveRet ret:%d\n", ret);
}

/////////////////////////////////////////////////////////////////////////////////////////
// ºì°ü
bool RMIGlobalClient::InitHongBaoDataAsyn(int _hong_bao_id_from, int _hong_bao_sys_type , rmi::RMIBackObject* backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(_hong_bao_id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitHongBaoDataAsyn");
		return false;
	}

	ret = s.Push(_hong_bao_sys_type);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitHongBaoDataAsyn");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_HONG_BAO, s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitHongBaoDataAsyn");
		return false;
	}

	return ret;
}

bool RMIInitHongBaoBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitHongBaoDataAsyn");
		return false;
	}

	int hongbao_sys_type = -1;
	if (!in_param.Pop(&hongbao_sys_type))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitHongBaoDataAsyn");
		return false;
	}

	static HongBaoParam hongbao_param;

	if (ret == 0)
	{
		if (!hongbao_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitHongBaoDataAsyn");
			return false;
		}
	}

	this->InitHongBaoRet(ret, hongbao_sys_type, hongbao_param);

	return true;
}

void RMIInitHongBaoBackObject::InitHongBaoRet(int _ret, int _hongbao_sys_type, HongBaoParam & _hongbao_param)
{
	printf("RMIInitHongBaoBackObject::InitHongBaoRet:%d\n", _ret);
}

void RMIInitHongBaoBackObject::__exception(int error)
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "InitHongBaoDataAsyn");
}

void RMIInitHongBaoBackObject::__timeout()
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, "InitHongBaoDataAsyn");
}

void RMIInitHongBaoBackObject::__free()
{
	delete this;
}

void RMIInitHongBaoBackObject::InitError()
{
	printf("RMIInitHongBaoBackObject InitError\n");
}

bool RMIGlobalClient::SaveHongBaoDataAsyn( const HongBaoParam& param, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK((sizeof(unsigned int) + sizeof(HongBaoParam)) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_HONG_BAO,
		s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitRAPaiMaiHangDataAsyn(long long id_from, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "RMIGlobalClient::InitRAPaiMaiHangDataAsyn:1");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_RA_ROLE_PAI_MAI_HANG_DATA,
		s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "RMIGlobalClient::InitRAPaiMaiHangDataAsyn:2");
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_RA_ROLE_PAI_MAI_HANG_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveRAPaiMaiHangDataAsyn(const RARoleParMaiHangDBParam & param, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK(sizeof(RARoleParMaiHangDBParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_RA_ROLE_PAI_MAI_HANG_DATA,
		s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMISaveHongBaoBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveHongBaoRet(ret);

	return true;
}

void RMISaveHongBaoBackObject::SaveHongBaoRet(int ret)
{
	printf("SaveHongBaoRet:%d\n", ret);
}

void RMISaveHongBaoBackObject::__free()
{
	delete this;
}

void RMISaveHongBaoBackObject::__exception(int error)
{
	printf("SaveHongBaoRet __exception: %d !!!\n", error);
}

void RMISaveHongBaoBackObject::__timeout()
{
	printf("SaveHongBaoRet __timeout !!!\n");
}


/////////////////////////////////////////////////////////////////////////////////////////
// ¸£Å£±¨Ï²
bool RMIGlobalClient::InitCowReportGoodNewsBuyListDataAsyn(long long _id_from, rmi::RMIBackObject* backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(_id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitCowReportGoodNewsBuyListDataAsyn");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_COW_REPORT_GOOD_NEWS_BUY_LIST, s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitCowReportGoodNewsBuyListDataAsyn");
		return false;
	}

	return ret;
}

bool RMIInitCowReportGoodNewsBuyListBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitCowReportGoodNewsBuyListDataAsyn_1");
		return false;
	}

	static CowReportGoodNewsBuyListParam param;

	if (ret == 0)
	{
		if (!param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitCowReportGoodNewsBuyListDataAsyn_2");
			return false;
		}
	}

	this->InitCowReportGoodNewsBuyListRet(ret, param);

	return true;
}

void RMIInitCowReportGoodNewsBuyListBackObject::InitCowReportGoodNewsBuyListRet(int _ret, CowReportGoodNewsBuyListParam & _param)
{
	printf("RMIInitCowReportGoodNewsBuyListBackObject::InitCowReportGoodNewsBuyListRet:%d\n", _ret);
}

void RMIInitCowReportGoodNewsBuyListBackObject::__exception(int error)
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "InitCowReportGoodNewsBuyListDataAsyn");
}

void RMIInitCowReportGoodNewsBuyListBackObject::__timeout()
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, "InitCowReportGoodNewsBuyListDataAsyn");
}

void RMIInitCowReportGoodNewsBuyListBackObject::__free()
{
	delete this;
}

void RMIInitCowReportGoodNewsBuyListBackObject::InitError()
{
	printf("InitCowReportGoodNewsBuyListDataAsyn InitError\n");
}

bool RMIGlobalClient::SaveCowReportGoodNewsBuyListDataAsyn(const CowReportGoodNewsBuyListParam & param, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK((sizeof(unsigned int) + sizeof(CowReportGoodNewsBuyListParam)) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_COW_REPORT_GOOD_NEWS_BUY_LIST,
		s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMISaveCowReportGoodNewsBuyListBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveCowReportGoodNewsBuyListRet(ret);

	return true;
}

void RMISaveCowReportGoodNewsBuyListBackObject::SaveCowReportGoodNewsBuyListRet(int ret)
{
	printf("SaveCowReportGoodNewsBuyListRet:%d\n", ret);
}

void RMISaveCowReportGoodNewsBuyListBackObject::__free()
{
	delete this;
}

void RMISaveCowReportGoodNewsBuyListBackObject::__exception(int error)
{
	printf("SaveCowReportGoodNewsBuyListRet __exception: %d !!!\n", error);
}

void RMISaveCowReportGoodNewsBuyListBackObject::__timeout()
{
	printf("SaveCowReportGoodNewsBuyListRet __timeout !!!\n");
}

bool RMISaveRAPaiMaiHangDataBackObject::__response(TLVUnserializer & in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveRAPaiMaiHangDataRet(ret);

	return true;
}

void RMISaveRAPaiMaiHangDataBackObject::__exception(int error)
{
	printf("%s error[%d]\n", __FUNCTION__, error);
}

void RMISaveRAPaiMaiHangDataBackObject::__timeout()
{
	printf("%s \n", __FUNCTION__);
}

void RMISaveRAPaiMaiHangDataBackObject::__free()
{
	delete this;
}

void RMISaveRAPaiMaiHangDataBackObject::SaveRAPaiMaiHangDataRet(int ret)
{
	printf("%s:%d\n", __FUNCTION__, ret);
}

bool RMIInitRAPaiMaiHangDataBackObject::__response(TLVUnserializer & in_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
		return false;
	}

	static RARoleParMaiHangDBParam ra_role_pai_mai_hang_db_param;

	if (0 == ret)
	{
		if (!ra_role_pai_mai_hang_db_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
			return false;
		}
	}

	this->InitRAPaiMaiHangDataRet(ret, ra_role_pai_mai_hang_db_param);

	return true;
}

void RMIInitRAPaiMaiHangDataBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, __FUNCTION__);
}

void RMIInitRAPaiMaiHangDataBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, __FUNCTION__);
}

void RMIInitRAPaiMaiHangDataBackObject::__free()
{
	delete this;
}

void RMIInitRAPaiMaiHangDataBackObject::InitRAPaiMaiHangDataRet(int ret, const RARoleParMaiHangDBParam & list_param)
{
	printf("%s:%d\n", __FUNCTION__, ret);
}


/////////////////////////////////////////////////////////////////////////////////////////
// ºÃÓÑÇ×ÃÜ¶È
bool RMIGlobalClient::InitFriendIntimacyDataAsyn(long long _id_from, rmi::RMIBackObject* backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(_id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitFriendIntimacyDataAsyn");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_FRIEND_INTIMACY, s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitFriendIntimacyDataAsyn");
		return false;
	}

	return ret;
}

bool RMIInitFriendIntimacyBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitFriendIntimacyDataAsyn_1");
		return false;
	}

	static FriendIntimacyListParam param;

	if (ret == 0)
	{
		if (!param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitFriendIntimacyDataAsyn_2");
			return false;
		}
	}

	this->InitFriendIntimacyRet(ret, param);

	return true;
}

void RMIInitFriendIntimacyBackObject::InitFriendIntimacyRet(int _ret, FriendIntimacyListParam & _param)
{
	printf("RMIInitFriendIntimacyBackObject::InitFriendIntimacyRet:%d\n", _ret);
}

void RMIInitFriendIntimacyBackObject::__exception(int error)
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "InitFriendIntimacyDataAsyn");
}

void RMIInitFriendIntimacyBackObject::__timeout()
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, "InitFriendIntimacyDataAsyn");
}

void RMIInitFriendIntimacyBackObject::__free()
{
	delete this;
}

void RMIInitFriendIntimacyBackObject::InitError()
{
	printf("InitFriendIntimacyDataAsyn InitError\n");
}

bool RMIGlobalClient::SaveFriendIntimacyDataAsyn(const FriendIntimacyListParam & param, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK((sizeof(unsigned int) + sizeof(FriendIntimacyListParam)) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_FRIEND_INTIMACY,
		s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitWeddingDataAsyn(int _id_from, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(_id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitWeddingDataAsyn");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_WEDDING_LIST, s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitWeddingDataAsyn");
		return false;
	}

	return ret;
}

bool RMIGlobalClient::SaveWeddingDataAsyn(const WeddingInfoDBList & param, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK((sizeof(unsigned int) + sizeof(WeddingInfoDBList)) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_WEDDING_LIST,
		s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitMarketItemDataAsyn(long long _id_from, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(_id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "RMIGlobalClient::InitMarketItemDataAsyn:1");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_MARKET_ITEM_DATA,
		s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "RMIGlobalClient::InitMarketItemDataAsyn:2");
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_MARKET_ITEM_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveMarketItemDataAsyn(const MarketManagerParam & param, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK(sizeof(MarketManagerParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_MARKET_ITEM_DATA,
		s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMISaveFriendIntimacyBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveFriendIntimacyRet(ret);

	return true;
}

void RMISaveFriendIntimacyBackObject::SaveFriendIntimacyRet(int ret)
{
	printf("SaveFriendIntimacyRet:%d\n", ret);
}

void RMISaveFriendIntimacyBackObject::__free()
{
	delete this;
}

void RMISaveFriendIntimacyBackObject::__exception(int error)
{
	printf("SaveFriendRet __exception: %d !!!\n", error);
}

void RMISaveFriendIntimacyBackObject::__timeout()
{
	printf("SaveFriendRet __timeout !!!\n");
}


/////////////////////////////////////////////////////////////////////////////////////////
// ºÃÓÑ
bool RMIGlobalClient::InitFriendDataAsyn(long long _id_from,  rmi::RMIBackObject* backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(_id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitFriendDataAsyn");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_FRIEND, s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitFriendDataAsyn");
		return false;
	}

	return ret;
}

bool RMIInitFriendBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitFriendDataAsyn");
		return false;
	}

	static FriendListParam friend_param;

	if (ret == 0)
	{
		if (!friend_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitFriendDataAsyn");
			return false;
		}
	}

	this->InitFriendRet(ret, friend_param);

	return true;
}

void RMIInitFriendBackObject::InitFriendRet(int _ret, FriendListParam & _friend_list_param)
{
	printf("RMIInitFriendBackObject::InitFriendRet:%d\n", _ret);
}

void RMIInitFriendBackObject::__exception(int error)
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "InitFriendDataAsyn");
}

void RMIInitFriendBackObject::__timeout()
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, "InitFriendDataAsyn");
}

void RMIInitFriendBackObject::__free()
{
	delete this;
}

void RMIInitFriendBackObject::InitError()
{
	printf("RMIInitHongBaoBackObject InitError\n");
}

bool RMIGlobalClient::SaveFriendDataAsyn(const FriendListParam & param, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK((sizeof(unsigned int) + sizeof(FriendListParam)) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_FRIEND,
		s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMISaveFriendBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveFriendRet(ret);

	return true;
}

void RMISaveFriendBackObject::SaveFriendRet(int ret)
{
	printf("SaveFriendRet:%d\n", ret);
}

void RMISaveFriendBackObject::__free()
{
	delete this;
}

void RMISaveFriendBackObject::__exception(int error)
{
	printf("SaveFriendRet __exception: %d !!!\n", error);
}

void RMISaveFriendBackObject::__timeout()
{
	printf("SaveFriendRet __timeout !!!\n");
}

/////////////////////////////////////////////////////////////////////////////////////////
// ºÚÃûµ¥
bool RMIGlobalClient::InitBlackListDataAsyn(long long _id_from, rmi::RMIBackObject* backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(_id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitBlackListDataAsyn");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_BLACK_LIST, s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitBlackListDataAsyn");
		return false;
	}

	return ret;
}

bool RMIInitBlackBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitBlackListDataAsyn");
		return false;
	}

	static BlacklistsParam black_param;

	if (ret == 0)
	{
		if (!black_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitBlackListDataAsyn");
			return false;
		}
	}

	this->InitBlackRet(ret, black_param);

	return true;
}

void RMIInitBlackBackObject::InitBlackRet(int _ret, BlacklistsParam & _black_list_param)
{
	printf("RMIInitBlackBackObject::InitBlackRet:%d\n", _ret);
}

void RMIInitBlackBackObject::__exception(int error)
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "InitBlackListDataAsyn");
}

void RMIInitBlackBackObject::__timeout()
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, "InitBlackListDataAsyn");
}

void RMIInitBlackBackObject::__free()
{
	delete this;
}

void RMIInitBlackBackObject::InitError()
{
	printf("RMIInitBlackBackObject InitError\n");
}

bool RMIGlobalClient::SaveBlackListDataAsyn(const BlacklistsParam & param, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK((sizeof(unsigned int) + sizeof(BlacklistsParam)) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_BLACK_LIST,
		s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitWorldArenaBaseDataAsyn(rmi::RMIBackObject* backobj)
{
	UNSTD_STATIC_CHECK(sizeof(long long) < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_WORLD_ARENA_BASE_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_WORLD_ARENA_BASE_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveWorldArenaBaseDataAsyn(const WorldArenaBaseParam& param, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK(sizeof(CloudArenaParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_WORLD_ARENA_BASE_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitWorldTeamArenaSignUpDataAsyn(long long id_from, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "RMIGlobalClient::InitWorldTeamArenaSignUpDataAsyn:1");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_WORLD_TEAM_ARENA_SIGN_UP_DATA,
		s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "RMIGlobalClient::InitWorldTeamArenaSignUpDataAsyn:2");
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_WORLD_TEAM_ARENA_SIGN_UP_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveWorldTeamArenaSignUpDataAsyn(const WorldTeamArenaSignUpParam & param, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK(sizeof(WorldTeamArenaSignUpParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_WORLD_TEAM_ARENA_SIGN_UP_DATA,
		s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitWorldTeamArenaTeamRankDataAsyn(long long id_from, rmi::RMIBackObject * backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "RMIGlobalClient::InitWorldTeamArenaTeamRankDataAsyn:1");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_WORLD_TEAM_ARENA_TEAM_RANK_DATA,
		s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "RMIGlobalClient::InitWorldTeamArenaTeamRankDataAsyn:2");
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_WORLD_TEAM_ARENA_TEAM_RANK_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveWorldTeamArenaTeamRankDataAsyn(const WorldTeamArenaTeamRankParam & param, rmi::RMIBackObject * backobj)
{
	UNSTD_STATIC_CHECK(sizeof(WorldTeamArenaTeamRankParam) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_WORLD_TEAM_ARENA_TEAM_RANK_DATA,
		s, backobj, true, 5000);

	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMISaveBlackBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveBlackRet(ret);

	return true;
}

void RMISaveBlackBackObject::SaveBlackRet(int ret)
{
	printf("SaveBlackRet:%d\n", ret);
}

void RMISaveBlackBackObject::__free()
{
	delete this;
}

void RMISaveBlackBackObject::__exception(int error)
{
	printf("SaveBlackRet __exception: %d !!!\n", error);
}

void RMISaveBlackBackObject::__timeout()
{
	printf("SaveBlackRet __timeout !!!\n");
}

bool RMISaveWorldTeamArenaTeamRankDataBackObject::__response(TLVUnserializer & in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveWorldTeamArenaTeamRankDataRet(ret);

	return true;
}

void RMISaveWorldTeamArenaTeamRankDataBackObject::__exception(int error)
{
	printf("%s error[%d]\n", __FUNCTION__, error);
}

void RMISaveWorldTeamArenaTeamRankDataBackObject::__timeout()
{
	printf("%s \n", __FUNCTION__);
}

void RMISaveWorldTeamArenaTeamRankDataBackObject::__free()
{
	delete this;
}

void RMISaveWorldTeamArenaTeamRankDataBackObject::SaveWorldTeamArenaTeamRankDataRet(int ret)
{
	printf("%s:%d\n", __FUNCTION__, ret);
}

bool RMIInitWorldTeamArenaTeamRankDataBackObject::__response(TLVUnserializer & in_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
		return false;
	}

	static WorldTeamArenaTeamRankParam world_team_arena_team_rank_param;

	if (0 == ret)
	{
		if (!world_team_arena_team_rank_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
			return false;
		}
	}

	this->InitWorldTeamArenaTeamRankDataRet(ret, world_team_arena_team_rank_param);
	return true;
}

void RMIInitWorldTeamArenaTeamRankDataBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, __FUNCTION__);
}

void RMIInitWorldTeamArenaTeamRankDataBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, __FUNCTION__);
}

void RMIInitWorldTeamArenaTeamRankDataBackObject::__free()
{
	delete this;
}

void RMIInitWorldTeamArenaTeamRankDataBackObject::InitWorldTeamArenaTeamRankDataRet(int ret, const WorldTeamArenaTeamRankParam & list_param)
{
	printf("%s:%d\n", __FUNCTION__, ret);
}

bool RMIInitWorldTeamArenaSignUpDataBackObject::__response(TLVUnserializer & in_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
		return false;
	}

	static WorldTeamArenaSignUpParam world_team_arena_sign_up_param;

	if (0 == ret)
	{
		if (!world_team_arena_sign_up_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
			return false;
		}
	}

	this->InitWorldTeamArenaSignUpDataRet(ret, world_team_arena_sign_up_param);

	return true;
}

void RMIInitWorldTeamArenaSignUpDataBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, __FUNCTION__);
}

void RMIInitWorldTeamArenaSignUpDataBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, __FUNCTION__);
}

void RMIInitWorldTeamArenaSignUpDataBackObject::__free()
{
	delete this;
}

void RMIInitWorldTeamArenaSignUpDataBackObject::InitWorldTeamArenaSignUpDataRet(int ret, const WorldTeamArenaSignUpParam & list_param)
{
	printf("%s:%d\n", __FUNCTION__, ret);
}

bool RMISaveWorldTeamArenaSignUpDataBackObject::__response(TLVUnserializer & in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveWorldTeamArenaSignUpDataRet(ret);

	return true;
}

void RMISaveWorldTeamArenaSignUpDataBackObject::__exception(int error)
{
	printf("%s error[%d]\n", __FUNCTION__, error);
}

void RMISaveWorldTeamArenaSignUpDataBackObject::__timeout()
{
	printf("%s \n", __FUNCTION__);
}

void RMISaveWorldTeamArenaSignUpDataBackObject::__free()
{
	delete this;
}

void RMISaveWorldTeamArenaSignUpDataBackObject::SaveWorldTeamArenaSignUpDataRet(int ret)
{
	printf("%s:%d\n", __FUNCTION__, ret);
}



/////////////////////////////////////////////////////////////////////////////////////////
// Òú»¢ÄÉ¸£
bool RMIGlobalClient::InitTigerBringsBlessingBuyListDataAsyn(long long _id_from, rmi::RMIBackObject* backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = s.Push(_id_from);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitTigerBringsBlessingBuyListDataAsyn");
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_TIGER_BRINGS_BLESSING_BUY_LIST, s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		RMIInitError(RMI_INIT_ERROR_REQUEST, "InitTigerBringsBlessingBuyListDataAsyn");
		return false;
	}

	return ret;
}

bool RMIInitTigerBringsBlessingBuyListBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitTigerBringsBlessingBuyListDataAsyn_1");
		return false;
	}

	static TigerBringsBlessingBuyListParam param;

	if (ret == 0)
	{
		if (!param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitTigerBringsBlessingBuyListDataAsyn_2");
			return false;
		}
	}

	this->InitTigerBringsBlessingBuyListRet(ret, param);

	return true;
}

void RMIInitTigerBringsBlessingBuyListBackObject::InitTigerBringsBlessingBuyListRet(int _ret, TigerBringsBlessingBuyListParam & _param)
{
	printf("RMIInitTigerBringsBlessingBuyListBackObject::InitTigerBringsBlessingBuyListRet:%d\n", _ret);
}

void RMIInitTigerBringsBlessingBuyListBackObject::__exception(int error)
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "InitTigerBringsBlessingBuyListDataAsyn");
}

void RMIInitTigerBringsBlessingBuyListBackObject::__timeout()
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, "InitTigerBringsBlessingBuyListDataAsyn");
}

void RMIInitTigerBringsBlessingBuyListBackObject::__free()
{
	delete this;
}

void RMIInitTigerBringsBlessingBuyListBackObject::InitError()
{
	printf("InitTigerBringsBlessingBuyListDataAsyn InitError\n");
}

bool RMIGlobalClient::SaveTigerBringsBlessingBuyListDataAsyn(const TigerBringsBlessingBuyListParam & param, rmi::RMIBackObject *backobj)
{
	UNSTD_STATIC_CHECK((sizeof(unsigned int) + sizeof(TigerBringsBlessingBuyListParam)) * 2 < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_TIGER_BRINGS_BLESSING_BUY_LIST,
		s, backobj, true, 10000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitWorldArenaScheduleDataAsyn(long long id_from, rmi::RMIBackObject* backobj)
{
	UNSTD_STATIC_CHECK(sizeof(long long) < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	if (!s.Push(id_from)) return false;

	bool ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_WORLD_ARENA_SCHEDULE_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_WORLD_ARENA_SCHEDULE_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::InitWorldArenaUserDataAsyn(long long id_from, rmi::RMIBackObject* backobj)
{
	UNSTD_STATIC_CHECK(sizeof(long long) < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	if (!s.Push(id_from)) return false;

	bool ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_WORLD_ARENA_USER_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_WORLD_ARENA_USER_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveWorldArenaUserDataAsyn(const WorldArenaUserListParamForSave& param, rmi::RMIBackObject* backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_WORLD_ARENA_USER_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::SaveWorldArenaScheduleDataAsyn(const WorldArenaScheduleListParamForSave& param, rmi::RMIBackObject* backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_WORLD_ARENA_SCHEDULE_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMIGlobalClient::InitOfflineUserBattleDataAsync(long long id_from, rmi::RMIBackObject* backobj)
{
	UNSTD_STATIC_CHECK(sizeof(long long) < sizeof(TLV_BUFF));

	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	if (!s.Push(id_from)) return false;

	bool ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::INIT_OFFLINE_USER_BATTLE_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}
	ON_RMI_CALL(GLOBAL, INIT_OFFLINE_USER_BATTLE_DATA, s, backobj);

	return ret;
}

bool RMIGlobalClient::SaveOfflineUserBattleDataAsync(const OfflineUserBattleDataListForSave& param, rmi::RMIBackObject *backobj)
{
	TLVSerializer s;
	s.Reset(TLV_BUFF, sizeof(TLV_BUFF));

	bool ret = param.Serialize(s);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	ret = m_session.rmi_module->Call(m_session, dataaccessrmi::GLOBAL, dataaccessrmi::global::SAVE_OFFLINE_USER_BATTLE_DATA, s, backobj, true, 5000);
	if (!ret)
	{
		delete backobj;
		return false;
	}

	return ret;
}

bool RMISaveTigerBringsBlessingBuyListBackObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveTigerBringsBlessingBuyListRet(ret);

	return true;
}

void RMISaveTigerBringsBlessingBuyListBackObject::SaveTigerBringsBlessingBuyListRet(int ret)
{
	printf("SaveTigerBringsBlessingBuyListRet:%d\n", ret);
}

void RMISaveTigerBringsBlessingBuyListBackObject::__free()
{
	delete this;
}

void RMISaveTigerBringsBlessingBuyListBackObject::__exception(int error)
{
	printf("SaveTigerBringsBlessingBuyListRet __exception: %d !!!\n", error);
}

void RMISaveTigerBringsBlessingBuyListBackObject::__timeout()
{
	printf("SaveTigerBringsBlessingBuyListRet __timeout !!!\n");
}
bool RMIInitWeddingBackObject::__response(TLVUnserializer & in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitWeddingDataAsyn_1");
		return false;
	}

	static WeddingInfoDBList param;

	if (ret == 0)
	{
		if (!param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitWeddingDataAsyn_2");
			return false;
		}
	}

	this->InitWeddingRet(ret, param);

	return true;
}

void RMIInitWeddingBackObject::__exception(int error)
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "InitWeddingDataAsyn");
}

void RMIInitWeddingBackObject::__timeout()
{
	this->InitError();
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, "InitWeddingDataAsyn");
}

void RMIInitWeddingBackObject::__free()
{
	delete this;
}

void RMIInitWeddingBackObject::InitWeddingRet(int _ret, WeddingInfoDBList & _param)
{
	printf("RMIInitFriendIntimacyBackObject::InitWeddingRet:%d\n", _ret);
}

void RMIInitWeddingBackObject::InitError()
{
	printf("InitWeddingDataAsyn InitError\n");
}

bool RMISaveWeddingBackObject::__response(TLVUnserializer & in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveWeddingRet(ret);

	return true;
}

void RMISaveWeddingBackObject::__exception(int error)
{
	printf("SaveWeddingRet __exception: %d !!!\n", error);
}

void RMISaveWeddingBackObject::__timeout()
{
	printf("SaveWeddingRet __timeout !!!\n");
}

void RMISaveWeddingBackObject::__free()
{
	delete this;
}

void RMISaveWeddingBackObject::SaveWeddingRet(int ret)
{
	printf("SaveWeddingRet:%d\n", ret);
}

bool RMISaveWorldArenaBaseDataObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveRet(ret);
	return true;
}

void RMISaveWorldArenaBaseDataObject::__exception(int error)
{
	printf("RMISaveWorldArenaBaseDataObject::__exception error[%d]\n", error);
}

void RMISaveWorldArenaBaseDataObject::__timeout()
{
	printf("RMISaveWorldArenaBaseDataObject::__timeout\n");
}

void RMISaveWorldArenaBaseDataObject::__free()
{
	delete this;
}

void RMISaveWorldArenaBaseDataObject::SaveRet(int ret)
{
	printf("RMISaveWorldArenaBaseDataObject::SaveRet ret[%d]\n", ret);
}

bool RMIInitWorldArenaBaseDataObject::__response(TLVUnserializer &in_param)
{
	ON_RMI_BACK(return false;);

	int result = -1;
	if (!in_param.Pop(&result))
	{
		return false;
	}

	static WorldArenaBaseParam data;
	if (0 == result)
	{
		if (!data.Unserialize(in_param))
		{
			return false;
		}
	}

	this->InitRet(result, data);

	return true;
}

void RMIInitWorldArenaBaseDataObject::__exception(int error)
{
	printf("RMIInitWorldArenaBaseDataObject::__exception error[%d]\n", error);
}

void RMIInitWorldArenaBaseDataObject::__timeout()
{
	printf("RMIInitWorldArenaBaseDataObject::__timeout\n");
}

void RMIInitWorldArenaBaseDataObject::__free()
{
	delete this;
}

void RMIInitWorldArenaBaseDataObject::InitRet(int result, const WorldArenaBaseParam& data)
{
	printf("RMIInitWorldArenaBaseDataObject::InitRet result[%d]\n", result);
}

bool RMIInitWorldArenaUserDataObject::__response(TLVUnserializer &in_param)
{
	ON_RMI_BACK(return false;);

	int result = -1;
	if (!in_param.Pop(&result))
	{
		return false;
	}

	WorldArenaUserListParamForInit data;
	if (0 == result)
	{
		if (!data.Unserialize(in_param))
		{
			return false;
		}
	}

	this->InitRet(result, data);

	return true;
}

void RMIInitWorldArenaUserDataObject::__exception(int error)
{
	printf("RMIInitWorldArenaUserDataObject::__exception error[%d]\n", error);
}

void RMIInitWorldArenaUserDataObject::__timeout()
{
	printf("RMIInitWorldArenaUserDataObject::__timeout\n");
}

void RMIInitWorldArenaUserDataObject::__free()
{
	delete this;
}

void RMIInitWorldArenaUserDataObject::InitRet(int result, const WorldArenaUserListParamForInit& data)
{
	printf("RMIInitWorldArenaUserDataObject::InitRet result[%d]\n", result);
}

bool RMIInitWorldArenaScheduleDataObject::__response(TLVUnserializer &in_param)
{
	ON_RMI_BACK(return false;);

	int result = -1;
	if (!in_param.Pop(&result))
	{
		return false;
	}

	WorldArenaScheduleListParamForInit data;
	if (0 == result)
	{
		if (!data.Unserialize(in_param))
		{
			return false;
		}
	}

	this->InitRet(result, data);

	return true;
}

void RMIInitWorldArenaScheduleDataObject::__exception(int error)
{
	printf("RMIInitWorldArenaScheduleDataObject::__exception error[%d]\n", error);
}

void RMIInitWorldArenaScheduleDataObject::__timeout()
{
	printf("RMIInitWorldArenaScheduleDataObject::__timeout\n");
}

void RMIInitWorldArenaScheduleDataObject::__free()
{
	delete this;
}

void RMIInitWorldArenaScheduleDataObject::InitRet(int result, const WorldArenaScheduleListParamForInit& data)
{
	printf("RMIInitWorldArenaScheduleDataObject::InitRet result[%d]\n", result);
}

bool RMISaveWorldArenaScheduleDataObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveRet(ret);

	return true;
}

void RMISaveWorldArenaScheduleDataObject::__exception(int error)
{
	printf("RMISaveWorldArenaScheduleDataObject::__exception error[%d]\n", error);
}

void RMISaveWorldArenaScheduleDataObject::__timeout()
{
	printf("RMISaveWorldArenaScheduleDataObject::__timeout\n");
}

void RMISaveWorldArenaScheduleDataObject::__free()
{
	delete this;
}

void RMISaveWorldArenaScheduleDataObject::SaveRet(int ret)
{
	printf("RMISaveWorldArenaScheduleDataObject ret[%d]\n", ret);
}

bool RMISaveWorldArenaUserDataObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveRet(ret);

	return true;
}

void RMISaveWorldArenaUserDataObject::__exception(int error)
{
	printf("RMISaveWorldArenaUserDataObject::__exception error[%d]\n", error);
}

void RMISaveWorldArenaUserDataObject::__timeout()
{
	printf("RMISaveWorldArenaUserDataObject::__timeout\n");
}

void RMISaveWorldArenaUserDataObject::__free()
{
	delete this;
}

void RMISaveWorldArenaUserDataObject::SaveRet(int ret)
{
	printf("RMISaveWorldArenaUserDataObject::SaveRet ret[%d]\n", ret);
}

bool RMISaveMarketItemDataBackObject::__response(TLVUnserializer & in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveMarketItemDataRet(ret);

	return true;
}

void RMISaveMarketItemDataBackObject::__exception(int error)
{
	printf("%s error[%d]\n", __FUNCTION__, error);
}

void RMISaveMarketItemDataBackObject::__timeout()
{
	printf("%s \n", __FUNCTION__);
}

void RMISaveMarketItemDataBackObject::__free()
{
	delete this;
}

void RMISaveMarketItemDataBackObject::SaveMarketItemDataRet(int ret)
{
	printf("%s:%d\n", __FUNCTION__, ret);
}

bool RMIInitMarketItemDataBackObject::__response(TLVUnserializer & in_param)
{
	ON_RMI_BACK(return false;);

	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
		return false;
	}

	static MarketManagerParam market_param;

	if (0 == ret)
	{
		if (!market_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, __FUNCTION__);
			return false;
		}
	}

	this->InitMarketItemDataRet(ret, market_param);

	return true;
}

void RMIInitMarketItemDataBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, __FUNCTION__);
}

void RMIInitMarketItemDataBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, __FUNCTION__);
}

void RMIInitMarketItemDataBackObject::__free()
{
	delete this;
}

void RMIInitMarketItemDataBackObject::InitMarketItemDataRet(int ret, const MarketManagerParam & list_param)
{
	printf("%s:%d\n", __FUNCTION__, ret);
}

bool RMIInitPartnerRankBackObject::__response(TLVUnserializer & in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitPartnerRankAsyn");
		return false;
	}

	static PartnerRankParam peartner_rank_param;
	peartner_rank_param.count = 0;

	if (0 == ret)
	{
		if (!peartner_rank_param.Unserialize(in_param))
		{
			RMIInitError(RMI_INIT_ERROR_RESPONSE, "InitPartnerRankAsyn");
			return false;
		}
	}

	this->InitPartnerRankRet(ret, peartner_rank_param);

	return true;
}

void RMIInitPartnerRankBackObject::__exception(int error)
{
	RMIInitError(RMI_INIT_ERROR_EXCEPTION, "InitPartnerRankAsyn");
}

void RMIInitPartnerRankBackObject::__timeout()
{
	RMIInitError(RMI_INIT_ERROR_TIMEOUT, "InitPartnerRankAsyn");
}

void RMIInitPartnerRankBackObject::__free()
{
	delete this;
}

void RMIInitPartnerRankBackObject::InitPartnerRankRet(int ret, const PartnerRankParam & person_rank_param)
{
	printf("InitPartnerRankRet:%d\n", ret);
}


bool RMISavePartnerRankBackObject::__response(TLVUnserializer & in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SavePartnerRankRet(ret);

	return true;
}

void RMISavePartnerRankBackObject::__exception(int error)
{
	printf("RMISavePartnerRankBackObject SaveError\n");
}

void RMISavePartnerRankBackObject::__timeout()
{
}

void RMISavePartnerRankBackObject::__free()
{
	delete this;
}

void RMISavePartnerRankBackObject::SavePartnerRankRet(int ret)
{
	printf("SavePartnerRankRet:%d\n", ret);
}

bool RMISaveOfflineUserBattleDataObject::__response(TLVUnserializer &in_param)
{
	int ret = -1;
	if (!in_param.Pop(&ret))
	{
		return false;
	}

	this->SaveRet(ret);

	return true;
}

void RMISaveOfflineUserBattleDataObject::__exception(int error)
{
	printf("RMISaveOfflineUserBattleDataObject::__exception error[%d]\n", error);
}

void RMISaveOfflineUserBattleDataObject::__timeout()
{
	printf("RMISaveOfflineUserBattleDataObject::__timeout\n");
}

void RMISaveOfflineUserBattleDataObject::__free()
{
	delete this;
}

void RMISaveOfflineUserBattleDataObject::SaveRet(int ret)
{
	printf("RMISaveOfflineUserBattleDataObject::SaveRet ret[%d]\n", ret);
}

bool RMIInitOfflineUserBattleDataObject::__response(TLVUnserializer &in_param)
{
	ON_RMI_BACK(return false;);

	int result = -1;
	if (!in_param.Pop(&result))
	{
		return false;
	}

	OfflineUserBattleDataListForInit data;
	if (0 == result)
	{
		if (!data.Unserialize(in_param))
		{
			return false;
		}
	}

	this->InitRet(result, data);

	return true;
}

void RMIInitOfflineUserBattleDataObject::__exception(int error)
{
	printf("RMIInitOfflineUserBattleDataObject::__exception error[%d]\n", error);
}

void RMIInitOfflineUserBattleDataObject::__timeout()
{
	printf("RMIInitOfflineUserBattleDataObject::__timeout\n");
}

void RMIInitOfflineUserBattleDataObject::__free()
{
	delete this;
}

void RMIInitOfflineUserBattleDataObject::InitRet(int result, const OfflineUserBattleDataListForInit& data)
{
	printf("RMIInitOfflineUserBattleDataObject::InitRet result[%d] count[%d]\n", result, (int)data.paramlist.size());
}
