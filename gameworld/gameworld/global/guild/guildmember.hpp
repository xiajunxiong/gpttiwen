#ifndef __GUILD_MEMBER_HPP__
#define __GUILD_MEMBER_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/guilddef.hpp"
#include "protocol/msgguild.hpp"

class Role;
class GuildMemberManager;
class GuildMember
{
public:
	GuildMember(GuildMemberManager *member_manager);
	~GuildMember();
	void Init(const GuildMemberParam::Member &member_param);
	void GetAttr(GuildMemberParam::Member *member_attr) const;

	bool GetInfo(Protocol::MemberInfo *info);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnWeekChange();

	int GetUid() const { return m_uid; } 
	int GetIndex() { return m_index; }
	time_t GetJoinTime() { return m_join_time; }
	int GetPost() { return m_post; }
	const char* GetName();
	void GetOriginalName(GameName original_name);
	void SetName(GameName name);
	void SetLevel(int level);
	int GetLevel();
	void SetAppearance(const Appearance& appearance);
	void GetAppearance(Appearance * appearance) { *appearance = m_appearance; }
	void SetHonorBattleFlag();	
	void ClearHonorBattleFlag();	

	void SetProfession(int profession);
	int GetProfession() { return m_profession; }

	void SysContribution(int value) { m_gongxian = value; };
	int AddGongXian(int value, const char* type);
	int GetTotalGongXian() { return m_total_gongxian; }
	int GetGongXian() { return m_gongxian; }
	const GuildMemberData & GetMemberData()   const { return m_member_data; }
	GuildMemberData& MutableMemberData() { return m_member_data; }
	GuildMemberData & GetMemberDataAndSetDirty();
	void SetWeekTaskContribute(int value);
	void AddWeekTaskContribute(int add_value);
	int GetWeekTaskContribute() const { return m_member_data.guild_task_contribute; }
	unsigned int GetLastTaskContributeChangeTime() const { return m_member_data.last_change_task_contribute_time; }

	void SetOnline(int online_state);
	int GetOnLinetate() const;
	void SetPost(int post);
	void SetGuildBossDayInfo(Role * role);		

	void OnRoleLogin(Role* role);
	void OnRoleLogout(Role* role);
	void UpdateMemberInfo(Role* role);
	void UpdateMemberInfoFromHidden(const GuildMemberParam::Member & member_param);

	unsigned int GetLastLogintime();
	void SetTanHeJoinFlag(unsigned char f);  //设置是否参加组长弹劾标志
	unsigned char GetSetTanHeJoinFlag();

	/////////////////////////////////////////////////////////////////////////////////
	// 家族活跃度  礼包及排行榜相关
	void AddWeekActiveVal(int _add_val);
	void SetWeekActiveVal(int _week_val , int _member_seven_day_active_today_active);
	unsigned int GetWeekActiveVal() { return m_member_data.week_active_val; }
	unsigned int GetLastChangeWeekActiveValTimes() { return m_member_data.last_change_week_active_val_times; }
	void ClearWeekActiveValInfo();
	
	void SetGuildDayActiveRewardFlag(int _flag);
	int GetGuildDayActiveRewardFlag() { return m_member_data.guild_day_active_reawrd_flag; }
	void ClearGuildDayActiveRewardFlag();
	void GuildActiveDayActiveReward(int _day_active_val, unsigned int _now_sec);
	void GuildActiveWeekActivePersonRankReward(int _rank_index, int _val, unsigned int _now_sec);

	/////////////////////////////////////////////////////////////////////////////////
	// 7日活跃
	int GetSevenDayTotalActive();
	int GetSevenDayTodayActive();

	//祈福
	int GetQiFuFlag() { return m_member_data.is_get_guild_attr; }
	void SetQiFuFlag();
	void ClearQiFuFlag();

	//战神榜
	void AddHallOfFameVal(int type, int _add_val);
	void SetHallOfFameVal(int type, int _add_val);
	int GetHallOfFameVal(int type);
	unsigned int GetLastChangeHallOfFameValTimes(int type);
	void ClearHallOfFameValInfo(int type);

	//建筑建造加速
	bool JoinSpeedUp(Role* role);
//	int SpeedUpTimes() { return m_member_data.is_cur_building_help; }			//今日参与加速次数
//	void ClearSpeedUpflag();	//建造完成后清除建筑加速标记
	void SynSpeedUpCount(int count);		// 同步加速次数

	void * operator new(size_t c);
	void operator delete(void *m);

private:
	GuildMemberManager *m_member_manager;

	int m_index;

	int m_uid;
	int m_level;
	int m_profession;
	GameName m_name;
	Appearance m_appearance;

	char m_post;

	int m_online_state;

	int m_gongxian;
	int m_total_gongxian;

	time_t m_join_time;
	time_t m_last_login_time;

	GuildMemberData m_member_data;

#ifdef _DEBUG
	int m_index_FOR_TEST;
#endif
};

#endif // __GUILD_MEMBER_HPP__

