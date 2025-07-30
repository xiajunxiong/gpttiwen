#ifndef __SURFACE_HPP__
#define __SURFACE_HPP__

#include "servercommon/surfacedef.hpp"
#include "other/attributelist.hpp"
#include "protocol/msgsurface.h"
#include "item/expense/norexitem.hpp"

class RoleModuleManager;
class Surface
{
public:
	Surface();
	~Surface();
	
	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *role_module_manager, const SurfaceParam &param);
	void GetOtherInitParam(SurfaceParam *param);

	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc);

	void Update(unsigned long interval, time_t now_second);

	int ActivateSurface(int id);
	int WearSurface(int type, int id);
	bool DecomposeSurface(short column, short index, int num);				//背包按钮分解时装
	bool AutoDecomposeActivatedSur(const NorexItem * norex_item, int num, bool is_noitce = true);	//获得道具自动使用,如果已经激活自动分解
	bool IsActiveSurface(int id);

	void GetWearSurfaceList(short wear_surface[SURFACE_TYPE_MAX]);

	int ExchangegItem(int seq);
	int GetActiveCount(int surface_type);

	void SendAllInfo();
	void SendSingleInfo(int seq, int reason = Protocol::SCSurfaceSingleInfo::SURFACE_REASON_TYPE_DEFAULT);
	void SendWearInfo();
private:
	RoleModuleManager *m_role_module_mgr;
	SurfaceParam m_param;
	AttributeList m_attrs_add;
	
	int m_active_surface_list[SURFACE_TYPE_MAX];			//已激活的类型数量列表
	int m_active_count;					
};


#endif // !__SURFACE_HPP__
