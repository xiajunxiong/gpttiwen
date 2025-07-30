#include "xunyoucar.hpp"
#include "config/logicconfigmanager.hpp"
#include "qingyuan/qingyuanconfig.hpp"
#include "obj/character/npc_gather.h"
#include "npc/npcmanager.h"
#include "engineadapter.h"
#include "scene/scene.h"
#include "gamelog.h"
#include "gamecommon.h"

#include "item/knapsack.h"
#include "other/route/mailroute.hpp"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "protocol/msgqingyuan.h"
#include "global/qingyuan/qingyuanmanager.h"

XunYouCar::XunYouCar() : Character(OBJ_TYPE_XUN_YOU_CAR)
{
	m_qing_yuan_id = 0;
	m_xun_you_type = 0;
	m_now_posi_index = 0;
	m_wait_to_move_time = 0;
}

XunYouCar::~XunYouCar()
{
}

void XunYouCar::Init(long long qing_yuan_id, int xun_you_type)
{
	m_xun_you_type = xun_you_type;
	m_qing_yuan_id = qing_yuan_id;
	m_now_posi_index = 0;
	m_wait_to_move_time = 0;

	const WeddingFB::XunYouCfg * xun_you_cfg
		= LOGIC_CONFIG->GetQingYuanConfig()->GeXunYouCfg(xun_you_type);
	if (NULL == xun_you_cfg)
	{
		this->SetSpeedRate(0);
	}
	else
	{//��д�� ������ûЧ�� 0����
		this->SetSpeedRate(0);
	}
}

void XunYouCar::Update(unsigned long interval, time_t now_second, unsigned int now_dayid)
{
	if (NULL == m_scene) return;

	Character::Update(interval, now_second, now_dayid);

	switch (this->GetActionStatus())
	{
	//δ�ƶ� / �ִ�
	case Character::ACTION_IDLE:
		{
			//�ȴ�
			if (m_wait_to_move_time > now_second)
			{
				return;
			}

			const WeddingFB::XunYouWayPoint * way_point_cfg
				= LOGIC_CONFIG->GetQingYuanConfig()->GeXunYouWayPoint(m_now_posi_index);
			if (NULL == way_point_cfg)
			{//����Ч/����  ɾ��
				QingYuan * p_qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(this->GetQingYuanID());
				if (NULL != p_qing_yuan)
				{
					std::vector<int> host_vec = p_qing_yuan->GetMemberList();
					for (int i = 0; i < (int)host_vec.size(); i++)
					{
						Role *host_role = m_scene->GetRoleByUID(host_vec[i]);
						if (NULL != host_role)
						{
							//����Ѳ��״̬
							host_role->GetRoleStatusManager()->SetXunYouStatus(0);
						}
					}
				}
				this->GetScene()->DeleteObj(m_obj_id);
				return;
			}

			Posi target_pos = way_point_cfg->way_poi;
			Posi now_pos = this->GetPos();
			Posi deltaP = target_pos - now_pos;

			//У������
			if (!deltaP.IsLongerThan(2))
			{//����յ�
				//�ȴ�ʱ��
				m_wait_to_move_time = now_second + way_point_cfg->stop_wait_time;

				this->CreatGift(m_now_posi_index);

				//��һ����
				m_now_posi_index++;

				//
				m_is_stop = 1;
				this->SyncCarStatusInfo();

				//�ȴ�
				return;
			}

			//�����ƶ�
			this->SetMoveLogic(target_pos);
			if (1 == m_is_stop)
			{
				m_is_stop = 0;
				this->SyncCarStatusInfo();
			}
		}
		break;
	//�ƶ���
	case Character::ACTION_RUN:
		{

		}
		break;
	default:
		return;
	}

	//����
	QingYuan * p_qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(this->GetQingYuanID());
	if (NULL != p_qing_yuan)
	{
		std::vector<int> host_vec = p_qing_yuan->GetMemberList();
		for (int i = 0; i < (int)host_vec.size(); i++)
		{
			//todo ����
			Role *host_role = m_scene->GetRoleByUID(host_vec[i]);
			if (NULL != host_role)
			{
				//Scalar _distance = 0.0f;
				//Posf _deltaP_f = host_role->GetPosf() - this->GetPosf();
				//Dir _dir = gamecommon::GetMoveDirFloat(_deltaP_f, &_distance);
				//
				//if (30 > _distance)
				//{
				//	host_role->Move(this->GetPos().x, this->GetPos().y, _dir, 1, 0, false);
				//}
				host_role->ResetPos(this->GetPos().x, this->GetPos().y);
				//����Ѳ��״̬
				host_role->GetRoleStatusManager()->SetXunYouStatus(1);
			}
		}
	}
}

