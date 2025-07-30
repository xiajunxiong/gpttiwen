
#ifndef RMIBACKOBJDEF_H
#define RMIBACKOBJDEF_H

#include "servercommon/logindef.h"
#include "rmiclient/rmiroleclient.h"
#include "rmiclient/rmiloginclient.h"
#include "rmiclient/rmihidden.h"
#include "inetworkmodule.h"
#include "servercommon/struct/roleinfolist.h"
#include "servercommon/crossdef.hpp"

class SceneManager;

class RMIRoleInitBackObjectImpl : public RMIRoleInitBackObject
{
public:
	SceneManager *scene_manager;
	SceneIndex	scene_index;
	int scene_id;
	GSNetID netid;
	IP userip;
	PlatName pname;
	UserID user_id;
	bool is_login;
	int inc_key;
	int plat_spid;

	bool is_micro_pc;

	void *	operator new(size_t c);
	void	operator delete(void *m);

	virtual void __timeout();
	virtual void __exception(int error);

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

class RMIRoleSaveBackObjImplLocal : public RMIRoleSaveBackObject
{
public:
	virtual void RoleSaveRet(int ret);

	void *	operator new(size_t c);
	void	operator delete(void *m);
};

class RMIRoleSaveBackObjImplRemote : public RMIRoleSaveBackObject
{
public:
	UserID uid;
	GameName role_name;
	GSNetID netid;
	int target_scene_id;
	int target_scene_key;
	int scene_id;
	unsigned short gs_index;
	PlatName pname;

	virtual void RoleSaveRet(int ret);

	void *	operator new(size_t c);
	void	operator delete(void *m);
};

class RMIRoleSaveBackObjectImplRole : public RMIRoleSaveBackObject
{
public:
	virtual void RoleSaveRet(int ret);

	void *	operator new(size_t c);
	void	operator delete(void *m);

	bool is_logout;
	int role_id;
};

class RMIGetGoldBackObjectImpl : public RMIGetGoldBackObject
{
public:
	SceneManager *scene_manager;

	UserID user_id;

	virtual void GetGoldRet(Int64 gold, int authority_type);

	void *	operator new(size_t c);
	void	operator delete(void *m);
};

class RMIGetGoldBackObjectImplChange : public RMIGetGoldBackObject
{
public:
	SceneManager *scene_manager;
	SceneIndex scene_index;
	ObjID obj_id;

	UserID user_id;
	Int64 gold_get;
	GSNetID netid;

	void *	operator new(size_t c);
	void	operator delete(void *m);
protected:
	virtual void GetGoldRet(Int64 gold);
};

class RMIChangeGoldBackObjectImpl : public RMIChangeGoldBackObject
{
public:
	SceneManager *scene_manager;

	SceneIndex scene_index; 
	ObjID obj_id; 
	UserID user_id;

	Int64 gold_get;
	int authority_type;

	PlatName pname;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void ChangeGoldRet(int result, Int64 new_gold);
	virtual void Error(const char *err_str);
};

class RMIChangeGoldBackObjectImplErrorAdd : public RMIChangeGoldBackObject
{
public:
	UserID user_id;
	Int64 gold_get;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void ChangeGoldRet(int result, Int64 new_gold);
	virtual void Error(const char *err_str);
};

class RMIRoleSaveStatusBackObjectImpl : public RMIRoleSaveStatusBackObject
{
public:
	virtual void RoleSaveStatusRet(int ret);

	void *	operator new(size_t c);
	void	operator delete(void *m);
};

class RMIRoleMuteBackObjectImpl : public RMIRoleMuteBackObject
{
public:
	int role_id;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void RoleMuteRet(int ret, unsigned int forbid_time, int forbid_talk_type);
};

class RMIRoleCmdSetAuthorityBackObjectImpl : public RMIRoleCmdSetAuthorityBackObject
{
public:
	int role_id;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void RoleCmdSetAuthorityRet(int ret, int authority_type);
};


class RMIQueryNameExistBackObjectImpl : public RMIQueryNameExistBackObject
{
public:
	UserID user_id;
	GameName role_name;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void QueryNameExistRet(int result);
};

class RMIResetNameBackObjectImpl : public RMIResetNameBackObject
{
public:
	UserID user_id;
	GameName role_name;
	long long name_strid;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void ResetNameRet(int result);
};

class RMIRoleQueryBackObjectImpl : public RMIRoleQueryBackObject
{
public:
	UserID viewer_user_id;
	int target_user_id;

	virtual void RoleQueryRet(int ret, const RoleQueryParam &p, const QueryItemListParam &item_list_param, const GodPrintItemListParam &god_print_item_list_param);

	virtual void __timeout();

	void *	operator new(size_t c);
	void	operator delete(void *m);
};

class RMIRoleReadDataObjectImpl : public RMIRoleReadDataObject
{
public:
	//void *	operator new(size_t c);
//	void	operator delete(void *m);

	virtual void __timeout();
	virtual void __exception(int error);
protected:
	virtual void RoleReadDataRet(int ret, const RoleInitParam &p,
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

class RMIRoleSaveDataObjectImpl : public RMIRoleSaveDataObject
{
public:

	//void *	operator new(size_t c);
	//void	operator delete(void *m);

protected:
	virtual void RoleSaveDataRet(int ret);

};

class RMIQueryRetrievePetBackObjectImpl : public RMIQueryRetrievePetBackObject
{
public:
	int cmd_id;
	int uid;
	int pet_id;
	long long item_unique_id;

	virtual void QueryRetrievePetRet(int ret, const ItemListParam &item_list_param);

	void *	operator new(size_t c);
	void	operator delete(void *m);
};

class RMIGetGoldHistoryBackObjectImpl :public RMIGetGoldHistoryBackObject
{
public:
	UserID user_id;

	virtual void GetGoldHistoryRet(Int64 gold_history);

	void *	operator new(size_t c);
	void	operator delete(void *m);
};

class RMIHiddenGetRoleInfoImpl : public RMIGetRoleInfoBackObject
{
public:
	RMIHiddenGetRoleInfoImpl() : origin_plat_type(0), origin_uid(0), origin_server_id(0)
	{
		memset(origin_pname, 0, sizeof(origin_pname));
	}

	int				origin_plat_type;
	int				origin_uid;
	int				origin_server_id;
	PlatName		origin_pname;
	CrossData_Business cross_business_data;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void GetRoleInfoRet(int ret, const RoleInfoList &role_info);
};

class RMIGetRoleCmdImpl : public RMIGetRoleCmd
{
public:
	int role_id;

	void * operator new(size_t c);
	void	operator delete(void * m);
protected:
	virtual void GetGetRoleCmdRet(long long role_cmd_id, const std::string & cmd,unsigned long long create_time);
};

class RMIComfirmRoleCmdImpl : public RMIComfirmRoleCmd
{
public:
	int role_id;
	long long role_cmd_id;
	unsigned long long	create_time;
	std::string cmd;

	void * operator new(size_t c);
	void	operator delete(void * m);
protected:
	virtual void ChangeRoleCmdComfirmTimeRet(int result);
};

class RMIComfirmRoleCmdErrorImpl : public RMIComfirmRoleCmd
{
public:
	int role_id;
	long long role_cmd_id;
	std::string cmd;

	void * operator new(size_t c);
	void	operator delete(void * m);
protected:
	virtual void ChangeRoleCmdComfirmTimeRet(int result);
};


#endif
