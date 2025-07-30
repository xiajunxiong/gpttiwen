#include "hongbaomanager.hpp"

#include "engineadapter.h"
#include "global/rmibackobjdef.h"
#include "serverlogic.h"
#include "internalcomm.h"
#include "gamelog.h"
#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"


#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/guildconfig.hpp"
#include "global/guild/guild.hpp"
#include "global/guild/guildmanager.hpp"
#include "global/guild/guildmembermanager.hpp"
#include "other/rolecross/rolecross.hpp"

#include "chat/chatmanager.h"
#include "servercommon/string/gameworldstr.h"
#include "global/offlineregister/offlineregistermanager.hpp"

#include "other/roleactivity/roleactivitymanager.hpp"

#include "global/datasynccheck/datasynccheck.hpp"

HongBaoManager & HongBaoManager::Instance()
{
	static HongBaoManager mgr;
	return mgr;
}

void HongBaoManager::Init(int _ret, int _hongbao_sys_type, HongBaoParam & _hongbao_param)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	unsigned int now_sec = (unsigned int)EngineAdapter::Instance().Time();

	if (0 != _ret || _hongbao_sys_type < 0 || _hongbao_sys_type >= HONGBAO_SYS_TYPE_MAX)
	{
		printf(" HongBaoManager::Init Failed . ret:%d  hongbao_sys_type:%d \n", _ret, _hongbao_sys_type);
		ServerLogic::GetInstServerLogic()->StopServer("HongBaoManager::Init");
		EngineAdapter::Instance().StopGame();
		return;
	}
	else
	{
		int next_hongbao_id_from = 0;

		for (int i = 0; i < _hongbao_param.count && i < MAX_ONCE_HONG_BAO_COUNT; ++i)
		{
			HongBaoItem & hongbao_item = _hongbao_param.hong_bao_list[i];

			// �ȼ���Ƿ��Ѿ�ʧЧ��
			if (now_sec >= hongbao_item.invalid_timestamp)
			{
				// ��Ҫɾ��������
				this->InitCheckInvalid(_hongbao_sys_type, hongbao_item);
				continue;
			}

			bool is_zero = false;

			{
				// ��Ϊ���������������ݵĴ���, ��Ҫ��Ϊ�˴���Ϸ�����
				if (0 != hongbao_item.ll_unique_hong_bao_id)
				{
					// �ж��Ƿ��ǺϷ������������� ,
					UniqueHongBaoID unique_hong_bao_id(hongbao_item.ll_unique_hong_bao_id);
					int server_id = unique_hong_bao_id.server_id;
					int original_server_id = LocalConfig::Instance().GetMergeServerId();

					if (server_id != original_server_id)
					{
						//  ����������������� , ����Ҫ�޸�ͬ���ɱ���������
						// ��ɾ���ɵ����� , �����ݲ���Ǵ���Ҳ����Ǳ�� , ������Ҫɾ��;
						this->SetDelecMergeMark(_hongbao_sys_type, hongbao_item.ll_unique_hong_bao_id);

						// �����µ�ll_uid
						unique_hong_bao_id.server_id = original_server_id;
						hongbao_item.ll_unique_hong_bao_id = UniqueHongBaoIDToLLUniqueHongBaoID(unique_hong_bao_id);
						// ���뵽������vec�� , ��ʱ���ID��ûˢ�� , ���ں���ˢ��
						m_delay_data_vec.push_back(hongbao_item);
						// ����ֻ���뵽�ӳٴ����� , �ɺ���������
						continue;
					}
				}
				else
				{
					is_zero = true;
					// ���ɾ��������
					UniqueHongBaoID old_unique_hong_bao_id(0, hongbao_item.hongbao_sys_type, hongbao_item.hong_bao_id);
					long long ll_unique_id = UniqueHongBaoIDToLLUniqueHongBaoID(old_unique_hong_bao_id);
					// Ҫɾ��������Ҫ����Ѵ���
					m_old_state[_hongbao_sys_type].insert(ll_unique_id);
					this->SetDirtyMark(_hongbao_sys_type, ll_unique_id);

					// �����ݴ���
					int original_server_id = LocalConfig::Instance().GetMergeServerId();
					UniqueHongBaoID unique_hong_bao_id(original_server_id, hongbao_item.hongbao_sys_type, hongbao_item.hong_bao_id);
					hongbao_item.ll_unique_hong_bao_id = UniqueHongBaoIDToLLUniqueHongBaoID(unique_hong_bao_id);
					// ��Ҫȥ��������
					this->SetDirtyMark(_hongbao_sys_type, hongbao_item.ll_unique_hong_bao_id);
				}
			}

			UniqueServerID u_sid = LocalConfig::Instance().GetUniqueServerID();

			if (HONGBAO_SYS_TYPE_GUILD == _hongbao_sys_type)
			{
				this->AddHongBaoIDToGuildMap(hongbao_item.info_data.guild_id, hongbao_item.hong_bao_id, u_sid);
			}

			this->InitUIDSet(hongbao_item);
			this->AddHongBaoItem(_hongbao_sys_type, hongbao_item, u_sid);
			this->AddInvalidInfo(hongbao_item.invalid_timestamp, _hongbao_sys_type, hongbao_item.ll_unique_hong_bao_id);
			if (!is_zero)
			{
				// ��0�Ĳ����Ѵ��� , 0������Ҫ�¼�
				m_old_state[_hongbao_sys_type].insert(hongbao_item.ll_unique_hong_bao_id);
			}

			if (hongbao_item.hong_bao_id > next_hongbao_id_from)
			{
				next_hongbao_id_from = hongbao_item.hong_bao_id;
			}
		}

		if (_hongbao_param.count >= MAX_ONCE_HONG_BAO_COUNT)
		{
			RMIInitHongBaoBackObjectImpl *impl = new RMIInitHongBaoBackObjectImpl();

			RMIGlobalClient ac;
			ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
			ac.InitHongBaoDataAsyn(next_hongbao_id_from, _hongbao_sys_type, impl);
		}
		else
		{
			unsigned int size = 0;
			// ����ֻ����һ��
			HongBaoServerMapIt it_server = m_hong_bao_arr[_hongbao_sys_type].begin();
			if (m_hong_bao_arr[_hongbao_sys_type].end() != it_server)
			{
				size = it_server->second.size();
			}

			printf("Init HongBao Succ,  HongBaoSysType:%d  Count: %u\n", _hongbao_sys_type, size);

			gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::Init finish load data at %u %lu", (unsigned int)EngineAdapter::Instance().Time(), EngineAdapter::Instance().GetGameTime());

			m_load_flag[_hongbao_sys_type] = true;
		}
	}

	this->CheckLoadFinish();
}

void HongBaoManager::OnGameworldAccept()
{
	if (!this->IsLoadFinish() || CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	for (int i = 0; i < HONGBAO_SYS_TYPE_MAX; ++i)
	{
		this->SyncAllDataToHidden(i);
	}
}

void HongBaoManager::OnSyncHongBaoInfoFromGame(crossgameprotocal::GameHiddenSyncHongBaoInfo * _hongbao_info)
{
	if (NULL == _hongbao_info || 0 > _hongbao_info->count || MAX_ONCE_HONG_BAO_COUNT < _hongbao_info->count ||
		_hongbao_info->sys_type < 0 || _hongbao_info->sys_type >= HONGBAO_SYS_TYPE_MAX)
	{
		return;
	}

	if (_hongbao_info->is_init)
	{
		this->InitHongBaoInfoFromGame(_hongbao_info);

		//��֤�û�ִ
		crossgameprotocal::CrossGameDataSyncCheckAck ack;
		ack.type = crossgameprotocal::DATA_SYNC_CHECK_TYPE_HONG_BAO;
		InternalComm::Instance().SendToGameThroughCross(_hongbao_info->from_usid.plat_type, _hongbao_info->from_usid.server_id, (const char *)&ack, sizeof(ack));
	}
	else
	{
		this->UpdataHongBaoInfoFromGame(_hongbao_info);
	}
}

void HongBaoManager::OnSyncReqGiveRetFromGame(crossgameprotocal::GameHiddenSyncHongBaoReqGiveRet * _req_ret)
{
	if (NULL == _req_ret)
		return;

	if (_req_ret->ret)
	{
		// �����ɹ�
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_req_ret->req_uid));
		if (NULL != role)
		{
			// �����Ϣ�����������
			this->OnHongBaoNotifyToChat(role, _req_ret->sys_type, _req_ret->type, _req_ret->hongbao_id, _req_ret->greeting);
		}
		// �����߲�����
	}
	else
	{
		// ����ʧ�� , ��Ҫ����Ҳ�����
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_req_ret->req_uid));
		if (NULL != role)
		{
			// �������ֱ�Ӳ�
			role->GetRoleModuleManager()->AddHongBaoTimes(_req_ret->type, __FUNCTION__);
		}
		else
		{
			// �����߾Ϳ��� , �������� 
			static crossgameprotocal::HiddenGameSyncHongBaoReqError error;
			error.from_usid = _req_ret->from_usid;
			error.req_uid = _req_ret->req_uid;
			error.req_type = crossgameprotocal::HiddenGameSyncHongBaoReqError_TYPE_GIVE;

			error.hongbao_id = _req_ret->hongbao_id;
			error.sys_type = _req_ret->sys_type;
			error.type = _req_ret->type;
			error.val = 0;

			InternalComm::Instance().SendToGameThroughCross(_req_ret->from_usid.plat_type, _req_ret->from_usid.server_id, (const char *)&error, sizeof(error));
		}
	}
}

void HongBaoManager::OnSyncReqGetRetFromGame(crossgameprotocal::GameHiddenSyncHongBaoReqGetRet * _req_ret)
{
	if (NULL == _req_ret)
		return;

	if (_req_ret->ret)
	{
		// �����ɹ�
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_req_ret->req_uid));
		if (NULL != role)
		{
			// ��������������
			role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(_req_ret->val, MONEY_TYPE_SILVER_COIN, __FUNCTION__);
			role->GetRoleModuleManager()->OnFetchGuildHongBao();
		}
		else
		{
			// �����߾Ϳ��� , �������� 
			static crossgameprotocal::HiddenGameSyncHongBaoReqError error;
			error.from_usid = _req_ret->from_usid;
			error.req_uid = _req_ret->req_uid;
			error.req_type = crossgameprotocal::HiddenGameSyncHongBaoReqError_TYPE_GET;

			error.hongbao_id = _req_ret->hongbao_id;
			error.sys_type = _req_ret->sys_type;
			error.type = 0;
			error.val = _req_ret->val;

			InternalComm::Instance().SendToGameThroughCross(_req_ret->from_usid.plat_type, _req_ret->from_usid.server_id, (const char *)&error, sizeof(error));
		}
	}
	else
	{
		// ����ʧ�� , ֪ͨһ����Ҳ���ʧ����
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_req_ret->req_uid));
		if (NULL != role)
		{
			role->NoticeNum(_req_ret->val);
		}
		// �����߾Ͳ�����
	}
}

void HongBaoManager::OnHiddenGameSyncHongBaoReqGiveFromHidden(crossgameprotocal::HiddenGameSyncHongBaoReqGive * _give_msg)
{
	if (NULL == _give_msg)
		return;

	this->GiveHongBaoFromHidden(_give_msg->give_hongbao_user_info);
}

void HongBaoManager::OnHiddenGameSyncHongBaoReqGetFromHidden(crossgameprotocal::HiddenGameSyncHongBaoReqGet * _get_msg)
{
	if (NULL == _get_msg)
		return;

	this->GetHongBaoFromHidden(_get_msg);
}

void HongBaoManager::OnHiddenGameSyncHongBaoReqError(crossgameprotocal::HiddenGameSyncHongBaoReqError * _error)
{
	if (NULL == _error)
		return;

	// ��ȷ��һ������ڲ�����
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(_error->req_uid));
	if (NULL != role)
	{
		// ���� , �Ͳ�������ҿ�
		switch (_error->req_type)
		{
		case crossgameprotocal::HiddenGameSyncHongBaoReqError_TYPE_GIVE:
			{
				role->GetRoleModuleManager()->AddHongBaoTimes(_error->type, __FUNCTION__);
			}
			break;
		case crossgameprotocal::HiddenGameSyncHongBaoReqError_TYPE_GET:
			{
				role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(_error->val, MONEY_TYPE_SILVER_COIN, __FUNCTION__);
				role->GetRoleModuleManager()->OnFetchGuildHongBao();
			}
			break;
		}
	}
	else
	{
		// ������ , �ӵ����������� , ���������ʱ����
		switch (_error->req_type)
		{
		case crossgameprotocal::HiddenGameSyncHongBaoReqError_TYPE_GIVE:
			{
				OfflineRegisterManager::Instance().AddHongBaoTimes(IntToUid(_error->req_uid), _error->type, 1);
			}
			break;
		case crossgameprotocal::HiddenGameSyncHongBaoReqError_TYPE_GET:
			{
				OfflineRegisterManager::Instance().AddHongBaoSilverCoin(IntToUid(_error->req_uid), _error->val);
			}
			break;
		}
	}
}

