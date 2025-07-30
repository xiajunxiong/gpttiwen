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

// 翻牌成员结构示例，必须包含的属性
struct OnceMemberTemp
{
	int index;							// 翻到的牌的index    [1,5]
	int role_uid;						// UID
	GameName role_name;					// 角色名字
	char is_can_turn;					// 是否可以翻牌
	char is_turned;						// 该牌是否翻过
	ItemID reward_id;					// 获得的奖励ID
	int reward_count;					// 奖励数量
	int luck_coin;						// 气运值
	unsigned int param_len;				// 净值长度																		[可选]
	unsigned int param_len_shengyu;		// 净值剩余长度																	[可选]
	NetValueItemParamData param_data;	//  char[256]  翻牌物品物品净值参数
};

// 队伍结构示例
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

	void *once_info[SIDE_MAX_ROLE_NUM];			// 必须有5个成员 且 命名为once_info

};

/*-------*/

// 队伍冲突的结构
struct TeamPokerIndex	//	翻牌冲突
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
	short pokker_index[SIDE_MAX_ROLE_NUM + 1];		// 判断如果都不翻牌的情况避免全部默认选第一张牌，而是按顺序选
};


#define POKER_MODULE PokerModuleManager::Instance()

/*
 *	PKM_OFFLINE:						用于掉线的逻辑
 *  PKM_ONLINE:							用于在线的逻辑
 *	PKM_ONLINE_AND_OFFLINE:				两者都有
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
	*	@brief						保存玩家翻牌数据
	*
	*	@param poker_recorder		存储的数据[通用]
	*	@param role					玩家
	*	@param reward				奖励配置
	*	@param put_reason			放入原因
	*	@param is_conflict			是否有队伍冲突的可能
	*	@param is_online			是否在线
	*	@param other_recorder		存储的数据[其他]
	*	@param team_id				队伍id
	*	@param team_struct			队伍结构
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

		// 防止野指针
		poker_recorder = NULL;
		team_struct = NULL;
	}

	/**
	*	@brief						结尾奖励设置
	*
	*	@param poker_recorder		存储的数据
	*	@param reward				配置奖励
	*	@param put_reason			放入原因
	*	@param is_conflict			是否有队伍冲突的可能
	*	@param other_recorder		存储的数据[其他]
	*	@param team_id				队伍id
	*	@param team_struct			队伍结构
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

			// 翻牌奖励净值
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
		
		// 防止野指针
		poker_recorder = NULL;
		team_struct = NULL;
	}

	/**
	*	@brief						掉线状态的权限
	*
	*	@param poker_recorder		存储的数据
	*	@param robot				机器人句柄
	*
	*	#return	void
	*	#call_situation		PKM_OFFLINE
	*/
	template<class RecorderStruct>
	PKM_OFFLINE void PermissionsRecorder(RecorderStruct *poker_recorder, Robot *robot)
	{
		if (robot == NULL) //可以翻牌
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
	*	@brief						获取队伍结构里某个成员的数据[一场景多boss同时死亡，调用] | 成员数据 [不考虑多boss同时死亡问题] 
		@note						// 如果不考虑一个场景boss同时死亡时，队伍成员奖励会有冲突的，的 team_id传 -1 ，Teamstruct 可以传TeamPokerRewardInfoTemp充数
	*
	*	@param role_uid				角色id
	*	@param team_id				队伍id
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
	*	@brief						获取队伍结构	[可用于下发获取结构]
	*
	*	@param team_id				队伍id
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
	*	@brief						不翻牌时队伍冲突的问题
	*
	*	@param cur_role_poker		翻牌的角色的信息结构
	*	@param poker_index			该角色翻得牌
	*	@param is_conflict			是否有队伍冲突的可能
	*	@param pidx					该角色所在的队伍结构
	*	@param put_reason			放入原因
	*	@param team_id				队伍id
	*	@param role					翻牌的这个角色
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
	*	@brief						释放个人数据 (结构必须包含Reset函数)
	*
	*	@param data					 要释放的数据
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
	// 没有队伍冲突
	void *recorder_type[SIDE_MAX_ROLE_NUM];		//存储的结构
	int recorder_type_count;					// 数量
	short pokker_index[SIDE_MAX_ROLE_NUM + 1];		// 判断如果都不翻牌的情况避免全部默认选第一张牌，而是按顺序选

	//队伍冲突
	std::map<int, TeamPokerIndex> m_team_id_to_poker;				// team_id  -->  poker_index[系统自动翻牌冲突]
	std::map<int, void *> m_team_id_to_team_poker;					// team_id  -->  Team_info
};

#endif // !__POKER_MODULE_MANAGER_HPP__
