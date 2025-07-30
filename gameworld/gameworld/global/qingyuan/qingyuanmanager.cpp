#include "world.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
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
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "scene/scenemanager.hpp"
#include "item/money.h"
#include "global/cross/crossmanager.h"

#include "servercommon/commonsavedef.h"
#include "qingyuan/qingyuanconfig.hpp"
#include "protocol/msgqingyuan.h"
#include "qingyuanmanager.h"
#include "qingyuan.h"

QingYuanManager & QingYuanManager::Instance()
{
	static QingYuanManager r;
	return r;
}

void QingYuanManager::Init()
{
	CommonSave * cs = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_QING_YUAN_DATA);
	assert(cs != NULL);
	if (cs != NULL)
	{
		RetCommonSaveData get_data;
		cs->BeginDataForeach();
		while (cs->GetNextData(get_data))
		{
			CommonSaveQingYuan & qy_data = get_data.data.qing_yuan_data;
			long long qy_id = qy_data.qing_yuan_id;
			if (qy_id == 0)
			{
				continue;
			}


			gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | init  qing_yuan_id:%lld ", __FUNCTION__, __LINE__, qy_id);

			QingYuan qing_yuan(qy_id);
			MapAdd(m_qing_yuan_info, qing_yuan.GetQingYuanId(), qing_yuan);

			std::vector<int> list = qing_yuan.GetMemberList();
			for (int i = 0; i < (int)list.size(); ++i)
			{
				int role_id = list[i];
				m_role_id_to_qing_yuan_id[role_id] = qy_id;
				gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | init role_id -> qing_yuan_id:%d -> %lld ", __FUNCTION__, __LINE__, role_id, qy_id);
			}
		}
	}
}

void QingYuanManager::OnGameServerConnect(std::set<int> server_id)
{
	if (server_id.empty())
	{
		return;
	}
	int send_id = *server_id.begin();
	this->SyncAllQingYuanInfoToGame(send_id);
}

void QingYuanManager::OnUserLogin(Role * role)
{
	if (!role)
	{
		return;
	}

	QingYuan * qing_yuan = this->GetQingYuanByRoleId(role->GetUID());
	if (qing_yuan)
	{
		qing_yuan->OnUserLogin(role);
	}

}

void QingYuanManager::OnUserLogout(Role * role)
{
	if (!role)
	{
		return;
	}

	QingYuan * qy = this->GetQingYuanByRoleId(role->GetUID());
	if (qy)
	{
		qy->OnUserLogout(role);
	}
}

void QingYuanManager::OnUserLoginOriginGame(int role_id)
{
	QingYuan * qy = this->GetQingYuanByRoleId(role_id);
	if (qy)
	{
		qy->ModifyMemberInfo(role_id);
	}
}

void QingYuanManager::OnUserLogoutOriginGame(int role_id)
{
	QingYuan * qy = this->GetQingYuanByRoleId(role_id);
	if (qy)
	{
		qy->ModifyMemberInfo(role_id);
	}
}

int QingYuanManager::Invite(Role * role, int target_role_id, int item_type)
{
	if (role == NULL)
	{
		ret_line;
	}

	if (!CrossManager::Instance().IsHiddenServerAlive())
	{
		role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
		ret_line;
	}

	int role_id = role->GetUID();
	if (role_id == target_role_id)
	{
		ret_line;
	}

	Role * target_role = World::GetInstWorld()->GetSceneManager()->GetRole(target_role_id);
	if (!target_role)
	{
		role->NoticeNum(errornum::EN_TARGET_NOT_ONLINE);
		ret_line;
	}

	if (this->HasQingYuan(role_id) || this->HasQingYuan(target_role_id))
	{
		role->NoticeNum(errornum::EN_HAS_QING_YUAN);
		ret_line;
	}

	QingYuanInviteInfo info;
	info.target_role_id = target_role_id;
	info.item_type = item_type;
	info.invite_time = EngineAdapter::Instance().Time();
	MapAddOrUpdate(m_invite_map, role_id, info);

	Protocol::SCQingYuanInviteRoute protocol;
	protocol.inviter_role_id = role->GetUID();
	protocol.be_inviter_role_id = target_role_id;
	protocol.item_type = item_type;
	EngineAdapter::Instance().NetSend(target_role->GetNetId(), &protocol, sizeof(protocol));
	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));

	return 0;
}

