#ifndef _SAN_XIAN_GUI_DONG_HPP_
#define _SAN_XIAN_GUI_DONG_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/commondata.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

class RoleModuleManager;
class SanXianGuiDong
{
public:
	SanXianGuiDong();
	~SanXianGuiDong();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* module_mgr,const int &param, const int &help_param, const unsigned int & tamp);
	void GetInitParam(int * param, int * help_param, unsigned int * tamp);
	void OnLogout();
	void OnDayChange();
	
	void UpDate(unsigned int now_second);
	void StarChoose();
	void Choose(bool is_true);
	void SendInfo();
	
	void EnterFb();//进入场景
private:
	RoleModuleManager *m_module_mgr;
	
	void EndPlay(int game_type);
	void SendEndPlayInfo(int game_type);

	int today_play_times;
	int pass_idx;
	unsigned int choose_tamp;//选择限制时间
};
#endif
