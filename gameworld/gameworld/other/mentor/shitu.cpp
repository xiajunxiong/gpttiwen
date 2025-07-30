#include "shitu.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "friend/friendmanager.h"
#include "servercommon/errornum.h"
#include "world.h"
#include "protocol/msgshitu.h"
#include "other/vip/vip.hpp"
#include "servercommon/noticenum.h"
#include "global/usercache/usercache.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/mentor/shituconfig.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "other/mentor/shituconfig.hpp"
#include "other/courseofgrowth/courseofgrowth.hpp"
#include "global/shituseeking/shituseekingmanager.h"
#include "other/route/mailroute.hpp"
#include "other/event/eventhandler.hpp"
#include "item/itembase.h"
#include "item/itempool.h"
#include "servercommon/string/gameworldstr.h"
#include "global/team/team.hpp"
#include "internalcomm.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/serverconfig/localconfig.hpp"
#include "other/capability/capability.hpp"
#include "gamelog.h"

#include "scene/scene.h"
#include "scene/speciallogicimpl/speciallogicshituchuangong.hpp"
#include "other/levelcomplement/levelcomplement.h"

ShiTu::ShiTu() :m_role_module_mgr(NULL), m_check_remove_time(0)
{
}

ShiTu::~ShiTu()
{
}



void ShiTu::Init(RoleModuleManager * mgr, const ShiTuParam & common_param)
{
	m_role_module_mgr = mgr;
	m_param = common_param;

	if (m_param.mod_ver != SHI_TU_PARAM_MOD_VER)
	{
		m_param.mod_ver = SHI_TU_PARAM_MOD_VER;

		for (int index = 0; index < ARRAY_LENGTH(m_param.relation_data.master_list) && index < ARRAY_LENGTH(m_param.relation_data_old.master_list); ++index)
		{
			RelationInfo & curr = m_param.relation_data.master_list[index];
			RelationInfoOld & curr_old = m_param.relation_data_old.master_list[index];
			curr.role_id = curr_old.role_id;
			curr.relation_break_time = curr_old.relation_break_time;

			if (curr.role_id != 0)
			{
				curr.shi_tu_time = SHI_TU_TIME;
			}
		}

		for (int index = 0; index < ARRAY_LENGTH(m_param.relation_data.apprentice_list) && index < ARRAY_LENGTH(m_param.relation_data_old.apprentice_list); ++index)
		{
			RelationInfo & curr = m_param.relation_data.apprentice_list[index];
			RelationInfoOld & curr_old = m_param.relation_data_old.apprentice_list[index];
			curr.role_id = curr_old.role_id;
			curr.relation_break_time = curr_old.relation_break_time;

			if (curr.role_id != 0)
			{
				curr.shi_tu_time = SHI_TU_TIME;
			}
		}
	}
}

void ShiTu::GetInitParam(ShiTuParam * param)
{
	*param = m_param;
}

void ShiTu::OnWeekChange()
{
	this->SetShiTuRewardFlag(false);

	m_param.week_chuan_gong_flag = 0;
	m_param.week_chuan_gong_exp_flag = 0;
	m_param.week_chuan_gong_score_max = 0;

	this->SendShiTuCommonInfo();
}

void ShiTu::Update(unsigned long interval, time_t now_second)
{
	if (now_second >= m_check_remove_time && !CrossConfig::Instance().IsHiddenServer())
	{
		m_check_remove_time = now_second + CHECK_REMOVE_INTERVAL;

		bool need_send = false;
		for (int i = 0; i < SHITU_MAX_MASTER_COUNT; ++i)
		{
			if (m_param.relation_data.master_list[i].role_id != 0 && m_param.relation_data.master_list[i].relation_break_time != 0)
			{				
				if (now_second >= m_param.relation_data.master_list[i].relation_break_time)
				{
					need_send = true;
					int role_id = m_param.relation_data.master_list[i].role_id;
					this->DeleteMaster(role_id);

					Role * target_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
					if (NULL != target_role)
					{
						target_role->GetRoleModuleManager()->GetShiTu()->DeleteApprentice(m_role_module_mgr->GetRole()->GetUID());
					}
					else
					{
						OfflineRegisterManager::Instance().OnShiTuBreakRelationReq(role_id, SHITU_RELATION_BREAK_NOTICE_TYPE_APPRENTICE_TO_MASTER, m_role_module_mgr->GetRole()->GetUID());
					}

					gamelog::g_log_shi_tu.printf(LL_INFO, "%s %d role_id[%d %s] del_master[%d] target_is_online:%d",
						__FUNCTION__, __LINE__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), role_id, NULL != target_role ? 1 : 0);
				}
			}
		}

		for (int i = 0; i < SHITU_MAX_APPRENTICE_COUNT; ++i)
		{
			if (m_param.relation_data.apprentice_list[i].role_id != 0 && m_param.relation_data.apprentice_list[i].relation_break_time != 0)
			{
				if (now_second >= m_param.relation_data.apprentice_list[i].relation_break_time)
				{
					need_send = true;
					int role_id = m_param.relation_data.apprentice_list[i].role_id;
					this->DeleteApprentice(role_id);

					Role * target_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
					if (NULL != target_role)
					{
						target_role->GetRoleModuleManager()->GetShiTu()->DeleteMaster(m_role_module_mgr->GetRole()->GetUID());
					}
					else
					{
						OfflineRegisterManager::Instance().OnShiTuBreakRelationReq(role_id, SHITU_RELATION_BREAK_NOTICE_TYPE_MASTER_TO_APPRENTICE, m_role_module_mgr->GetRole()->GetUID());
					}

					gamelog::g_log_shi_tu.printf(LL_INFO, "%s %d role_id[%d %s] del_apprentice[%d] target_is_online:%d",
						__FUNCTION__, __LINE__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), role_id, NULL != target_role ? 1 : 0);
				}
			}
		}

		if (need_send)
		{
			this->SendRelationInfo();
		}
	}
}

