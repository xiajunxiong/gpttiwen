#include "advancecareer.hpp"

#include "engineadapter.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "scene/scene.h"
#include "scene/scenemanager.hpp"
#include "scene/speciallogic.hpp"
#include "other/funopen/funopen.hpp"
#include "other/capability/capability.hpp"
#include "servercommon/errornum.h"
#include "task/taskrecorder.h"
#include "item/money.h"
#include "other/prestige/prestige.hpp"
#include "servercommon/serverconfig/appearanceconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "world.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "internalcomm.h"
#include "other/rolecross/rolecross.hpp"
#include "global/cross/crossmanager.h"
#include "battle/battle_manager_local.hpp"
#include "gamelog.h"

#include "other/realmbreak/realmbreak.hpp"
#include "other/dreamnoteschallenge/dreamnoteschallenge.hpp"
#include "other/couragechallenge/couragechallenge.h"
#include "other/lunhuiwangchuan/lunhuiwangchuan.hpp"
#include "other/dujie/dujie.hpp"
#include "other/endlesstower/endlesstower.hpp"
#include "other/event/eventhandler.hpp"
#include "servercommon/noticenum.h"
#include "other/title/title.hpp"
#include "other/rolefirstkill/rolefirstkill.hpp"
#include "other/yaoxingshilian/yaoxingshilian.hpp"
#include "other/mirageterritory/mirageterritory.hpp"
#include "other/hongmengtianjie/rolehongmengtianjie.hpp"
#include "other/couragechallenge/newcouragechallenge.hpp"
#include "other/netherworldpurgatory/netherworldpurgatory.hpp"

AdvanceCareer::AdvanceCareer()
	:m_role_module_mgr(NULL)
{

}

AdvanceCareer::~AdvanceCareer()
{

}

void AdvanceCareer::Init(RoleModuleManager * _role_module_manager, const AdvanceCareerParam & _param)
{
	m_role_module_mgr = _role_module_manager;
	m_param = _param;
}

void AdvanceCareer::GetOtherInitParam(AdvanceCareerParam * _param)
{
	*_param = m_param;
}

void AdvanceCareer::OnLogin()
{
	this->SendSubInfo();
	this->InitHistoryData();
}

bool AdvanceCareer::RoleAdvanceCareer(int to_profession, int to_color, bool is_gm)
{
	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role)
		return false;

	if (!role->GetScene()->GetSpecialLogic()->CanChangeProf(role))
	{
		return false;
	}

	if (!FunOpen::IsFunOpen(role, FUN_TYPE_PROF_ADVANCE, true))
	{
		return false;
	}
	
	// 需要先判断是晋级小阶段 还是大阶段
	int stage_size = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceStageSize(role->GetProfession());
	if (-1 == stage_size)
	{
		role->NoticeNum(errornum::EN_CARRER_ADVANCE_UNKNOWN_PROF);
	}
	bool is_advance_sub = false; 
	if (m_param.sub_level + 1 < stage_size) // 分段从 0开始 配置数量从1开始
	{
		is_advance_sub = true;
	}

	bool ret = false;
	if (is_advance_sub)
	{
		ret = this->RoleAdvanceCareerSub(is_gm);
	}
	else
	{
		ret = this->RoleAdvanceCareerMain(to_profession , to_color , is_gm);
	}

	return ret;
}

