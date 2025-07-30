#include "bigcrossmanager.hpp"
#include "servercommon/serverconfig/localconfig.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/serverconfig/bigcrossconfig/bigcrossserverconfig.hpp"
#include "internalcomm.h"
#include "servercommon/bigcrossserverdef.h"
#include "gamelog.h"
#include "config/sharedconfig/sharedconfig.h"
#include "global/universe_team_arena/UTAShadow.hpp"
#include "bigcrossvideoshadow.hpp"
#include "obj/character/role.h"
#include "global/bigcross/utaobshadow.hpp"
#include "global/bigcross/utachampionrecordmanager.hpp"
#include "global/cross/crossmanager.h"

BigCrossManager & BigCrossManager::Instance()
{
	static BigCrossManager bcm;
	return bcm;
}

void BigCrossManager::OnServerStart()
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;
}

void BigCrossManager::OnServerStop()
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;
}

bool BigCrossManager::IsLoadFinish() const
{
	return true;
}

void BigCrossManager::Update(unsigned int _now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;


	// 心跳包连接检查
	this->CheckBigCrossServerConnect(_now_second);
}

void BigCrossManager::OnRecvMsg(NetID _netid, const char * _msg, int _length)
{
	bigchgameprotocol::MessageHeader *header = (bigchgameprotocol::MessageHeader*)_msg;

	int msg_type = header->msg_type;
	if (msg_type < 0 || msg_type >= ARRAY_ITEM_COUNT(m_bigc_hgame_handler_list) || NULL == m_bigc_hgame_handler_list[msg_type])
	{
		return;
	}

	(this->*(m_bigc_hgame_handler_list[msg_type]))(_netid, _msg, _length);
}

bool BigCrossManager::IsBigCrossServerConnected()
{
	return ((NetID)-1 != InternalComm::Instance().m_bigc_server_netid) && 0 == m_bigc_server_connect_ret;
}

bool BigCrossManager::IsBigCrossServerConnecting()
{
	return (-1 != InternalComm::Instance().m_reconnet_bigc_handle);
}

void BigCrossManager::ConnectToBigCrossServer()
{
	if (this->IsBigCrossServerConnected() || this->IsBigCrossServerConnecting()) return;

	if (!GlobalConfig::Instance()->IsOpenBigCross())
	{
		//gamelog::g_log_world.printf(LL_INFO, "[DEBUG] ConnectToBigCrossServer IsOpenBigCross[False], Abort Connect");
		return;
	}

	if (!BigCrossServerConfig::Instance().IsOpenBigCross())
	{
		return;
	}

	const BigCrossServerConfigItem & bigc_config = BigCrossServerConfig::Instance().GetBigCrossServerConfig();

	int handle = -1;
	bool ret = InternalComm::Instance().NetConnectAsyn(bigc_config.big_cross_server_addr.ipstr.c_str(), bigc_config.big_cross_server_addr.port, &handle, bigcrossdef::BIG_CROSS_CONNECT_ASYN_TIME_OUT_MS);
	if (ret)
	{
		InternalComm::Instance().m_reconnet_bigc_handle = handle;

		if (!m_has_start_connect_to_bigc)
		{
			m_has_start_connect_to_bigc = true;
			gamelog::g_log_world.printf(LL_INFO, "[DEBUG] %u: ConnectToBigCrossServer addr[%s:%d] ...\n", (unsigned int)EngineAdapter::Instance().Time(), bigc_config.big_cross_server_addr.ipstr.c_str(), bigc_config.big_cross_server_addr.port);
		}
	}
}

void BigCrossManager::DisconnectFromBigCrossServer()
{
	if (!this->IsBigCrossServerConnected()) return;

	NetID netid = InternalComm::Instance().m_bigc_server_netid;
	if ((NetID)-1 != netid)
	{
		InternalComm::Instance().NetDisconnect(netid);
	}
}

void BigCrossManager::OnConnectBigCrossServerSucc(NetID netid)
{
	if (!m_has_connected_to_bigc)
	{
		m_has_connected_to_bigc = true;
		gamelog::g_log_world.printf(LL_INFO, "[DEBUG] %u: Connect BigCrossServer Succ!\n", (unsigned int)EngineAdapter::Instance().Time());
	}

	m_bigc_server_hearbeat_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	// 链接上聊天服务器后 , 第一个包应该是秘钥验证 ,验证未通过发包会被直接断开与bigcrossserver的连接
	this->SecretKey();
}

