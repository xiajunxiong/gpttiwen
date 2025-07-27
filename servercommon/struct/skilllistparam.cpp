#include "skilllistparam.hpp"
#include "common/tlvprotocol.h"

bool SkillListParam::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < MAX_SKILL_NUM; ++i)
	{
		ret = outstream.Push(skill_list[i].index)
			&& outstream.Push(skill_list[i].change_state)
			&& outstream.Push(skill_list[i].skill_id)
			&& outstream.Push(skill_list[i].skill_level)
			&& outstream.Push(skill_list[i].skill_exp);
		
		if (!ret)
		{
			return false;
		}
	}

	return ret;
}

bool SkillListParam::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < MAX_SKILL_NUM; ++i)
	{
		ret = instream.Pop(&skill_list[i].index)
			&& instream.Pop(&skill_list[i].change_state)
			&& instream.Pop(&skill_list[i].skill_id)
			&& instream.Pop(&skill_list[i].skill_level)
			&& instream.Pop(&skill_list[i].skill_exp);
		
		if (!ret)
		{
			return false;
		}
	}

	return ret;
}