void ShiTu::OnMakeRelationReq(int type, int target_uid)
{
	if (target_uid == m_role_module_mgr->GetRole()->GetUID())
	{
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		UniqueServerID origin_usid(LocalConfig::Instance().GetPlatType(), IntToUid(m_role_module_mgr->GetRole()->GetUID()).db_index);
		const UniqueServerID * usid = World::GetInstWorld()->GetMotherServerId(origin_usid);
		if (usid == NULL)
		{
			return;
		}

		UniqueServerID target_origin_usid(LocalConfig::Instance().GetPlatType(), IntToUid(target_uid).db_index);
		const UniqueServerID * target_usid = World::GetInstWorld()->GetMotherServerId(target_origin_usid);
		if (target_usid == NULL)
		{
			return;
		}

		if (*World::GetInstWorld()->GetMotherServerId(origin_usid) != *World::GetInstWorld()->GetMotherServerId(target_origin_usid))
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_CAN_NOT_MAKE_SHI_TI);
			return;
		}
	}

	if (FriendManager::Instance().IsBlack(m_role_module_mgr->GetRole()->GetUID(), target_uid))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_IN_YOUR_BLACKLISTS);
		return;
	}

	Role * target_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(target_uid));
	if (NULL == target_role)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_ROLE_NOT_ONLINE);
		return;
	}

	if (FriendManager::Instance().IsBlack(target_role->GetUID(), m_role_module_mgr->GetRole()->GetUID()))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_YOU_ARE_IN_BLACKLISTS);
		return;
	}

	const ShiTuOtherCfg & other_cfg = LOGIC_CONFIG->GetShiTuConfig()->GetOtherCfg();

	if (SHITU_OPERATE_TYPE_APPRENTICE == type)				// 拜师请求
	{
		if (this->IsMasterFull())
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SHITU_ALREADY_HAS_MASTER_FULL);
			return;
		}

		if (this->IsMaster(target_uid))
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SHITU_TARGET_ALREADY_SHITU);
			return;
		}

		if (this->IsApprentice(target_uid))
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SHITU_TARGET_ALREADY_SHITU);
			return;
		}

		if (target_role->GetRoleModuleManager()->GetShiTu()->IsApprenticeFull())
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SHITU_TARGET_APPRENTICE_FULL);
			return;
		}

		if (target_role->GetLevel() < other_cfg.graduation_level)
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SHITU_ROLE_LEVEL_NOT_SATISFY);
			return;
		}
	}
	else if (SHITU_OPERATE_TYPE_MASTER == type)		// 收徒请求
	{
		if (this->IsApprenticeFull())
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SHITU_APPRENTICE_FULL);
			return;
		}

		if (this->IsMaster(target_uid))
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SHITU_TARGET_ALREADY_SHITU);
			return;
		}

		if (this->IsApprentice(target_uid))
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SHITU_TARGET_ALREADY_SHITU);
			return;
		}

		if (target_role->GetRoleModuleManager()->GetShiTu()->IsMasterFull())
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SHITU_TARGET_ALREADY_HAS_MASTER);
			return;
		}
	}

	// 发送
	static Protocol::SCShiTuMakeRelationRoute stmrr;
	stmrr.type = type;
	stmrr.uid = m_role_module_mgr->GetRole()->GetUID();
	m_role_module_mgr->GetRole()->GetName(stmrr.name);
	stmrr.level = m_role_module_mgr->GetRole()->GetLevel();
	stmrr.sex = m_role_module_mgr->GetRole()->GetSex();
	stmrr.prof = m_role_module_mgr->GetRole()->GetProfession();
	stmrr.reserve_ch = 0;
	stmrr.avatar_type = m_role_module_mgr->GetRole()->GetAvatarType();
	stmrr.headshot_id = m_role_module_mgr->GetRole()->GetHeadshotID();

	EngineAdapter::Instance().NetSend(target_role->GetNetId(), (const char*)&stmrr, sizeof(stmrr));

	static int notice_num[SHITU_OPERATE_TYPE_MAX] =
	{
		noticenum::NT_SHITU_MAKE_RELATION_MASTER_REQ_SUCC,
		noticenum::NT_SHITU_MAKE_RELATION_APPRENTICE_REQ_SUCC
	};

	if (type < SHITU_OPERATE_TYPE_MAX && type >= 0)
	{
		m_role_module_mgr->GetRole()->NoticeNum(notice_num[type]);
	}
}

void ShiTu::OnMakeRelationAck(int type, int target_uid, bool is_agree)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		UniqueServerID origin_usid(LocalConfig::Instance().GetPlatType(), IntToUid(m_role_module_mgr->GetRole()->GetUID()).db_index);
		const UniqueServerID * usid = World::GetInstWorld()->GetMotherServerId(origin_usid);
		if (usid == NULL)
		{
			return;
		}

		UniqueServerID target_origin_usid(LocalConfig::Instance().GetPlatType(), IntToUid(target_uid).db_index);
		const UniqueServerID * target_usid = World::GetInstWorld()->GetMotherServerId(target_origin_usid);
		if (target_usid == NULL)
		{
			return;
		}

		if (*World::GetInstWorld()->GetMotherServerId(origin_usid) != *World::GetInstWorld()->GetMotherServerId(target_origin_usid))
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_CAN_NOT_MAKE_SHI_TI);
			return;
		}
	}

	if (type < SHITU_OPERATE_TYPE_APPRENTICE || type >= SHITU_OPERATE_TYPE_MAX)
	{
		return;
	}

	// 检查
	if (target_uid == m_role_module_mgr->GetRole()->GetUID())
	{
		return;
	}

	if (FriendManager::Instance().IsBlack(m_role_module_mgr->GetRole()->GetUID(), target_uid))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_IN_YOUR_BLACKLISTS);
		return;
	}

	Role *target_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(target_uid));
	if (NULL == target_role)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_TARGET_NOT_ONLINE);
		return;
	}

	if (FriendManager::Instance().IsBlack(target_role->GetUID(), m_role_module_mgr->GetRole()->GetUID()))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_YOU_ARE_IN_BLACKLISTS);
		return;
	}

	if (!is_agree)
	{
		target_role->NoticeNum(errornum::EN_SHITU_TARGET_REJECT_REQ);
		return;
	}

	if (this->IsMaster(target_uid) || this->IsApprentice(target_uid))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SHITU_TARGET_ALREADY_SHITU);
		return;
	}

	const ShiTuOtherCfg & other_cfg = LOGIC_CONFIG->GetShiTuConfig()->GetOtherCfg();

	if (SHITU_OPERATE_TYPE_APPRENTICE == type)				// 回复  对方收徒请求
	{
		if (target_role->GetRoleModuleManager()->GetShiTu()->IsApprenticeFull())
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SHITU_TARGET_APPRENTICE_FULL);
			return;
		}

		if (this->IsMasterFull())
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SHITU_ALREADY_HAS_MASTER_FULL);
			return;
		}

		if (target_role->GetLevel() < other_cfg.graduation_level)
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SHITU_ROLE_LEVEL_NOT_SATISFY);
			return;
		}

		this->AddMaster(target_uid);
		target_role->GetRoleModuleManager()->GetShiTu()->AddApprentice(m_role_module_mgr->GetRole()->GetUID());
	}
	else if (SHITU_OPERATE_TYPE_MASTER == type)		// 回复对方拜师请求
	{
		if (this->IsApprenticeFull())
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SHITU_APPRENTICE_FULL);
			return;
		}

		if (target_role->GetRoleModuleManager()->GetShiTu()->IsMasterFull())
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SHITU_TARGET_ALREADY_HAS_MASTER);
			return;
		}

		this->AddApprentice(target_uid);
		target_role->GetRoleModuleManager()->GetShiTu()->AddMaster(m_role_module_mgr->GetRole()->GetUID());
	}

	m_role_module_mgr->GetRole()->NoticeNum(noticenum::NT_SHITU_MAKE_RELATION_SUCC);
	static int notice_num[SHITU_OPERATE_TYPE_MAX] =
	{
		noticenum::NT_SHITU_MAKE_RELATION_APPRENTICE_ACK_SUCC,
		noticenum::NT_SHITU_MAKE_RELATION_MASTER_ACK_SUCC
	};

	if (type < SHITU_OPERATE_TYPE_MAX && type >= 0)
	{
		target_role->NoticeNum(notice_num[type]);
	}

	this->SendRelationInfo();
	target_role->GetRoleModuleManager()->GetShiTu()->SendRelationInfo();
}