void AdvanceCareer::CheckAdvanceCond(int _cond_type, int _param1, int _param2)
{
	if (_cond_type < ADVANCE_COND_TYPE_NO || _cond_type >= ADVANCE_COND_TYPE_MAX )
		return;

	switch (_cond_type)
	{
		// 1. p1: == 
	case ADVANCE_COND_TYPE_MISSION:
	case ADVANCE_COND_TYPE_SHAN_HAI_BOSS:
	case ADVANCE_COND_TYPE_CHALLENGE_MONSTER_SUCC:
	case ADVANCE_COND_TYPE_HONG_MENG_TIAN_JIE:
		{
			this->AdvanceCondChangeHelpType1(_cond_type, _param1);
		}
		break;
		// 2. p1: >=
	case ADVANCE_COND_TYPE_XIN_LING_ZHI_JING:
	case ADVANCE_COND_TYPE_MENG_YUAN:
	case ADVANCE_COND_TYPE_GU_MO_ZHI_TA:
	case ADVANCE_COND_TYPE_LUN_HUI_WANG_CHUAN:
	case ADVANCE_COND_TYPE_DU_JIE:
	case ADVANCE_COND_TYPE_WU_JIN_ZHI_TA:
	case ADVANCE_COND_TYPE_SHANG_HAI_BOSS_SUCC:
	case ADVANCE_COND_TYPE_BIG_DIPPER_SUCC:
	case ADVANCE_COND_TYPE_YAO_XING_SHI_LIAN:
	case ADVANCE_COND_TYPE_SHI_SHEN_ZHI_TA:
	case ADVANCE_COND_TYPE_MIRAGE_TERRITORY:
		{
			this->AdvanceCondChangeHelpType2(_cond_type, _param1);
		}
		break;
		// 3. p1: ==  p2 : ==
	case ADVANCE_COND_TYPE_YUAN_ZHENG_BOSS:
		{
			this->AdvanceCondChangeHelpType3(_cond_type, _param1, _param2);
		}
		break;
		// 4. 满足条件1.p1: >  2.p1: ==  p2: >=
	case ADVANCE_COND_TYPE_NETHER_WORLD_PURGATORY:
		{
			this->AdvanceCondChangeHelpType4(_cond_type, _param1, _param2);
		}
		break;
	default:
		break;
	}
}

void AdvanceCareer::OnChangeColor(int _to_color)
{
	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role)
		return;

	if (role->GetColor() == _to_color)
	{
		return;
	}

	// 获取对应的配置
	const CareerAdvanceCfg* cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(role->GetProfession(), m_param.sub_level);
	if (NULL == cfg)
	{
		role->NoticeNum(errornum::EN_CARRER_ADVANCE_UNKNOWN_PROF);
		return ;
	}

	// 验证消耗
	if (cfg->change_color_coin > 0)
	{
		if (!m_role_module_mgr->GetMoney()->MoneyTypeMoreThan(MONEY_TYPE_COIN_BIND, cfg->change_color_coin))
		{
			role->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
	}

	int has_num = m_role_module_mgr->GetKnapsack()->Count(cfg->change_item_id);
	if (has_num < cfg->change_item_num)
	{
		role->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	// 消耗铜币
	if (cfg->change_color_coin > 0)
	{
		if (!m_role_module_mgr->GetMoney()->UseCoinBind(cfg->change_color_coin ,__FUNCTION__))
		{
			role->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
	}

	// 消耗道具
	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->change_item_id, cfg->change_item_num, __FUNCTION__))
	{
		role->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return;
	}

	// 生效
	int old_color = role->GetColor();
	role->SetColor(static_cast<char>(_to_color));
	m_role_module_mgr->MakeAppearanceData();
	role->SendChangeAppearanceNotice();
	role->NoticeNum(noticenum::NT_ROLE_CHANGE_COLOR_SUCC);

	// 日志
	gamelog::g_log_advance_career.printf(LL_INFO, "%s user[%d %s] "
		"old_color%d , _to_color:%d  prof:%d  sub_level:%d",
		__FUNCTION__ ,role->GetUID(), role->GetName(),
		old_color, _to_color, role->GetProfession() , m_param.sub_level);
}

int AdvanceCareer::GetSubLevel()
{
	return m_param.sub_level;
}

int AdvanceCareer::GetMainLevel()
{
	return m_role_module_mgr->GetRole()->GetAdvanceTimes();
}

void AdvanceCareer::SendSubInfo()
{
	Protocol::SCAdvanceCareerSubInfo info;

	info.sub_level = m_param.sub_level;
	int prof_index = m_role_module_mgr->GetRole()->GetProfession() % PROFESSION_BASE;
	info.is_mission_complete = 0 == (m_param.cond_complete_flag[prof_index] & LL_ONE_BIT << m_param.sub_level) ? 0 : 1;
	info.reserve_ch = 0;
	info.reserve_sh = 0;

	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*)&info, sizeof(info));
}

