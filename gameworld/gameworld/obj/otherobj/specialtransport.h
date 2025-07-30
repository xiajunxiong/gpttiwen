#ifndef __TRANSPORT_OBJ_H__
#define __TRANSPORT_OBJ_H__

#include "obj/obj.h"

class Role;

//send_param1
//�´��ع����������ű�ʾ�ǵ�ǰ����
//ˮ���������������ű�ʾ�ǵ�ǰ����
//���׸������������ű�ʾ�Ǵ��������ĳ���id��ֵΪ0���뿪���������ţ�

struct SpecialTransportExtraInfo
{
	SpecialTransportExtraInfo()
	{
		send_param1 = 0;
		send_param2 = 0;
		param1 = 0;
	}

	//----------------------------ͨ��������ҰЭ���·�----------------------------
	int send_param1;				//��ͬ�淨����ͬ��˼���Ϳͻ���Э���
	int send_param2;				//��ͬ�淨����ͬ��˼���Ϳͻ���Э���

	//----------------------------�����ֶβ����·��ͻ���----------------------------
	int param1;
};

class SpecialTransport : public Obj
{
public:
	SpecialTransport();
	virtual ~SpecialTransport();

	void* operator new(size_t c);
	void operator delete(void *m);

	void Transport(Role * role);
	
	SpecialTransportExtraInfo & GetExtraInfo();
private:
	SpecialTransportExtraInfo m_extra_info;
};

#endif