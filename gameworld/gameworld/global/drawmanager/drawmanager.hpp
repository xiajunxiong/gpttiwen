#ifndef __DRAW_MANAGER_HPP__
#define __DRAW_MANAGER_HPP__

#include "servercommon/struct/global/drawdef.hpp"

/*
���ƹ�����  ֻ���� �������ź�ѡ��index��֪ͨ (�ֶ���ȡ, ��ʱ�Զ��·�)

	���ݽṹ:
		������id   �������1000 ��ʼ����
		����map<������id, ��������Ϣ> (�����)

	���ƹ�����:
		������ ȫ�� �㲥 ��Ա �ķ���ѡ��index(�ͻ���չʾ��  ʵ�ʽ������ڴ���������ʱ���Ѿ�ȷ��)
		���ֶ�����ʱ �����·���Ӧ�Ľ��� �� �㲥����ѡ��   TODO:����һ����̬����ָ������֧��һЩ���⴦��
		�ڳ�ʱʱ	 �·���Ӧ������㲥		֮��� ɾ�� ������ ��Ϣ

*/
/*ʹ��
	����
	DrawGroupInfo group_info;
	for(int i = 0; i < DRAW_GROUP_MAX_ITEM_NUM; i++)
	{
		group_info.draw_count++;
		group_info.draw_info_list[i].role_uid = ӵ����id;
		group_info.draw_info_list[uid].draw_index = ������ȡ״̬;

		group_info.draw_info_list[uid].money_type[������������] = ��������;

		//������Ʒ
		group_info.draw_info_list[uid].item_list.inster(������Ʒ);
		//���� �Զ�������Ʒ���������� DrawManager::Instance().PutRewardItem(group_info.draw_info_list[uid], ��Ʒ����, ��Ʒ�б�)

		//��Ϣ֪ͨЭ�鲢û�����ϵ�һ�� ������ҪȡһЩ��Ϣ��֪ͨЭ����
		if (��ɫ����)
		{
			role.getname(Э��Ҫ�õĽ�ɫ��);
		}
		else
		{
			 DrawManager::Instance().GetRoleOrRobotName(ӵ����id,Э��Ҫ�õĽ�ɫ��);
		}
	}
	//����ʧЧʱ��(��) ʧЧ����Զ�������
	group_info.out_time = now_time + xxx;

	//���ҽ���(һ�㲻��Ҫ, ����������������)
	//DrawManager::Instance().ShuffDrawGroupByUid(group_info);

	//���������  
	DrawManager::Instance().AddDrawGroup(group_info);
	
	//���ú�Żᴫ���Զ�����Ľ�����id
	group_info.draw_group_id;

	//ĿǰֻҪʧЧʱ��û�� ������Խ���������κβ���
	//������ ����ϵͳ�� ��ȡ������������⴦��
	//TODO:ͨ�����ܷ��� ������������Ͻ��� �罱������ ����.....
*/

//������ ����map<������id, ��������Ϣ>
typedef std::map<int, DrawGroupInfo> DrawGroupMap;

//������ ��ʱmap<ʧЧʱ���, ������id>
typedef std::multimap<unsigned int, int> DrawGroupOutTimeMap;

//uidset<uid, ������id>
typedef std::map<int, int> DrawUIDMap;

class Role;

class DrawManager
{
public:

	static DrawManager & Instance();

	void OnServerStart();
	void OnServerStop();

	void OnRoleLogOut(Role * role);

	void Update(unsigned long interval, time_t now_second);

	void OnReq(Role *role, int type, int param_1, int param_2);

	/*
	��ӽ�����
	deaw_group_info ����ʱ����	draw_group_type ������ ����
								draw_count		���ƽ�����
								draw_info		�������������߼�����
	need_rand		�Ƿ���� ������������ �� ����
	*/
	void AddDrawGroup(DrawGroupInfo &deaw_group_info, int draw_group_type);

	//��ȡ��������Ϣ
	const DrawGroupInfo *GetDrawGroup(int draw_group_id) const;

	//��ȡ����(�ֶ�����)
	void OnRoleDraw(Role *role, int draw_group_id, int draw_index);

	//���Ž�����ȫ������
	void GiveAllDraw(DrawGroupInfo &draw_group_info);

	//�����������(����ɫ)
	void GiveRandDraw(int uid, DrawGroupInfo &draw_group_info, Role *role = NULL);

	bool IsDrawing(int uid);

	//���뽱����Ʒ(�����Ʒ����)
	static void PutRewardItem(DrawGroupInfo::DrawItem &reward_info, int num, const ItemConfigData *item_cfg_list, int prof = 0);
	//ϴ��  ���ҽ��� (ֻ��uidϴ��,,���������uid����)
	static void ShuffDrawGroupByUid(DrawGroupInfo &draw_group_info);

	//��role����reward_info��Ľ���  NULL == *role ʱ ����reward_info.uid ���ʼ�
	static void SendGroupReward(DrawGroupInfo::DrawItem reward_info, int group_type, Role *role = NULL);

	//��ý�ɫ���������
	static void GetRoleOrRobotName(int role_uid, GameName &name);

	static void GetItemInfoType(ItemDrawInfo &info_type, const ItemDataWrapper & item_Data);

private:
	DrawManager();
	~DrawManager();

	//��ⳬʱ(����������ʧЧ)
	void CheckOutTime(unsigned int now_second, bool clear_all = false);

	void GetNewDrawGroupId(int &new_draw_group_id);
	bool IsAllUserFetch(DrawGroupInfo & draw_group_info);		//���÷���������Ƿ�ȫ����ȡ��

	//��ʱ���
	unsigned int m_next_check_timeout;
	DrawGroupOutTimeMap m_draw_group_out_time;

	
	int m_draw_group_id_auto;		//������ ����id
	//������map
	DrawGroupMap m_draw_group;

	//���ڷ��ƵĽ�ɫ��¼ һ��һ����ɫֻ����1��δ��ȡ�Ľ����� ���Բ����Ƕ�������
	DrawUIDMap m_draw_uid_map;
};

//�޸ĺ���֪ͨ�ͻ����޸�Э���������		//todo:gfp
UNSTD_STATIC_CHECK(sizeof(ItemDataWrapper) == 1052);
UNSTD_STATIC_CHECK(sizeof(NetValueItemParamData) == 1024);

#endif