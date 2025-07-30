#ifndef __CAPABILITY_HPP__
#define __CAPABILITY_HPP__

#include "servercommon/servercommon.h"
#include "other/attributelist.hpp"

class RoleModuleManager;
class Capability
{
public:
	Capability();
	~Capability();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* module_mgr, int capability, int max_capability, int huashen_capability, int huashen_max_capability);
	void GetCapabilityInitParam(int *capability, int *max_capability, int *huashen_capability, int * huashen_max_capability);
	//����
	void GetAttrListParam(AttributeList & attr_List);
	void CalcRoleCapability();						// ���㱾��ս��

	int GetRoleCapability() { return m_capability; }
	int GetRoleMaxCapability() { return m_max_capability; }
	int GetRoleExtraCapability() { return m_extra_capability; }

	//����
	void GetHuaShenAttrListParam(AttributeList & attr_List);
	void CalcHuaShenCapability();						// ���㻯��ս��

	int GetRoleHuaShenCapability() { return m_huashen_capability; }
	int GetRoleHuaShenMaxCapability() { return m_huashen_max_capability; }

	AttributeList & GetAttributeList(int role_type = ROLE_TYPE_BENZUN);		//��ñ�����������б�
	int CalcCapabityByAttrList(AttributeList & attr_list);

	int GetCalcCapByAttrList(AttributeList & attr_list);					//�ɸ�����ͨ����ʽ��������

	void OnSendMaxChangeInfo();
	int GmCalcCapability() { return this->CalcCapablity(); }
private:
	int CalcCapablity(int role_type = ROLE_TYPE_BENZUN); //����ս��
	int CalcExtraCapability();

	RoleModuleManager *m_module_mgr;

	AttributeList m_attr_list;					//��ұ���������
	AttributeList m_huashen_attr_list;			//��һ���������
	int m_capability;							//��ұ���ǰս��
	int m_max_capability;						//��ұ�����ʷ���ս��
	int m_extra_capability;						//��ҳ�ս���������ս��������⣩��ͨ������ֱ�Ӽӵ�ս����

	int m_huashen_capability;					//��һ���ǰս��
	int m_huashen_max_capability;				//��һ�����ʷ���ս��
};


#endif
