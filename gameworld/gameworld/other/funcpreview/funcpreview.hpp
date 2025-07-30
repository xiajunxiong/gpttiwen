#ifndef __FUNC_PREVIEW_HPP__
#define __FUNC_PREVIEW_HPP__

#include "servercommon/funcpreviewdef.h"

class RoleModuleManager;
class FuncPreview
{
public:
	FuncPreview();
	~FuncPreview();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* mgr, const FuncPreviewParam& param);
	void GetInitParam(ARG_OUT FuncPreviewParam* param);

	void ClaimReward(int notice_id);
	void OnLogin();
	void OnLevelUp();
	void SendInfo();
private:
	void CheckClaimable(bool is_notice_client);

	RoleModuleManager* m_mgr;
	FuncPreviewParam m_param;
};

#endif