void AdvanceCareer::RoleLoginFightMonsterTypeCheck()
{
	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role)
		return;

	const std::vector< CareerAdvanceCfg> * _cond_cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfgByCondType(ADVANCE_COND_TYPE_CHALLENGE_MONSTER_SUCC);
	if (NULL == _cond_cfg)
		return;

	std::vector< CareerAdvanceCfg>::const_iterator it_begin = _cond_cfg->begin();
	std::vector< CareerAdvanceCfg>::const_iterator it_end = _cond_cfg->end();

	for (; it_begin != it_end; ++it_begin)
	{
		if (role->GetProfession() / PROFESSION_BASE != it_begin->profession_id / PROFESSION_BASE)
			continue;

		const int prof_index = it_begin->profession_id % PROFESSION_BASE;
		const int sub_level = it_begin->order;

		if (m_param.cond_complete_flag[prof_index] & LL_ONE_BIT << sub_level)
			continue;

		if (!m_role_module_mgr->GetRoleFirstKill()->HasKillMonsterGroup(it_begin->cond_param1))
		{
			continue;
		}

		int old_flag = m_param.cond_complete_flag[prof_index];
		m_param.cond_complete_flag[prof_index] |= LL_ONE_BIT << sub_level;

		{
			// 日志
			gamelog::g_log_advance_career.printf(LL_INFO, "AdvanceCareer::RoleLoginFightMonsterTypeCheck[SUCC] user[%d %s] "
				"cond_type[%d] cond_param1[%d]  old_flag[%d]  new_flag[%d] "
				" prof[%d] sub_level[%d]  role_curr_prof[%d]  role_curr_sub_level[%d]",
				role->GetUID(), role->GetName(),
				ADVANCE_COND_TYPE_CHALLENGE_MONSTER_SUCC, it_begin->cond_param1, old_flag, m_param.cond_complete_flag[prof_index],
				it_begin->profession_id, sub_level, role->GetProfession(), m_param.sub_level);
		}

		// 如果是当前阶段的变更 , 就发消息通知客户端
		if (it_begin->profession_id == role->GetProfession()
			&& sub_level == m_param.sub_level)
		{
			this->SendSubInfo();
		}
	}
}

void AdvanceCareer::RoleLoginHongMengTianJieTypeCheck()
{
	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role)
		return;

	const std::vector< CareerAdvanceCfg> * _cond_cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfgByCondType(ADVANCE_COND_TYPE_HONG_MENG_TIAN_JIE);
	if (NULL == _cond_cfg)
		return;

	std::vector< CareerAdvanceCfg>::const_iterator it_begin = _cond_cfg->begin();
	std::vector< CareerAdvanceCfg>::const_iterator it_end = _cond_cfg->end();

	for (; it_begin != it_end; ++it_begin)
	{
		if (role->GetProfession() / PROFESSION_BASE != it_begin->profession_id / PROFESSION_BASE)
			continue;

		const int prof_index = it_begin->profession_id % PROFESSION_BASE;
		const int sub_level = it_begin->order;

		if (m_param.cond_complete_flag[prof_index] & LL_ONE_BIT << sub_level)
			continue;

		if (!m_role_module_mgr->GetRoleHongMengTianJie()->IsFBPass(it_begin->cond_param1))
		{
			continue;
		}

		int old_flag = m_param.cond_complete_flag[prof_index];
		m_param.cond_complete_flag[prof_index] |= LL_ONE_BIT << sub_level;

		{
			// 日志
			gamelog::g_log_advance_career.printf(LL_INFO, "AdvanceCareer::RoleLoginFightMonsterTypeCheck[SUCC] user[%d %s] "
				"cond_type[%d] cond_param1[%d]  old_flag[%d]  new_flag[%d] "
				" prof[%d] sub_level[%d]  role_curr_prof[%d]  role_curr_sub_level[%d]",
				role->GetUID(), role->GetName(),
				ADVANCE_COND_TYPE_HONG_MENG_TIAN_JIE, it_begin->cond_param1, old_flag, m_param.cond_complete_flag[prof_index],
				it_begin->profession_id, sub_level, role->GetProfession(), m_param.sub_level);
		}

		// 如果是当前阶段的变更 , 就发消息通知客户端
		if (it_begin->profession_id == role->GetProfession()
			&& sub_level == m_param.sub_level)
		{
			this->SendSubInfo();
		}
	}
}

