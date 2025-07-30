#include "worldarena.hpp"
#include "worldarenasignup.hpp"
#include "gamelog.h"
#include "servercommon/errornum.h"
#include "world.h"

WorldArenaSignUp::WorldArenaSignUp(WorldArena* arena_ptr) : m_arena_ptr(arena_ptr)
{

}

WorldArenaSignUp::~WorldArenaSignUp()
{

}

void WorldArenaSignUp::OnInitSetUserSignup(long long uuid)
{
	m_signup_recorder.insert(uuid);
}

void WorldArenaSignUp::OnUserSignUp(long long uuid)
{
	m_signup_recorder.insert(uuid);
}

bool WorldArenaSignUp::HasUserSignedUp(long long uuid)
{
	return m_signup_recorder.find(uuid) != m_signup_recorder.end();
}

int WorldArenaSignUp::GetSignupNum() const
{
	return static_cast<int>(m_signup_recorder.size());
}

void WorldArenaSignUp::OnSeasonRestart()
{
	m_signup_recorder.clear();
}

const std::set<long long>& WorldArenaSignUp::GetSignupRecorder() const
{
	return m_signup_recorder;
}

