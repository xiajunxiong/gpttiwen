#include "world.h"
#include "global/team/team.hpp"
#include "obj/character/role.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "internalcomm.h"
#include "global/usercache/usercache.hpp"
#include "gamelog.h"
#include "friend/friendintimacy.hpp"
#include "config/logicconfigmanager.hpp"
#include "servercommon/errornum.h"
#include "global/worldstatus/worldstatus.hpp"
//#include "global/commonsave/commonsavemanager.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"

#include "global/jieyi/jieyizuconfig.h"
#include "servercommon/commonsavedef.h"
#include "protocol/msgjieyi.h"
#include "jieyizumanager.h"

JieYiZuManager & JieYiZuManager::Instance()
{
	static JieYiZuManager r;
	return r;
}

void JieYiZuManager::Init()
{
	CommonSave * cs = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_JIE_YI_ZU);
	assert(cs != NULL);
	if (cs != NULL)
	{
		RetCommonSaveData get_data;
		cs->BeginDataForeach();
		while (cs->GetNextData(get_data))
		{
			long long jie_yi_zu_id = get_data.data.jie_yi_zu_data.jie_yi_zu_id;
			if (jie_yi_zu_id == 0)
			{
				continue;
			}
			CommonSaveJieYiZu & jie_yi_zu_data = get_data.data.jie_yi_zu_data;

			gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d | init  jie_yi_zu_id:%lld jie_yi_name:%s", __FUNCTION__, __LINE__, jie_yi_zu_id, JieYIZuNameToString(jie_yi_zu_data.jie_yi_name).c_str());

			JieYiZu zu(jie_yi_zu_id);
			MapAdd(m_jie_yi_zu_info, zu.GetJieYiZuId(), zu);

			std::vector<int> list = zu.GetMemberList();
			for (int i = 0; i < (int)list.size(); ++i)
			{
				int role_id = list[i];
				m_role_id_to_jie_yi_zu_id[role_id] = jie_yi_zu_id;
				gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d | init role_id to jie_yi_zu_id:%d to %lld ", __FUNCTION__, __LINE__, role_id, jie_yi_zu_id);
			}
		}
	}

	this->CheckRepeatMember();
}

void JieYiZuManager::CheckRepeatMember()
{
	std::map<int, std::set<long long> > check_repeat;  // 判断哪些玩家是又两个结义组的，保留加入时间最新的那个结义组

	for (std::map<long long, JieYiZu>::iterator iter = m_jie_yi_zu_info.begin(); iter != m_jie_yi_zu_info.end(); ++iter)
	{
		JieYiZu & curr = iter->second;
		long long jie_yi_zu_id = curr.GetJieYiZuId();

		std::vector<int> list = curr.GetMemberList();
		for (int i = 0; i < (int)list.size(); ++i)
		{
			int role_id = list[i];
			check_repeat[role_id].insert(jie_yi_zu_id);
		}
	}

	for (std::map<int, std::set<long long> >::iterator iter = check_repeat.begin(); iter != check_repeat.end(); ++iter)
	{
		int role_id = iter->first;
		std::set<long long> jie_yi_zu_id_list = iter->second;

		if (jie_yi_zu_id_list.size() <= 1)
		{
			continue;
		}

		long long newly_jie_yi_zu_id = 0;
		long long newly_join_time = 0;
		for (std::set<long long> ::iterator iter2 = jie_yi_zu_id_list.begin(); iter2 != jie_yi_zu_id_list.end(); ++iter2)
		{
			long long jie_yi_zu_id = *iter2;

			JieYiZu * jie_yi_zu = this->GetJieYiZuByZuId(jie_yi_zu_id);
			if (!jie_yi_zu) continue;

			CommonSaveJieYiZuRoleInfoItem * member = jie_yi_zu->GetZuMemberData(role_id);
			if (!member) continue;

			if (newly_jie_yi_zu_id == 0)
			{
				newly_jie_yi_zu_id = jie_yi_zu_id;
				newly_join_time = member->join_time;
				continue;
			}

			if (newly_join_time < member->join_time)
			{
				newly_jie_yi_zu_id = jie_yi_zu_id;
				newly_join_time = member->join_time;
			}
		}

		for (std::set<long long> ::iterator iter2 = jie_yi_zu_id_list.begin(); iter2 != jie_yi_zu_id_list.end(); ++iter2)
		{
			long long jie_yi_zu_id = *iter2;
			if (jie_yi_zu_id == newly_jie_yi_zu_id) continue;

			JieYiZu * jie_yi_zu = this->GetJieYiZuByZuId(jie_yi_zu_id);
			if (!jie_yi_zu) continue;

			CommonSaveJieYiZuRoleInfoItem * member = jie_yi_zu->GetZuMemberData(role_id);
			if (!member) continue;

			member->Reset();
			gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d | Reset repeat member:%d jie_yi_zu_id:%lld jie_yi_name:%s", __FUNCTION__, __LINE__, role_id, jie_yi_zu_id, JieYIZuNameToString(jie_yi_zu->GetJieYiZuName()).c_str());

			jie_yi_zu->AutoSetZuZhang();
		}

		this->OnRoleAdd(newly_jie_yi_zu_id, role_id, __FUNCTION__);
	}
}

void JieYiZuManager::OnGameServerConnect(std::set<int> server_id)
{
	if (server_id.empty())
	{
		return;
	}
	int send_id = *server_id.begin();
	this->SyncAllJieYiZuInfoToGame(send_id);
}

