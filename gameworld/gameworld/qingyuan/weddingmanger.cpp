#include "weddingmanger.hpp"
#include "engineadapter.h"
#include "internalcomm.h"
#include "gamelog.h"

#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"

#include "obj/character/role.h"
#include "scene/speciallogicimpl/speciallogicweddingfb.hpp"
#include "scene/scene.h"

#include "protocol/msgqingyuan.h"
#include "global/rmibackobjdef.h"
#include "config/logicconfigmanager.hpp"
#include "qingyuan/qingyuanconfig.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "global/usercache/usercache.hpp"
#include "other/route/mailroute.hpp"
#include "obj/otherobj/xunyoucar.hpp"
#include "global/qingyuan/qingyuanmanager.h"
#include "global/worldstatus/worldstatus.hpp"

void RMIInitWeddingBackObjectImpl::InitWeddingRet(int _ret, WeddingInfoDBList & _param)
{
	WeddingManger::Instance().LoadWeddingList(_ret, _param);
}


WeddingManger & WeddingManger::Instance()
{
	static WeddingManger wedding_manger;
	return wedding_manger;
}

void WeddingManger::OnServerStart()
{
	if (this->IsLoadFinish())
	{
		return;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	RMIInitWeddingBackObjectImpl *impl = new RMIInitWeddingBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	ac.InitWeddingDataAsyn(0, impl);
}

void WeddingManger::OnServerStop()
{
	if (this->IsLoadFinish())
	{
		this->SaveWeddingList();
	}

}

void WeddingManger::LoadWeddingList(int _ret, WeddingInfoDBList & _param)
{
	if (this->IsLoadFinish())
	{
		return;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (0 != _ret
		|| 0 > _param.m_count
		|| WeddingInfoDBList::MAX_SIZE < _param.m_count)
	{
		printf(" WeddingManger::LoadWeddingList Failed . ret:%d m_count:%d \n", _ret, _param.m_count);
		ServerLogic::GetInstServerLogic()->StopServer("WeddingManger::Init");
		EngineAdapter::Instance().StopGame();
		return;
	}

	int load_save_index_max = 0;

	for (int i = 0; i < WeddingInfoDBList::MAX_SIZE && i < _param.m_count; i++)
	{
		WeddingFB::WeddingInfoDBParam &db_param = _param.m_wedding_info_list[i];
		if (load_save_index_max < db_param.save_index)
		{
			load_save_index_max = db_param.save_index;
		}

		this->AddWeddingInfo(_param.m_wedding_info_list[i], true);
	}


	if (0 >= _param.m_count)
	{//加载完成
		m_is_load_finish = 1;
		return;
	}
	
	//加载下一波
	RMIInitWeddingBackObjectImpl *impl = new RMIInitWeddingBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	ac.InitWeddingDataAsyn(load_save_index_max + 1, impl);
}

void WeddingManger::SaveWeddingList()
{
	if (!this->IsLoadFinish())
	{
		return;
	}

	static WeddingInfoDBList db_param;
	db_param.m_save_index_begin = 0;
	db_param.m_count = 0;


	for (int i = 0; i < WeddingFB::WEDDING_INFO_SIZE; i++)
	{
		if (m_wedding_info_[i].IsValid())
		{//有效
			if (structcommon::CS_NONE == m_wedding_info_[i].GetDataStatus())
			{//未改变
				continue;
			}

			if (m_wedding_info_old_flag_[i])
			{//更新
				m_wedding_info_[i].GetDBParam(db_param.m_wedding_info_list[db_param.m_count]);
				db_param.m_wedding_info_list[db_param.m_count].change_state = structcommon::CS_UPDATE;
				db_param.m_count++;
			}
			else
			{//插入
				m_wedding_info_[i].GetDBParam(db_param.m_wedding_info_list[db_param.m_count]);
				db_param.m_wedding_info_list[db_param.m_count].change_state = structcommon::CS_INSERT;
				db_param.m_count++;
			}
		}
		else
		{//无效
			if (m_wedding_info_old_flag_[i])
			{//删除
				db_param.m_wedding_info_list[db_param.m_count].change_state = structcommon::CS_DELETE;
				db_param.m_wedding_info_list[db_param.m_count].wedding_id = 0;
				db_param.m_wedding_info_list[db_param.m_count].save_index = i;
				db_param.m_count++;
			}
			else
			{
				continue;
			}
		}

		if (WeddingInfoDBList::MAX_SIZE <= db_param.m_count)
		{
			RMISaveWeddingBackObjectImpl *backobj = new RMISaveWeddingBackObjectImpl();
			RMIGlobalClient ac;
			ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

			bool ret = ac.SaveWeddingDataAsyn(db_param, backobj);
			if (ret)
			{
				for (int index = 0; index < db_param.m_count && index < WeddingInfoDBList::MAX_SIZE; index++)
				{
					int temp_save_index = db_param.m_wedding_info_list[db_param.m_count].save_index;
					m_wedding_info_[temp_save_index].SetDataStatus(structcommon::CS_NONE);
					m_wedding_info_old_flag_[temp_save_index] = m_wedding_info_[temp_save_index].IsValid();
				}
			}

			db_param.m_save_index_begin = 0;
			db_param.m_count = 0;
		}
	}

	if (0 < db_param.m_count)
	{
		RMISaveWeddingBackObjectImpl *backobj = new RMISaveWeddingBackObjectImpl();
		RMIGlobalClient ac;
		ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

		bool ret = ac.SaveWeddingDataAsyn(db_param, backobj);
		if (ret)
		{
			for (int index = 0; index < db_param.m_count && index < WeddingInfoDBList::MAX_SIZE; index++)
			{
				int temp_save_index = db_param.m_wedding_info_list[db_param.m_count].save_index;
				m_wedding_info_[temp_save_index].SetDataStatus(structcommon::CS_NONE);
				m_wedding_info_old_flag_[temp_save_index] = m_wedding_info_[temp_save_index].IsValid();
			}
		}

		db_param.m_save_index_begin = 0;
		db_param.m_count = 0;
	}
}

void WeddingManger::Update(unsigned long interval, time_t now_second)
{
	if (!this->IsLoadFinish())
	{
		return;
	}
	if (m_next_save_time < now_second)
	{
		m_next_save_time = now_second + 60;

		this->SaveWeddingList();
	}

	this->WeddingStatusCheck(now_second);

	this->WeddingVaildCheck(now_second);
}

void WeddingManger::OnRoleLogIn(Role * role)
{
	if (NULL == role)
	{
		return;
	}
	if (!this->IsLoadFinish())
	{
		return;
	}

	//有预约婚宴 下发邀请列表
	this->SendInviteList(role);

	this->SendApplyList(role);
}

void WeddingManger::OnRoleLogOut(Role * role)
{
	//移除预约列表的订阅
	this->PubSubRemove(role->GetUID());
}

void WeddingManger::OnGameServerConnect(std::set<int> server_id)
{
	for (WeddingBeginKeySet::iterator it = m_wedding_begin_set.begin()
		; it != m_wedding_begin_set.end()
		; it++)
	{
		this->SendWeddingStatusSync(0, it->wedding_id);
	}
}

void WeddingManger::OnReq(Role * role, int type, int param_1, int param_2)
{
	if (NULL == role)
	{
		return;
	}

	//操作转跨服
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		switch (type)
		{
		case Protocol::WEDDING_FB_REQ_TYPE_WEDDING_SHOW_INFO:
			{
				this->SendGameHiddenRoleReq(role->GetUID(), type, param_1, param_2);
			}
			break;
		default:
			{
				role->NoticeNum(errornum::EN_HIDDEN_REQ);
			}
			break;
		}

		return;
	}
	
	
	switch (type)
	{
	case Protocol::WEDDING_FB_REQ_TYPE_FB_COMMON:
		{//场景操作 转到场景
			Scene * scene = role->GetScene();
			if (NULL == scene)
			{
				return;
			}

			if (Scene::SCENE_TYPE_WEDDING_FB != scene->GetSceneType())
			{
				return;
			}

			SpecialLogicWeddingFB * wedding_special = (SpecialLogicWeddingFB *)scene->GetSpecialLogic();
			if (NULL == wedding_special)
			{
				return;
			}
			wedding_special->OnFBCommonReq(role, param_1, param_2);
		}
		break;
	case Protocol::WEDDING_FB_REQ_TYPE_REGIST:
		{
			this->WeddingRegist(role, param_1);
		}
		break;
	case Protocol::WEDDING_FB_REQ_TYPE_REGIST_OFF:
		{
			this->WeddingRegistOff(role);
		}
		break;
	case Protocol::WEDDING_FB_REQ_TYPE_INVITE:
		{
			this->WeddingInvite(role, param_1);
		}
		break;
	case Protocol::WEDDING_FB_REQ_TYPE_APPLY:
		{
			this->WeddingApply(role, param_1);
		}
		break;
	case Protocol::WEDDING_FB_REQ_TYPE_MARK:
		{
			this->WeddingApplyMark(role, param_1, 1 == param_2);
		}
		break;
	case Protocol::WEDDING_FB_REQ_TYPE_REQ_REGIST_LIST:
		{
			this->SendRegistList(role);
			this->PubSubRegist(role);
		}
		break;
	case Protocol::WEDDING_FB_REQ_TYPE_REQ_SUB_REGIST_LIST:
		{
			if (0 == param_1)
			{//取消订阅
				this->PubSubRemove(role->GetUID());
			}
			else
			{//订阅
				this->PubSubRegist(role);
			}
		}
		break;
	case Protocol::WEDDING_FB_REQ_TYPE_REQ_INVITE_LIST:
		{
			this->SendInviteList(role);
		}
		break;
	case Protocol::WEDDING_FB_REQ_TYPE_OTHER_INFO:
		{

		}
		break;
	case Protocol::WEDDING_FB_REQ_TYPE_INVITE_SIZE_BUY:
		{
			this->WeddingBuyInviteSize(role);
		}
		break;
	case Protocol::WEDDING_FB_REQ_TYPE_XUN_YOU:
		{
			this->RegistXunYou(role, param_1);
		}
		break;
	case Protocol::WEDDING_FB_REQ_TYPE_WEDDING_SHOW_INFO:
		{
			this->SendWeddingShow(role->GetUID(), param_1);
		}
		break;
	case Protocol::WEDDING_FB_REQ_TYPE_WEDDING_OPEN_INFO:
		{
			this->SendWeddingStatusSync(role->GetUID(), param_1);
		}
		break;
	case Protocol::WEDDING_FB_REQ_TYPE_REQ_SUB_ACT:
		{//场景操作 转到场景
			Scene * scene = role->GetScene();
			if (NULL == scene)
			{
				return;
			}

			if (Scene::SCENE_TYPE_WEDDING_FB != scene->GetSceneType())
			{
				return;
			}

			SpecialLogicWeddingFB * wedding_special = (SpecialLogicWeddingFB *)scene->GetSpecialLogic();
			if (NULL == wedding_special)
			{
				return;
			}
			wedding_special->OnFBSubActReq(role, param_1, param_2);
		}
		break;
	default:
		return;
	}
}

void WeddingManger::SendFBBarrage(Role *role, const char * chat, unsigned int chat_len)
{
	Scene * scene = role->GetScene();
	if (NULL == scene)
	{
		return;
	}

	if (Scene::SCENE_TYPE_WEDDING_FB != scene->GetSceneType())
	{
		return;
	}

	SpecialLogicWeddingFB * wedding_special = (SpecialLogicWeddingFB *)scene->GetSpecialLogic();
	if (NULL == wedding_special)
	{
		return;
	}
	wedding_special->SendBarrage(role, chat, chat_len);
}

void WeddingManger::SendGameHiddenRoleReq(int uid, int type, int param_1, int param_2)
{
	crossgameprotocal::GameHiddenWeddingRoleReq info;
	info.uid = uid;
	info.type = type;
	info.param_1 = param_1;
	info.param_2 = param_2;

	InternalComm::Instance().SendToHiddenThroughCross((char *)&info, sizeof(info));
}

void WeddingManger::GameHiddenRoleReq(crossgameprotocal::GameHiddenWeddingRoleReq * req)
{
	if (NULL == req)
	{
		return;
	}

	switch (req->type)
	{
	case Protocol::WEDDING_FB_REQ_TYPE_WEDDING_SHOW_INFO:
		{
			this->SendWeddingShow(req->uid, req->param_1);
		}
		break;
	case Protocol::WEDDING_FB_REQ_TYPE_WEDDING_OPEN_INFO:
		{
			this->SendWeddingStatusSync(req->uid, req->param_1);
		}
		break;
	default:
		return;
	}
}

void WeddingManger::SyncLocal(int type, int p_1, int p_2, int p_3)
{
	crossgameprotocal::HiddenGameWeddingInfoSync info;

	info.type = type;
	info.param_1 = p_1;
	info.param_2 = p_2;
	info.param_3 = p_3;
	InternalComm::Instance().SendToAllGameThroughCross((char *)&info, sizeof(info));

	WeddingLocalManger::Instance().OnSycnWeddingInfo(&info);
}

WeddingInfo * WeddingManger::GetWeddingInfo(WeddingID wedding_id)
{
	WeddingIndexInfoMap::const_iterator wedding_id_it = m_wedding_id_map.find(wedding_id);
	if (m_wedding_id_map.end() == wedding_id_it)
	{
		return NULL;
	}
	
	return this->GetWeddingInfoByIndex(wedding_id_it->second);
}

WeddingInfo * WeddingManger::GetWeddingInfoByUid(int uid)
{
	WeddingUidMap::const_iterator uid_it = m_wedding_uid_map.find(uid);
	if (m_wedding_uid_map.end() == uid_it)
	{
		return NULL;
	}

	return this->GetWeddingInfo(uid_it->second);
}

WeddingInfo * WeddingManger::GetWeddingInfoByBegin(time_t begin_time)
{
	static WeddingBeginKey temp_key;
	temp_key.begin_time = begin_time;
	temp_key.wedding_id = 0;

	WeddingBeginKeyMap::const_iterator begin_key_it = m_wedding_time_map.upper_bound(temp_key);
	if (m_wedding_time_map.end() == begin_key_it)
	{
		return NULL;
	}
	if (begin_key_it->first.begin_time != temp_key.begin_time)
	{
		return NULL;
	}
	
	return this->GetWeddingInfoByIndex(begin_key_it->second);
}

WeddingInfo * WeddingManger::GetWeddingInfoByIndex(int save_index)
{
	if (0 > save_index
		|| WeddingFB::WEDDING_INFO_SIZE <= save_index)
	{
		return NULL;
	}

	if (m_wedding_info_[save_index].IsValid())
	{
		return &m_wedding_info_[save_index];
	}

	return NULL;
}

void WeddingManger::OnWeddingFinish(WeddingID wedding_id)
{
	WeddingInfo * wedding_info = this->GetWeddingInfo(wedding_id);
	if (NULL == wedding_info)
	{
		return;
	}

	if (!wedding_info->IsValid())
	{
		return;
	}

	long long qing_yuan_id = wedding_info->GetCommonInfo(false).qing_yuan_id;
	int xun_you_type = wedding_info->GetCommonInfo(false).xun_you_type;

	{
		int creat_xun_you_ret = 0;
		const WeddingFB::XunYouCfg * xun_you_cfg
			= LOGIC_CONFIG->GetQingYuanConfig()->GeXunYouCfg(xun_you_type);
		if (NULL != xun_you_cfg)
		{
			creat_xun_you_ret = this->CreatXunYouCar(wedding_id);
		}

		gamelog::g_log_qing_yuan.printf(LL_INFO, "WeddingManger::OnWeddingFinish "
			"| wedding_id[%d] qing_yuan_id[%lld] xun_you_type:[%d] xun_you_cfg:[%d] creat_xun_you_ret[%d]",
			wedding_id, qing_yuan_id, xun_you_type, NULL != xun_you_cfg ? 1 : 0, creat_xun_you_ret);
	}

	//情缘信息
	QingYuan * p_qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(qing_yuan_id);
	if (NULL != p_qing_yuan)
	{
		CommonSaveQingYuan * p_qing_yuan_common = p_qing_yuan->GetCommonSaveQingYuan(true);
		if (NULL != p_qing_yuan_common)
		{
			p_qing_yuan_common->has_hun_yan = WeddingFB::WEDDING_REGIST_STATUS_DONE;
		}
	}

	//删除 数据 与 迭代器失效
	this->RemoveWeddingInfo(wedding_id);
}

bool WeddingManger::WeddingRegist(Role * host_role, time_t regist_wedding_time)
{
	if (NULL == host_role)
	{
		return false;
	}

	//可预约
	//情缘信息
	QingYuan * p_qing_yuan = QingYuanManager::Instance().GetQingYuanByRoleId(host_role->GetUID());
	if (NULL == p_qing_yuan)
	{
		host_role->NoticeNum(errornum::EN_QING_YUAN_NULL);
		return false;
	}
	
	CommonSaveQingYuan * p_qing_yuan_common = p_qing_yuan->GetCommonSaveQingYuan(true);
	if (NULL == p_qing_yuan_common)
	{
		host_role->NoticeNum(errornum::EN_WEDDING_NULL);
		return false;
	}
	
	//未预约
	if (WeddingFB::WEDDING_REGIST_STATUS_DONE == p_qing_yuan_common->has_hun_yan)
	{
		host_role->NoticeNum(errornum::EN_WEDDING_XUN_YOU_HAS_BUY);
		return false;
	}

	if (NULL != this->GetWeddingInfoByUid(host_role->GetUID()))
	{//已预约
		host_role->NoticeNum(errornum::EN_WEDDING_REGIST_FULL);
		return false;
	}

	if (!LOGIC_CONFIG->GetQingYuanConfig()->CheckWeddingBeginTime(regist_wedding_time))
	{//非标准时间
		host_role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}
	//时间过近
	if (EngineAdapter::Instance().Time() + LOGIC_CONFIG->GetQingYuanConfig()->GetFbPrePareTime() + 120 > regist_wedding_time)
	{
		host_role->NoticeNum(errornum::EN_WEDDING_REGIST_TOO_NEAR);
		return false;
	}
	//时间重复
	if (NULL != this->GetWeddingInfoByBegin(regist_wedding_time))
	{
		host_role->NoticeNum(errornum::EN_WEDDING_REGIST_OVER);
		return false;
	}

	//预约
	WeddingFB::WeddingInfoDBParam db_param;
	db_param.wedding_id = WorldStatus::Instance().CreatWeddingID();
	db_param.begin_time = (unsigned int)regist_wedding_time;
	db_param.is_finsih = WeddingFB::WEDDING_REGIST_STATUS_NOW;

	db_param.common_info.qing_yuan_id = p_qing_yuan->GetQingYuanId();

	db_param.cfg_ver = LOGIC_CONFIG->GetQingYuanConfig()->GetCfgVer();
	std::vector<int> mem_list = p_qing_yuan->GetMemberList();
	{//主办者信息  先来一个
		int mem_list_size = (int)mem_list.size();
		for (int i = 0; i < mem_list_size && i < WeddingFB::HOST_NUM; i++)
		{
			db_param.host_info_[i].uid = mem_list[i];
		}
	}
	int ret = this->AddWeddingInfo(db_param);

	if (0 != ret)
	{
		host_role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return false;
	}
	//日志
	gamelog::g_log_qing_yuan.printf(LL_INFO, "%s |  role[%d %s]"
		" begin_time:%d, wedding_id:%d, cfg_ver:%d",
		__FUNCTION__, 
		host_role->GetUID(), host_role->GetName(),
		db_param.begin_time, db_param.wedding_id, db_param.cfg_ver);

	p_qing_yuan_common->has_hun_yan = WeddingFB::WEDDING_REGIST_STATUS_NOW;
	p_qing_yuan->SendInfo();

	{
		this->SendWeddingChange(host_role, WEDDING_CHANGE_TYPE_REGIST_ADD, (int)regist_wedding_time);
		
		this->SendWeddingChange(host_role, WEDDING_CHANGE_TYPE_ROLE_REGIST_CHANGE, (int)regist_wedding_time);
	}
	
	this->SendInviteAdd(db_param.wedding_id, 0, 1);

	{
		tm day_tm;
		if (!GetTimeTM(db_param.begin_time, day_tm))
		{
			return true;
		}

		static MailContentParam contentparam;
		contentparam.Reset();

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject),
			gamestring::g_wedding_regist_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt),
			gamestring::g_wedding_regist_content,
			day_tm.tm_year + 1900, day_tm.tm_mon + 1, day_tm.tm_mday,
			day_tm.tm_hour, day_tm.tm_min);

		if (lensub > 0 && length > 0)
		{
			int mem_list_size = (int)mem_list.size();
			for (int i = 0; i < mem_list_size && i < WeddingFB::HOST_NUM; i++)
			{
				MailRoute::MailToUser(IntToUid(mem_list[i]), MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}

	return true;
}

bool WeddingManger::WeddingRegistOff(Role * host_role)
{
	WeddingBeginKey temp_key;

	{
		const WeddingInfo * wedding_info = this->GetWeddingInfoByUid(host_role->GetUID());
		if (NULL == wedding_info)
		{//未预约
			host_role->NoticeNum(errornum::EN_WEDDING_NULL);
			return false;
		}

		if (!wedding_info->IsValid())
		{
			return false;
		}

		temp_key.wedding_id = wedding_info->GetWeddingID();
		temp_key.begin_time = wedding_info->GetBeginTime();
	}

	if (m_wedding_begin_set.end() != m_wedding_begin_set.find(temp_key))
	{
		host_role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	//取消预约  数据失效
	this->RemoveWeddingInfo(temp_key.wedding_id);

	//情缘信息
	QingYuan * p_qing_yuan = QingYuanManager::Instance().GetQingYuanByRoleId(host_role->GetUID());
	if (NULL != p_qing_yuan)
	{
		CommonSaveQingYuan * p_qing_yuan_common = p_qing_yuan->GetCommonSaveQingYuan(true);
		if (NULL != p_qing_yuan_common)
		{
			if (WeddingFB::WEDDING_REGIST_STATUS_NOW == p_qing_yuan_common->has_hun_yan)
			{
				p_qing_yuan_common->has_hun_yan = WeddingFB::WEDDING_REGIST_STATUS_NULL;
				
			}
			p_qing_yuan->SendInfo();
		}
	}

	//日志
	gamelog::g_log_qing_yuan.printf(LL_INFO, "%s |  role[%d %s]"
		" wedding_id:%d.",
		__FUNCTION__,
		host_role->GetUID(), host_role->GetName(),
		temp_key.wedding_id);

	{
		this->SendWeddingChange(host_role, WEDDING_CHANGE_TYPE_REGIST_REMOVE, (int)temp_key.begin_time);
		this->SendWeddingChange(host_role, WEDDING_CHANGE_TYPE_ROLE_REGIST_CHANGE, 0);
	}

	return true;
}

void WeddingManger::WeddingInvite(Role * host_role, int invite_uid)
{
	//找到婚宴
	WeddingInfo * wedding_info = this->GetWeddingInfoByUid(host_role->GetUID());
	if (NULL == wedding_info)
	{
		host_role->NoticeNum(errornum::EN_WEDDING_NULL);
		return;
	}
	const WeddingFB::BaseCfg & base_cfg
		= LOGIC_CONFIG->GetQingYuanConfig()->GetFBBaseCfg();
	int invite_max = base_cfg.base_invite_max + base_cfg.buy_invite_max_add * wedding_info->GetCommonInfo(false).buy_invite_times;

	QingYuan * p_qing_yuan = QingYuanManager::Instance().GetQingYuanByRoleId(host_role->GetUID());
	if (NULL == p_qing_yuan)
	{
		host_role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	//邀请
	UserCacheNode *node = UserCacheManager::Instance().GetUserNode(invite_uid);
	if (NULL == node)
	{
		host_role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (wedding_info->BeInvite(invite_uid))
	{
		host_role->NoticeNum(errornum::EN_WEDDING_BE_INVITED);
		return;
	}

	if (invite_max <= wedding_info->GetInviteNum())
	{
		host_role->NoticeNum(errornum::EN_WEDDING_INVITED_LIST_FULL);
		return;
	}

	int ret = wedding_info->AddInvite(invite_uid);

	if (WeddingInfo::ERROR_TYPE_SUC == ret)
	{
		this->SendInviteAdd(wedding_info->GetWeddingID(), invite_uid);

		tm day_tm;
		if (!GetTimeTM(wedding_info->GetBeginTime(), day_tm))
		{
			return;
		}

		static GameName host_name_list[WeddingFB::HOST_NUM];
		memset(host_name_list, 0, sizeof(host_name_list));

		std::vector<int> host_vec = p_qing_yuan->GetMemberList();
		for (int i = 0; i < (int)host_vec.size() && i < WeddingFB::HOST_NUM; i++)
		{
			UserCacheNode *node = UserCacheManager::Instance().GetUserNode(host_vec[i]);
			if (NULL == node)
			{
				continue;
			}
			node->GetName(host_name_list[i]);
		}

		//邀请邮件
		static MailContentParam contentparam;
		contentparam.Reset();
		UNSTD_STATIC_CHECK(2 == WeddingFB::HOST_NUM)
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject),
			gamestring::g_wedding_invite_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt),
			gamestring::g_wedding_invite_content,
			host_name_list[0], host_name_list[1],
			day_tm.tm_year + 1900, day_tm.tm_mon + 1, day_tm.tm_mday,
			day_tm.tm_hour, day_tm.tm_min,
			wedding_info->GetWeddingID());

		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(IntToUid(invite_uid), MAIL_REASON_DEFAULT, contentparam);
		}
	}
}

