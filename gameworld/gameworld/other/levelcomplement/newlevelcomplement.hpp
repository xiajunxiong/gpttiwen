#ifndef __NEW_LEVEL_COMPLEMENT_HPP__
#define __NEW_LEVEL_COMPLEMENT_HPP__

#include "other/rolemodulemanager.hpp"
#include "servercommon/levelcomplementdef.hpp"

class RoleNewLevelComplement
{
public:
	RoleNewLevelComplement();
	~RoleNewLevelComplement();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const RoleNewLevelComplementParam & param);
	void GetInitParam(RoleNewLevelComplementParam * param);
	void OnRoleLogin();
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnRoleLevelUp();

	//���ճ�����,��������
	void OnAddExp(long long add_exp, const char * reason);
	//��ȡ�������鵽��ɫ����
	void OnBuyExp();

	void SendInfo();

private:

	//��ȡ�ȼ���ֵ
	int GetLevelDifference();
	//��ȡƽ���ȼ�
	int GetAverageLevel();
	//������������
	void ComplementHelpExp();
	//��⵱ǰ�ۼƾ����Ƿ���Ϲ淶
	void CheckAccExpConform();
	//�Ƿ����㿪���ù���
	bool IsOpen();
	//��ȡ�����±�
	int GetMultipleIndex();

	RoleModuleManager * m_module_mgr;

	RoleNewLevelComplementParam m_param;
};


#endif