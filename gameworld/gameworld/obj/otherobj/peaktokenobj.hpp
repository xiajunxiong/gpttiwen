#ifndef __PEAK_TOKEN_OBJ_HPP__
#define __PEAK_TOKEN_OBJ_HPP__

#include "obj/obj.h"


class Role;

//天下至尊令 雕像
class PeakTokenObj : public Obj
{
public:
	//玩家参与信息
	class JoinInfo
	{
	public:
		JoinInfo();
		~JoinInfo();

		int m_role_uid;			//参与者uid
		time_t m_begin_time;	//参与时间

		int fetch_reward_times;	//领奖次数
		//奖励记录
		//std::vector<ItemConfigData> m_reward_vec;
	};
	//参与记录 <uid, JoinInfo>
	typedef std::map<int, JoinInfo> JoinInfoMap;
public:
	PeakTokenObj();
	virtual ~PeakTokenObj();

	void Init(Role *creat_role, int reward_group_id);
	virtual void Update(unsigned long interval, time_t now_second, unsigned int now_dayid);

	//开始
	void RoleJoin(Role *role);
	//领奖励
	void RoleFetch(Role *role);
	//结算 客户端记录
	//void RoleStatle(Role *role);

	time_t GetRoleJoinTime(Role *role);
	int GetRoleFetchTime(Role *role);

	int GetCreatUid() const { return m_role_uid; };
	void GetCreatName(GameName & name) const;
	const Appearance & GetCreatAppearance() const {return m_role_appearance;};

	long long GetActID() const { return m_peak_token_act_id; };
	time_t GetCreatTime() const { return m_creat_time; };
	time_t GetRemoveTime() const { return m_remove_time; };


	void SendRoleStatus(Role *role);
protected:
	//公告
	void ActNoticeAll();


	//天下至尊令 活动id
	long long m_peak_token_act_id;
	int m_reward_group_id;

	time_t m_creat_time;	//创建时间
	time_t m_remove_time;	//自删时间

	//创建外观
	int m_role_uid;
	GameName m_role_name;
	Appearance m_role_appearance;



	time_t m_next_notice_time;	//下次公告时间

	//参与map
	JoinInfoMap m_jion_info_map;

};

#endif