#ifndef RMIROLECLIENT_H
#define RMIROLECLIENT_H

#include "rmi/rmibase.h"
#include "servercommon/serverdef.h"
#include "servercommon/logindef.h"
#include "servercommon/roleactivitydef.hpp"

class RMIRoleSaveBackObject;
class SkillListParam;
class ItemListParam;
class RoleQueryParam;
class RoleOtherInitParam;
class RoleInitParam;
class FriendListParam;
class BlacklistsParam;
class MailListParam;
class RoleFirstKillParamList;
class RMIRoleSaveDataObject;
struct CommonDataParam;
class RoleCommonSaveDataParam;
class RoleCourageChallengeLevelParamList;
class GodPrintItemListParam;
class NewCourageChallengeLevelParamList;

class RMIRoleClient : public rmi::RMIProxyObject
{
public:
	RMIRoleClient() {}
	virtual ~RMIRoleClient() {}

	bool RoleInitAsyn(int uid, rmi::RMIBackObject *backobj, const char * pname);

	bool RoleSaveAsyn(int uid,
					  const RoleInitParam &p,
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
					  RMIRoleSaveBackObject *backobj,
					  bool is_logout = false);

	bool RoleSaveStatusAsyn(int uid, bool is_logout, bool is_cross, bool is_micro_pc, PlatName pname, rmi::RMIBackObject *backobj);
	bool RoleQueryAsyn(int uid, rmi::RMIBackObject *backobj);
	bool RoleReadDataAsyn(int uid, rmi::RMIBackObject *backobj);
	bool RoleSaveDataAsyn(int uid, const CommonDataParam & common_param, RMIRoleSaveDataObject * backobj);
	bool QueryRetrievePetAsyn(int uid, int pet_id, long long item_unique_id, rmi::RMIBackObject *backobj);
	bool RoleMuteAsyn(int uid, unsigned int forbid_time, int forbid_talk_type, rmi::RMIBackObject * backobj);
	bool RoleCmdToRoleSetAuthorityAsyn(int uid, int authority_type, rmi::RMIBackObject * backobj);
};

class RMIRoleInitBackObject : public rmi::RMIBackObject
{
public:
	RMIRoleInitBackObject() {}
	virtual ~RMIRoleInitBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error) {}
	virtual void __timeout() {}
	virtual void __free();

protected:
	virtual void RoleInitRet(int ret, const RoleInitParam &p,
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
							 long long last_save_time);

};

class RMIRoleQueryBackObject : public rmi::RMIBackObject
{
public:
	RMIRoleQueryBackObject() {}
	virtual ~RMIRoleQueryBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error) {}
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void RoleQueryRet(int ret, const RoleQueryParam &p, const QueryItemListParam &item_list_param, const GodPrintItemListParam &god_print_item_list_param);
};

class RMIQueryRetrievePetBackObject : public rmi::RMIBackObject
{
public:
	RMIQueryRetrievePetBackObject() {}
	virtual ~RMIQueryRetrievePetBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void QueryRetrievePetRet(int ret, const ItemListParam &item_list_param);
};

class RMIRoleSaveBackObject : public rmi::RMIBackObject
{
public:
	RMIRoleSaveBackObject();
	virtual ~RMIRoleSaveBackObject();

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

	void SetData(const TLVSerializer &data);

	UserID log_user_id; 

protected:
	virtual void RoleSaveRet(int ret);
	void Error(const UserID &user_id, int reason, int ret);
};

class RMIRoleSaveStatusBackObject : public rmi::RMIBackObject
{
public:
	RMIRoleSaveStatusBackObject();
	virtual ~RMIRoleSaveStatusBackObject();

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

	UserID log_user_id; 

protected:
	virtual void RoleSaveStatusRet(int ret);
	void Error(const UserID &user_id, int reason, int ret);
};

class RMIRoleReadDataObject : public rmi::RMIBackObject
{
public:
	RMIRoleReadDataObject() {}
	~RMIRoleReadDataObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error) {}
	virtual void __timeout() {}
	virtual void __free();
protected:
	virtual void RoleReadDataRet(int ret, const RoleInitParam &p,
		const RoleOtherInitParam &other_p,
		const ItemListParam& item_list_param,
		const SkillListParam& skill_list_param,
		const MailListParam& maillist_param,
		const RoleFirstKillParamList& first_kill_list,
		const RandActivityRoleParamList & rand_activity_role_list,
		const RoleCommonSaveDataParam & role_common_save_list,
		const RoleCourageChallengeLevelParamList & role_courage_challenge_List,
		const GodPrintItemListParam & god_print_item_list,
		const NewCourageChallengeLevelParamList & new_courage_challenge_List,
		long long last_save_time);
};

class RMIRoleSaveDataObject : public rmi::RMIBackObject
{
public:
	RMIRoleSaveDataObject() {}
	~RMIRoleSaveDataObject() {}

	int uid;
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error) {}
	virtual void __timeout() {}
	virtual void __free();
protected:
	virtual void RoleSaveDataRet(int ret);

};

class RMIRoleMuteBackObject : public rmi::RMIBackObject
{
public:
	RMIRoleMuteBackObject() {}
	virtual ~RMIRoleMuteBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error) {}
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void RoleMuteRet(int ret, unsigned int forbid_time, int forbid_talk_type);
};

class RMIRoleCmdSetAuthorityBackObject : public rmi::RMIBackObject
{
public:
	RMIRoleCmdSetAuthorityBackObject() {}
	virtual ~RMIRoleCmdSetAuthorityBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error) {}
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void RoleCmdSetAuthorityRet(int ret, int authority_type);
};

#endif

