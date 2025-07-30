#ifndef __RAND_ACTIVITY_ANIMAL_RACE_HPP
#define __RAND_ACTIVITY_ANIMAL_RACE_HPP__

#include "servercommon/activitydef.hpp"
#include "global/randactivity/randactivity.hpp"
#include "servercommon/struct/global/animalracedef.hpp"
#include "servercommon/roleactivity/raanimalraceparam.hpp"
#include <map>
#include <vector>

struct arrive_info
{
	arrive_info() :times(0), number(0)
	{}

	double times;	//代表神兽抵达终点时所用时间
	int number;		//代表是哪个神兽index [0,3] 
};


class RandActivityAnimalRace : public RandActivity
{
public:
	RandActivityAnimalRace(RandActivityManager *activity_manager);
	~RandActivityAnimalRace();

	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);
	virtual void Update(unsigned long interval, time_t now_second);

	virtual void Init(const ActivityData &arg_data);
	virtual void GetInitParam(ActivityData & arg_data);

	virtual void OnUserLogout(Role *user);

	void AddBetCount(int add_num, int animal_race_number, int uid);

	int GetBetCount() { return m_param.total_bet_count; }
	void GetTypeCountAndTypeResponse(int *type_count, int *type_response_rate);
	long long GetTimestamp(int * op_type, char *rank, int len);
	void GetSceneRoleApp(role_info * appearance, int * cur_count, Role *role);

	void GetNeedInfo(int * server_id, int * date_id, time_t* next_refresh_timestamp);

	bool IsBetTime() { return m_param.is_bet; }
	void DistributeResults();
	void SceneRoleNumAdd(Role * role);
	void SceneRoleNumLess(int uid);
	int GetInvaildIndex();
	void GetAnimationData(AnimalRaceData * data);

	void GmForceStatus();
	virtual bool CanSyncActivityDataToCross() { return false; }
private:
	
	void OnRefreshGameInfo(time_t now_second);
	bool GetRefreshTimestamp(time_t now_second);
	void SetRefreshTimestamp(time_t now_second);
	void SetRoleInfo(Role * role, role_info *user_info);
	void ResetApp();
	void OnRefreshAnimationData();

	void GetSaveDBData(AnimalRaceEntry & param);
	
	struct ActivityAnimalParam
	{
		ActivityAnimalParam() { this->Reset(); }

		void Reset()
		{
			is_bet = false;
			server_id = 0;
			date_id = 0;
			next_refresh_game_info_stamp = 0;
			bet_end_timestamp = 0;
			win_animal_race_number = 0;
			total_bet_count = 0;
			memset(m_type_count, 0, sizeof(m_type_count));
			memset(m_type_response_rate, 0, sizeof(m_type_response_rate));
			animation_data.Reset();
		}

		void RefreshReset()
		{
			is_bet = false;
			server_id = 0;
			next_refresh_game_info_stamp = 0;
			bet_end_timestamp = 0;
			win_animal_race_number = 0;
			total_bet_count = 0;
			memset(m_type_count, 0, sizeof(m_type_count));
			memset(m_type_response_rate, 0, sizeof(m_type_response_rate));
			animation_data.Reset();
		}


		bool is_bet;												//是否可以押注
		int server_id;												//本期的在哪个服务器
		int date_id;												//这是哪一期
		long long next_refresh_game_info_stamp;						//下次刷新比赛信息的时间戳
		long long bet_end_timestamp;								//押注结束的时间戳
		int win_animal_race_number;									//随机获胜的神兽序号 
		int total_bet_count;										//所有玩家总共的押注数
		int m_type_count[MAX_ANIMAL_RACE_NUM];						//每个神兽押注数量
		int m_type_response_rate[MAX_ANIMAL_RACE_NUM];				//每个神兽此时的回报率
		AnimalRaceData animation_data;								//动画数据
	};

	bool is_time;													//是否在时间内 12 - 14 19 - 21

	role_info activity_scene_apppearance[MAX_ANIMAL_RACE_SCENE_NUM];	//固定保存19个玩家外观,点位跟配置表seq对应
	//std::map<int, Appearance> all_role_appearance;					//以uid为key   保存当前动画场景的所有角色外观   可为协议发送补充人数外观

	ActivityAnimalParam m_param;									//需要保存到数据库中的数据

	//std::map<int, std::vector<role_bet_info> > m_info_list;		//以神兽的序号为key     second为玩家的押注信息

};


#endif