void AdvanceCareer::AdvanceCondChangeHelpType1(int _cond_type, int _param_1)
{
	if (_cond_type >= ADVANCE_COND_TYPE_MAX || _cond_type < ADVANCE_COND_TYPE_NO )
	{
		return;
	}

	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role)
		return;

	const std::vector< CareerAdvanceCfg> * _cond_cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfgByCondType(_cond_type);
	if (NULL == _cond_cfg) 
		return;

	std::vector< CareerAdvanceCfg>::const_iterator it_begin = _cond_cfg->begin();
	std::vector< CareerAdvanceCfg>::const_iterator it_end = _cond_cfg->end();

	for (; it_begin != it_end; ++it_begin)
	{
		if(role->GetProfession() / PROFESSION_BASE != it_begin->profession_id / PROFESSION_BASE)
			continue;

		const int prof_index = it_begin->profession_id % PROFESSION_BASE;
		const int sub_level = it_begin->order;

		if (m_param.cond_complete_flag[prof_index] & LL_ONE_BIT << sub_level)
			continue;
		
		if( it_begin->cond_param1 != _param_1)
			continue;

		int old_flag = m_param.cond_complete_flag[prof_index];
		m_param.cond_complete_flag[prof_index] |= LL_ONE_BIT << sub_level;

		{
			// 日志
			gamelog::g_log_advance_career.printf(LL_INFO, "AdvanceCareer::AdvanceCondChangeHelpType1[SUCC] user[%d %s] "
				"cond_type[%d]  param1[%d]  old_flag[%d]  new_flag[%d] "
				" prof[%d] sub_level[%d]  role_curr_prof[%d]  role_curr_sub_level[%d]",
				role->GetUID(), role->GetName(),
				_cond_type, _param_1, old_flag, m_param.cond_complete_flag[prof_index],
				it_begin->profession_id, sub_level, role->GetProfession(), m_param.sub_level);
		}

		// 如果是当前阶段的变更 , 就发消息通知客户端
		if (it_begin->profession_id == role->GetProfession()
			&& sub_level == m_param.sub_level)
		{
			this->SendSubInfo();
		}
	}
}

void AdvanceCareer::AdvanceCondChangeHelpType2(int _cond_type, int _param_1)
{
	if (_cond_type >= ADVANCE_COND_TYPE_MAX || _cond_type < ADVANCE_COND_TYPE_NO)
	{
		return;
	}

	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role)
		return;

	const std::vector< CareerAdvanceCfg> * _cond_cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfgByCondType(_cond_type);
	if (NULL == _cond_cfg)
		return;

	std::vector< CareerAdvanceCfg>::const_iterator it_begin = _cond_cfg->begin();
	std::vector< CareerAdvanceCfg>::const_iterator it_end = _cond_cfg->end();

	for (; it_begin != it_end; ++it_begin)
	{
		if (role->GetProfession() / PROFESSION_BASE != it_begin->profession_id / PROFESSION_BASE)
			continue;

		const int prof_index = it_begin->profession_id % PROFESSION_BASE;
		const int sub_level = it_begin->order;

		if (m_param.cond_complete_flag[prof_index] & LL_ONE_BIT << sub_level)
			continue;

		if (it_begin->cond_param1 > _param_1)
			continue;

		int old_flag = m_param.cond_complete_flag[prof_index];
		m_param.cond_complete_flag[prof_index] |= LL_ONE_BIT << sub_level;

		{
			// 日志
			gamelog::g_log_advance_career.printf(LL_INFO, "AdvanceCareer::AdvanceCondChangeHelpType2[SUCC] user[%d %s] "
				"cond_type[%d]  param1[%d]  old_flag[%d]  new_flag[%d] "
				" prof[%d] sub_level[%d]  role_curr_prof[%d]  role_curr_sub_level[%d]",
				role->GetUID(), role->GetName(),
				_cond_type, _param_1, old_flag, m_param.cond_complete_flag[prof_index],
				it_begin->profession_id, sub_level, role->GetProfession(), m_param.sub_level);
		}

		// 如果是当前阶段的变更 , 就发消息通知客户端
		if (it_begin->profession_id == role->GetProfession()
			&& sub_level == m_param.sub_level)
		{
			this->SendSubInfo();
		}
	}
}