void WeddingManger::WeddingApply(Role * _role, int wedding_id)
{
	//找到婚宴
	WeddingInfo * wedding_info = this->GetWeddingInfo(wedding_id);
	if (NULL == wedding_info)
	{
		_role->NoticeNum(errornum::EN_WEDDING_NULL);
		return;
	}

	//未已开启
	WeddingBeginKey temp_beging_key;
	temp_beging_key.begin_time = wedding_info->GetBeginTime();
	temp_beging_key.wedding_id = wedding_info->GetWeddingID();
	if (m_wedding_begin_set.end() == m_wedding_begin_set.find(temp_beging_key))
	{
		_role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	//已申请
	if (wedding_info->BeInvite(_role->GetUID()))
	{
		_role->NoticeNum(errornum::EN_WEDDING_APPLY_WAIT);
		return;
	}

	//申请
	if (WeddingInfo::ERROR_TYPE_SUC != wedding_info->AddApply(_role->GetUID()))
	{
		_role->NoticeNum(errornum::EN_WEDDING_APPLY_WAIT);
		return;
	}
	
	_role->NoticeNum(errornum::EN_WEDDING_APPLY_SEND);


	this->SendApplyAdd(wedding_id, _role->GetUID());
}

void WeddingManger::WeddingApplyMark(Role * host_role, int mark_uid, bool mark_invite)
{
	//找到婚宴
	WeddingInfo * wedding_info = this->GetWeddingInfoByUid(host_role->GetUID());
	if (NULL == wedding_info)
	{
		host_role->NoticeNum(errornum::EN_WEDDING_NULL);
		return;
	}

	//已邀请
	if (wedding_info->BeInvite(mark_uid))
	{
		host_role->NoticeNum(errornum::EN_WEDDING_BE_INVITED);
		return;
	}

	//处理
	if (mark_invite)
	{
		wedding_info->RemoveApply(mark_uid);

		this->WeddingInvite(host_role, mark_uid);
		//this->SendWeddingShow(mark_uid, wedding_info->GetWeddingID());
	}
	else
	{
		wedding_info->RemoveApply(mark_uid);
		this->SendWeddingChange(host_role, WEDDING_CHANGE_TYPE_APPLY_REMOVE, mark_uid);
	}
}

void WeddingManger::WeddingBuyInviteSize(Role * host_role)
{
	if (NULL == host_role)
	{
		return;
	}

	WeddingInfo *wedding_info = GetWeddingInfoByUid(host_role->GetUID());
	if (NULL == wedding_info)
	{
		return;
	}

	if (!wedding_info->IsValid())
	{
		return;
	}

	const WeddingFB::BaseCfg & base_cfg
		= LOGIC_CONFIG->GetQingYuanConfig()->GetFBBaseCfg();

	int & buy_invite_times = wedding_info->GetCommonInfo(true).buy_invite_times;

	//限制
	if (base_cfg.buy_invite_max_times <= buy_invite_times)
	{
		return;
	}

	//消耗
	if (!host_role->GetRoleModuleManager()->GetMoney()->UseMoney(base_cfg.buy_invite_max_cost_type, base_cfg.buy_invite_max_cost_num, __FUNCTION__, true))
	{
		host_role->NoticeNum(errornum::EN_CONDITIONS_NOT_REACHED);
		return;
	}

	//记录
	buy_invite_times++;

	gamelog::g_log_qing_yuan.printf(LL_INFO, "WeddingManger::WeddingBuyInviteSize "
		"| role_id[%d] buy_invite_times:[%d] wedding_id:[%d]",
		host_role->GetUID(), buy_invite_times, wedding_info->GetWeddingID());
	
	this->SendInviteAdd(wedding_info->GetWeddingID(), 0);
}

void WeddingManger::SendRegistList(Role *role)
{
	if (NULL == role)
	{
		return;
	}
	static Protocol::SCWeddingRegistList info;
	info.regist_time = 0;
	{
		const WeddingInfo * wedding_info = this->GetWeddingInfoByUid(role->GetUID());
		if (NULL != wedding_info)
		{
			info.regist_time = (unsigned int)wedding_info->GetBeginTime();
		}
	}

	info.count = 0;
	WeddingBeginKeyMap::const_iterator begin_it = m_wedding_time_map.begin();
	for (; begin_it != m_wedding_time_map.end(); begin_it++)
	{
		info.be_regist_time_list[info.count] = (unsigned int)begin_it->first.begin_time;
		info.count++;
	}

	int send_ln = sizeof(Protocol::SCWeddingRegistList)
		- (WeddingFB::WEDDING_INFO_SIZE - info.count)
			* sizeof(info.be_regist_time_list[0]);

	role->GetRoleModuleManager()->NetSend(&info, send_ln);
}

void WeddingManger::SendInviteList(Role * role)
{
	if (NULL == role)
	{
		return;
	}

	WeddingInfo * wedding_info = this->GetWeddingInfoByUid(role->GetUID());
	if (NULL == wedding_info)
	{
		return;
	}

	static Protocol::SCWeddingInviteList info;
	info.wedding_id = wedding_info->GetWeddingID();
	info.wedding_begin_time = wedding_info->GetBeginTime();
	info.buy_invite_times = wedding_info->GetCommonInfo(false).buy_invite_times;

	for (int i = 0; i < WeddingFB::HOST_NUM; i++)
	{
		info.host_uid_[i] = wedding_info->GetHostUid(i);
	}
	

	info.is_total = 1;
	info.count = 0;

	const WeddingFB::InviteInfo & invite_info = wedding_info->GetInviteList();
	for (int i = 0;  i < invite_info.count && i < WeddingFB::MAX_INVITE_SIZE; i++)
	{
		const int &uid = invite_info.invite_uid_[i];

		UserCacheNode *node = UserCacheManager::Instance().GetUserNode(uid);
		if (NULL == node)
		{
			continue;
		}

		info.invite_list[info.count].role_uid = uid;
		node->GetName(info.invite_list[info.count].role_name);

		info.count++;
	}

	int send_ln = sizeof(Protocol::SCWeddingInviteList)
		- (WeddingFB::MAX_INVITE_SIZE - info.count)
		* sizeof(info.invite_list[0]);

	role->GetRoleModuleManager()->NetSend(&info, send_ln);
}

void WeddingManger::SendInviteAdd(WeddingID wedding_id, int add_uid, int is_total)
{
	WeddingInfo * wedding_info = this->GetWeddingInfo(wedding_id);
	if (NULL == wedding_info)
	{
		return;
	}

	static Protocol::SCWeddingInviteList info;
	info.wedding_id = wedding_info->GetWeddingID();
	info.wedding_begin_time = wedding_info->GetBeginTime();
	info.buy_invite_times = wedding_info->GetCommonInfo(false).buy_invite_times;
	for (int i = 0; i < WeddingFB::HOST_NUM; i++)
	{
		info.host_uid_[i] = wedding_info->GetHostUid(i);
	}

	info.is_total = is_total;
	info.count = 0;
	UserCacheNode *node = UserCacheManager::Instance().GetUserNode(add_uid);
	if (NULL != node)
	{
		info.count = 1;

		info.invite_list[0].role_uid = add_uid;
		node->GetName(info.invite_list[0].role_name);
	}
	int send_ln = sizeof(Protocol::SCWeddingInviteList)
		- (WeddingFB::MAX_INVITE_SIZE - info.count)
		* sizeof(info.invite_list[0]);

	//sendto双方
	int host_count = 0;
	const WeddingFB::HostInfo * host_list = wedding_info->GetHostInfoList(host_count);

	for (int i = 0; i < WeddingFB::HOST_NUM; i++)
	{
		if (0 == host_list[i].uid)
		{
			continue;
		}

		World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(host_list[i].uid), &info, send_ln);
		//InternalComm::Instance().SendToAllGameUserThroughCross(host_list[i].uid, (const char *)&info, sizeof(info));
	}
}