void JieYiZuManager::OnUserLogin(Role * role)
{
	if (!role)
	{
		return;
	}

	JieYiZu * jie_yi_zu = this->GetJieYiZuByRoleId(role->GetUID());
	if (jie_yi_zu)
	{
		jie_yi_zu->OnUserLogin(role);
	}
}

void JieYiZuManager::OnUserLogout(Role * role)
{
	if (!role)
	{
		return;
	}

	this->CancelCreateJieYiZuByRoleId(role->GetUID(), JIE_YI_CANCEL_TYPE_LOGOUT);

	JieYiZu * jie_yi_zu = this->GetJieYiZuByRoleId(role->GetUID());
	if (jie_yi_zu)
	{
		jie_yi_zu->OnUserLogout(role);
	}
}

void JieYiZuManager::OnUserLoginOriginGame(int role_id)
{
	JieYiZu * jie_yi_zu = this->GetJieYiZuByRoleId(role_id);
	if (jie_yi_zu)
	{
		jie_yi_zu->OnUserLoginOriginGame(role_id);
	}
}

void JieYiZuManager::OnUserLogoutOriginGame(int role_id)
{
	JieYiZu * jie_yi_zu = this->GetJieYiZuByRoleId(role_id);
	if (jie_yi_zu)
	{
		jie_yi_zu->ModifyMemberInfo(role_id);
		jie_yi_zu->NoticeOnlineStatus(role_id, UserStatusNode::STATUS_TYPE_OFFLINE);
	}
}

void JieYiZuManager::OnRoleDayChange(Role * role)
{
	if (NULL == role)
	{
		return;
	}

	JieYiZu * jie_yi_zu = this->GetJieYiZuByRoleId(role->GetUID());
	if (jie_yi_zu)
	{
		jie_yi_zu->OnDayChange(role);
	}
}

void JieYiZuManager::Update(unsigned long interval, time_t now_second)
{
	this->UpdateModVer(interval, now_second);
	this->UpdateCreateInfo(interval, now_second);
	this->UpdateJieYiZu(interval, now_second);
	this->UpdateJieYiXuanYan(interval, now_second);
}

void JieYiZuManager::UpdateModVer(unsigned long interval, time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	WorldStatusCommonDataParam & common_data = WorldStatus::Instance().GetCommonData();
	if (common_data.jie_yi_mod_ver == 0)
	{
		common_data.jie_yi_mod_ver += 1;

		for (std::map<long long, JieYiZu>::iterator iter = m_jie_yi_zu_info.begin(); iter != m_jie_yi_zu_info.end(); ++iter)
		{
			JieYiZu & curr = iter->second;
			int zu_zhang_role_id = curr.GetZuZhangRoleId();
			std::vector<int> vec = curr.GetMemberList();
			for (int i = 0; i < (int)vec.size(); ++i)
			{
				int role_id = vec[i];

				// 发邮件
				UNSTD_STATIC_CHECK(MAX_ATTACHMENT_ITEM_NUM >= 2);

				const JieYiOtherCfg & ohter_cfg = LOGIC_CONFIG->GetJieYiConfig()->GetOtherCfg();
				static MailContentParam contentparam; contentparam.Reset();
				contentparam.item_list[0].item_id = ohter_cfg.jie_yi_member_name_consume_item_id;
				contentparam.item_list[0].num = ohter_cfg.jie_yi_member_name_consume_num;;
				contentparam.item_list[0].is_bind = 1;

				if (role_id == zu_zhang_role_id)
				{
					contentparam.item_list[1].item_id = ohter_cfg.jie_yi_name_consume_item_id;
					contentparam.item_list[1].num = ohter_cfg.jie_yi_name_consume_num;
					contentparam.item_list[1].is_bind = 1;
				}

				int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_jie_yi_mod_ver_0_subject);
				int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_jie_yi_mod_ver_0_content);
				if (length1 > 0 && length2 > 0)
				{
					MailRoute::MailToUser(role_id, MAIL_REASON_DEFAULT, contentparam);
				}
			}
		}
	}
}

void JieYiZuManager::UpdateCreateInfo(unsigned long interval, time_t now_second)
{
	std::set<int> finish;
	std::set<int> cancel;

	for (std::map<int, JieYiZuCreate>::iterator iter = m_jie_yi_zu_create.begin(); iter != m_jie_yi_zu_create.end(); ++iter)
	{
		int key = iter->first;
		JieYiZuCreate & create_info = iter->second;
		if (create_info.IsAllConfirm())
		{
			finish.insert(key);
		}
		else if (now_second >= create_info.end_time)
		{
			cancel.insert(key);
		}
	}

	for (std::set<int> ::iterator iter = finish.begin(); iter != finish.end(); ++iter)
	{
		this->OnCreateAllConfirm(*iter);
	}

	for (std::set<int> ::iterator iter = cancel.begin(); iter != cancel.end(); ++iter)
	{
		this->CancelCreateJieYiZuByKey(*iter, JIE_YI_CANCEL_TYPE_TIMEOUT);
	}
}

void JieYiZuManager::UpdateJieYiZu(unsigned long interval, time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	std::set<long long> need_del_list;
	for (std::map<long long, JieYiZu>::iterator iter = m_jie_yi_zu_info.begin(); iter != m_jie_yi_zu_info.end(); ++iter)
	{
		long long zu_id = iter->first;
		JieYiZu & curr = iter->second;
		curr.Update(interval, now_second);

		if (curr.GetMemberCount() <= 1)
		{
			need_del_list.insert(zu_id);
		}
	}

	for (std::set<long long>::iterator iter = need_del_list.begin(); iter != need_del_list.end(); ++iter)
	{
		this->DismissJieYiZu(*iter);
	}
}