void AdvanceCareer::AdvanceCondChangeHelpType3(int _cond_type, int _param_1, int _param_2)
{
	if (_cond_type >= ADVANCE_COND_TYPE_MAX || _cond_type < ADVANCE_COND_TYPE_NO)
	{
		return;
	}

	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role)
		return;

	const std::vector< CareerAdvanceCfg> * _cond_cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfgByCondType(_cond_type);
	if (NULL == _cond_cfg)
		return;

	std::vector< CareerAdvanceCfg>::const_iterator it_begin = _cond_cfg->begin();
	std::vector< CareerAdvanceCfg>::const_iterator it_end = _cond_cfg->end();

	for (; it_begin != it_end; ++it_begin)
	{
		if (role->GetProfession() / PROFESSION_BASE != it_begin->profession_id / PROFESSION_BASE)
			continue;

		const int prof_index = it_begin->profession_id % PROFESSION_BASE;
		const int sub_level = it_begin->order;

		if (m_param.cond_complete_flag[prof_index] & LL_ONE_BIT << sub_level)
			continue;

		if (it_begin->cond_param1 != _param_1 || it_begin->cond_param2 != _param_2)
			continue;

		int old_flag = m_param.cond_complete_flag[prof_index];
		m_param.cond_complete_flag[prof_index] |= LL_ONE_BIT << sub_level;

		{
			// 日志
			gamelog::g_log_advance_career.printf(LL_INFO, "AdvanceCareer::AdvanceCondChangeHelpType3[SUCC] user[%d %s] "
				"cond_type[%d]  param1[%d] param2[%d] old_flag[%d]  new_flag[%d] "
				" prof[%d] sub_level[%d]  role_curr_prof[%d]  role_curr_sub_level[%d]",
				role->GetUID(), role->GetName(),
				_cond_type, _param_1, _param_2, old_flag, m_param.cond_complete_flag[prof_index],
				it_begin->profession_id, sub_level, role->GetProfession(), m_param.sub_level);
		}

		// 如果是当前阶段的变更 , 就发消息通知客户端
		if (it_begin->profession_id == role->GetProfession()
			&& sub_level == m_param.sub_level)
		{
			this->SendSubInfo();
		}
	}
}

void AdvanceCareer::AdvanceCondChangeHelpType4(int _cond_type, int _param_1, int _param_2)
{
	if (_cond_type >= ADVANCE_COND_TYPE_MAX || _cond_type < ADVANCE_COND_TYPE_NO)
	{
		return;
	}

	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role)
		return;

	const std::vector< CareerAdvanceCfg> * _cond_cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfgByCondType(_cond_type);
	if (NULL == _cond_cfg)
		return;

	std::vector< CareerAdvanceCfg>::const_iterator it_begin = _cond_cfg->begin();
	std::vector< CareerAdvanceCfg>::const_iterator it_end = _cond_cfg->end();

	for (; it_begin != it_end; ++it_begin)
	{
		if (role->GetProfession() / PROFESSION_BASE != it_begin->profession_id / PROFESSION_BASE)
			continue;

		const int prof_index = it_begin->profession_id % PROFESSION_BASE;
		const int sub_level = it_begin->order;

		if (m_param.cond_complete_flag[prof_index] & LL_ONE_BIT << sub_level)
			continue;

		if (it_begin->cond_param1 > _param_1)
		{
			continue;
		}
		else if (it_begin->cond_param1 == _param_1 && it_begin->cond_param2 > _param_2)
		{
			continue;
		}

		int old_flag = m_param.cond_complete_flag[prof_index];
		m_param.cond_complete_flag[prof_index] |= LL_ONE_BIT << sub_level;

		{
			// 日志
			gamelog::g_log_advance_career.printf(LL_INFO, "AdvanceCareer::AdvanceCondChangeHelpType4[SUCC] user[%d %s] "
				"cond_type[%d]  param1[%d] param2[%d] old_flag[%d]  new_flag[%d] "
				" prof[%d] sub_level[%d]  role_curr_prof[%d]  role_curr_sub_level[%d]",
				role->GetUID(), role->GetName(),
				_cond_type, _param_1, _param_2, old_flag, m_param.cond_complete_flag[prof_index],
				it_begin->profession_id, sub_level, role->GetProfession(), m_param.sub_level);
		}

		// 如果是当前阶段的变更 , 就发消息通知客户端
		if (it_begin->profession_id == role->GetProfession()
			&& sub_level == m_param.sub_level)
		{
			this->SendSubInfo();
		}
	}
}

