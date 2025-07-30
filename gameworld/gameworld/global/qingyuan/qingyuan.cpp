#include "world.h"
#include "obj/character/role.h"
#include "global/usercache/usercache.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "internalcomm.h"
#include "friend/friendintimacy.hpp"
#include "friend/friendintimacy.hpp"
#include "config/logicconfigmanager.hpp"
#include "item/knapsack.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/errornum.h"
#include "global/cross/crossmanager.h"
#include "gamelog.h"
#include "other/title/title.hpp"

#include "global/commonsave/commonsavemanager.hpp"
#include "qingyuan/qingyuanconfig.hpp"
#include "protocol/msgqingyuan.h"
#include "qingyuanmanager.h"
#include "qingyuan.h"
#include "other/event/eventhandler.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityqingyuanshizhuang.hpp"
#include "other/roleactivity/impl/roleactivityqingyuanleichong.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"

bool QingYuan::SetCreateData(const std::vector<CommonSaveQingYuanRoleInfoItem> & list)
{
	CommonSaveQingYuan * zu_info_ptr = this->GetCommonSaveQingYuan(true);
	if (!zu_info_ptr)
	{
		return false;
	}

	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	CommonSaveQingYuan & zu_info = *zu_info_ptr;
	zu_info.qing_yuan_id = m_unique_id;
	zu_info.create_time = now;
	zu_info.has_hun_yan = 0;
	for (int i = 0; i < (int)list.size() && i < ARRAY_LENGTH(zu_info.role_info); ++i)
	{
		zu_info.role_info[i] = list[i];
	}

	return true;
}

int QingYuan::SetSyncData(const CommonSaveQingYuan & data)
{
	CommonSaveQingYuan * zu_info_ptr = this->GetCommonSaveQingYuan(true);
	if (!zu_info_ptr)
	{
		return -__LINE__;
	}

	std::set<int> leave_role_id;
	for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
	{
		leave_role_id.insert(zu_info_ptr->role_info[i].role_id);
	}

	for (int i = 0; i < ARRAY_LENGTH(data.role_info); ++i)
	{
		leave_role_id.erase(data.role_info[i].role_id);
	}

	*zu_info_ptr = data;
	//this->SendDismissInfo(leave_role_id);  
	this->SendInfo();
	this->SendUpDataActivityInfo();

	gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d |    qing_yuan_info:%s", __FUNCTION__, __LINE__, this->ToString().c_str());

	for (std::set<int>::iterator iter = leave_role_id.begin(); iter != leave_role_id.end(); ++iter)
	{
		this->OnRoleLeave(*iter, __FUNCTION__);
	}

	std::vector<int> list = this->GetMemberList();
	for (int i = 0; i < (int)list.size(); ++i)
	{
		this->OnRoleAdd(list[i], __FUNCTION__);
	}

	return 0;
}

int QingYuan::SetSyncCommonData(const CommonSaveQingYuanCommonData & data)
{
	CommonSaveQingYuan * zu_info_ptr = this->GetCommonSaveQingYuan(true);
	if (!zu_info_ptr)
	{
		return -__LINE__;
	}

	memcpy(&zu_info_ptr->common_data, &data, sizeof(CommonSaveQingYuanCommonData));

	this->SendInfo();
	this->SendUpDataActivityInfo();

	gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d |    qing_yuan_info:%s", __FUNCTION__, __LINE__, this->ToString().c_str());

	this->SyncDataHiddenToGame();

	return 0;
}

void QingYuan::DeleteData()
{
	m_common_save->RemoveData(this->GetQingYuanId());
	m_data = NULL;
}

void QingYuan::SyncDataHiddenToGame(int server_id, bool is_create, int item_type)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	CommonSaveQingYuan * data = this->GetCommonSaveQingYuan(false);
	if (!data)
	{
		return;
	}

	gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d |  send_server_id:%d %s", __FUNCTION__, __LINE__, server_id, this->ToString().c_str());

	crossgameprotocal::HiddenGameQingYuanSyncInfo protocol;
	protocol.sync_type = QING_YUAN_SYNC_TYPE_UPDATE;
	protocol.is_create = is_create;
	protocol.item_type = item_type;
	protocol.qing_yuan_info = *data;
	InternalComm::Instance().SendToGameThroughCross2((char *)&protocol, sizeof(protocol), server_id);
}

void QingYuan::SendMailAllMember(int reason, const MailContentParam & contentparam, int exclude_role_id)
{
	const CommonSaveQingYuan * zu_info_ptr = this->GetCommonSaveQingYuan(false);
	if (!zu_info_ptr)
	{
		return;
	}

	for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
	{
		int role_id = zu_info_ptr->role_info[i].role_id;
		if (role_id == 0 || role_id == exclude_role_id)
		{
			continue;
		}

		MailRoute::MailToUser(role_id, reason, contentparam);
	}
}

void QingYuan::SetDataChange()
{
	long long qing_yuan_id = this->GetQingYuanId();
	m_common_save->SetDataChange(qing_yuan_id);
}

void QingYuan::OnRoleLeave(int role_id, const char * reason)
{
	this->DismissBuFa(role_id);
	QingYuanManager::Instance().OnRoleLeave(role_id, this->GetQingYuanId(), reason);
}

void QingYuan::OnRoleAdd(int role_id, const char * reason)
{
	QingYuanManager::Instance().OnRoleAdd(this->GetQingYuanId(), role_id, reason);
}

QingYuan::QingYuan(long long unique_id)
{
	m_unique_id = unique_id;
	m_common_save = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_QING_YUAN_DATA);
	assert(m_common_save != NULL);
	m_data = m_common_save->GetTypeData<CommonSaveQingYuan>(m_unique_id, true);
	assert(m_data != NULL);
	m_can_dismiss = false;
}

QingYuan::~QingYuan()
{
}

void QingYuan::OnUserLogin(Role * role)
{
	if (role==NULL)
	{
		return;
	}

	int role_id = role->GetUID();
	this->SendInfo(role);
	this->ModifyMemberInfo(role_id);
}

void QingYuan::OnUserLogout(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	int role_id = role->GetUID();
	this->ModifyMemberInfo(role_id);
}

