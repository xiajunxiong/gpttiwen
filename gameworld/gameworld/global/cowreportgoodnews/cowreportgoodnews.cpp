#include "cowreportgoodnews.hpp"

#include "engineadapter.h"
#include "global/rmibackobjdef.h"
#include "rmiclient/rmiglobal.hpp"
#include "internalcomm.h"
#include "serverlogic.h"
#include "gamelog.h"
#include "global/commonsave/commonsavemanager.hpp"

#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"

#include "config/logicconfigmanager.hpp"
#include "config/sharedconfig/sharedconfig.h"
#include "global/cowreportgoodnews/cowreportgoodnewsconfig.hpp"

#include "obj/character/role.h"
#include "global/usercache/usercache.hpp"
#include "other/rolemodulemanager.hpp"
#include "item/money.h"
#include "other/funopen/funopen.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"

#include "other/robot/robotmanager.hpp"
#include "servercommon/robotdef.hpp"
#include "other/robot/robotconfig.hpp"
#include "global/worldstatus/worldstatus.hpp"

#include "other/route/mailroute.hpp"
#include "servercommon/noticenum.h"

void RMIInitCowReportGoodNewsBuyListBackObjectImpl::InitCowReportGoodNewsBuyListRet(int _ret, CowReportGoodNewsBuyListParam & _param)
{
	CowReportGoodNewsManager::Instance().InitParam(_ret, _param);
}

CowReportGoodNewsManager & CowReportGoodNewsManager::Instance()
{
	static CowReportGoodNewsManager st_mg;
	return st_mg;
}

void CowReportGoodNewsManager::InitParam(int _ret, CowReportGoodNewsBuyListParam & _param)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if (0 != _ret)
	{
		printf(" CowReportGoodNewsManager::InitParam Failed . ret:%d  \n", _ret);
		ServerLogic::GetInstServerLogic()->StopServer("CowReportGoodNewsManager::InitParam");
		EngineAdapter::Instance().StopGame();
		return;
	}
	else
	{
		for (int i = 0; i < _param.count && i < MAX_ONCE_COW_REPORT_GOOD_NEWS_COUNT; ++i)
		{
			int role_id = _param.user_list[i].role_id;
			std::map<int, CowReportGoodNewsBuyInfo>::iterator it = m_buy_map.find(role_id);
			if (m_buy_map.end() != it)
			{
				gamelog::g_log_cow_report_good_news.printf(LL_INFO, "CowReportGoodNewsManager::InitParam[ERROR_1] role_id :%d ", _param.user_list[i].role_id);
				continue;
			}

			m_buy_map.insert(std::make_pair(role_id, _param.user_list[i]));
			m_old_state.insert(role_id);// 标记该数据是已经存在的数据
		}

		if (_param.count >= MAX_ONCE_COW_REPORT_GOOD_NEWS_COUNT)
		{
			RMIInitCowReportGoodNewsBuyListBackObjectImpl *impl = new RMIInitCowReportGoodNewsBuyListBackObjectImpl();

			RMIGlobalClient ac;
			ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
			ac.InitCowReportGoodNewsBuyListDataAsyn(_param.id_from, impl);
		}
		else
		{
			printf("Init CowReportGoodNewsManager::InitParam Succ,  size:%d  \n", (int)m_buy_map.size());
			m_load_flag = true;
		}
	}

	this->CheckLoadFinish();
}

void CowReportGoodNewsManager::OnServerStart()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	{
		// 初始化好友信息
		RMIInitCowReportGoodNewsBuyListBackObjectImpl *impl = new RMIInitCowReportGoodNewsBuyListBackObjectImpl();

		RMIGlobalClient ac;
		ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
		ac.InitCowReportGoodNewsBuyListDataAsyn(0, impl);
	}
}

void CowReportGoodNewsManager::OnServerStop()
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	this->Save();
}

bool CowReportGoodNewsManager::IsLoadFinish() const
{
	if (CrossConfig::Instance().IsHiddenServer())
		return true;

	return (COW_REPORT_GOOD_NEWS_DATA_STATUS_LOAD_FINISH == m_data_state);
}

void CowReportGoodNewsManager::Update(unsigned long interval, time_t now_second)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if (COW_REPORT_GOOD_NEWS_DATA_STATUS_LOAD_FINISH != m_data_state) return;

	CommonSaveCowReportGoodNewsData * save_data = this->GetCommonSaveCowReportGoodNewsData();
	if (NULL == save_data)
		return;

	const CowReportGoodNewsOtherCfg & other_cfg = LOGIC_CONFIG->GetCowReportGoodNewsConfig()->GetOtherCfg();
	unsigned int open_server_time = (unsigned int)SHAREDCONFIG->GetOpenServerConfig().GetServerRealStartTime();
	if (0 == save_data->buy_start_times)
	{
		if (open_server_time < other_cfg.timestamp)
		{
			return;
		}
	}

	static bool is_first = true;
	if (is_first)
	{
		if (0 == save_data->buy_start_times)
		{
			// 初始化时间
			save_data->buy_start_times = GetZeroTime(open_server_time) + other_cfg.start_time * 3600;
			save_data->buy_end_times = save_data->buy_start_times + other_cfg.until_time * 3600;
			save_data->show_start_times = save_data->buy_end_times + other_cfg.known_time * 3600;
			save_data->show_end_times = save_data->show_start_times + other_cfg.show_time * 3600;
			this->SetCommonSaveDirtyMark();
		}

		is_first = false;
	}

	if (this->IsFinish())
		return;

	this->CalcStatus(now_second);

	static time_t last_save_time = now_second;

	if (now_second >= last_save_time + COW_REPORT_GOOD_NEWS_SAVE_INTERVAL)
	{
		this->Save();
		last_save_time = now_second;
	}
}

void CowReportGoodNewsManager::OnLogin(Role * _role)
{
	if (NULL == _role)
		return;

	this->SendInfo(_role);
	this->SendBuyInfo(_role);

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		std::map<int, CowReportGoodNewsBuyInfo>::iterator it = m_buy_map.find(_role->GetUID());
		if (m_buy_map.end() != it)
		{
			if (0 == it->second.last_notice_timestamp)
			{
				// 飘字提醒
				Protocol::SCSystemMsg sm;
				int sendlen = 0;
				int remind_len = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_cow_report_good_news_remind);
				if (remind_len > 0)
				{
					unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
					if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, remind_len, GetMsgType(SYS_MSG_FLOAT)))
					{
						it->second.last_notice_timestamp = now_second;
						EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char*)&sm, sendlen);
						this->SetDirtyMark(it->first);
					}
				}
			}
		}
	}
}