bool AdvanceCareer::RoleAdvanceCareerSub(bool _is_gm)
{
	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role)
		return false;

	// 小阶段
	// 验证 看一下是否有下一个阶段
	const CareerAdvanceCfg* next_cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(role->GetProfession(), m_param.sub_level + 1);
	if (NULL == next_cfg)
	{
		role->NoticeNum(errornum::EN_CARRER_ADVANCE_UNKNOWN_PROF);
		return false;
	}

	// 当前阶段配置的要求 进入到下一阶段
	const CareerAdvanceCfg* curr_cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(role->GetProfession(), m_param.sub_level);
	if (NULL == curr_cfg)
	{
		role->NoticeNum(errornum::EN_CARRER_ADVANCE_UNKNOWN_PROF);
		return false;
	}

	if (!_is_gm)
	{
		if (m_role_module_mgr->GetCapability()->GetRoleCapability() < curr_cfg->pre_score)
		{
			role->NoticeNum(errornum::EN_CAREER_ADVANCE_CAPABILITY_NOT_WORTH);
			return false;
		}

		if (role->GetLevel() < curr_cfg->need_level)
		{
			role->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
			return false;
		}

		int prof_index = role->GetProfession() % PROFESSION_BASE;
		if (ADVANCE_COND_TYPE_NO != curr_cfg->cond_type
			&& !(m_param.cond_complete_flag[prof_index] & LL_ONE_BIT << m_param.sub_level))
		{
			role->NoticeNum(errornum::EN_TASK_PREDECESSOR_NEED);
			return false;
		}

		if (curr_cfg->need_bind_coin > 0 && !m_role_module_mgr->GetMoney()->AllCoinMoreThan(curr_cfg->need_bind_coin))
		{
			role->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return false;
		}

		if (curr_cfg->need_bind_coin > 0 && !m_role_module_mgr->GetMoney()->UseAllCoin(curr_cfg->need_bind_coin, __FUNCTION__))
		{
			role->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return false;
		}
	}

	// 广播需要广播达到的阶段的信息 , 所以用下一个阶段
	this->AdvanceNotice(next_cfg);
	++m_param.sub_level;
	this->SendSubInfo();
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_INVALID, __FUNCTION__);

	const CareerAdvanceCfg* cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(m_role_module_mgr->GetRole()->GetProfession(), this->GetSubLevel());
	if (NULL != cfg && cfg->reward_title > 0) m_role_module_mgr->GetTitle()->AddTitle(cfg->reward_title, __FUNCTION__);

	EventHandler::Instance().OnCareerAdvance(role, this->GetMainLevel(), this->GetSubLevel());
	return true;
}