void QingYuan::ModifyMemberInfo(int role_id)
{
	UserCacheNode * node = UserCacheManager::Instance().GetUserNode(role_id);
	if (node == NULL)
	{
		return;
	}

	CommonSaveQingYuanRoleInfoItem * member_data = this->GetMemberData(role_id);
	if (member_data == NULL)
	{
		return;
	}

	node->GetName(member_data->role_name);
	member_data->level = node->level;
	member_data->avatar_type = node->avatar_type;
	member_data->prof = node->GetProfession();
	member_data->headshot_id = node->appearance.headshot_id;
	member_data->last_login_time = (unsigned int)EngineAdapter::Instance().Time();

	this->SyncDataHiddenToGame();   //这些信息暂时没什么用，客户端也是直接用好友的信息，日志会用到一些名字的，就不修改后同步到游戏服。
}

void QingYuan::OnDayChange()
{
	//修改数据要只能隐藏服修改
	this->CheackTitle();
	this->CheackQingYuanDurationReward();
	this->ResetGiftDayFetchFlag();
}

void QingYuan::Update(unsigned long interval, time_t now_second)
{
	this->UpdateLeave(interval, now_second);
}

void QingYuan::UpdateLeave(unsigned long interval, time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	CommonSaveQingYuan * zu_info_ptr = this->GetCommonSaveQingYuan(false);
	if (!zu_info_ptr)
	{
		return;
	}

	m_can_dismiss = false;
	if (zu_info_ptr->leave_end_time != 0 && now_second > zu_info_ptr->leave_end_time)
	{
		m_can_dismiss = true;
		return;
	}

	for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
	{
		CommonSaveQingYuanRoleInfoItem & curr = zu_info_ptr->role_info[i];
		if (curr.role_id == 0)
		{
			continue;
		}

		if (curr.leave_end_time != 0 && now_second > curr.leave_end_time)
		{
			m_can_dismiss = true;
			break;
		}
	}
}

int QingYuan::GiveGift(Role * role, int gift_type)
{
	if (!CrossManager::Instance().IsHiddenServerAlive())
	{
		role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
		ret_line;
	}

	int role_id = role->GetUID();
	CommonSaveQingYuanRoleInfoItem * other_data = this->GetOtherSideMemberData(role_id);
	if (!other_data)
	{
		ret_line;
	}

	time_t now = EngineAdapter::Instance().Time();
	if (now < other_data->gift_type_end_time)
	{
		role->NoticeNum(errornum::EN_QING_YUAN_CANT_GIVE);
		ret_line;
	}

	const QingYuanGiftCfg * gift_cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetQingYuanGiftCfg(gift_type);
	if (!gift_cfg)
	{
		ret_line;
	}

	if (!role->GetRoleModuleManager()->GetMoney()->UseMoney(gift_cfg->need_money_type, gift_cfg->need_money_value, __FUNCTION__))
	{
		ret_line;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenQingYuanGiftGive active_reward;
		active_reward.role_id = role_id;
		active_reward.gift_type = gift_type;

		InternalComm::Instance().SendToHiddenThroughCross((char *)&active_reward, sizeof(active_reward));
		return 0;
	}

	this->ActiveGift(role_id, gift_type);
	return 0;
}

int QingYuan::GiveGiftFromGame(int role_id, int gift_type)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		ret_line;
	}

	return this->ActiveGift(role_id, gift_type);
}

int QingYuan::ActiveGift(int role_id, int gift_type)
{
	CommonSaveQingYuanRoleInfoItem * member_data = this->GetMemberData(role_id);
	if (!member_data)
	{
		ret_line;
	}

	CommonSaveQingYuanRoleInfoItem * other_data = this->GetOtherSideMemberData(role_id);
	if (!other_data)
	{
		ret_line;
	}

	time_t now = EngineAdapter::Instance().Time();
	unsigned int zero_time = (unsigned int)::GetZeroTime(now);
	other_data->gift_type = gift_type;
	other_data->fetch_gift_flag = 0;
	other_data->gift_type_end_time = zero_time + 30 * SECOND_PER_DAY;
	this->SyncDataHiddenToGame();
	this->SendInfo();

	// 发邮件
	static MailContentParam contentparam; contentparam.Reset();
	int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_qing_yuan_give_gift_subject);
	int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_qing_yuan_give_gift_content, role_id, member_data->role_name, gift_type);
	if (length1 > 0 && length2 > 0)
	{
		MailRoute::MailToUser(other_data->role_id, MAIL_REASON_DEFAULT, contentparam);
	}

	return 0;
}

int QingYuan::GiftFetch(Role * role)
{
	if (role == NULL)
	{
		ret_line;
	}

	int role_id = role->GetUID();
	if (!CrossManager::Instance().IsHiddenServerAlive())
	{
		role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
		ret_line;
	}

	CommonSaveQingYuanRoleInfoItem * member_data = this->GetMemberData(role_id);
	if (!member_data)
	{
		ret_line;
	}

	if (IsSetBit(member_data->fetch_gift_flag, QING_YUAN_GIFT_FETCH_TYPE_ACTIVE))
	{
		role->NoticeNum(errornum::EN_QING_YUAN_GIFT_ALREADY_FETCH);
		ret_line;
	}

	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	if (now > member_data->gift_type_end_time)
	{
		ret_line;
	}

	const QingYuanGiftCfg * gift_cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetQingYuanGiftCfg(member_data->gift_type);
	if (!gift_cfg)
	{
		ret_line;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenQingYuanGiftFetchReward fetch_reward;
		fetch_reward.role_id = role_id;
		InternalComm::Instance().SendToHiddenThroughCross((char *)&fetch_reward, sizeof(fetch_reward));
		return 0;
	}

	role->GetRoleModuleManager()->GetMoney()->OnAddMoney(gift_cfg->active_reward_money_type, gift_cfg->active_reward_money_value, __FUNCTION__);
	SetBit(member_data->fetch_gift_flag, QING_YUAN_GIFT_FETCH_TYPE_ACTIVE);
	this->SendInfo();
	this->SyncDataHiddenToGame();

	return 0;
}