void BigCrossManager::OnDisconnectFromBigCrossServer(bool is_from_disconnect)
{
	if (m_has_connected_to_bigc)
	{
		if (is_from_disconnect)
		{
			UTAOBShadow::Instance().OnDisconnectFromBigCrossServer();

		}

		m_has_connected_to_bigc = false;
		m_has_start_connect_to_bigc = false;
		gamelog::g_log_world.printf(LL_INFO, "[DEBUG] %u: Disconnect From BigCrossServer! is_from_disconnect:%d \n", (unsigned int)EngineAdapter::Instance().Time(), is_from_disconnect);
	}
}

void BigCrossManager::CheckBigCrossServerConnect(unsigned int _now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

#ifdef _DEBUG
	const static int BIG_CROSS_SERVER_HEARTBEAT_TIME_OUT = 180;				// 心跳包超时时间戳
#else
	const static int BIG_CROSS_SERVER_HEARTBEAT_TIME_OUT = 60;				// 心跳包超时时间戳
#endif

	const static int BIG_CROSS_SERVER_RE_CONNECT_TIME_CHECK = 15;			// BigCross服务器重链间隔

	// 如果已经连接上
	if (this->IsBigCrossServerConnected())
	{
		if (0 != m_bigc_server_hearbeat_timestamp &&  static_cast<unsigned int>(_now_second) >= m_bigc_server_hearbeat_timestamp + BIG_CROSS_SERVER_HEARTBEAT_TIME_OUT)
		{
			m_bigc_server_hearbeat_timestamp = 0;

			this->DisconnectFromBigCrossServer();
			gamelog::g_log_world.printf(LL_INFO, "BigCrossManager::not receive chat hello more than %d s, disconnect chat_netid:%d", BIG_CROSS_SERVER_HEARTBEAT_TIME_OUT, InternalComm::Instance().m_bigc_server_netid);
		}
	}
	else
	{
		// 没链接上
		// 每隔15秒检查一下网络链接
		if (_now_second >= m_bigc_server_check_timestamp + BIG_CROSS_SERVER_RE_CONNECT_TIME_CHECK)
		{
			m_bigc_server_check_timestamp = _now_second;
			this->ConnectToBigCrossServer();
		}
	}
}

void BigCrossManager::SecretKey()
{
	bigchgameprotocol::HGameBigCSecretKey  hb_key;
	hb_key.unique_hidden_server_id = UniqueHiddenServerID(LocalConfig::Instance().GetPlatType(), CrossConfig::Instance().GetHiddenServerIDX());

	std::string  temp_key("q1]=zx/.");// 秘钥先直接写死	
	memcpy(hb_key.key, temp_key.c_str(), sizeof(hb_key.key));

	InternalComm::Instance().SendToBigCrossServer((const char *)&hb_key, sizeof(hb_key));
}

void BigCrossManager::RegisterGameServer()
{
	bigchgameprotocol::HGameBigCRegisterHGameServer reg;
	reg.unique_hidden_server_id = UniqueHiddenServerID(LocalConfig::Instance().GetPlatType(), CrossConfig::Instance().GetHiddenServerIDX());
	reg.server_open_time = static_cast<unsigned int>(SHAREDCONFIG->GetOpenServerConfig().GetOpenServerZeroTime());

	InternalComm::Instance().SendToBigCrossServer((const char *)&reg, sizeof(reg));

	// 把已经注册到隐藏服的主服信息发送到 bigc里
	std::set<int> server_set;
	World::GetInstWorld()->GetServerIDSet(server_set);
	this->SyncGameServerInfoToBigCross(bigchgameprotocol::SYNC_GAME_SERVER_INFO_REQ_TYPE_REGISTER, server_set);
}

void BigCrossManager::SyncGameServerInfoToBigCross(int _sync_type, int _count, int* _server_list)
{
	bigchgameprotocol::HGameBigCSyncGameServerInfo sync;
	sync.unique_hidden_server_id = UniqueHiddenServerID(LocalConfig::Instance().GetPlatType(), CrossConfig::Instance().GetHiddenServerIDX());
	sync.req_type = _sync_type;

	for (int i = 0; i < _count && i < bigchgameprotocol::HGameBigCSyncGameServerInfo::MAX_SERVER_ID_NUM; ++i)
	{
		sync.server_id_list[i] = _server_list[i];
	}
	sync.count = _count;

	if (sync.count <= 0)
		return;

	int length = sizeof(sync) - (bigchgameprotocol::HGameBigCSyncGameServerInfo::MAX_SERVER_ID_NUM - sync.count) * sizeof(sync.server_id_list[0]);

	InternalComm::Instance().SendToBigCrossServer((const char *)&sync, length);
}

