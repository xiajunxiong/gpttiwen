#ifndef __DU_JIE_HPP__
#define __DU_JIE_HPP__

#include "servercommon/dujiedef.hpp"
class RoleModuleManager;
class DuJie
{
public:
	DuJie();
	~DuJie();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *module_mgr, const DuJieParam & param);
	void GetInitParam(DuJieParam * param);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	//角色操作
	void OnReq(int req_type, int param_1);

	//可进入副本
	bool CanEnterFB(int fb_seq, int is_assist);
	//结算
	void OnFBFinish(int fb_seq);
	//助战结算
	void AssistFinish(int fb_index);
	int GetMaxPassSeq() { return m_param.max_pass_fb_seq; } 

	void ChangeTaskParam(int task_type, int task_param);
	int GetTaskParam(int fb_seq);

	void SendInfo();
private:


	RoleModuleManager * m_module_mgr;

	DuJieParam m_param;
};

#endif