void ShiTu::OnBreakRelationReq(int type, int target_uid)
{
	const ShiTuOtherCfg & other_cfg = LOGIC_CONFIG->GetShiTuConfig()->GetOtherCfg();

	if (!this->IsMaster(target_uid) && !this->IsApprentice(target_uid))
	{
		return;
	}

	if (this->GetShiTuDay(target_uid) < other_cfg.graduation_day)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SHI_TU_BREAK_NOT_ENOUGH_DAY);
		return;
	}

	RelationInfo * info = this->GetShiTuRelationInfo(target_uid);
	if (!info)
	{
		return;
	}

	if (info->relation_break_time == 0)
	{
		this->SetAutoBreakTime(SHITU_RELATION_BREAK_TYPE_AUTO, target_uid, AUTO_BREAK_TIME);
		gamelog::g_log_shi_tu.printf(LL_INFO, "%s %d type:%d role_id[%d %s] %d",
			__FUNCTION__, __LINE__, type, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), target_uid);

		this->SendRelationInfo();
	}

	Role * target_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(target_uid));
	if (NULL != target_role)
	{
		static Protocol::SCShiTuBreakRelationRoute stmrr;
		stmrr.type = type;
		stmrr.uid = m_role_module_mgr->GetRole()->GetUID();
		m_role_module_mgr->GetRole()->GetName(stmrr.name);
		stmrr.level = m_role_module_mgr->GetRole()->GetLevel();
		stmrr.sex = (char)m_role_module_mgr->GetRole()->GetSex();
		stmrr.prof = m_role_module_mgr->GetRole()->GetProfession();
		stmrr.reserve_ch = 0;
		stmrr.avatar_timestamp = m_role_module_mgr->GetRole()->GetAvatarTimestamp();
		EngineAdapter::Instance().NetSend(target_role->GetNetId(), (const char *)&stmrr, sizeof(stmrr));
		m_role_module_mgr->GetRole()->NoticeNum(noticenum::NT_SHITU_MAKE_RELATION_DISPATCH_TEACHERS_REQ_SUCC);
	}
	else
		m_role_module_mgr->GetRole()->NoticeNum(noticenum::EN_SHITU_TARGET_OTHER_PARTY_NOT_ONLINE_REQ);
}

void ShiTu::OnBreakRelationAck(int target_uid, bool is_agree)
{
	const ShiTuOtherCfg & other_cfg = LOGIC_CONFIG->GetShiTuConfig()->GetOtherCfg();
	Role * target_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(target_uid));

	if (!is_agree)
	{
		if (NULL != target_role)
		{
			target_role->NoticeNum(errornum::EN_SHITU_TARGET_REJECT_REQ);			
		}

		m_role_module_mgr->NoticeNum(noticenum::EN_SHITU_TARGET_REJECT_REQ);
		return;
	}

	if (NULL == target_role)
	{
		return;
	}

	if (this->GetShiTuDay(target_uid) < other_cfg.graduation_day)
	{
		return;
	}

	int type = -1;
	if (!this->IsMaster(target_uid) && !this->IsApprentice(target_uid))
	{
		return;
	}

	if (this->IsMaster(target_uid))
	{
		type = SHITU_OPERATE_TYPE_APPRENTICE;			// 回复 师傅（对方）对 徒弟（自己） 毕业请求
	}

	if (this->IsApprentice(target_uid))
	{
		type = SHITU_OPERATE_TYPE_MASTER;			// 回复 徒弟（对方）对 师傅（自己）  出师请求
	}

	switch (type)
	{
		case SHITU_OPERATE_TYPE_APPRENTICE:
			{
				target_role->GetRoleModuleManager()->GetShiTu()->DeleteApprentice(m_role_module_mgr->GetRole()->GetUID());
				this->DeleteMaster(target_uid);
				m_role_module_mgr->GetRole()->NoticeNum(noticenum::NT_SHITU_MAKE_RELATION_DISPATCH_TEACHERS_ACK_SUCC);
				target_role->NoticeNum(noticenum::NT_SHITU_MAKE_RELATION_DISPATCH_TEACHERS_SUCC);
			}
			break;
		case SHITU_OPERATE_TYPE_MASTER:
			{
				target_role->GetRoleModuleManager()->GetShiTu()->DeleteMaster(m_role_module_mgr->GetRole()->GetUID());
				this->DeleteApprentice(target_uid);
				m_role_module_mgr->GetRole()->NoticeNum(noticenum::NT_SHITU_MAKE_RELATION_GRADUATION_ACK_SUCC);
				target_role->NoticeNum(noticenum::NT_SHITU_MAKE_RELATION_GRADUATION_SUCC);
			}
			break;
		default:
			return;
	}

	gamelog::g_log_shi_tu.printf(LL_INFO, "%s %d type:%d role_id[%d %s] %d",
		__FUNCTION__, __LINE__, type, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), target_uid);

	this->SendRelationInfo();
	target_role->GetRoleModuleManager()->GetShiTu()->SendRelationInfo();
}

void ShiTu::OnBreakRelationForece(int target_uid, bool is_use_gold)
{
	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	RelationInfo * info = this->GetShiTuRelationInfo(target_uid);
	if (!info)
	{
		return;
	}

	if (info->relation_break_time != 0)
	{
		return;
	}

	const ShiTuOtherCfg & other_cfg = LOGIC_CONFIG->GetShiTuConfig()->GetOtherCfg();

	if (!is_use_gold)			//如果不是使用元宝，就要判断对方离线时间是否满足
	{

		UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(target_uid);
		if (!user_node)
		{
			return;
		}

		if (now < user_node->last_online_timestamp + other_cfg.force_break_relation_offline_time)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_TARGET_OFFLINE_TIME_NOT_ENOUTH);
			this->SendRelationInfo();
			return;
		}
	}
	else
	{
		if (!m_role_module_mgr->GetMoney()->UseGold(other_cfg.force_break_relation_consume_gold, __FUNCTION__))
		{
			return;
		}
	}

	this->SetAutoBreakTime(SHITU_RELATION_BREAK_TYPE_FORCE, target_uid, other_cfg.force_break_relation_time);
	this->SendRelationInfo();

	Protocol::SCShiTuOpNotice notice;
	notice.notice_type = SHI_TU_OP_NOTICE_TYPE_FORCE_BREAK;
	notice.p1 = target_uid;
	m_role_module_mgr->NetSend(&notice, sizeof(notice));
	
	this->SendBreakTimeMailNotice(target_uid, now + other_cfg.force_break_relation_time);
}

void ShiTu::OnCancelBreakRelation(int target_uid)
{
	RelationInfo * info = this->GetShiTuRelationInfo(target_uid);
	if (info->relation_break_time == 0)
	{
		return;
	}

	info->relation_break_time_type = 0;
	info->relation_break_time = 0;
	this->SendRelationInfo();

	Protocol::SCShiTuOpNotice notice;
	notice.notice_type = SHI_TU_OP_NOTICE_TYPE_FORCE_BREAK_CANCEL;
	notice.p1 = target_uid;
	m_role_module_mgr->NetSend(&notice, sizeof(notice));
}