int QingYuanManager::InviteReply(Role * role, int inviter_role_id, bool is_agree)
{
	if (role == NULL)
	{
		ret_line;
	}

	if (!CrossManager::Instance().IsHiddenServerAlive())
	{
		role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
		ret_line;
	}

	int role_id = role->GetUID();
	if (role_id == inviter_role_id)
	{
		ret_line;
	}

	Role * inviter_role = World::GetInstWorld()->GetSceneManager()->GetRole(inviter_role_id);
	if (!inviter_role)
	{
		role->NoticeNum(errornum::EN_TARGET_NOT_ONLINE);
		ret_line;
	}

	Protocol::SCQingYuanInviteRet ret;
	ret.target_role_id = role_id;
	ret.is_agree = is_agree;
	EngineAdapter::Instance().NetSend(inviter_role->GetNetId(), &ret, sizeof(ret));

	if (!is_agree)
	{
		ret_line;
	}

	const QingYuanInviteInfo * invita_item = MapValuesConstPtr(m_invite_map, inviter_role_id);
	if (!invita_item || invita_item->target_role_id != role_id)
	{
		inviter_role->NoticeNum(errornum::EN_MARRY_REPLY_TIME_OUT);
		ret_line;
	}

	const QingYuanMarryCfg * cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetQingYuanMarryCfg(invita_item->item_type);
	if (!cfg)
	{
		ret_line;
	}

	if (this->HasQingYuan(role_id) || this->HasQingYuan(inviter_role_id))
	{
		role->NoticeNum(errornum::EN_HAS_QING_YUAN);
		ret_line;
	}

	const QingYuanOtherCfg & other_cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetOtherCfg();
	int friend_intimacy = FriendIntimacyManager::Instance().GetFriendIntimacy(inviter_role_id, role_id);
	if (friend_intimacy < other_cfg.qing_yuan_need_intimacy)
	{
		role->NoticeNum(errornum::EN_HAS_QING_YUAN_INTIMACY_NOT_ENGOUH);
		inviter_role->NoticeNum(errornum::EN_HAS_QING_YUAN_INTIMACY_NOT_ENGOUH);
		ret_line;
	}

	std::vector<CommonSaveQingYuanRoleInfoItem> vec;
	std::vector<Role *> role_list;
	role_list.push_back(inviter_role);
	role_list.push_back(role);
	for (int i = 0; i < (int)role_list.size(); ++i)
	{
		Role * curr_role = role_list[i];
		if (!curr_role)
		{
			continue;
		}

		CommonSaveQingYuanRoleInfoItem item;
		item.Reset();
		item.role_id = curr_role->GetUID();
		curr_role->GetName(item.role_name);
		item.level = curr_role->GetLevel();
		item.avatar_type = curr_role->GetAvatarType();
		item.prof = curr_role->GetProfession();
		item.headshot_id = curr_role->GetHeadshotID();
		vec.push_back(item);		
	}

	if ((int)vec.size() != QING_YUAN_MAX_ROLE_NUM)
	{
		ret_line;
	}

	if (!inviter_role->GetRoleModuleManager()->GetMoney()->UseMoney(cfg->money_type, cfg->money_value, __FUNCTION__))
	{
		ret_line;
	}

	return this->QingYuanCreate(cfg->marry_type, vec);
}

int QingYuanManager::QingYuanCreate(int item_type, const std::vector<CommonSaveQingYuanRoleInfoItem> & vec)
{
	if (vec.size() != 2)
	{
		ret_line;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenQingYuanCreate create_protocol;
		create_protocol.item_type = item_type;
		for (int i = 0; i < (int)vec.size() && i < ARRAY_LENGTH(create_protocol.list); ++i)
		{
			create_protocol.list[i] = vec[i];
		}

		unsigned int send_len = sizeof(create_protocol);
		InternalComm::Instance().SendToHiddenThroughCross((char *)&create_protocol, send_len);

		ret_line;
	}

	for (int i = 0; i < (int)vec.size(); ++i)
	{
		if (this->HasQingYuan(vec[i].role_id))
		{
			ret_line;
		}
	}

	long long unique_key = 0;
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		ret_line;
	}

	QingYuan qing_yuan(unique_key);
	if (!qing_yuan.SetCreateData(vec))
	{
		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | create_qing_yuan[%lld] fail", __FUNCTION__, __LINE__, unique_key);
		ret_line;
	}

	MapAdd(m_qing_yuan_info, unique_key, qing_yuan);
	qing_yuan.SendInfo();
	qing_yuan.SyncDataHiddenToGame(0, true, item_type);
	qing_yuan.OnCreateQingYuan(item_type);
	gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | create_qing_yuan[%lld] succ", __FUNCTION__, __LINE__, unique_key);

	return 0;
}

void QingYuanManager::Gmop(Role * role, int type, int p1, int p2)
{
	if (!role)
	{
		return;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_NEED_IN_CROSS);
		return;
	}

	int role_id = role->GetUID();
	if (type == 1)
	{
		this->OnDayChange();
	}


	QingYuan * qy = this->GetQingYuanByRoleId(role_id);
	if (qy)
	{
		qy->Gmop(role, type, p1, p2);
	}
}