int QingYuan::GiftFetchFromGame(int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		ret_line;
	}

	CommonSaveQingYuanRoleInfoItem * member_data = this->GetMemberData(role_id);
	if (!member_data)
	{
		ret_line;
	}

	if (IsSetBit(member_data->fetch_gift_flag, QING_YUAN_GIFT_FETCH_TYPE_ACTIVE))
	{
		ret_line;
	}

	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	if (now > member_data->gift_type_end_time)
	{
		ret_line;
	}

	const QingYuanGiftCfg * gift_cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetQingYuanGiftCfg(member_data->gift_type);
	if (!gift_cfg)
	{
		ret_line;
	}

	SetBit(member_data->fetch_gift_flag, QING_YUAN_GIFT_FETCH_TYPE_ACTIVE);
	this->SyncDataHiddenToGame();

	crossgameprotocal::HiddenGameQingYuanGiftFetchRewardRet fetch_reward_ret;
	fetch_reward_ret.role_id = role_id;
	fetch_reward_ret.gift_type = gift_cfg->qing_yuan_gift_type;
	InternalComm::Instance().SendToAllGameThroughCross((char *)&fetch_reward_ret, sizeof(fetch_reward_ret));

	return 0;
}

int QingYuan::GiftFetchFromGameRet(int role_id, int gift_type)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		ret_line;
	}

	const QingYuanGiftCfg * gift_cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetQingYuanGiftCfg(gift_type);
	if (!gift_cfg)
	{
		ret_line;
	}

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	if (!role)
	{
		ret_line;
	}

	role->GetRoleModuleManager()->GetMoney()->OnAddMoney(gift_cfg->active_reward_money_type, gift_cfg->active_reward_money_value, __FUNCTION__);

	return 0;
}

int QingYuan::GiveFetchDay(Role * role)
{
	if (role == NULL)
	{
		ret_line;
	}

	int role_id = role->GetUID();
	if (!CrossManager::Instance().IsHiddenServerAlive())
	{
		role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
		ret_line;
	}

	CommonSaveQingYuanRoleInfoItem * member_data = this->GetMemberData(role_id);
	if (!member_data)
	{
		ret_line;
	}

	if (IsSetBit(member_data->fetch_gift_flag, QING_YUAN_GIFT_FETCH_TYPE_DAY))
	{
		role->NoticeNum(errornum::EN_QING_YUAN_GIFT_ALREADY_FETCH);
		ret_line;
	}

	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	if (now > member_data->gift_type_end_time)
	{
		ret_line;
	}

	const QingYuanGiftCfg * gift_cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetQingYuanGiftCfg(member_data->gift_type);
	if (!gift_cfg)
	{
		ret_line;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList2(gift_cfg->day_rewards))
	{
		role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		ret_line;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenQingYuanGiftFetchDayReward fetch_day_reward;
		fetch_day_reward.role_id = role_id;
		InternalComm::Instance().SendToHiddenThroughCross((char *)&fetch_day_reward, sizeof(fetch_day_reward));
		return 0;
	}

	role->GetRoleModuleManager()->GetKnapsack()->PutVec(gift_cfg->day_rewards, PUT_REASON_QING_YUAN_GIFT_DAY_REWARD);
	SetBit(member_data->fetch_gift_flag, QING_YUAN_GIFT_FETCH_TYPE_DAY);
	this->SendInfo();
	this->SyncDataHiddenToGame();

	return 0;
}

int QingYuan::GiveFetchDayFromGame(int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		ret_line;
	}

	CommonSaveQingYuanRoleInfoItem * member_data = this->GetMemberData(role_id);
	if (!member_data)
	{
		ret_line;
	}

	if (IsSetBit(member_data->fetch_gift_flag, QING_YUAN_GIFT_FETCH_TYPE_DAY))
	{
		ret_line;
	}

	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	if (now > member_data->gift_type_end_time)
	{
		ret_line;
	}

	const QingYuanGiftCfg * gift_cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetQingYuanGiftCfg(member_data->gift_type);
	if (!gift_cfg)
	{
		ret_line;
	}

	SetBit(member_data->fetch_gift_flag, QING_YUAN_GIFT_FETCH_TYPE_DAY);
	this->SyncDataHiddenToGame();

	crossgameprotocal::HiddenGameQingYuanGiftFetchDayRewardRet fetch_reward_ret;
	fetch_reward_ret.role_id = role_id;
	fetch_reward_ret.gift_type = gift_cfg->qing_yuan_gift_type;
	InternalComm::Instance().SendToAllGameThroughCross((char *)&fetch_reward_ret, sizeof(fetch_reward_ret));

	return 0;
}

int QingYuan::GiveFetchDayFromGameRet(int role_id, int gift_type)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		ret_line;
	}

	const QingYuanGiftCfg * gift_cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetQingYuanGiftCfg(gift_type);
	if (!gift_cfg)
	{
		ret_line;
	}

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	if (!role)
	{
		ret_line;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->PutVec(gift_cfg->day_rewards, PUT_REASON_QING_YUAN_GIFT_DAY_REWARD))
	{
		ret_line;
	}

	return 0;
}

int QingYuan::Leave(Role * role)
{
	if (NULL == role)
	{
		ret_line;
	}

	int role_id = role->GetUID();
	CommonSaveQingYuan * data = this->GetCommonSaveQingYuan();
	if (!data)
	{
		ret_line;
	}

	if (data->leave_end_time != 0)
	{
		ret_line;
	}

	CommonSaveQingYuanRoleInfoItem * other_member_data = this->GetOtherSideMemberData(role_id);
	if (!other_member_data)
	{
		ret_line;
	}

	Role * other_role = World::GetInstWorld()->GetSceneManager()->GetRole(other_member_data->role_id);
	if (!other_role)
	{
		role->NoticeNum(errornum::EN_TARGET_NOT_ONLINE);
		ret_line;
	}

	Protocol::SCQingYuanLeaveRoute protocol;
	EngineAdapter::Instance().NetSend(other_role->GetNetId(), &protocol, sizeof(protocol));

	return 0;
}