void ShiTu::OnRoleLogin()
{
	this->CheckShitu();

	//师傅
	for (int i = 0; i < SHITU_MAX_MASTER_COUNT; ++i)
	{
		int &master_uid = m_param.relation_data.master_list[i].role_id;
		if (0 == master_uid)
		{
			continue;
		}
		this->OnMasterOnline(master_uid, true);
	}

	//徒弟
	for (int i = 0; i < SHITU_MAX_APPRENTICE_COUNT; ++i)
	{
		int &apprentice_uid = m_param.relation_data.apprentice_list[i].role_id;
		if (0 == apprentice_uid)
		{
			continue;
		}

		Role * apprentice_role = World::GetInstWorld()->GetSceneManager()->GetRole(apprentice_uid);
		if (NULL == apprentice_role)
		{
			return;
		}
		apprentice_role->GetRoleModuleManager()->GetShiTu()->OnMasterOnline(m_role_module_mgr->GetUid());
	}
}


void ShiTu::OnRoleUpLevel(int from_level, int to_level)
{
	//没有升级奖励
}

void ShiTu::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	//const std::vector<ShiTuRewardConfig> & cfg = LOGIC_CONFIG->GetShiTuConfig()->GetLevelRewardItem();
	//
	//unsigned int now_zero_time = (unsigned int)::GetZeroTime(EngineAdapter::Instance().Time());
	//for (int reward_index = 0; reward_index < (int)cfg.size(); ++reward_index)
	//{
	//	const ShiTuRewardConfig & curr_cfg = cfg[reward_index];
	//	for (int index = 0; index < ARRAY_LENGTH(m_param.relation_data.master_list); ++index)
	//	{
	//		RelationInfo & curr = m_param.relation_data.master_list[index];
	//		if (curr.role_id == 0 || (curr.reward_flag & 1 << curr_cfg.seq))
	//		{
	//			continue;
	//		}
	//
	//		int day = (now_zero_time - ::GetZeroTime(curr.shi_tu_time) + SECOND_PER_DAY - 1) / SECOND_PER_DAY;
	//		if (day < curr_cfg.need_day)
	//		{
	//			continue;
	//		}
	//
	//		UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(curr.role_id);
	//		if (NULL == user_node)
	//		{
	//			continue;
	//		}
	//		
	//		//发放徒弟奖励
	//		static MailContentParam contentparam; contentparam.Reset();
	//		const ItemConfigData & reward = curr_cfg.apprentice_reward;
	//		const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);
	//		if (item_base != NULL)
	//		{
	//			contentparam.item_list[0].item_id = reward.item_id;
	//			contentparam.item_list[0].num = reward.num;
	//			contentparam.item_list[0].is_bind = (reward.is_bind) ? 1 : 0;
	//			contentparam.item_list[0].invalid_time = item_base->CalInvalidTime();
	//
	//			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_shi_tu_subject);
	//			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_shi_tu_reward, curr.role_id, user_node->GetNamePtr(), day);
	//			if (lensub > 0 && length > 0)
	//			{
	//				curr.reward_flag |= (1 << curr_cfg.seq);
	//				MailRoute::MailToUser(IntToUid(m_role_module_mgr->GetRole()->GetUID()), MAIL_REASON_DEFAULT, contentparam);
	//			}
	//		}
	//	}
	//
	//	for (int index = 0; index < ARRAY_LENGTH(m_param.relation_data.apprentice_list); ++index)
	//	{
	//		RelationInfo & curr = m_param.relation_data.apprentice_list[index];
	//		if (curr.role_id == 0 || (curr.reward_flag & 1 << curr_cfg.seq))
	//		{
	//			continue;
	//		}
	//
	//		int day = (now_zero_time - ::GetZeroTime(curr.shi_tu_time) + SECOND_PER_DAY - 1) / SECOND_PER_DAY;
	//		if (day < curr_cfg.need_day)
	//		{
	//			continue;
	//		}
	//
	//		UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(curr.role_id);
	//		if (NULL == user_node)
	//		{
	//			continue;
	//		}
	//
	//		//发放徒弟奖励
	//		static MailContentParam contentparam; contentparam.Reset();
	//		const ItemConfigData & reward = curr_cfg.apprentice_reward;
	//		const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);
	//		if (item_base != NULL)
	//		{
	//			contentparam.item_list[0].item_id = reward.item_id;
	//			contentparam.item_list[0].num = reward.num;
	//			contentparam.item_list[0].is_bind = (reward.is_bind) ? 1 : 0;
	//			contentparam.item_list[0].invalid_time = item_base->CalInvalidTime();
	//
	//			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_shi_tu_subject);
	//			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_shi_tu_reward, curr.role_id, user_node->GetNamePtr(), day);
	//			if (lensub > 0 && length > 0)
	//			{
	//				curr.reward_flag |= (1 << curr_cfg.seq);
	//				MailRoute::MailToUser(IntToUid(m_role_module_mgr->GetRole()->GetUID()), MAIL_REASON_DEFAULT, contentparam);
	//			}
	//		}
	//	}
	//}

	for (int i = 0; i < ARRAY_LENGTH(m_param.relation_data.master_list); i++)
	{
		if (m_param.relation_data.master_list[i].role_id > 0)
		{
			m_role_module_mgr->GetCourseOfGrowth()->EventOperator(COG_CON_APPRENTICE_DAY_NUM, 0, this->GetApprenticeDayNum(m_param.relation_data.master_list[i]));
		
			m_param.relation_data.master_list[i].day_online_flag = 0;

			this->OnMasterOnline(m_param.relation_data.master_list[i].role_id, false);
		}

	}

	m_param.day_mile_cap_fetch_num = 0;
	m_param.day_mile_pass_day_fetch_num = 0;

	this->SendShiTuCommonInfo();
}

void RelationInfoToSCShiTuRelationInfoShiTuItemInfo(const RelationInfo & src, Protocol::SCShiTuRelationInfo::ShiTuItemInfo & dst)
{

	UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(src.role_id);
	if (NULL != user_node)
	{
		dst.uid = user_node->uid;
		user_node->GetName(dst.name);
		dst.level = user_node->level;
		dst.prof = user_node->profession;
		dst.avatar_type = user_node->avatar_type;
		dst.headshot_id = user_node->appearance.headshot_id;
		dst.is_online = UserCacheManager::Instance().IsUserOnline(src.role_id) ? 1 : 0;
		dst.relation_break_time_type = src.relation_break_time_type;
		dst.relation_break_time = src.relation_break_time;
		dst.shi_tu_time = ::GetZeroTime(src.shi_tu_time);
		dst.login_time = user_node->last_online_timestamp;

		dst.apprentice_old_cap = (int)src.apprentice_old_cap;
		dst.apprentice_cap = (int)user_node->GetCapability();
		dst.mile_reward_flag = (int)src.mile_reward_flag;
	}
}


