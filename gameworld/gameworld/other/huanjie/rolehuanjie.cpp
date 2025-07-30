#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"
#include "item/money.h"

#include "protocol/msghuanjiezhanchang.h"
#include "global/activity/activityimpl/activityhuanjiezhanchang.hpp"
#include "rolehuanjie.hpp"

RoleHuanJie::RoleHuanJie() :m_role_module_mgr(NULL)
{
}

RoleHuanJie::~RoleHuanJie()
{
}

void RoleHuanJie::Init(RoleModuleManager * mgr, const RoleHuanJieParam & param)
{
	m_role_module_mgr = mgr;
	m_param = param;
}

void RoleHuanJie::GetInitParam(RoleHuanJieParam * param)
{
	*param = m_param;
}

void RoleHuanJie::SendInfo()
{
	Protocol::SCHuanJieZhanChangTianTiRecord protocol;
	protocol.type = Protocol::SC_HUANJIE_ZHAN_CHANG_TIAN_TI_RECORD_TYPE_ALL;
	protocol.count = 0;

	for (int i = 0; i < ARRAY_LENGTH(m_param.tian_ti_record) && i < ARRAY_LENGTH(protocol.record); ++i)
	{
		if (m_param.tian_ti_record[i].IsEmpty())
		{
			continue;
		}

		protocol.count += 1;
		protocol.record[i] = m_param.tian_ti_record[i];
	}

	int send_len = sizeof(protocol) - (ARRAY_LENGTH(protocol.record) - protocol.count) * sizeof(protocol.record[0]);
	m_role_module_mgr->NetSend(&protocol, send_len);
}

void RoleHuanJie::AddTianTiRecord(const CommonSaveHuanJieTianTiReceordData & record_data)
{
	if (m_param.time >= record_data.time)
	{
		return;
	}

	m_param.time = record_data.time;
	m_role_module_mgr->GetMoney()->AddOtherCoin(record_data.add_huan_jie_coin, MONEY_TYPE_HUAN_JIE_ZHAN_CHANG_COIN, __FUNCTION__);

	int index = m_param.tian_ti_record_index % ARRAY_LENGTH(m_param.tian_ti_record);
	m_param.tian_ti_record[index] = record_data;
	m_param.tian_ti_record_index += 1;

	Protocol::SCHuanJieZhanChangTianTiRecord protocol;
	protocol.type = Protocol::SC_HUANJIE_ZHAN_CHANG_TIAN_TI_RECORD_TYPE_ADD;
	protocol.count = 1;
	protocol.record[0] = record_data;

	int send_len = sizeof(protocol) - (ARRAY_LENGTH(protocol.record) - protocol.count) * sizeof(protocol.record[0]);
	m_role_module_mgr->NetSend(&protocol, send_len);
}
