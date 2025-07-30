#include "muteplayerrecorder.hpp"
#include "servercommon/servercommon.h"

MutePlayerRecorder& MutePlayerRecorder::Instance()
{
	static MutePlayerRecorder inst;
	return inst;
}

void MutePlayerRecorder::Update(unsigned int now)
{
	if (now > m_next_check_expire_timestamp)
	{
		m_next_check_expire_timestamp = now + 10;
		this->CheckMuteTimeExpire(now);
	}
}

MutePlayerRecorder::MutePlayerRecorder() : m_next_check_expire_timestamp(0u)
{

}

MutePlayerRecorder::~MutePlayerRecorder()
{

}

bool MutePlayerRecorder::IsMute(int role_id)
{
	return m_mute_map.find(role_id) != m_mute_map.end();
}

void MutePlayerRecorder::AddMutePlayer(int role_id, unsigned int unmute_timestamp)
{
	MutePlayerMap::iterator it = m_mute_map.find(role_id);
	if (it == m_mute_map.end())
	{
		m_mute_map.insert(MutePlayerMap::value_type(role_id, unmute_timestamp));
	}
	else
	{
		it->second = GetMin(it->second, unmute_timestamp);
	}
}

void MutePlayerRecorder::ClearMutePlayer(int role_id)
{
	m_mute_map.erase(role_id);
}

void MutePlayerRecorder::CheckMuteTimeExpire(unsigned int now)
{
	MutePlayerMap::iterator it = m_mute_map.begin();
	for (; it != m_mute_map.end();)
	{
		unsigned int unmute_timestamp = it->second;
		if (now > unmute_timestamp)
		{
			m_mute_map.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

