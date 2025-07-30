#ifndef __POKER_MODULE_MANAGER_HPP__
#define __POKER_MODULE_MANAGER_HPP__

#include <memory>
#include <vector>
#include <map>

#include "servercommon/serverdef.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/userprotocal/msgsystem.h"

#include "gameworld/gameworld/engineadapter.h"
#include "gameworld/gameworld/scene/scenemanager.hpp"

#include "protocol/msgpoker.hpp"

#include "obj/character/robot.h"
#include "obj/character/role.h"
#include "other/robot/robotmanager.hpp"

// ���Ƴ�Ա�ṹʾ�����������������
struct OnceMemberTemp
{
	int index;							// �������Ƶ�index    [1,5]
	int role_uid;						// UID
	GameName role_name;					// ��ɫ����
	char is_can_turn;					// �Ƿ���Է���
	char is_turned;						// �����Ƿ񷭹�
	ItemID reward_id;					// ��õĽ���ID
	int reward_count;					// ��������
	int luck_coin;						// ����ֵ
	unsigned int param_len;				// ��ֵ����																		[��ѡ]
	unsigned int param_len_shengyu;		// ��ֵʣ�೤��																	[��ѡ]
	NetValueItemParamData param_data;	//  char[256]  ������Ʒ��Ʒ��ֵ����
};

// ����ṹʾ��
struct TeamPokerRewardInfoTemp	
{
	TeamPokerRewardInfoTemp()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(once_info, 0, sizeof(void*) * SIDE_MAX_ROLE_NUM);
	}

	void *once_info[SIDE_MAX_ROLE_NUM];			// ������5����Ա �� ����Ϊonce_info

};

/*-------*/

// �����ͻ�Ľṹ
struct TeamPokerIndex	//	���Ƴ�ͻ
{
	TeamPokerIndex()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(pokker_index, 0, sizeof(pokker_index));
		pokker_index[0] = -1;
	}
	short pokker_index[SIDE_MAX_ROLE_NUM + 1];		// �ж�����������Ƶ��������ȫ��Ĭ��ѡ��һ���ƣ����ǰ�˳��ѡ
};


#define POKER_MODULE PokerModuleManager::Instance()

/*
 *	PKM_OFFLINE:						���ڵ��ߵ��߼�
 *  PKM_ONLINE:							�������ߵ��߼�
 *	PKM_ONLINE_AND_OFFLINE:				���߶���
 */
#define PKM_OFFLINE
#define PKM_ONLINE
#define PKM_ONLINE_AND_OFFLINE

class Robot;
class Role;

class PokerModuleManager 
{
public:
	PokerModuleManager();
	~PokerModuleManager();

	static PokerModuleManager& Instance();

	void Init();

	/**
	*	@brief						������ҷ�������
	*
	*	@param poker_recorder		�洢������[ͨ��]
	*	@param role					���
	*	@param reward				��������
	*	@param put_reason			����ԭ��
	*	@param is_conflict			�Ƿ��ж����ͻ�Ŀ���
	*	@param is_online			�Ƿ�����
	*	@param other_recorder		�洢������[����]
	*	@param team_id				����id
	*	@param team_struct			����ṹ
	*
	*	#return	void
	*	#call_situation		PKM_ONLINE_AND_OFFLINE
	*/
	template<class RecorderStruct>
	PKM_ONLINE_AND_OFFLINE void RoleRecorder(RecorderStruct *poker_recorder, Role *role, std::vector<ItemConfigData> &reward, int put_reason, bool is_conflict, bool is_online,int other_recorder,int team_id = -1,void * team_struct = NULL)
	{
		poker_recorder->role_uid = 0;
		if (is_online)
		{
			poker_recorder->role_uid = role->GetUID();
			STRNCPY(poker_recorder->role_name, role->GetName(), sizeof(poker_recorder->role_name));
		}
		poker_recorder->reward_id = reward[0].item_id;
		poker_recorder->reward_count = reward[0].num;

		const ItemBase *item = ITEMPOOL->GetItem(poker_recorder->reward_id);
		if (!item) return;
		NetValueItemParam tmp_item_param;
		if (!item->HasRandomNetValue(&tmp_item_param))
		{
			item->RandomNetValueOnPutItem(NULL, &tmp_item_param, put_reason);
			item->SetRandomNetValueFlag(&tmp_item_param);
		}

		if (other_recorder)
		{
			poker_recorder->param_len = tmp_item_param.GetDataByItemType(item->GetItemType(), poker_recorder->param_data);
			poker_recorder->param_len_shengyu = sizeof(poker_recorder->param_data) - poker_recorder->param_len;
		}


		if (is_conflict && team_id != -1 && team_struct != NULL)
		{	
			m_team_id_to_team_poker[team_id] = team_struct;

		}
		else if (is_conflict == false)
		{
			if (recorder_type_count >= SIDE_MAX_ROLE_NUM)
			{
				recorder_type_count = 0;
			}
			recorder_type[recorder_type_count++] = poker_recorder;
		}

		// ��ֹҰָ��
		poker_recorder = NULL;
		team_struct = NULL;
	}

