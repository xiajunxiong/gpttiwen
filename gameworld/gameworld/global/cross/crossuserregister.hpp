#ifndef __CROSS_USER_REGISTER__
#define __CROSS_USER_REGISTER__

#include "engineadapter.h"
#include "servercommon/servercommon.h"
#include "servercommon/internalprotocal/crossglobalprotocal.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"

class Role;

class CrossUserRegister
{
public:
	struct CrossUserInfo
	{
		CrossUserInfo() : original_plat_type(-1), original_server_id(0), 
			original_user_id(INVALID_USER_ID), can_enter_hidden(false)
		{
			
		}

		int						original_plat_type;							// 原服平台类型
		int						original_server_id;							// 原服服务器ID
		UserID					original_user_id;							// 在原服的角色ID
		GameName				original_rolename;							// 在原服的角色名
		bool					can_enter_hidden;							// 是否可以进入隐藏服
		CrossData_Business		business_data;								// 业务数据，用于告知隐藏服，玩家跨服后要做什么
	};

public:
	static CrossUserRegister & Instance();

	bool OnStartCrossReq(crossgameprotocal::CrossHiddenStartCrossReq *req);
	void CacheCrossRoleInfo(PlatName pname, const UserID& new_user_id, int origin_plat_type, 
		int origin_server_id, const UserID& origin_user_id, GameName origin_rolename, const CrossData_Business& business_data);

	void OnUserLogin(const UserID& user_id);
	void OnUserLogout(int uid);
	void OnUserKicked(Role* user);

	void OnConnectCrossServerSucc();

	void SendToAllCrossUser(const char *msg, int len);
	void KickAllCrossUser();

	CrossUserInfo* GetCrossUserInfo(const UserID& new_user_id);
	bool IsHasCrossUser(const UserID& new_user_id);
	UserID GetCrossUserID(long long uuid);
private:
	CrossUserRegister();
	~CrossUserRegister();

	bool CheckCanStartCross(long long uuid);
	void SendCrossUserLoginToCross(const UserID &user_id);
	void CheckActivityEnterScene(const UserID& user_id);

	typedef std::map<long long, UserID> OriginUuidToUserIDmap;
	OriginUuidToUserIDmap m_origin_uuid_uid_map;							// 跨服角色原服uuid与当前user_id对应表

	typedef std::map<UserID, CrossUserInfo> CrossUserInfoMap;
	typedef std::map<UserID, CrossUserInfo>::iterator CrossUserInfoMapIt;
	CrossUserInfoMap m_cross_user_info_map;									// 跨服角色user_id和原服角色信息对应表

	int m_uid_generator;

	SessionKey m_cross_session_key;
};

#endif // __CROSS_USER_REGISTER__

