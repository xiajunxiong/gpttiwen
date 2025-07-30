#include "global/rmibackobjdef.h"
#include "internalcomm.h"
#include "engineadapter.h"
#include "gamelog.h"

#include "commonsave.hpp"

CommonSaveKey::CommonSaveKey(int p1, int p2, int p3)
{
	param1 = p1;
	param2 = p2;
	param3 = p3;
}

CommonSaveKey::CommonSaveKey(int p1, long long p2_3)
{
	int temp[2] = { 0 };
	::LongLongToInt(p2_3, temp);

	param1 = p1;
	param2 = temp[0];
	param3 = temp[1];
}

CommonSaveKey::CommonSaveKey(long long p1_2)
{
	int temp[2] = { 0 };
	::LongLongToInt(p1_2, temp);

	param1 = temp[0];
	param2 = temp[1];
	param3 = 0;
}

bool CommonSaveKey::operator<(const CommonSaveKey & other) const
{
	if (param1 < other.param1)
		return true;
	if (param1 > other.param1)
		return false;

	if (param2 < other.param2)
		return true;
	if (param2 > other.param2)
		return false;

	if (param3 < other.param3)
		return true;
	if (param3 > other.param3)
		return false;

	return false;
}

CommonSave::CommonSave(int save_type)
{
	m_save_type = save_type;
	m_next_data_save_time = 0;
	m_is_data_change = false;
}

CommonSave::~CommonSave()
{
}

std::vector<char> CommonSave::ConvertVecData(int save_type, const CommonSaveData & role_data)
{
	std::vector<char> ret;
	unsigned int data_len = ::GetCommonSaveDataLengthByType(save_type);
	unsigned int min_len = std::min(data_len, (unsigned int)sizeof(role_data));

	if (min_len > 0)
	{
		ret.resize(data_len);
		memcpy(&ret[0], &role_data, min_len);
	}

	return ret;
}

CommonSaveData CommonSave::ConvertCommonSaveData(const std::vector<char> & vec_role_data)
{
	CommonSaveData ret;
	ret.Reset(COMMON_SAVE_DATA_TYPE_INVALID);
	size_t data_len = vec_role_data.size();
	data_len = std::min(data_len, sizeof(ret));
	if (data_len > 0)
	{
		memcpy(&ret, &vec_role_data[0], data_len);
	}

	return ret;
}

void CommonSave::Update(unsigned long interval, time_t now_second)
{
	if (now_second > m_next_data_save_time)
	{
		m_next_data_save_time = now_second + 60;
		this->Save();
	}
}

void CommonSave::InitData(const CommonSaveKey & key, const CommonSaveData & save_data)
{
	CommonSaveDataNode data_node;
	data_node.data = CommonSave::ConvertVecData(m_save_type, save_data);
	MapAddOrUpdate(m_save_data, key, data_node);
}

void CommonSave::SetData(const CommonSaveKey & key, const CommonSaveData & data)
{
	bool is_new_create = true;

	std::map<CommonSaveKey, CommonSaveDataNode>::iterator iter = m_save_data.find(key);
	if (iter != m_save_data.end())
	{
		is_new_create = iter->second.is_new_create;
	}

	CommonSaveDataNode & data_node = m_save_data[key];
	data_node.data = CommonSave::ConvertVecData(m_save_type, data);

	data_node.is_dirty = true;
	data_node.is_invalid = false;
	data_node.is_new_create = is_new_create;
	m_is_data_change = true;
}