void WeddingManger::SendApplyList(Role * role)
{
	if (NULL == role)
	{
		return;
	}

	const WeddingInfo * wedding_info = this->GetWeddingInfoByUid(role->GetUID());
	if (NULL == wedding_info)
	{
		return;
	}
	WeddingBeginKey temp_key;
	temp_key.begin_time = wedding_info->GetBeginTime();
	temp_key.wedding_id = wedding_info->GetWeddingID();

	if (m_wedding_begin_set.end() == m_wedding_begin_set.find(temp_key))
	{
		return;
	}

	static Protocol::SCWeddingApplyList info;
	info.is_total = 1;
	info.count = 0;

	const WeddingInfo::ApplyTimeSet & apply_time_set = wedding_info->GetApplyTimeSet();

	WeddingInfo::ApplyTimeSet::const_iterator apply_cit = apply_time_set.begin();
	for (; apply_cit != apply_time_set.end()  && info.count < WeddingFB::MAX_APPLY_SIZE; apply_cit++)
	{
		const int &uid = apply_cit->uid;

		UserCacheNode *node = UserCacheManager::Instance().GetUserNode(uid);
		if (NULL == node)
		{
			continue;
		}

		info.apply_list[info.count].role_uid = uid;
		node->GetName(info.apply_list[info.count].role_name);

		info.count++;
	}

	int send_ln = sizeof(Protocol::SCWeddingApplyList)
		- (WeddingFB::MAX_APPLY_SIZE - info.count)
		* sizeof(info.apply_list[0]);

	role->GetRoleModuleManager()->NetSend(&info, send_ln);
}