void HongBaoManager::OnServerStart()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	UniqueServerID u_sid = LocalConfig::Instance().GetUniqueServerID();

	for (int i = 0; i < HONGBAO_SYS_TYPE_MAX; ++i)
	{
		// ��ʼ��
		HongBaoServerMapIt it_server = m_hong_bao_arr[i].find(u_sid);
		if (m_hong_bao_arr[i].end() == it_server)
		{
			HongBaoMap temp_map;
			m_hong_bao_arr[i].insert(std::make_pair(u_sid, temp_map));
		}

		RMIInitHongBaoBackObjectImpl *impl = new RMIInitHongBaoBackObjectImpl();

		RMIGlobalClient ac;
		ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
		ac.InitHongBaoDataAsyn(0, i, impl);
	}
}

void HongBaoManager::OnServerStop()
{
	this->Save();
}

bool HongBaoManager::IsLoadFinish() const
{
	if (CrossConfig::Instance().IsHiddenServer())
		return true;

	return (HONGBAO_DATA_STATUS_LOAD_FINISH == m_data_state);
}

void HongBaoManager::Update(unsigned long interval, time_t now_second)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if (HONGBAO_DATA_STATUS_LOAD_FINISH != m_data_state) return;

	static time_t last_save_time = now_second;

	this->CheckInvalid(now_second);

	if (now_second >= last_save_time + HONGBAO_SAVE_INTERVAL)
	{
		this->Save();
		last_save_time = now_second;
	}
}

int HongBaoManager::GetNewHongBaoID(int _sys_type)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return 0;

	if (_sys_type < 0 || _sys_type >= HONGBAO_SYS_TYPE_MAX)
	{
		return 0;
	}

	m_hongbao_id_auto_increment[_sys_type] += 1;

	if (m_hongbao_id_auto_increment[_sys_type] <= 0)
	{
		m_hongbao_id_auto_increment[_sys_type] = 1;		// ��ʼֵ������1  0����Чֵ
	}

	return m_hongbao_id_auto_increment[_sys_type];
}

void HongBaoManager::FirstChongZhi(Role * _role)
{
	if (NULL == _role) return;

	const GuildHongBaoCfg::HongBaoCfg * cfg = LOGIC_CONFIG->GetGuildConfig()->GetGuildHongBaoCfgByType(HONGBAO_GUILD_TYPE_FIRST_CHONG);
	if (NULL == cfg )
	{
		// ��һ����־ , ���������
		gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::FirstChongZhi[ERROR_1]  user[%d %s] ",
			_role->GetUID(), _role->GetName());
		return;
	}

	if (!cfg->is_take_effect)
	{
		// ��һ����־ , ���������
		gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::FirstChongZhi[ERROR_2]  user[%d %s]  is_take_effect:%d",
			_role->GetUID(), _role->GetName() , cfg->is_take_effect);
		return;
	}

	/*Guild * _guild = _role->GetGuild();
	if (NULL != _guild)
	{
		_guild->OnRankValueAdd(_role, Guild::HALLOFFAME_TYPE_RICH, cfg->redbag_heroism);
	}*/

	this->AddHongBaoTimes(_role, HONGBAO_GUILD_TYPE_FIRST_CHONG, __FUNCTION__);
}

void HongBaoManager::AddHongBaoTimesByChongZhi(Role * _role, long long _chong_zhi_gold)
{
	if (NULL == _role || _chong_zhi_gold < 0)
		return;

	// �׳���
	if (1 == _role->GetRoleModuleManager()->GetCommonData().chongzhi_count)
	{
		this->FirstChongZhi(_role);
	}

	// ��ֵ���
	UNSTD_STATIC_CHECK(HONGBAO_TYPE_MAX == 13);
	// ��ֵ�����λ ��ֵ��� 1 - 4
	for (int i = HONGBAO_GUILD_TYPE_CHONG_ZHI_1; i <= HONGBAO_GUILD_TYPE_CHONG_ZHI_4; ++i)
	{
		this->AddHongBaoTimesByChongZhi(_role, _chong_zhi_gold, i);
	}

	// ��ֵ�����λ ��ֵ��� 5 ��Ϊ��λ��Ծ�� , ���ηֿ�
	this->AddHongBaoTimesByChongZhi(_role, _chong_zhi_gold, HONGBAO_GUILD_TYPE_CHONG_ZHI_5);
}

void HongBaoManager::AddHongBaoTimesByChongZhi(Role * _role, long long _chong_zhi_gold, int _type)
{
	if (NULL == _role || _chong_zhi_gold <= 0 || _type <= HONGBAO_TYPE_INVALID ||
		_type >= HONGBAO_TYPE_MAX  || !this->IsChongZhiType(_type))
		return;

	const GuildHongBaoCfg::HongBaoCfg * cfg = LOGIC_CONFIG->GetGuildConfig()->GetGuildHongBaoCfgByType(_type);
	if (NULL == cfg)
	{
		// ��һ����־ , �������Ҳ�
		gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::AddHongBaoTimesByChongZhi[ERROR_1]  user[%d %s] type:%d  chongzhi:%lld",
			_role->GetUID(), _role->GetName(), _type, _chong_zhi_gold);
		return;
	}

	if (!cfg->is_take_effect)
	{
		// ��һ����־ , �������Ҳ�
		gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::AddHongBaoTimesByChongZhi[ERROR_2]  user[%d %s]  type:%d  chongzhi:%lld",
			_role->GetUID(), _role->GetName(), _type, _chong_zhi_gold);
		return;
	}

	if (_chong_zhi_gold != cfg->need_chong_zhi)
	{
		// ��һ����־ , �������Ҳ�
		gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::AddHongBaoTimesByChongZhi[ERROR_3] user[%d %s]  type:%d  chongzhi:%lld  cfg_chongzhi:%d",
			_role->GetUID(), _role->GetName(), _type, _chong_zhi_gold, cfg->need_chong_zhi);
		return;
	}

	int day_count = _role->GetRoleModuleManager()->GetCommonData().get_hongbao_day_times[_type];
	if (0 != cfg->day_times_limit && day_count >= cfg->day_times_limit)
	{
		// ��һ����־ , �������Ҳ�
		gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::AddHongBaoTimesByChongZhi[ERROR_4] user[%d %s]  type:%d  day_count:%d  day_limit:%d",
			_role->GetUID(), _role->GetName(), _type, day_count, cfg->day_times_limit);
		return;
	}

	/*Guild * _guild = _role->GetGuild();
	if (NULL != _guild)
	{
		_guild->OnRankValueAdd(_role, Guild::HALLOFFAME_TYPE_RICH, cfg->redbag_heroism);
	}*/

	this->AddHongBaoTimes(_role, _type, __FUNCTION__);

	// ���ǲ�����¼��־, ʵ���Ƿ����Ӧ�ÿ�����һ��������������־
	gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::AddHongBaoTimesByChongZhi user[%d %s]  type:%d  _chong_zhi:%lld",
		_role->GetUID(), _role->GetName(), _type, _chong_zhi_gold);
}

void HongBaoManager::AddHongBaoTimesByLeiJiChongZhi(Role * _role, long long _chong_zhi_gold)
{
	if (NULL == _role || _chong_zhi_gold < 0)
		return;

	UNSTD_STATIC_CHECK(HONGBAO_TYPE_MAX == 13);

	long long history_chongzhi = _role->GetRoleModuleManager()->GetRoleActivityManager()->GetCommonParam().history_chongzhi_num;
	// �ۼƳ�ֵ�����λ �ۼƳ�ֵ 1 - 4
	for (int i = HONGBAO_GUILD_TYPE_LEI_CHONG_1; i <= HONGBAO_GUILD_TYPE_LEI_CHONG_5; ++i)
	{
		this->AddHongBaoTimesByLeiJiChongZhi(_role, _chong_zhi_gold , history_chongzhi, i);
	}
}

void HongBaoManager::AddHongBaoTimesByLeiJiChongZhi(Role * _role, long long _chong_zhi_gold, long long _lei_chong_gold , int _type)
{
	if (NULL == _role || _chong_zhi_gold <= 0 || _type <= HONGBAO_TYPE_INVALID ||
		_type >= HONGBAO_TYPE_MAX || !this->IsLeiJiChongZhiType(_type))
		return;

	const GuildHongBaoCfg::HongBaoCfg * cfg = LOGIC_CONFIG->GetGuildConfig()->GetGuildHongBaoCfgByType(_type);
	if (NULL == cfg)
	{
		// ��һ����־ , �������Ҳ�
		gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::AddHongBaoTimesByLeiJiChongZhi[ERROR_1]  user[%d %s] type:%d  _lei_chong_gold:%lld",
			_role->GetUID(), _role->GetName(), _type, _lei_chong_gold);
		return;
	}

	if (!cfg->is_take_effect)
	{
		// ��һ����־ , �������Ҳ�
		gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::AddHongBaoTimesByLeiJiChongZhi[ERROR_2]  user[%d %s]  type:%d  _lei_chong_gold:%lld",
			_role->GetUID(), _role->GetName(), _type, _lei_chong_gold);
		return;
	}

	if (_lei_chong_gold < cfg->need_chong_zhi)
	{
		// ��һ����־ , �������Ҳ�
		gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::AddHongBaoTimesByLeiJiChongZhi[ERROR_3] user[%d %s]  type:%d  _lei_chong_gold:%lld  cfg_chongzhi:%d",
			_role->GetUID(), _role->GetName(), _type, _lei_chong_gold, cfg->need_chong_zhi);
		return;
	}

	int total_count = _role->GetRoleModuleManager()->GetCommonData().get_hongbao_total_times[_type];
	if (0 != total_count)
	{
		// ��һ����־ , �������Ҳ�
		gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::AddHongBaoTimesByLeiJiChongZhi[ERROR_4] user[%d %s]  type:%d  total_count:%d  _lei_chong_gold:%lld",
			_role->GetUID(), _role->GetName(), _type, total_count, _lei_chong_gold);
		return;
	}

	/*Guild * _guild = _role->GetGuild();
	if (NULL != _guild)
	{
		_guild->OnRankValueAdd(_role, Guild::HALLOFFAME_TYPE_RICH, cfg->redbag_heroism);
	}*/

	this->AddHongBaoTimes(_role, _type, __FUNCTION__);

	// ���ǲ�����¼��־, ʵ���Ƿ����Ӧ�ÿ�����һ��������������־
	gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::AddHongBaoTimesByLeiJiChongZhi user[%d %s]  type:%d  _lei_chong_gold:%lld",
		_role->GetUID(), _role->GetName(), _type, _lei_chong_gold);
}

bool HongBaoManager::IsChongZhiType(int _type)
{
	UNSTD_STATIC_CHECK(HONGBAO_TYPE_MAX == 13);
	switch (_type)
	{
	case HONGBAO_GUILD_TYPE_CHONG_ZHI_1:
	case HONGBAO_GUILD_TYPE_CHONG_ZHI_2:
	case HONGBAO_GUILD_TYPE_CHONG_ZHI_3:
	case HONGBAO_GUILD_TYPE_CHONG_ZHI_4:
	case HONGBAO_GUILD_TYPE_CHONG_ZHI_5:
		return true;
	default:
		break;
	}

	return false;
}

bool HongBaoManager::IsLeiJiChongZhiType(int _type)
{
	UNSTD_STATIC_CHECK(HONGBAO_TYPE_MAX == 13);
	switch (_type)
	{
	case HONGBAO_GUILD_TYPE_LEI_CHONG_1:
	case HONGBAO_GUILD_TYPE_LEI_CHONG_2:
	case HONGBAO_GUILD_TYPE_LEI_CHONG_3:
	case HONGBAO_GUILD_TYPE_LEI_CHONG_4:
	case HONGBAO_GUILD_TYPE_LEI_CHONG_5:
		return true;
	default:
		break;
	}

	return false;
}

void HongBaoManager::AddHongBaoTimes(Role * _role, int _hong_bao_type, const char * _reason, int _times /*= 1*/)
{
	if (NULL == _role) return;
	_role->GetRoleModuleManager()->AddHongBaoTimes(_hong_bao_type, _reason, _times);
}

void HongBaoManager::GetHongBao(Role * _role, int _hongbao_sys_type, int _hongbao_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->GetHongBaoByHidden(_role, _hongbao_sys_type, _hongbao_id);
	}
	else
	{
		this->GetHongBaoByGameWorld(_role, _hongbao_sys_type, _hongbao_id);
	}
}