void CommonSave::Save(bool is_stop_server)
{
	if (m_is_data_change)
	{
		int loop_count = 0;
		while (loop_count++ < 150)
		{
			CommonSaveDataParam save_data_param;
			save_data_param.count = 0;
			this->GetChangeDataList(save_data_param);

			if (save_data_param.count > 0)
			{
				gamelog::g_log_serious_error.printf(LL_WARNING, "%s line:%d save_type:%d count:%d",
					__FUNCTION__, __LINE__, this->GetSaveType(), save_data_param.count);


				RMISaveCommonDataBackObjectImpl * impl = new RMISaveCommonDataBackObjectImpl();
				RMIGlobalClient ac;
				ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
				if (ac.SaveCommonDataAsyn(save_data_param, impl))
				{
					if (save_data_param.count < ARRAY_LENGTH(save_data_param.data_list))
					{
						m_is_data_change = false;
					}

					this->ClearChangeFlag(save_data_param);
				}
			}
			else
			{
				m_is_data_change = false;
			}

			if (is_stop_server == false || m_is_data_change == false)		//关服要保存所有数据，否则一次最多保存100条
			{
				break;
			}
		}
	}
}

std::vector<char> * CommonSave::GetData(const CommonSaveKey & key, bool is_auto_create)
{
	std::vector<char> * ret = NULL;

	std::map<CommonSaveKey, CommonSaveDataNode>::iterator iter = m_save_data.find(key);
	if (iter == m_save_data.end())
	{
		if (is_auto_create)
		{
			m_is_data_change = true;

			CommonSaveData save_data;
			save_data.Reset(m_save_type);

			CommonSaveDataNode data_node;
			data_node.data = CommonSave::ConvertVecData(m_save_type, save_data);
			data_node.is_dirty = true;
			data_node.is_new_create = true;
			std::pair<std::map<CommonSaveKey, CommonSaveDataNode>::iterator, int> ret_iter = m_save_data.insert(std::make_pair(key, data_node));

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

				CommonSaveData save_data;
				save_data.Reset(m_save_type);

				CommonSaveDataNode data_node;
				data_node.data = CommonSave::ConvertVecData(m_save_type, save_data);
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

void CommonSave::SetDataChange(const CommonSaveKey & key)
{
	std::map<CommonSaveKey, CommonSaveDataNode> ::iterator iter = m_save_data.find(key);
	if (iter != m_save_data.end())
	{
		CommonSaveDataNode & data_node = iter->second;
		data_node.is_dirty = true;
		m_is_data_change = true;
	}
}

void CommonSave::ClearAllData()
{
	for (std::map<CommonSaveKey, CommonSaveDataNode>::iterator iter = m_save_data.begin(); iter != m_save_data.end(); ++iter)
	{
		CommonSaveDataNode & data_node = iter->second;
		data_node.is_dirty = true;
		data_node.is_invalid = true;
	}

	m_is_data_change = true;
}

void CommonSave::RemoveData(const CommonSaveKey & key)
{
	std::map<CommonSaveKey, CommonSaveDataNode> ::iterator iter = m_save_data.find(key);
	if (iter != m_save_data.end())
	{
		CommonSaveDataNode & data_node = iter->second;
		data_node.is_dirty = true;
		data_node.is_invalid = true;
		m_is_data_change = true;
	}
}

int CommonSave::GetSaveType()
{
	return m_save_type;
}

void CommonSave::BeginDataForeach()
{
	m_data_iter = m_save_data.begin();
}

bool CommonSave::GetNextData(RetCommonSaveData ARG_OUT & data)
{
	while (m_data_iter != m_save_data.end())
	{
		if (m_data_iter->second.is_invalid)
		{
			++m_data_iter;
			continue;
		}

		data.key = m_data_iter->first;
		data.data = CommonSave::ConvertCommonSaveData(m_data_iter->second.data);
		++m_data_iter;

		return true;
	}

	return false;
}

bool CommonSave::GetLastData(RetCommonSaveData ARG_OUT & data)
{
	for (std::map<CommonSaveKey, CommonSaveDataNode>::reverse_iterator iter = m_save_data.rbegin(); iter != m_save_data.rend(); ++iter)
	{
		if (iter->second.is_invalid)
		{
			continue;
		}

		data.key = iter->first;
		data.data = CommonSave::ConvertCommonSaveData(iter->second.data);

		return true;
	}

	return false;
}

bool CommonSave::GetRangeLastData(const CommonSaveKey & k1, const CommonSaveKey & k2, RetCommonSaveData ARG_OUT & data)
{
	if (m_save_data.key_comp()(k2, k1))
	{		
		return false;
	}

	std::map<CommonSaveKey, CommonSaveDataNode>::const_iterator iter1 = m_save_data.lower_bound(k1);
	std::map<CommonSaveKey, CommonSaveDataNode>::const_iterator iter2 = m_save_data.upper_bound(k2);

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
		data.data = CommonSave::ConvertCommonSaveData(iter2->second.data);
		break;
	}

	return is_get;
}

std::vector<RetCommonSaveData> CommonSave::GetRangeData(const CommonSaveKey & k1, const CommonSaveKey & k2)
{
	std::vector<RetCommonSaveData> ret;

	if (m_save_data.key_comp()(k2, k1))
	{
		return ret;
	}

	std::map<CommonSaveKey, CommonSaveDataNode>::const_iterator iter1 = m_save_data.lower_bound(k1);
	std::map<CommonSaveKey, CommonSaveDataNode>::const_iterator iter2 = m_save_data.upper_bound(k2);

	for (; iter1 != iter2; ++iter1)
	{
		if (iter1->second.is_invalid)
		{
			continue;
		}
		RetCommonSaveData data;
		data.key = iter1->first;
		data.data = CommonSave::ConvertCommonSaveData(iter1->second.data);
		ret.push_back(data);
	}

	return ret;
}

std::vector<RetCommonSaveData> CommonSave::GetAllData()
{
	std::vector<RetCommonSaveData> ret;

	RetCommonSaveData get_data;
	this->BeginDataForeach();
	while (this->GetNextData(get_data))
	{
		ret.push_back(get_data);
	}

	return ret;
}

void CommonSave::GetChangeDataList(CommonSaveDataParam & ARG_OUT role_data_list, bool is_clear_new_and_invalid_item)
{
	role_data_list.count = 0;

	for (std::map<CommonSaveKey, CommonSaveDataNode>::iterator iter = m_save_data.begin(); iter != m_save_data.end();)
	{
		if (role_data_list.count >= ARRAY_LENGTH(role_data_list.data_list))
		{
			break;
		}

		const CommonSaveDataNode & role_data_node = iter->second;
		const  CommonSaveKey & key = iter->first;
		if (role_data_node.is_dirty)
		{
			CommonSaveDataParam::DBCommonSaveDataItem role_data_item;
			role_data_item.save_data_type = m_save_type;
			role_data_item.param1 = key.param1;
			role_data_item.param2 = key.param2;
			role_data_item.param3 = key.param3;

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
				role_data_item.data = CommonSave::ConvertCommonSaveData(role_data_node.data);

				role_data_list.data_list[role_data_list.count++] = role_data_item;
			}
		}

		++iter;
	}
}

void CommonSave::ClearChangeFlag(const CommonSaveDataParam & data_param)
{
	for (int i = 0; i < data_param.count && i < ARRAY_LENGTH(data_param.data_list); ++i)
	{
		const CommonSaveDataParam::DBCommonSaveDataItem & curr = data_param.data_list[i];
		CommonSaveKey  key(curr.param1, curr.param2, curr.param3);
		int change_state = curr.change_state;
		if (change_state == structcommon::CS_DELETE)
		{
			m_save_data.erase(key);
		}
		else
		{
			CommonSaveDataNode & data_node = m_save_data[key];
			data_node.is_dirty = false;
			data_node.is_new_create = false;
		}
	}
}


void RMISaveCommonDataBackObjectImpl::SaveDataRet(int ret)
{
	if (0 != ret)
	{
		printf("%s Failed ret:%d\n", __FUNCTION__, ret);
	}
}