void ShiTu::SendRelationInfo()
{
	Protocol::SCShiTuRelationInfo strd;

	for (int index = 0; index < SHITU_MAX_MASTER_COUNT; ++index)
	{
		const RelationInfo & curr_src = m_param.relation_data.master_list[index];
		Protocol::SCShiTuRelationInfo::ShiTuItemInfo & curr_dst = strd.master_info_list[index];

		::RelationInfoToSCShiTuRelationInfoShiTuItemInfo(curr_src, curr_dst);

		curr_dst.apprentice_cap = m_role_module_mgr->GetCapability()->GetRoleCapability();
	}
	

	strd.apprentice_count = 0;
	for (int index = 0; strd.apprentice_count < SHITU_MAX_APPRENTICE_COUNT && index < SHITU_MAX_APPRENTICE_COUNT; ++index)
	{
		const RelationInfo & curr_src = m_param.relation_data.apprentice_list[index];
		Protocol::SCShiTuRelationInfo::ShiTuItemInfo & curr_dst = strd.apprentice_info_list[strd.apprentice_count];
		if (curr_src.role_id == 0)
		{
			continue;
		}
		
		::RelationInfoToSCShiTuRelationInfoShiTuItemInfo(curr_src, curr_dst);

		strd.apprentice_count += 1;
	}
	
	int send_len = sizeof(strd) - (SHITU_MAX_APPRENTICE_COUNT - strd.apprentice_count) * sizeof(strd.apprentice_info_list[0]);
	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*)&strd, send_len);
}


bool ShiTu::IsMasterFull()
{
	for (int i = 0; i < SHITU_MAX_MASTER_COUNT; ++i)
	{
		if (m_param.relation_data.master_list[i].role_id == 0)
		{
			return false;
		}
	}

	return true;
}

bool ShiTu::HasMaster()
{
	for (int i = 0; i < SHITU_MAX_MASTER_COUNT; ++i)
	{
		if (m_param.relation_data.master_list[i].role_id != 0)
		{
			return true;
		}
	}

	return false;
}

int ShiTu::GetApprenticeUIDList(int apprentice_uid_list[], int list_len)
{
	if (NULL == apprentice_uid_list || list_len <= 0)
	{
		return 0;
	}

	int apprentice_count = 0;
	for (int index = 0; index < SHITU_MAX_APPRENTICE_COUNT; ++index)
	{
		if (m_param.relation_data.apprentice_list[index].role_id > 0)
		{
			apprentice_uid_list[apprentice_count++] = m_param.relation_data.apprentice_list[index].role_id;

			if (apprentice_count >= list_len)
			{
				break;
			}
		}
	}

	return apprentice_count;
}

int ShiTu::GetApprenticeCount()
{
	int apprentice_count = 0;
	for (int index = 0; index < SHITU_MAX_APPRENTICE_COUNT; ++index)
	{
		if (m_param.relation_data.apprentice_list[index].role_id > 0)
		{
			apprentice_count++;
		}
	}
	return apprentice_count;
}

bool ShiTu::IsMaster(int uid)
{
	if (0 == uid)
	{
		return false;
	}

	for (int i = 0; i < ARRAY_LENGTH(m_param.relation_data.master_list); ++i)
	{
		if (m_param.relation_data.master_list[i].role_id == uid)
		{
			return true;
		}
	}

	return false;
}

bool ShiTu::IsApprentice(int uid)
{
	if (0 == uid)
	{
		return false;
	}

	for (int index = 0; index < ARRAY_LENGTH(m_param.relation_data.apprentice_list); ++index)
	{
		if (uid == m_param.relation_data.apprentice_list[index].role_id)
		{
			return true;
		}
	}

	return false;
}

bool ShiTu::IsApprenticeFull()
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.relation_data.apprentice_list); ++i)
	{
		if (m_param.relation_data.apprentice_list[i].role_id == 0)
		{
			return false;
		}
	}

	return true;
}

void ShiTu::SetAutoBreakTime(int type, int target_uid, unsigned int time)
{
	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	RelationInfo * info = this->GetShiTuRelationInfo(target_uid);
	if (!info)
	{
		return;
	}

	info->relation_break_time_type = type;
	info->relation_break_time = now + time;
}

void ShiTu::DeleteRelation(int type, int target_uid)
{
	if (SHITU_RELATION_BREAK_NOTICE_TYPE_MASTER_TO_APPRENTICE == type)
	{
		this->DeleteMaster(target_uid);
	}
	else if (SHITU_RELATION_BREAK_NOTICE_TYPE_APPRENTICE_TO_MASTER == type)
	{
		this->DeleteApprentice(target_uid);
	}
}

void ShiTu::InvitationShiTuFb()
{
	Team * team = m_role_module_mgr->GetRole()->GetMyTeam(true);
	if (team == NULL)
	{
		return;
	}

	Protocol::SCShiTuFbInvitationNotice notice;
	m_role_module_mgr->GetRole()->GetName(notice.name);
	notice.team_index = team->GetTeamIndex();
	notice.uid = m_role_module_mgr->GetRole()->GetUID();
	notice.is_cross = CrossConfig::Instance().IsHiddenServer();

	if (this->HasMaster())
	{
		for (int i = 0; i < ARRAY_LENGTH(m_param.relation_data.master_list); ++i)
		{
			int role_id = m_param.relation_data.master_list[i].role_id;
			if (role_id == 0)
			{
				continue;
			}

			Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);

			role != NULL ? EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&notice, sizeof(notice)) :
				InternalComm::Instance().SendToHiddenUserThroughCross(role_id, (const char *)&notice, sizeof(notice));
		}
	}
	else
	{
		for (int i = 0; i < ARRAY_LENGTH(m_param.relation_data.apprentice_list); ++i)
		{
			int role_id = m_param.relation_data.apprentice_list[i].role_id;
			if (role_id == 0)
			{
				continue;
			}

			Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);

			role != NULL ? EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&notice, sizeof(notice)) :
				InternalComm::Instance().SendToHiddenUserThroughCross(role_id, (const char *)&notice, sizeof(notice));
		}
	}

	m_role_module_mgr->NoticeNum(noticenum::NT_TEAM_INVITE_SUCC);
}

int ShiTu::GetShiTuRewardFlag()
{
	return m_param.shi_tu_fb_reward_flag;
}

void ShiTu::SetShiTuRewardFlag(bool is_get)
{
	m_param.shi_tu_fb_reward_flag = is_get ? 1 : 0;
	this->SendShiTuCommonInfo();
}

void ShiTu::SendShiTuCommonInfo()
{
	Protocol::SCShiTuFbCommonInfo protocol;
	protocol.is_get_shi_tu_reward = m_param.shi_tu_fb_reward_flag;

	protocol.day_mile_cap_fetch_num = m_param.day_mile_cap_fetch_num;
	protocol.day_mile_pass_day_fetch_num = m_param.day_mile_pass_day_fetch_num;

	protocol.week_chuan_gong_flag = m_param.week_chuan_gong_flag;
	protocol.week_chuan_gong_exp_flag = m_param.week_chuan_gong_exp_flag;
	protocol.week_chuan_gong_score_max = m_param.week_chuan_gong_score_max;
	protocol.mile_reward_flag = m_param.mile_reward_flag;

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

RelationInfo * ShiTu::GetShiTuRelationInfo(int role_id)
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.relation_data.master_list); ++i)
	{
		if (role_id == m_param.relation_data.master_list[i].role_id)
		{
			return &m_param.relation_data.master_list[i];
		}
	}

	for (int i = 0; i < ARRAY_LENGTH(m_param.relation_data.apprentice_list); ++i)
	{
		if (role_id == m_param.relation_data.apprentice_list[i].role_id)
		{
			return &m_param.relation_data.apprentice_list[i];
		}
	}

	return NULL;
}