bool SortJieYiRoleXuanYan(const RetCommonSaveData & a, const RetCommonSaveData & b)
{
	if (a.data.jie_yi_role_xuan_yan_data.time < b.data.jie_yi_role_xuan_yan_data.time)
		return true;
	if (a.data.jie_yi_role_xuan_yan_data.time > b.data.jie_yi_role_xuan_yan_data.time)
		return false;

	if (a.data.jie_yi_role_xuan_yan_data.role_id < b.data.jie_yi_role_xuan_yan_data.role_id)
		return true;
	if (a.data.jie_yi_role_xuan_yan_data.role_id > b.data.jie_yi_role_xuan_yan_data.role_id)
		return false;

	return false;
}

bool SortJieYiZuXuanYan(const RetCommonSaveData & a, const RetCommonSaveData & b)
{
	if (a.data.jie_yi_zu_xuan_yan_data.time < b.data.jie_yi_zu_xuan_yan_data.time)
		return true;
	if (a.data.jie_yi_zu_xuan_yan_data.time > b.data.jie_yi_zu_xuan_yan_data.time)
		return false;

	if (a.data.jie_yi_zu_xuan_yan_data.jie_yi_zu_id < b.data.jie_yi_zu_xuan_yan_data.jie_yi_zu_id)
		return true;
	if (a.data.jie_yi_zu_xuan_yan_data.jie_yi_zu_id > b.data.jie_yi_zu_xuan_yan_data.jie_yi_zu_id)
		return false;

	return false;
}

void JieYiZuManager::UpdateJieYiXuanYan(unsigned long interval, time_t now_second)
{
	if (now_second < m_next_check_xuan_yan_time)
	{
		return;
	}

	m_next_check_xuan_yan_time = now_second + 600;

	typedef bool (*JieYiXuanYanSortFunc)(const RetCommonSaveData &, const RetCommonSaveData &);
	int check_type_list[] = { COMMON_SAVE_DATA_TYPE_JIE_YI_ROLE_XUAN_YAN ,COMMON_SAVE_DATA_TYPE_JIE_YI_ZU_XUAN_YAN };
	JieYiXuanYanSortFunc fuction_list[] = { SortJieYiRoleXuanYan ,SortJieYiZuXuanYan };
	for (int index = 0; index < ARRAY_LENGTH(check_type_list) && index < ARRAY_LENGTH(fuction_list); ++index)
	{
		int type = check_type_list[index];
		if (!fuction_list[index])
		{
			continue;
		}
		CommonSave * cs = CommonSaveManager::Instance().GetCommonSave(type);
		if (cs == NULL)
		{
			return;
		}

		std::vector<RetCommonSaveData> all_data = cs->GetAllData();
		if ((int)all_data.size() > JIE_YI_XUAN_YAN_ITEM_MAX_NUM)
		{
			int delete_num = all_data.size() - JIE_YI_XUAN_YAN_ITEM_MAX_NUM;
			std::sort(all_data.begin(), all_data.end(), fuction_list[index]);
			for (int i = 0; i < delete_num && i < (int)all_data.size(); ++i)
			{
				cs->RemoveData(all_data[i].key);
			}
		}
	}
}

void JieYiZuManager::CreateJieYiZu(Role * role, const  JieYIZuName & name)
{
	if (!role)
	{
		return;
	}

	Team * team = role->GetMyTeam();
	if (!team)
	{
		return;
	}

	if (!team->IsLeader(role))
	{
		return;
	}

	Role * leader = team->GetLeaderRole();
	if (!leader)
	{
		return;
	}

	this->CancelCreateJieYiZuByRoleId(role->GetUID(), JIE_YI_CANCEL_TYPE_ROLE);

	JieYiZuCreate create;
	create.key = this->GetCreateKey();
	create.jie_yi_name = name;
	create.end_time = EngineAdapter::Instance().Time() + 300;

	int leader_uid = leader->GetUID();
	for (int member_idx = 0; member_idx < team->GetMemberCount(); member_idx++)
	{
		TeamMember * member = team->GetMemberInfo(member_idx);
		if (!member)
		{
			continue;
		}

		if (this->HasJieYiZu(member->uid))
		{
			team->SendAllMemberIncompatibleNotice(member_idx, Protocol::TEAM_MEMBER_INCOMPATIBLE_HAS_JIE_YI_ZU);
			return;
		}

		if (leader_uid != member->uid)
		{
			const JieYiOtherCfg & ohter_cfg = LOGIC_CONFIG->GetJieYiConfig()->GetOtherCfg();
			int r = FriendIntimacyManager::Instance().GetFriendIntimacy(leader_uid, member->uid);
			if (r < ohter_cfg.intimacy_request)
			{
				role->NoticeNum(errornum::EN_JIE_YI_QIN_MI_DU_BU_ZU);
				return;
			}
		}

		JieYiZuCreateRole item;
		item.role_base_info.uid = member->uid;
		SNPRINTF(item.role_base_info.name, sizeof(item.role_base_info.name), "%s", member->member_name);
		item.role_base_info.role_level = member->level;
		item.role_base_info.avatar_type = member->avatar_type;
		item.role_base_info.prof = member->profession;
		item.role_base_info.headshot_id = member->appearance.headshot_id;
		item.is_zu_zhang = leader->GetUID() == member->uid;
		MapAdd(create.role_info, item.role_base_info.uid, item);
	}

	if (create.role_info.size() <= 1)
	{
		return;
	}

	std::map<int, JieYiZuCreate>::iterator iter = MapAdd(m_jie_yi_zu_create, create.key, create);
	iter->second.SendInfo();
}