void QingYuanManager::UpDataQingYuanCommond(long long qingyuan_id, CommonSaveQingYuanCommonData & common_data)
{
	if (qingyuan_id <= 0 || !CrossConfig::Instance().IsHiddenServer())
		return;

	QingYuan * qing_yuan = this->GetQingYuanByQingYuanId(qingyuan_id);
	if (NULL == qing_yuan)
		return;

	qing_yuan->SetSyncCommonData(common_data);
}

void QingYuanManager::UpDataQingYuanLeiChongAddChongZhi(long long qingyuan_id, int add_chongzhi)
{
	if (qingyuan_id <= 0 || !CrossConfig::Instance().IsHiddenServer())
		return;

	QingYuan * qing_yuan = this->GetQingYuanByQingYuanId(qingyuan_id);
	if (NULL == qing_yuan)
		return;

	qing_yuan->RAQingYuanLeiChongAddChongZhi(add_chongzhi);
}

bool QingYuanManager::HasQingYuan(int role_id)
{
	return this->GetQingYuanByRoleId(role_id) != NULL;
}

QingYuan * QingYuanManager::GetQingYuanByRoleId(int role_id)
{
	for (std::map<long long, QingYuan> ::iterator iter = m_qing_yuan_info.begin(); iter != m_qing_yuan_info.end(); ++iter)
	{
		std::vector<int> list = iter->second.GetMemberList();
		for (int i = 0; i < (int)list.size(); ++i)
		{
			if (list[i] == role_id)
			{
				return &iter->second;
			}
		}
	}

	return NULL;
}

QingYuan * QingYuanManager::GetQingYuanByQingYuanId(long long qing_yuan_id)
{
	if (qing_yuan_id == 0)
	{
		return NULL;
	}

	return MapValuesPtr(m_qing_yuan_info, qing_yuan_id);
}

void QingYuanManager::GetQingYuanInfoByRoleId(int role_id, int & qing_yuan_role_id, GameName & qing_yuan_name)
{
	for (std::map<long long, QingYuan> ::iterator iter = m_qing_yuan_info.begin(); iter != m_qing_yuan_info.end(); ++iter)
	{
		std::vector<int> list = iter->second.GetMemberList();
		for (int i = 0; i < (int)list.size(); ++i)
		{
			if (list[i] == role_id)
			{
				CommonSaveQingYuanRoleInfoItem * qing_yuan_target = iter->second.GetOtherSideMemberData(role_id, false);
				if (NULL != qing_yuan_target)
				{
					qing_yuan_role_id = qing_yuan_target->role_id;
					F_STRNCPY(qing_yuan_name, qing_yuan_target->role_name, sizeof(qing_yuan_name));
				}
			}
		}
	}
}

void QingYuanManager::DismissQingYuan(long long qing_yuan_id, const char * reson)
{
	if (reson == NULL)
	{
		reson = "";
	}

	QingYuan * qy = this->GetQingYuanByQingYuanId(qing_yuan_id);
	if (qy)
	{
		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | qing_yuan_id[%lld] succ", __FUNCTION__, __LINE__, qing_yuan_id);
		qy->OnDismissQingYuan(reson);
		m_qing_yuan_info.erase(qing_yuan_id);
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameQingYuanDismiss protocol;
		protocol.qing_yuan_id = qing_yuan_id;
		InternalComm::Instance().SendToAllGameThroughCross((char *)&protocol, sizeof(protocol));
	}
}

void QingYuanManager::OnRoleLeave(int role_id, long long qing_yuan_id, const char * reason)
{
	if (reason == NULL)
	{
		reason = "";
	}

	gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | clear role_id to qing_yuan_id:%d to %lld reason:%s", __FUNCTION__, __LINE__, role_id, qing_yuan_id, reason);

	const long long * ptr = MapValuesConstPtr(m_role_id_to_qing_yuan_id, role_id);
	if (ptr != NULL && *ptr == qing_yuan_id)
	{
		m_role_id_to_qing_yuan_id.erase(role_id);
		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | real clear role_id to qing_yuan_id:%d to %lld reason:%s", __FUNCTION__, __LINE__, role_id, qing_yuan_id, reason);
	}
}

void QingYuanManager::OnRoleAdd(long long qy_id, int role_id, const char * reason)
{
	if (reason == NULL)
	{
		reason = "";
	}

	gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | role_id to qing_yuan_id:%d to %lld reason:%s", __FUNCTION__, __LINE__, role_id, qy_id, reason);
	m_role_id_to_qing_yuan_id[role_id] = qy_id;
}

