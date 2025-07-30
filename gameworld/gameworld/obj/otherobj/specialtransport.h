#ifndef __TRANSPORT_OBJ_H__
#define __TRANSPORT_OBJ_H__

#include "obj/obj.h"

class Role;

//send_param1
//勇闯地宫创建传送门表示是当前层数
//水晶副本创建传送门表示是当前层数
//进阶副本创建传送门表示是传送所到的场景id（值为0即离开场景传送门）

struct SpecialTransportExtraInfo
{
	SpecialTransportExtraInfo()
	{
		send_param1 = 0;
		send_param2 = 0;
		param1 = 0;
	}

	//----------------------------通过进入视野协议下发----------------------------
	int send_param1;				//不同玩法代表不同意思，和客户端协议好
	int send_param2;				//不同玩法代表不同意思，和客户端协议好

	//----------------------------下面字段不会下发客户端----------------------------
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