void JieYiZuManager::CreateJieYiZuName(int role_id, const JieYIZuName & name)
{
	JieYiZuCreate * create_info = this->GetCreateInfoByRoleId(role_id);
	if (!create_info || create_info->is_confirm_stage)
	{
		return;
	}

	JieYiZuCreateRole * role_info = create_info->GetRoleInfo(role_id);
	if (!role_info || !role_info->is_zu_zhang)
	{
		return;
	}

	create_info->jie_yi_name = name;
	create_info->SendNameUpdate();
}

void JieYiZuManager::CreateJieYiZuNameFinish(int role_id)
{
	JieYiZuCreate * create_info = this->GetCreateInfoByRoleId(role_id);
	if (!create_info)
	{
		return;
	}

	JieYiZuCreateRole * role_info = create_info->GetRoleInfo(role_id);
	if (!role_info || !role_info->is_zu_zhang)
	{
		return;
	}

	if (create_info->is_confirm_stage)
	{
		return;
	}

	create_info->is_confirm_stage = true;
	create_info->SendInfo();
}

void JieYiZuManager::CreateJieYiZuMemberConfirm(int role_id)
{
	JieYiZuCreate * create_info = this->GetCreateInfoByRoleId(role_id);
	if (!create_info)
	{
		return;
	}

	if (!create_info->is_confirm_stage)
	{
		return;
	}

	JieYiZuCreateRole * role_info = create_info->GetRoleInfo(role_id);
	if (!role_info)
	{
		return;
	}

	role_info->is_confirm = 1;
	create_info->SendConfirm();
}

void JieYiZuManager::OnCreateAllConfirm(int key)
{
	JieYiZuCreate * create_info = this->GetCreateInfoByKey(key);
	if (!create_info)
	{
		return;
	}

	std::vector<CommonSaveJieYiZuRoleInfoItem> vec;
	for (std::map<int, JieYiZuCreateRole>::iterator iter = create_info->role_info.begin(); iter != create_info->role_info.end(); ++iter)
	{
		const JieYiZuCreateRole & role_item = iter->second;

		CommonSaveJieYiZuRoleInfoItem item;
		item.Reset();
		item.role_id = role_item.role_base_info.uid;
		SNPRINTF(item.role_name, sizeof(item.role_name), "%s", role_item.role_base_info.name);
		item.level = role_item.role_base_info.role_level;
		item.avatar_type = role_item.role_base_info.avatar_type;
		item.prof = role_item.role_base_info.prof;
		item.headshot_id = role_item.role_base_info.headshot_id;
		item.is_zu_zhang = (unsigned char)role_item.is_zu_zhang;
		vec.push_back(item);
	}

	this->RealCreateJieYiZu(create_info->jie_yi_name, vec);
	m_jie_yi_zu_create.erase(key);
}

void JieYiZuManager::RealCreateJieYiZu(const JieYIZuName & jie_yi_name, const std::vector<CommonSaveJieYiZuRoleInfoItem> & vec)
{
	if (vec.size() <= 1)
	{
		return;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenJieYiZuCreate create_protocol;
		create_protocol.jie_yi_name = jie_yi_name;
		create_protocol.count = 0;
		for (int i = 0; i < (int)vec.size() && create_protocol.count < ARRAY_LENGTH(create_protocol.list); ++i)
		{
			create_protocol.list[create_protocol.count] = vec[i];
			create_protocol.count += 1;
		}

		unsigned int send_len = sizeof(create_protocol) - (ARRAY_LENGTH(create_protocol.list) - create_protocol.count) * sizeof(create_protocol.list[0]);
		assert(send_len < sizeof(create_protocol));
		InternalComm::Instance().SendToHiddenThroughCross((char *)&create_protocol, send_len);

		return;
	}

	long long unique_key = 0;
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		return;
	}

	for (int i = 0; i < (int)vec.size(); ++i)
	{
		const CommonSaveJieYiZuRoleInfoItem & curr = vec[i];
		if (this->HasJieYiZu(curr.role_id,true))
		{
			gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d | create_jie_yi_zu[%lld] role[%d %s] ", __FUNCTION__, __LINE__, unique_key, curr.role_id, curr.role_name);
			return;
		}
	}

	for (int i = 0; i < (int)vec.size(); ++i)
	{
		const CommonSaveJieYiZuRoleInfoItem & curr = vec[i];
		gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d | create_jie_yi_zu[%lld] role[%d %s] ", __FUNCTION__, __LINE__, unique_key, curr.role_id, curr.role_name);
	}

	JieYiZu zu(unique_key);
	if (!zu.SetCreateData(jie_yi_name, vec))
	{
		gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d | create_jie_yi_zu[%lld] fail", __FUNCTION__, __LINE__, unique_key);
		return;
	}
	MapAdd(m_jie_yi_zu_info, unique_key, zu);
	zu.SendInfo();
	zu.SyncDataHiddenToGame();

	gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d | create_jie_yi_zu[%lld] succ", __FUNCTION__, __LINE__, unique_key);
}

void JieYiZuManager::CancelCreateJieYiZuByRoleId(int role_id, int type)
{
	JieYiZuCreate * create_info = this->GetCreateInfoByRoleId(role_id);
	if (!create_info)
	{
		return;
	}

	this->CancelCreateJieYiZuByKey(create_info->key, type, role_id);
}

