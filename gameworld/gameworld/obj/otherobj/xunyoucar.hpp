#ifndef __XUN_YOU_CAR_HPP__
#define __XUN_YOU_CAR_HPP__

#include "obj/character/character.h"
#include "servercommon/weddingdef.hpp"
#include "obj/character/npc_gather.h"


class Role;

//���� Ѳ�� ����
class XunYouCar : public Character
{
public:
	//�߼�
	//|->�ȴ�->���ɲɼ���->�ƶ�->|
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

	//����״̬֪ͨ
	short IsStop() { return m_is_stop; };

	//֪ͨ����״̬
	void SyncCarStatusInfo();

	//Ѳ�βɼ����� ��һ���
	static void OnGatherGift(Role *gather, NPCGather * npc_gather);
protected:
	//��������
	void CreatGift(int posi_index);

	long long m_qing_yuan_id;
	int m_xun_you_type;
	//��ǰ·����index
	int m_now_posi_index;
	//�ȴ�����ʱ��
	time_t m_wait_to_move_time;

	short m_is_stop;
};



#endif