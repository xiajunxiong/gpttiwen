#ifndef __SPECIAL_LOGIC_WEDDING_FB_HPP__
#define __SPECIAL_LOGIC_WEDDING_FB_HPP__

#include "scene/speciallogic.hpp"
#include "servercommon/weddingdef.hpp"

class Role;

namespace WeddingFB
{
	////小游戏 
	//class SubActBase
	//{
	////操作
	////奖励
	////参数
	//};
	//
	//class SubActBaoWeiXiuQiu : public SubActBase
	//{
	//
	//};

	//场景角色信息
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

		time_t meal__last_fetch_time;//用餐  上传用餐时间

		//公共操作参数[类型]
		int commom_param[COMMON_OPER_TYPE_MAX];

		//活动参数
		// 寻情觅爱
		Posi xun_qing_mi_ai_posi;//随个点(没必要)
		int xun_qing_mi_ai_fetch_flag;
		// 心心相印
		int xin_xin_xiang_yin_fetch_flag;

		// 保卫绣球
		////客户端要转成(npc_id, 状态)下发
		int bao_wei_xiu_qiu_fetch_flag;

		// 枣生桂子
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

	//弹幕
	void SendBarrage(Role *role, const char * chat, unsigned int chat_len);

	//进场
	int CenEnterFB(Role* role);

	void GMNextLink();
private:
	void CheckLink(time_t now_second);
	//结束
	void Finish();

	//领喜糖
	void FetchCandies(Role *role);
	//拜堂参数设置
	void SetRiteParam(Role *role, int param);
	//用餐
	void Meal(Role *role);
	//购买祝福
	void BuyBless(Role *role, int shop_item_index);
	//领取热度奖励
	void FetchHeatReward(Role *role, int reward_seq);
	//增加热度
	void AddHeat(int add_num);

private:
	//小游戏
	// 寻情觅爱
	////领奖
	void FetchXubQingMiAi(Role *role);
	////通知
	void SendXubQingMiAi(Role *role);

	// 心心相印
	////领奖
	void FetchXinXinXiangYin(Role *role);
	////通知
	void SendXinXinXiangYin(Role *role);

	// 保卫绣球
	////领奖
	void FetchBaoWeiXiuQiu(Role *role, int index);
	////通知
	void SendBaoWeiXiuQiu(Role *role);

	// 枣生桂子
	////领奖
	void FetchZaoShengGuiZi(Role *role, int index);
	////通知
	void SendZaoShengGuiZi(Role *role);

private:
	WeddingFB::RoleInfo *GetRoleInfo(int uid);

	int m_is_finish;	//副本完成标记

	WeddingID m_wedding_id;	//婚宴id

	//副本时间 客户端展示用
	time_t m_fb_begin_time;	//副本起始时间
	time_t m_fb_end_time;	//副本结束时间

	int m_link_seq;			//当前环节
	time_t m_link_begin_time;//环节起始时间
	time_t m_link_end_time;	//环节结束时间

	int m_wedding_heat;	//婚宴热度

	time_t m_next_check_time;	//下次检查时间

	WeddingFB::HostInfo host_info_[WeddingFB::HOST_NUM];	//主办者列表

	std::map<int, WeddingFB::RoleInfo> m_join_role_info_map;
};


#endif