void JieYiZuManager::CancelCreateJieYiZuByKey(int key, int type, int role_id)
{
	JieYiZuCreate * create_info = this->GetCreateInfoByKey(key);
	if (!create_info)
	{
		return;
	}

	create_info->SendCancelInfo(role_id, type);
	m_jie_yi_zu_create.erase(create_info->key);
}

void JieYiZuManager::RoleXuanYan(int role_id, bool is_delele, const JieYIZuXuanYanText * xuan_yan_text_ptr)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		if (xuan_yan_text_ptr == NULL)
		{
			return;
		}

		crossgameprotocal::GameHiddenJieYiRoleXuanYan protocol;
		protocol.is_delete = is_delele;
		protocol.role_id = role_id;
		SameArrayTypeCopy(protocol.text, *xuan_yan_text_ptr);
		InternalComm::Instance().SendToHiddenThroughCross((char *)&protocol, sizeof(protocol));
		return;
	}

	CommonSave * cs = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_JIE_YI_ROLE_XUAN_YAN);
	if (cs == NULL)
	{
		return;
	}

	if (is_delele)
	{
		cs->RemoveData(role_id);
	}
	else
	{
		if (this->HasJieYiZu(role_id))
		{
			return;
		}

		CommonSaveJieYiRoleXuanYan * data = cs->GetTypeData<CommonSaveJieYiRoleXuanYan>(role_id);
		if (!data)
		{
			return;
		}

		if (xuan_yan_text_ptr == NULL)
		{
			return;
		}

		data->role_id = role_id;
		data->time = (unsigned int)EngineAdapter::Instance().Time();
		SameArrayTypeCopy(data->xuan_yan_text, *xuan_yan_text_ptr);
		cs->SetDataChange(role_id);
	}

	this->SendRoleXuanYan(role_id);
}

void JieYiZuManager::JieYiZuXuanYan(int role_id, const JieYIZuXuanYanText & xuan_yan_text, bool is_delele)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenJieYiZuXuanYan protocol;
		protocol.is_delete = is_delele;
		protocol.role_id = role_id;
		SameArrayTypeCopy(protocol.text, xuan_yan_text);
		InternalComm::Instance().SendToHiddenThroughCross((char *)&protocol, sizeof(protocol));
		return;
	}

	JieYiZu * jie_yi_zu = this->GetJieYiZuByRoleId(role_id);
	if (!jie_yi_zu)
	{
		return;
	}

	if (!jie_yi_zu->IsZuZhang(role_id))
	{
		return;
	}

	long long jie_yi_zu_id = jie_yi_zu->GetJieYiZuId();
	this->JieYiZuXuanYanByJieYiZuId(jie_yi_zu_id, is_delele, &xuan_yan_text);
	this->SendJieYiZuXuanYan(role_id);
}

void JieYiZuManager::JieYiZuXuanYanByJieYiZuId(long long jie_yi_zu_id, bool is_delele, const JieYIZuXuanYanText * xuan_yan_text_ptr)
{
	CommonSave * cs = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_JIE_YI_ZU_XUAN_YAN);
	if (cs == NULL)
	{
		return;
	}

	CommonSaveJieYiZuXuanYan * data = cs->GetTypeData<CommonSaveJieYiZuXuanYan>(jie_yi_zu_id);
	if (!data)
	{
		return;
	}

	if (is_delele)
	{
		cs->RemoveData(jie_yi_zu_id);
	}
	else
	{
		if (xuan_yan_text_ptr == NULL)
		{
			return;
		}
		const JieYIZuXuanYanText & xuan_yan_text = *xuan_yan_text_ptr;
		data->jie_yi_zu_id = jie_yi_zu_id;
		data->time = (unsigned int)EngineAdapter::Instance().Time();
		SameArrayTypeCopy(data->xuan_yan_text, xuan_yan_text);
		cs->SetDataChange(jie_yi_zu_id);
	}
}

void JieYiZuManager::SendRoleXuanYan(int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenJieYiXuanYanList protocol;
		protocol.type = JIE_YI_XUAN_YAN_TYPE_ROLE;
		protocol.role_id = role_id;
		InternalComm::Instance().SendToHiddenThroughCross((char *)&protocol, sizeof(protocol));
		return;
	}

	Protocol::SCJieYiRoleXuanYanList protocol;
	protocol.count = 0;
	CommonSave * cs = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_JIE_YI_ROLE_XUAN_YAN);
	if (cs != NULL)
	{
		std::vector<RetCommonSaveData> all_data = cs->GetAllData();
		std::sort(all_data.begin(), all_data.end(), SortJieYiRoleXuanYan);
		for (int i = 0; i < (int)all_data.size() && protocol.count < ARRAY_LENGTH(protocol.xuan_yan_items); ++i)
		{
			const CommonSaveJieYiRoleXuanYan & src = all_data[i].data.jie_yi_role_xuan_yan_data;
			SCJieYiRoleXuanYanList::XuanYanItem & dst = protocol.xuan_yan_items[protocol.count];
			UserCacheNode * node = UserCacheManager::Instance().GetUserNode(src.role_id);
			if (node == NULL)
			{
				continue;
			}

			dst.role_info.uid = node->uid;
			SameArrayTypeCopy(dst.role_info.name, node->role_name);
			dst.role_info.role_level = node->level;
			dst.role_info.prof = node->profession;
			dst.role_info.avatar_type = node->avatar_type;
			dst.role_info.headshot_id = node->appearance.headshot_id;
			SameArrayTypeCopy(dst.text, src.xuan_yan_text);
			protocol.count += 1;
		}
	}

	unsigned int send_len = sizeof(protocol) - (ARRAY_LENGTH(protocol.xuan_yan_items) - protocol.count) * sizeof(protocol.xuan_yan_items[0]);
	assert(send_len < sizeof(protocol));

	World::GetInstWorld()->SendToRoleAny(role_id, &protocol, send_len);
}