void CowReportGoodNewsManager::OnCowReportGoodNewsOfflineEventByLogin(Role * _role, int _val)
{
	if (NULL == _role)
		return;

	// 返还元宝 
	_role->GetRoleModuleManager()->GetMoney()->AddGold(_val, __FUNCTION__);
	//  同步清理另一个服务器上的信息
	this->SyncCowReportGoodNewsReturnGoldOfflineEvent(_role->GetUID(), 0);
}

void CowReportGoodNewsManager::OnCowReportGoodNewsCanBuyFromHidden(crossgameprotocal::HiddenGameCowReportGoodNewsCheckCanBuy * _msg)
{
	if (NULL == _msg || CrossConfig::Instance().IsHiddenServer())
		return;

	CommonSaveCowReportGoodNewsData * save_data = this->GetCommonSaveCowReportGoodNewsData();
	if (NULL == save_data)
	{
		gamelog::g_log_cow_report_good_news.printf(LL_INFO, "%s[ERROR_1]  user[%d ] , save_data == NULL",
			__FUNCTION__, _msg->role_id);
		return;
	}

	// 先检查是否还在购买时限里
	if (!this->IsBuyTimes())
	{
		this->SendCanBuyAckToHidden(_msg->role_id, errornum::EN_COW_REPORT_GOOD_NEWS_CAN_NOT_BUY);
		gamelog::g_log_cow_report_good_news.printf(LL_INFO, "%s[ERROR_2]  user[%d] curr_status:%d  now_second:%u  buy_start_time:%u  buy_end_time:%u",
			__FUNCTION__, _msg->role_id, save_data->curren_status, (unsigned int)EngineAdapter::Instance().Time(), save_data->buy_start_times, save_data->buy_end_times);
		return;
	}

	// 是否已经买过了
	if (IsBuy(_msg->role_id))
	{
		this->SendCanBuyAckToHidden(_msg->role_id, errornum::EN_COW_REPORT_GOOD_NEWS_BUY_TIMES_LIMIT);
		return;
	}

	this->SendCanBuyAckToHidden(_msg->role_id, 0);

	// 记录日志用于查反馈
	gamelog::g_log_cow_report_good_news.printf(LL_INFO, "%s  user[%d] curr_status:%d  now_second:%u  buy_start_time:%u  buy_end_time:%u",
		__FUNCTION__, _msg->role_id, save_data->curren_status, (unsigned int)EngineAdapter::Instance().Time(), save_data->buy_start_times, save_data->buy_end_times);
}

void CowReportGoodNewsManager::OnCowReportGoodNewsBuyItemFromHidden(crossgameprotocal::HiddenGameCowReportGoodNewsBuyItem * _msg)
{
	if (NULL == _msg || CrossConfig::Instance().IsHiddenServer())
		return;

	CommonSaveCowReportGoodNewsData * save_data = this->GetCommonSaveCowReportGoodNewsData();
	if (NULL == save_data)
	{
		gamelog::g_log_cow_report_good_news.printf(LL_INFO, "%s[ERROR_1]  user[%d] , save_data == NULL",
			__FUNCTION__, _msg->role_id);
		return;
	}

	//再次检查是否还在购买时限里
	if (!this->IsBuyTimes())
	{
		this->SendBuyAckToHidden(_msg->role_id, errornum::EN_COW_REPORT_GOOD_NEWS_CAN_NOT_BUY);
		gamelog::g_log_cow_report_good_news.printf(LL_INFO, "%s[ERROR_2]  user[%d] curr_status:%d  now_second:%u  buy_start_time:%u  buy_end_time:%u",
			__FUNCTION__, _msg->role_id, save_data->curren_status, (unsigned int)EngineAdapter::Instance().Time(), save_data->buy_start_times, save_data->buy_end_times);
		return;
	}

	// 是否已经买过了
	if (IsBuy(_msg->role_id))
	{
		this->SendBuyAckToHidden(_msg->role_id, errornum::EN_COW_REPORT_GOOD_NEWS_BUY_TIMES_LIMIT);
		return;
	}

	// 在隐藏服已经用过钱了 ,这里不需要再去判断货币消耗了

	// 添加购买
	CowReportGoodNewsBuyInfo buy_info;
	buy_info.Reset();
	buy_info.role_id = _msg->role_id;
	m_buy_map.insert(std::make_pair(_msg->role_id, buy_info));
	this->SetDirtyMark(_msg->role_id);

	// 购买完成通知玩家
	this->SendBuyInfoByGameWorld(_msg->role_id);
	this->SendBuyAckToHidden(_msg->role_id, 0);

	// 记录一下日志
	gamelog::g_log_cow_report_good_news.printf(LL_INFO, "%s user[%d]",
		__FUNCTION__, _msg->role_id);
}

void CowReportGoodNewsManager::CowReportGoodNewsGetInfoFromHidden(crossgameprotocal::HiddenGameCowReportGoodNewsGetInfo * _get_info)
{
	if (NULL == _get_info || CrossConfig::Instance().IsHiddenServer())
		return;

	switch (_get_info->type)
	{
	case crossgameprotocal::HIDDEN_GAME_COW_REPORT_GOOD_NEWS_GET_INFO_TYPE_SYS_INFO:
		{
			this->SendInfoToHidden(_get_info->role_id);
		}
		break;
	case crossgameprotocal::HIDDEN_GAME_COW_REPORT_GOOD_NEWS_GET_INFO_TYPE_REWARD_INFO:
		{
			this->SendRewardInfoToHidden(_get_info->role_id);
		}
		break;
	case crossgameprotocal::HIDDEN_GAME_COW_REPORT_GOOD_NEWS_GET_INFO_TYPE_BUY_INFO:
		{
			this->SendBuyInfoByGameWorld(_get_info->role_id);
		}
		break;
	default:
		break;
	}
}