int QingYuan::LeaveReply(Role * role, bool is_agree)
{
	int role_id = role->GetUID();
	int other_role_id = this->GetOtherSideRoleId(role_id);
	if (!is_agree)
	{
		World::GetInstWorld()->NoticeNumAny(other_role_id, errornum::EN_QING_YUAN_DISAGREE_DISMISS);
		return 0;
	}

	CommonSaveQingYuan * data = this->GetCommonSaveQingYuan();
	if (!data)
	{
		ret_line;
	}

	if (data->leave_end_time != 0)
	{
		ret_line;
	}

	time_t now = EngineAdapter::Instance().Time();
	const QingYuanOtherCfg & other_cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetOtherCfg();
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenQingYuanLeave leave;
		leave.qing_yuan_id = this->GetQingYuanId();
		InternalComm::Instance().SendToHiddenThroughCross((char *)&leave, sizeof(leave));
		return 0;
	}

	data->leave_end_time = (unsigned int)now + other_cfg.qing_yuan_leave_wait_time;
	World::GetInstWorld()->NoticeNumAny(other_role_id, errornum::EN_QING_YUAN_AGREE_DISMISS);

	this->SendInfo();
	this->SyncDataHiddenToGame();

	return 0;
}

int QingYuan::SetLeave()
{
	CommonSaveQingYuan * data = this->GetCommonSaveQingYuan();
	if (!data)
	{
		ret_line;
	}

	if (data->leave_end_time != 0)
	{
		ret_line;
	}

	time_t now = EngineAdapter::Instance().Time();
	const QingYuanOtherCfg & other_cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetOtherCfg();
	data->leave_end_time = (unsigned int)now + other_cfg.qing_yuan_leave_wait_time;

	this->SendInfo();
	this->SyncDataHiddenToGame();

	return 0;
}

int QingYuan::LeaveSingle(Role * role)
{
	if (!role)
	{
		ret_line;
	}

	if (!CrossManager::Instance().IsHiddenServerAlive())
	{
		role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
		ret_line;
	}

	int role_id = role->GetUID();
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenQingYuanSingleLeave leave;
		leave.role_id = role_id;
		InternalComm::Instance().SendToHiddenThroughCross((char *)&leave, sizeof(leave));
		return 0;
	}

	CommonSaveQingYuanRoleInfoItem * member_data = this->GetMemberData(role_id);
	if (!member_data)
	{
		ret_line;
	}

	CommonSaveQingYuanRoleInfoItem * other_member_data = this->GetOtherSideMemberData(role_id);
	if (!other_member_data)
	{
		ret_line;
	}

	time_t now = EngineAdapter::Instance().Time();
	const QingYuanOtherCfg & other_cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetOtherCfg();

	UserCacheNode * user_cache = UserCacheManager::Instance().GetUserNode(other_member_data->role_id);
	if (user_cache)
	{
		if (now < user_cache->last_online_timestamp + other_cfg.qing_yuan_logout_duration_can_leave)
		{
			World::GetInstWorld()->NoticeNumAny(role_id, errornum::EN_QING_YUAN_CANT_SINGLE_LEAVE);
			ret_line;
		}
	}

	member_data->leave_end_time = (unsigned int)now + other_cfg.qing_yuan_leave_wait_time;

	this->SendInfo();
	this->SyncDataHiddenToGame();

	return 0;
}

int QingYuan::LeaveSingleFromGame(int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		ret_line;
	}

	CommonSaveQingYuanRoleInfoItem * member_data = this->GetMemberData(role_id);
	if (!member_data)
	{
		ret_line;
	}

	CommonSaveQingYuanRoleInfoItem * other_member_data = this->GetOtherSideMemberData(role_id);
	if (!other_member_data)
	{
		ret_line;
	}

	const QingYuanOtherCfg & other_cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetOtherCfg();
	time_t now = EngineAdapter::Instance().Time();
	if (now < other_member_data->last_login_time + other_cfg.qing_yuan_logout_duration_can_leave)
	{
		ret_line;
	}

	member_data->leave_end_time = (unsigned int)now + other_cfg.qing_yuan_leave_wait_time;

	this->SendInfo();
	this->SyncDataHiddenToGame();

	return 0;
}

int QingYuan::LeaveForce(Role * role)
{
	if (!role)
	{
		ret_line;
	}

	if (!CrossManager::Instance().IsHiddenServerAlive())
	{
		role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
		ret_line;
	}

	int role_id = role->GetUID();

	CommonSaveQingYuanRoleInfoItem * member_data = this->GetMemberData(role_id);
	if (!member_data)
	{
		ret_line;
	}

	if (member_data->leave_end_time != 0)
	{
		ret_line;
	}

	const QingYuanOtherCfg & other_cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetOtherCfg();
	if (!role->GetRoleModuleManager()->GetMoney()->UseGold(other_cfg.qing_yuan_force_leave_comsume_gold, __FUNCTION__))
	{
		role->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		ret_line;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenQingYuanForceLeave leave;
		leave.role_id = role_id;
		InternalComm::Instance().SendToHiddenThroughCross((char *)&leave, sizeof(leave));
		return 0;
	}

	time_t now = EngineAdapter::Instance().Time();
	member_data->leave_end_time = (unsigned int)now + other_cfg.qing_yuan_leave_wait_time;

	this->SendInfo();
	this->SyncDataHiddenToGame();

	return 0;
}

int QingYuan::LeaveForceFromGame(int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		ret_line;
	}

	CommonSaveQingYuanRoleInfoItem * member_data = this->GetMemberData(role_id);
	if (!member_data)
	{
		ret_line;
	}

	if (member_data->leave_end_time != 0)
	{
		ret_line;
	}

	const QingYuanOtherCfg & other_cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetOtherCfg();
	time_t now = EngineAdapter::Instance().Time();
	member_data->leave_end_time = (unsigned int)now + other_cfg.qing_yuan_leave_wait_time;

	this->SendInfo();
	this->SyncDataHiddenToGame();

	return 0;
}

