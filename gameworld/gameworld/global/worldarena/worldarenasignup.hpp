#ifndef __WORLD_ARENA_SIGN_UP_HPP__
#define __WORLD_ARENA_SIGN_UP_HPP__

#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/struct/global/worldarenaparam.hpp"
#include <set>

class WorldArena;
struct RoleBattleInfo;
class WorldArenaSignUp
{
public:
	typedef std::set<long long> SignUpRecorder;

public:
	WorldArenaSignUp(WorldArena* arena_ptr);
	~WorldArenaSignUp();

	void OnInitSetUserSignup(long long uuid);
	void OnUserSignUp(long long uuid);

	bool HasUserSignedUp(long long uuid);
	int GetSignupNum() const;

	void OnSeasonRestart();
	const std::set<long long>& GetSignupRecorder() const;
private:
	WorldArena* m_arena_ptr;
	SignUpRecorder m_signup_recorder;

};

#endif