void HongBaoManager::GetHongBaoByGameWorld(Role * _role, int _hongbao_sys_type, int _hongbao_id)
{
	if (NULL == _role || _hongbao_sys_type < 0 || _hongbao_sys_type >= HONGBAO_SYS_TYPE_MAX)
		return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		_role->NoticeNum(errornum::EN_HONG_BAO_GET_LIMIT_FOR_HIDDEN);
		return;
	}

	// ��Ϊ����ֻ����һ�� , ��ô�鷳��Ϊ�˼��ݿ���ڻ�ȡ��Ϣ
	HongBaoServerMapIt  it_server = m_hong_bao_arr[_hongbao_sys_type].begin();
	if (m_hong_bao_arr[_hongbao_sys_type].end() == it_server)
	{
		_role->NoticeNum(errornum::EN_HONG_BAO_INVALID_TIMESTAMP);
		return;
	}

	HongBaoMapIt it = it_server->second.find(_hongbao_id);
	if (it_server->second.end() == it)
	{
		_role->NoticeNum(errornum::EN_HONG_BAO_INVALID_TIMESTAMP);
		return;
	}

	if (!this->CanGetHongBao(_role, _hongbao_sys_type, it->second))
	{
		return;
	}

	if (it->second.hongbao_reward_info.total_count <= it->second.hongbao_reward_info.get_count)
	{
		_role->NoticeNum(errornum::EN_HONG_BAO_OVER);
		return;
	}

	// ����
	// 1. ��¼��Ϣ
	int old_index = it->second.hongbao_reward_info.get_count;
	this->UpdataHongBaoBaseInfo(_role, it->second.hongbao_reward_info.get_info[old_index]);
	it->second.get_uid_set.insert(_role->GetUID());
	it->second.used_val += it->second.hongbao_reward_info.get_info[old_index].val;
	++it->second.hongbao_reward_info.get_count;

	this->SetDirtyMark(_hongbao_sys_type, it->second.ll_unique_hong_bao_id);

	// 2. ���Ž���
	_role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(it->second.hongbao_reward_info.get_info[old_index].val, MONEY_TYPE_SILVER_COIN, __FUNCTION__);
	_role->GetRoleModuleManager()->OnFetchGuildHongBao();

	// 3. �߻�Ҫ��ʵʱˢ�� , �㲥�����Ϣ
	this->SendHongBaoBaseInfoChange(_role, it->second, Protocol::SCHongBaoBaseInfoChange::CHANGE_TYPE_ADD, old_index);

	// 4. ͬ����Ϣ�����  
	this->SyncDataToHidden(_hongbao_sys_type, crossgameprotocal::SYNC_HONG_BAO_TO_CROSS_STATUS_UPDATA, it->second, old_index);

	// 5. ��¼��־
	gamelog::g_log_hong_bao.printf(LL_INFO, "%s user[%d  %s]  sys_type:%d  hongbao_id:%d  hongbao_type:%d "
		" send_uid:%d  send_name:%s  used_val:%d  total_count:%d  old_index:%d   reward_val:%d ",
		__FUNCTION__, _role->GetUID(), _role->GetName(), it->second.hongbao_sys_type, it->second.hong_bao_id, it->second.hongbao_type,
		it->second.send_uid, it->second.send_name, it->second.used_val, it->second.hongbao_reward_info.total_count, old_index, it->second.hongbao_reward_info.get_info[old_index].val);

	// 6. ����Ƿ��Ѿ���ȡ��
	this->CheckHongBaoGetInfo(it->second);
}

void HongBaoManager::GetHongBaoByHidden(Role * _role, int _hongbao_sys_type, int _hongbao_id)
{
	if (NULL == _role || _hongbao_sys_type < 0 || _hongbao_sys_type >= HONGBAO_SYS_TYPE_MAX)
		return;

	// �ڿ�����ж�һ���ǲ���һ���˳�����2����
	if (_role->GetRoleModuleManager()->GetCommonData().today_quit_guild_count >= 2)
	{
		_role->NoticeNum(errornum::EN_HONG_BAO_QUIT_GUILD_GET_LIMIT);
		return;
	}

	// ��Ϣֱ�ӷ������� , ��������ȥ��֤
	static crossgameprotocal::HiddenGameSyncHongBaoReqGet get_msg;

	int plat_type = _role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType();
	int server_id = _role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId();
	UniqueServerID us_id(plat_type, server_id);

	get_msg.from_usid = us_id;
	get_msg.target_hongbao_id = _hongbao_id;
	get_msg.hongbao_sys_type = _hongbao_sys_type;
	get_msg.uid = _role->GetUID();
	memcpy(get_msg.name, _role->GetRoleModuleManager()->GetRoleCross()->GetOriginRoleName(), sizeof(GameName));

	switch (_hongbao_sys_type)
	{
	case HONGBAO_SYS_TYPE_GUILD:
		{
			get_msg.info_data.guild_id = _role->GetGuildID();
		}
		break;
	}

	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, (const char *)&get_msg, sizeof(get_msg));
}

void HongBaoManager::GetHongBaoFromHidden(const crossgameprotocal::HiddenGameSyncHongBaoReqGet * _req)
{
	if (NULL == _req)
		return;

	// ��Ϊ����ֻ����һ�� , ��ô�鷳��Ϊ�˼��ݿ���ڻ�ȡ��Ϣ
	HongBaoServerMapIt  it_server = m_hong_bao_arr[_req->hongbao_sys_type].begin();
	if (m_hong_bao_arr[_req->hongbao_sys_type].end() == it_server)
	{
		this->SyncReqGetRetToHidden(_req->uid, false, _req->target_hongbao_id, _req->hongbao_sys_type, errornum::EN_HONG_BAO_INVALID_TIMESTAMP);
		return;
	}

	HongBaoMapIt it = it_server->second.find(_req->target_hongbao_id);
	if (it_server->second.end() == it)
	{
		this->SyncReqGetRetToHidden(_req->uid, false, _req->target_hongbao_id, _req->hongbao_sys_type, errornum::EN_HONG_BAO_INVALID_TIMESTAMP);
		return;
	}

	if (!this->CanGetHongBaoFromHidden(_req, it->second))
	{
		return;
	}

	if (it->second.hongbao_reward_info.total_count <= it->second.hongbao_reward_info.get_count)
	{
		this->SyncReqGetRetToHidden(_req->uid, false, _req->target_hongbao_id, _req->hongbao_sys_type, errornum::EN_HONG_BAO_OVER);
		return;
	}

	// ����
	// 1. ��¼��Ϣ
	int old_index = it->second.hongbao_reward_info.get_count;
	this->UpdataHongBaoBaseInfoFromHidden(_req, it->second.hongbao_reward_info.get_info[old_index]);
	it->second.get_uid_set.insert(_req->uid);
	it->second.used_val += it->second.hongbao_reward_info.get_info[old_index].val;
	++it->second.hongbao_reward_info.get_count;

	this->SetDirtyMark(_req->hongbao_sys_type, it->second.ll_unique_hong_bao_id);

	// 2. ���Ž��� , ͬ����ȡ�ɹ�����Ϣ����� �����ȥ��
	this->SyncReqGetRetToHidden(_req->uid, true, _req->target_hongbao_id, _req->hongbao_sys_type, it->second.hongbao_reward_info.get_info[old_index].val);

	// 3. �߻�Ҫ��ʵʱˢ�� , �㲥�����Ϣ
	this->SendHongBaoBaseInfoChange(NULL, it->second, Protocol::SCHongBaoBaseInfoChange::CHANGE_TYPE_ADD, old_index);

	// 4. ͬ����Ϣ�����  
	this->SyncDataToHidden(_req->hongbao_sys_type, crossgameprotocal::SYNC_HONG_BAO_TO_CROSS_STATUS_UPDATA, it->second, old_index);

	// 5. ��¼��־
	gamelog::g_log_hong_bao.printf(LL_INFO, "%s user[%d  %s]  sys_type:%d  hongbao_id:%d  hongbao_type:%d "
		" send_uid:%d  send_name:%s  used_val:%d  total_count:%d  old_index:%d   reward_val:%d ",
		__FUNCTION__, _req->uid, _req->name, it->second.hongbao_sys_type, it->second.hong_bao_id, it->second.hongbao_type,
		it->second.send_uid, it->second.send_name, it->second.used_val, it->second.hongbao_reward_info.total_count, old_index, it->second.hongbao_reward_info.get_info[old_index].val);

	// 6. ����Ƿ��Ѿ���ȡ��
	this->CheckHongBaoGetInfo(it->second);
}

void HongBaoManager::UpdataHongBaoBaseInfo(Role * _role, HongBaoItem::RewardInfo & _hongbao_reward_info_item)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if (NULL == _role)
		return;

	_hongbao_reward_info_item.uid = _role->GetUID();
	_role->GetName(_hongbao_reward_info_item.name);
	_hongbao_reward_info_item.times = (unsigned int)EngineAdapter::Instance().Time();
	// ��Ҫ��ֵval , val�Ǻ���Ľ��� , �ں��������ʱ����Ѿ���ǰ�����
}

void HongBaoManager::UpdataHongBaoBaseInfoFromHidden(const crossgameprotocal::HiddenGameSyncHongBaoReqGet * _req, HongBaoItem::RewardInfo & _hongbao_reward_info_item)
{
	_hongbao_reward_info_item.uid = _req->uid;
	memcpy(_hongbao_reward_info_item.name, _req->name, sizeof(GameName));
	_hongbao_reward_info_item.times = (unsigned int)EngineAdapter::Instance().Time();
	// ��Ҫ��ֵval , val�Ǻ���Ľ��� , �ں��������ʱ����Ѿ���ǰ�����
}

bool HongBaoManager::CanGetHongBao(Role * _role, int _hongbao_sys_type, const HongBaoItem & _hongbao_item)
{
	if (NULL == _role || _hongbao_sys_type < 0 || _hongbao_sys_type >= HONGBAO_SYS_TYPE_MAX)
	{
		return false;
	}

	if (_hongbao_item.get_uid_set.find(_role->GetUID()) != _hongbao_item.get_uid_set.end())
	{
		// �Ѿ���ȡ����
		_role->NoticeNum(errornum::EN_HONG_BAO_GET_LIMIT);
		return false;
	}

	switch (_hongbao_sys_type)
	{
	case HONGBAO_SYS_TYPE_GUILD:
		{
			return this->CanGetHongBaoByGuildHongBao(_role, _hongbao_item.info_data);
		}
		break;
	default:
		break;
	}

	return true;
}

bool HongBaoManager::CanGetHongBaoFromHidden(const crossgameprotocal::HiddenGameSyncHongBaoReqGet * _req, const HongBaoItem & _hongbao_item)
{
	if (NULL == _req)
	{
		return false;
	}

	if (_hongbao_item.get_uid_set.find(_req->uid) != _hongbao_item.get_uid_set.end())
	{
		// �Ѿ���ȡ����
		this->SyncReqGetRetToHidden(_req->uid, false, _req->target_hongbao_id, _req->hongbao_sys_type, errornum::EN_HONG_BAO_GET_LIMIT);
		return false;
	}

	switch (_req->hongbao_sys_type)
	{
	case HONGBAO_SYS_TYPE_GUILD:
		{
			return this->CanGetHongBaoByGuildHongBaoFromHidden(_req, _hongbao_item.info_data);
		}
		break;
	default:
		break;
	}

	return true;
}