int QingYuan::LeaveCancel(Role * role)
{
	if (!role)
	{
		ret_line;
	}

	if (!CrossManager::Instance().IsHiddenServerAlive())
	{
		role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
		ret_line;
	}

	int role_id = role->GetUID();
	CommonSaveQingYuan * data = this->GetCommonSaveQingYuan();
	if (!data)
	{
		ret_line;
	}

	CommonSaveQingYuanRoleInfoItem * member_data = this->GetMemberData(role_id);
	if (!member_data)
	{
		ret_line;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenQingYuanCancelLeave leave;
		leave.role_id = role_id;
		InternalComm::Instance().SendToHiddenThroughCross((char *)&leave, sizeof(leave));
		return 0;
	}

	data->leave_end_time = 0;
	member_data->leave_end_time = 0;

	this->SendInfo();
	this->SyncDataHiddenToGame();

	return 0;
}

int QingYuan::LeaveCancelFromGame(int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		ret_line;
	}

	CommonSaveQingYuan * data = this->GetCommonSaveQingYuan();
	if (!data)
	{
		ret_line;
	}

	CommonSaveQingYuanRoleInfoItem * member_data = this->GetMemberData(role_id);
	if (!member_data)
	{
		ret_line;
	}

	data->leave_end_time = 0;
	member_data->leave_end_time = 0;

	this->SendInfo();
	this->SyncDataHiddenToGame();

	return 0;
}

int QingYuan::SetSkillNoEffectFlag(Role * role, unsigned long long skill_no_effect_flag)
{
	if (!role)
	{
		ret_line;
	}

	if (!CrossManager::Instance().IsHiddenServerAlive())
	{
		role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
		ret_line;
	}

	int role_id = role->GetUID();
	CommonSaveQingYuanRoleInfoItem * member_data = this->GetMemberData(role_id);
	if (!member_data)
	{
		ret_line;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenQingYuanSetSkillFlag active_reward;
		active_reward.role_id = role_id;
		active_reward.skill_flag = skill_no_effect_flag;
		InternalComm::Instance().SendToHiddenThroughCross((char*)&active_reward, sizeof(active_reward));

		return 0;
	}

	member_data->skill_no_effect_flag = skill_no_effect_flag;
	this->SyncDataHiddenToGame();
	this->SendInfo();

	return 0;
}

int QingYuan::SetSkillNoEffectFlagFromGame(int role_id, unsigned long long skill_no_effect_flag)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		ret_line;
	}

	CommonSaveQingYuanRoleInfoItem * member_data = this->GetMemberData(role_id);
	if (!member_data)
	{
		ret_line;
	}

	member_data->skill_no_effect_flag = skill_no_effect_flag;
	this->SyncDataHiddenToGame();
	this->SendInfo();

	return 0;
}

void QingYuan::SetRAQingYuanShiZhuangParam(unsigned int * ra_ra_shizhuang_join_timestamp, int * buy_mark)
{
	CommonSaveQingYuan * data = this->GetCommonSaveQingYuan();
	if (NULL == data) return;

	if (NULL != ra_ra_shizhuang_join_timestamp) data->common_data.ra_ra_shizhuang_join_timestamp = *ra_ra_shizhuang_join_timestamp;
	if (NULL != buy_mark) data->common_data.ra_qingyuan_shizhuang_buy_mark = *buy_mark;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->SetDataChange();
		this->SyncDataHiddenToGame();
	}
	else
	{
		crossgameprotocal::GameHiddenQingYuanCommonData info;
		info.qingyuan_id = m_unique_id;
		memcpy(&info.common_data, &m_data->common_data, sizeof(CommonSaveQingYuanCommonData));

		InternalComm::Instance().SendToHiddenThroughCross((char *)&info, sizeof(info));
	}
}

void QingYuan::GetRAQingYuanShiZhuangParam(unsigned int * ra_ra_shizhuang_join_timestamp, int * buy_mark)
{
	if (NULL == m_data) return;

	if (NULL != ra_ra_shizhuang_join_timestamp) *ra_ra_shizhuang_join_timestamp = m_data->common_data.ra_ra_shizhuang_join_timestamp;
	if (NULL != buy_mark) *buy_mark = m_data->common_data.ra_qingyuan_shizhuang_buy_mark;
}

void QingYuan::SetRAQingYuanLeiChongParam(unsigned int * begin_times, unsigned int * end_times, int * chongzhi_gold)
{
	CommonSaveQingYuan * data = this->GetCommonSaveQingYuan();
	if (NULL == data) return;

	if (NULL != begin_times) data->common_data.ra_qingyuan_leichong_begin_timestamp = *begin_times;
	if (NULL != end_times) data->common_data.ra_qingyuan_leichong_end_timestamp = *end_times;
	if (NULL != chongzhi_gold) data->common_data.ra_qingyuan_leichong_chongzhi_gold = *chongzhi_gold;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->SetDataChange();
		this->SyncDataHiddenToGame();
	}
	else
	{
		crossgameprotocal::GameHiddenQingYuanCommonData info;
		info.qingyuan_id = m_unique_id;
		memcpy(&info.common_data, &m_data->common_data, sizeof(CommonSaveQingYuanCommonData));

		InternalComm::Instance().SendToHiddenThroughCross((char *)&info, sizeof(info));
	}
}

void QingYuan::GetRAQingYuanLeiChongParam(unsigned int * begin_times, unsigned int * end_times, int * chongzhi_gold)
{
	if (NULL == m_data) return;

	if (NULL != begin_times) *begin_times = m_data->common_data.ra_qingyuan_leichong_begin_timestamp;
	if (NULL != end_times) *end_times = m_data->common_data.ra_qingyuan_leichong_end_timestamp;
	if (NULL != chongzhi_gold) *chongzhi_gold = m_data->common_data.ra_qingyuan_leichong_chongzhi_gold;
}

