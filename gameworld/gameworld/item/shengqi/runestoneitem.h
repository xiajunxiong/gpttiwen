#ifndef __RUNE_STONE_ITEM_H__
#define __RUNE_STONE_ITEM_H__

#include "item/itembase.h"

class RuneStoneItem : public ItemBase
{
public:
	enum RUNE_STONE_TYPE		// 符石类型
	{
		RUNE_STONE_TYPE_INVALID,

		RUNE_STONE_TYPE_QING_LONG = 1,	// 青龙石 
		RUNE_STONE_TYPE_BAI_HU = 2,		// 白虎石
		RUNE_STONE_TYPE_ZHU_QUE = 3,	// 朱雀石
		RUNE_STONE_TYPE_XUAN_WU = 4,	// 玄武石
		RUNE_STONE_TYPE_QI_LIN = 5,		// 麒麟石

		RUNE_STONE_TYPE_MAX,
	};

	static const int MAX_RUNE_STONE_LEVEL = 5;	// 符石最高等级

public:
	RuneStoneItem();
	virtual ~RuneStoneItem() {}

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

	virtual void RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const;

	int GetFuShiType() const { return m_runestone_type; }
	int GetFuShiLevel() const { return m_runestone_level; }

protected:

	int m_runestone_type;	// 符石类型
	int m_runestone_level;	// 符石等级
};




#endif
