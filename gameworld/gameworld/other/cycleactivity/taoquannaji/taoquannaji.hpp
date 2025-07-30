#ifndef _TAO_QUAN_NA_JI_HPP_
#define _TAO_QUAN_NA_JI_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/commondata.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/taoquannajidef.hpp"

class RoleModuleManager;
class TaoQuanNaJi
{
public:
	TaoQuanNaJi();
	~TaoQuanNaJi();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* module_mgr,const TaoQuanNaJiParam& param);
	void GetInitParam(TaoQuanNaJiParam * param);
	void Update(unsigned int now_second);
	void OnDayChange();
	void LeaveFb();
	void Begain();
	void TaoQuan(int x, int y);
	void EnterFb();
	void SendInfo();
	void SendPlayInfo();
private:
	void RandMap();
	int MapEffectDisapperSameType(int item_type);//��ͼЧ��-����ͬ��
	int MapEffectDisapperPointAround(int x,int y);//��ͼЧ��-����������ΧһȦ
	bool CheckMapBeClear();//����Ƿ�ȫ�����������

	void EndGame();//��Ϸ����
	RoleModuleManager *m_module_mgr;

	TaoQuanNaJiParam m_param;
	unsigned int later_send_msg_tamp;
};
#endif