void QingYuan::DismissBuFa(int role_id)
{
	if (NULL == m_data) return;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (NULL != role)
	{
		if (m_data->common_data.ra_qingyuan_shizhuang_buy_mark > 0)
		{
			RoleActivityQingYuanShiZhuang * qingyuan_shizhuang = static_cast<RoleActivityQingYuanShiZhuang *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_QING_YUAN_SHI_ZHUANG));
			if (NULL != qingyuan_shizhuang)
			{
				qingyuan_shizhuang->OnLiHunBuFa(m_data->common_data.ra_qingyuan_shizhuang_buy_mark);
			}
		}
		if (m_data->common_data.ra_qingyuan_leichong_chongzhi_gold > 0)
		{
			RoleActivityQingYuanLeiChong * qingyuan_leichong = static_cast<RoleActivityQingYuanLeiChong *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_QING_YUAN_LEI_CHONG));
			if (NULL != qingyuan_leichong)
			{
				qingyuan_leichong->OnLiHunBuFa(m_data->common_data.ra_qingyuan_leichong_chongzhi_gold);
			}
		}
	}
	else
	{
		if (!CrossConfig::Instance().IsHiddenServer())
		{
			if (m_data->common_data.ra_qingyuan_shizhuang_buy_mark > 0)
			{
				OfflineRegisterManager::Instance().OnQingYuanShiZhuangBuyMark(role_id, m_data->common_data.ra_qingyuan_shizhuang_buy_mark);
			}
			if (m_data->common_data.ra_qingyuan_leichong_chongzhi_gold > 0)
			{
				OfflineRegisterManager::Instance().OnQingYuanLeiChongGold(role_id, m_data->common_data.ra_qingyuan_leichong_chongzhi_gold);
			}
		}
	}
}

void QingYuan::RAQingYuanLeiChongAddChongZhi(int add_chongzhi)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		CommonSaveQingYuan * data = this->GetCommonSaveQingYuan();
		if (NULL == data) return;
		
		long long gold = data->common_data.ra_qingyuan_leichong_chongzhi_gold + add_chongzhi;
		if (gold > MAX_INT) gold = MAX_INT;

		data->common_data.ra_qingyuan_leichong_chongzhi_gold = gold;

		this->SetDataChange();
		this->SyncDataHiddenToGame();

		this->SendUpDataActivityInfo();

		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | qing_yuna_id[%lld] add_chongzhi[%d] total_chongzhi[%d]", __FUNCTION__, 
			__LINE__, this->GetQingYuanId(), add_chongzhi, data->common_data.ra_qingyuan_leichong_chongzhi_gold);
	}
	else
	{
		crossgameprotocal::GameHiddenQingYuanLeiChongAddChongZhi info;
		info.qingyuan_id = m_unique_id;
		info.add_chongzhi = add_chongzhi;

		InternalComm::Instance().SendToHiddenThroughCross((char *)&info, sizeof(info));
	}
}

void QingYuan::SendInfo(Role * role)
{
	Protocol::SCQingYuanInfo protocol;
	CommonSaveQingYuan * zu_info_ptr = this->GetCommonSaveQingYuan(false);
	if (!zu_info_ptr)
	{
		return;
	}

	protocol.qing_yuan_id = this->GetQingYuanId();
	protocol.has_hun_yan = zu_info_ptr->has_hun_yan;
	protocol.create_time = zu_info_ptr->create_time;
	protocol.leave_end_time = zu_info_ptr->leave_end_time;
	for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info) && i < ARRAY_LENGTH(protocol.role_info); ++i)
	{
		protocol.role_info[i].role_id = zu_info_ptr->role_info[i].role_id;
		protocol.role_info[i].single_or_force_leave_end_time = zu_info_ptr->role_info[i].leave_end_time;
		protocol.role_info[i].gift_type = zu_info_ptr->role_info[i].gift_type;
		protocol.role_info[i].fetch_gift_flag = zu_info_ptr->role_info[i].fetch_gift_flag;
		protocol.role_info[i].gift_type_end_time = zu_info_ptr->role_info[i].gift_type_end_time;
		protocol.role_info[i].skill_no_effect_flag = (unsigned int)zu_info_ptr->role_info[i].skill_no_effect_flag;
	}

	unsigned int send_len = sizeof(protocol);
	if (role == NULL)
	{
		this->SendMsgAllMember(&protocol, send_len);
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, send_len);
	}

}

void QingYuan::SendDismissInfo(const std::set<int> & leave_role_id)
{
}

void QingYuan::SendMsgAllMember(const void * msg, unsigned int send_len)
{
	const CommonSaveQingYuan * zu_info_ptr = this->GetCommonSaveQingYuan(false);
	if (!zu_info_ptr)
	{
		return;
	}

	for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
	{
		int role_id = zu_info_ptr->role_info[i].role_id;
		if (role_id == 0)
		{
			continue;
		}
		Role * tmp = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (tmp)
		{
			EngineAdapter::Instance().NetSend(tmp->GetNetId(), msg, send_len);
		}
	}
}

void QingYuan::SendMsgAllMemberAny(const void * msg, unsigned int send_len)
{
	const CommonSaveQingYuan * zu_info_ptr = this->GetCommonSaveQingYuan(false);
	if (!zu_info_ptr)
	{
		return;
	}

	for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
	{
		int role_id = zu_info_ptr->role_info[i].role_id;
		if (role_id == 0)
		{
			continue;
		}

		World::GetInstWorld()->SendToRoleAny(role_id, msg, send_len);
	}
}

void QingYuan::SendUpDataActivityInfo()
{
	const CommonSaveQingYuan * zu_info_ptr = this->GetCommonSaveQingYuan(false);
	if (!zu_info_ptr)
	{
		return;
	}

	for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
	{
		int role_id = zu_info_ptr->role_info[i].role_id;
		if (role_id == 0)
		{
			continue;
		}
		Role * tmp = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (tmp)
		{
			RoleActivityQingYuanShiZhuang * qingyuan_shizhuang = static_cast<RoleActivityQingYuanShiZhuang *>
				(tmp->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_QING_YUAN_SHI_ZHUANG));
			if (NULL != qingyuan_shizhuang)
			{
				qingyuan_shizhuang->RASendInfo();
			}

			RoleActivityQingYuanLeiChong * qingyuan_leichong = static_cast<RoleActivityQingYuanLeiChong *>
				(tmp->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_QING_YUAN_LEI_CHONG));
			if (NULL != qingyuan_leichong)
			{
				qingyuan_leichong->RASendInfo();
			}
		}
	}
}

