#include "pokermodulemanager.hpp"
#include "gameworld/item/itembase.h"
#include "gameworld/item/knapsack.h"

PokerModuleManager::PokerModuleManager()
{
	this->Init();
}

PokerModuleManager::~PokerModuleManager()
{
}

PokerModuleManager & PokerModuleManager::Instance()
{
	static PokerModuleManager m_poker_manager;
	return m_poker_manager;
}

void PokerModuleManager::Init()
{
	recorder_type_count = 0;
	memset(recorder_type, 0, sizeof(void*) * SIDE_MAX_ROLE_NUM);
	memset(pokker_index, 0, sizeof(pokker_index));
	pokker_index[0] = -1;

	m_team_id_to_poker.clear();
	m_team_id_to_team_poker.clear();
}