void JieYiZuManager::SendJieYiZuXuanYan(int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenJieYiXuanYanList protocol;
		protocol.type = JIE_YI_XUAN_YAN_TYPE_ZU;
		protocol.role_id = role_id;
		InternalComm::Instance().SendToHiddenThroughCross((char *)&protocol, sizeof(protocol));
		return;
	}

	Protocol::SCJieYiZuXuanYanList protocol;
	protocol.count = 0;
	CommonSave * cs = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_JIE_YI_ZU_XUAN_YAN);
	if (cs != NULL)
	{
		std::vector<RetCommonSaveData> all_data = cs->GetAllData();
		std::sort(all_data.begin(), all_data.end(), SortJieYiZuXuanYan);
		for (int i = 0; i < (int)all_data.size() && protocol.count < ARRAY_LENGTH(protocol.xuan_yan_items); ++i)
		{
			const CommonSaveJieYiZuXuanYan & src = all_data[i].data.jie_yi_zu_xuan_yan_data;
			SCJieYiZuXuanYanList::XuanYanItem & dst = protocol.xuan_yan_items[protocol.count];
			JieYiZu * jie_yi_zu = this->GetJieYiZuByZuId(src.jie_yi_zu_id);
			if (!jie_yi_zu)
			{
				continue;
			}

			dst.jie_yi_zu_id = src.jie_yi_zu_id;
			dst.zu_name = jie_yi_zu->GetJieYiZuName();
			SameArrayTypeCopy(dst.text, src.xuan_yan_text);
			protocol.count += 1;
		}
	}

	unsigned int send_len = sizeof(protocol) - (ARRAY_LENGTH(protocol.xuan_yan_items) - protocol.count) * sizeof(protocol.xuan_yan_items[0]);
	assert(send_len < sizeof(protocol));

	World::GetInstWorld()->SendToRoleAny(role_id, &protocol, send_len);
}

void JieYiZuManager::Gmop(Role * role, int type, int p1, int p2)
{
	if (!role)
	{
		return;
	}

	if (type == 1)
	{
		JieYIZuName name;
		name.Reset();
		name.str1[0] = 'a';
		name.str2[0] = 'a';
		this->CreateJieYiZu(role, name);
	}

	if (type == 2)
	{
		JieYIZuName name;
		name.Reset();
		name.str1[0] = 'a';
		name.str2[0] = 'a' + rand() % 26;
		this->CreateJieYiZuName(role->GetUID(), name);
	}

	if (type == 3)
	{
		this->CreateJieYiZuNameFinish(role->GetUID());
	}

	if (type == 4)
	{
		this->CreateJieYiZuMemberConfirm(role->GetUID());
	}

	if (type == 5)
	{
		this->CreateJieYiZuMemberConfirm(role->GetUID());
	}

	if (type == 6)
	{
		JieYIZuXuanYanText text = { "aaaa" };
		text[0] = rand() % 26 + 'a';
		this->RoleXuanYan(role->GetUID(), p1 != 0, &text);
	}

	if (type == 7)
	{
		JieYIZuXuanYanText text = { "aaaa" };
		text[0] = rand() % 26 + 'a';
		this->JieYiZuXuanYan(role->GetUID(), text, p1 != 0);
	}

	if (type == 8)
	{
		this->SendRoleXuanYan(role->GetUID());
		this->SendJieYiZuXuanYan(role->GetUID());
	}
}

JieYiZuCreate * JieYiZuManager::GetCreateInfoByRoleId(int role_id)
{
	for (std::map<int, JieYiZuCreate>::iterator iter = m_jie_yi_zu_create.begin(); iter != m_jie_yi_zu_create.end(); ++iter)
	{
		//int key = iter->first;
		JieYiZuCreate & create_info = iter->second;

		for (std::map<int, JieYiZuCreateRole> ::iterator role_info_iter = create_info.role_info.begin(); role_info_iter != create_info.role_info.end(); ++role_info_iter)
		{
			if (role_id == role_info_iter->second.role_base_info.uid)
			{
				return &create_info;
			}
		}
	}

	return NULL;
}

JieYiZuCreate * JieYiZuManager::GetCreateInfoByKey(int key)
{
	return MapValuesPtr(m_jie_yi_zu_create, key);
}

bool JieYiZuManager::HasJieYiZu(int role_id, bool is_check_all)
{
	if (is_check_all)
	{
		for (std::map<long long, JieYiZu>::iterator iter = m_jie_yi_zu_info.begin(); iter != m_jie_yi_zu_info.end(); ++iter)
		{
			JieYiZu & curr = iter->second;
			if (curr.IsZuMember(role_id))
			{
				return true;
			}
		}
	}

	return this->GetJieYiZuByRoleId(role_id) != NULL;
}

JieYiZu * JieYiZuManager::GetJieYiZuByRoleId(int role_id)
{
	const long long * jie_yi_zu_id_ptr = MapValuesConstPtr(m_role_id_to_jie_yi_zu_id, role_id);
	if (jie_yi_zu_id_ptr != NULL)
	{
		return this->GetJieYiZuByZuId(*jie_yi_zu_id_ptr);
	}

	return NULL;
}