int ShiTu::GetShiTuDay(int role_id)
{
	RelationInfo * info=this->GetShiTuRelationInfo(role_id);
	if (!info)
	{
		return 0;
	}

	unsigned int now_zero_time = (unsigned int)::GetZeroTime(EngineAdapter::Instance().Time());
	int day = (now_zero_time - ::GetZeroTime(info->shi_tu_time)) / SECOND_PER_DAY + 1;
	return day;
}

void ShiTu::OnShiTuReq(int type, int p_1, int p_2)
{
	switch (type)
	{
	case Protocol::CSShiTuChuanGongFBReq::REQ_TYPE_FETCH_MILE:
		{
			this->FetchMile(p_1, p_2);
		}
		break;
		//场景操作
	case Protocol::CSShiTuChuanGongFBReq::REQ_TYPE_GAME_READY:
	case Protocol::CSShiTuChuanGongFBReq::REQ_TYPE_GAME_INFO:
	case Protocol::CSShiTuChuanGongFBReq::REQ_TYPE_GAME_END:
		{
			Scene *scene = m_role_module_mgr->GetRole()->GetScene();
			if (NULL == scene)
			{
				return;
			}
			if (scene->GetSceneType() != Scene::SCENE_TYPE_SHI_TU_CHUAN_GONG)
			{
				return;
			}

			SpecialLogicShiTuChuanGong * _special = (SpecialLogicShiTuChuanGong *)scene->GetSpecialLogic();
			if (NULL == _special)
			{
				return;
			}
			_special->OnReq(m_role_module_mgr->GetRole(), type, p_1, p_2);
		}
		break;
	default:
		break;
	}
}

void ShiTu::FetchMile(int target_uid, int index)
{
	//师徒信息
	RelationInfo * relation_info = this->GetShiTuRelationInfo(target_uid);
	if (NULL == relation_info)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	//配置
	const ShiTuChuanGongOtherCfg & other_cfg = LOGIC_CONFIG->GetShiTuConfig()->GetChuanGongOtherCfg();

	const ShiTuMileCfg * mile_cfg = LOGIC_CONFIG->GetShiTuConfig()->GetShiTuMileCfg(index);
	if (NULL == mile_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	//已领
	if (IsSetBit(m_param.mile_reward_flag, mile_cfg->index))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	bool taget_is_master = this->IsMaster(relation_info->role_id);

	//领取限制
	switch (mile_cfg->type)
	{
	case SHI_TU_MILE_TYPE_CAP_UP:
		{
			//每日领取次数限制
			if (other_cfg.day_mile_fetch_num <= m_param.day_mile_cap_fetch_num)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_SHI_TU_MILE_FETCH_LIMIT);
				return;
			}

			//战力提升数值
			long long cap_up = 0;
			if (taget_is_master)
			{//目标是 师傅
				cap_up = m_role_module_mgr->GetCapability()->GetRoleCapability()
							- relation_info->apprentice_old_cap;
			}
			else
			{//目标是 徒弟
				UserCacheNode * user_node 
					= UserCacheManager::Instance().GetUserNode(relation_info->role_id);
				if (NULL == user_node)
				{
					m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
					return;
				}
				cap_up = user_node->capability - relation_info->apprentice_old_cap;
			}

			if (mile_cfg->param > cap_up)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_CONDITIONS_NOT_REACHED);
				return;
			}
			
			//记录
			m_param.day_mile_cap_fetch_num++;
			SetBit(m_param.mile_reward_flag, mile_cfg->index);
		}
		break;
	case SHI_TU_MILE_TYPE_PASS_DAY:
		{
			//每日领取次数限制
			if (other_cfg.day_mile_fetch_num <= m_param.day_mile_pass_day_fetch_num)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_SHI_TU_MILE_FETCH_LIMIT);
				return;
			}

			int shi_tu_pass_day = this->GetShiTuDay(relation_info->role_id);
			if (mile_cfg->param > shi_tu_pass_day)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_CONDITIONS_NOT_REACHED);
				return;
			}

			//记录
			m_param.day_mile_pass_day_fetch_num++;
			SetBit(m_param.mile_reward_flag, mile_cfg->index);
		}
		break;
	default:
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
		break;
	}//注 数据改变 此后不再ret
	
	//奖励
	//发放
	const std::vector<ItemConfigData> * reward = taget_is_master
		? &mile_cfg->apprentice_reward : &mile_cfg->master_reward;
	if (m_role_module_mgr->GetKnapsack()->CheckForPutList2(*reward))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(*reward, PUT_REASON_WEDDING);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(*reward, 1, SNED_MAIL_TYPE_DEFAULT, true);
	}

	this->SendRelationInfo();
	this->SendShiTuCommonInfo();

	//log
	gamelog::g_log_role_activity.printf(LL_INFO, "ShiTu::FetchMile "
		"| role_id[%d] target_role_id:[%d] fetch_index:[%d] flag:[%lld]",
		m_role_module_mgr->GetUid(), target_uid, index, m_param.mile_reward_flag);
}

void ShiTu::FetchAllMile(int target_uid)
{
	//师徒信息
	RelationInfo * relation_info = this->GetShiTuRelationInfo(target_uid);
	if (NULL == relation_info)
	{
		return;
	}
	bool taget_is_master = this->IsMaster(relation_info->role_id);

	UserCacheNode * user_node
		= UserCacheManager::Instance().GetUserNode(relation_info->role_id);
	if (NULL == user_node)
	{
		return;
	}
	long long cap_up = 0;
	if (taget_is_master)
	{//目标是 师傅
		cap_up = m_role_module_mgr->GetCapability()->GetRoleCapability()
			- relation_info->apprentice_old_cap;
	}
	else
	{//目标是 徒弟
		cap_up = user_node->capability - relation_info->apprentice_old_cap;
	}

	int shi_tu_pass_day = this->GetShiTuDay(relation_info->role_id);


	gamelog::g_log_role_activity.printf(LL_INFO, "ShiTu::FetchAllMile "
		"| role_id[%d] target_role_id:[%d] flag:[%lld]: cap_up[%lld], shi_tu_pass_day[%d]. ",
		m_role_module_mgr->GetUid(), target_uid, m_param.mile_reward_flag,
		cap_up, shi_tu_pass_day);

	const int count = sizeof(m_param.mile_reward_flag) * BIT_COUNT_OF_BYTE;
	for (int index = 0; index < count; index++)
	{
		const ShiTuMileCfg * mile_cfg = LOGIC_CONFIG->GetShiTuConfig()->GetShiTuMileCfg(index);
		if (NULL == mile_cfg)
		{
			break;
		}

		//已领
		if (IsSetBit(m_param.mile_reward_flag, mile_cfg->index))
		{
			continue;
		}


		//领取限制
		switch (mile_cfg->type)
		{
		case SHI_TU_MILE_TYPE_CAP_UP:
			{
				////每日领取次数限制
				//if (other_cfg.day_mile_fetch_num <= m_param.day_mile_cap_fetch_num)
				//{
				//	continue;
				//}

				//战力提升数值
				if (mile_cfg->param > cap_up)
				{
					continue;
				}

				//记录
				//m_param.day_mile_cap_fetch_num++;
				SetBit(m_param.mile_reward_flag, mile_cfg->index);
			}
			break;
		case SHI_TU_MILE_TYPE_PASS_DAY:
			{
				////每日领取次数限制
				//if (other_cfg.day_mile_fetch_num <= m_param.day_mile_pass_day_fetch_num)
				//{
				//	continue;
				//}

				if (mile_cfg->param > shi_tu_pass_day)
				{
					continue;
				}

				//记录
				//m_param.day_mile_pass_day_fetch_num++;
				SetBit(m_param.mile_reward_flag, mile_cfg->index);
			}
			break;
		default:
			{
				continue;
			}
			break;
		}//注 数据改变 此后不再ret

		 //奖励
		const std::vector<ItemConfigData> * reward = taget_is_master
			? &mile_cfg->apprentice_reward : &mile_cfg->master_reward;

		//发放
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, *reward);
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_shitu_mile_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), 
			taget_is_master ? gamestring::g_shitu_mile_apprentice_reward : gamestring::g_shitu_mile_master_reward,
			user_node->GetNamePtr());
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(IntToUid(m_role_module_mgr->GetRole()->GetUID()), MAIL_REASON_DEFAULT, contentparam);
		}
		
		//log
		gamelog::g_log_role_activity.printf(LL_INFO, "ShiTu::FetchAllMile "
			"| role_id[%d] target_role_id:[%d] fetch_index:[%d] flag:[%lld]",
			m_role_module_mgr->GetUid(), target_uid, index, m_param.mile_reward_flag);
	}
}

