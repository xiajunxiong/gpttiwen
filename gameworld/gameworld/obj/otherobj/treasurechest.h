#ifndef __TREASURE_CHEST_OBJ_H__
#define __TREASURE_CHEST_OBJ_H__

#include "obj/obj.h"
class RoleModuleManager;
class Role;

class TreasureChest : public Obj
{
public:
	TreasureChest();
	TreasureChest(int treasure_chest_id);
	virtual ~TreasureChest();

	void Init(int treasure_chest_id);

	void* operator new(size_t c);
	void operator delete(void *m);

	void Open(Role * role);

	bool IsOpen();
	void SetOpen(bool is_open);
	int GetTreasureChestid();

	void SetParam(const int * p1, const int * p2 = NULL, const int * p3 = NULL);
	void GetParam(int * ARG_OUT p1, int * ARG_OUT p2, int * ARG_OUT p3);
private:
	int m_treasure_chest_id;
	bool m_is_open;

	int m_param1;
	int m_param2;
	int m_param3;
};

#endif