bool AdvanceCareer::RoleAdvanceCareerMain(int _to_profession, int _to_color, bool _is_gm)
{
	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role)
		return false;

	// 大阶段 
	// 验证看一下是否有下一个阶段
	const CareerAdvanceCfg* next_cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(_to_profession, 0);
	if (NULL == next_cfg)
	{
		role->NoticeNum(errornum::EN_CARRER_ADVANCE_UNKNOWN_PROF);
		return false;
	}

	// 当前阶段配置的要求 进入到下一阶段
	const CareerAdvanceCfg* curr_cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(role->GetProfession(), m_param.sub_level);
	if (NULL == curr_cfg)
	{
		role->NoticeNum(errornum::EN_CARRER_ADVANCE_UNKNOWN_PROF);
		return false;
	}

	if (!_is_gm)
	{
		if (next_cfg->pre_profession != role->GetProfession())
		{
			role->NoticeNum(errornum::EN_CAREER_ADVANCE_PRE_PROF_LIMIT);
			return false;
		}

		if (m_role_module_mgr->GetCapability()->GetRoleCapability() < curr_cfg->pre_score)
		{
			role->NoticeNum(errornum::EN_CAREER_ADVANCE_CAPABILITY_NOT_WORTH);
			return false;
		}

		if (role->GetLevel() < curr_cfg->need_level)
		{
			role->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
			return false;
		}

		int prof_index = role->GetProfession() % PROFESSION_BASE;
		if (ADVANCE_COND_TYPE_NO != curr_cfg->cond_type
			&& !(m_param.cond_complete_flag[prof_index] & LL_ONE_BIT << m_param.sub_level))
		{
			role->NoticeNum(errornum::EN_TASK_PREDECESSOR_NEED);
			return false;
		}

		if (curr_cfg->need_bind_coin > 0 && !m_role_module_mgr->GetMoney()->AllCoinMoreThan(curr_cfg->need_bind_coin))
		{
			role->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return false;
		}

		if (curr_cfg->need_bind_coin > 0 && !m_role_module_mgr->GetMoney()->UseAllCoin(curr_cfg->need_bind_coin, __FUNCTION__))
		{
			role->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return false;
		}

		if (!AppearanceConfig::Instance().CheckAvatarAndColor(role->GetAvatarType(), static_cast<char>(_to_color), _to_profession))
		{
			role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return false;
		}
	}

	m_param.sub_level = 0;
	// 广播需要广播达到的阶段的信息 , 所以用下一个阶段
	this->AdvanceNotice(next_cfg);
	role->SetColor(static_cast<char>(_to_color));
	role->SetProfession(_to_profession, __FUNCTION__);
	this->SendSubInfo();

	EventHandler::Instance().OnCareerAdvance(role, this->GetMainLevel(), this->GetSubLevel());
	return true;
}

void AdvanceCareer::AdvanceNotice(const CareerAdvanceCfg* _cfg)
{
	if (NULL == _cfg || !_cfg->is_broadcast)
		return;

	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role)
		return ;

	//传闻默认发本服+跨服
	if (0 != _cfg->is_broadcast)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_role_advance_content, role->GetName(), _cfg->job_name);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
			{
				World::GetInstWorld()->SendToAllGateway((const char *)&sm, sizeof(sm));
				if (CrossConfig::Instance().IsHiddenServer())
				{
					InternalComm::Instance().SendToGameAllUserThroughCross(m_role_module_mgr->GetRoleCross()->GetOriginPlatType(),
						m_role_module_mgr->GetRoleCross()->GetOriginServerId(), (const char *)&sm, sizeof(sm));
				}
				else
				{
					if (CrossManager::Instance().IsCrossServerConnected())
					{
						InternalComm::Instance().SendToHiddenAllUserThroughCross((const char *)&sm, sizeof(sm));
					}
				}
			}
		}
	}
}