	/**
	*	@brief						��β��������
	*
	*	@param poker_recorder		�洢������
	*	@param reward				���ý���
	*	@param put_reason			����ԭ��
	*	@param is_conflict			�Ƿ��ж����ͻ�Ŀ���
	*	@param other_recorder		�洢������[����]
	*	@param team_id				����id
	*	@param team_struct			����ṹ
	*
	*	#return	void
	*/
	template<class RecorderStruct>
	void EndLoopRecorder(RecorderStruct *poker_recorder, std::vector<ItemConfigData> &reward,int put_reason,bool is_conflict,bool other_record,int team_id = -1,void *team_struct = NULL)
	{
		if (poker_recorder->role_uid != 0) poker_recorder->Reset();
		if (reward.size() > 0)
		{
			poker_recorder->reward_id = reward[0].item_id;
			poker_recorder->reward_count = reward[0].num;

			// ���ƽ�����ֵ
			const ItemBase *item = ITEMPOOL->GetItem(poker_recorder->reward_id);
			if (!item) return;
			NetValueItemParam tmp_item_param;
			if (!item->HasRandomNetValue(&tmp_item_param))
			{
				item->RandomNetValueOnPutItem(NULL, &tmp_item_param, put_reason);
				item->SetRandomNetValueFlag(&tmp_item_param);
			}

			if (other_record)
			{
				poker_recorder->param_len = tmp_item_param.GetDataByItemType(item->GetItemType(), poker_recorder->param_data);
				poker_recorder->param_len_shengyu = sizeof(poker_recorder->param_data) - poker_recorder->param_len;
			}
		}

		if (is_conflict && team_id != -1 && team_struct != NULL)
		{
			m_team_id_to_team_poker[team_id] = team_struct;

		}
		else if(is_conflict == false)
		{
			if (recorder_type_count >= SIDE_MAX_ROLE_NUM)
			{
				recorder_type_count = 0;
			}
			recorder_type[recorder_type_count++] = poker_recorder;
		}
		
		// ��ֹҰָ��
		poker_recorder = NULL;
		team_struct = NULL;
	}

	/**
	*	@brief						����״̬��Ȩ��
	*
	*	@param poker_recorder		�洢������
	*	@param robot				�����˾��
	*
	*	#return	void
	*	#call_situation		PKM_OFFLINE
	*/
	template<class RecorderStruct>
	PKM_OFFLINE void PermissionsRecorder(RecorderStruct *poker_recorder, Robot *robot)
	{
		if (robot == NULL) //���Է���
		{
			poker_recorder->is_can_turn = 1;
			poker_recorder->is_turned = 0;
			poker_recorder->index = 0;
		} else
		{
			poker_recorder->is_can_turn = 0;
			poker_recorder->is_turned = 0;
			poker_recorder->index = 0;
		}
	}

	/**
	*	@brief						��ȡ����ṹ��ĳ����Ա������[һ������bossͬʱ����������] | ��Ա���� [�����Ƕ�bossͬʱ��������] 
		@note						// ���������һ������bossͬʱ����ʱ�������Ա�������г�ͻ�ģ��� team_id�� -1 ��Teamstruct ���Դ�TeamPokerRewardInfoTemp����
	*
	*	@param role_uid				��ɫid
	*	@param team_id				����id
	*
	*	#return	RecorderStruct*		
	*	#call_situation		
	*/
	template<class RecorderStruct,class Teamstruct>
	RecorderStruct* Get(int role_uid ,int team_id)
	{
		if (team_id == -1)
		{
			for (int i = 0; i < SIDE_MAX_ROLE_NUM; ++i)
			{
				RecorderStruct *cur = (RecorderStruct *) recorder_type[i];
				if (cur && cur->role_uid == role_uid)
				{
					return cur;
				}
			}
		} else
		{
			std::map<int, void *>::const_iterator it = m_team_id_to_team_poker.find(team_id);
			if (it != m_team_id_to_team_poker.end())
			{
				for (int i = 0; i < SIDE_MAX_ROLE_NUM; ++i)
				{
					RecorderStruct *cur = (RecorderStruct *) &((Teamstruct *)it->second)->once_info[i];
					if (cur && cur->role_uid == role_uid)
					{
						return cur;
					}
				}
			}

		}
		
		return NULL;
	}

