#ifndef __HOLY_ITEM_H__
#define __HOLY_ITEM_H__

#include "item/itembase.h"

class HolyItem : public ItemBase
{
public:
	enum HOLY_ITEM_TYPE
	{
		HOLY_ITEM_TYPE_ALL = 0,				// 天赋通用类型圣器(目前只用于天赋技能)
		HOLY_ITEM_TYPE_ATTACK = 1,			// 攻击类圣器
		HOLY_ITEM_TYPE_HELP = 2,			// 辅助类圣器
//		HOLY_ITEM_TYPE_SHOOT_DOWN = 3,		// 落宝类圣器

		HOLY_ITEM_TYPE_MAX,
	};

	enum HOLY_ATTR_TYPE		// 圣器基础属性
	{
		HOLY_ATTR_TYPE_MIN = 0,

		HOLY_ATTR_TYPE_ACTIVENESS = HOLY_ATTR_TYPE_MIN,		//!< 活跃度
		HOLY_ATTR_TYPE_ADD_ATTACK = 1,						//!< 伤害能力
		HOLY_ATTR_TYPE_SPEED = 2,							//!< 速度
		HOLY_ATTR_TYPE_RECOVER = 3,							//!< 回复		
		HOLY_ATTR_TYPE_HELP = 4,							//!< 援助
		HOLY_ATTR_TYPE_FALL = 5,							//!< 落宝
		HOLY_ATTR_TYPE_COPE_FALL = 6,						//!< 抗落宝

		HOLY_ATTR_TYPE_MAX,
	};

	static const int MAX_HOLY_ITEM_LEVEL = 120;		// 圣器最高等级

public:
	HolyItem();
	virtual ~HolyItem() {}

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

	virtual void RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const;

	int GetShengQiType() const { return m_shengqi_type; }
	

protected:

	int m_shengqi_type;		// 圣器类型

};


#endif
