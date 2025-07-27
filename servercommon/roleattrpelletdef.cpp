#include "roleattrpelletdef.hpp"

int GetAttrTypeByPelletType(int attr_pellet_type)
{
	if(attr_pellet_type < 0 || attr_pellet_type >= ATTR_PELLET_TYPE_MAX) return -1;

	return ATTR_PELLET_TYPE_LIST[attr_pellet_type];
}

int GetPelletTypeByAttrType(int attr_type)
{
	if(attr_type < 0 || attr_type >= ARRAY_LENGTH(ATTR_BATTLE_TYPE_LIST)) return -1;

	return ATTR_BATTLE_TYPE_LIST[attr_type];
}
