#ifndef __MSG_QING_YUAN_H__
#define __MSG_QING_YUAN_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/commondata.hpp"
#include "servercommon/servercommon.h"
#include "gamedef.h"
#include "servercommon/qingyuandef.h"

#include "servercommon/weddingdef.hpp"

#pragma pack(push) 
#pragma pack(4)
namespace Protocol
{
	//////婚宴
	//玩家操作  4240
	enum WeddingFBReqType
	{
		WEDDING_FB_REQ_TYPE_FB_COMMON = 0,	//副本公共操作   p_1:操作类型  p_2:操作参数

		WEDDING_FB_REQ_TYPE_REGIST = 1,				//婚宴--预约		p_1:预约时间戳
		WEDDING_FB_REQ_TYPE_REGIST_OFF = 2,			//婚宴--解除预约
		WEDDING_FB_REQ_TYPE_INVITE = 3,				//婚宴--邀请		p_1:目标uid
		WEDDING_FB_REQ_TYPE_APPLY = 4,				//婚宴--申请		p_1:婚宴id
		WEDDING_FB_REQ_TYPE_MARK = 5,				//婚宴--申请处理	p_1:目标uid	p_2:0:拒绝	1:同意
	
		WEDDING_FB_REQ_TYPE_REQ_REGIST_LIST = 6,	//婚宴--发送预约列表(并订阅)
		WEDDING_FB_REQ_TYPE_REQ_SUB_REGIST_LIST = 7,//婚宴--预约列表 订阅 p_1:1:订阅 0:取消订阅
		WEDDING_FB_REQ_TYPE_REQ_INVITE_LIST = 8,	//婚宴--发送邀请列表

		WEDDING_FB_REQ_TYPE_OTHER_INFO = 9,			//婚宴--其他信息
		WEDDING_FB_REQ_TYPE_INVITE_SIZE_BUY = 10,	//婚宴--扩展邀请上限
		WEDDING_FB_REQ_TYPE_XUN_YOU = 11,			//预订巡游			p_1:巡游类型

		WEDDING_FB_REQ_TYPE_WEDDING_SHOW_INFO = 12,	//婚宴--展示信息	p_1:婚宴id
		WEDDING_FB_REQ_TYPE_WEDDING_OPEN_INFO = 13,	//婚宴--开启信息

		WEDDING_FB_REQ_TYPE_REQ_SUB_ACT = 100,//副本--小游戏操作	p_1:小游戏类型	p_2:操作参数
	};
	class CSWeddingFBReq
	{
	public:
		CSWeddingFBReq();
		MessageHeader		header;

		int req_type;
		int param_1;
		int param_2;
	};

	//副本信息  4241
	class SCWeddingFBSceneInfo
	{
	public:
		SCWeddingFBSceneInfo();
		MessageHeader		header;

		int m_is_finish;	//副本完成标记

		WeddingID m_wedding_id;	//婚宴id

		unsigned int m_fb_begin_time;	//副本起始时间
		unsigned int m_fb_end_time;		//副本结束时间

		int m_link_seq;		//当前环节
		unsigned int m_link_begin_time;	//环节起始时间
		unsigned int m_link_end_time;	//环节结束时间

		int m_wedding_heat;	//婚宴热度

		//情缘信息 未定
		int xun_you_type;		//巡游类型

		int host_uid[WeddingFB::HOST_NUM];			//主办者uid[2]
		GameName host_name[WeddingFB::HOST_NUM];	//主办者名字[2]
		int host_avatar_type[WeddingFB::HOST_NUM];	//主办者形象[2]
	};

	//副本角色信息  4242
	class SCWeddingFBRoleInfo
	{
	public:
		SCWeddingFBRoleInfo();
		MessageHeader		header;

		unsigned int meal__last_fetch_time;//用餐  上传用餐时间

		//公共操作参数[类型]
		int count;
		int commom_param[WeddingFB::COMMON_OPER_TYPE_MAX];
	};

	//场景广播		4243
	enum WeddingFBSyncType
	{
		WEDDING_FB_SYNC_TYPE_BLESS_SHOP = 0,	//购买祝福商品通知 p:商品index
		WEDDING_FB_SYNC_TYPE_HEAT,				//热度通知 p:当前热度
	};
	class SCWeddingFBSync
	{
	public:
		SCWeddingFBSync();
		MessageHeader		header;