void XunYouCar::SyncCarStatusInfo()
{
	Protocol::SCXunYouCarStatusInfoSync info;
	info.obj_id = this->GetId();
	info.is_stop = m_is_stop;
	this->GetScene()->GetZoneMatrix()->NotifyAreaMsg(this->GetObserHandle(), &info, sizeof(info));
}

void XunYouCar::OnGatherGift(Role * gather, NPCGather * npc_gather)
{
	if (NULL == gather)
	{
		return;
	}

	if (NULL == npc_gather)
	{
		return;
	}
	int xun_you_type = 0;

	npc_gather->GetParam(NULL, &xun_you_type, NULL);

	const WeddingFB::XunYouCfg * xun_you_cfg
		= LOGIC_CONFIG->GetQingYuanConfig()->GeXunYouCfg(xun_you_type);
	if (NULL == xun_you_cfg)
	{
		return;
	}

	//����
	long long reward_coin_bind = 0;
	std::vector<ItemConfigData> temp_reward_vec;
	int group_id_size = (int)xun_you_cfg->gift_group_id_list.size();
	for (int i = 0; i < group_id_size; i++)
	{
		const WeddingFB::GroupItemCfg * group_cfg
			= LOGIC_CONFIG->GetQingYuanConfig()->GetGroupRandCfg(xun_you_cfg->gift_group_id_list[i]);
		if (NULL == group_cfg)
		{
			continue;
		}

		reward_coin_bind += group_cfg->coin_bind;
		if (0 != group_cfg->reward_item.item_id)
		{
			temp_reward_vec.push_back(group_cfg->reward_item);
		}
	}
	if (gather->GetRoleModuleManager()->GetKnapsack()->CheckForPutList2(temp_reward_vec))
	{
		gather->GetRoleModuleManager()->GetKnapsack()->PutVec(temp_reward_vec, PUT_REASON_WEDDING);
	}
	else
	{
		gather->GetRoleModuleManager()->GetKnapsack()->SendMail(temp_reward_vec, SNED_MAIL_TYPE_DEFAULT, true);
	}

	gamelog::g_log_qing_yuan.printf(LL_INFO, "XunYouCar::OnGatherGift "
		"| gather_id[%d]",
		gather->GetUID());
}

void XunYouCar::CreatGift(int posi_index)
{
	const WeddingFB::XunYouCfg * xun_you_cfg
		= LOGIC_CONFIG->GetQingYuanConfig()->GeXunYouCfg(m_xun_you_type);
	if (NULL == xun_you_cfg)
	{
		return;
	}

	const WeddingFB::XunYouWayPoint * way_point_cfg
		= LOGIC_CONFIG->GetQingYuanConfig()->GeXunYouWayPoint(posi_index);
	if (NULL == way_point_cfg)
	{
		return;
	}

	time_t delete_time = EngineAdapter::Instance().Time() + xun_you_cfg->gift_valid_time;

	//����
	std::vector<Posi> creat_point_vec = way_point_cfg->creat_gift_vec;
	std::random_shuffle(creat_point_vec.begin(), creat_point_vec.end());

	int creat_point_size = (int)creat_point_vec.size();
	for (int i = 0
		; i < xun_you_cfg->creat_gift_num && i < creat_point_size
		; i++)
	{
		//����
		NPCGather * gather = NpcManager::Instance().CreateNPCGather(xun_you_cfg->gift_gather_id, this->GetScene(), creat_point_vec[i]);
		if (gather != NULL)
		{
			gather->SetParam(NULL, &m_xun_you_type);
			gather->SetSubType(NPCGather::SUB_TYPE_XUN_YOU);
			gather->SetRemoveAfterGatherFlag(true);
			gather->SetDeleteTime(delete_time);
		}
	}
}