bool HongBaoManager::CanGetHongBaoByGuildHongBao(Role * _role, const HongBaoItem::InfoData & _info_data)
{
	if (NULL == _role)
		return false;

	GuildID guild_id = _role->GetGuildID();
	if (INVALID_GUILD_ID == guild_id)
	{
		_role->NoticeNum(errornum::EN_GUILD_NOT_IN_GUILD);
		return false;
	}

	if (_info_data.guild_id != guild_id)
	{
		_role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	if (_role->GetRoleModuleManager()->GetCommonData().today_quit_guild_count >= 2)
	{
		_role->NoticeNum(errornum::EN_HONG_BAO_QUIT_GUILD_GET_LIMIT);
		return false;
	}

	return true;
}

bool HongBaoManager::CanGetHongBaoByGuildHongBaoFromHidden(const crossgameprotocal::HiddenGameSyncHongBaoReqGet * _req, const HongBaoItem::InfoData & _info_data)
{
	if (INVALID_GUILD_ID == _req->info_data.guild_id)
	{
		this->SyncReqGetRetToHidden(_req->uid, false, _req->target_hongbao_id, _req->hongbao_sys_type, errornum::EN_GUILD_NOT_IN_GUILD);
		return false;
	}

	if (_info_data.guild_id != _req->info_data.guild_id)
	{
		this->SyncReqGetRetToHidden(_req->uid, false, _req->target_hongbao_id, _req->hongbao_sys_type, errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	return true;
}

void HongBaoManager::CheckHongBaoGetInfo(HongBaoItem & _hongbao_item)
{
	if (_hongbao_item.hongbao_reward_info.get_count != _hongbao_item.hongbao_reward_info.total_count)
	{
		return;
	}

	// ����ȡ����
	// �޸ĺ����ʧЧʱ��
	unsigned int old_invalid_timestamp = _hongbao_item.invalid_timestamp;
	// �߻�Ҫ��̶�5��
	_hongbao_item.invalid_timestamp = (unsigned int)EngineAdapter::Instance().Time() + 5;

	this->RemoveInvalidInfo(old_invalid_timestamp, _hongbao_item.hongbao_sys_type, _hongbao_item.ll_unique_hong_bao_id);
	this->AddInvalidInfo(_hongbao_item.invalid_timestamp, _hongbao_item.hongbao_sys_type, _hongbao_item.ll_unique_hong_bao_id);
}

void HongBaoManager::GiveHongBao(Role * _role, int _hongbao_sys_type, int _hongbao_type, char * _greeting)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->GiveHongBaoByHidden(_role, _hongbao_sys_type, _hongbao_type, _greeting);
	}
	else
	{
		this->GiveHongBaoByGameWorld(_role, _hongbao_sys_type, _hongbao_type, _greeting);
	}
}

void HongBaoManager::GiveHongBaoByGameWorld(Role * _role, int _hongbao_sys_type, int _hongbao_type, char * _greeting)
{
	if (NULL == _role || NULL == _greeting || _hongbao_sys_type < 0 || _hongbao_sys_type >= HONGBAO_SYS_TYPE_MAX ||
		_hongbao_type < 0 || _hongbao_type >= HONGBAO_TYPE_MAX)
		return;

	if (!this->CanGiveHongBao(_role, _hongbao_sys_type, _hongbao_type))
	{
		return;
	}

	// �����
	static HongBaoItem _new_item;
	_new_item.Reset();
	if (!this->CreateNewHongBao(_role, _hongbao_sys_type, _hongbao_type, _greeting, _new_item))
	{
		return;
	}

	// ���ʧЧ����
	this->AddInvalidInfo(_new_item.invalid_timestamp, _hongbao_sys_type, _new_item.ll_unique_hong_bao_id);

	// �㲥
	this->SendHongBaoListChange(_hongbao_sys_type, _new_item, Protocol::SCHongBaoBaseInfoChange::CHANGE_TYPE_ADD);

	// �����Ϣ�����������
	this->OnHongBaoNotifyToChat(_role, _new_item.hongbao_sys_type, _new_item.hongbao_type, _new_item.hong_bao_id, _new_item.greeting);

	// ͬ�����
	this->SyncDataToHidden(_hongbao_sys_type, crossgameprotocal::SYNC_HONG_BAO_TO_CROSS_STATUS_INSERT,
		_new_item);

	// ��¼��־        
	gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::GiveHongBao user[%d %s] sys_type%d hongbao_type:%d  hongbao_id:%d",
		_role->GetUID(), _role->GetName(), _hongbao_sys_type, _hongbao_type, _new_item.hong_bao_id);
}

void HongBaoManager::GiveHongBaoByHidden(Role * _role, int _hongbao_sys_type, int _hongbao_type, char * _greeting)
{
	if (NULL == _role || NULL == _greeting || _hongbao_sys_type < 0 || _hongbao_sys_type >= HONGBAO_SYS_TYPE_MAX ||
		_hongbao_type < 0 || _hongbao_type >= HONGBAO_TYPE_MAX)
		return;

	if (!this->CanGiveHongBao(_role, _hongbao_sys_type, _hongbao_type))
	{
		return;
	}

	// �۴��� , �����ط���ֱ�ӿ۵� , �����������ʧ���� , �ٲ�����
	if (!_role->GetRoleModuleManager()->UseHongBaoTimes(_hongbao_type, __FUNCTION__))
	{
		_role->NoticeNum(errornum::EN_HONG_BAO_TYPE_NOT_TIMES);
		return;
	}

	static crossgameprotocal::HiddenGameSyncHongBaoReqGive give_msg;
	give_msg.give_hongbao_user_info.Reset();

	int plat_type = _role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType();
	int server_id = _role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId();
	UniqueServerID us_id(plat_type, server_id);

	give_msg.give_hongbao_user_info.from_usid = us_id;
	give_msg.give_hongbao_user_info.hongbao_type = _hongbao_type;
	give_msg.give_hongbao_user_info.hongbao_sys_type = _hongbao_sys_type;
	give_msg.give_hongbao_user_info.send_uid = _role->GetUID();
	give_msg.give_hongbao_user_info.send_avatar_type = _role->GetAvatarType();
	give_msg.give_hongbao_user_info.send_headshot_id = _role->GetHeadshotID();
	memcpy(give_msg.give_hongbao_user_info.send_name, _role->GetRoleModuleManager()->GetRoleCross()->GetOriginRoleName(), sizeof(GameName));
	memcpy(give_msg.give_hongbao_user_info.greeting, _greeting, sizeof(HongBaoGreeting));

	switch (_hongbao_sys_type)
	{
	case HONGBAO_SYS_TYPE_GUILD:
		{
			give_msg.give_hongbao_user_info.info_data.guild_id = _role->GetGuildID();
		}
		break;
	}

	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, (const char *)&give_msg, sizeof(give_msg));
}

void HongBaoManager::GiveHongBaoFromHidden(const GiveHongBaoUserInfo & _info)
{
	// �����
	static HongBaoItem _new_item;
	_new_item.Reset();
	if (!this->CreateNewHongBaoFromHidden(_info, _new_item))
	{
		// ͬ��������������ط�
		this->SyncReqGiveRetToHidden(_info.send_uid, false, 0, _info.hongbao_sys_type, _info.hongbao_type);
		return;
	}

	// ���ʧЧ����
	this->AddInvalidInfo(_new_item.invalid_timestamp, _new_item.hongbao_sys_type, _new_item.ll_unique_hong_bao_id);

	// �㲥
	this->SendHongBaoListChange(_new_item.hongbao_sys_type, _new_item, Protocol::SCHongBaoBaseInfoChange::CHANGE_TYPE_ADD);

	// ͬ�����
	this->SyncDataToHidden(_new_item.hongbao_sys_type, crossgameprotocal::SYNC_HONG_BAO_TO_CROSS_STATUS_INSERT,
		_new_item);

	// ��¼��־        
	gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::GiveHongBaoFromHidden user[%d %s] sys_type%d  hongbao_type:%d  hongbao_id:%d",
		_new_item.send_uid, _new_item.send_name, _new_item.hongbao_sys_type, _new_item.hongbao_type, _new_item.hong_bao_id);

	// ͬ��������������ط� , ��Ϣ�㲥�ŵ����ط��� , ��Ϊ��ʱ�����ڲ������õ�һЩ��Ҫ������
	this->SyncReqGiveRetToHidden(_info.send_uid, true, _new_item.hong_bao_id, _new_item.hongbao_sys_type, _new_item.hongbao_type, _new_item.greeting);
}

bool HongBaoManager::CanGiveHongBao(Role * _role, int _hongbao_sys_type, int _hongbao_type)
{
	if (NULL == _role || _hongbao_sys_type < 0 || _hongbao_sys_type >= HONGBAO_SYS_TYPE_MAX ||
		_hongbao_type < 0 || _hongbao_type >= HONGBAO_TYPE_MAX)
		return false;

	switch (_hongbao_sys_type)
	{
	case HONGBAO_SYS_TYPE_GUILD:
		{
			return this->CanGiveHongBaoByGuildHongBao(_role, _hongbao_type);
		}
		break;
	default:
		break;
	}

	return true;
}

bool HongBaoManager::CanGiveHongBaoByGuildHongBao(Role * _role, int _hongbao_type)
{
	if (NULL == _role || _hongbao_type < 0 || _hongbao_type >= HONGBAO_TYPE_MAX)
		return false;


	GuildID guild_id = _role->GetGuildID();
	if (INVALID_GUILD_ID == guild_id)
	{
		_role->NoticeNum(errornum::EN_GUILD_NOT_IN_GUILD);
		return false;
	}

	if (_role->GetRoleModuleManager()->GetCommonData().today_quit_guild_count >= 2)
	{
		_role->NoticeNum(errornum::EN_HONG_BAO_QUIT_GUILD_SEED_LIMIT);
		return false;
	}

	if (_role->GetRoleModuleManager()->GetHongBaoTimes(_hongbao_type) <= 0)
	{
		_role->NoticeNum(errornum::EN_HONG_BAO_TYPE_NOT_TIMES);
		return false;
	}

	return true;
}

bool HongBaoManager::CreateNewHongBao(Role * _role, int _hongbao_sys_type, int _hongbao_type, char * _greeting, HongBaoItem & _out_item)
{
	if (NULL == _role || NULL == _greeting || _hongbao_sys_type < 0 || _hongbao_sys_type >= HONGBAO_SYS_TYPE_MAX ||
		_hongbao_type < 0 || _hongbao_type >= HONGBAO_TYPE_MAX)
		return false;

	// ����֤һ�δ���
	if (_role->GetRoleModuleManager()->GetHongBaoTimes(_hongbao_type) <= 0)
	{
		_role->NoticeNum(errornum::EN_HONG_BAO_TYPE_NOT_TIMES);
		return false;
	}

	// ��ʼ������Ļ�����Ϣ
	_out_item.hongbao_type = _hongbao_type;
	_out_item.hongbao_sys_type = _hongbao_sys_type;
	_out_item.send_uid = _role->GetUID();
	_out_item.send_avatar_type = _role->GetAvatarType();
	_out_item.send_headshot_id = _role->GetHeadshotID();
	_role->GetName(_out_item.send_name);
	memcpy(_out_item.greeting, _greeting, sizeof(HongBaoGreeting));

	bool ret = false;
	switch (_hongbao_sys_type)
	{
	case HONGBAO_SYS_TYPE_GUILD:
		{
			ret = this->CreateNewHongBaoByGuildHongBao(_role->GetGuildID(), _out_item);
		}
		break;
	default:
		return false;
	}

	if (!ret)
		return false;

	// �۴���
	if (!_role->GetRoleModuleManager()->UseHongBaoTimes(_hongbao_type, __FUNCTION__))
	{
		_role->NoticeNum(errornum::EN_HONG_BAO_TYPE_NOT_TIMES);
		return false;
	}

	HongBaoServerMapIt it_server = m_hong_bao_arr[_hongbao_sys_type].begin();
	if (m_hong_bao_arr[_hongbao_sys_type].end() == it_server)
	{
		gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::CreateNewHongBao[error_2] user[%d %s] sys_type%d hongbao_type:%d",
			_role->GetUID(), _role->GetName(), _hongbao_sys_type, _hongbao_type);
		return false;
	}

	// ��Ӻ��ID
	int new_hongbao_id = this->GetNewHongBaoID(_hongbao_sys_type);
	if (0 == new_hongbao_id)
	{
		gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::CreateNewHongBao[error] user[%d %s] sys_type%d hongbao_type:%d",
			_role->GetUID(), _role->GetName(), _hongbao_sys_type, _hongbao_type);
		return false;
	}
	_out_item.hong_bao_id = new_hongbao_id;

	// ���LL_UniqueHongBaoID
	int server_id = LocalConfig::Instance().GetMergeServerId();
	UniqueHongBaoID unique_id(server_id, _out_item.hongbao_sys_type, _out_item.hong_bao_id);
	_out_item.ll_unique_hong_bao_id = UniqueHongBaoIDToLLUniqueHongBaoID(unique_id);

	// �������
	this->AddHongBaoIndex(_hongbao_sys_type, _out_item, it_server->first);

	const GuildHongBaoCfg::HongBaoCfg * cfg = LOGIC_CONFIG->GetGuildConfig()->GetGuildHongBaoCfgByType(_hongbao_type);
	if (NULL == cfg)
		return false;

	Guild * guild = GuildManager::Instance().GetGuild(_role->GetGuildID());
	if (NULL != guild)
	{
		guild->OnRankValueAdd(_role, Guild::HALLOFFAME_TYPE_RICH, cfg->redbag_heroism);
	}

	gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::CreateNewHongBao user[%d %s] sys_type%d hongbao_type:%d",
		_role->GetUID(), _role->GetName(), _hongbao_sys_type, _hongbao_type);

	return true;
}