void BigCrossManager::SyncGameServerInfoToBigCross(int _sync_type, std::set<int > & _server_set)
{
	bigchgameprotocol::HGameBigCSyncGameServerInfo sync;
	sync.unique_hidden_server_id = UniqueHiddenServerID(LocalConfig::Instance().GetPlatType(), CrossConfig::Instance().GetHiddenServerIDX());
	sync.req_type = _sync_type;
	sync.count = 0;

	std::set<int >::iterator it_begin = _server_set.begin();
	std::set<int >::iterator it_end = _server_set.end();

	for (; it_begin != it_end && sync.count < bigchgameprotocol::HGameBigCSyncGameServerInfo::MAX_SERVER_ID_NUM;  ++it_begin)
	{
		sync.server_id_list[sync.count++] = *it_begin;
	}

	if (sync.count <= 0)
		return;

	int length = sizeof(sync) - (bigchgameprotocol::HGameBigCSyncGameServerInfo::MAX_SERVER_ID_NUM - sync.count) * sizeof(sync.server_id_list[0]);

	InternalComm::Instance().SendToBigCrossServer((const char *)&sync, length);
}

void BigCrossManager::OnUserLogin(const UserID& user_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->OnUserLoginHidden(user_id);
	}
	else
	{
		this->OnUserLoginOrigin(user_id);
	}	
}

void BigCrossManager::OnUserLoginHidden(const UserID& user_id)
{
	bigchgameprotocol::HGameBigCUserLoginHiddenNotice notice;
	notice.plat_type = LocalConfig::Instance().GetPlatType();
	notice.hidden_server_idx = CrossManager::Instance().GetHiddenServerIDX();
	notice.role_id = UidToInt(user_id);

	InternalComm::Instance().SendToBigCrossServer((const char*)&notice, sizeof(notice));
}

void BigCrossManager::OnUserLoginOrigin(const UserID& user_id)
{
	bigchgameprotocol::HGameBigCUserLoginOriginNotice notice;
	notice.plat_type = LocalConfig::Instance().GetPlatType();
	notice.hidden_server_idx = CrossManager::Instance().GetHiddenServerIDX();
	notice.role_id = UidToInt(user_id);

	InternalComm::Instance().SendToBigCrossServer((const char*)&notice, sizeof(notice));
}

void BigCrossManager::OnUTAGiveServerItems(NetID _netid, const char* _data, int _length)
{
	bigchgameprotocol::BigCHGameUTAGiveItemsToServer real_msg;
	int offset = 0;
	if (!real_msg.Deserialize(_data, _length, &offset))
	{
		return;
	}

	gamelog::g_log_uta.buff_printf("OnUTAGiveServerItems item_list[");
	for (size_t i = 0; i < real_msg.itemlist.size(); ++i)
	{
		gamelog::g_log_uta.buff_printf("{%d,%d},", 
			real_msg.itemlist[i].item_id, real_msg.itemlist[i].num);
	}
	gamelog::g_log_uta.buff_printf("]");
	gamelog::g_log_uta.commit_buff(LL_INFO);

	crossgameprotocal::HiddenGameUTAGiveItemsToServer transfer_msg;
	transfer_msg.itemlist = real_msg.itemlist;
	transfer_msg.mail_subject = real_msg.mail_subject;
	transfer_msg.mail_content = real_msg.mail_content;
	InternalComm::Instance().SendSerializeMsgToAllGameThroughCross(&transfer_msg);
}

void BigCrossManager::OnUTAScheduleTeamMemberDetailResp(NetID _netid, const char* _data, int _length)
{
	bigchgameprotocol::BigCHGameUTAScheduleTeamMemberDetailResp* resp = 
		(bigchgameprotocol::BigCHGameUTAScheduleTeamMemberDetailResp*)_data;

	UTAShadow::Instance().OnUTAScheduleTeamMemberDetailResp(resp);
}

