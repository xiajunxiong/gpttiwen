#include "tianming.hpp"
#include "config/logicconfigmanager.hpp"
#include "tianmingconfig.hpp"
#include "gamelog.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/msgtianming.h"
#include "engineadapter.h"


TianMing::TianMing():m_mgr(NULL)
{

}

TianMing::~TianMing()
{

}

void TianMing::Init(RoleModuleManager * mgr, const TianMingParam & param)
{
	m_mgr = mgr;
	m_param = param;
}

void TianMing::GetInitParam(TianMingParam * param)
{
	*param = m_param;
}

int TianMing::GetAttrPer(int _sys_attr_type)
{
	if (_sys_attr_type <= TIAN_MING_ADD_ATTR_SYS_TYPE_INVALID || _sys_attr_type >= TIAN_MING_ADD_ATTR_SYS_TYPE_MAX)
		return 0;

	int sys_level = m_param.sys_level[_sys_attr_type];

	if (sys_level <= 0)	// 0����Ȼû�мӳ�
		return 0;

	const TianMingCfg * cfg = LOGIC_CONFIG->GetTianMingConfig()->GetTianMingCfg(sys_level);
	if (NULL == cfg)
		return 0;

	return cfg->att_per_tol;
}

void TianMing::AddExp(int _exp , const char* _add_reason)
{
	if (_exp <= 0)
	{
		gamelog::g_log_tian_ming.printf(LL_INFO, "TianMing::AddExp[Error_1] user[%d  %s]  _exp:%d  reason:%s", 
			m_mgr->GetRole()->GetUID() , m_mgr->GetRole()->GetName() , _exp , NULL == _add_reason ? "" : _add_reason);
		return;
	}

	int old_level = m_param.level , old_base_exp = m_param.exp;
	int temp_level = m_param.level, temp_exp = m_param.exp, need_add_exp = _exp;

	// ��Ҫ��ȡ��һ���ȼ�����Ϣ , ���������ǵȼ� + 1
	const TianMingCfg * cfg = LOGIC_CONFIG->GetTianMingConfig()->GetTianMingCfg(temp_level + 1);
	if (NULL == cfg)
	{
		// ������������ , ��¼һ����־���������
		gamelog::g_log_tian_ming.printf(LL_INFO, "TianMing::AddExp[Error_2] user[%d  %s]  level:%d  reason:%s",
			m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), m_param.level ,NULL == _add_reason ? "" : _add_reason);
		return;
	}

	int level_exp = cfg->need_exp;
	while (need_add_exp > 0)
	{
		if (temp_exp + need_add_exp >= level_exp)
		{
			need_add_exp -= (level_exp - temp_exp);
			++temp_level;
			temp_exp = 0;

			cfg = LOGIC_CONFIG->GetTianMingConfig()->GetTianMingCfg(temp_level + 1);
			if (NULL == cfg)
			{
				// ������������ , ��¼һ����־���������
				gamelog::g_log_tian_ming.printf(LL_INFO, "TianMing::AddExp[Error_2] user[%d  %s]  level:%d  reason:%s ",
					m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), temp_level, NULL == _add_reason ? "" : _add_reason);
				break;
			}

			level_exp = cfg->need_exp;
		}
		else
		{
			temp_exp += need_add_exp;
			need_add_exp = 0;
		}
	}

	m_param.level = temp_level;
	m_param.exp = temp_exp;

	// ��¼һ����־
	gamelog::g_log_tian_ming.printf(LL_INFO, "TianMing::AddExp user[%d  %s]  add_exp:%d  reason:%s  old_exp:%d  new_exp:%d  old_level:%d  new_level:%d ",
		m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), _exp,NULL == _add_reason ? "" : _add_reason , old_base_exp, m_param.exp, old_level, m_param.level );

	// ������Ϣ���ͻ���
	this->SendInfo();

	// ����������� , ��Ҫ����һ��ս����
	if (old_level != m_param.level)
	{
		// ��Ϊÿ���ӳɵĶ���һ�� , �������һ�������˶���ȼ� , ��Ҫȥѭ������һ��
		for (int i = old_level + 1; i <= m_param.level; ++i)
		{
			this->UpLevel(i);
		}
	}
}

void TianMing::UpLevel(int _level)
{
	if (_level <= 0 || _level >= MAX_TIAN_MING_LEVEL_COUNT)
		return;

	// ��Ҫ��ȡ��Ӧ�ȼ�����Ϣ
	const TianMingCfg * cfg = LOGIC_CONFIG->GetTianMingConfig()->GetTianMingCfg(_level);
	if (NULL == cfg)
	{
		gamelog::g_log_tian_ming.printf(LL_INFO, "TianMing::UpLevel[Error_1] user[%d  %s]  level:%d",
			m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), _level);
		return;
	}

	int sys_type = cfg->att_type;
	// ��Ӧ��ϵͳ�ȼ�����Ϊ��ǰ��Ӧ�ĵȼ�
	int old_level = m_param.sys_level[sys_type];
	m_param.sys_level[sys_type] = _level;

	// ����һ�¶�Ӧϵͳ������
	this->ReCalcAttr(sys_type);

	// ��¼һ����־ ���������
	gamelog::g_log_tian_ming.printf(LL_INFO, "TianMing::UpLevel user[%d  %s]  _level:%d   sys_type:%d  old_level:%d  new_level:%d ",
		m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), _level , sys_type , old_level, m_param.sys_level[sys_type]);
}

void TianMing::SendInfo()
{
	Protocol::SCTianMingInfo info;

	info.level = m_param.level;
	info.exp = m_param.exp;

	UNSTD_STATIC_CHECK(sizeof(info.sys_type_level) == sizeof(m_param.sys_level));
	memcpy(info.sys_type_level, m_param.sys_level, sizeof(info.sys_type_level));

	EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&info, sizeof(info));
}

void TianMing::ReCalcAttr(int _sys_type)
{
	if (_sys_type <= TIAN_MING_ADD_ATTR_SYS_TYPE_INVALID || _sys_type >= TIAN_MING_ADD_ATTR_SYS_TYPE_MAX)
		return;

	switch (_sys_type)
	{
	case TIAN_MING_ADD_ATTR_SYS_TYPE_PEI_SHI_HU_JING:
	case TIAN_MING_ADD_ATTR_SYS_TYPE_PEI_SHI_HUA_DIAN:
	case TIAN_MING_ADD_ATTR_SYS_TYPE_PEI_SHI_JING_DAI:
	case TIAN_MING_ADD_ATTR_SYS_TYPE_PEI_SHI_XIANG_NANG:
	case TIAN_MING_ADD_ATTR_SYS_TYPE_PEI_SHI_XIU_JIAN:
		{
			// ���Ͻ�������ϵͳ , ��������ս�����Ϳ�����
			m_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ACCESSORIES, __FUNCTION__);
		}
		break;
	case TIAN_MING_ADD_ATTR_SYS_TYPE_EQUIP_STRENGTHEN:
	case TIAN_MING_ADD_ATTR_SYS_TYPE_GEM:
	case TIAN_MING_ADD_ATTR_SYS_TYPE_EQUIPMENT:
		{
			// ���Ͻ���װ��ϵͳ , ����װ�����Ժ�ս�����Ϳ�����
			m_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
		}
		break;
	default:
		break;
	}
}

void TianMing::OnLogin()
{
	this->SendInfo();
}

