#ifndef __XUN_YOU_CAR_HPP__
#define __XUN_YOU_CAR_HPP__

#include "obj/character/character.h"
#include "servercommon/weddingdef.hpp"
#include "obj/character/npc_gather.h"


class Role;

//婚宴 巡游 花车
class XunYouCar : public Character
{
public:
	//逻辑
	//|->等待->生成采集物->移动->|
	//|<-------------------------|
public:
	XunYouCar();
	virtual ~XunYouCar();

	void *	operator new(size_t c);
	void	operator delete(void *m);

	void Init(long long qing_yuan_id, int xun_you_type);
	virtual void Update(unsigned long interval, time_t now_second, unsigned int now_dayid);

	int GetCarType() { return m_xun_you_type; };
	long long GetQingYuanID() { return m_qing_yuan_id; };

	//花车状态通知
	short IsStop() { return m_is_stop; };

	//通知花车状态
	void SyncCarStatusInfo();

	//巡游采集奖励 放一起吧
	static void OnGatherGift(Role *gather, NPCGather * npc_gather);
protected:
	//创建礼物
	void CreatGift(int posi_index);

	long long m_qing_yuan_id;
	int m_xun_you_type;
	//当前路径点index
	int m_now_posi_index;
	//等待结束时间
	time_t m_wait_to_move_time;

	short m_is_stop;
};



#endif