JieYiZu * JieYiZuManager::GetJieYiZuByZuId(long long jie_yi_zu_id)
{
	if (jie_yi_zu_id == 0)
	{
		return NULL;
	}

	return MapValuesPtr(m_jie_yi_zu_info, jie_yi_zu_id);
}

void JieYiZuManager::GetJieYiZuInfoByRoleId(int role_id, JieYIZuName & jie_yi_name, int max_num, GameName * jie_yi_role_name_list)
{
	const long long * jie_yi_zu_id_ptr = MapValuesConstPtr(m_role_id_to_jie_yi_zu_id, role_id);
	if (jie_yi_zu_id_ptr != NULL)
	{
		JieYiZu * jie_yi = this->GetJieYiZuByZuId(*jie_yi_zu_id_ptr);
		if (NULL != jie_yi)
		{
			CommonSaveJieYiZu * jie_yi_info = jie_yi->GetCommonSaveJieYiZu(false);
			if (NULL != jie_yi_info)
			{
				jie_yi_name = jie_yi_info->jie_yi_name;
				jie_yi_name.member_num = jie_yi->GetMemberCount();
				if (NULL != jie_yi_role_name_list)
				{
					int index = 0;
					for (int i = 0; i < ARRAY_LENGTH(jie_yi_info->role_info) && index < max_num; ++i)
					{
						if (0 != jie_yi_info->role_info[i].role_id && role_id != jie_yi_info->role_info[i].role_id)
						{
							F_STRNCPY(jie_yi_role_name_list[index], jie_yi_info->role_info[i].role_name, sizeof(jie_yi_role_name_list[index]));
							index++;
						}
					}
				}
			}
		}
	}
}

void JieYiZuManager::DismissJieYiZu(long long jie_yi_zu_id)  // 游戏服和隐藏服都会调用，游戏服调用时机是收到隐藏服删除通知
{
	JieYiZu * jie_yi_zu = this->GetJieYiZuByZuId(jie_yi_zu_id);
	if (jie_yi_zu)
	{
		gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d | jie_yi_zu[%lld] succ", __FUNCTION__, __LINE__, jie_yi_zu_id);
		jie_yi_zu->OnDismissJieYiZu();
		m_jie_yi_zu_info.erase(jie_yi_zu_id);
		this->JieYiZuXuanYanByJieYiZuId(jie_yi_zu_id, true);		
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameJieYiZuDismiss protocol;
		protocol.jie_yi_zu_id = jie_yi_zu_id;
		InternalComm::Instance().SendToAllGameThroughCross((char *)&protocol, sizeof(protocol));
	}
}

void JieYiZuManager::OnRoleLeave(int role_id, long long jie_yi_zu_id, const char * reason)
{
	if (reason == NULL)
	{
		reason = "";
	}

	gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d | clear role_id to jie_yi_zu_id:%d to %lld reason:%s", __FUNCTION__, __LINE__, role_id, jie_yi_zu_id, reason);

	const long long * ptr = MapValuesConstPtr(m_role_id_to_jie_yi_zu_id, role_id);
	if (ptr != NULL && *ptr == jie_yi_zu_id)
	{
		m_role_id_to_jie_yi_zu_id.erase(role_id);
		gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d | real clear role_id to jie_yi_zu_id:%d to %lld reason:%s", __FUNCTION__, __LINE__, role_id, jie_yi_zu_id, reason);
	}
}

void JieYiZuManager::OnRoleAdd(long long jie_yi_zu_id, int role_id, const char * reason)
{
	if (reason == NULL)
	{
		reason = "";
	}

	gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d | role_id to jie_yi_zu_id:%d to %lld reason:%s", __FUNCTION__, __LINE__, role_id, jie_yi_zu_id, reason);
	m_role_id_to_jie_yi_zu_id[role_id] = jie_yi_zu_id;	
}

void JieYiZuManager::SyncAllJieYiZuInfoToGame(int send_server_id)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d |  start send_server_id:%d", __FUNCTION__, __LINE__, send_server_id);

	{
		crossgameprotocal::HiddenGameJieYiZuSync protocol;
		protocol.sync_type = JIE_YI_ZU_SYNC_TYPE_START;
		InternalComm::Instance().SendToGameThroughCross2((char *)&protocol, sizeof(protocol), send_server_id);
	}

	{
		for (std::map<long long, JieYiZu>::iterator iter = m_jie_yi_zu_info.begin(); iter != m_jie_yi_zu_info.end(); ++iter)
		{			
			JieYiZu & curr = iter->second;

			gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d |  jie_yi_zu_id:%lld jie_yi_name:%s", __FUNCTION__, __LINE__, curr.GetJieYiZuId(), JieYIZuNameToString(curr.GetJieYiZuName()).c_str());
			curr.SyncDataHiddenToGame(send_server_id);
		}
	}

	{
		crossgameprotocal::HiddenGameJieYiZuSync protocol;
		protocol.sync_type = JIE_YI_ZU_SYNC_TYPE_END;
		InternalComm::Instance().SendToGameThroughCross2((char *)&protocol, sizeof(protocol), send_server_id);
	}

	gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d |  end send_server_id:%d", __FUNCTION__, __LINE__, send_server_id);
}

void JieYiZuManager::OnSyncJieYiZuInfoFromHiddenStart()
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	m_sync_jie_yi_zu_id_list.clear();
}

