#ifndef __GOD_PRINT_EQUIPMENT_H__
#define __GOD_PRINT_EQUIPMENT_H__

#include "gamedef.h"
#include "item/itembase.h"
#include "item/itemgriddata.h"
#include "servercommon/struct/itemlistparam.h"

// ------------------------------------------------------------------------------------------------------------

class GodPrintEquipment : public ItemBase
{
public:
	enum PET_EQUIPMENT_TYPE		// 宠物装备类型
	{
		PET_EQUIPMENT_TYPE_0 = 0,		// 宠物装备1

		PET_EQUIPMENT_TYPE_MAX
	};
	GodPrintEquipment(short equip_type);
	virtual ~GodPrintEquipment() {}

	virtual bool Init(const char* path, std::string *err);
	virtual void RandomNetValueOnPutItem(Role* role, NetValueItemParam* param_data, int put_reason) const;
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;
	int GetParam1()const { return m_param1; }
	int GetParam2()const { return m_param2; }
	int GetParam3()const { return m_param3; }
protected:

	int m_param1;
	int m_param2;
	int m_param3;
};

#endif