void CowReportGoodNewsManager::OnCowReportGoodNewsCheckCanBuyAckFromGameWorld(crossgameprotocal::GameHiddenCowReportGoodNewsCheckCanBuyAck * _ack)
{
	if (NULL == _ack || !CrossConfig::Instance().IsHiddenServer())
		return;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(_ack->role_id);
	if (NULL == role)
		return;

	if (0 == _ack->ret)
	{
		// 表示可以购买
		this->BuyItemToGameWorld(role);
	}
	else
	{
		// 错误码 通知玩家
		role->NoticeNum(_ack->ret);
	}
}

void CowReportGoodNewsManager::OnCowReportGoodNewsBuyItemAckFromGameWorld(crossgameprotocal::GameHiddenCowReportGoodNewsBuyItemAck * _ack)
{
	if (NULL == _ack || !CrossConfig::Instance().IsHiddenServer())
		return;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(_ack->role_id);
	if (NULL == role)
	{
		if (0 != _ack->ret)
		{
			// 购买失败 且 玩家不在线 , 这是最糟糕的状态 
			// 返还玩家消耗的元宝添加到离线 , 玩家登录是补发 ,
			// 同步消息到回主服同步添加离线 , 不管玩家下次哪条线登录 ,都能补发到
			// 补发后再同步清除
			const CowReportGoodNewsOtherCfg & other_cfg = LOGIC_CONFIG->GetCowReportGoodNewsConfig()->GetOtherCfg();
			OfflineRegisterManager::Instance().AddCowReportGoodNewsReturnGoldOfflineEvent(_ack->role_id, other_cfg.current_price);
			this->SyncCowReportGoodNewsReturnGoldOfflineEvent(_ack->role_id, other_cfg.current_price);
		}
		return;
	}

	// 0= 表示购买成功了 无视就好了 , 主要对失败做处理
	if (0 != _ack->ret)
	{
		// 返还玩家消耗的元宝
		const CowReportGoodNewsOtherCfg & other_cfg = LOGIC_CONFIG->GetCowReportGoodNewsConfig()->GetOtherCfg();
		role->GetRoleModuleManager()->GetMoney()->AddGold(other_cfg.current_price, __FUNCTION__);
		//提示错误
		role->NoticeNum(_ack->ret);
	}
	else
	{
		role->NoticeNum(noticenum::NT_COW_REPORT_GOOD_NEWS_BUY_SUCC);
	}
}

void CowReportGoodNewsManager::OnSyncCowReportGoodNewsBuyItemOfflineEvent(crossgameprotocal::SyncCowReportGoodNewsBuyItemOfflineEvent * _sync)
{
	if (NULL == _sync)
		return;

	OfflineRegisterManager::Instance().AddCowReportGoodNewsReturnGoldOfflineEvent(IntToUid(_sync->role_id), _sync->return_val);
}

void CowReportGoodNewsManager::OnClientReq(Role * _role, Protocol::CSCowReportGoodNewsClientReq * _req)
{
	if (NULL == _req || NULL == _role)
		return;

	if (!FunOpen::IsFunOpen(_role, FUN_TYPE_COW_REPORT_GOOD_NEWS))
		return;

	switch (_req->req_type)
	{
	case Protocol::COW_REPORT_GOOD_NEWS_CLIENT_REQ_TYPE_BUY:
		{
			this->OnBuyItem(_role);
		}
		break;
	case Protocol::COW_REPORT_GOOD_NEWS_CLIENT_REQ_TYPE_GET_REWARD_LIST:
		{
			this->SendRewardInfo(_role);
		}
		break;
	default:
		break;
	}
}

void CowReportGoodNewsManager::GmCowReportGoodNewsResetData()
{
	// 清理活动的系统数据
	CommonSaveCowReportGoodNewsData * save_data = this->GetCommonSaveCowReportGoodNewsData();
	if (NULL == save_data)
		return;
	save_data->Reset();
	this->SetCommonSaveDirtyMark();

	// 清理购买数据
	std::map<int, CowReportGoodNewsBuyInfo>::iterator it_begin = m_buy_map.begin();
	while (it_begin != m_buy_map.end())
	{
		this->SetDirtyMark(it_begin->first);
		m_buy_map.erase(it_begin);
		it_begin = m_buy_map.begin();
	}

	// 重新初始化活动信息
	const CowReportGoodNewsOtherCfg & other_cfg = LOGIC_CONFIG->GetCowReportGoodNewsConfig()->GetOtherCfg();
	unsigned int open_server_time = (unsigned int)SHAREDCONFIG->GetOpenServerConfig().GetServerRealStartTime();
	if (open_server_time < other_cfg.timestamp)
	{
		return;
	}

	if (0 == save_data->buy_start_times)
	{
		// 初始化时间
		save_data->buy_start_times = GetZeroTime(open_server_time) + other_cfg.start_time * 3600;
		save_data->buy_end_times = save_data->buy_start_times + other_cfg.until_time * 3600;
		save_data->show_start_times = save_data->buy_end_times + other_cfg.known_time * 3600;
		save_data->show_end_times = save_data->show_start_times + other_cfg.show_time * 3600;
		this->SetCommonSaveDirtyMark();
	}
	
	this->CalcStatus((unsigned int)EngineAdapter::Instance().Time());

	this->SendInfo(NULL);

	Protocol::SCCowReportGoodNewsRoleBuyInfo info;
	info.is_buy = 0;

	World::GetInstWorld()->SendToAllGateway((const char*)&info, sizeof(info));
	InternalComm::Instance().SendToHiddenUniqueServerUserThroughCross((const char*)&info, sizeof(info));
}

CowReportGoodNewsManager::CowReportGoodNewsManager()
	:m_data_state(COW_REPORT_GOOD_NEWS_DATA_STATUS_INVALID), m_load_flag(false), m_change_flag(false)
{

}

CowReportGoodNewsManager::~CowReportGoodNewsManager()
{

}

void CowReportGoodNewsManager::Save()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	this->SaveParam();
}