void JieYiZuManager::OnSyncJieYiZuInfoFromHidden(const CommonSaveJieYiZu & jie_yi_zu)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	JieYiZu * zu_ptr = this->GetJieYiZuByZuId(jie_yi_zu.jie_yi_zu_id);
	if (zu_ptr)
	{
		zu_ptr->SetSyncData(jie_yi_zu);
	}
	else
	{
		long long unique_key = jie_yi_zu.jie_yi_zu_id;
		JieYiZu zu(unique_key);
		zu.SetSyncData(jie_yi_zu);
		MapAdd(m_jie_yi_zu_info, unique_key, zu);
	}
	m_sync_jie_yi_zu_id_list.insert(jie_yi_zu.jie_yi_zu_id);
}

void JieYiZuManager::OnSyncJieYiZuInfoFromHiddenEnd()
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	std::set<long long> dismiss_list;
	for (std::map<long long, JieYiZu>::iterator iter = m_jie_yi_zu_info.begin(); iter != m_jie_yi_zu_info.end(); ++iter)
	{
		long long zu_id = iter->first;
		//JieYiZu & curr = iter->second;
		if (m_sync_jie_yi_zu_id_list.find(zu_id) != m_sync_jie_yi_zu_id_list.end())
		{
			continue;
		}
		dismiss_list.insert(zu_id);
	}

	for (std::set<long long>::iterator iter = dismiss_list.begin(); iter != dismiss_list.end(); ++iter)
	{
		this->DismissJieYiZu(*iter);
	}
}

JieYiZuCreate::JieYiZuCreate()
{
	key = 0;
	is_confirm_stage = false;
	jie_yi_name.Reset();
	end_time = 0;
}

bool JieYiZuCreate::IsAllConfirm() const
{
	if (!this->is_confirm_stage)
	{
		return false;
	}

	for (std::map<int, JieYiZuCreateRole>::const_iterator iter = role_info.begin(); iter != role_info.end(); ++iter)
	{
		if (!iter->second.is_confirm)
		{
			return false;
		}
	}

	return true;
}

JieYiZuCreateRole * JieYiZuCreate::GetRoleInfo(int role_id)
{
	for (std::map<int, JieYiZuCreateRole>::iterator iter = role_info.begin(); iter != role_info.end(); ++iter)
	{
		if (iter->second.role_base_info.uid == role_id)
		{
			return &iter->second;
		}
	}

	return NULL;
}

void JieYiZuCreate::SendInfo() const
{
	Protocol::SCJieYiCreateInfo protocol;
	protocol.is_name_finish = this->is_confirm_stage;
	protocol.jie_yi_name = this->jie_yi_name;
	protocol.role_count = 0;
	for (std::map<int, JieYiZuCreateRole>::const_iterator iter = role_info.begin(); iter != role_info.end() && protocol.role_count < ARRAY_LENGTH(protocol.role_info); ++iter)
	{
		protocol.role_info[protocol.role_count++] = iter->second;
	}

	for (std::map<int, JieYiZuCreateRole>::const_iterator iter = role_info.begin(); iter != role_info.end(); ++iter)
	{
		int role_id = iter->first;
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (role)
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));;
		}
	}
}

void JieYiZuCreate::SendCancelInfo(int role_id, int type) const
{
	Protocol::SCJieYiCreateCancel protocol;
	protocol.type = type;
	protocol.key = this->key;
	protocol.role_id = role_id;

	for (std::map<int, JieYiZuCreateRole>::const_iterator iter = role_info.begin(); iter != role_info.end(); ++iter)
	{
		int role_id = iter->first;
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (role)
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));;
		}
	}
}

void JieYiZuCreate::SendNameUpdate() const
{
	Protocol::SCJieYiCreateNameUpdate protocol;
	protocol.key = this->key;
	protocol.jie_yi_name = this->jie_yi_name;

	for (std::map<int, JieYiZuCreateRole>::const_iterator iter = role_info.begin(); iter != role_info.end(); ++iter)
	{
		int role_id = iter->first;
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (role)
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));;
		}
	}
}

void JieYiZuCreate::SendConfirm() const
{
	Protocol::SCJieYiCreateConfirm protocol;
	protocol.key = this->key;

	for (int i = 0; i < ARRAY_LENGTH(protocol.role_id); ++i)
	{
		protocol.role_id[i] = 0;
	}

	int count = 0;
	for (std::map<int, JieYiZuCreateRole>::const_iterator iter = role_info.begin(); iter != role_info.end() && count < ARRAY_LENGTH(protocol.role_id); ++iter)
	{
		if (!iter->second.is_confirm)
		{
			continue;
		}

		protocol.role_id[count++] = iter->first;
	}

	for (std::map<int, JieYiZuCreateRole>::const_iterator iter = role_info.begin(); iter != role_info.end(); ++iter)
	{
		int role_id = iter->first;
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (role)
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));;
		}
	}
}


JieYiZuManager::JieYiZuManager()
{
	m_next_check_xuan_yan_time = 0;
}

JieYiZuManager::~JieYiZuManager()
{
}

int JieYiZuManager::GetCreateKey()
{
	static int key = 1;
	return key++;
}

JieYiZuManagerLoadSuccCallback::JieYiZuManagerLoadSuccCallback()
{
}

JieYiZuManagerLoadSuccCallback::~JieYiZuManagerLoadSuccCallback()
{
}

void JieYiZuManagerLoadSuccCallback::Callback()
{
	JieYiZuManager::Instance().Init();
}

std::string JieYIZuNameToString(const JieYIZuName & jie_yi_name)
{
	std::string r = STRING_SPRINTF("%s%s_%d", jie_yi_name.str1, jie_yi_name.str2, jie_yi_name.member_num);
	return r;
}