		int sync_type;
		int param;
	};
	//婚宴 小游戏 寻情觅爱   4251
	class SCWeddingSubAct_XubQingMiAi
	{
	public:
		SCWeddingSubAct_XubQingMiAi();
		MessageHeader		header;
	
		int poi_x;
		int poi_y;

		int fetch_flag;
	};

	//婚宴 小游戏 心心相印	4252
	class SCWeddingSubAct_XinXinXiangYin
	{
	public:
		SCWeddingSubAct_XinXinXiangYin();
		MessageHeader		header;

		int fetch_flag;
	};
	//婚宴 小游戏 保卫绣球	4253
	class SCWeddingSubAct_BaoWeiXiuQing
	{
	public:
		SCWeddingSubAct_BaoWeiXiuQing();
		MessageHeader		header;
		
		struct RewardInfo
		{
			RewardInfo()
			{
				npc_id = 0;
				is_fetch = 0;
			}

			int npc_id;		//npc_id
			int is_fetch;	//奖励已领取(已击败)
		};

		int count;
		RewardInfo info_list[WeddingFB::SUB_ACT_BAO_WEI_XIU_QIU_MAX_CFG_SIZE];
	};

	//婚宴 小游戏 枣生桂子	4254
	class SCWeddingSubAct_ZaoShengGuiZi
	{
	public:
		SCWeddingSubAct_ZaoShengGuiZi();
		MessageHeader		header;

		int fetch_flag;
	};

	//婚宴--预约列表		4245
	class SCWeddingRegistList
	{
	public:
		SCWeddingRegistList();
		MessageHeader		header;

		//已预约时间戳  0: 未预约
		unsigned int regist_time;

		//时间列表--已被预约
		int count;
		unsigned int be_regist_time_list[WeddingFB::WEDDING_INFO_SIZE];
	};

	//婚宴--邀请列表		4246
	class SCWeddingInviteList
	{
	public:
		SCWeddingInviteList();
		MessageHeader		header;

		struct InviteInfo
		{
			InviteInfo()
			{
				role_uid = 0;
				memset(role_name, 0, sizeof(role_name));
			}
			int role_uid;
			GameName role_name;
		};

		WeddingID wedding_id;	//婚宴id
		int wedding_begin_time;	//开启时间
		int buy_invite_times;	//已拓展次数

		int host_uid_[WeddingFB::HOST_NUM];

		//被邀请列表
		int is_total;		//1:全部  0:新增
		int count;
		InviteInfo invite_list[WeddingFB::MAX_INVITE_SIZE];
	};

	//婚宴--申请列表		4247
	class SCWeddingApplyList
	{
	public:
		SCWeddingApplyList();
		MessageHeader		header;

		struct ApplyInfo
		{
			ApplyInfo()
			{
				role_uid = 0;
				memset(role_name, 0, sizeof(role_name));
			}
			int role_uid;
			GameName role_name;
		};

		//申请列表
		int is_total;		//1:全部  0:新增
		int count;
		ApplyInfo apply_list[WeddingFB::MAX_APPLY_SIZE];
	};

	//婚宴--通知		4248
	enum WeddingChangeType
	{//																参数1
		WEDDING_CHANGE_TYPE_REGIST_ADD,			//预约列表--新增	已被预约时间
		WEDDING_CHANGE_TYPE_REGIST_REMOVE,		//预约列表--移除	已被预约时间
		WEDDING_CHANGE_TYPE_ROLE_REGIST_CHANGE,	//本人预约时间		预约时间
		WEDDING_CHANGE_TYPE_APPLY_REMOVE,		//申请列表--移除	申请者uid
	};
	class SCWeddingChange
	{
	public:
		SCWeddingChange();
		MessageHeader		header;

		int change_type;
		int param_1;
	};
	//婚宴--基础信息	4249
	class SCWeddingBaseInfo
	{
	public:
		SCWeddingBaseInfo();
		MessageHeader		header;

		unsigned int begin_time;//开启时间

		int buy_invite_times;	//已拓展次数
	};
	//婚宴--开启广播	4250
	class SCWeddingStatusSync
	{
	public:
		SCWeddingStatusSync();
		MessageHeader		header;

		WeddingID wedding_id;			//婚宴id
		unsigned int begin_time;		//开启时间
	};

	//婚宴--展示信息(邀请函)	4244
	class SCWeddingShowInfo
	{
	public:
		enum StatusType
		{
			STATUS_TYPE_DEF = 0,//无
			STATUS_TYPE_APPLIED,//已申请
			STATUS_TYPE_INVITED,//已被邀请
		};
	public:
		SCWeddingShowInfo();
		MessageHeader		header;

