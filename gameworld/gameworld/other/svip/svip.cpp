#include "svip.hpp"
#include "servercommon/servercommon.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/msgother.h"

SVip::SVip() : m_mgr(NULL)
{

}

SVip::~SVip()
{

}

void SVip::Init(RoleModuleManager * mgr)
{
	m_mgr = mgr;
}

void SVip::SetClientCustomData(int data_1, int data_2)
{
	m_mgr->GetCommonData().svip_client_custom_data_1 = data_1;
	m_mgr->GetCommonData().svip_client_custom_data_2 = data_2;

	this->SendSVipInfo();
}

void SVip::SendSVipInfo()
{
	Protocol::SCSvipClientCustomData msg;
	msg.data_1 = m_mgr->GetCommonData().svip_client_custom_data_1;
	msg.data_2 = m_mgr->GetCommonData().svip_client_custom_data_2;

	m_mgr->NetSend(&msg, sizeof(msg));
}