void BigCrossManager::OnTransferMsgFromBigcrossBattleToHidden(NetID _netid, const char* _data, int _length)
{
	bigchgameprotocol::BigCHGameTransferMsgFromBattleToHidden* msg = (bigchgameprotocol::BigCHGameTransferMsgFromBattleToHidden*)_data;
	const char* real_msg = msg->data;
	int real_length = msg->length;

	World::GetInstWorld()->GetSceneManager()->OnRecvMsgFromBattle(NetID(), real_msg, real_length);
}

void BigCrossManager::OnSyncVideoData(NetID _netid, const char* _data, int _length)
{
	bigchgameprotocol::BigCHGameSyncVideoData* msg = (bigchgameprotocol::BigCHGameSyncVideoData*)_data;
	BigCrossVideoShadow::Instance().SetBigCrossVideo(msg->video_id, msg->data, msg->length);

	const char* video_data = msg->data;
	int video_length = msg->length;
	World::GetInstWorld()->TrySendMsgToUser(IntToUid(msg->role_id), video_data, video_length);
}

void BigCrossManager::OnUTASyncBattleInfoForOB(NetID _netid, const char* _data, int _length)
{
	bigchgameprotocol::BigCHGameUTASyncBattleInfoForOB real_msg;
	int offset = 0;
	if (!real_msg.Deserialize(_data, _length, &offset))
	{
		return;
	}
	UTAOBShadow::Instance().OnSyncBattleInfoData(&real_msg);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		InternalComm::Instance().TransferBigCrossMsgToAllOriginGame(_data, _length);
	}
}

void BigCrossManager::OnUTASyncRoundResultForOB(NetID _netid, const char* _data, int _length)
{
	bigchgameprotocol::BigCHGameUTASyncRoundResultForOB* msg = (bigchgameprotocol::BigCHGameUTASyncRoundResultForOB*)_data;
	UTAOBShadow::Instance().OnSyncRoundResultData(msg);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		InternalComm::Instance().TransferBigCrossMsgToAllOriginGame(_data, _length);
	}
}

void BigCrossManager::OnUTABattleEndStopOB(NetID _netid, const char* _data, int _length)
{
	bigchgameprotocol::BigCHGameUTABattleEndStopOB* msg = (bigchgameprotocol::BigCHGameUTABattleEndStopOB*)_data;
	UTAOBShadow::Instance().OnBattleEndStopOB(msg->schedule_id);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		InternalComm::Instance().TransferBigCrossMsgToAllOriginGame(_data, _length);
	}
}

void BigCrossManager::OnUTAChampionRecordRequestUserUpdateData(NetID _netid, const char* _data, int _length)
{
	bigchgameprotocol::BigCHGameUTAChampionRecordRequestUpdateData* msg = (bigchgameprotocol::BigCHGameUTAChampionRecordRequestUpdateData*)_data;
	UTAChampionRecordManager::Instance().OnRequestUpdateUserData(msg->role_id);
}

void BigCrossManager::OnUTAUserEnterSceneContinueBattle(NetID _netid, const char* _data, int _length)
{
	bigchgameprotocol::BigCHGameUserEnterSceneContinueUTABattle* msg = (bigchgameprotocol::BigCHGameUserEnterSceneContinueUTABattle*)_data;
	
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(msg->role_id));
	if (NULL == role) return;

	int scene_id = 0;
	int scene_key = 0;
	if (!UTAShadow::Instance().TryReEnterBattleScene(msg->role_id, &scene_id, &scene_key))
	{
		return;
	}

	if (role->GetScene()->GetSceneID() == scene_id && 
		role->GetScene()->GetSceneKey() == scene_key)
	{
		return;
	}

	World::GetInstWorld()->GetSceneManager()->GoTo(role, scene_id, scene_key);
}

void BigCrossManager::OnBattleEndNoticeTeam(NetID _netid, const char* _data, int _length)
{
	bigchgameprotocol::BigCHGameUTABattleEndNoticeTeam* msg = (bigchgameprotocol::BigCHGameUTABattleEndNoticeTeam*)_data;
	UTAShadow::Instance().OnBattleEndNoticeTeam(msg->schedule_id, msg->team_unique_id);
}

BigCrossManager::BigCrossManager()
	:m_has_start_connect_to_bigc(false), m_has_connected_to_bigc(false), m_bigc_server_check_timestamp(0), m_bigc_server_hearbeat_timestamp(0), m_bigc_server_connect_ret(0)
{
	this->InitBigCHGameHandler();
}

BigCrossManager::~BigCrossManager()
{

}