void WeddingManger::SendApplyAdd(WeddingID wedding_id, int add_uid)
{

	WeddingInfo * wedding_info = this->GetWeddingInfo(wedding_id);
	if (NULL == wedding_info)
	{
		return;
	}

	UserCacheNode *node = UserCacheManager::Instance().GetUserNode(add_uid);
	if (NULL == node)
	{
		return;
	}

	static Protocol::SCWeddingApplyList info;
	info.is_total = 0;
	info.count = 1;

	info.apply_list[0].role_uid = add_uid;
	node->GetName(info.apply_list[0].role_name);

	int send_ln = sizeof(Protocol::SCWeddingApplyList)
		- (WeddingFB::MAX_APPLY_SIZE - 1)
		* sizeof(info.apply_list[0]);

	//sendto双方
	int host_count = 0;
	const WeddingFB::HostInfo * host_list = wedding_info->GetHostInfoList(host_count);

	for (int i = 0; i < WeddingFB::HOST_NUM && i < host_count; i++)
	{
		if (0 == host_list[i].uid)
		{
			continue;
		}

		World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(host_list[i].uid), &info, send_ln);

		//InternalComm::Instance().SendToAllGameUserThroughCross(host_list[i].uid, (const char *)&info, sizeof(info));
	}
}

void WeddingManger::SendWeddingChange(Role * role, int change_type, int param_1)
{
	if (NULL == role)
	{
		return;
	}

	static Protocol::SCWeddingChange info;
	info.change_type = change_type;
	info.param_1 = param_1;

	switch (change_type)
	{
		//预约列表改变 通知订阅者
	case WEDDING_CHANGE_TYPE_REGIST_ADD:
	case WEDDING_CHANGE_TYPE_REGIST_REMOVE:
		{
			this->SendToSubRole(&info, sizeof(info));
		}
		break;
	case WEDDING_CHANGE_TYPE_ROLE_REGIST_CHANGE:
		{
			//情缘信息
			QingYuan * p_qing_yuan = QingYuanManager::Instance().GetQingYuanByRoleId(role->GetUID());
			if (NULL == p_qing_yuan)
			{
				return;
			}
			std::vector<int> mem_list = p_qing_yuan->GetMemberList();

			int mem_list_size = (int)mem_list.size();
			for (int i = 0; i < mem_list_size && i < WeddingFB::HOST_NUM; i++)
			{
				World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(mem_list[i]), &info, sizeof(info));
			}
		}
		break;
	default:
		{
			role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
		}
		break;
	}
	return;
}

