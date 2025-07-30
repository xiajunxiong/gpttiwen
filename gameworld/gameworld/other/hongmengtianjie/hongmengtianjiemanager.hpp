#ifndef __HONGMENG_TIANJIE_MANAGER_HPP__
#define __HONGMENG_TIANJIE_MANAGER_HPP__

#include "obj/character/role.h"
#include "servercommon/commonsavedef.h"



class Role;
class RoleModuleManager;
struct CommonSaveWorldHongMengTianJie;
class CommonSave;
class Team;

class WorldHongMengTianJieManager
{
public:
	static WorldHongMengTianJieManager & Instance();

	bool IsFirstPass(int fb_id);
	int GetDifficultf(int fb_type, int fb_seq);

	void SetFirstPassData(int fb_id, Team * team);

	bool CanEnterFB(Role * role, int fb_type, int fb_seq);
	
	void OnReq(const char * msg);

	void SendPassTime(Role * role);
	void SendPassTeam(Role * role, int fb_id);
	void EnterFB(Role* role, int type, int fb_seq, int member_num, int* member_list);

	bool CanRoleEnterFB(Role* role, int fb_type, int fb_seq);
private:
	WorldHongMengTianJieManager();
	virtual ~WorldHongMengTianJieManager();

	CommonSave * GetCommonSave() const;
	CommonSaveWorldHongMengTianJie * GetCommonSaveFBData(bool is_auto_create = true);
	void SetCommonSaveDirtyMark();
};

#endif