void AdvanceCareer::InitHistoryData()
{
	const ProfOtherCfg & other_cfg = LOGIC_CONFIG->GetProfessionConfig()->GetOtherCfg();

	if (other_cfg.ver == m_param.is_history_data)
		return;

	m_param.is_history_data = other_cfg.ver;

	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role)
		return;

	// 为了兼容老号的数据 , 需要过一次全任务
	//1 .任务
	const std::vector<CareerAdvanceCfg> * task_vec = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfgByCondType(ADVANCE_COND_TYPE_MISSION);
	if (NULL != task_vec)
	{
		std::vector< CareerAdvanceCfg>::const_iterator it_begin = task_vec->begin();
		std::vector< CareerAdvanceCfg>::const_iterator it_end = task_vec->end();

		for (; it_begin != it_end; ++it_begin)
		{
			bool ret = m_role_module_mgr->GetTaskRecorder()->HaveFinished(it_begin->cond_param1);
			if (ret)
			{
				const int prof_index = it_begin->profession_id % PROFESSION_BASE;
				const int sub_level = it_begin->order;

				if (m_param.cond_complete_flag[prof_index] & LL_ONE_BIT << sub_level)
					continue;

				int old_flag = m_param.cond_complete_flag[prof_index];
				m_param.cond_complete_flag[prof_index] |= LL_ONE_BIT << sub_level;

				{
					// 日志
					gamelog::g_log_advance_career.printf(LL_INFO, "AdvanceCareer::InitHistoryData[SUCC] user[%d %s] "
						"cond_type[%d]  param1[%d]  old_flag[%d]  new_flag[%d] "
						" prof[%d] sub_level[%d]  role_curr_prof[%d]  role_curr_sub_level[%d]",
						role->GetUID(), role->GetName(),
						ADVANCE_COND_TYPE_MISSION, it_begin->cond_param1, old_flag, m_param.cond_complete_flag[prof_index],
						it_begin->profession_id, sub_level, role->GetProfession(), m_param.sub_level);
				}
			}
		}
	}

	// 2.通关心灵之境
	int fight_seq = m_role_module_mgr->GetRealmBreak()->GetCurFightSeq();
	this->CheckAdvanceCond(ADVANCE_COND_TYPE_XIN_LING_ZHI_JING, fight_seq, 0);

	// 3. 通关梦渊
	int level = m_role_module_mgr->GetDreamNotesChallenge()->GetPassLevel();
	this->CheckAdvanceCond(ADVANCE_COND_TYPE_MENG_YUAN, level, 0);

	// 4. 通关锢魔之塔
	int ta_seq = m_role_module_mgr->GetCourageChallenge()->GetPassMaxSeq();
	this->CheckAdvanceCond(ADVANCE_COND_TYPE_GU_MO_ZHI_TA, ta_seq, 0);

	// 5. 通关轮回忘川
	int idx = (m_role_module_mgr->GetLunHuiWangChuan()->GetPassIDX() <= 0) ? 0 : m_role_module_mgr->GetLunHuiWangChuan()->GetPassIDX() - 1;
	this->CheckAdvanceCond(ADVANCE_COND_TYPE_LUN_HUI_WANG_CHUAN, idx, 0);

	// 6. 通关渡劫
	int max_pass_seq = m_role_module_mgr->GetDuJie()->GetMaxPassSeq();
	this->CheckAdvanceCond(ADVANCE_COND_TYPE_DU_JIE, max_pass_seq, 0);

	// 7. 无尽之塔
	int max_wujin_level = m_role_module_mgr->GetEndlessTower()->GetBattleSeq();
	this->CheckAdvanceCond(ADVANCE_COND_TYPE_WU_JIN_ZHI_TA, max_wujin_level, 0);

	// 8. 战胜山海BOSS
	int max_pass_times = m_role_module_mgr->GetPassShanHaiTimes();
	this->CheckAdvanceCond(ADVANCE_COND_TYPE_SHANG_HAI_BOSS_SUCC, max_pass_times, 0);

	// 9. 战胜北斗七星
	int big_dipper_fight_succ_times = m_role_module_mgr->GetBigDipperFightSuccTimes();
	this->CheckAdvanceCond(ADVANCE_COND_TYPE_BIG_DIPPER_SUCC, big_dipper_fight_succ_times, 0);

	// 10. 通关幽冥炼狱
	int nwp_chapters = m_role_module_mgr->GetNetherWorldPurgatory()->GetChallengeChapter();
	int nwp_challenge_id = m_role_module_mgr->GetNetherWorldPurgatory()->GetChallengeId();
	this->CheckAdvanceCond(ADVANCE_COND_TYPE_NETHER_WORLD_PURGATORY, nwp_chapters, nwp_challenge_id);

	// 11. 妖行试炼
	int yxsl_pass_seq = m_role_module_mgr->GetYaoXingShiLian()->GetPassBattleSeq();
	this->CheckAdvanceCond(ADVANCE_COND_TYPE_YAO_XING_SHI_LIAN, yxsl_pass_seq, 0);

	// 12. 战胜怪物组
	this->RoleLoginFightMonsterTypeCheck();

	// 13. 通关弑神之塔
	int new_ta_seq = m_role_module_mgr->GetNewCourageChallenge()->GetPassMaxSeq();
	this->CheckAdvanceCond(ADVANCE_COND_TYPE_SHI_SHEN_ZHI_TA, new_ta_seq, 0);

	// 14. 琉璃蜃境
	int mt_min_top_seq = m_role_module_mgr->GetMirageTerritory()->GetMinAllTopBattleSeq();
	this->CheckAdvanceCond(ADVANCE_COND_TYPE_MIRAGE_TERRITORY, mt_min_top_seq, 0);

	// 15. 鸿蒙天界
	this->RoleLoginHongMengTianJieTypeCheck();

	this->SendSubInfo();
}