void WeddingManger::SendToSubRole(const void * data, unsigned int length)
{
	for (TimeUidMap::iterator net_it = m_regist_pub_sub_map.begin()
		; net_it != m_regist_pub_sub_map.end()
		; net_it++)
	{
		//发送
		EngineAdapter::Instance().NetSend(net_it->second, data, length);
	}
}

void WeddingManger::SendWeddingStatusSync(int to_role_id, WeddingID wedding_id)
{
	WeddingInfo * wedding_info = GetWeddingInfo(wedding_id);
	if (NULL == wedding_info)
	{//失效
		return;
	}

	Protocol::SCWeddingStatusSync info;
	info.wedding_id = wedding_info->GetWeddingID();
	info.begin_time = (unsigned int)wedding_info->GetBeginTime();

	if (0 == to_role_id)
	{
		InternalComm::Instance().SendToAllGameAllUserThroughCross((const char *)&info, sizeof(info));
	}
	else
	{
		World::GetInstWorld()->SendToRole(to_role_id, (const char *)&info, sizeof(info));
	}
}
void WeddingManger::SendWeddingShow(int to_role_id, WeddingID wedding_id)
{
	//不可群发
	if (0 == to_role_id)
	{
		return;
	}
	WeddingInfo * wedding_info = GetWeddingInfo(wedding_id);
	if (NULL == wedding_info)
	{//失效
		//_role->NoticeNum(errornum::EN_WEDDING_NULL);
		Protocol::SCNoticeNum en;
		en.notice_num = errornum::EN_WEDDING_NULL;

		World::GetInstWorld()->SendToRole(to_role_id, (const char *)&en, sizeof(en));

		return;
	}

	Protocol::SCWeddingShowInfo info;
	info.wedding_id = wedding_info->GetWeddingID();
	info.begin_time = (unsigned int)wedding_info->GetBeginTime();
	
	info.status = 0;
	if (wedding_info->BeInvite(to_role_id))
	{
		info.status = Protocol::SCWeddingShowInfo::STATUS_TYPE_INVITED;
	}
	else if (wedding_info->HasApply(to_role_id))
	{
		info.status = Protocol::SCWeddingShowInfo::STATUS_TYPE_APPLIED;
	}

	info.host_count = 0;
	const WeddingFB::HostInfo * host_list = wedding_info->GetHostInfoList(info.host_count);
	for (int i = 0 ; i < info.host_count && i < WeddingFB::HOST_NUM; i++)
	{
		info.host_list[i].uid = host_list[i].uid;

		UserCacheNode *node = UserCacheManager::Instance().GetUserNode(host_list[i].uid);
		if (NULL != node)
		{
			node->GetName(info.host_list[i].role_name);
			info.host_list[i].appearance = node->appearance;
		}
		else
		{//取不到缓存
			memcpy(info.host_list[i].role_name, host_list[i].name, sizeof(info.host_list[i].role_name));
			info.host_list[i].appearance = host_list[i].appearance;
		}
	}

	World::GetInstWorld()->SendToRole(to_role_id, (const char *)&info, sizeof(info));
}