void QingYuan::Gmop(Role * role, int type, int p1, int p2)
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
	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	if (type == 100)
	{
		CommonSaveQingYuanRoleInfoItem * member_data = this->GetMemberData(role_id);
		if (member_data)
		{
			member_data->leave_end_time =now + 1800;
		}
	}

	if (type == 101)
	{
		CommonSaveQingYuanRoleInfoItem * member_data = this->GetMemberData(role_id);
		if (member_data)
		{
			member_data->leave_end_time = now;
		}
	}

	if (type == 102)
	{
		CommonSaveQingYuan * data = this->GetCommonSaveQingYuan();
		if (data)
		{
			data->leave_end_time = now + 1800;
		}
	}


	if (type == 103)
	{
		CommonSaveQingYuan * data = this->GetCommonSaveQingYuan();
		if (data)
		{
			data->leave_end_time = now;
		}
	}

	if (type == 200)
	{
		CommonSaveQingYuan * data = this->GetCommonSaveQingYuan();
		if (data)data->leave_end_time = now;
		CommonSaveQingYuanRoleInfoItem * member_data = this->GetMemberData(role_id);
		if (member_data)member_data->leave_end_time = now;
		CommonSaveQingYuanRoleInfoItem * member_data2 = this->GetOtherSideMemberData(role_id);
		if (member_data2)member_data2->leave_end_time = now;
	}

	this->SyncDataHiddenToGame();
	this->SendInfo();
}

int QingYuan::GetQinyYuanIntimacy()
{
	std::vector<int> list = this->GetMemberList();
	if (list.size() < 2)
	{
		return 0;
	}

	int friend_intimacy = FriendIntimacyManager::Instance().GetFriendIntimacy(list[0], list[1]);
	if (0 >= friend_intimacy)
	{
		friend_intimacy = FriendIntimacyManager::Instance().GetFriendIntimacy(list[1], list[0]);
	}

	return friend_intimacy;
}

int QingYuan::GetOtherSideRoleId(int role_id)
{
	CommonSaveQingYuanRoleInfoItem * member_data = this->GetOtherSideMemberData(role_id);
	if (member_data)
	{
		return member_data->role_id;
	}

	return 0;
}

std::set<int> QingYuan::GetSkillIdList(int role_id)
{
	std::set<int> ret;
	CommonSaveQingYuanRoleInfoItem * member_data = this->GetMemberData(role_id);
	if (!member_data)
	{
		return ret;
	}

	int friend_intimacy = this->GetQinyYuanIntimacy();
	//int day = this->GetDaySinceCreate();
	std::vector<QingYuanSkillCfg> skill_cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetQingYuanSkillCfg(friend_intimacy);
	for (int i = 0; i < (int)skill_cfg.size(); ++i)
	{
		const QingYuanSkillCfg & curr = skill_cfg[i];
		if (member_data->skill_no_effect_flag & (1LL << curr.seq))
		{
			continue;
		}

		ret.insert(curr.skill_id);
	}

	return ret;
}

std::string QingYuan::ToString()
{
	std::string s;

	CommonSaveQingYuan * data = this->GetCommonSaveQingYuan();
	if (data && ARRAY_LENGTH(data->role_info) >= 2)
	{
		s = STRING_SPRINTF("qing_yuan_info[id:%lld,  role_1:%d role_name_1:%s , role_2:%d role_name_2:%s] ", this->GetQingYuanId(), data->role_info[0].role_id, data->role_info[0].role_name,
			data->role_info[1].role_id, data->role_info[1].role_name);
	}
	else
	{
		s = STRING_SPRINTF("qing_yuan_info[id:%lld] ", this->GetQingYuanId());
	}

	return s;
}

void QingYuan::OnCreateQingYuan(int item_type)
{
	this->CheackQingYuanDurationReward();

	const CommonSaveQingYuan * zu_info_ptr = this->GetCommonSaveQingYuan(false);
	if (!zu_info_ptr)
	{
		return;
	}

	for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
	{
		int role_id = zu_info_ptr->role_info[i].role_id;
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (role)
		{
			role->GetRoleModuleManager()->GetTitle()->CalQingYuanTitle();

			const QingYuanMarryCfg * cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetQingYuanMarryCfg(item_type);
			if (cfg)
			{
				role->GetRoleModuleManager()->GetKnapsack()->PutVec(cfg->rewards, PUT_REASON_QING_YUAN_MARRAY_REWARD);
			}

			EventHandler::Instance().OnQingYuanJieHun(role, m_unique_id);
		}
	}

	if (ARRAY_LENGTH(zu_info_ptr->role_info) >= 2)
	{	// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_qing_yuan_create_broadcast, zu_info_ptr->role_info[0].role_id, zu_info_ptr->role_info[0].role_name,
			zu_info_ptr->role_info[1].role_id, zu_info_ptr->role_info[1].role_name);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}

		Protocol::SCQingYuanCreateNotice notice;
		notice.role_id[0] = zu_info_ptr->role_info[0].role_id;
		notice.role_id[1] = zu_info_ptr->role_info[1].role_id;

		World::GetInstWorld()->SendToAllGateway((const char *)&notice, sizeof(notice));
	}


}

void QingYuan::OnDismissQingYuan(const char * reson)
{
	if (reson == NULL)
	{
		reson = "";
	}

	gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | qing_yu na_id[%lld] reson:%s", __FUNCTION__, __LINE__, this->GetQingYuanId(), reson);


	std::vector<int> list = this->GetMemberList();
	if (CrossConfig::Instance().IsHiddenServer())
	{
		const tm * now_tm = EngineAdapter::Instance().LocalTime();
		if (now_tm)
		{
			static MailContentParam contentparam; contentparam.Reset();
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_qing_yuan_dismiss_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_qing_yuan_dismiss_content, 1900 + now_tm->tm_year, now_tm->tm_mon + 1, now_tm->tm_mday, this->GetDaySinceCreate());
			this->SendMailAllMember(MAIL_REASON_DEFAULT, contentparam);
		}

		if (list.size() >= 2)
		{
			FriendIntimacyManager::Instance().ClearIntimacy(list[0], list[1], __FUNCTION__);
		}
	}

	Protocol::SCQingYuanLeaveNotice notice;
	notice.qing_yuan_id = this->GetQingYuanId();
	this->SendMsgAllMember(&notice,sizeof(notice));
	this->ClearTitle();

	for (int i = 0; i < (int)list.size(); ++i)
	{
		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | qing_yu na_id[%lld] role_id[%d]", __FUNCTION__, __LINE__, this->GetQingYuanId(), list[i]);
		this->OnRoleLeave(list[i], reson);
	}

	this->DeleteData();
}