void CowReportGoodNewsManager::SaveParam()
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if (!m_change_flag)
	{
		return;
	}

	static CowReportGoodNewsBuyListParam param;
	param.count = 0;
	int count = 0;

	std::set < int >::iterator it_dirty_begin = m_dirty_mark.begin();
	std::set < int >::iterator it_dirty_end = m_dirty_mark.end();
	for (; it_dirty_begin != it_dirty_end && count < MAX_ONCE_COW_REPORT_GOOD_NEWS_COUNT; ++it_dirty_begin)
	{
		std::map<int, CowReportGoodNewsBuyInfo>::iterator it = m_buy_map.find(*it_dirty_begin);
		if (m_buy_map.end() == it)
		{
			std::set < int> ::iterator it_old = m_old_state.find(*it_dirty_begin);
			if (m_old_state.end() == it_old)
			{
				// 既然之前不存在于数据库 ,那就直接丢弃这份数据好了 没必要去数据库里计算
				continue;
			}
			else
			{
				// 这个ID之前存在于数据里内 , 那么去数据库里删除一下
				param.user_list[count].change_state = structcommon::CS_DELETE;
				param.user_list[count].role_id = *it_dirty_begin;
			}
		}
		else
		{
			param.user_list[count] = it->second;

			std::set < int > ::iterator it_old = m_old_state.find(*it_dirty_begin);
			if (m_old_state.end() == it_old)
			{
				param.user_list[count].change_state = structcommon::CS_INSERT;
			}
			else
			{
				param.user_list[count].change_state = structcommon::CS_UPDATE;
			}
		}
		++count;
	}

	param.count = count;

	if (0 != param.count)
	{
		RMISaveCowReportGoodNewsBuyListBackObjectImpl *backobj = new RMISaveCowReportGoodNewsBuyListBackObjectImpl();
		RMIGlobalClient ac;
		ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

		bool ret = ac.SaveCowReportGoodNewsBuyListDataAsyn(param, backobj);
		if (ret)
		{
			this->ClearDirtyMark();
		}
	}
}

void CowReportGoodNewsManager::ClearDirtyMark()
{
	if (!m_change_flag)
		return;

	m_dirty_mark.clear();
	m_old_state.clear();

	std::map<int, CowReportGoodNewsBuyInfo>::iterator it_begin = m_buy_map.begin();
	std::map<int, CowReportGoodNewsBuyInfo>::iterator it_end = m_buy_map.end();
	for (; it_begin != it_end; ++it_begin)
	{
		m_old_state.insert(it_begin->first);
	}

	m_change_flag = false;
}

void CowReportGoodNewsManager::SetDirtyMark(int _role_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if ((int)m_dirty_mark.size() >= MAX_ONCE_COW_REPORT_GOOD_NEWS_COUNT)
	{
		this->SaveParam();
	}

	m_change_flag = true;
	m_dirty_mark.insert(_role_id);
}

void CowReportGoodNewsManager::CheckLoadFinish()
{
	if (m_load_flag)
		m_data_state = COW_REPORT_GOOD_NEWS_DATA_STATUS_LOAD_FINISH;

	if (COW_REPORT_GOOD_NEWS_DATA_STATUS_LOAD_FINISH == m_data_state)
	{
		// 如果有需要加载完成时进行处理的在这里处理
	}
}

void CowReportGoodNewsManager::ClearDataByFinish()
{
	std::map<int, CowReportGoodNewsBuyInfo>::iterator it_begin = m_buy_map.begin();
	while (it_begin != m_buy_map.end())
	{
		this->SetDirtyMark(it_begin->first);
		m_buy_map.erase(it_begin);
		it_begin = m_buy_map.begin();
	}

	CommonSaveCowReportGoodNewsData * save_data = this->GetCommonSaveCowReportGoodNewsData();
	if (NULL == save_data)
	{
		gamelog::g_log_cow_report_good_news.printf(LL_INFO, "%s[ERROR_1] , save_data == NULL",
			__FUNCTION__);
		return;
	}

	memset(save_data->reward_user_info, 0, sizeof(save_data->reward_user_info));
	this->SetCommonSaveDirtyMark();
}

void CowReportGoodNewsManager::NoticeStatusChange()
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	this->NoticeStatusChangeByGameWorld();
	this->NoticeStatusChangeToHidden();
}

void CowReportGoodNewsManager::NoticeStatusChangeByGameWorld()
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	this->SendInfo(NULL);
}

void CowReportGoodNewsManager::NoticeStatusChangeToHidden()
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	CommonSaveCowReportGoodNewsData * save_data = this->GetCommonSaveCowReportGoodNewsData();
	if (NULL == save_data)
		return;

	Protocol::SCCowReportGoodNewsInfo info;
	info.curren_status = save_data->curren_status;
	info.buy_start_times = save_data->buy_start_times;
	info.buy_end_times = save_data->buy_end_times;
	info.show_start_times = save_data->show_start_times;
	info.show_end_times = save_data->show_end_times;

	InternalComm::Instance().SendToHiddenUniqueServerUserThroughCross((const char*)&info, sizeof(info));
}

void CowReportGoodNewsManager::GetInfoToGameWorld(int _role_id, int _type)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::HiddenGameCowReportGoodNewsGetInfo info;
	info.role_id = _role_id;
	info.type = _type;

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(_role_id).db_index, (const char *)&info, sizeof(info));
}

void CowReportGoodNewsManager::SendInfo(Role * _role)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->GetInfoToGameWorld(_role->GetUID(), crossgameprotocal::HIDDEN_GAME_COW_REPORT_GOOD_NEWS_GET_INFO_TYPE_SYS_INFO);
	}
	else
	{
		this->SendInfoByGameWorld(_role);
	}
}

void CowReportGoodNewsManager::SendInfoByGameWorld(Role * _role)
{
	CommonSaveCowReportGoodNewsData * save_data = this->GetCommonSaveCowReportGoodNewsData();
	if (NULL == save_data)
		return;

	Protocol::SCCowReportGoodNewsInfo info;
	info.curren_status = save_data->curren_status;
	info.buy_start_times = save_data->buy_start_times;
	info.buy_end_times = save_data->buy_end_times;
	info.show_start_times = save_data->show_start_times;
	info.show_end_times = save_data->show_end_times;

	if (NULL == _role)
	{
		World::GetInstWorld()->SendToAllGateway((const char*)&info, sizeof(info));
	}
	else
	{
		EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char*)&info, sizeof(info));
	}
}

