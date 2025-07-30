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

		int						original_plat_type;							// ԭ��ƽ̨����
		int						original_server_id;							// ԭ��������ID
		UserID					original_user_id;							// ��ԭ���Ľ�ɫID
		GameName				original_rolename;							// ��ԭ���Ľ�ɫ��
		bool					can_enter_hidden;							// �Ƿ���Խ������ط�
		CrossData_Business		business_data;								// ҵ�����ݣ����ڸ�֪���ط�����ҿ����Ҫ��ʲô
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
	OriginUuidToUserIDmap m_origin_uuid_uid_map;							// �����ɫԭ��uuid�뵱ǰuser_id��Ӧ��

	typedef std::map<UserID, CrossUserInfo> CrossUserInfoMap;
	typedef std::map<UserID, CrossUserInfo>::iterator CrossUserInfoMapIt;
	CrossUserInfoMap m_cross_user_info_map;									// �����ɫuser_id��ԭ����ɫ��Ϣ��Ӧ��

	int m_uid_generator;

	SessionKey m_cross_session_key;
};

#endif // __CROSS_USER_REGISTER__

