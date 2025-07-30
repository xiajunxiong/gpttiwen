#ifndef __SPECIAL_LOGIC_WEDDING_FB_HPP__
#define __SPECIAL_LOGIC_WEDDING_FB_HPP__

#include "scene/speciallogic.hpp"
#include "servercommon/weddingdef.hpp"

class Role;

namespace WeddingFB
{
	////С��Ϸ 
	//class SubActBase
	//{
	////����
	////����
	////����
	//};
	//
	//class SubActBaoWeiXiuQiu : public SubActBase
	//{
	//
	//};

	//������ɫ��Ϣ
	struct RoleInfo
	{
		RoleInfo()
		{
			role_uid = 0;
			
			meal__last_fetch_time = 0;
			memset(commom_param, 0, sizeof(commom_param));

			xun_qing_mi_ai_fetch_flag = 0;
			xin_xin_xiang_yin_fetch_flag = 0;
			bao_wei_xiu_qiu_fetch_flag = 0;
			zao_sheng_gui_zi_fetch_flag = 0;
		}
		int role_uid;	//uid

		time_t meal__last_fetch_time;//�ò�  �ϴ��ò�ʱ��

		//������������[����]
		int commom_param[COMMON_OPER_TYPE_MAX];

		//�����
		// Ѱ���ٰ�
		Posi xun_qing_mi_ai_posi;//�����(û��Ҫ)
		int xun_qing_mi_ai_fetch_flag;
		// ������ӡ
		int xin_xin_xiang_yin_fetch_flag;

		// ��������
		////�ͻ���Ҫת��(npc_id, ״̬)�·�
		int bao_wei_xiu_qiu_fetch_flag;

		// ��������
		int zao_sheng_gui_zi_fetch_flag;

	}; 

}

class SpecialLogicWeddingFB : public SpecialLogic
{
public:
	SpecialLogicWeddingFB(Scene* scene);
	virtual ~SpecialLogicWeddingFB() {};

	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnRoleEnterScene(Role *role);

	virtual bool CanDestroy() { return 0 != m_is_finish; }
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack);
	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_WEDDING_FB; }

	virtual bool CanRoleStartFight(Role * role, CanRoleStartFightParam &fight_param);


	void SetSceneParam(int wedding_id, time_t begin_time);

	void OnFBCommonReq(Role *role, int common_type, int param);
	void OnFBSubActReq(Role *role, int common_type, int param);

	void SendSceneInfo(Role * role);
	void SendRoleInfo(Role * role);
	void SendFBSync(Role * role, int sync_type, int param);

	//��Ļ
	void SendBarrage(Role *role, const char * chat, unsigned int chat_len);

	//����
	int CenEnterFB(Role* role);

	void GMNextLink();
private:
	void CheckLink(time_t now_second);
	//����
	void Finish();

	//��ϲ��
	void FetchCandies(Role *role);
	//���ò�������
	void SetRiteParam(Role *role, int param);
	//�ò�
	void Meal(Role *role);
	//����ף��
	void BuyBless(Role *role, int shop_item_index);
	//��ȡ�ȶȽ���
	void FetchHeatReward(Role *role, int reward_seq);
	//�����ȶ�
	void AddHeat(int add_num);

private:
	//С��Ϸ
	// Ѱ���ٰ�
	////�콱
	void FetchXubQingMiAi(Role *role);
	////֪ͨ
	void SendXubQingMiAi(Role *role);

	// ������ӡ
	////�콱
	void FetchXinXinXiangYin(Role *role);
	////֪ͨ
	void SendXinXinXiangYin(Role *role);

	// ��������
	////�콱
	void FetchBaoWeiXiuQiu(Role *role, int index);
	////֪ͨ
	void SendBaoWeiXiuQiu(Role *role);

	// ��������
	////�콱
	void FetchZaoShengGuiZi(Role *role, int index);
	////֪ͨ
	void SendZaoShengGuiZi(Role *role);

private:
	WeddingFB::RoleInfo *GetRoleInfo(int uid);

	int m_is_finish;	//������ɱ��

	WeddingID m_wedding_id;	//����id

	//����ʱ�� �ͻ���չʾ��
	time_t m_fb_begin_time;	//������ʼʱ��
	time_t m_fb_end_time;	//��������ʱ��

	int m_link_seq;			//��ǰ����
	time_t m_link_begin_time;//������ʼʱ��
	time_t m_link_end_time;	//���ڽ���ʱ��

	int m_wedding_heat;	//�����ȶ�

	time_t m_next_check_time;	//�´μ��ʱ��

	WeddingFB::HostInfo host_info_[WeddingFB::HOST_NUM];	//�������б�

	std::map<int, WeddingFB::RoleInfo> m_join_role_info_map;
};


#endif
