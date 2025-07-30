#ifndef __SVIP_HPP__
#define __SVIP_HPP__

#include <cstddef>
#include "common/memory/mempool.h"

class RoleModuleManager;
class SVip
{
public:
	SVip();
	~SVip();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr);
	void SetClientCustomData(int data_1, int data_2);
	void SendSVipInfo();
private:
	RoleModuleManager* m_mgr;
};

#endif