void CowReportGoodNewsManager::SendInfoToHidden(int _role_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	CommonSaveCowReportGoodNewsData * save_data = this->GetCommonSaveCowReportGoodNewsData();
	if (NULL == save_data)
		return;

	Protocol::SCCowReportGoodNewsInfo info;
	info.curren_status = save_data->curren_status;
	info.buy_start_times = save_data->buy_start_times;
	info.buy_end_times = save_data->buy_end_times;
	info.show_start_times = save_data->show_start_times;
	info.show_end_times = save_data->show_end_times;

	InternalComm::Instance().SendToHiddenUserThroughCross(_role_id, (const char*)&info, sizeof(info));
}

void CowReportGoodNewsManager::SendRewardInfo(Role * _role)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->GetInfoToGameWorld(_role->GetUID(), crossgameprotocal::HIDDEN_GAME_COW_REPORT_GOOD_NEWS_GET_INFO_TYPE_REWARD_INFO);
	}
	else
	{
		this->SendRewardInfoByGameWorld(_role);
	}
}

void CowReportGoodNewsManager::SendRewardInfoByGameWorld(Role * _role)
{
	if (NULL == _role || CrossConfig::Instance().IsHiddenServer())
		return;

	if (!IsShowTime())
	{
		_role->NoticeNum(errornum::EN_COW_REPORT_GOOD_NEWS_IS_NOT_SHOW_TIME);
		return;
	}

	CommonSaveCowReportGoodNewsData * save_data = this->GetCommonSaveCowReportGoodNewsData();
	if (NULL == save_data)
	{
		return;
	}

	Protocol::SCCowReportGoodNewsRewardListInfo info;

	for (int i = 0; i < MAX_COW_REPORT_GOOD_NEWS_REWARD_USER_COUNT; ++i)
	{
		info.user_info[i].uid = save_data->reward_user_info[i].role_id;
		F_STRNCPY(info.user_info[i].role_name, save_data->reward_user_info[i].role_name, sizeof(GameName));
		info.user_info[i].avatar_type = save_data->reward_user_info[i].avatar_type;
		info.user_info[i].headshot_id = save_data->reward_user_info[i].headshot_id;
	}

	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char *)&info, sizeof(info));
}

void CowReportGoodNewsManager::SendRewardInfoToHidden(int _role_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if (!IsShowTime())
	{
		return;
	}

	CommonSaveCowReportGoodNewsData * save_data = this->GetCommonSaveCowReportGoodNewsData();
	if (NULL == save_data)
	{
		return;
	}

	Protocol::SCCowReportGoodNewsRewardListInfo info;

	for (int i = 0; i < MAX_COW_REPORT_GOOD_NEWS_REWARD_USER_COUNT; ++i)
	{
		info.user_info[i].uid = save_data->reward_user_info[i].role_id;
		F_STRNCPY(info.user_info[i].role_name, save_data->reward_user_info[i].role_name, sizeof(GameName));
		info.user_info[i].avatar_type = save_data->reward_user_info[i].avatar_type;
		info.user_info[i].headshot_id = save_data->reward_user_info[i].headshot_id;
	}

	InternalComm::Instance().SendToHiddenUserThroughCross(_role_id, (const char*)&info, sizeof(info));
}

void CowReportGoodNewsManager::SendBuyInfo(Role * _role)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->GetInfoToGameWorld(_role->GetUID(), crossgameprotocal::HIDDEN_GAME_COW_REPORT_GOOD_NEWS_GET_INFO_TYPE_BUY_INFO);
	}
	else
	{
		this->SendBuyInfoByGameWorld(_role->GetUID());
	}
}

void CowReportGoodNewsManager::SendBuyInfoByGameWorld(int _role_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	bool is_buy = false;
	std::map<int, CowReportGoodNewsBuyInfo>::iterator it = m_buy_map.find(_role_id);
	if (m_buy_map.end() != it)
	{
		is_buy = true;
	}

	Protocol::SCCowReportGoodNewsRoleBuyInfo info;
	info.is_buy = is_buy;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_role_id));
	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));
	}
	else
	{
		// 玩家不在线 应该是从隐藏服来获取的 发送到隐藏服去吧
		InternalComm::Instance().SendToHiddenUserThroughCross(_role_id, (const char*)&info, sizeof(info));
	}
}

void CowReportGoodNewsManager::GmResetSendBuyInfo()
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	Protocol::SCCowReportGoodNewsRoleBuyInfo info;
	info.is_buy = 0;
	World::GetInstWorld()->SendToAllGateway((const char*)&info, sizeof(info));
	InternalComm::Instance().SendToHiddenUniqueServerUserThroughCross((const char*)&info, sizeof(info));
}

void CowReportGoodNewsManager::SendCanBuyAckToHidden(int _role_id, int _ret)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenCowReportGoodNewsCheckCanBuyAck ack;

	ack.role_id = _role_id;
	ack.ret = _ret;

	InternalComm::Instance().SendToHiddenThroughCross((const char*)&ack, sizeof(ack));
}

void CowReportGoodNewsManager::SendBuyAckToHidden(int _role_id, int _ret)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenCowReportGoodNewsBuyItemAck ack;

	ack.role_id = _role_id;
	ack.ret = _ret;

	InternalComm::Instance().SendToHiddenThroughCross((const char*)&ack, sizeof(ack));
}

void CowReportGoodNewsManager::SyncCowReportGoodNewsReturnGoldOfflineEvent(int _role_id, int _val)
{
	crossgameprotocal::SyncCowReportGoodNewsBuyItemOfflineEvent offlien_event;

	offlien_event.role_id = _role_id;
	offlien_event.return_val = _val;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(_role_id).db_index, (char *)&offlien_event, sizeof(offlien_event));
	}
	else
	{
		InternalComm::Instance().SendToHiddenThroughCross((char *)&offlien_event, sizeof(offlien_event));
	}
}

