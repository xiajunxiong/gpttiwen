#ifndef __MAP_UNLOCK_HPP__
#define __MAP_UNLOCK_HPP__

#include "gamedef.h"
#include "servercommon/mapunlockdef.h"

#include <set>
#include <vector>

class RoleModuleManager;
class MapUnlock				// ��������
{
public:
	MapUnlock();
	~MapUnlock();

	//void* operator new(size_t c);
	//void operator delete(void* m);

	RoleModuleManager * GetRoleModuleManager() { return m_role_module_mgr; }

	void Init(RoleModuleManager * role_module_manager, const MapUnlockParam & map_unlock_data);
	void GetOtherInitParam(MapUnlockParam * map_unlock_data);

	void OnRoleLogin();									// ��ҵ�½ʱ�Ĳ���
	void OnRoleLevelUp();
	bool CheckMapIsUnlock(int scene_id, int *param = NULL);			// ��ⳡ���Ƿ����

	void OnEnterScene(int scene_id);				// ��¼��ҽ��볡������ⵯ��
	bool IsRoleHaveBeenEntered(int scene_id);		// �������Ƿ��������ĳ������

	void OnRolePassEctype(int ectype_type);			// ��¼���ͨ�ظ���
	bool IsRoleHaveBeenPassed(int ectype_type);		// �������Ƿ�ͨ�ع�ĳ������

	void ReqestRemoveActiveIrregularID(int remove_id);

	void GmChangeCheckSwitch(int switch_state);		// �ı����ǵ�״̬

private:
	RoleModuleManager * m_role_module_mgr;
	MapUnlockParam		m_map_unlock_param;

	bool gm_check_switch;		// �����Ϸ��Լ���ǣ����Ϊ false ��ʾ����ⳡ������
	std::set<int> unlock_irregular_id_set;	// ��ǰ�����Ĵ��ͼID
	std::set<int> active_irregular_id_set;	// ��Ҫ���Ŷ����ı��

	bool CheckUnlockByCondition(int unlock_type, int unlock_param);
	void SendSceneActiveID(int active_id);			// �����״ν��볡��ʱ��ʾ����
	void SendIrregularIdList();
	void SendFBTypeFirstEnter();
	void CheckAllSceneUnlock();
};

#endif