bool HongBaoManager::CreateNewHongBaoFromHidden(const GiveHongBaoUserInfo & _info, HongBaoItem & _out_item)
{
	// ��ʼ������Ļ�����Ϣ
	_out_item.hongbao_type = _info.hongbao_type;
	_out_item.hongbao_sys_type = _info.hongbao_sys_type;
	_out_item.send_uid = _info.send_uid;
	_out_item.send_avatar_type = _info.send_avatar_type;
	_out_item.send_headshot_id = _info.send_headshot_id;
	memcpy(_out_item.send_name, _info.send_name, sizeof(GameName));
	memcpy(_out_item.greeting, _info.greeting, sizeof(HongBaoGreeting));

	bool ret = false;
	switch (_info.hongbao_sys_type)
	{
	case HONGBAO_SYS_TYPE_GUILD:
		{
			ret = this->CreateNewHongBaoByGuildHongBao(_info.info_data.guild_id, _out_item);
		}
		break;
	default:
		return false;
	}

	if (!ret)
		return false;

	HongBaoServerMapIt it_server = m_hong_bao_arr[_info.hongbao_sys_type].begin();
	if (m_hong_bao_arr[_info.hongbao_sys_type].end() == it_server)
	{
		gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::CreateNewHongBaoFromHidden[error_1] user[%d %s] sys_type%d hongbao_type:%d",
			_info.send_uid, _info.send_name, _info.hongbao_sys_type, _info.hongbao_type);
		return false;
	}

	// ��Ӻ��ID
	int new_hongbao_id = this->GetNewHongBaoID(_info.hongbao_sys_type);
	if (0 == new_hongbao_id)
	{
		gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::CreateNewHongBaoFromHidden[error_2] user[%d %s] sys_type%d hongbao_type:%d",
			_info.send_uid, _info.send_name, _info.hongbao_sys_type, _info.hongbao_type);
		return false;
	}
	_out_item.hong_bao_id = new_hongbao_id;

	// ���LL_UniqueHongBaoID
	int server_id = LocalConfig::Instance().GetMergeServerId();
	UniqueHongBaoID unique_id(server_id, _out_item.hongbao_sys_type, _out_item.hong_bao_id);
	_out_item.ll_unique_hong_bao_id = UniqueHongBaoIDToLLUniqueHongBaoID(unique_id);

	// �������
	this->AddHongBaoIndex(_info.hongbao_sys_type, _out_item, it_server->first);

	const GuildHongBaoCfg::HongBaoCfg * cfg = LOGIC_CONFIG->GetGuildConfig()->GetGuildHongBaoCfgByType(_info.hongbao_type);
	if (NULL == cfg)
		return false;

	Guild * guild = GuildManager::Instance().GetGuild(_out_item.info_data.guild_id);
	if (NULL != guild)
	{
		guild->OnRankValueAddByUid(_info.send_uid, Guild::HALLOFFAME_TYPE_RICH, cfg->redbag_heroism);
	}

	gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::CreateNewHongBaoFromHidden user[%d %s] sys_type%d hongbao_type:%d",
		_info.send_uid, _info.send_name, _info.hongbao_sys_type, _info.hongbao_type);

	return true;
}

bool HongBaoManager::CreateNewHongBaoByGuildHongBao(int _guild_id, HongBaoItem & _out_item)
{
	_out_item.info_data.guild_id = _guild_id;

	const GuildOtherCfg & other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfgRef();

	const GuildHongBaoCfg::HongBaoCfg * cfg = LOGIC_CONFIG->GetGuildConfig()->GetGuildHongBaoCfgByType(_out_item.hongbao_type);
	if (NULL == cfg)
		return false;

	unsigned int invalid_timestamp = (unsigned int)EngineAdapter::Instance().Time() + other_cfg.redbag_time_limit;
	this->CalsNewHongBaoVal(_out_item, invalid_timestamp, cfg->redbag_num, cfg->lingyu_min, cfg->lingyu_max, cfg->lingyu_sum, cfg->lingyu_change);

	return true;
}

void HongBaoManager::CalsNewHongBaoVal(HongBaoItem & _out_item, unsigned int _invalid_times, int _redbag_max, int _min_val, int _max_val, int _total, int _change_val)
{
	// ��ǰ��ú���ڵ���ֵ

	_out_item.hongbao_reward_info.total_count = _redbag_max;
	_out_item.invalid_timestamp = _invalid_times;

	static int rand_power_list[MAX_HONG_BAO_GET_MEMBER_COUNT] = { 0 };
	memset(rand_power_list, 0, sizeof(rand_power_list));

	int total_rand_reward = _total - _redbag_max * _min_val;
	int total_power = 0;
	for (int i = 0; i < _redbag_max; i++)
	{
		rand_power_list[i] = rand() % 10000;
		total_power += rand_power_list[i];
	}

	/*
		�߻���������������Ҫ�ڷ�Χֵ�� ,�����|Сֵ���ֻ��1��
	*/

	bool has_min = false, has_max = false;
	int power = 0;
	int reward_lower_val = 0, reward_upper_val = 0;
	int curr_min_val = _min_val, curr_max_val = _max_val;
	int residual_val = 0;
	for (int i = 0; i < _redbag_max; i++)
	{
		power += rand_power_list[i];
		reward_lower_val = reward_upper_val;
		reward_upper_val = (int)(1.0f * power / total_power * total_rand_reward);
		int temp_val = reward_upper_val - reward_lower_val + _min_val;

		// ���С����Сֵ, ����Ҫ��Ӹ�ֵ
		if (curr_min_val >= temp_val)
		{
			int sub_val = curr_min_val - temp_val;
			temp_val += sub_val;
			residual_val -= sub_val;
			if (!has_min)
			{
				has_min = true;
				curr_min_val += _change_val;
			}
		}
		// ����������ֵ, ����Ҫ��С��ֵ
		if (curr_max_val <= temp_val)
		{
			int sub = temp_val - curr_max_val;

			temp_val -= sub;
			residual_val += sub;

			if (!has_max)
			{
				has_max = true;
				curr_max_val -= _change_val;
			}
		}

		_out_item.hongbao_reward_info.get_info[i].val = temp_val;
	}

	// �Կ��ܻ���ֵ���ֵ������
	if (residual_val > 0)
	{
		int curr_max = _max_val - _change_val;
		for (int i = _redbag_max - 1; i > 0 && residual_val > 0; --i)
		{
			if (_out_item.hongbao_reward_info.get_info[i].val + residual_val <= curr_max)
			{
				_out_item.hongbao_reward_info.get_info[i].val += residual_val;
				residual_val = 0;
			}
			else
			{
				int sub = curr_max - _out_item.hongbao_reward_info.get_info[i].val;
				_out_item.hongbao_reward_info.get_info[i].val = curr_max;
				residual_val -= sub;
			}
		}
	}
	else if (residual_val < 0)
	{
		residual_val *= -1;
		int curr_min = _min_val + _change_val;
		for (int i = _redbag_max - 1; i > 0 && residual_val > 0; --i)
		{
			if (_out_item.hongbao_reward_info.get_info[i].val - residual_val >= curr_min)
			{
				_out_item.hongbao_reward_info.get_info[i].val -= residual_val;
				residual_val = 0;
			}
			else
			{
				int sub = _out_item.hongbao_reward_info.get_info[i].val - curr_min;
				_out_item.hongbao_reward_info.get_info[i].val = curr_min;
				residual_val -= sub;
			}
		}
	}
}

int HongBaoManager::GetChatChannel(int _hongbao_sys_type)
{
	switch (_hongbao_sys_type)
	{
	case HONGBAO_SYS_TYPE_GUILD:
		{
			return chatdef::CHANNEL_TYPE_GUILD;
		}
	default:
		break;
	}

	return chatdef::CHANNEL_TYPE_MAX;
}

void HongBaoManager::OnHongBaoNotifyToChat(Role * _role, int _sys_type, int _type, int _hongbao_id, char * _greeting)
{
	if (NULL == _role)
		return;

	static char buffer[512] = { 0 };
	memset(buffer, 0, sizeof(buffer));

	static Protocol::CSChannelChat chat;

	int chat_channel_type = this->GetChatChannel(_sys_type);
	if (chatdef::CHANNEL_TYPE_MAX == chat_channel_type)
		return;

	chat.channel_type = CHAT_CONTENT_TYPE_TEXT;
	chat.reserve_ch = 0;
	chat.channel_type = chat_channel_type;
	chat.group_id = 0;
	memset(chat.item_info, -1, sizeof(chat.item_info));

	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_hongbao_notify_content,
		_sys_type, _type, _hongbao_id, _greeting);

	if (length <= 0)
		return;

	// SNPRINTF �����ַ������Ȳ�����'\0' ������Ҫ+1
	chat.msg_length = length + 1;
	memcpy(buffer, &chat, sizeof(Protocol::CSChannelChat));
	memcpy(buffer + sizeof(Protocol::CSChannelChat), gamestring::GAMESTRING_BUF, length + 1);

	ChatManager::Instance().OnChannelChat(_role, buffer, sizeof(Protocol::CSChannelChat) + length + 1);
}

void HongBaoManager::AddChongZhi(Role * _role, long long _chong_zhi_gold)
{
	if (NULL == _role || _chong_zhi_gold < 0)
		return;

	this->AddHongBaoTimesByChongZhi(_role, _chong_zhi_gold);
	this->AddHongBaoTimesByLeiJiChongZhi(_role, _chong_zhi_gold);

	// ��¼һ����ҵĳ�ֵ��Ϣ , ��Ҫ������ҷ���ʱ�˶�
	gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::AddChongZhi  user[%d %s]  _chong_zhi_gold:%lld",
		_role->GetUID(), _role->GetName(), _chong_zhi_gold);
}

void HongBaoManager::AddGuildHongBaoTimesByZhanLingUpGrade(Role * _role)
{
	if (NULL == _role) return;

	const GuildHongBaoCfg::HongBaoCfg * cfg = LOGIC_CONFIG->GetGuildConfig()->GetGuildHongBaoCfgByType(HONGBAO_GUILD_TYPE_ZHAN_LING);
	if (NULL == cfg)
	{
		// ��һ����־ , ���������
		gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::OnZhanLingUpGrade[ERROR_1]  user[%d %s]",
			_role->GetUID(), _role->GetName());
		return;
	}
	/*Guild * _guild = _role->GetGuild();
	if (NULL != _guild)
	{
		_guild->OnRankValueAdd(_role, Guild::HALLOFFAME_TYPE_RICH, cfg->redbag_heroism);
	}*/
	this->AddHongBaoTimes(_role, HONGBAO_GUILD_TYPE_ZHAN_LING, __FUNCTION__);
}

void HongBaoManager::OnClientReq(Role * _role, Protocol::CSHongBaoClientReq * _req)
{
	if (NULL == _req || NULL == _role) return;

	switch (_req->req_type)
	{
	case Protocol::CSHongBaoClientReq::OPERA_TYPE_SEND_LIST_REQ:
		{
			_role->GetRoleModuleManager()->SendHongBaoInfo();
		}
		break;
	case Protocol::CSHongBaoClientReq::OPERA_TYPE_GET_LIST_REQ:
		{
			this->SendHongBaoList(_role, _req->param1);
		}
		break;
	case Protocol::CSHongBaoClientReq::OPERA_TYPE_BASE_INFO_REQ:
		{
			this->SendHongBaoBaseInfo(_role, _req->param1, _req->param2);
		}
		break;
	case Protocol::CSHongBaoClientReq::OPERA_TYPE_SEND_HONG_BAO:
		{
			this->GiveHongBao(_role, _req->param1, _req->param2, _req->greeting);
		}
		break;
	case Protocol::CSHongBaoClientReq::OPERA_TYPE_GET_HONG_BAO:
		{
			this->GetHongBao(_role, _req->param1, _req->param2);
		}
		break;
	default:
		break;
	}
}

HongBaoManager::HongBaoManager()
	:m_data_state(HONGBAO_DATA_STATUS_INVALID)
{
	memset(m_load_flag, 0, sizeof(m_load_flag));
	memset(m_change_flag, 0, sizeof(m_change_flag));
	memset(m_hongbao_id_auto_increment, 0, sizeof(m_hongbao_id_auto_increment));
}

HongBaoManager::~HongBaoManager()
{

}

void HongBaoManager::InitUIDSet(HongBaoItem & _hongbao_param)
{
	for (int i = 0; i < _hongbao_param.hongbao_reward_info.get_count && i < MAX_HONG_BAO_GET_MEMBER_COUNT; ++i)
	{
		_hongbao_param.get_uid_set.insert(_hongbao_param.hongbao_reward_info.get_info[i].uid);
	}
}

void HongBaoManager::InitIDAutoIncrement()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	for (int i = 0; i < HONGBAO_SYS_TYPE_MAX; ++i)
	{
		// ��Ϊ����ֻ����һ�� , ��ô�鷳��Ϊ�˼��ݿ���ڻ�ȡ��Ϣ
		HongBaoServerMapIt  it_server = m_hong_bao_arr[i].begin();
		if (m_hong_bao_arr[i].end() == it_server)
		{
			continue;;
		}

		HongBaoMapRIt rit = it_server->second.rbegin();
		if (it_server->second.rend() != rit)
		{
			m_hongbao_id_auto_increment[i] = rit->first + 1;
		}

		if (0 >= m_hongbao_id_auto_increment[i])
		{
			m_hongbao_id_auto_increment[i] = 1; // ��ʼֵ������1  0����Чֵ
		}
	}
}

void HongBaoManager::InitDelayData()
{
	if (m_delay_data_vec.size() <= 0)		return;	for (unsigned int i = 0; i < m_delay_data_vec.size(); ++i)	{		UniqueServerID u_sid = LocalConfig::Instance().GetUniqueServerID();
		UniqueHongBaoID old_unique_hong_bao_id(m_delay_data_vec[i].ll_unique_hong_bao_id);
		old_unique_hong_bao_id.hong_bao_id = this->GetNewHongBaoID(m_delay_data_vec[i].hongbao_sys_type);

		/// ˢ��LL_unique_hong_bao_id�ͺ��ID
		m_delay_data_vec[i].ll_unique_hong_bao_id = UniqueHongBaoIDToLLUniqueHongBaoID(old_unique_hong_bao_id);
		m_delay_data_vec[i].hong_bao_id = old_unique_hong_bao_id.hong_bao_id;

		this->InitUIDSet(m_delay_data_vec[i]);
		this->AddHongBaoItem(m_delay_data_vec[i].hongbao_sys_type, m_delay_data_vec[i], u_sid);
		this->AddInvalidInfo(m_delay_data_vec[i].invalid_timestamp, m_delay_data_vec[i].hongbao_sys_type, m_delay_data_vec[i].ll_unique_hong_bao_id);

		/// ��Ϊ��Щ����������, ����ֻ�ܱ�����޸Ĳ��ܱ���Ѵ���
		this->SetDirtyMark(m_delay_data_vec[i].hongbao_sys_type, m_delay_data_vec[i].ll_unique_hong_bao_id);
	}	m_delay_data_vec.clear();
}

