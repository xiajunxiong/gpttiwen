#include "loginserver/randnamemanager/randnamemanager.hpp"
#include "servercommon/serverdef.h"
#include "loginserver/server/protocal/msglogin.h"
#include "loginserver/server/loginlog.h"
#include "loginserver/engineadapter.h"
#include "servercommon/errornum.h"
#include "servercommon/userprotocal/msgsystem.h"

void RandNameManager::Init(NameStrIdList name_str_id_list)
{
	m_name_str_id_list = name_str_id_list;
}

int RandNameManager::StrIdExist(long long find_str_id)
{
	if (find_str_id < 0)
	{
		return VALUE_ERROR;
	}

	int ret = NOT_EXIST;

	if (m_name_str_id_list.find(find_str_id) != m_name_str_id_list.end())
	{
		ret = EXIST;
	}

	return ret;
}

bool RandNameManager::AddNameStrId(long long add_str_id)
{
	loginlog::g_log_login.printf(LL_ERROR, "[RandNameManager] AddNameStrId called, add_str_id: %lld", add_str_id);

	if (0 > add_str_id)
	{
		loginlog::g_log_login.printf(LL_ERROR, "[RandNameManager] AddNameStrId failed: add_str_id < 0 (%lld)", add_str_id);
		return false;
	}

	// 已经有就不要加了
	if (EXIST == this->StrIdExist(add_str_id))
	{
		loginlog::g_log_login.printf(LL_ERROR, "[RandNameManager] AddNameStrId failed: add_str_id already exists (%lld)", add_str_id);
		return false;
	}

	m_name_str_id_list.insert(add_str_id);
	loginlog::g_log_login.printf(LL_ERROR, "[RandNameManager] AddNameStrId success: add_str_id inserted (%lld)", add_str_id);
	return true;
}


long long RandNameManager::GetRandNameId(int sex, NameStringIdParam* strid_param)
{
	if (NULL == strid_param)
	{
		return -1;
	}

	const NameCfg &name_cfg = NameConfig::Instance()->GetNameCfg();

	// 性别个数加上中性个数
	unsigned int rand_fir_length = (unsigned int)(name_cfg.first_name[sex].name_str_list.size());

	unsigned int rand_last_length = (unsigned int)(name_cfg.last_name[sex].name_str_list.size());

	unsigned int rand_mid_length = (unsigned int)(name_cfg.middle_name[sex].name_str_list.size());

	int loop_count = 0;
	long long name_str_id = 0;
	unsigned int rand_index;

	while (loop_count < 1000)
	{
		if (rand_fir_length > 0)
		{
			// 随机前缀
			rand_index = rand() % rand_fir_length;
			strid_param->front_sex = (short)sex;
			strid_param->front_id = (short)name_cfg.first_name[sex].name_str_list[rand_index].name_id;
			name_str_id += strid_param->front_sex * MAX_NAME_CFG_NUM + strid_param->front_id;	
		}
		else
		{
			strid_param->front_sex = 0;
			strid_param->front_id = 0;
		}

		if (rand_mid_length > 0)
		{
			// 随机中缀
			rand_index = rand() % rand_mid_length;
			strid_param->middle_sex = (short)sex;
			strid_param->middle_id = (short)name_cfg.middle_name[sex].name_str_list[rand_index].name_id;
			name_str_id *= OFFSET_NUM;
			name_str_id += strid_param->middle_sex * MAX_NAME_CFG_NUM + strid_param->middle_id;
		}
		else
		{
			strid_param->middle_sex = 0;
			strid_param->middle_id = 0;
			name_str_id *= OFFSET_NUM;
		}

		if (rand_last_length > 0)
		{
			// 随机后缀
			rand_index = rand() % rand_last_length;
			strid_param->last_sex = (short)sex;
			strid_param->last_id = (short)name_cfg.last_name[sex].name_str_list[rand_index].name_id;
			name_str_id *= OFFSET_NUM;
			name_str_id += strid_param->last_sex * MAX_NAME_CFG_NUM + strid_param->last_id;
		}
		else
		{
			strid_param->last_sex = 0;
			strid_param->last_id = 0;
			name_str_id *= OFFSET_NUM;
		}
		
		if (this->StrIdExist(name_str_id) == NOT_EXIST)
		{
			return name_str_id;
		}

		++ loop_count;
	}

	return 0;
}

void RandNameManager::OnGetRandNameId(NetID netid, int sex)
{
	if (sex < 0 || sex >= SEX_MAX)
	{
		return;
	}

	NameStringIdParam strid_param;
	long long name_str_id = this->GetRandNameId(sex, &strid_param);

	// 不允许为0
	if (0 >= name_str_id)
	{
		loginlog::g_log_login.printf(LL_ERROR,"[RandNameManager::OnGetRandNameId Error] netid: %d,sex: %d, get random name fail cause: no more name to use.: %lld.",
			netid, sex, name_str_id);
		return;
	}

	Protocol::SCRandomNameRet rnr;
	rnr.front_sex = strid_param.front_sex;
	rnr.front_id = strid_param.front_id;
	rnr.middle_sex = strid_param.middle_sex;
	rnr.middle_id = strid_param.middle_id;
	rnr.last_sex = strid_param.last_sex;
	rnr.last_id = strid_param.last_id;
	rnr.name_str_id = name_str_id;

	EngineAdapter::Instance().NetSend(netid, (char*)&rnr, sizeof(rnr));

	loginlog::g_log_login.printf(LL_INFO,"[RandNameManager::OnGetRandNameId Success] netid: %d, sex: %d, name_strid: %lld.",
		netid, sex, name_str_id);

#ifdef _DEBUG
	int ret = this->StrIdExist(name_str_id);
	if (EXIST == ret)
	{
		Protocol::SCNoticeNum scnn;
		scnn.notice_num = errornum::EN_DEBUG_RAND_NAME_EXIST;
		EngineAdapter::Instance().NetSend(netid, (char*)&scnn, sizeof(scnn));
	}
#endif
}