	/**
	*	@brief						��ȡ����ṹ	[�������·���ȡ�ṹ]
	*
	*	@param team_id				����id
	*
	*	#return	Teamstruct *
	*	#call_situation
	*/
	template<class Teamstruct>
	Teamstruct* Get(int team_id)
	{
		std::map<int, void *>::const_iterator it = m_team_id_to_team_poker.find(team_id);
		if (it != m_team_id_to_team_poker.end())
		{
			return (Teamstruct*)it->second;
		}
		return NULL;
	}

	/**
	*	@brief						������ʱ�����ͻ������
	*
	*	@param cur_role_poker		���ƵĽ�ɫ����Ϣ�ṹ
	*	@param poker_index			�ý�ɫ������
	*	@param is_conflict			�Ƿ��ж����ͻ�Ŀ���
	*	@param pidx					�ý�ɫ���ڵĶ���ṹ
	*	@param put_reason			����ԭ��
	*	@param team_id				����id
	*	@param role					���Ƶ������ɫ
	*
	*	#return	void
	*/
	template<class RecorderStruct>
	void PokerConflict(RecorderStruct *cur_role_poker, int poker_index, bool is_conflict,TeamPokerIndex *pidx = NULL, int reason = -1,int team_id = -1,Role *role = NULL)
	{
		if (is_conflict && team_id != -1 && pidx != NULL)
		{	
			if (pidx->pokker_index[poker_index] != 1)
			{
				cur_role_poker->index = poker_index;
				pidx->pokker_index[poker_index] = 1;
			} else
			{
				if (reason == Protocol::CSPokerReq::POKER_REQ_SYSTEM)
				{
					for (int i = 1; i < SIDE_MAX_ROLE_NUM + 1; ++i)
					{
						if (pidx->pokker_index[i] != 1)
						{
							cur_role_poker->index = i;
							pidx->pokker_index[i] = 1;
							break;
						}
					}
				} else
				{
					if (role == NULL)
					{
						return;
					}
					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_has_poker_get);
					if (length > 0)
					{
						Protocol::SCSystemMsg sm;
						int sendlen = 0;
						if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
						{
							EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*) &sm, sendlen);
						}
					}

					role = NULL;
					return;
				}

			}

			cur_role_poker = NULL;
		}
		else
		{
			if (pokker_index[poker_index] != 1)
			{
				cur_role_poker->index = poker_index;
				pokker_index[poker_index] = 1;
			} else
			{
				for (int i = 1; i < SIDE_MAX_ROLE_NUM + 1; ++i)
				{
					if (pokker_index[i] != 1)
					{
						cur_role_poker->index = i;
						pokker_index[i] = 1;
						break;
					}
				}
			}

			cur_role_poker = NULL;
		}
	}

	/**
	*	@brief						�ͷŸ������� (�ṹ�������Reset����)
	*
	*	@param data					 Ҫ�ͷŵ�����
	*
	*	#return	void
	*/
	template<class FreeStruct>
	void FreeData(void *data)
	{
		if (data)
		{
			((FreeStruct*) data)->Reset();
		}

		data = NULL;
	}

	// 
	TeamPokerIndex& GetTeamPokerIndexByTeamId(int team_id)
	{
		return m_team_id_to_poker[team_id];
	}

private:
	// û�ж����ͻ
	void *recorder_type[SIDE_MAX_ROLE_NUM];		//�洢�Ľṹ
	int recorder_type_count;					// ����
	short pokker_index[SIDE_MAX_ROLE_NUM + 1];		// �ж�����������Ƶ��������ȫ��Ĭ��ѡ��һ���ƣ����ǰ�˳��ѡ

	//�����ͻ
	std::map<int, TeamPokerIndex> m_team_id_to_poker;				// team_id  -->  poker_index[ϵͳ�Զ����Ƴ�ͻ]
	std::map<int, void *> m_team_id_to_team_poker;					// team_id  -->  Team_info
};

#endif // !__POKER_MODULE_MANAGER_HPP__