void HongBaoManager::InitCheckInvalid(int _sys_type, HongBaoItem & _hongbao_param)
{
	if (0 == _hongbao_param.ll_unique_hong_bao_id)
	{
		// �����ݴ���
		UniqueHongBaoID unique_id(0, _sys_type, _hongbao_param.hong_bao_id);
		// ��Ϊ��0serverid���������������ݿ����ڲ����ڵ� , ���Ի�ȥ�� hongbao_id �� sys_type��ɾ��һ��
		long long ll_unique_id = UniqueHongBaoIDToLLUniqueHongBaoID(unique_id);

		// Ҫɾ��������Ҫ����Ѵ���
		m_old_state[_sys_type].insert(ll_unique_id);
		this->SetDirtyMark(_sys_type, ll_unique_id);
	}
	else
	{
		m_old_state[_sys_type].insert(_hongbao_param.ll_unique_hong_bao_id);
		this->SetDirtyMark(_sys_type, _hongbao_param.ll_unique_hong_bao_id);
	}
}

void HongBaoManager::AddInvalidInfo(unsigned int _times, int _sys_type, LL_UniqueHongBaoID _ll_unique_id)
{
	std::map<unsigned int, std::set<LL_UniqueHongBaoID> >::iterator it = m_invaild_map[_sys_type].find(_times);
	if (m_invaild_map[_sys_type].end() != it)
	{
		it->second.insert(_ll_unique_id);
	}
	else
	{
		std::set<LL_UniqueHongBaoID > temp_set;
		temp_set.insert(_ll_unique_id);
		m_invaild_map->insert(std::make_pair(_times, temp_set));
	}
}

void HongBaoManager::RemoveInvalidInfo(unsigned int _times, int _sys_type, LL_UniqueHongBaoID _ll_unique_id)
{
	std::map<unsigned int, std::set<LL_UniqueHongBaoID> >::iterator it = m_invaild_map[_sys_type].find(_times);
	if (m_invaild_map[_sys_type].end() != it)
	{
		it->second.erase(_ll_unique_id);
	}
}

void HongBaoManager::CheckLoadFinish()
{
	bool is_finish = true;
	for (int i = 0; i < HONGBAO_SYS_TYPE_MAX; ++i)
	{
		if (!m_load_flag[i])
		{
			is_finish = false;
			break;
		}
	}

	if (is_finish)
	{
		this->InitIDAutoIncrement();
		this->InitDelayData();
		m_data_state = HONGBAO_DATA_STATUS_LOAD_FINISH;
	}
}

void HongBaoManager::CheckInvalid(unsigned int _now_sec)
{
	for (int i = 0; i < HONGBAO_SYS_TYPE_MAX; ++i)
	{
		// ����ֻ��һ��
		HongBaoServerMapIt it_server = m_hong_bao_arr[i].begin();
		if (m_hong_bao_arr[i].end() == it_server)
			continue;

		//ʧЧ��ѯ��
		while (m_invaild_map[i].size() > 0)
		{
			std::map<unsigned int, std::set<LL_UniqueHongBaoID> >::iterator it = m_invaild_map[i].begin();
			if (m_invaild_map[i].end() == it)
				break;

			if (_now_sec < it->first)
				break;

			std::set<LL_UniqueHongBaoID>::iterator it_begin = it->second.begin();
			std::set<LL_UniqueHongBaoID>::iterator it_end = it->second.end();
			for (; it_begin != it_end; ++it_begin)
			{
				UniqueHongBaoID u_hongbao_id = UniqueHongBaoID(*it_begin);

				HongBaoMapIt it = it_server->second.find(u_hongbao_id.hong_bao_id);
				if (it_server->second.end() != it)
				{
					// 1 .�㲥��Ϣ
					this->SendHongBaoListChange(i, it->second,
						Protocol::SCHongBaoBaseInfoChange::CHANGE_TYPE_DEL);

					// 2. ͬ�������
					this->SyncDataToHidden(i, crossgameprotocal::SYNC_HONG_BAO_TO_CROSS_STATUS_DELETE,
						it->second);

					// 3 .ɾ������������
					if (HONGBAO_SYS_TYPE_GUILD == i)
					{
						UniqueServerID u_sid = it_server->first;
						this->RemoveHongBaoIDToGuildMap(it->second.info_data.guild_id,
							u_hongbao_id.hong_bao_id,
							u_sid);
					}

					// 4. ɾ���������
					it_server->second.erase(u_hongbao_id.hong_bao_id);

					this->SetDirtyMark(i, UniqueHongBaoIDToLLUniqueHongBaoID(u_hongbao_id));
				}
			}

			// ��������ɾ��
			m_invaild_map[i].erase(it);
		}
	}
}

void HongBaoManager::Save()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	for (int i = 0; i < HONGBAO_SYS_TYPE_MAX; ++i)
	{
		this->SaveHongBaoParam(i);
	}
}

void RMIInitHongBaoBackObjectImpl::InitHongBaoRet(int _ret, int _hongbao_sys_type, HongBaoParam & _hongbao_param)
{
	HongBaoManager::Instance().Init(_ret, _hongbao_sys_type, _hongbao_param);
}

void HongBaoManager::SaveHongBaoParam(int _sys_type)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if (_sys_type < 0 || _sys_type >= HONGBAO_SYS_TYPE_MAX)
	{
		return;
	}

	if (!m_change_flag[_sys_type])
	{
		return;
	}

	static HongBaoParam param;
	param.count = 0;

	int count = 0;
	// ��Ϊ����ֻ����һ�� , ��ô�鷳��Ϊ�˼��ݿ���ڻ�ȡ��Ϣ
	HongBaoServerMapIt it_server = m_hong_bao_arr[_sys_type].begin();
	if (m_hong_bao_arr[_sys_type].end() == it_server)
	{
		return;
	}

	// �ȴ���Ϸ���Ҫɾ��������
	if (m_delete_merge[_sys_type].size() > 0)
	{
		std::set < LL_UniqueHongBaoID > ::iterator it_begin = m_delete_merge[_sys_type].begin();
		std::set < LL_UniqueHongBaoID >::iterator it_end = m_delete_merge[_sys_type].end();
	for (; it_begin != it_end && count < MAX_ONCE_HONG_BAO_COUNT; ++it_begin)
	{
			LL_UniqueHongBaoID temp_ll = *it_begin;
			UniqueHongBaoID unique_hong_bao_id(temp_ll);

			param.hong_bao_list[count].change_state = structcommon::CS_DELETE;
			param.hong_bao_list[count].ll_unique_hong_bao_id = temp_ll;
			param.hong_bao_list[count].hong_bao_id = unique_hong_bao_id.hong_bao_id;
			param.hong_bao_list[count].hongbao_sys_type = unique_hong_bao_id.hong_bao_sys_type;
			++count;
		}
	}

	// �ٴ�������
	std::set < LL_UniqueHongBaoID > ::iterator it_begin = m_dirty_mark[_sys_type].begin();
	std::set < LL_UniqueHongBaoID >::iterator it_end = m_dirty_mark[_sys_type].end();
	for (; it_begin != it_end && count < MAX_ONCE_HONG_BAO_COUNT; ++it_begin)
	{
		LL_UniqueHongBaoID temp_ll = *it_begin;
		UniqueHongBaoID unique_hong_bao_id(temp_ll);

		HongBaoMapIt it_find = it_server->second.find(unique_hong_bao_id.hong_bao_id);
		if (it_server->second.end() == it_find)
		{
			// ˵����������Ѿ����Ӻ���б���ɾ��
			std::set < LL_UniqueHongBaoID > ::iterator it = m_old_state[_sys_type].find(temp_ll);
			if (m_old_state[_sys_type].end() == it)
			{
				// ��Ȼ֮ǰ�����������ݿ��Ǿ�ֱ�Ӷ���������ݺ��� û��Ҫȥ���ݿ������
				continue;
			}
			else
			{
				// ���ID֮ǰ�������������� , ��ôȥ���ݿ���ɾ��һ��
				param.hong_bao_list[count].change_state = structcommon::CS_DELETE;
				param.hong_bao_list[count].ll_unique_hong_bao_id = temp_ll;
				param.hong_bao_list[count].hong_bao_id = unique_hong_bao_id.hong_bao_id;
				param.hong_bao_list[count].hongbao_sys_type = unique_hong_bao_id.hong_bao_sys_type;
			}
		}
		else
		{
			std::set < LL_UniqueHongBaoID > ::iterator it = m_old_state[_sys_type].find(*it_begin);
			if (m_old_state[_sys_type].end() == it)
			{
				param.hong_bao_list[count].change_state = structcommon::CS_INSERT;
			}
			else
			{
				param.hong_bao_list[count].change_state = structcommon::CS_UPDATE;
			}

			param.hong_bao_list[count].ll_unique_hong_bao_id = it_find->second.ll_unique_hong_bao_id;
			param.hong_bao_list[count].hong_bao_id = it_find->second.hong_bao_id;
			param.hong_bao_list[count].invalid_timestamp = it_find->second.invalid_timestamp;
			param.hong_bao_list[count].hongbao_type = it_find->second.hongbao_type;
			param.hong_bao_list[count].hongbao_sys_type = it_find->second.hongbao_sys_type;
			param.hong_bao_list[count].send_avatar_type = it_find->second.send_avatar_type;
			param.hong_bao_list[count].send_headshot_id = it_find->second.send_headshot_id;
			param.hong_bao_list[count].send_uid = it_find->second.send_uid;
			F_STRNCPY(param.hong_bao_list[count].send_name, it_find->second.send_name, sizeof(GameName));
			F_STRNCPY(param.hong_bao_list[count].greeting, it_find->second.greeting, sizeof(HongBaoGreeting));
			param.hong_bao_list[count].used_val = it_find->second.used_val;
			param.hong_bao_list[count].info_data.SetData(it_find->second.info_data.data);
			param.hong_bao_list[count].hongbao_reward_info = it_find->second.hongbao_reward_info;
		}
		++count;
	}

	param.count = count;

	if (0 != param.count)
	{
		RMISaveHongBaoBackObjectImpl *backobj = new RMISaveHongBaoBackObjectImpl();
		RMIGlobalClient ac;
		ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

		bool ret = ac.SaveHongBaoDataAsyn(param, backobj);
		if (ret)
		{
			this->ClearDirtyMark(_sys_type);
		}
	}
}

void HongBaoManager::ClearDirtyMark(int _sys_type)
{
	if (_sys_type < 0 || _sys_type >= HONGBAO_SYS_TYPE_MAX)
	{
		return;
	}

	if (!m_change_flag[_sys_type]) return;

	m_dirty_mark[_sys_type].clear();
	m_old_state[_sys_type].clear();

	// ��Ϊ����ֻ����һ�� , ��ô�鷳��Ϊ�˼��ݿ���ڻ�ȡ��Ϣ
	HongBaoServerMapIt it_server = m_hong_bao_arr[_sys_type].begin();
	if (m_hong_bao_arr[_sys_type].end() == it_server)
	{
		return;
	}

	HongBaoMapIt it_begin = it_server->second.begin();
	HongBaoMapIt it_end = it_server->second.end();
	for (; it_begin != it_end; ++it_begin)
	{
		m_old_state[_sys_type].insert(it_begin->second.ll_unique_hong_bao_id);
	}

	m_change_flag[_sys_type] = false;
}

void HongBaoManager::SetDirtyMark(int _sys_type, LL_UniqueHongBaoID _ll_unique_hong_bao_id)
{
	if (m_dirty_mark[_sys_type].size() + m_delete_merge[_sys_type].size() >= MAX_ONCE_HONG_BAO_COUNT)
	{
		this->SaveHongBaoParam(_sys_type);
	}

	m_change_flag[_sys_type] = true;
	m_dirty_mark[_sys_type].insert(_ll_unique_hong_bao_id);
}

void HongBaoManager::ClearDelecMergeMark(int _sys_type)
{
	if (_sys_type < 0 || _sys_type >= HONGBAO_SYS_TYPE_MAX)
	{
		return;
	}

	if (!m_change_flag[_sys_type]) return;

	m_delete_merge[_sys_type].clear();
	m_change_flag[_sys_type] = false;
}

void HongBaoManager::SetDelecMergeMark(int _sys_type, LL_UniqueHongBaoID _ll_unique_hong_bao_id)
{
	if (m_dirty_mark[_sys_type].size() + m_delete_merge[_sys_type].size() >= MAX_ONCE_HONG_BAO_COUNT)
	{
		this->SaveHongBaoParam(_sys_type);
	}

	m_change_flag[_sys_type] = true;
	m_delete_merge[_sys_type].insert(_ll_unique_hong_bao_id);
}

