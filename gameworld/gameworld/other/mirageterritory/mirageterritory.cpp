#include "mirageterritory.hpp"
#include "other/funopen/funopen.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/mirageterritory/mirageterritoryconfig.hpp"
#include "protocol/msgother.h"
#include "servercommon/errornum.h"
#include "battle/battle_business_local.hpp"
#include "gameworld/battle/battle_manager_local.hpp"
#include "item/knapsack.h"
#include "gameworld/engineadapter.h"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/partner/partner.hpp"
#include "other/partner/partnerconfig.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "other/event/eventhandler.hpp"
#include "servercommon/protobuf/role_mirageterritory_data.pb.h"
#include "servercommon/protobufmanager.hpp"

MirageTerritory::MirageTerritory()
{
}

MirageTerritory::~MirageTerritory()
{
}

void MirageTerritory::Init(RoleModuleManager * module_mgr, const MirageTerritoryParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;
}

void MirageTerritory::GetInitParam(MirageTerritoryParam * param)
{
	*param = m_param;
}

void MirageTerritory::GetInfoParam(RoleMirageTerritoryPartnerInfoParam * param)
{
	PB_RoleMirageTerritoryInfoData_MirageTerritoryAllInfo* pb_data = (PB_RoleMirageTerritoryInfoData_MirageTerritoryAllInfo*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_ROLE_MIRAGETERRITORY_INFO_DATA);
	if (NULL == pb_data)
		return;

	pb_data->mutable_partner_list()->Clear();
	MirageTerritoryInfoParam role_info_data;
	role_info_data.Reset();
	role_info_data.today_pass_times = m_param.today_pass_times;
	role_info_data.pass_seq = m_param.pass_seq;
	role_info_data.pass_area = m_param.pass_area;
	for (int i = 0; i < MAX_MIRAGE_TERRITORY_PROGRAMME_NUM; i++)
	{
		role_info_data.partner_list[i].now_seq = m_param.partner_list[i].now_seq;
		role_info_data.partner_list[i].top_seq = m_param.partner_list[i].top_seq;
	}

	if (!pb_data->SerializeToArray(param->mirage_territory_data, ARRAY_LENGTH(param->mirage_territory_data)))
	{
#ifdef _DEBUG
		assert(0);
#endif
		return;
	}
	
}

void MirageTerritory::OnBattleFinishWin(BattleData_Business * business_data, int round_num)
{
	if (NULL != business_data)
	{
		const MirageTerritoryChalleCfg * challenge_cfg = LOGIC_CONFIG->GetMirageTerritoryConfig()->GetChallengeCfg(business_data->param2, business_data->param3);
		if (NULL == challenge_cfg)return;
		
		const MirageTerritoryRewardCfg * reward_cfg = LOGIC_CONFIG->GetMirageTerritoryConfig()->GetRewardCfg(challenge_cfg->reward_id);
		if (NULL != reward_cfg)
		{
			std::vector<ItemConfigData> real_item_vec;
			real_item_vec.clear();

			if ((int)reward_cfg->reward_vec.size() > 0)
			{
				real_item_vec.insert(real_item_vec.end(), reward_cfg->reward_vec.begin(), reward_cfg->reward_vec.end());
				if (!m_module_mgr->GetKnapsack()->PutList((short)real_item_vec.size(), &real_item_vec[0], PUT_REASON_MIRAGE_TERRITORY, false))
				{
					//发邮件
					m_module_mgr->GetKnapsack()->SendMail(&real_item_vec[0], (int)real_item_vec.size(), SNED_MAIL_TYPE_DEFAULT, true);
				}
				m_module_mgr->GetKnapsack()->NotifyGetItemList((short)real_item_vec.size(), &real_item_vec[0], GET_REASON_MIRAGE_TERRITORY);
			}
		}
		if (business_data->param2 >= 0 && business_data->param2 < MAX_MIRAGE_TERRITORY_PROGRAMME_NUM)
		{
			MirageTerritoryPartnerBattleInfo &node = m_param.partner_list[business_data->param2];
			node.now_seq = business_data->param3;
			if (node.now_seq > node.top_seq)
			{
				node.top_seq = node.now_seq;
			}
			EventHandler::Instance().OnPassMirageTerritory(m_module_mgr->GetRole(), business_data->param2, node.now_seq);
		}
		
		this->SendInfo(business_data->param2);
	}
}