void WeddingManger::GMCreatXunYouCar(int uid, int xun_you_type)
{
	const WeddingFB::XunYouCfg * xun_you_cfg
		= LOGIC_CONFIG->GetQingYuanConfig()->GeXunYouCfg(xun_you_type);
	if (NULL == xun_you_cfg)
	{
		return;
	}

	const WeddingFB::XunYouWayPoint * way_point_cfg
		= LOGIC_CONFIG->GetQingYuanConfig()->GeXunYouWayPoint(0);
	if (NULL == way_point_cfg)
	{
		return;
	}

	QingYuan * p_qing_yuan = QingYuanManager::Instance().GetQingYuanByRoleId(uid);
	if (NULL == p_qing_yuan)
	{
		return;
	}

	Scene* scene = World::GetInstWorld()->GetSceneManager()
		->GetSceneById(xun_you_cfg->scene_id, COMMON_SCENE_KEY);
	if (NULL == scene)
	{
		return;
	}
	XunYouCar * car = new XunYouCar();
	car->Init(p_qing_yuan->GetQingYuanId(), xun_you_type);
	car->SetPos(way_point_cfg->way_poi);
	scene->AddObj(car);

	return;
}

WeddingID WeddingManger::GMOpenWedding(Role * role)
{
	WeddingInfo * wedding_info = GetWeddingInfoByUid(role->GetUID());
	if (NULL == wedding_info)
	{//失效
		return 0;
	}
	this->OpenWedding(wedding_info->GetWeddingID());

	//场景
	const WeddingFB::BaseCfg& fb_base_cfg
		= LOGIC_CONFIG->GetQingYuanConfig()->GetFBBaseCfg();
	int target_scene_id = fb_base_cfg.scene_id;
	int target_scene_key = wedding_info->GetWeddingID();

	SpecialLogicWeddingFB * special_logic = World::GetInstWorld()->GetSceneManager()
		->GetSpecialLogicWeddingFB(target_scene_id, target_scene_key, false);

	if (NULL == special_logic)
	{
		return 0;
	}

	special_logic->GMNextLink();
	return wedding_info->GetWeddingID();
}

void WeddingManger::PubSubRegist(Role * role)
{
	TimeUidKey temp_key;
	temp_key.uid = role->GetUID();

	//删旧
	this->PubSubRemove(temp_key.uid);

	{//加新
		temp_key.valid_time = EngineAdapter::Instance().Time() + 60;
		m_regist_uid_map[temp_key.uid] = temp_key.valid_time;
		m_regist_pub_sub_map[temp_key] = role->GetNetId();
	}
}

void WeddingManger::PubSubRemove(int uid)
{
	TimeUidKey temp_key;
	temp_key.uid = uid;

	UidTimeMap::iterator uid_it = m_regist_uid_map.find(temp_key.uid);
	if (uid_it == m_regist_uid_map.end())
	{
		return;
	}
	temp_key.valid_time = uid_it->second;
	m_regist_uid_map.erase(uid_it);
	m_regist_pub_sub_map.erase(temp_key);
}

int WeddingManger::AddWeddingInfo(WeddingFB::WeddingInfoDBParam &wedding_db_param, bool is_load_data)
{
	if (!wedding_db_param.IsValid())
	{
		return -1;
	}

	//储存位置
	int save_index = -1;
	if (is_load_data)
	{
		save_index = wedding_db_param.save_index;
		m_wedding_info_old_flag_[save_index] = true;
		wedding_db_param.change_state = structcommon::CS_NONE;
	}
	else
	{
		for (int i = 0; i < WeddingFB::WEDDING_INFO_SIZE; i++)
		{
			if (m_wedding_info_[i].IsValid())
			{
				continue;
			}
			save_index = i;
			wedding_db_param.change_state = structcommon::CS_INSERT;
			break;
		}
	}

	if (0 > save_index
		|| WeddingFB::WEDDING_INFO_SIZE < save_index)
	{
		return -2;
	}

	WeddingInfo &wedding_info = m_wedding_info_[save_index];
	
	if (wedding_info.IsValid())
	{//存在旧的
		return -3;
	}

	wedding_db_param.save_index = save_index;

	//加新
	wedding_info.InitDBParam(wedding_db_param);

	WeddingBeginKey new_key;
	new_key.begin_time = wedding_info.GetBeginTime();
	new_key.wedding_id = wedding_info.GetWeddingID();

	m_wedding_id_map[new_key.wedding_id] = save_index;
	m_wedding_time_map[new_key] = save_index;

	UNSTD_STATIC_CHECK(2 == WeddingFB::HOST_NUM);
	m_wedding_uid_map[wedding_db_param.host_info_[0].uid] = new_key.wedding_id;
	m_wedding_uid_map[wedding_db_param.host_info_[1].uid] = new_key.wedding_id;

	return 0;
}

void WeddingManger::RemoveWeddingInfo(int wedding_id)
{
	WeddingIndexInfoMap::iterator wedding_id_it = m_wedding_id_map.find(wedding_id);
	if (m_wedding_id_map.end() == wedding_id_it)
	{
		return;
	}

	int save_index = wedding_id_it->second;
	if (0 > save_index
		|| WeddingFB::WEDDING_INFO_SIZE < save_index)
	{
		m_wedding_id_map.erase(wedding_id_it);
		return;
	}


	WeddingInfo &wedding_info = m_wedding_info_[save_index];

	WeddingBeginKey old_key;
	old_key.wedding_id = wedding_id;
	old_key.begin_time = wedding_info.GetBeginTime();

	//删除 迭代器失效
	m_wedding_id_map.erase(wedding_id_it);
	m_wedding_time_map.erase(old_key);

	int host_size = 0;
	const WeddingFB::HostInfo * host_list = wedding_info.GetHostInfoList(host_size);
	for (int i = 0; i < host_size && i < WeddingFB::HOST_NUM; i++)
	{
		m_wedding_uid_map.erase(host_list[i].uid);
	}

	wedding_info.Reset();
}

void WeddingManger::WeddingStatusCheck(time_t now_second)
{
	if (!this->IsLoadFinish())
	{
		m_next_check_time = now_second + 10;
		return;
	}

	if (m_next_check_time > now_second)
	{
		return;
	}
	m_next_check_time = now_second + 10;

	//提前预热时间
	time_t fb_pre_pare_time_s = LOGIC_CONFIG->GetQingYuanConfig()->GetFbPrePareTime();

	//预热时间可以不用很准确 取前后10s入开启列表
	WeddingBeginKey temp_key;
	temp_key.begin_time = fb_pre_pare_time_s + now_second - 10;

	WeddingBeginKeyMap::iterator wedding_begin_it = m_wedding_time_map.upper_bound(temp_key);
	
	temp_key.begin_time = fb_pre_pare_time_s + now_second + 10;
	WeddingBeginKeyMap::iterator wedding_end_it = m_wedding_time_map.upper_bound(temp_key);

	for (; wedding_begin_it != wedding_end_it; wedding_begin_it++)
	{//选出进入开启队列的婚宴
		//开启检测
		this->OpenWedding(wedding_begin_it->first.wedding_id);
	}
}

