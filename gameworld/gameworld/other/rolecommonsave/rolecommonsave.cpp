#include <algorithm>

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "gamelog.h"
#include "servercommon/struct/structcommon.h"
#include "rolecommonsave.hpp"

RoleCommonSaveKey::RoleCommonSaveKey(int p1, int p2)
{
	param1 = p1;
	param2 = p2;
}

bool RoleCommonSaveKey::operator<(const RoleCommonSaveKey & other) const
{
	if (param1 < other.param1)
		return true;
	if (param1 > other.param1)
		return false;

	if (param2 < other.param2)
		return true;
	if (param2 > other.param2)
		return false;

	return false;
}

RoleCommonSave::RoleCommonSave(int save_type)
{
	m_role_module_mgr = NULL;
	m_save_type = save_type;
	m_is_data_change = false;
}

RoleCommonSave::~RoleCommonSave()
{
}

std::vector<char> RoleCommonSave::ConvertVecData(int save_type, const RoleCommonSaveData & role_data)
{
	std::vector<char> ret;
	unsigned int data_len = ::GetRoleCommonSaveDataLengthByType(save_type);
	unsigned int min_len = std::min(data_len, (unsigned int)sizeof(role_data));

	if (min_len > 0)
	{
		ret.resize(data_len);
		memcpy(&ret[0], &role_data, min_len);
	}

	return ret;
}

RoleCommonSaveData RoleCommonSave::ConvertRoleCommonSaveData(const std::vector<char> & vec_role_data)
{
	RoleCommonSaveData ret;
	ret.Reset(ROLE_COMMON_SAVE_DATA_TYPE_INVALID);
	size_t data_len = vec_role_data.size();
	data_len = std::min(data_len, sizeof(ret));
	if (data_len > 0)
	{
		memcpy(&ret, &vec_role_data[0], data_len);
	}

	return ret;
}

void RoleCommonSave::InitRoleModuleManager(RoleModuleManager * mgr)
{
	m_role_module_mgr = mgr;
}

void RoleCommonSave::InitData(const RoleCommonSaveKey & key, const RoleCommonSaveData & role_data)
{
	RoleCommonSaveDataNode data_node;
	data_node.data = RoleCommonSave::ConvertVecData(m_save_type, role_data);
	MapAddOrUpdate(m_save_data, key, data_node);
}

void RoleCommonSave::SetData(const RoleCommonSaveKey & key, const RoleCommonSaveData & data)
{
	bool is_new_create = true;

	std::map<RoleCommonSaveKey, RoleCommonSaveDataNode>::iterator iter = m_save_data.find(key);
	if (iter != m_save_data.end())
	{
		is_new_create = iter->second.is_new_create;
	}

	RoleCommonSaveDataNode & data_node = m_save_data[key];
	data_node.data = RoleCommonSave::ConvertVecData(m_save_type, data);

	data_node.is_dirty = true;
	data_node.is_invalid = false;
	data_node.is_new_create = is_new_create;
	m_is_data_change = true;
}

std::vector<char> * RoleCommonSave::GetData(const RoleCommonSaveKey & key, bool is_auto_create)
{
	std::vector<char> * ret = NULL;

	std::map<RoleCommonSaveKey, RoleCommonSaveDataNode>::iterator iter = m_save_data.find(key);
	if (iter == m_save_data.end())
	{
		if (is_auto_create)
		{
			m_is_data_change = true;

			RoleCommonSaveData save_data;
			save_data.Reset(m_save_type);

			RoleCommonSaveDataNode data_node;
			data_node.data = RoleCommonSave::ConvertVecData(m_save_type, save_data);
			data_node.is_dirty = true;
			data_node.is_new_create = true;
			std::pair<std::map<RoleCommonSaveKey, RoleCommonSaveDataNode>::iterator, int> ret_iter = m_save_data.insert(std::make_pair(key, data_node));

			ret = &ret_iter.first->second.data;
		}
	}
	else
	{
		if (iter->second.is_invalid == false)
		{
			ret = &iter->second.data;
		}
		else
		{
			if (is_auto_create)
			{
				m_is_data_change = true;

				RoleCommonSaveData save_data;
				save_data.Reset(m_save_type);

				RoleCommonSaveDataNode data_node;
				data_node.data = RoleCommonSave::ConvertVecData(m_save_type, save_data);
				data_node.is_dirty = true;
				data_node.is_new_create = iter->second.is_new_create;
				data_node.is_invalid = false;
				iter->second = data_node;

				ret = &iter->second.data;
			}
		}
	}

#ifdef _DEBUG
	if (is_auto_create)
	{
		assert(ret != NULL);
	}
#endif

	return ret;
}