void CowReportGoodNewsManager::CalcStatus(unsigned int now_second)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	CommonSaveCowReportGoodNewsData * save_data = this->GetCommonSaveCowReportGoodNewsData();
	if (NULL == save_data)
		return;

	bool is_need_notice = false, is_change = false;
	if (COW_REPORT_GOOD_NEWS_STATUS_INVALID == save_data->curren_status && now_second >= save_data->buy_start_times)
	{
		save_data->curren_status = COW_REPORT_GOOD_NEWS_STATUS_BUY;
		is_need_notice = true;
		is_change = true;
	}
	else if (COW_REPORT_GOOD_NEWS_STATUS_BUY == save_data->curren_status && now_second >= save_data->buy_end_times)
	{
		save_data->curren_status = COW_REPORT_GOOD_NEWS_STATUS_REWARD;
		is_need_notice = true;
		is_change = true;
		this->CalcReward();
	}
	else if (COW_REPORT_GOOD_NEWS_STATUS_REWARD == save_data->curren_status && now_second >= save_data->show_start_times)
	{
		save_data->curren_status = COW_REPORT_GOOD_NEWS_STATUS_SHOW;
		is_need_notice = true;
		is_change = true;
		this->GiveReward();
	}
	else if (COW_REPORT_GOOD_NEWS_STATUS_SHOW == save_data->curren_status && now_second >= save_data->show_end_times)
	{
		save_data->curren_status = COW_REPORT_GOOD_NEWS_STATUS_FINISH;
		is_need_notice = true;
		is_change = true;

		//活动结束了清理掉数据

	}

	if (is_change)
		this->SetCommonSaveDirtyMark();

	if (is_need_notice)
		this->NoticeStatusChange();
}

bool CowReportGoodNewsManager::IsShowTime()
{
	CommonSaveCowReportGoodNewsData * save_data = this->GetCommonSaveCowReportGoodNewsData();
	if (NULL == save_data)
		return false;

	return COW_REPORT_GOOD_NEWS_STATUS_SHOW == save_data->curren_status;
}

bool CowReportGoodNewsManager::IsBuyTimes()
{
	CommonSaveCowReportGoodNewsData * save_data = this->GetCommonSaveCowReportGoodNewsData();
	if (NULL == save_data)
		return false;

	return COW_REPORT_GOOD_NEWS_STATUS_BUY == save_data->curren_status;
}

bool CowReportGoodNewsManager::IsRewardTimes()
{
	CommonSaveCowReportGoodNewsData * save_data = this->GetCommonSaveCowReportGoodNewsData();
	if (NULL == save_data)
		return false;

	return COW_REPORT_GOOD_NEWS_STATUS_REWARD == save_data->curren_status;
}

bool CowReportGoodNewsManager::IsFinish()
{
	CommonSaveCowReportGoodNewsData * save_data = this->GetCommonSaveCowReportGoodNewsData();
	if (NULL == save_data)
		return true;

	return COW_REPORT_GOOD_NEWS_STATUS_FINISH == save_data->curren_status;
}

bool CowReportGoodNewsManager::IsBuy(int _role_id)
{
	std::map<int, CowReportGoodNewsBuyInfo>::iterator it = m_buy_map.find(_role_id);
	if (m_buy_map.end() == it)
		return false;

	return true;
}

void CowReportGoodNewsManager::OnBuyItem(Role * _role)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->BuyItemByHidden(_role);
	}
	else
	{
		this->BuyItemByGameWorld(_role);
	}
}

void CowReportGoodNewsManager::BuyItemByHidden(Role * _role)
{
	if (NULL == _role || !CrossConfig::Instance().IsHiddenServer())
		return;

	//1. 跨服购买先发送主服去检查一下
	crossgameprotocal::HiddenGameCowReportGoodNewsCheckCanBuy hg;

	hg.role_id = _role->GetUID();

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), _role->GetUserId().db_index, (const char *)&hg, sizeof(hg));
}

void CowReportGoodNewsManager::BuyItemByGameWorld(Role * _role)
{
	if (NULL == _role || CrossConfig::Instance().IsHiddenServer())
		return;

	CommonSaveCowReportGoodNewsData * save_data = this->GetCommonSaveCowReportGoodNewsData();
	if (NULL == save_data)
	{
		gamelog::g_log_cow_report_good_news.printf(LL_INFO, "%s[ERROR_1]  user[%d %s] , save_data == NULL",
			__FUNCTION__, _role->GetUID(), _role->GetName());
		return;
	}

	// 先检查是否还在购买时限里
	if (!this->IsBuyTimes())
	{
		_role->NoticeNum(errornum::EN_COW_REPORT_GOOD_NEWS_CAN_NOT_BUY);
		gamelog::g_log_cow_report_good_news.printf(LL_INFO, "%s[ERROR_2]  user[%d %s] curr_status:%d  now_second:%u  buy_start_time:%u  buy_end_time:%u",
			__FUNCTION__, _role->GetUID(), _role->GetName(), (unsigned int)EngineAdapter::Instance().Time(), save_data->curren_status, save_data->buy_start_times, save_data->buy_end_times);
		return;
	}

	// 是否已经买过了
	if (IsBuy(_role->GetUID()))
	{
		_role->NoticeNum(errornum::EN_COW_REPORT_GOOD_NEWS_BUY_TIMES_LIMIT);
		return;
	}

	const CowReportGoodNewsOtherCfg & other_cfg = LOGIC_CONFIG->GetCowReportGoodNewsConfig()->GetOtherCfg();
	// 消耗元宝
	if (!_role->GetRoleModuleManager()->GetMoney()->UseGold(other_cfg.current_price, __FUNCTION__))
	{
		_role->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}

	// 添加购买
	CowReportGoodNewsBuyInfo buy_info;
	buy_info.Reset();
	buy_info.role_id = _role->GetUID();
	m_buy_map.insert(std::make_pair(_role->GetUID(), buy_info));
	this->SetDirtyMark(_role->GetUID());

	// 购买完成通知玩家
	this->SendBuyInfo(_role);
	_role->NoticeNum(noticenum::NT_COW_REPORT_GOOD_NEWS_BUY_SUCC);

	// 记录一下日志
	gamelog::g_log_cow_report_good_news.printf(LL_INFO, "%s user[%d %s] price:%d ",
		__FUNCTION__, _role->GetUID(), _role->GetName(), other_cfg.current_price);
}