void MirageTerritory::OnRoleLogin()
{
	this->SendAllInfo();
}

void MirageTerritory::OnDayChange()
{
	m_param.OnDayChange();
	this->SendAllInfo();
}

void MirageTerritory::OnOpreat(int req_type, int param1, int param2, int param3, int param4)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_MIRAGE_TERRITORY, true))return;
	
	switch (req_type)
	{
	case Protocol::CSMirageTerritoryReq::REQ_TYPE_INFO:
		this->SendInfo(param1);
		break;
	case Protocol::CSMirageTerritoryReq::REQ_TYPE_BATTLE:
		this->StarBattle(param1);
		break;
	case Protocol::CSMirageTerritoryReq::REQ_TYPE_ONE_KEY_PASS:
		this->OneKeyPass(param1, param2, param3, param4);
		break;
	case Protocol::CSMirageTerritoryReq::REQ_TYPE_BATTLE_ZHEN_INFO:
		this->SendPartnerBattleInfo(param1);
		break;
	default:
		break;
	}
}

void MirageTerritory::StarBattle(int area)
{
	//策划要求去掉飞升限制
	/*if (m_module_mgr->GetFlyUpSystem()->GetFlyUpFlag() <= FlyUpSystemParam::FLY_UP_STAG_NO)
	{
		m_module_mgr->NoticeNum(errornum::EN_NEED_ROLE_FLY_UP);
		return;
	}*/
	if (area < 0 || area >= MAX_MIRAGE_TERRITORY_PROGRAMME_NUM)return;
	MirageTerritoryPartnerBattleInfo &node = m_param.partner_list[area];
	//获取配置
	const MirageTerritoryChalleCfg * challenge_cfg = LOGIC_CONFIG->GetMirageTerritoryConfig()->GetChallengeCfg(area, node.now_seq + 1);
	if (NULL == challenge_cfg)return;
	const MirageTerritoryAreaCfg * area_cfg = LOGIC_CONFIG->GetMirageTerritoryConfig()->GetAreaCfg(challenge_cfg->area);
	if (NULL == area_cfg)return;
	
	std::vector<PartnerBattleData> ack_partner_battle_data;		//调用战斗函数所需要用到的数据

	static int fight_partner_list[MAX_MIRAGE_TERRITORY_PARTNER_NUM] = {0};
	static int fight_posi_list[MAX_MIRAGE_TERRITORY_PARTNER_NUM] = { 0 };

	for (int i = 0; i < MAX_MIRAGE_TERRITORY_PARTNER_NUM; ++i)
	{
		const PartnerBasicCfg * base_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(node.partner_id_list[i]);
		if (NULL == base_cfg)continue;
		if (base_cfg->profession != area_cfg->job1 && base_cfg->profession != area_cfg->job2)return;
		PartnerBattleData partner_data; partner_data.Reset();
		if (m_module_mgr->GetPartner()->GetBattleCharacterData(node.partner_id_list[i], &partner_data, &fight_partner_list[0], MAX_MIRAGE_TERRITORY_PARTNER_NUM, false))
		{
			UNSTD_STATIC_CHECK(ARRAY_LENGTH(BATTLE_POS_RELATION) == SIDE_MAX_ROLE_NUM);
			partner_data.battle_column = node.partner_posi_list[i];
			if((int)ack_partner_battle_data.size() >= 0 && (int)ack_partner_battle_data.size() < MAX_MIRAGE_TERRITORY_PARTNER_NUM)fight_posi_list[(int)ack_partner_battle_data.size()] = node.partner_posi_list[i];
			ack_partner_battle_data.push_back(partner_data);
		}
	}

	if ((int)ack_partner_battle_data.size() <= 0)return;

	BattleData_Business role_data; role_data.Reset();
	role_data.monster_group_id = challenge_cfg->monster_group;
	role_data.param2 = area;
	role_data.param3 = node.now_seq + 1;
	role_data.limit_max_round = MAX_MIRAGE_TERRITORY_BATTLE_ROUND_NUM;
	
	if (!BattleManagerLocal::GetInstance().StartPartnerPVEReq(m_module_mgr->GetRole(), (int)ack_partner_battle_data.size(), &ack_partner_battle_data[0], &fight_posi_list[0], challenge_cfg->monster_group, role_data))
	{
		return;
	}
}