void RoleCommonSave::SetDataChange(const RoleCommonSaveKey & key)
{
	std::map<RoleCommonSaveKey, RoleCommonSaveDataNode> ::iterator iter = m_save_data.find(key);
	if (iter != m_save_data.end())
	{
		RoleCommonSaveDataNode & data_node = iter->second;
		data_node.is_dirty = true;
		m_is_data_change = true;
	}
}

void RoleCommonSave::ClearAllData()
{
	for (std::map<RoleCommonSaveKey, RoleCommonSaveDataNode>::iterator iter = m_save_data.begin(); iter != m_save_data.end(); ++iter)
	{
		RoleCommonSaveDataNode & data_node = iter->second;
		data_node.is_dirty = true;
		data_node.is_invalid = true;
	}

	m_is_data_change = true;
}

void RoleCommonSave::RemoveData(const RoleCommonSaveKey & key)
{
	std::map<RoleCommonSaveKey, RoleCommonSaveDataNode> ::iterator iter = m_save_data.find(key);
	if (iter != m_save_data.end())
	{
		RoleCommonSaveDataNode & data_node = iter->second;
		data_node.is_dirty = true;
		data_node.is_invalid = true;
		m_is_data_change = true;
	}
}

int RoleCommonSave::GetSaveType()
{
	return m_save_type;
}

void RoleCommonSave::BeginDataForeach()
{
	m_data_iter = m_save_data.begin();
}

bool RoleCommonSave::GetNextData(RetRoleCommonSaveData ARG_OUT & data)
{
	while (m_data_iter != m_save_data.end())
	{
		if (m_data_iter->second.is_invalid)
		{
			++m_data_iter;
			continue;
		}

		data.key = m_data_iter->first;
		data.data = RoleCommonSave::ConvertRoleCommonSaveData(m_data_iter->second.data);
		++m_data_iter;

		return true;
	}

	return false;
}

bool RoleCommonSave::GetLastData(RetRoleCommonSaveData ARG_OUT & data)
{
	for (std::map<RoleCommonSaveKey, RoleCommonSaveDataNode>::reverse_iterator iter = m_save_data.rbegin(); iter != m_save_data.rend(); ++iter)
	{
		if (iter->second.is_invalid)
		{
			continue;
		}

		data.key = iter->first;
		data.data = RoleCommonSave::ConvertRoleCommonSaveData(iter->second.data);

		return true;
	}

	return false;
}

bool RoleCommonSave::GetRangeLastData(const RoleCommonSaveKey & k1, const RoleCommonSaveKey & k2, RetRoleCommonSaveData ARG_OUT & data)
{
	if (m_save_data.key_comp()(k2, k1))
	{
		return false;
	}

	std::map<RoleCommonSaveKey, RoleCommonSaveDataNode>::const_iterator iter1 = m_save_data.lower_bound(k1);
	std::map<RoleCommonSaveKey, RoleCommonSaveDataNode>::const_iterator iter2 = m_save_data.upper_bound(k2);

	if (iter1 == iter2)
	{
		return false;
	}

	--iter1;
	--iter2;

	bool is_get = false;
	while (iter2 != iter1)
	{
		if (iter2->second.is_invalid)
		{
			--iter2;
			continue;
		}

		is_get = true;
		data.key = iter2->first;
		data.data = RoleCommonSave::ConvertRoleCommonSaveData(iter2->second.data);
		break;
	}

	return is_get;
}