void CowReportGoodNewsManager::BuyItemToGameWorld(Role * _role)
{
	if (NULL == _role || !CrossConfig::Instance().IsHiddenServer())
		return;

	// 消耗元宝
	const CowReportGoodNewsOtherCfg & other_cfg = LOGIC_CONFIG->GetCowReportGoodNewsConfig()->GetOtherCfg();
	if (!_role->GetRoleModuleManager()->GetMoney()->UseGold(other_cfg.current_price, __FUNCTION__))
	{
		_role->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}

	// 发送购买请求
	crossgameprotocal::HiddenGameCowReportGoodNewsBuyItem buy;

	buy.role_id = _role->GetUID();
	_role->GetName(buy.role_name);
	buy.avatar_type = _role->GetAvatarType();
	buy.headshot_id = _role->GetHeadshotID();

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), _role->GetUserId().db_index, (const char *)&buy, sizeof(buy));

	// 记录一下日志 , 记录操作用于查反馈
	gamelog::g_log_cow_report_good_news.printf(LL_INFO, "%s user[%d %s] ",
		__FUNCTION__, _role->GetUID(), _role->GetName());
}

void CowReportGoodNewsManager::CalcReward()
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	CommonSaveCowReportGoodNewsData * save_data = this->GetCommonSaveCowReportGoodNewsData();
	if (NULL == save_data)
	{
		gamelog::g_log_cow_report_good_news.printf(LL_INFO, "%s[ERROR_1]  save_data == NULL",
			__FUNCTION__);
		return;
	}

	int size = m_buy_map.size();
	if (size >= MAX_COW_REPORT_GOOD_NEWS_REWARD_USER_COUNT)
	{
		// 大于等于5 从购买列表里找
		std::set<int> temp_index_set;
		int count = 0;
		// 循环最多尝试100次
		for (int i = 100; i > 0 && count < MAX_COW_REPORT_GOOD_NEWS_REWARD_USER_COUNT; --i)
		{
			int index = rand() % size;
			std::set<int>::iterator it = temp_index_set.find(index);
			if (temp_index_set.end() != it)
			{
				continue;
			}
			else
			{
				temp_index_set.insert(index);
				++count;
			}
		}

		if (temp_index_set.size() < MAX_COW_REPORT_GOOD_NEWS_REWARD_USER_COUNT)
		{
			// 不够就补满
			for (int i = 0; i < size && temp_index_set.size() < MAX_COW_REPORT_GOOD_NEWS_REWARD_USER_COUNT; ++i)
			{
				std::set<int>::iterator it = temp_index_set.find(i);
				if (temp_index_set.end() != it)
				{
					continue;
				}
				else
				{
					temp_index_set.insert(i);
				}
			}
		}

		std::map<int, CowReportGoodNewsBuyInfo>::iterator it_begin = m_buy_map.begin();
		std::map<int, CowReportGoodNewsBuyInfo>::iterator it_end = m_buy_map.end();
		int reward_i = 0;
		for (int i = 0; it_begin != it_end && reward_i < MAX_COW_REPORT_GOOD_NEWS_REWARD_USER_COUNT; ++it_begin, ++i)
		{
			std::set<int>::iterator it = temp_index_set.find(i);
			if (temp_index_set.end() == it)
			{
				continue;
			}
			else
			{
				UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(it_begin->first);
				if (NULL != user_node)
				{
					save_data->reward_user_info[reward_i].role_id = user_node->uid;
					user_node->GetName(save_data->reward_user_info[reward_i].role_name);
					save_data->reward_user_info[reward_i].avatar_type = user_node->appearance.avatar_type;
					save_data->reward_user_info[reward_i].headshot_id = user_node->appearance.headshot_id;
					++reward_i;
				}
			}
		}
	}
	else
	{
		// 小于等于5 , 缺的用机器人补上
		std::map<int, CowReportGoodNewsBuyInfo>::iterator it_begin = m_buy_map.begin();
		std::map<int, CowReportGoodNewsBuyInfo>::iterator it_end = m_buy_map.end();
		int reward_i = 0;
		for (int i = 0; it_begin != it_end; ++it_begin, ++i)
		{
			UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(it_begin->first);
			if (NULL != user_node)
			{
				save_data->reward_user_info[reward_i].role_id = user_node->uid;
				user_node->GetName(save_data->reward_user_info[reward_i].role_name);
				save_data->reward_user_info[reward_i].avatar_type = user_node->appearance.avatar_type;
				save_data->reward_user_info[reward_i].headshot_id = user_node->appearance.headshot_id;
				++reward_i;
			}
		}

		RobotParam robot_param;
		for (int i = reward_i; i < MAX_COW_REPORT_GOOD_NEWS_REWARD_USER_COUNT; )
		{
			robot_param.Reset();
			int level = WorldStatus::Instance().GetWorldLevel();
			if (!RobotManager::Instance().ConstructRobotParamByLevel(level, ROBOT_CONFIG_USE_TYPE_RANGER_ROBOT, false, &robot_param ,0 , RobotConfig::ROBOT_NAME_TYPE_ROLE))
				continue;

			save_data->reward_user_info[reward_i].role_id = 0;
			F_STRNCPY(save_data->reward_user_info[reward_i].role_name, robot_param.robot_name, sizeof(GameName));
			save_data->reward_user_info[reward_i].avatar_type = robot_param.avatar_type;
			save_data->reward_user_info[reward_i].headshot_id = 0;	// 机器人没有头像
			++reward_i;
			++i;
		}
	}

	this->SetCommonSaveDirtyMark();

	// 记录一下开奖结果
	gamelog::g_log_cow_report_good_news.buff_printf("%s reward_LIST " , __FUNCTION__);
	for (int i = 0; i < MAX_COW_REPORT_GOOD_NEWS_REWARD_USER_COUNT; ++i)
	{
		gamelog::g_log_cow_report_good_news.buff_printf("user[%d  %s] ",
			save_data->reward_user_info[i].role_id,
			save_data->reward_user_info[i].role_name);
	}
	gamelog::g_log_cow_report_good_news.commit_buff(LL_INFO);
}

void CowReportGoodNewsManager::GiveReward()
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	std::set<int > temp_win_set;
	// 先发得奖的
	this->GiveWinReward(temp_win_set);
	// 再发未得奖的
	this->GiveLoseReward(temp_win_set);
	// 传闻
	int len = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_cow_report_good_news_notice);
	if (len > 0)
	{
		World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, len, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
	}
	// 再发飘字提醒
	Protocol::SCSystemMsg sm;
	int sendlen = 0;
	int remind_len = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_cow_report_good_news_remind);
	if (remind_len > 0)
	{
		unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, remind_len, GetMsgType(SYS_MSG_FLOAT)))
		{
			std::map<int, CowReportGoodNewsBuyInfo>::iterator it_begin = m_buy_map.begin();
			std::map<int, CowReportGoodNewsBuyInfo>::iterator it_end = m_buy_map.end();
			for (; it_begin != it_end; ++it_begin)
			{
				Role * to_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(it_begin->first));
				if (NULL != to_role)
				{
					it_begin->second.last_notice_timestamp = now_second;
					EngineAdapter::Instance().NetSend(to_role->GetNetId(), (const char*)&sm, sendlen);
					this->SetDirtyMark(it_begin->first);
				}
			}
		}
	}
}