int MirageTerritory::GetNowBattleSeq(int area)
{
	if (area < 0 || area >= MAX_MIRAGE_TERRITORY_PROGRAMME_NUM)return 0;
	MirageTerritoryPartnerBattleInfo &node = m_param.partner_list[area];
	return node.now_seq;
}

int MirageTerritory::GetTopBattleSeq(int area)
{
	if (area < 0 || area >= MAX_MIRAGE_TERRITORY_PROGRAMME_NUM)return 0;
	MirageTerritoryPartnerBattleInfo &node = m_param.partner_list[area];
	return node.top_seq;
}

int MirageTerritory::GetMinAllTopBattleSeq()
{	
	int top_seq = MAX_INT;
	for (int i = 0; i < ARRAY_LENGTH(m_param.partner_list); ++i)
	{
		top_seq = GetMin(top_seq, m_param.partner_list[i].top_seq);
	}

	return top_seq;
}

void MirageTerritory::OneKeyPass(int area, int partner_id_1, int partner_id_2, int partner_id_3)
{
	if (area < 0 || area >= MAX_MIRAGE_TERRITORY_PROGRAMME_NUM)return;
	MirageTerritoryPartnerBattleInfo &node = m_param.partner_list[area];

	//检查派遣
	const MirageTerritoryOtherCfg & other_cfg = LOGIC_CONFIG->GetMirageTerritoryConfig()->GetOtherCfg();
	int real_partner_num = 0;
	int partner_list[MAX_MIRAGE_TERRITORY_PARTNER_NUM] = { 0,0,0 };
	if (partner_id_1 > 0 && m_module_mgr->GetPartner()->IsPartnerAlreadyActive(partner_id_1))partner_list[real_partner_num++] = partner_id_1;
	if (partner_id_2 > 0 && m_module_mgr->GetPartner()->IsPartnerAlreadyActive(partner_id_2))partner_list[real_partner_num++] = partner_id_2;
	if (partner_id_3 > 0 && m_module_mgr->GetPartner()->IsPartnerAlreadyActive(partner_id_3))partner_list[real_partner_num++] = partner_id_3;
	
	const MirageTerritoryAreaCfg * area_cfg = LOGIC_CONFIG->GetMirageTerritoryConfig()->GetAreaCfg(area);
	if (NULL == area_cfg)return;

	for (int i = 0; i < real_partner_num && i < MAX_MIRAGE_TERRITORY_PARTNER_NUM; ++i)//职业检查
	{
		const PartnerBasicCfg * base_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_list[i]);
		if (NULL == base_cfg)continue;
		if (base_cfg->profession != area_cfg->job1 && base_cfg->profession != area_cfg->job2)return;
	}

	if (other_cfg.dispatch_num < real_partner_num)
	{
		return;//超出派遣人数
	}
	const MirageTerritoryChalleCfg * challenge_cfg = LOGIC_CONFIG->GetMirageTerritoryConfig()->GetChallengeCfg(area, node.top_seq);
	if (NULL == challenge_cfg)return;

	const MirageTerritoryRewardCfg * reward_cfg = LOGIC_CONFIG->GetMirageTerritoryConfig()->GetRewardCfg(challenge_cfg->reward_id);
	if (NULL == reward_cfg)return;
	std::vector<ItemConfigData> real_item_vec;
	real_item_vec.clear();

	if ((int)reward_cfg->pass_reward_vec.size() > 0)
	{
		real_item_vec.insert(real_item_vec.end(), reward_cfg->pass_reward_vec.begin(), reward_cfg->pass_reward_vec.end());
	}

	for (int i = 0; i < real_partner_num && i < MAX_MIRAGE_TERRITORY_PARTNER_NUM; i++)
	{
		const PartnerArmChipCfg * partner_arm_cfg = LOGIC_CONFIG->GetMirageTerritoryConfig()->GetParnterRewardCfg(partner_list[i]);
		if (NULL != partner_arm_cfg)
		{
			ItemConfigData node;
			node.item_id = partner_arm_cfg->arms_id;
			node.num = reward_cfg->arm_chip_num;
			node.is_bind = 1;
			real_item_vec.push_back(node);
		}
	}

	if ((int)real_item_vec.size() > 0 && !m_module_mgr->GetKnapsack()->PutList((short)real_item_vec.size(), &real_item_vec[0], PUT_REASON_MIRAGE_TERRITORY))
	{
		//发邮件
		m_module_mgr->GetKnapsack()->SendMail(&real_item_vec[0], (int)real_item_vec.size(), SNED_MAIL_TYPE_DEFAULT, true);
	}
	m_module_mgr->GetKnapsack()->NotifyGetItemList((short)real_item_vec.size(), &real_item_vec[0], GET_REASON_MIRAGE_TERRITORY);
	m_param.today_pass_times += 1;
	m_param.pass_area = area;
	m_param.pass_seq = node.top_seq;
	this->SendInfo(area);
}

