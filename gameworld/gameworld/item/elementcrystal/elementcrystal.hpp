#ifndef __ELEMENT_CRYSTAL_HPP__
#define __ELEMENT_CRYSTAL_HPP__

#include "item/itembase.h"

class Role;
struct ItemGridData;
class ElementCrystal : public ItemBase
{
public:
	ElementCrystal();
	virtual ~ElementCrystal();

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

	short GetElementFire() const { return m_element_fire; }
	short GetElementWater() const { return m_element_water; }
	short GetElementWind() const { return m_element_wind; }
	short GetElementEarth() const { return m_element_earth; }
	short GetCrystalLimitTimes() const { return m_limit_num; }
	short GetElementType() const { return m_element_type; }

	virtual void RandomNetValueOnPutItem(Role* role, NetValueItemParam* param_data, int put_reason) const;
private:
	short m_element_fire;
	short m_element_water;
	short m_element_wind;
	short m_element_earth;
	short m_limit_num;
	short m_element_type;		//‘™Àÿ¿‡–Õ \see ELEMENT_TYPE_MAX
};

#endif 