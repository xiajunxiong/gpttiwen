#ifndef __MARRIAGE_HPP__
#define __MARRIAGE_HPP__

#include "servercommon/marrydef.hpp"
#include "servercommon/attributedef.hpp"
#include "other/attributelist.hpp"
#include <map>

class Role;
class RoleModuleManager;

class Marriage
{
public:
	Marriage();
	~Marriage();
	
	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *role_module_manager, const MarriageParam &param);
	void GetOtherInitParam(MarriageParam *param);

	void OnLogout();

	void Update(unsigned long interval, time_t now_second);

	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);

	bool IsOnlyLoverInTeam();
	bool IsMarried();
	bool IsNewlyMarried();
	Role * GetLover();
	int  GetLoverUid();
	void SendInfo();
	void SendOpResult(int type);

	//
	void Propose(int marry_type, int target_uid);
	void CancelPropose();
	void ProposeReply(Role * marry_target_role, bool is_accept);

	void DivorceReq();
	void DivorceReply(Role * reply_role,bool is_accpet);
	void ForceDivorceReq();
	void CancelForceDivorce();
	void OnDivorce(unsigned int divorce_time, bool clear_lover = false);

	bool CanStart(int param1);
	bool CanAutoFb();
	void SetPlayTimes(int param1, int times = 1);
	void SendMarriageFbReward(int wave, bool is_auto_fb = false);

	MarriageParam & GetParam();

	void QueryOtherMarriageInfo(int role_id);


private:
	struct SendReqInfo
	{
		SendReqInfo()
		{
			Reset();
		}

		void Reset()
		{
			is_sender = false;
			target_uid = 0;
			time = 0;
			param1 = -1;
		}

		bool is_sender;
		int target_uid;
		time_t time;
		int param1;
	};

	RoleModuleManager * m_role_module_mgr;

	MarriageParam m_param;
	AttributeList m_attrs_add;

	SendReqInfo m_propose_info;	
	unsigned int m_req_divorce_time;;
};


#endif // !__MARRIAGE_HPP__