QingYuanManager::QingYuanManager()
{
}

QingYuanManager::~QingYuanManager()
{
}

void QingYuanManager::OnDayChange()
{
	std::set<long long> need_del_list;
	for (std::map<long long, QingYuan>::iterator iter = m_qing_yuan_info.begin(); iter != m_qing_yuan_info.end(); ++iter)
	{
		long long qing_yuan_id = iter->first;
		QingYuan & curr = iter->second;
		curr.OnDayChange();
	}
}

void QingYuanManager::Update(unsigned long interval, time_t now_second)
{
	this->UpdateQingYuan(interval, now_second);
}

void QingYuanManager::UpdateQingYuan(unsigned long interval, time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	std::set<long long> need_del_list;
	for (std::map<long long, QingYuan>::iterator iter = m_qing_yuan_info.begin(); iter != m_qing_yuan_info.end(); ++iter)
	{
		long long zu_id = iter->first;
		QingYuan & curr = iter->second;
		curr.Update(interval, now_second);

		if (curr.CanDismissQingYuan())
		{
			need_del_list.insert(zu_id);
		}
	}

	for (std::set<long long>::iterator iter = need_del_list.begin(); iter != need_del_list.end(); ++iter)
	{
		this->DismissQingYuan(*iter, __FUNCTION__);
	}
}

void QingYuanManager::SyncAllQingYuanInfoToGame(int send_server_id)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d |  start send_server_id:%d", __FUNCTION__, __LINE__, send_server_id);

	{
		crossgameprotocal::HiddenGameQingYuanSyncInfo protocol;
		protocol.sync_type = QING_YUAN_SYNC_TYPE_START;
		InternalComm::Instance().SendToGameThroughCross2((char *)&protocol, sizeof(protocol), send_server_id);
	}

	{
		for (std::map<long long, QingYuan>::iterator iter = m_qing_yuan_info.begin(); iter != m_qing_yuan_info.end(); ++iter)
		{
			QingYuan & curr = iter->second;

			gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d |  qing_yuan_info:%s ", __FUNCTION__, __LINE__, curr.ToString().c_str());
			curr.SyncDataHiddenToGame(send_server_id);
		}
	}

	{
		crossgameprotocal::HiddenGameQingYuanSyncInfo protocol;
		protocol.sync_type = QING_YUAN_SYNC_TYPE_END;
		InternalComm::Instance().SendToGameThroughCross2((char *)&protocol, sizeof(protocol), send_server_id);
	}

	gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d |  end send_server_id:%d", __FUNCTION__, __LINE__, send_server_id);
}

void QingYuanManager::OnSyncQingYuanInfoFromHiddenStart()
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	m_sync_qing_yuan_id_list.clear();
}

void QingYuanManager::OnSyncQingYuanInfoFromHidden(const CommonSaveQingYuan & qing_yuan_data, bool is_create,int item_type)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	long long qing_yuan_id = qing_yuan_data.qing_yuan_id;

	QingYuan * zu_ptr = this->GetQingYuanByQingYuanId(qing_yuan_id);
	if (zu_ptr)
	{
		zu_ptr->SetSyncData(qing_yuan_data);
	}
	else
	{
		QingYuan zu(qing_yuan_id);
		zu.SetSyncData(qing_yuan_data);
		MapAdd(m_qing_yuan_info, qing_yuan_id, zu);

		if (is_create)
		{
			zu.OnCreateQingYuan(item_type);
		}
	}

	m_sync_qing_yuan_id_list.insert(qing_yuan_id);
}

void QingYuanManager::OnSyncQingYuanInfoFromHiddenEnd()
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	std::set<long long> dismiss_list;
	for (std::map<long long, QingYuan>::iterator iter = m_qing_yuan_info.begin(); iter != m_qing_yuan_info.end(); ++iter)
	{
		long long zu_id = iter->first;
		if (m_sync_qing_yuan_id_list.find(zu_id) != m_sync_qing_yuan_id_list.end())
		{
			continue;
		}
		dismiss_list.insert(zu_id);
	}

	for (std::set<long long>::iterator iter = dismiss_list.begin(); iter != dismiss_list.end(); ++iter)
	{
		this->DismissQingYuan(*iter, __FUNCTION__);
	}
}

QingYuanManagerLoadSuccCallback::QingYuanManagerLoadSuccCallback()
{
}

QingYuanManagerLoadSuccCallback::~QingYuanManagerLoadSuccCallback()
{
}

void QingYuanManagerLoadSuccCallback::Callback()
{
	QingYuanManager::Instance().Init();
}