bool QingYuan::CanDismissQingYuan()
{
	return m_can_dismiss;
}

long long QingYuan::GetQingYuanId()
{
	return m_unique_id;
}

int QingYuan::GetDaySinceCreate()
{
	long long day = 0;
	CommonSaveQingYuan * cs = this->GetCommonSaveQingYuan(false);
	if (!cs)
	{
		return (int)day;
	}

	time_t now = EngineAdapter::Instance().Time();
	day = (now - ::GetZeroTime(cs->create_time)) / SECOND_PER_DAY + 1;
	return (int)day;
}

std::vector<int> QingYuan::GetMemberList()
{
	std::vector<int> ret;
	const CommonSaveQingYuan * zu_info_ptr = this->GetCommonSaveQingYuan(false);
	if (zu_info_ptr)
	{
		for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
		{
			int role_id = zu_info_ptr->role_info[i].role_id;
			if (role_id == 0)
			{
				continue;
			}
			ret.push_back(role_id);
		}
	}

	return ret;
}

std::set<int> QingYuan::GetQingYuanTitle()
{
	int friend_intimacy = this->GetQinyYuanIntimacy();
	int day = this->GetDaySinceCreate();
	std::set<int>  ret = LOGIC_CONFIG->GetQingYuanConfig()->GetQingYuanTitle(friend_intimacy, day);

	return ret;
}

void QingYuan::CheackTitle()
{
	CommonSaveQingYuan * zu_info_ptr = this->GetCommonSaveQingYuan(false);
	if (!zu_info_ptr)
	{
		return;
	}

	for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
	{
		int role_id = zu_info_ptr->role_info[i].role_id;
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (role)
		{
			role->GetRoleModuleManager()->GetTitle()->CalQingYuanTitle();
		}
	}
}

void QingYuan::ClearTitle()
{
	CommonSaveQingYuan * zu_info_ptr = this->GetCommonSaveQingYuan(false);
	if (!zu_info_ptr)
	{
		return;
	}

	std::set<int> exclude_title_id;
	for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
	{
		int role_id = zu_info_ptr->role_info[i].role_id;
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (role)
		{
			role->GetRoleModuleManager()->GetTitle()->RemoveTitleBySrcType(TITLESOURCETYPE_QING_YUAN, __FUNCTION__, TITLE_NOTICE_TYPE_DEFAULT, exclude_title_id);
		}
	}
}

void QingYuan::CheackQingYuanDurationReward()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	CommonSaveQingYuan * qing_yuan_data = this->GetCommonSaveQingYuan();
	if (!qing_yuan_data)
	{
		return;
	}

	int day = this->GetDaySinceCreate();
	std::vector < QingYuanDurationRewardCfg > cfgs = LOGIC_CONFIG->GetQingYuanConfig()->GetQingYuanDurationCfg(qing_yuan_data->qing_yuan_duration_reward_already_fetch_day, day);
	if (cfgs.empty())
	{
		return;
	}

	for (int cfgs_index = 0; cfgs_index < (int)cfgs.size(); ++cfgs_index)
	{
		const QingYuanDurationRewardCfg & curr = cfgs[cfgs_index];
		qing_yuan_data->qing_yuan_duration_reward_already_fetch_day = std::max(qing_yuan_data->qing_yuan_duration_reward_already_fetch_day, curr.need_marry_day);
		

		static MailContentParam contentparam;
		contentparam.Reset();
		FillMailContentParam(contentparam, curr.rewards);
		int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_qing_yuan_duration_reward_subject);
		int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_qing_yuan_duration_reward_content, curr.need_marry_day);
		this->SendMailAllMember(MAIL_REASON_DEFAULT, contentparam);
	}
}

void QingYuan::ResetGiftDayFetchFlag()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	CommonSaveQingYuan * zu_info_ptr = this->GetCommonSaveQingYuan(false);
	if (!zu_info_ptr)
	{
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
		{
			CommonSaveQingYuanRoleInfoItem & curr = zu_info_ptr->role_info[i];
			WipeBit(curr.fetch_gift_flag, QING_YUAN_GIFT_FETCH_TYPE_DAY);
		}
	}

	this->SyncDataHiddenToGame();
	this->SendInfo();
}

CommonSaveQingYuanRoleInfoItem * QingYuan::GetMemberData(int role_id, bool is_set_dirty)
{
	CommonSaveQingYuan * cs = this->GetCommonSaveQingYuan(is_set_dirty);
	if (!cs)
	{
		return NULL;
	}

	for (int i = 0; i < ARRAY_LENGTH(cs->role_info); ++i)
	{
		if (cs->role_info[i].role_id == role_id)
		{
			return &cs->role_info[i];
		}

	}

	return NULL;
}

CommonSaveQingYuanRoleInfoItem * QingYuan::GetOtherSideMemberData(int role_id, bool is_set_dirty)
{
	CommonSaveQingYuan * cs = this->GetCommonSaveQingYuan(is_set_dirty);
	if (!cs)
	{
		return NULL;
	}

	for (int i = 0; i < ARRAY_LENGTH(cs->role_info); ++i)
	{
		if (cs->role_info[i].role_id != 0 && cs->role_info[i].role_id != role_id)
		{
			return &cs->role_info[i];
		}
	}

	return NULL;
}


CommonSaveQingYuan * QingYuan::GetCommonSaveQingYuan(bool is_set_dirty)
{
	is_set_dirty = true;  //先所有都保存
	if (is_set_dirty)
	{
		this->SetDataChange();
	}

	return m_data;
}
