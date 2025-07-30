#ifndef __SUPER_SKILL_BOOK_CONVERT_HPP__
#define __SUPER_SKILL_BOOK_CONVERT_HPP__

#include "servercommon/servercommon.h"
#include "obj/character/role.h"
#include "servercommon/struct/itemlistparam.h"

class SkillBookConvert 
{
public:
	SkillBookConvert();
	~SkillBookConvert();
  
	static bool ConvertSkillBook(Role * role, ItemConfigData book_item_1, ItemConfigData book_item_2);

private:	
};




#endif // !__SUPER_SKILL_BOOK_CONVERT_HPP__