void MirageTerritory::SendInfo(int area)
{
	if (area < 0 || area >= MAX_MIRAGE_TERRITORY_PROGRAMME_NUM)return;
	MirageTerritoryPartnerBattleInfo &node = m_param.partner_list[area];
	Protocol::SCMirageTerritoryInfo info;
	info.today_pass_times = m_param.today_pass_times;
	info.pass_area = m_param.pass_area;
	info.pass_seq = m_param.pass_seq;
	info.area = area;
	info.now_seq = node.now_seq;
	info.top_seq = node.top_seq;
	m_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void MirageTerritory::SendAllInfo()
{
	for (int i = 0; i < MAX_MIRAGE_TERRITORY_PROGRAMME_NUM; i++)
	{
		this->SendInfo(i);
	}
}

void MirageTerritory::SetPartnerBattleInfo(int programme_idx, int partner_id_list[MAX_MIRAGE_TERRITORY_PARTNER_NUM], int partner_posi_list[MAX_MIRAGE_TERRITORY_PARTNER_NUM])
{
	if (programme_idx < 0 || programme_idx >= MAX_MIRAGE_TERRITORY_PROGRAMME_NUM)return;
	MirageTerritoryPartnerBattleInfo &node = m_param.partner_list[programme_idx];
	node.partner_num = 0;
	for (int i = 0; i < MAX_MIRAGE_TERRITORY_PARTNER_NUM; i++)
	{
		node.partner_id_list[i] = partner_id_list[i];
		node.partner_posi_list[i] = partner_posi_list[i];
		if (partner_id_list[i] != 0)
		{
			node.partner_num += 1;
		}
	}
	this->SendPartnerBattleInfo(programme_idx);
}

void MirageTerritory::SendPartnerBattleInfo(int programme_idx)
{
	Protocol::SCMirageTerritorySetPartnerInfo info;
	info.programme_idx = programme_idx;
	MirageTerritoryPartnerBattleInfo &node = m_param.partner_list[programme_idx];
	for (int i = 0; i < MAX_MIRAGE_TERRITORY_PARTNER_NUM; i++)
	{
		info.partner_id_list[i] = node.partner_id_list[i];
		info.partner_posi_list[i] = node.partner_posi_list[i];
	}
	m_module_mgr->NetSend((const char*)&info, sizeof(info));
}
