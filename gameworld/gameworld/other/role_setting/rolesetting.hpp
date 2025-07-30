#ifndef __ROLE_SETTING_HPP__
#define __ROLE_SETTING_HPP__

#include "servercommon/rolesettingdef.hpp"

class Role;
class RoleModuleManager;

class RoleSetting
{
public:
	RoleSetting();
	~RoleSetting();
	
	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * role_module_manager, const RoleSetSystemParam & param);
	void GetOtherInitParam(RoleSetSystemParam *param);
	void OnRoleLogin();

	bool SetRoleSetting(short set_system_type,short param);
	bool SetRoleChatGroupSetting(long long group_id, int value);
	int GetNumberOfScreens() const { return m_param.number_of_screens; }
private:
	void ReserSystemSet() { m_param.Reset(); }
	void SetTotalVolume(short volume);
	void SetMusicVolume(short volume);
	void SetSoundEffects(short volume);
	void SetPictureQuality(short effect_level);
	void SetNumberOfScreens(short player_num);
	void ShowOtherPlayer(bool is_show);
	void ShowPlayerName(bool is_show);
	void ShowOwnSideHp(bool is_show);
	void ShowSystemMSG(bool is_show);
	void SetRefuseTeamRequest(bool is_refuse);
	void SetRefuseStrangerMSG(bool is_refuse);
	void SetAutoResearchTask(bool is_auto);
	void SetNoticeChatGroup(int group_index ,bool is_notice);
	void SetUseJoyStick(bool is_use);
	void SetPowerSave(bool is_save);
	inline bool IsAutoResearchTask() const { return m_param.is_auto_research_task > 0; }
	void SendSystemSetting();
	void SendSystemSettingChatGroup();
	void CheckChatGroupSettingTransfer();
	void SetShieldingFlowers(bool _is_shielding);
	void SetBattleControlMode(int mode);

	RoleSetSystemParam & GetSetSystemParam();

	//Role * m_role;
	RoleModuleManager * m_role_module_mgr;

	RoleSetSystemParam m_param;
};


#endif // !__BABY_HPP__
