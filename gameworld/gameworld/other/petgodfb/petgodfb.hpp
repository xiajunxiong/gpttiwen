#ifndef __PET_GOD_FB_HPP__
#define __PET_GOD_FB_HPP__

#include "servercommon/petgodfbdef.hpp"

class RoleModuleManager;
class PetGodFB
{
public:
	PetGodFB();
	~PetGodFB();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *module_mgr, const PetGodFBParam & param);
	void GetInitParam(PetGodFBParam * param);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	//��ɫ����
	void OnReq(int req_type, int param_1);

	//�ɽ��븱��
	bool CanEnterFB(int fb_seq, bool is_auto = false);
	//����
	void OnFBFinish(int fb_seq, bool is_auto = false);

	//�������
	void BuyDayTimes();
	void SendInfo();
private:


	RoleModuleManager * m_module_mgr;

	PetGodFBParam m_param;
};

#endif