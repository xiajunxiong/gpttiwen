#ifndef __MSG_JIE_YI_HPP__
#define __MSG_JIE_YI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/commonsavedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum JIE_YI_REQ_RET_TYPE
	{
		JIE_YI_REQ_RET_TYPE_CREATE_CONFIRM,		//确认
		JIE_YI_REQ_RET_TYPE_CREATE_CANCEL,			//取消
		JIE_YI_REQ_RET_TYPE_CREATE_NAME_FINISH,		//命名完成
		JIE_YI_REQ_RET_TYPE_LEAVE,				//离开 p1 0是取消，1是离开
		JIE_YI_REQ_RET_TYPE_KICK,				//踢人  p1 role_id
		JIE_YI_REQ_RET_TYPE_INVITE,				//邀请  p1 role_id
		JIE_YI_REQ_RET_TYPE_INVITE_APPLY,			//邀请回复 p1 4211协议zu_id  ,p2 是否同意
		JIE_YI_REQ_RET_TYPE_ZU_INFO,			//请求结义组信息  p1 zu_id,返回4207协议
		JIE_YI_REQ_RET_TYPE_TAN_HE,			//弹劾
		JIE_YI_REQ_RET_TYPE_TAN_HE_VOTE,			//弹劾投票 p1 target_role_id
	};


	class CSJieYiReq				//!< 4200
	{
	public:
		CSJieYiReq();
		MessageHeader header;

		int req_type;
		long long p1;
		int p2;
	}; 

	class CSJieYiCreate				//!< 4201 创建
	{
	public:
		CSJieYiCreate();
		MessageHeader header;

		JieYIZuName jie_yi_name;
	};

	class CSJieYiCreateName				//!< 4202  命名
	{
	public:
		CSJieYiCreateName();
		MessageHeader header;

		int key;
		JieYIZuName jie_yi_name;
	};

	class SCJieYiCreateInfo				//!< 4203   正在创建的组信息
	{
	public:
		SCJieYiCreateInfo();
		MessageHeader header;
		
		int key;
		int is_name_finish;
		JieYIZuName jie_yi_name;
		int role_count;
		JieYiZuCreateRole role_info[JIE_YI_ZU_MAX_ROLE_ITEM_NUM];  //5
	};

	class SCJieYiCreateNameUpdate			//!< 4204  //创建名字更改，其他人收到
	{
	public:
		SCJieYiCreateNameUpdate();
		MessageHeader header;

		int key;
		JieYIZuName jie_yi_name;
	};

	class SCJieYiCreateConfirm			//!< 4205			//创建确认，其他人收到
	{
	public:
		SCJieYiCreateConfirm();
		MessageHeader header;

		int key;
		int role_id[JIE_YI_ZU_MAX_ROLE_ITEM_NUM];		//!< 已经确认的玩家
	};

	class SCJieYiCreateCancel				//!< 4206
	{
	public:
		SCJieYiCreateCancel();			//有人取消操作，其他人收到
		MessageHeader header;

		int type; //!< 0是取消，1是掉线
		int key;
		int role_id;
	};

	class SCJieYiZuInfo		//!< 4207			结义组信息
	{
	public:
		SCJieYiZuInfo();					//!< 角色加入结义组，
		MessageHeader header;

		struct RoleInfo
		{
			JieYiZuRoleBaseInfo role_base_info;
			short is_zu_zhang;			//!< 是否是组长
			short has_fisrt_member_name;		//!< 是否已经改过成员名字，第一次免费
			unsigned int  join_time;			//!< 加入时间
			unsigned int leave_end_time;		//!< 到时间就离开， 申请离开时间, ②点击割袍断义，弹出确认弹窗，若玩家点击确认则进入24小时冷静期
			JieYIZuMemberName member_name;
			unsigned int last_login_time;
			int online_state;
			short is_vote;			//这个成员是否已经投票了
			short tan_he_number_of_votes;			//获得投票次数
		};

		int info_type;  //1是查询
		long long jie_yi_zu_id;
		long long create_time;
		JieYIZuName jie_yi_name;
		JieYIZuText jie_yi_text;
		unsigned int vote_end_time;

		int role_info_count;
		RoleInfo role_info[JIE_YI_ZU_MAX_ROLE_ITEM_NUM];
	};

	class CSJieYiZuMemberName  //!< 4208  成员改名
	{
	public:
		CSJieYiZuMemberName();
		MessageHeader header;

		JieYIZuMemberName member_name;
	};

	class CSJieYiZuText				//!< 4209 组长修改文字
	{
	public:
		CSJieYiZuText();
		MessageHeader header;

		JieYIZuText text;
	};

	class CSJieYiZuName				//!< 4210 组长修改结义名字
	{
	public:
		CSJieYiZuName();
		MessageHeader header;

		JieYIZuName jie_yi_name;
	};

	class SCJieYiInviteRoute				//!< 4211  邀请人时，被邀请的人会收到
	{
	public:
		SCJieYiInviteRoute();
		MessageHeader header;
		
		long long jie_yi_zu_id;
		int role_id;
	};

	class SCJieYiRoleXuanYanList				//!< 4212  自荐宣言
	{
	public:
		SCJieYiRoleXuanYanList();
		MessageHeader header;

		struct XuanYanItem
		{
			JieYiZuRoleBaseInfo role_info;
			JieYIZuXuanYanText text;
		};

		int count;
		XuanYanItem xuan_yan_items[100];
	};

	class SCJieYiZuXuanYanList				//!< 4213  纳新宣言
	{
	public:
		SCJieYiZuXuanYanList();
		MessageHeader header;

		struct XuanYanItem
		{
			long long jie_yi_zu_id;
			JieYIZuName zu_name;
			JieYIZuXuanYanText text;
		};

		int count;
		XuanYanItem xuan_yan_items[100];
	};

	class CSJieYiZuXuanYanReq				//!< 4214 请求宣言
	{
	public:
		CSJieYiZuXuanYanReq();
		MessageHeader header;
		
		short type;     //!< 0是自荐，1是纳新
		short is_delete;     //!< 0添加，1是删除
		JieYIZuXuanYanText text;
	};

	class CSJieYiZuXuanYanReqList				//!< 4215 请求宣言列表
	{
	public:
		CSJieYiZuXuanYanReqList();
		MessageHeader header;

		int type;     //!< 0是自荐，1是纳新
	};

	class SCJieYiZuLeaveNotice		//!< 4216  结义组 离开通知
	{
	public:
		SCJieYiZuLeaveNotice();
		MessageHeader header;

		long long jie_yi_zu_id;
	};

	class SCJieYiZuRoleOnlineStatusNotice		//!< 4217  结义组 成员在线
	{
	public:
		SCJieYiZuRoleOnlineStatusNotice();
		MessageHeader header;

		int role_id;
		unsigned int last_login_time;
		int online_status;
	};
}

#pragma pack(pop)

#endif