void ShiTu::FinishChuanGong(int target_uid, int score)
{
	//师徒信息
	RelationInfo * relation_info = this->GetShiTuRelationInfo(target_uid);
	if (NULL == relation_info)
	{
		return;
	}
	bool taget_is_master = this->IsMaster(relation_info->role_id);

	int grade_count = LOGIC_CONFIG->GetShiTuConfig()->GetChuanGongGradeCount();

	//奖励
	std::vector<ItemConfigData> reward_vec;
	long long reward_exp = 0;

	//经验 徒弟才有  且徒弟等级低于师傅
	bool can_reward_exp = false;
	{
		UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(target_uid);
		if (NULL != user_node)
		{
			can_reward_exp = user_node->GetLevel() > m_role_module_mgr->GetRole()->GetLevel();
		}
	}

	if (can_reward_exp
		&& taget_is_master
		&& 0 == m_param.week_chuan_gong_exp_flag)
	{
		const ShiTuChuanGongExpCfg * exp_cfg
			= LOGIC_CONFIG->GetShiTuConfig()->GetChuanGongExpCfg(m_role_module_mgr->GetRole()->GetLevel());
		if (NULL != exp_cfg)
		{
			m_param.week_chuan_gong_exp_flag = 1;

			reward_exp = exp_cfg->reward_exp;

		}

		gamelog::g_log_role_activity.printf(LL_INFO, "ShiTu::FinishChuanGong EXP"
			"| role_id[%d] target_role_id:[%d] level:[%d] reward_exp:[%lld] flag:[%d]",
			m_role_module_mgr->GetUid(), target_uid, m_role_module_mgr->GetRole()->GetLevel(),
			reward_exp, m_param.week_chuan_gong_exp_flag);
	}


	if (0 < reward_exp)
	{
		long long real_add_exp = m_role_module_mgr->GetRole()->AddExp(reward_exp, ADD_EXP_TYPE_SHI_TU_CHUAN_GONG, __FUNCTION__);
		m_role_module_mgr->GetLevelComplement()->OnAddExp(real_add_exp);
		EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_role_module_mgr->GetRole(), reward_exp, __FUNCTION__);
	
	}

	for (int grade = 0
		; grade < grade_count && grade < (int)(BIT_COUNT_OF_BYTE * sizeof(m_param.week_chuan_gong_flag))
		; grade++)
	{
		//已领
		if (IsSetBit(m_param.week_chuan_gong_flag, grade))
		{
			continue;
		}

		//配置
		const ShiTuChuanGongGradeCfg * grade_cfg
			= LOGIC_CONFIG->GetShiTuConfig()->GetChuanGongGradeCfg(grade);
		if (NULL == grade_cfg)
		{
			continue;
		}

		//可领
		if (grade_cfg->score_begin > score)
		{
			break;
		}

		//记录
		SetBit(m_param.week_chuan_gong_flag, grade);

		const std::vector<ItemConfigData> * temp_vec 
			= &(taget_is_master ? grade_cfg->apprentice_reward_vec : grade_cfg->master_reward_vec);

		reward_vec.insert(reward_vec.end(), temp_vec->begin(), temp_vec->end());

		gamelog::g_log_role_activity.printf(LL_INFO, "ShiTu::FinishChuanGong "
			"| role_id[%d] target_role_id:[%d] score:[%d] grade:[%d] flag:[%d]",
			m_role_module_mgr->GetUid(), target_uid, score, 
			grade_cfg->grade, m_param.week_chuan_gong_flag);
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutList2(reward_vec))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(reward_vec, PUT_REASON_SHI_YU_CHUAN_GONG);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(reward_vec, SNED_MAIL_TYPE_DEFAULT, true);
	}
	

	if (0 < reward_exp)
	{
		reward_vec.insert(reward_vec.begin(), ItemConfigData(ItemNamespace::VIRTUAL_ITEM_EXP, false, reward_exp));
	}

	int old_score = m_param.week_chuan_gong_score_max;
	if (!reward_vec.empty())
	{//通知
		m_role_module_mgr->GetKnapsack()->NotifyGetItemListVec(
			reward_vec, GET_REASON_SHI_TU_CHUAN_GONG,
			score, old_score);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->NotifyGetItemList(0, NULL,
			GET_REASON_SHI_TU_CHUAN_GONG,
			score, old_score);
	}

	if (score > m_param.week_chuan_gong_score_max)
	{
		m_param.week_chuan_gong_score_max = score;
	}

	this->SendShiTuCommonInfo();
}

void ShiTu::OnMasterOnline(int master_uid, bool is_login)
{
	RelationInfo * relation_info = this->GetShiTuRelationInfo(master_uid);
	if (NULL == relation_info)
	{
		return;
	}

	if (!this->IsMaster(master_uid))
	{
		return;
	}
	UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(master_uid);
	if (NULL == user_node)
	{
		return;
	}
	//进度通知
	{
		if (0 != relation_info->day_online_flag)
		{
			//已通知过
		}
		else if (!UserCacheManager::Instance().IsUserOnline(master_uid))
		{
			//不在线
		}
		else if (0 != m_param.week_chuan_gong_exp_flag //传功经验已获得
			&& 0 != m_param.shi_tu_fb_reward_flag//天书传道奖励已获得
			)
		{
			//没奖励
		}
		else
		{//通知
			Protocol::SCShiTuSyncInfo info;
			info.type = is_login ? Protocol::SCShiTuSyncInfo::ROLE_ONLINE_DEF
									: Protocol::SCShiTuSyncInfo::ROLE_ONLINE_UPDATA;
			info.param_1 = master_uid;

			//客户端要完成状态
			//按位写入  完成为1
			//0:传功
			if (0 != m_param.week_chuan_gong_exp_flag)
			{
				SetBit(info.param_2, 0);
			}
			//1:师徒副本
			if (0 != m_param.shi_tu_fb_reward_flag)
			{
				SetBit(info.param_2, 1);
			}

			m_role_module_mgr->NetSend(&info, sizeof(info));

			relation_info->day_online_flag = 1;
		}
	}
}

