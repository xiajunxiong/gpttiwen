#ifndef __HOLY_ITEM_H__
#define __HOLY_ITEM_H__

#include "item/itembase.h"

class HolyItem : public ItemBase
{
public:
	enum HOLY_ITEM_TYPE
	{
		HOLY_ITEM_TYPE_ALL = 0,				// �츳ͨ������ʥ��(Ŀǰֻ�����츳����)
		HOLY_ITEM_TYPE_ATTACK = 1,			// ������ʥ��
		HOLY_ITEM_TYPE_HELP = 2,			// ������ʥ��
//		HOLY_ITEM_TYPE_SHOOT_DOWN = 3,		// �䱦��ʥ��

		HOLY_ITEM_TYPE_MAX,
	};

	enum HOLY_ATTR_TYPE		// ʥ����������
	{
		HOLY_ATTR_TYPE_MIN = 0,

		HOLY_ATTR_TYPE_ACTIVENESS = HOLY_ATTR_TYPE_MIN,		//!< ��Ծ��
		HOLY_ATTR_TYPE_ADD_ATTACK = 1,						//!< �˺�����
		HOLY_ATTR_TYPE_SPEED = 2,							//!< �ٶ�
		HOLY_ATTR_TYPE_RECOVER = 3,							//!< �ظ�		
		HOLY_ATTR_TYPE_HELP = 4,							//!< Ԯ��
		HOLY_ATTR_TYPE_FALL = 5,							//!< �䱦
		HOLY_ATTR_TYPE_COPE_FALL = 6,						//!< ���䱦

		HOLY_ATTR_TYPE_MAX,
	};

	static const int MAX_HOLY_ITEM_LEVEL = 120;		// ʥ����ߵȼ�

public:
	HolyItem();
	virtual ~HolyItem() {}

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

	virtual void RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const;

	int GetShengQiType() const { return m_shengqi_type; }
	

protected:

	int m_shengqi_type;		// ʥ������

};


#endif
