#include "topicmanager.hpp"
#include "obj/character/role.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/topicmanager/topicimpl/topicmarket.hpp"
#include "global/topicmanager/topicimpl/topicteamchannel.hpp"
#include "global/topicmanager/topicimpl/topicchiefelection.hpp"
#include "global/topicmanager/topicimpl/topicchiefpeakbattle.hpp"
#include "global/topicmanager/topicimpl/topicworldbossrank.hpp"
#include "global/topicmanager/topicimpl/topicworldboss2rank.hpp"
#include "global/topicmanager/topicimpl/topicworldboss3rank.hpp"
#include "global/topicmanager/topicimpl/topicrapaimaihang.hpp"

TopicManager* TopicManager::Instance()
{
	static TopicManager inst;
	return &inst;
}

bool TopicManager::OnRoleSubscribe(Role* role, int topic)
{
	if (!this->AddSubscribe(role->GetUID(), role->GetNetId(), topic)) return false;

	m_topic_list[topic]->OnTopicSubscribe(role);

	return true;
}

bool TopicManager::OnRoleUnSubscribe(Role* role, int topic)
{
	return this->RemoveSubscribe(role->GetUID(), topic);
}

void TopicManager::OnRoleLogout(Role* role)
{
	for (int topic = 0; topic < TOPIC_TYPE_MAX; ++topic)
	{
		this->RemoveSubscribe(role->GetUID(), topic);
	}
}

void TopicManager::OnTopicUpdate(int topic, const char* data, int length)
{
	if (topic < 0 || topic >= TOPIC_TYPE_MAX || NULL == m_topic_list[topic]) return;

	m_topic_list[topic]->OnTopicUpdate(m_subscriber_map[topic], data, length);
}

bool TopicManager::AddSubscribe(int role_id, const GSNetID& netid, int topic)
{
	if (topic < 0 || topic >= TOPIC_TYPE_MAX) return false;

	if ((TOPIC_TYPE_CHIEF_ELECTION_START <= topic && topic <= TOPIC_TYPE_CHIEF_ELECTION_END) ||
	    topic == TOPIC_TYPE_CHIEF_PEAK_BATTLE)
	{
		if (!CrossConfig::Instance().IsHiddenServer())
		{
			return false;
		}
	}

	SubscriberMap::iterator it = m_subscriber_map[topic].find(role_id);
	if (it != m_subscriber_map[topic].end()) return false;

	static Subscriber subscriber;
	subscriber.role_id = role_id;
	subscriber.netid = netid;

	m_subscriber_map[topic].insert(SubscriberMap::value_type(role_id, subscriber));

	return true;
}

bool TopicManager::RemoveSubscribe(int role_id, int topic)
{
	if (topic < 0 || topic >= TOPIC_TYPE_MAX) return false;

	SubscriberMap::iterator it = m_subscriber_map[topic].find(role_id);
	if (it == m_subscriber_map[topic].end()) return false;

	m_subscriber_map[topic].erase(it);

	return true;
}

TopicManager::TopicManager()
{
	memset(m_topic_list, 0, sizeof(m_topic_list));

	m_topic_list[TOPIC_TYPE_TEAM_CHANNEL] = new TopicTeamChannel();

	for (int i = TOPIC_TYPE_CHIEF_ELECTION_START; i <= TOPIC_TYPE_CHIEF_ELECTION_END; ++i)
	{
		m_topic_list[i] = new TopicChiefElection();
	}

	m_topic_list[TOPIC_TYPE_CHIEF_PEAK_BATTLE] = new TopicChiefPeakBattle();
	m_topic_list[TOPIC_TYPE_MARKET] = new TopicMarket();
	m_topic_list[TOPIC_TYPE_WORLD_BOSS_1] = new TopicWorldBossRank();
	m_topic_list[TOPIC_TYPE_WORLD_BOSS_2] = new TopicWorldBoss2Rank();
	m_topic_list[TOPIC_TYPE_RA_PAI_MAI_HANG] = new TopicRAPaiMaiHang();
	m_topic_list[TOPIC_TYPE_WORLD_BOSS_3] = new TopicWorldBoss3Rank();

	UNSTD_STATIC_CHECK(TOPIC_TYPE_MAX == 15);
}

TopicManager::~TopicManager()
{
	for (int topic = 0; topic < TOPIC_TYPE_MAX; ++topic)
	{
		if (NULL != m_topic_list[topic])
		{
			delete m_topic_list[topic];
			m_topic_list[topic] = NULL;
		}
	}
}

