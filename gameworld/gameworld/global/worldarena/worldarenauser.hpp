#ifndef __WORLD_ARENA_USER_HPP__
#define __WORLD_ARENA_USER_HPP__

#include "worldarenauserdata.hpp"

namespace Protocol
{
	struct MsgRankListUserInfo;
}

struct RobotParam;
struct WorldArenaRobotAttrAdjustCfg;
struct PetBattleData;
struct PartnerBattleData;
struct PetHelperBattleData;
class WorldArenaUser
{
public:
	WorldArenaUser(bool old_state);
	~WorldArenaUser();

	void OnDayChange();

	long long GetUUID() const;
	int GetUID() const;
	const char* GetName() const;
	void SetSignUpInfo(const WorldArenaSignUpInfo& info);
	void SetSeasonIndex(short season_index);
	void SetRoleBattleData(const RoleBattleData& rbd);

	void SetGroupID(long long group_id);
	long long GetGroupID() const;
	void SetScheduleID(int round_idx, long long schedule_id);
	long long GetScheduleID(int round_idx);
	bool IsRobot() const;
	const RoleBattleData& GetRoleBattleData();
	void AssembleRobotParam(RobotParam& param, int level);
	void GetScheduleList(ARG_OUT long long* schedule_list, int max_num);

	int GetAvatarType() const;
	const char* GetGuildName() const;
	int GetHeadshotId() const;
	int GetLevel() const;
	int GetPlatType() const;
	int GetProfession() const;
	short GetSeasonIndex() const;
	const Appearance& GetAppearance() const;

	const WorldArenaSignUpInfo* GetSignUpInfoPtr() const;
	const WorldArenaSignUpInfo& GetSignUpInfoRef() const;

	bool IsDirty() const;
	void ClearDirtyMark();
	void SetLastSaveTimestampMS(unsigned long long timestamp_ms);

	void SetUserInfoForInit(const WorldArenaUserInfo& info);
	const WorldArenaUserInfo& GetUserInfoForSave() const { return m_user_data.GetUserData(); }

	int GetScore() const;
	int GetWinNum() const;
	int GetLoseNum() const;
	int GetDrawNum() const;
	int GetTotalFightNum() const;
	int GetTotalBattleRound() const;
	bool HasFetchSignupReward() const;
	void OnFetchSignupReward();
	bool CanFetchDailyReward() const;
	void ResetDailyRewardFetchFlag();
	void OnFetchDailyRerward();
	char GetFetchDailyRewardFlag() const;
	void SetDailyRewardAvaliable();
	int GetWinBoxRewardFlag();
	void OnFetchWinBox(int sn);
	void OnSetFightResult(bool is_win, bool is_draw, bool is_lose, bool is_giveup, int round_num);
	void AssembleMsgRankListUserInfo(ARG_OUT Protocol::MsgRankListUserInfo* info);
	bool HasTodayGiveupNotice() const;
	void ClearTodayGiveNotice();
	void UpdateUserSignupInfo(const WorldArenaSignUpInfo& info);
	void UpdateRobotLevel(int level);
private:
	void SendGiveupNoticeMail();
	void AdjustRobotAttr(BattleAttr& attr, int adjust_percent);
	bool FillRobotPetData(PetBattleData& data, int robot_level, const WorldArenaRobotAttrAdjustCfg* adjust_cfg, bool is_pet_helper);
	bool FillRobotPartnerData(PartnerBattleData& partner_data, int robot_level, int partner_id, const WorldArenaRobotAttrAdjustCfg* adjust_cfg);
	bool FillRobotPetHelperData(PetHelperBattleData& pet_helper_data, int robot_level, const WorldArenaRobotAttrAdjustCfg* adjust_cfg);

	WorldArenaUserData m_user_data;
};


#endif