void CowReportGoodNewsManager::GiveWinReward(std::set<int > & _set)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	CommonSaveCowReportGoodNewsData * save_data = this->GetCommonSaveCowReportGoodNewsData();
	if (NULL == save_data)
	{
		gamelog::g_log_cow_report_good_news.printf(LL_INFO, "%s[ERROR_1]  save_data == NULL",
			__FUNCTION__);
		return;
	}

	const CowReportGoodNewsOtherCfg & other_cfg = LOGIC_CONFIG->GetCowReportGoodNewsConfig()->GetOtherCfg();
	// 先构建邮件信息
	MailContentParam win_content; win_content.Reset();
	int len_sub = SNPRINTF(win_content.subject, sizeof(win_content.subject), gamestring::g_cow_report_good_news_subject);
	int len_w = SNPRINTF(win_content.contenttxt, sizeof(win_content.contenttxt),
		gamestring::g_cow_report_good_news_win_content, save_data->reward_user_info[0].role_name,
		save_data->reward_user_info[1].role_name, save_data->reward_user_info[2].role_name,
		save_data->reward_user_info[3].role_name, save_data->reward_user_info[4].role_name);
	for (int i = 0; i < other_cfg.reward_count; ++i)
	{
		win_content.item_list[i].item_id = other_cfg.reward_item[i].item_id;
		win_content.item_list[i].num = other_cfg.reward_item[i].num;
		win_content.item_list[i].is_bind = other_cfg.reward_item[i].is_bind;
	}

	// 发得奖的
	for (int i = 0; i < MAX_COW_REPORT_GOOD_NEWS_REWARD_USER_COUNT; ++i)
	{
		if (0 != save_data->reward_user_info[i].role_id)
		{
			if (len_sub > 0 && len_w > 0)
			{
				MailRoute::MailToUser(IntToUid(save_data->reward_user_info[i].role_id), MAIL_REASON_DEFAULT, win_content);

				_set.insert(save_data->reward_user_info[i].role_id);

				// 记录一下发奖的日志
				gamelog::g_log_cow_report_good_news.buff_printf("%s  win_reward  user[%d %s]  reward_item  ",
					__FUNCTION__, save_data->reward_user_info[i].role_id, save_data->reward_user_info[i].role_name);
				for (int reward_i = 0; reward_i < other_cfg.reward_count; ++reward_i)
				{
					gamelog::g_log_cow_report_good_news.buff_printf("item_info[%d %d] ",
						other_cfg.reward_item[i].item_id, other_cfg.reward_item[i].num);
				}
				gamelog::g_log_cow_report_good_news.commit_buff(LL_INFO);
			}
		}
	}
}

void CowReportGoodNewsManager::GiveLoseReward(const std::set<int > & _set)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	CommonSaveCowReportGoodNewsData * save_data = this->GetCommonSaveCowReportGoodNewsData();
	if (NULL == save_data)
	{
		gamelog::g_log_cow_report_good_news.printf(LL_INFO, "%s[ERROR_1]  save_data == NULL",
			__FUNCTION__);
		return;
	}

	const CowReportGoodNewsOtherCfg & other_cfg = LOGIC_CONFIG->GetCowReportGoodNewsConfig()->GetOtherCfg();

	// 先构建邮件信息
	MailContentParam lose_content; lose_content.Reset();
	int len_sub = SNPRINTF(lose_content.subject, sizeof(lose_content.subject), gamestring::g_cow_report_good_news_subject);
	int len_l = SNPRINTF(lose_content.contenttxt, sizeof(lose_content.contenttxt),
		gamestring::g_cow_report_good_news_lose_content, save_data->reward_user_info[0].role_name,
		save_data->reward_user_info[1].role_name, save_data->reward_user_info[2].role_name,
		save_data->reward_user_info[3].role_name, save_data->reward_user_info[4].role_name);
	// 构建未中奖人的奖励
	lose_content.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] = other_cfg.current_price;
	lose_content.virtual_item_list.virtual_item[MAIL_VIRTUAL_SILVER_COIN] = other_cfg.return_count;

	// 发奖
	std::map<int, CowReportGoodNewsBuyInfo>::iterator it_begin = m_buy_map.begin();
	std::map<int, CowReportGoodNewsBuyInfo>::iterator it_end = m_buy_map.end();
	for (; it_begin != it_end; ++it_begin)
	{
		std::set<int >::iterator it = _set.find(it_begin->first);
		if (_set.end() != it)
			continue;

		if (len_sub > 0 && len_l > 0)
		{
			MailRoute::MailToUser(IntToUid(it_begin->first), MAIL_REASON_DEFAULT, lose_content);

			// 记录一下发奖的日志
			gamelog::g_log_cow_report_good_news.printf(LL_INFO ,"%s  lose_reward  user[%d]  return_gold:%d  return_lingyu:%d ",
				__FUNCTION__, it_begin->first , other_cfg.current_price , other_cfg.return_count);
		}
	}
}

CommonSave * CowReportGoodNewsManager::GetCommonSave() const
{
	CommonSave * cs = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_COW_REPORT_GOOD_NEWS_DATA);
	return cs;
}

CommonSaveCowReportGoodNewsData * CowReportGoodNewsManager::GetCommonSaveCowReportGoodNewsData(bool is_auto_create /*= true */)
{
	CommonSave * cs = this->GetCommonSave();
	if (cs == NULL)
	{
		return NULL;
	}

	CommonSaveCowReportGoodNewsData * save_data = cs->GetTypeData<CommonSaveCowReportGoodNewsData>(0, is_auto_create);
	return save_data;
}

void CowReportGoodNewsManager::SetCommonSaveDirtyMark()
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	CommonSave * cs = this->GetCommonSave();
	if (cs == NULL)
		return;

	cs->SetDataChange(0);
}