std::vector<RetRoleCommonSaveData> RoleCommonSave::GetRangeData(const RoleCommonSaveKey & k1, const RoleCommonSaveKey & k2)
{
	std::vector<RetRoleCommonSaveData> ret;

	if (m_save_data.key_comp()(k2, k1))
	{
		return ret;
	}

	std::map<RoleCommonSaveKey, RoleCommonSaveDataNode>::const_iterator iter1 = m_save_data.lower_bound(k1);
	std::map<RoleCommonSaveKey, RoleCommonSaveDataNode>::const_iterator iter2 = m_save_data.upper_bound(k2);

	for (; iter1 != iter2; ++iter1)
	{
		if (iter1->second.is_invalid)
		{
			continue;
		}
		RetRoleCommonSaveData data;
		data.key = iter1->first;
		data.data = RoleCommonSave::ConvertRoleCommonSaveData(iter1->second.data);
		ret.push_back(data);
	}

	return ret;
}

int RoleCommonSave::GetSaveDataNum() const
{
	int num = 0;
	for (std::map<RoleCommonSaveKey, RoleCommonSaveDataNode>::const_iterator iter = m_save_data.begin(); iter != m_save_data.end(); ++iter)
	{
		if (iter->second.is_invalid)
		{
			continue;
		}

		num += 1;
	}

	return num;
}

void RoleCommonSave::GetChangeDataList(RoleCommonSaveDataParam & ARG_OUT role_data_list, bool is_clear_new_and_invalid_item)
{
	for (std::map<RoleCommonSaveKey, RoleCommonSaveDataNode>::iterator iter = m_save_data.begin(); iter != m_save_data.end();)
	{
		if (role_data_list.count >= ARRAY_LENGTH(role_data_list.data_list))
		{
			gamelog::g_log_role_common_save.printf(LL_INFO, "%s line:%d | role[%d %s] save_type:%d", __FUNCTION__, __LINE__,
							       m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), this->GetSaveType());
			break;
		}

		const RoleCommonSaveDataNode & role_data_node = iter->second;
		const  RoleCommonSaveKey & key = iter->first;
		if (role_data_node.is_dirty)
		{
			RoleCommonSaveDataParam::DBRoleCommonSaveDataItem role_data_item;
			role_data_item.save_data_type = m_save_type;
			role_data_item.param1 = key.param1;
			role_data_item.param2 = key.param2;

			if (role_data_node.is_invalid)
			{
				if (role_data_node.is_new_create)
				{
					if (is_clear_new_and_invalid_item)
					{
						m_save_data.erase(iter++);
					}

					continue;
				}

				role_data_item.change_state = structcommon::CS_DELETE;
				role_data_list.data_list[role_data_list.count++] = role_data_item;
			}
			else
			{
				role_data_item.change_state = !role_data_node.is_new_create ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
				role_data_item.data = RoleCommonSave::ConvertRoleCommonSaveData(role_data_node.data);

				role_data_list.data_list[role_data_list.count++] = role_data_item;
			}
		}

		++iter;
	}
}

void RoleCommonSave::ClearDirtyMark(const RoleCommonSaveDataParam::DBRoleCommonSaveDataItem & data_item)
{
		RoleCommonSaveKey  key(data_item.param1, data_item.param2);
		int change_state = data_item.change_state;
		if (change_state == structcommon::CS_DELETE)
		{
			m_save_data.erase(key);
		}
		else
		{
			RoleCommonSaveDataNode & data_node = m_save_data[key];
			data_node.is_dirty = false;
			data_node.is_new_create = false;
		}
}