void WeddingManger::OpenWedding(int wedding_id)
{
	WeddingInfo * wedding_info = GetWeddingInfo(wedding_id);
	if (NULL == wedding_info)
	{//失效
		return;
	}

	if (!wedding_info->IsValid())
	{//失效
		return;
	}

	if (WeddingFB::WEDDING_REGIST_STATUS_NOW != wedding_info->GetStatus())
	{//已开过
		return;
	}

	//情缘失效
	QingYuan * p_qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(wedding_info->GetCommonInfo(false).qing_yuan_id);
	if (NULL == p_qing_yuan)
	{
		return;
	}

	WeddingBeginKey begin_key;
	begin_key.begin_time = wedding_info->GetBeginTime();
	begin_key.wedding_id = wedding_info->GetWeddingID();
	//已在开启列表
	if (m_wedding_begin_set.find(begin_key) != m_wedding_begin_set.end())
	{
		return;
	}

	//创建场景
	const WeddingFB::BaseCfg& fb_base_cfg
		= LOGIC_CONFIG->GetQingYuanConfig()->GetFBBaseCfg();
	int target_scene_id = fb_base_cfg.scene_id;
	int target_scene_key = begin_key.wedding_id;

	SpecialLogicWeddingFB * special_logic = World::GetInstWorld()->GetSceneManager()
		->GetSpecialLogicWeddingFB(target_scene_id, target_scene_key);
	if (NULL == special_logic)
	{
		return;
	}
	special_logic->SetSceneParam(begin_key.wedding_id, begin_key.begin_time);

	//加入开启列表
	m_wedding_begin_set.insert(begin_key);

	//全局通知
	this->SendWeddingStatusSync(0, begin_key.wedding_id);

	//同步婚宴信息
	this->SyncLocal(crossgameprotocal::HiddenGameWeddingInfoSync::WEDDING_BEGIN,
		begin_key.wedding_id);

	gamelog::g_log_qing_yuan.printf(LL_INFO, "WeddingManger::OpenWedding "
		"| wedding_id:[%d] ",
		wedding_info->GetWeddingID());
}

void WeddingManger::WeddingVaildCheck(time_t now_second)
{
	if (!this->IsLoadFinish())
	{
		m_wedding_vaild_check_time = now_second + 600;
		return;
	}
	if (m_wedding_vaild_check_time > now_second)
	{
		return;
	}
	m_wedding_vaild_check_time = now_second + 600;

	time_t invalid_time = now_second - LOGIC_CONFIG->GetQingYuanConfig()->GetFbTotalTime() - 600;

	WeddingBeginKey temp_key;
	temp_key.begin_time = invalid_time;
	WeddingBeginKeyMap::iterator wedding_begin_it = m_wedding_time_map.begin();

	for (; wedding_begin_it != m_wedding_time_map.end(); )
	{//失效删除
		if (wedding_begin_it->first.begin_time > temp_key.begin_time)
		{
			break;
		}
		
		WeddingID wedding_id = wedding_begin_it->first.wedding_id;

		//失效预约 迭代器失效
		this->TimeOutWedding(wedding_id);

		//必须最后
		wedding_begin_it = m_wedding_time_map.begin();
	}
}

void WeddingManger::TimeOutWedding(WeddingID wedding_id)
{
	//不返回 必须删
	long long qing_yuan_id = 0;
	int qing_yuan_common_has_hun_yan = WeddingFB::WEDDING_REGIST_STATUS_DONE;
	{
		WeddingInfo * wedding_info = this->GetWeddingInfo(wedding_id);
		if (NULL != wedding_info
			&& wedding_info->IsValid()
			&& WeddingFB::WEDDING_REGIST_STATUS_DONE != wedding_info->GetStatus())
		{
			qing_yuan_id = wedding_info->GetCommonInfo(false).qing_yuan_id;
			qing_yuan_common_has_hun_yan = WeddingFB::WEDDING_REGIST_STATUS_NULL;
		}

	}
	//取消预约  数据失效
	this->RemoveWeddingInfo(wedding_id);

	//情缘信息
	QingYuan * p_qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(qing_yuan_id);
	if (NULL != p_qing_yuan)
	{
		CommonSaveQingYuan * p_qing_yuan_common = p_qing_yuan->GetCommonSaveQingYuan(true);
		if (NULL != p_qing_yuan_common)
		{
			p_qing_yuan_common->has_hun_yan = qing_yuan_common_has_hun_yan;
			p_qing_yuan->SendInfo();
		}
	}

	gamelog::g_log_qing_yuan.printf(LL_INFO, "%s |"
		" wedding_id:%d. qing_yuan_id:%lld, qing_yuan_common_has_hun_yan:%d",
		__FUNCTION__,
		wedding_id, qing_yuan_id, qing_yuan_common_has_hun_yan);
}

void WeddingManger::RegistXunYou(Role * role, int xun_you_type)
{
	if (NULL == role)
	{
		return;
	}
	//改成场景操作
	Scene * scene = role->GetScene();
	if (NULL == scene)
	{
		return;
	}

	if (Scene::SCENE_TYPE_WEDDING_FB != scene->GetSceneType())
	{
		return;
	}

	SpecialLogicWeddingFB * wedding_special = (SpecialLogicWeddingFB *)scene->GetSpecialLogic();
	if (NULL == wedding_special)
	{
		return;
	}

	//可预约
	WeddingInfo *wedding_info = GetWeddingInfoByUid(role->GetUID());
	if (NULL == wedding_info)
	{
		return;
	}
	if (!wedding_info->IsValid())
	{
		return;
	}

	if (0 != wedding_info->GetCommonInfo(false).xun_you_type)
	{
		role->NoticeNum(errornum::EN_WEDDING_XUN_YOU_HAS_BUY);
		return;
	}

	//消耗
	const WeddingFB::XunYouCfg * xun_you_cfg 
		= LOGIC_CONFIG->GetQingYuanConfig()->GeXunYouCfg(xun_you_type);
	if (NULL == xun_you_cfg)
	{
		return;
	}
	
	if (!role->GetRoleModuleManager()->GetMoney()->UseMoney(xun_you_cfg->cost_type, xun_you_cfg->cost_num, __FUNCTION__, true))
	{
		role->NoticeNum(errornum::EN_CONDITIONS_NOT_REACHED);
		return;
	}
	//记录
	wedding_info->GetCommonInfo(true).xun_you_type = xun_you_type;

	gamelog::g_log_qing_yuan.printf(LL_INFO, "WeddingManger::RegistXunYou "
		"| role_id[%d] xun_you_type:[%d] wedding_id:[%d]",
		role->GetUID(), xun_you_type, wedding_info->GetWeddingID());

	wedding_special->SendSceneInfo(NULL);
	role->NoticeNum(errornum::EN_WEDDING_XUN_YOU_BUY_SUCC);

	{
		time_t xun_you_begin_time
			= wedding_info->GetBeginTime()
			- LOGIC_CONFIG->GetQingYuanConfig()->GetFbPrePareTime()
			+ LOGIC_CONFIG->GetQingYuanConfig()->GetFbTotalTime();
		tm day_tm;
		if (!GetTimeTM(xun_you_begin_time, day_tm))
		{
			return;
		}

		static MailContentParam contentparam;
		contentparam.Reset();

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject),
			gamestring::g_wedding_xun_you_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt),
			gamestring::g_wedding_xun_you_content,
			day_tm.tm_year + 1900, day_tm.tm_mon + 1, day_tm.tm_mday,
			day_tm.tm_hour, day_tm.tm_min);

		if (lensub > 0 && length > 0)
		{
			for (int i = 0; i < WeddingFB::HOST_NUM; i++)
			{
				int host_uid = wedding_info->GetHostUid(i);
				if (0 == host_uid)
				{
					continue;
				}
				MailRoute::MailToUser(IntToUid(host_uid), MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}
}

int WeddingManger::CreatXunYouCar(WeddingID wedding_id)
{
	WeddingInfo *wedding_info = GetWeddingInfo(wedding_id);
	if (NULL == wedding_info)
	{
		return -1;
	}
	if (!wedding_info->IsValid())
	{
		return -2;
	}

	const WeddingFB::XunYouCfg * xun_you_cfg
		= LOGIC_CONFIG->GetQingYuanConfig()->GeXunYouCfg(wedding_info->GetCommonInfo(false).xun_you_type);
	if (NULL == xun_you_cfg)
	{
		return -3;
	}

	const WeddingFB::XunYouWayPoint * way_point_cfg
		= LOGIC_CONFIG->GetQingYuanConfig()->GeXunYouWayPoint(0);
	if (NULL == way_point_cfg)
	{
		return -4;
	}

	Scene* scene = World::GetInstWorld()->GetSceneManager()
		->GetSceneById(xun_you_cfg->scene_id, COMMON_SCENE_KEY);
	if (NULL == scene)
	{
		return -5;
	}

	XunYouCar * car = new XunYouCar();
	car->Init(wedding_info->GetCommonInfo(false).qing_yuan_id, xun_you_cfg->xun_you_car_type);
	car->SetPos(way_point_cfg->way_poi);
	scene->AddObj(car);

	QingYuan * p_qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(car->GetQingYuanID());
	if (NULL != p_qing_yuan)
	{
		static GameName host_name_list[WeddingFB::HOST_NUM];
		memset(host_name_list, 0, sizeof(host_name_list));
		std::vector<int> host_vec = p_qing_yuan->GetMemberList();
		for (int i = 0; i < (int)host_vec.size() && i < WeddingFB::HOST_NUM; i++)
		{
			UserCacheNode *node = UserCacheManager::Instance().GetUserNode(host_vec[i]);
			if (NULL == node)
			{
				continue;
			}
			node->GetName(host_name_list[i]);
		}
		UNSTD_STATIC_CHECK(2 == WeddingFB::HOST_NUM)
		int length = SNPRINTF(gamestring::GAMESTRING_BUF,
			sizeof(gamestring::GAMESTRING_BUF),
			gamestring::g_wedding_xun_you_notice,
			host_name_list[0], host_name_list[1]);

		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}
	}

	return 0;
}

