#ifndef __PEAK_TOKEN_OBJ_HPP__
#define __PEAK_TOKEN_OBJ_HPP__

#include "obj/obj.h"


class Role;

//���������� ����
class PeakTokenObj : public Obj
{
public:
	//��Ҳ�����Ϣ
	class JoinInfo
	{
	public:
		JoinInfo();
		~JoinInfo();

		int m_role_uid;			//������uid
		time_t m_begin_time;	//����ʱ��

		int fetch_reward_times;	//�콱����
		//������¼
		//std::vector<ItemConfigData> m_reward_vec;
	};
	//�����¼ <uid, JoinInfo>
	typedef std::map<int, JoinInfo> JoinInfoMap;
public:
	PeakTokenObj();
	virtual ~PeakTokenObj();

	void Init(Role *creat_role, int reward_group_id);
	virtual void Update(unsigned long interval, time_t now_second, unsigned int now_dayid);

	//��ʼ
	void RoleJoin(Role *role);
	//�콱��
	void RoleFetch(Role *role);
	//���� �ͻ��˼�¼
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
	//����
	void ActNoticeAll();


	//���������� �id
	long long m_peak_token_act_id;
	int m_reward_group_id;

	time_t m_creat_time;	//����ʱ��
	time_t m_remove_time;	//��ɾʱ��

	//�������
	int m_role_uid;
	GameName m_role_name;
	Appearance m_role_appearance;



	time_t m_next_notice_time;	//�´ι���ʱ��

	//����map
	JoinInfoMap m_jion_info_map;

};

#endif