void ShiTu::CheckShitu()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	bool need_send = false;
	for (int index = 0; index < SHITU_MAX_MASTER_COUNT; ++index)
	{
		RelationInfo & curr = m_param.relation_data.master_list[index];
		if (curr.role_id == 0)
		{
			continue;
		}

		UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(curr.role_id);
		if (NULL != user_node)
		{
			continue;
		}

		gamelog::g_log_shi_tu.printf(LL_INFO, "%s %d 1 role_id[%d] %d",
			__FUNCTION__, __LINE__, m_role_module_mgr->GetUid(), curr.role_id);

		curr.Reset();
		need_send = true;


	}

	for (int index = 0; index < SHITU_MAX_APPRENTICE_COUNT; ++index)
	{
		RelationInfo & curr = m_param.relation_data.apprentice_list[index];
		if (curr.role_id == 0)
		{
			continue;
		}

		UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(curr.role_id);
		if (NULL != user_node)
		{
			continue;
		}

		gamelog::g_log_shi_tu.printf(LL_INFO, "%s %d 2 role_id[%d] %d",
			__FUNCTION__, __LINE__, m_role_module_mgr->GetUid(), curr.role_id);

		curr.Reset();
		need_send = true;
	}

	if (need_send)
	{
		this->SendRelationInfo();
	}
}

void ShiTu::AddMaster(int uid)
{
	for (int index = 0; index < ARRAY_LENGTH(m_param.relation_data.master_list); ++index)
	{
		RelationInfo & curr = m_param.relation_data.master_list[index];
		if (curr.role_id <= 0)
		{
			curr.role_id = uid;
			curr.relation_break_time = 0;
			curr.shi_tu_time = (unsigned int)EngineAdapter::Instance().Time();
			curr.apprentice_old_cap = m_role_module_mgr->GetCapability()->GetRoleCapability();

			EventHandler::Instance().OnAddMaster(m_role_module_mgr->GetRole());
			break;;

			gamelog::g_log_shi_tu.printf(LL_INFO, "%s %d role_id[%d %s] %d",
				__FUNCTION__, __LINE__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), curr.role_id);
		}
	}

	if (this->IsMasterFull())
	{
		ShiTuSeekingManager::Instance().RemoveSeeking(m_role_module_mgr->GetRole());
	}
}

void ShiTu::AddApprentice(int uid)
{
	for (int index = 0; index < ARRAY_LENGTH(m_param.relation_data.apprentice_list); ++index)
	{
		RelationInfo & curr = m_param.relation_data.apprentice_list[index];
		if (curr.role_id <= 0)
		{
			curr.role_id = uid;
			curr.relation_break_time = 0;
			curr.shi_tu_time = (unsigned int)EngineAdapter::Instance().Time();
			UserCacheNode * user_node
				= UserCacheManager::Instance().GetUserNode(uid);
			if (NULL != user_node)
			{
				curr.apprentice_old_cap = user_node->capability;
			}

			gamelog::g_log_shi_tu.printf(LL_INFO, "%s %d role_id[%d %s] %d",
				__FUNCTION__, __LINE__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), curr.role_id);

			EventHandler::Instance().OnAddApprentice(m_role_module_mgr->GetRole());
			return;
		}
	}

	if (this->IsApprenticeFull())
	{
		ShiTuSeekingManager::Instance().RemoveSeeking(m_role_module_mgr->GetRole());
	}
}

void ShiTu::DeleteMaster(int uid)
{
	for (int index = 0; index < SHITU_MAX_MASTER_COUNT; ++index)
	{
		RelationInfo & master_info = m_param.relation_data.master_list[index];
		if (master_info.role_id == uid)
		{		
			gamelog::g_log_shi_tu.printf(LL_INFO, "%s %d role_id[%d %s] %d",
				__FUNCTION__, __LINE__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), master_info.role_id);

			this->SendBreakMailNotice(uid);
			this->FetchAllMile(uid);			

			EventHandler::Instance().OnGraduation(m_role_module_mgr->GetRole(), this->GetApprenticeDayNum(master_info));
			master_info.Reset();			
			break;
		}
	}
}

void ShiTu::DeleteApprentice(int uid)
{
	for (int index = 0; index < ARRAY_LENGTH(m_param.relation_data.apprentice_list); ++index)
	{
		RelationInfo & curr = m_param.relation_data.apprentice_list[index];
		if (curr.role_id == uid)
		{
			gamelog::g_log_shi_tu.printf(LL_INFO, "%s %d role_id[%d %s] %d",
				__FUNCTION__, __LINE__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), curr.role_id);

			this->SendBreakMailNotice(uid);
			this->FetchAllMile(uid);			

			curr.Reset();
			EventHandler::Instance().OnDeleteApprentice(m_role_module_mgr->GetRole());
			return;
		}
	}
}

int ShiTu::GetApprenticeDayNum(const RelationInfo & master_info)
{
	if(master_info.role_id <= 0) return 0;

	unsigned int real_time = (unsigned int)EngineAdapter::Instance().Time();
	if (0 != master_info.relation_break_time && real_time > master_info.relation_break_time)	
	{
		real_time = master_info.relation_break_time;
	}
	return GetDayIndex(master_info.shi_tu_time, real_time) + 1;
}

void ShiTu::SendBreakTimeMailNotice(int target_uid, unsigned int break_end_time)
{
	int role_id = m_role_module_mgr->GetRoleUID();
	const char * role_name = m_role_module_mgr->GetRoleName();
	const char * target_name = "";

	{
		UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(role_id);
		if (user_node)
		{
			role_name = user_node->GetNamePtr();
		}
	}

	{
		UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(target_uid);
		if (user_node)
		{
			target_name = user_node->GetNamePtr();
		}
	}

	static MailContentParam contentparam; contentparam.Reset();
	int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_shi_tu_subject);
	int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_shitu_break_time_notice, 
		role_id, role_name, target_uid, target_name, break_end_time);
	if (lensub > 0 && length > 0)
	{
		MailRoute::MailToUser(target_uid, MAIL_REASON_DEFAULT, contentparam);
		MailRoute::MailToUser(role_id, MAIL_REASON_DEFAULT, contentparam);		
	}
}

void ShiTu::SendBreakMailNotice(int target_uid)
{
	int role_id = m_role_module_mgr->GetRoleUID();
	const char * target_name = "";
	UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(target_uid);
	if (user_node)
	{
		target_name = user_node->GetNamePtr();
	}

	static MailContentParam contentparam; contentparam.Reset();
	int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_shi_tu_subject);
	int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_shitu_break_notice, role_id,m_role_module_mgr->GetRoleName(),
		target_uid,target_name);

	if (lensub > 0 && length > 0)
	{
		MailRoute::MailToUser(role_id, MAIL_REASON_DEFAULT, contentparam);
	}
}