WeddingManger::WeddingManger()
{
	m_is_load_finish = 0;
	
	memset(m_wedding_info_old_flag_, 0, sizeof(m_wedding_info_old_flag_));

	m_next_save_time = 0;

	m_next_check_time = 0;

	m_wedding_vaild_check_time = 0;
}

WeddingManger::~WeddingManger()
{
}





WeddingInfo::WeddingInfo()
{
	this->Reset();
}

WeddingInfo::~WeddingInfo()
{
}

void WeddingInfo::InitDBParam(WeddingFB::WeddingInfoDBParam & init_param)
{
	this->Reset();

	m_param = init_param;

	for (int i = 0; i < WeddingFB::HOST_NUM && i < m_param.invite_info.count; i++)
	{
		int &invite_uid = m_param.invite_info.invite_uid_[i];
		m_invite_save_map[invite_uid] = i;
	}
}

WeddingFB::HostInfo * WeddingInfo::GetHost(int uid)
{
	for (int i = 0; i < WeddingFB::HOST_NUM; i++)
	{
		if (m_param.host_info_[i].uid == uid)
		{
			return &m_param.host_info_[i];
		}
	}

	return NULL;
}

int WeddingInfo::GetHostUid(int index) const
{
	if (0 > index
		|| WeddingFB::HOST_NUM <= index)
	{
		return 0;
	}
	return m_param.host_info_[index].uid;
}

void WeddingInfo::SetStatus(int finish_suatus)
{
	m_param.change_state = structcommon::CS_UPDATE;
	m_param.is_finsih = finish_suatus;
}


const WeddingFB::HostInfo * WeddingInfo::GetHostInfoList(int & out_count) const
{
	out_count = WeddingFB::HOST_NUM;
	return m_param.host_info_;
}

const WeddingFB::InviteInfo & WeddingInfo::GetInviteList() const
{
	return m_param.invite_info;
}

const WeddingInfo::ApplyTimeSet & WeddingInfo::GetApplyTimeSet() const
{
	return m_apply_set;
}

WeddingFB::CommonInfo & WeddingInfo::GetCommonInfo(bool change)
{
	if (change)
	{
		m_param.change_state = structcommon::CS_UPDATE;
	}
	return m_param.common_info;
}

int WeddingInfo::GetInviteNum()
{
	return m_param.invite_info.count;
}

int WeddingInfo::AddInvite(int invite_uid)
{
	if (this->BeInvite(invite_uid))
	{
		return -ERROR_TYPE_IN_LIST;
	}

	if ((int)m_invite_save_map.size() >= WeddingFB::MAX_INVITE_SIZE)
	{
		return -ERROR_TYPE_FULL;
	}

	//找到空位
	int save_indxe = m_param.invite_info.count;

	if (0 > save_indxe
		|| WeddingFB::MAX_INVITE_SIZE <= save_indxe)
	{
		return -777;
	}

	m_param.invite_info.invite_uid_[save_indxe] = invite_uid;
	m_invite_save_map[invite_uid] = save_indxe;
	m_param.invite_info.count++;

	m_param.change_state = structcommon::CS_UPDATE;

	return ERROR_TYPE_SUC;
}

bool WeddingInfo::BeInvite(int invite_uid)
{
	if (NULL != this->GetHost(invite_uid))
	{
		return true;
	}
	UidIndexMap::const_iterator uid_it = m_invite_save_map.find(invite_uid);
	if (m_invite_save_map.end() != uid_it)
	{
		return true;
	}
	return false;
}

int WeddingInfo::AddApply(int apply_uid)
{
	if (this->HasApply(apply_uid))
	{
		return -ERROR_TYPE_IN_LIST;
	}

	ApplyInfo new_info;
	new_info.uid = apply_uid;
	new_info.apply_time = (unsigned int)EngineAdapter::Instance().Time();

	m_apply_uid_map[apply_uid] = new_info;
	m_apply_set.insert(new_info);

	return ERROR_TYPE_SUC;
}

int WeddingInfo::RemoveApply(int apply_uid)
{
	ApplyUidMap::iterator uid_it = m_apply_uid_map.find(apply_uid);
	if (m_apply_uid_map.end() == uid_it)
	{
		return -ERROR_TYPE_NO_LIST;
	}

	ApplyInfo key_info = uid_it->second;

	//移除 迭代器失效
	m_apply_uid_map.erase(uid_it);
	m_apply_set.erase(key_info);

	return ERROR_TYPE_SUC;
}

bool WeddingInfo::HasApply(int apply_uid)
{
	ApplyUidMap::iterator uid_it = m_apply_uid_map.find(apply_uid);
	if (m_apply_uid_map.end() != uid_it)
	{
		return true;
	}
	return false;
}

void WeddingInfo::Reset()
{
	m_param.Reset();

	m_invite_save_map.clear();
	m_apply_uid_map.clear();
	m_apply_set.clear();
}

WeddingLocalManger & WeddingLocalManger::Instance()
{
	static WeddingLocalManger wedding_manger;
	return wedding_manger;
}

void WeddingLocalManger::OnRoleLogIn(Role * role)
{
	if (NULL == role)
	{
		return;
	}
	if (m_open_set.empty())
	{
		return;
	}
	//有婚宴开启
	//发送婚礼状态
	for (std::set<WeddingID>::iterator begin_it = m_open_set.begin()
		; begin_it != m_open_set.end()
		; begin_it++)
	{
		this->SendGameHiddenRoleReq(role->GetUID(),
			Protocol::WEDDING_FB_REQ_TYPE_WEDDING_OPEN_INFO,
			*begin_it);
	}
}

void WeddingLocalManger::SendGameHiddenRoleReq(int uid, int type, int param_1, int param_2)
{
	crossgameprotocal::GameHiddenWeddingRoleReq info;
	info.uid = uid;
	info.type = type;
	info.param_1 = param_1;
	info.param_2 = param_2;

	InternalComm::Instance().SendToHiddenThroughCross((char *)&info, sizeof(info));
}

void WeddingLocalManger::OnSycnWeddingInfo(crossgameprotocal::HiddenGameWeddingInfoSync * req)
{
	if (NULL == req)
	{
		return;
	}

	switch (req->type)
	{
	case crossgameprotocal::HiddenGameWeddingInfoSync::WEDDING_BEGIN:
		{
			this->AddOpenWedding(req->param_1);
		}
		break;
	case crossgameprotocal::HiddenGameWeddingInfoSync::WEDDING_END:
		{
			this->RemoveOpenWedding(req->param_1);
		}
		break;
	default:
		return;
	}
}

void WeddingLocalManger::AddOpenWedding(int wedding_id)
{
	if (m_open_set.end() != m_open_set.find(wedding_id))
	{
		return;
	}
	m_open_set.insert(wedding_id);
}

void WeddingLocalManger::RemoveOpenWedding(int wedding_id)
{
	m_open_set.erase(wedding_id);
}

WeddingLocalManger::WeddingLocalManger()
{
}

WeddingLocalManger::~WeddingLocalManger()
{
}
