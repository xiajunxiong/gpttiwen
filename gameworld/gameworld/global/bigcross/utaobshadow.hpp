#ifndef __UTA_OB_SHADOW_HPP__
#define __UTA_OB_SHADOW_HPP__

#include <map>
#include <set>

namespace bigchgameprotocol
{
	struct BigCHGameSyncRoundResult;
	struct BigCHGameUTASyncBattleInfoForOB;
	struct BigCHGameUTASyncRoundResultForOB;
}

class Role;
class UTAOBFightData;
class UTAOBShadow
{
public:
	typedef std::map<int, long long> ObserverMap;
	typedef std::map<long long, UTAOBFightData*> FightDataMap;
	typedef std::set<long long> EndedFightSet;

public:
	static UTAOBShadow& Instance();

	void Update(unsigned int now);

	void StartOB(Role* role, long long schedule_id);
	void StopOB(Role* role);

	void OnSyncRoundResultData(bigchgameprotocol::BigCHGameUTASyncRoundResultForOB* msg);
	void OnSyncBattleInfoData(bigchgameprotocol::BigCHGameUTASyncBattleInfoForOB* msg);
	void OnBattleEndStopOB(long long schedule_id);
	void OnDisconnectFromBigCrossServer();
	void OnDisconnectFromCrossServer();

	bool IsFightEnd(long long schedule_id);
	const UTAOBFightData* GetFightData(long long schedule_id);
	bool IsRoleOBIng(Role* role);
private:
	UTAOBShadow();
	~UTAOBShadow();
	UTAOBShadow(const UTAOBShadow&);
	UTAOBShadow& operator=(const UTAOBShadow&) const;

	void Release();
	void CheckBattleExpire(unsigned int now);
	UTAOBFightData* MutableFightData(long long schedule_id);
	void RemoveFightData(long long schedule_id);
	void StopAllOBAndRemoveAllFightData();
	void AddEndedFightToSet(long long schedule_id);
private:
	ObserverMap m_ob_map;
	FightDataMap m_fight_data_map;
	EndedFightSet m_ended_fight_set;
};

#endif