		int wedding_id;			//婚宴id
		unsigned int begin_time;//开启时间

		int status;

		//主办者信息
		struct HostShowInfo
		{
			HostShowInfo()
			{
				uid = 0;
				memset(role_name, 0, sizeof(role_name));
				appearance.Reset();
			}

			int uid;
			GameName role_name;		//角色名
			Appearance appearance;	//外观
		};
		int host_count;
		HostShowInfo host_list[WeddingFB::HOST_NUM];
	};

	//巡游 花车 状态信息同步  4450
	class SCXunYouCarStatusInfoSync
	{
	public:
		SCXunYouCarStatusInfoSync();
		MessageHeader		header;

		ObjID obj_id;	//花车obj_id
		short is_stop;	//是等待状态
	};

	enum QING_YUAN_REQ_TYPE
	{
		QING_YUAN_REQ_TYPE_INVITE,		//!< 邀请  p1 role_id ，p2 情缘类型
		QING_YUAN_REQ_TYPE_INVITE_REPLY,		//!< 邀请  p1 role_id,p2 是否同意
		QING_YUAN_REQ_TYPE_GIFT,		//!< 赠送对方礼物 p1 礼物类型
		QING_YUAN_REQ_TYPE_GIFT_FETCH,		//!< 领取礼物首次奖励
		QING_YUAN_REQ_TYPE_GIFT_FETCH_DAY,		//!< 领取礼物每日奖励
		QING_YUAN_REQ_TYPE_SKILL_NOT_EFFECT_FLAG,		//!< p1 设置技能不生效标志，二进制，哪位是1对应第几个技能不生效
		QING_YUAN_REQ_TYPE_LEAVE,		//!< p1 请求对方离婚
		QING_YUAN_REQ_TYPE_LEAVE_REPLY,		//!<回复离婚申请 p1 是否同意
		QING_YUAN_REQ_TYPE_LEAVE_SINGLE,		//!< 离线超过7天，请求离婚 
		QING_YUAN_REQ_TYPE_LEAVE_FORCE,		//!< 强制离婚
		QING_YUAN_REQ_TYPE_LEAVE_CANCEL,		//!< 取消离婚
	};

	class CSQingYuanReq				//!< 4260
	{
	public:
		CSQingYuanReq();
		MessageHeader header;

		int req_type;
		int p1;
		int p2;
	};

	class SCQingYuanInviteRoute				//!< 4261 情缘邀请转发
	{
	public:
		SCQingYuanInviteRoute();
		MessageHeader header;

		int item_type;
		int inviter_role_id;
		int be_inviter_role_id;
	};

	class SCQingYuanInfo		//!< 4262			
	{
	public:
		SCQingYuanInfo();					
		MessageHeader header;

		struct RoleInfo
		{
			int role_id;
			unsigned int single_or_force_leave_end_time;   //时间到，自动解除情缘
			int gift_type;			//开通月卡类型
			int fetch_gift_flag;		// bit 0 是否领取开通奖励，bit 1 是否领取今天奖励
			int gift_type_end_time;		//开通月卡结束时间
			unsigned int skill_no_effect_flag;
		};


		long long qing_yuan_id;
		unsigned int create_time;			//创建时间
		int has_hun_yan;			//是否举办过婚宴
		unsigned int leave_end_time;   //时间到，自动解除情缘
		RoleInfo role_info[QING_YUAN_MAX_ROLE_NUM];
	};


	class SCQingYuanLeaveRoute				//!< 4263 情缘协议离婚转发
	{
	public:
		SCQingYuanLeaveRoute();
		MessageHeader header;
	};

	class SCQingYuanInviteRet				//!< 4264 邀请返回
	{
	public:
		SCQingYuanInviteRet();
		MessageHeader header;

		int target_role_id;
		int is_agree;
	};

	class SCQingYuanLeaveNotice		//!< 4265  情缘离开通知
	{
	public:
		SCQingYuanLeaveNotice();
		MessageHeader header;

		long long qing_yuan_id;
	};

	class SCQingYuanCreateNotice		//!< 4266  情缘创建广播
	{
	public:
		SCQingYuanCreateNotice();
		MessageHeader header;

		int role_id[QING_YUAN_MAX_ROLE_NUM];
	};
}

#pragma pack(pop)

#endif