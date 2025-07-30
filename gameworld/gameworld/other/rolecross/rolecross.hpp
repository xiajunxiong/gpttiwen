
#ifndef __ROLE_CROSS_HPP__
#define __ROLE_CROSS_HPP__

#include "servercommon/rolecrossdef.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"

class RoleModuleManager;
class RoleCross
{
public:
	RoleCross();
	~RoleCross();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *mgr, const RoleCrossParam &p);
	void GetInitParam(RoleCrossParam *p);
	long long GetOriginUUID() const { return m_param.origin_uuid; }
	int GetOriginServerId() const { return m_param.origin_server_id; }
	const char* GetOriginRoleName() const { return m_param.origin_role_name; }
	int GetOriginPlatType() const { return m_param.origin_plat_type; }
	inline bool IsPreparingForCross() const { return m_is_preparing_for_cross; }
	void ResetIsPreparingForCross() { m_is_preparing_for_cross = false; }
	void Update(time_t now_second);

	bool OnStartCrossReq(const CrossData_Business& business_data);
	void OnStartCrossAck(crossgameprotocal::CrossGameStartCrossAck* cgsca);
	void OnRoleChangeName();
	void OnBackToOriginServerReq(ARG_IN const ReturnOriginData_Business* in_business_data = NULL);
	void OnBackToOriginServerAck(crossgameprotocal::GameHiddenRoleReturnOriginServerAck* ack);

	void SendCrossConnectInfo();
private:

	RoleModuleManager* m_module_mgr;
	RoleCrossParam m_param;
	bool m_is_preparing_for_cross;
};

#endif