void HongBaoManager::AddHongBaoIDToGuildMap(int _guild_id, int _hongbao_id, const UniqueServerID & u_sid)
{
	HongBaoGuildServerMapIt it_server = m_hong_bao_guild_map.find(u_sid);
	if (m_hong_bao_guild_map.end() == it_server)
	{
		HongBaoGuildMap temp_map;
		std::set<int > temp_set;
		temp_set.insert(_hongbao_id);
		temp_map.insert(std::make_pair(_guild_id, temp_set));
		m_hong_bao_guild_map.insert(std::make_pair(u_sid, temp_map));
	}
	else
	{
		HongBaoGuildMapIt it = it_server->second.find(_guild_id);
		if (it_server->second.end() == it)
		{
			std::set<int > temp_set;
			temp_set.insert(_hongbao_id);
			it_server->second.insert(std::make_pair(_guild_id, temp_set));
		}
		else
		{
			it->second.insert(_hongbao_id);
		}
	}
}

void HongBaoManager::RemoveHongBaoIDToGuildMap(int _guild_id, int _hongbao_id, UniqueServerID & u_sid)
{
	HongBaoGuildServerMapIt it_server = m_hong_bao_guild_map.find(u_sid);
	if (m_hong_bao_guild_map.end() == it_server)
		return;

	HongBaoGuildMapIt it = it_server->second.find(_guild_id);
	if (it_server->second.end() != it)
	{
		it->second.erase(_hongbao_id);

		if (it->second.size() == 0)
		{
			it_server->second.erase(_guild_id);
		}
	}
}

void HongBaoManager::AddHongBaoIndex(int _sys_type, const HongBaoItem & _hongbao_item, const UniqueServerID & u_sid)
{
	this->AddHongBaoItem(_sys_type, _hongbao_item, u_sid);

	switch (_hongbao_item.hongbao_sys_type)
	{
	case HONGBAO_SYS_TYPE_GUILD:
		{
			this->AddHongBaoIDToGuildMap(_hongbao_item.info_data.guild_id, _hongbao_item.hong_bao_id, u_sid);
		}
		break;
	}
}

void HongBaoManager::AddHongBaoItem(int _sys_type, const HongBaoItem & _hongbao_item, const  UniqueServerID & u_sid)
{
	if (_sys_type < 0 || _sys_type >= HONGBAO_SYS_TYPE_MAX)
		return;

	HongBaoServerMapIt it_server = m_hong_bao_arr[_sys_type].find(u_sid);
	if (m_hong_bao_arr[_sys_type].end() != it_server)
	{
		it_server->second.insert(std::make_pair(_hongbao_item.hong_bao_id, _hongbao_item));
		this->SetDirtyMark(_sys_type, _hongbao_item.ll_unique_hong_bao_id);
	}
}

void HongBaoManager::SyncAllDataToHidden(int _sys_type)
{
	if (_sys_type < 0 || _sys_type >= HONGBAO_SYS_TYPE_MAX || CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	static crossgameprotocal::GameHiddenSyncHongBaoInfo sync_info;
	sync_info.count = 0;
	sync_info.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	sync_info.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	sync_info.sys_type = _sys_type;
	sync_info.is_init = true;
	sync_info.is_init_add = 0;

	HongBaoServerMapIt it_server = m_hong_bao_arr[_sys_type].begin();
	if (m_hong_bao_arr[_sys_type].end() != it_server)
	{
		HongBaoMapIt it_begin = it_server->second.begin();
		HongBaoMapIt it_end = it_server->second.end();

		int count = 0;
		for (; it_begin != it_end && count < MAX_ONCE_HONG_BAO_COUNT; ++it_begin)
		{
			sync_info.item_list[count++].hongbap_item = it_begin->second;

			if (count >= MAX_ONCE_HONG_BAO_COUNT)
			{
				sync_info.count = count;

				int send_length = sizeof(crossgameprotocal::GameHiddenSyncHongBaoInfo) -
					((MAX_ONCE_HONG_BAO_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

#ifdef _DEBUG
				gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::SyncAllDataToHidden _sys_type[%d] length[%d]",
					_sys_type, send_length);
#endif

				InternalComm::Instance().SendToCross((const char *)&sync_info, send_length);
				DataSyncCheck::Instance().AddCheck(crossgameprotocal::DATA_SYNC_CHECK_TYPE_HONG_BAO);
				count = 0;
				sync_info.count = 0;
				sync_info.is_init_add = 1;
			}
		}

		if (count > 0)
		{
			sync_info.count = count;
		}
	}

	int send_length = sizeof(crossgameprotocal::GameHiddenSyncHongBaoInfo) -
		((MAX_ONCE_HONG_BAO_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

#ifdef _DEBUG
	gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::SyncAllDataToHidden _sys_type[%d] length[%d]",
		_sys_type, send_length);
#endif

	InternalComm::Instance().SendToCross((const char *)&sync_info, send_length);
	DataSyncCheck::Instance().AddCheck(crossgameprotocal::DATA_SYNC_CHECK_TYPE_HONG_BAO);
}

void HongBaoManager::SyncDataToHidden(int _sys_type, int _sync_status, const HongBaoItem & _hongbao_item, int _index /*= 0*/)
{
	if (_sys_type < 0 || _sys_type >= HONGBAO_SYS_TYPE_MAX || CrossConfig::Instance().IsHiddenServer()
		|| _sync_status <= crossgameprotocal::SYNC_HONG_BAO_TO_CROSS_STATUS_INVALID ||
		_sync_status >= crossgameprotocal::SYNC_HONG_BAO_TO_CROSS_STATUS_MAX)
	{
		return;
	}

	static crossgameprotocal::GameHiddenSyncHongBaoInfo sync_info;
	sync_info.count = 1;
	sync_info.from_usid.plat_type = LocalConfig::Instance().GetPlatType();
	sync_info.from_usid.server_id = LocalConfig::Instance().GetMergeServerId();
	sync_info.sys_type = _sys_type;
	sync_info.is_init = false;
	sync_info.is_init_add = 0;
	sync_info.reserve_sh = 0;

	sync_info.item_list[0].hongbap_item = _hongbao_item;
	sync_info.item_list[0].sync_type = _sync_status;
	sync_info.item_list[0].index = _index;

	int send_length = sizeof(crossgameprotocal::GameHiddenSyncHongBaoInfo) -
		((CROSS_PERSON_RANK_SYNC_PAGE_ELEMENT_COUNT - sync_info.count) * sizeof(sync_info.item_list[0]));

#ifdef _DEBUG
	gamelog::g_log_hong_bao.printf(LL_INFO, "HongBaoManager::SyncDataToHidden sys_type[%d] length[%d]",
		_sys_type, send_length);
#endif

	InternalComm::Instance().SendToCross((const char *)&sync_info, send_length);
}

void HongBaoManager::SyncReqGiveRetToHidden(int _uid, int _ret, int _hongbao_id /*= 0 */, int _hongbao_sys_type /*= -1 */, int _hongbao_type /*= -1 */, char * _greeting /*= NULL */)
{
	if (_hongbao_sys_type < 0 || _hongbao_sys_type >= HONGBAO_SYS_TYPE_MAX ||
		_hongbao_type < HONGBAO_TYPE_INVALID || _hongbao_type >= HONGBAO_TYPE_MAX)
		return;

	// ���ͷ�����������֪ͨ��������
	static crossgameprotocal::GameHiddenSyncHongBaoReqGiveRet ret;

	UniqueServerID us_id(LocalConfig::Instance().GetPlatType(), LocalConfig::Instance().GetMergeServerId());

	ret.from_usid = us_id;
	ret.req_uid = _uid;
	ret.ret = _ret;

	ret.hongbao_id = _hongbao_id;
	ret.sys_type = _hongbao_sys_type;
	ret.type = _hongbao_type;

	if (NULL == _greeting)
		ret.greeting[0] = '\0';
	else
		memcpy(ret.greeting, _greeting, sizeof(HongBaoGreeting));

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&ret, sizeof(ret));
}

void HongBaoManager::SyncReqGetRetToHidden(int _uid, int _ret, int _hongbao_id, int _hongbao_sys_type, int _val)
{
	if (_hongbao_sys_type < 0 || _hongbao_sys_type >= HONGBAO_SYS_TYPE_MAX)
		return;

	// �����������֪ͨ��������
	static crossgameprotocal::GameHiddenSyncHongBaoReqGetRet ret;

	UniqueServerID us_id(LocalConfig::Instance().GetPlatType(), LocalConfig::Instance().GetMergeServerId());

	ret.from_usid = us_id;
	ret.req_uid = _uid;
	ret.ret = _ret;

	ret.hongbao_id = _hongbao_id;
	ret.sys_type = _hongbao_sys_type;
	ret.val = _val;

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&ret, sizeof(ret));
}

void HongBaoManager::InitHongBaoInfoFromGame(crossgameprotocal::GameHiddenSyncHongBaoInfo * _hongbao_info)
{
	// 1. ��ʼ�����������Ϣ
	std::pair<HongBaoServerMapIt, bool> temp_pair;
	HongBaoServerMapIt it_server_temp = m_hong_bao_arr[_hongbao_info->sys_type].end();
	HongBaoServerMapIt it_server = m_hong_bao_arr[_hongbao_info->sys_type].find(_hongbao_info->from_usid);
	if (m_hong_bao_arr[_hongbao_info->sys_type].end() == it_server)
	{
		HongBaoMap temp_map;
		temp_pair = m_hong_bao_arr[_hongbao_info->sys_type].insert(std::make_pair(_hongbao_info->from_usid, temp_map));

		if (temp_pair.second)
		{
			it_server_temp = temp_pair.first;
		}
	}
	else
	{
		it_server_temp = it_server;
	}

	if (m_hong_bao_arr[_hongbao_info->sys_type].end() == it_server_temp)
	{
		// ��ʼ��ʧ��
		gamelog::g_log_hong_bao.printf(LL_INFO, "%s[ERROR_1]sys_type:%d  plat:%d  server_id:%d",
			__FUNCTION__, _hongbao_info->sys_type, _hongbao_info->from_usid.plat_type, _hongbao_info->from_usid.server_id);
		return;
	}

	// 2 . ��ʼ��������������Ϣ
	std::pair<HongBaoGuildServerMapIt, bool> temp_guild_pair;
	HongBaoGuildServerMapIt it_guild_server_temp = m_hong_bao_guild_map.end();
	HongBaoGuildServerMapIt it_guild_server = m_hong_bao_guild_map.find(_hongbao_info->from_usid);
	if (m_hong_bao_guild_map.end() == it_guild_server)
	{
		HongBaoGuildMap temp_map;
		temp_guild_pair = m_hong_bao_guild_map.insert(std::make_pair(_hongbao_info->from_usid, temp_map));

		if (temp_guild_pair.second)
		{
			it_guild_server_temp = temp_guild_pair.first;
		}
	}
	else
	{
		it_guild_server_temp = it_guild_server;
	}

	if (m_hong_bao_guild_map.end() == it_guild_server_temp)
	{
		// ��ʼ��ʧ��
		gamelog::g_log_hong_bao.printf(LL_INFO, "%s[ERROR_2] sys_type:%d  plat:%d  server_id:%d",
			__FUNCTION__, _hongbao_info->sys_type, _hongbao_info->from_usid.plat_type, _hongbao_info->from_usid.server_id);
		return;
	}

	// ��ʼ��������
	if (!_hongbao_info->is_init_add)
	{
		// ȫ����
		it_server_temp->second.clear();

		if (HONGBAO_SYS_TYPE_GUILD == _hongbao_info->sys_type)
			it_guild_server_temp->second.clear();
	}

	for (int i = 0; i < _hongbao_info->count && i < MAX_ONCE_HONG_BAO_COUNT; ++i)
	{
		HongBaoItem temp_item;
		temp_item = _hongbao_info->item_list[i].hongbap_item;
		this->InitUIDSet(temp_item);

		// 1. ���������
		it_server_temp->second.insert(std::make_pair(_hongbao_info->item_list[i].hongbap_item.hong_bao_id, temp_item));

		// 2. ������������
		if (HONGBAO_SYS_TYPE_GUILD == _hongbao_info->sys_type)
		{
			this->AddHongBaoIDToGuildMap(_hongbao_info->item_list[i].hongbap_item.info_data.guild_id,
				_hongbao_info->item_list[i].hongbap_item.hong_bao_id,
				_hongbao_info->from_usid);
		}
	}
}

void HongBaoManager::UpdataHongBaoInfoFromGame(crossgameprotocal::GameHiddenSyncHongBaoInfo * _hongbao_info)
{
	// 1. ��ȡ���������Ϣ
	HongBaoServerMapIt it_server = m_hong_bao_arr[_hongbao_info->sys_type].find(_hongbao_info->from_usid);
	if (m_hong_bao_arr[_hongbao_info->sys_type].end() == it_server)
	{
		gamelog::g_log_hong_bao.printf(LL_INFO, "%s[ERROR_1]sys_type:%d  plat:%d  server_id:%d",
			__FUNCTION__, _hongbao_info->sys_type, _hongbao_info->from_usid.plat_type, _hongbao_info->from_usid.server_id);
		return;
	}

	// 2 . ��ȡ������������Ϣ
	HongBaoGuildServerMapIt it_guild_server = m_hong_bao_guild_map.find(_hongbao_info->from_usid);
	if (m_hong_bao_guild_map.end() == it_guild_server)
	{
		// ��ʼ��ʧ��
		gamelog::g_log_hong_bao.printf(LL_INFO, "%s[ERROR_2] sys_type:%d  plat:%d  server_id:%d",
			__FUNCTION__, _hongbao_info->sys_type, _hongbao_info->from_usid.plat_type, _hongbao_info->from_usid.server_id);
		return;
	}

	for (int i = 0; i < _hongbao_info->count; ++i)
	{
		switch (_hongbao_info->item_list[i].sync_type)
		{
		case crossgameprotocal::SYNC_HONG_BAO_TO_CROSS_STATUS_INSERT:
			{
				// 1. �������
				HongBaoItem temp_item;
				temp_item = _hongbao_info->item_list[i].hongbap_item;
				this->InitUIDSet(temp_item);

				it_server->second.insert(std::make_pair(_hongbao_info->item_list[i].hongbap_item.hong_bao_id,
					temp_item));

				// 2. ����������
				if (HONGBAO_SYS_TYPE_GUILD == _hongbao_info->sys_type)
				{
					this->AddHongBaoIDToGuildMap(_hongbao_info->item_list[i].hongbap_item.info_data.guild_id,
						_hongbao_info->item_list[i].hongbap_item.hong_bao_id,
						_hongbao_info->from_usid);
				}

				// 3 .�㲥��Ϣ
				this->SendHongBaoListChange(_hongbao_info->sys_type, temp_item,
					Protocol::SCHongBaoBaseInfoChange::CHANGE_TYPE_ADD);
			}
			break;
		case crossgameprotocal::SYNC_HONG_BAO_TO_CROSS_STATUS_UPDATA:
			{
				// 1. �������
				HongBaoMapIt it = it_server->second.find(_hongbao_info->item_list[i].hongbap_item.hong_bao_id);
				if (it_server->second.end() != it)
				{
					int old_count = it->second.hongbao_reward_info.get_count;
					int new_count = _hongbao_info->item_list[i].hongbap_item.hongbao_reward_info.get_count;
					it->second = _hongbao_info->item_list[i].hongbap_item;

					for (int index = old_count; index < new_count; ++index)
					{
						it->second.get_uid_set.insert(_hongbao_info->item_list[i].hongbap_item.hongbao_reward_info.get_info[index].uid);
					}

					// 2 .�㲥��Ϣ
					this->SendHongBaoBaseInfoChange(NULL, it->second,
						Protocol::SCHongBaoBaseInfoChange::CHANGE_TYPE_ADD, _hongbao_info->item_list[i].index);
				}
			}
			break;
		case crossgameprotocal::SYNC_HONG_BAO_TO_CROSS_STATUS_DELETE:
			{
				// 1. �������
				HongBaoMapIt it = it_server->second.find(_hongbao_info->item_list[i].hongbap_item.hong_bao_id);
				if (it_server->second.end() != it)
				{
					// 2 .�㲥��Ϣ
					this->SendHongBaoListChange(_hongbao_info->sys_type, it->second,
						Protocol::SCHongBaoBaseInfoChange::CHANGE_TYPE_DEL);

					// 3. ɾ���������
					it_server->second.erase(_hongbao_info->item_list[i].hongbap_item.hong_bao_id);

					// 2 .ɾ������������
					if (HONGBAO_SYS_TYPE_GUILD == _hongbao_info->sys_type)
					{
						this->RemoveHongBaoIDToGuildMap(_hongbao_info->item_list[i].hongbap_item.info_data.guild_id,
							_hongbao_info->item_list[i].hongbap_item.hong_bao_id,
							_hongbao_info->from_usid);
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

void HongBaoManager::SendHongBaoList(Role * _role, int _hongbao_sys_type)
{
	if (NULL == _role) return;

	switch (_hongbao_sys_type)
	{
	case HONGBAO_SYS_TYPE_GUILD:
		{
			this->SendHongBaoListByGuildHongBao(_role);
		}
		break;
	default:
		break;
	}
}

void HongBaoManager::SendHongBaoListByGuildHongBao(Role * _role)
{
	if (NULL == _role)
	{
		return;
	}

	GuildID guild_id = _role->GetGuildID();
	if (INVALID_GUILD_ID == guild_id)
	{
		_role->NoticeNum(errornum::EN_GUILD_NOT_IN_GUILD);
		return;
	}

	static const int MAX_HONGBAO_BUFF_LENGTH = 8192;
	static char buff[sizeof(Protocol::SCHongBaoGetList) + MAX_HONGBAO_BUFF_LENGTH + 1] = { 0 };
	memset(buff, 0, sizeof(buff));

	Protocol::SCHongBaoGetList info;
	info.hongbao_sys_type = HONGBAO_SYS_TYPE_GUILD;
	info.count = 0;

	int original_plat = _role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType();
	int original_server_id = _role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId();
	UniqueServerID original_server(original_plat, original_server_id);

	HongBaoGuildServerMapIt it_server = m_hong_bao_guild_map.find(original_server);
	if (m_hong_bao_guild_map.end() == it_server)
	{
		return;
	}

	HongBaoGuildMapIt it = it_server->second.find(guild_id);
	if (it_server->second.end() == it)
	{
		EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char *)&info, sizeof(info));
	}
	else
	{
		std::set<int >::iterator it_begin = it->second.begin();
		std::set<int >::iterator it_end = it->second.end();

		static Protocol::SCHongBaoGetList::GetInfo get_info;
		int one_get_info_size = sizeof(get_info);
		memset(&get_info, 0, one_get_info_size);

		HongBaoServerMapIt it_server_hongbao = m_hong_bao_arr[HONGBAO_SYS_TYPE_GUILD].find(original_server);
		if (m_hong_bao_arr[HONGBAO_SYS_TYPE_GUILD].end() != it_server_hongbao)
		{
			for (; it_begin != it_end; ++it_begin)
			{
				HongBaoMapIt it_hongbao = it_server_hongbao->second.find(*it_begin);
				if (it_server_hongbao->second.end() != it_hongbao)
				{
					// �Ѿ���ȡ���˵��򲻷���
					if (it_hongbao->second.get_uid_set.end() != it_hongbao->second.get_uid_set.find(_role->GetUID()))
						continue;

					get_info.hongbao_id = *it_begin;
					get_info.send_avatar_type = it_hongbao->second.send_avatar_type;
					get_info.send_headshot_id = it_hongbao->second.send_headshot_id;
					F_STRNCPY(get_info.send_name, it_hongbao->second.send_name, sizeof(GameName));
					memcpy(buff + sizeof(Protocol::SCHongBaoGetList) + info.count * one_get_info_size, &get_info, one_get_info_size);
					++info.count;
				}
			}
		}

		memcpy(buff, &info, sizeof(Protocol::SCHongBaoGetList));
		int length = sizeof(Protocol::SCHongBaoGetList) + info.count * one_get_info_size;
		EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char *)buff, length);
	}
}

void HongBaoManager::SendHongBaoListChange(int _hongbao_sys_type, const HongBaoItem & _hongbao_item, int _change_type)
{
	Protocol::SCHongBaoGetListChange change;

	change.change_type = _change_type;
	change.hongbao_sys_type = _hongbao_sys_type;
	change.info.hongbao_id = _hongbao_item.hong_bao_id;
	change.info.send_avatar_type = _hongbao_item.send_avatar_type;
	change.info.send_headshot_id = _hongbao_item.send_headshot_id;
	F_STRNCPY(change.info.send_name, _hongbao_item.send_name, sizeof(GameName));

	switch (_hongbao_sys_type)
	{
	case HONGBAO_SYS_TYPE_GUILD:
		{
			this->SendHongBaoListChangeByGuildHongBao(_hongbao_item, change);
		}
		break;
	default:
		break;
	}
}

void HongBaoManager::SendHongBaoListChangeByGuildHongBao(const HongBaoItem & _hongbao_item, const Protocol::SCHongBaoGetListChange & change)
{
	Guild * guild = GuildManager::Instance().GetGuild(_hongbao_item.info_data.guild_id);
	if (NULL == guild)
	{
		return;
	}

	guild->GetMemberManagerRef().SendInfoToAllMember((const char *)&change, sizeof(change));
}

void HongBaoManager::SendHongBaoBaseInfo(Role * _role, int _hongbao_sys_type, int _hongbao_id)
{
	if (NULL == _role || _hongbao_sys_type < 0 || _hongbao_sys_type >= HONGBAO_SYS_TYPE_MAX)
		return;


	int original_plat = _role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType();
	int original_server_id = _role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId();
	UniqueServerID original_server(original_plat, original_server_id);

	HongBaoServerMapIt it_server = m_hong_bao_arr[_hongbao_sys_type].find(original_server);
	if (m_hong_bao_arr[_hongbao_sys_type].end() == it_server)
		return;

	HongBaoMapIt it = it_server->second.find(_hongbao_id);
	if (it_server->second.end() == it)
	{
		_role->NoticeNum(errornum::EN_TARGET_HONG_BAO_INEXISTENCE);
		return;
	}

	if (!this->CanGetHongBaoBaseInfo(_role, it->second))
	{
		return;
	}

	Protocol::SCHongBaoBaseInfo info;

	info.hong_bao_id = _hongbao_id;
	info.invalid_timestamp = it->second.invalid_timestamp;
	info.hongbao_type = it->second.hongbao_type;
	info.hongbao_sys_type = it->second.hongbao_sys_type;
	info.send_uid = it->second.send_uid;
	F_STRNCPY(info.send_name, it->second.send_name, sizeof(GameName));
	info.used_val = it->second.used_val;
	info.get_count = it->second.hongbao_reward_info.get_count;
	F_STRNCPY(info.greeting, it->second.greeting, sizeof(HongBaoGreeting));

	if (info.get_count > MAX_HONG_BAO_GET_MEMBER_COUNT)
	{
		// �������ǲ���� ,�������֤��Ҫ�Ƿ�ֹ����Խ��
		info.get_count = MAX_HONG_BAO_GET_MEMBER_COUNT;
	}

	UNSTD_STATIC_CHECK(sizeof(info.get_info[0]) == sizeof(it->second.hongbao_reward_info.get_info[0]));
	memcpy(info.get_info, it->second.hongbao_reward_info.get_info, sizeof(HongBaoItem::RewardInfo) * info.get_count);
	int length = sizeof(info) - (MAX_HONG_BAO_GET_MEMBER_COUNT - info.get_count) * sizeof(info.get_info[0]);

	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char *)&info, length);
}

bool HongBaoManager::CanGetHongBaoBaseInfo(Role * _role, const HongBaoItem & _hongbao_item)
{
	if (NULL == _role)
		return false;

	switch (_hongbao_item.hongbao_sys_type)
	{
	case HONGBAO_SYS_TYPE_GUILD:
		{
			if (INVALID_GUILD_ID == _role->GetGuildID() || _role->GetGuildID() != _hongbao_item.info_data.guild_id)
			{
				_role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return false;
			}
		}
		break;
	default:
		break;
	}

	return true;
}

void HongBaoManager::SendHongBaoBaseInfoChange(Role * _role, const HongBaoItem & _hongbao_item, int _change_type, int _change_index /*= 0*/)
{
	if (_change_index < 0 || _change_index >= MAX_HONG_BAO_GET_MEMBER_COUNT)
		return;

	Protocol::SCHongBaoBaseInfoChange change;

	change.change_type = _change_type;
	change.hong_bao_id = _hongbao_item.hong_bao_id;
	change.invalid_timestamp = _hongbao_item.invalid_timestamp;
	change.hongbao_type = _hongbao_item.hongbao_type;
	change.hongbao_sys_type = _hongbao_item.hongbao_sys_type;
	change.used_val = _hongbao_item.used_val;
	change.get_info = _hongbao_item.hongbao_reward_info.get_info[_change_index];

	switch (change.hongbao_sys_type)
	{
	case HONGBAO_SYS_TYPE_GUILD:
		{
			this->SendHongBaoBaseInfoChangeByGuildHongBao(_hongbao_item, change);
		}
		break;
	}
}

void HongBaoManager::SendHongBaoBaseInfoChangeByGuildHongBao(const HongBaoItem & _hongbao_item, const  Protocol::SCHongBaoBaseInfoChange & _change)
{
	Guild * guild = GuildManager::Instance().GetGuild(_hongbao_item.info_data.guild_id);
	if (NULL == guild)
	{
		return;
	}

	guild->GetMemberManagerRef().SendInfoToAllMember((const char *)&_change, sizeof(_change));
}

