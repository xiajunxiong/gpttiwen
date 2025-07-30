#ifndef __MSGTEAM_H__
#define __MSGTEAM_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"
#include "servercommon/teamdef.hpp"
#include "gamedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{	
	class CSCreateTeam					//!< 创建队伍 9150
	{
	public:
		CSCreateTeam();
		MessageHeader		header;

		short		limit_level_low;	//!< 最低等级
		short		limit_level_high;	//!< 最高等级
		short		team_type;			//!< 组队目标
		char		is_cross;			//!< 是否跨服 (1:是  0:否)
		char		is_match;			//!< 是否开启匹配 (1:是  0:否)
		char		allow_red_join;		//!< 是否允许红名玩家加入 (1:是  0:否)
		char		ch_reserve;
		short		sh_reserve;
	};

	class CSChangeTeamLimit				//!< 修改队伍信息 9163
	{
	public:
		CSChangeTeamLimit();
		MessageHeader		header;

		short		limit_level_low;	//!< 最低等级
		short		limit_level_high;	//!< 最高等级
		int			team_type;			//!< 组队目标
		char		allow_red_join;		//!< 否允许红名玩家加入
		char		ch_reserve;
		short		sh_reserve;
	};

	class CSDismissTeam					//!< 解散队伍 9155
	{
	public:
		CSDismissTeam();
		MessageHeader		header;

	};

	class SCTeamListAck			//!< 9104  请求队伍列表回复
	{
	public:
		SCTeamListAck();
		MessageHeader	header;

		int				count;	//!< 队伍列表数量
		TeamItem		team_list[MAX_TEAM_LIST_NUM]; //!< 队伍列表
	};

	class SCChivalrousTeamList	//!< 9112  侠义值队伍列表返回
	{
	public:
		SCChivalrousTeamList();
		MessageHeader header;

		struct ChivalrousTeam
		{
			int add_chivalrous_value;	// 侠义值
			TeamItem team_info;			// 队伍信息
		};

		int				count;	//!< 队伍列表数量
		ChivalrousTeam  team_list[MAX_TEAM_LIST_NUM]; //!< 队伍列表
	};

	class CSChivalrousTeamListReq	//!< 9201 请求侠义值队伍列表
	{
	public:
		CSChivalrousTeamListReq();
		MessageHeader header;
	};

	class CSUnfullTeamListByTypeReq			//!< 9165 获取组队列表 
	{
	public:
		CSUnfullTeamListByTypeReq();
		MessageHeader		header;

		int				team_type;
	};

	struct TeamMemberInfo
	{
		TeamMemberInfo() : real_member_idx(0), show_member_idx(0),
		is_partner(0), level(0), uid(0), profession(0), is_follow(0), 
		is_online(0), pos_index(0), curr_hp(0), max_hp(0), curr_mp(0), 
		max_mp(0), partner_skin_id(0), pet_id(0), pet_level(0),
		pet_str_level(0), pet_curr_hp(0), pet_max_hp(0), pet_max_mp(0),
		sub_partner_id(0), sub_partner_cur_hp(0), sub_partner_max_hp(0),
		sub_partner_cur_mp(0), sub_partner_max_mp(0)
		{
			memset(member_name, 0, sizeof(member_name));
			appearance.Reset(); 
		}

		int real_member_idx;//!< 真实的index
		int show_member_idx;//!< 显示的index
		int is_partner;		//!< 是否为伙伴
		int level;			//!< 等级
		int uid;			//!< 角色uid 或者 伙伴id
		int profession;		//!< 职业
		char is_follow;		//!< 跟随
		char is_online;		//!< 是否在线
		short pos_index;	//!< 组队站位[0,5)
		int curr_hp;		//!< 血量
		int max_hp;			//!< 血量
		int curr_mp;		//!< 魔力
		int max_mp;			//!< 魔力
		int partner_skin_id;//!< 伙伴皮肤

		int pet_id;
		int pet_level;
		int pet_str_level;		
		int pet_curr_hp;	//!< 宠物血量
		int pet_max_hp;		//!< 宠物血量
		int pet_curr_mp;	//!< 宠物魔力
		int pet_max_mp;		//!< 宠物魔力	
		int pet_skin_id;	//!< 宠物外观

		int sub_partner_id;
		int sub_partner_cur_hp;
		int sub_partner_max_hp;
		int sub_partner_cur_mp;
		int sub_partner_max_mp;

		GameName member_name;
		Appearance appearance; //!< 玩家外观信息
	};

	class SCTeamInfo					//!< 发送队伍信息给玩家 9100
	{
	public:
		SCTeamInfo();
		MessageHeader		header;

		int team_id;
		short			limit_level_low;
		short			limit_level_high;
		int				team_type;
		short			check_type;					//!< 0为拒绝所有申请 1为需要审批 2为自动加入
		char			is_cross;					//!< 0：非跨服  1：跨服
		char			allow_red_join;				//!< 0：不允许红名加入  1：允许红名加入

		int				leader_index;
		int				member_count;
		TeamMemberInfo	member_list[MAX_TEAM_MEMBER_NUM];
	};
	UNSTD_STATIC_CHECK(MAX_TEAM_MEMBER_NUM == 5);

	class SCTeamEnterRoute			//!<9108 进入副本请求转发
	{
	public:
		SCTeamEnterRoute();
		MessageHeader		header;

		int enter_type;
		int enter_type_param;
		unsigned int agree_enter_end_time;
		int param1;
		int param2;
		int param3;
	};

	class SCTeamEnterFbInfo			//!< 9109 进入副本队员确定信息
	{
	public:
		SCTeamEnterFbInfo();
		MessageHeader		header;

		short member_index;
		short is_determine;
	};

	class CSTeamEnterFbRet		//!< 9176 回复进入副本请求
	{
	public:
		CSTeamEnterFbRet();
		MessageHeader		header;

		int is_determine;
	};


	class SCOutOfTeam				//!< 9101  通知你已经没有队伍了 被踢了或者自己退了
	{
	public:
		SCOutOfTeam();
		MessageHeader		header;
	};

	class SCInviteNotice			//!< 9102 你收到了组队邀请
	{
	public:
		SCInviteNotice();
		MessageHeader		header;

		int xia_yi_zhi;
		int team_index;
		int team_type;
		MsgRoleBaseInfo inviter_info;
	};
	class SCJoinReq				//!< 9103 你收到了入队申请
	{
	public:
		SCJoinReq();
		MessageHeader		header;

		int req_index;
		int uid;
		int level;
		int profession;
		long long avatar_timestamp;
		unsigned int req_tmestamp;
		GameName name;
		short avatar_type;
		short headshot_id;
	};

	class CSInviteJoinReq		//!< 9151 邀请加入队伍
	{
	public:
		CSInviteJoinReq();
		MessageHeader		header;

		int uid;			//!< 
	};

	class CSExitTeamReq		//!< 9158 退队
	{
	public:
		CSExitTeamReq();
		MessageHeader		header;
	};

	class CSKickOutOfTeamReq		//!< 9156 将成员踢出队伍
	{
	public:
		CSKickOutOfTeamReq();
		MessageHeader		header;

		int kick_index;
	};

	class CSJoinTeamReq		//!< 9154 申请入队
	{
	public:
		CSJoinTeamReq();
		MessageHeader		header;

		int team_index;
		int is_apply_cross_team; //!< 是否申请跨服队伍 [0:否 1:是]
	};

	class CSJoinTeamReqRet		//!< 9153 审批入队申请
	{
	public:
		CSJoinTeamReqRet();
		MessageHeader		header;

		int uid;				//!< 申请人uid
		int is_agree;			//!< 0 为拒绝入队申请 其他为同意
	};

	class CSInviteJoinRet		//!< 9152 答复入队邀请
	{
	public:
		CSInviteJoinRet();
		MessageHeader		header;

		int uid;				//!< 邀请人uid
		int is_agree;			//!< 0 为拒绝入队邀请 其他为同意
	};

	class SCTeamMatchInfo		//!< 9169
	{
	public:
		SCTeamMatchInfo();
		MessageHeader		header;

		int is_match;		//!< 0为未匹配
	};

	class CSTeamMatchOp			//!< 9170 匹配请求
	{
	public:
		CSTeamMatchOp();
		MessageHeader		header;

		int match_type;			//!< 要匹配的组队类型
		short op_type;			//!< 操作类型   0：取消匹配  1：开始匹配
		char is_cross;			//!< 是否跨服匹配  [0:否 1:是] 
		char is_auto_create;	//!< 一定时间匹配不到人是否自动创建队伍 [0:否 1:是] 
	};

	class CSChangeTeamLeader			//!< 9157 换队长
	{
	public:
		CSChangeTeamLeader();
		MessageHeader		header;

		int member_index;
	};

	class CSChangeMustCheck			//!< 9160 改变队伍加入是否是要队长通过
	{
	public:
		CSChangeMustCheck();
		MessageHeader		header;

		int check_type;				//!< 0为拒绝所有申请 1为需要审批 2为允许自动加入
	};

	class CSTemporarilyPartReq			//!< 9171 暂离
	{
	public:
		CSTemporarilyPartReq();
		MessageHeader		header;

	};

	class CSTeamInvitationReq			//!< 9172 组队邀请
	{
	public:
		CSTeamInvitationReq();
		MessageHeader		header;

		int uid;
		int invite_type;
	};

	enum TEAM_INVITATION_NOTICE_TYPE
	{
		TEAM_INVITATION_NOTICE_TYPE_DEFAULT,
		TEAM_INVITATION_NOTICE_TYPE_UPDATE,
		TEAM_INVITATION_NOTICE_TYPE_NOT_EXIST,
	};

	class SCTeamInvitationNotice			//!< 9173 组队邀请通知
	{
	public:
		SCTeamInvitationNotice();
		MessageHeader		header;
	
		int notice_type;
		short is_from_cross;
		short invite_type;
		int xia_yi_zhi;
		int team_index;
		int team_type;		
		int team_member_num;
		MsgRoleBaseInfo inviter_info; 
	};

	class CSTeamInvitationRet			//!< 9174 组队邀请回复
	{
	public:
		CSTeamInvitationRet();
		MessageHeader		header;

		int uid;
		int is_accept;					//!< 0拒绝 1 接受
		int is_from_cross;				//!< 由9173协议下发，原样发过来即可
		int team_index;					//!< 由9173协议下发，原样发过来即可
	};

	class CSTeamRecruit		//!< 9175 招募
	{
	public:
		CSTeamRecruit();
		MessageHeader header;
	};

	class CSTeamSwitchToCross	//!< 9177 队伍切换到跨服
	{
	public:
		CSTeamSwitchToCross();
		MessageHeader header;
	};

	class SCTeamChannelInfo		//!< 9178 组队频道信息
	{
	public:
		SCTeamChannelInfo();
		MessageHeader header;

		enum REASON_TYPE					//!< 下发原因
		{
			REASON_TYPE_NORMAL = 0,			//!< 0 正常情况（整个列表下发）
			REASON_TYPE_INFO_CHANGE = 1,	//!< 1 信息改变
			REASON_TYPE_ADD_TEAM = 2,		//!< 2 新增队伍
			REASON_TYPE_REMOVE_TEAM = 3,	//!< 3 从列表中去掉队伍
		};

		short reason;
		short count;

		TeamItem team_list[MAX_TEAM_CHANNEL_ITEM_NUM];
	};

	class CSTeamSwitchPosition	//!< 交换站位 9179
	{
	public:
		CSTeamSwitchPosition();
		MessageHeader header;

		short pos1;			//!< 交换的位置1
		short pos2;			//!< 交换的位置2
	};

	class SCTeamMemberAppearanceChange		//!< 9180 成员外观变化通知
	{
	public:
		SCTeamMemberAppearanceChange();
		MessageHeader header;

		int member_index;					
		Appearance new_appearance;
	};

	class SCTeamPosInfo			//!< 9110 队伍站位信息
	{
	public:
		SCTeamPosInfo();
		MessageHeader header;

		struct MemberPos
		{
			int id;				//!< 玩家或伙伴的id  为0则是无效数据
			short is_partner;	//!< 是否伙伴
			short reserve_sh;	//!< 保留字
		};

		MemberPos pos_list[MAX_TEAM_MEMBER_NUM];
	};

	class SCTeamLeaderChangeNoticeArea		// 9111 队长改变通知附近玩家
	{
	public:
		SCTeamLeaderChangeNoticeArea();
		MessageHeader header;

		ObjID change_objid;		// 改变的对象ID		
		short is_leader;		// 是否队长[0:否 1:是]
	};

	class SCTeamRemoveApply		// 9181 队伍审批列表删除一个玩家的申请
	{
	public:
		SCTeamRemoveApply();
		MessageHeader header;

		int remove_uid;
	};

	class SCApplyList			//!< 9182 下发申请列表（转移队长时下发）
	{
	public:
		SCApplyList();
		MessageHeader		header;

		static const int MAX_APPLY_LIST_SIZE = 5;

		struct ApplyInfo
		{
			int req_index;
			int uid;
			int level;
			int profession;
			long long avatar_timestamp;
			unsigned int req_tmestamp;
			GameName name;
			short avatar_type;
			short headshot_id;
		};
		
		int count;
		ApplyInfo apply_list[MAX_APPLY_LIST_SIZE];
	};

	class SCTeamVoteStart					//!< 投票开始通知 9183
	{
	public:
		SCTeamVoteStart();
		MessageHeader header;

		unsigned int vote_end_timestamp;	//!< 投票结束时间戳
		GameName campaigner_name;			//!< 发起者名称
	};

	class CSTeamLaunchLeaderVote //!< 发起投票 9184
	{
	public:
		CSTeamLaunchLeaderVote();
		MessageHeader header;
	};

	class CSTeamVote	//!< 投票 9185
	{
	public:
		CSTeamVote();
		MessageHeader header;

		int is_yes;		//!< [0:不同意  1:同意]
	};

	class SCUpdateMemberInfo		//!< 9186 同步成员信息
	{
	public:
		SCUpdateMemberInfo();
		MessageHeader header;

		int uid;					//!< 角色uid 或者 伙伴id
		int is_partner;				//!< 0:主角  1:伙伴
		int hp;						//!< 血量
		int max_hp;
		int mp;						//!< 魔力
		int max_mp;					//!< 魔力
		int skin_id;				//!< 皮肤ID  伙伴时有效
		int level;					//!< 等级
	};

	enum TEAM_LEADER_NPC_REQ_TYPE
	{
		TEAM_LEADER_NPC_REQ_TYPE_LOAD = 0,//!< 初始化
		TEAM_LEADER_NPC_REQ_TYPE_ADD = 1,//!< 添加
		TEAM_LEADER_NPC_REQ_TYPE_DELETE = 2,//!< 删除
	};
	const static int TEAM_LEADER_NPC_SEQ_COUNT = 150;// 队长最大可视npc下发列表
	class CSTeamLeaderNpcReq		//!< 9187 队长可视NPC操作
	{
	public:
		CSTeamLeaderNpcReq();
		MessageHeader header;

		int type;
		int npc_count;
		int npc_seq_list[TEAM_LEADER_NPC_SEQ_COUNT];
	};

	class SCTeamLeaderNpcInfo		//!< 9188 队长可视NPC下发
	{
	public:
		SCTeamLeaderNpcInfo();
		MessageHeader header;

		int type;
		int npc_count;
		int npc_seq_list[TEAM_LEADER_NPC_SEQ_COUNT];
	};

	class SCUpdateMemberPetHPMP		//!< 9189 队员出战宠物信息
	{
	public:
		SCUpdateMemberPetHPMP();
		MessageHeader header;

		int uid;					//!< 角色uid 
		int pet_id;
		int pet_level;
		int pet_str_level;
		int curr_hp;					//!< 血量
		int max_hp;					//!< 血量
		int curr_mp;					//!< 魔力
		int max_mp;					//!< 魔力
		int skin_id;				//!< 皮肤
	};

	class SCTeamMemberAddHpMpNotice		//!< 9190 队员帮助加血加蓝通知
	{
	public:
		SCTeamMemberAddHpMpNotice();
		MessageHeader header;

		short from_team_member_index;
		short is_to_use_pet;		
		int partner_id;
		short item_id;
		short use_gold_recovery;  //!< 1是恢复血，2是恢复蓝
	};

	class SCTeamSimpleInfoChangeNotice	//!< 9191 队伍简易信息更变通知
	{
	public:
		SCTeamSimpleInfoChangeNotice();
		MessageHeader header;

		short			limit_level_low;
		short			limit_level_high;
		int				team_type;
		short			check_type;					//!< 0为拒绝所有申请 1为需要审批 2为自动加入
		char			is_cross;					//!< 0：非跨服  1：跨服
		char			allow_red_join;				//!< 0：不允许红名加入  1：允许红名加入
	};

	struct ChangeTeamMemberBaseInfo
	{
		int level;				//!< 等级
		int uid;				//!< 角色uid
		int profession;			//!< 职业

		GameName member_name;	//!< 名字
		Appearance appearance;	//!< 玩家外观信息
	};

	class SCTeamMemberBaseInfoChangeNotice	//!< 9192 队员信息变化通知
	{
	public:
		SCTeamMemberBaseInfoChangeNotice();
		MessageHeader header;

		ChangeTeamMemberBaseInfo member_info;
	};

	class CSTeamLeaderSummonMember	//!< 9193 召回请求
	{
	public:
		CSTeamLeaderSummonMember();
		MessageHeader header;

		int summon_role_id;			//!< 要召回的玩家ID
	};

	class SCTeamLeaderSummonMemberNotice	//!< 9194 通知队员：队长需要召回你
	{
	public:
		SCTeamLeaderSummonMemberNotice();
		MessageHeader header;
	};

	class CSTeamMemberReplyLeaderSummon	//!< 9195 队员回复召回
	{
	public:
		CSTeamMemberReplyLeaderSummon();
		MessageHeader header;

		int is_return;					//!< 是否回归 0：不回归   1：回归
	};

	enum TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE
	{
		TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_WORLD_BOSS1,			//世界boss1 次数不足
		TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_WORLD_BOSS2,			//世界boss2 次数不足
		TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_LEVEL,					//队员等级不足  p1 目标等级
		TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_ITEM,					//队员物品p1 达不到目标数p2，p1 item_id, p2 数量
		TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_JOIN_TIMES,				//xxx已经超过参加次数
		TEAM_MEMBER_INCOMPATIBLE_OVER_MEMBER_NUM,					// 队伍人数太多  p1 允许最大人数
		TEAM_MEMBER_INCOMPATIBLE_NOT_ENOUGH_MEMBER_NUM,			// 队伍人数不足 p1 需要人数
		TEAM_MEMBER_INCOMPATIBLE_NOT_TU_DI,							// 队友(team_member_index)不是你的徒弟
		TEAM_MEMBER_INCOMPATIBLE_BRAVE_GROUND_CANT_NEXT,			// 暂时无法进入，将在世界等级达到 p1 级后开放
		TEAM_MEMBER_INCOMPATIBLE_HAS_JIE_YI_ZU,				//xxx已经超过参加次数
		TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_WORLD_BOSS3,			//世界boss3(天神降临) 次数不足
		TEAM_MEMBER_INCOMPATIBLE_NO_TASK_RECORD,					//队员X param1任务 未达成
	};

	class TeamMemberIncompatibleNotice	//!< 队友不符合条件通知 9196
	{
	public:
		TeamMemberIncompatibleNotice();
		MessageHeader header;

		int notice_type;
		int team_member_index;			//队友index
		int param1;
		int param2;
	};

	class CSLeaderPartnerInfo	//!< 请求队长的伙伴信息 9197
	{
	public:
		CSLeaderPartnerInfo();
		MessageHeader header;
	};

	class SCLeaderPartnerInfo	//!< 返回队长的伙伴信息 9198
	{
	public:
		SCLeaderPartnerInfo();
		MessageHeader header;

		struct Info
		{
			int partner_id;
			int maxhp;
			int curhp;
			int maxmp;
			int curmp;
			int member_index;
		};

		int num;
		Info info[MAX_TEAM_MEMBER_NUM];
	};

	class CSLeaderCheckMemberItem	//!< 9199 队长请求检查队员特有道具信息
	{
	public:
		CSLeaderCheckMemberItem();
		MessageHeader header;

		int item_id;
		int item_num;
	};

	class SCLeaderCheckMemberItemRet	//!< 9200 队长请求检查队员特有道具信息返回
	{
	public:
		SCLeaderCheckMemberItemRet();
		MessageHeader header;
		struct Member_item_check
		{
			Member_item_check() :member_idx(-1), item_num(0) {}
			int member_idx;			//!< 队员index
			int item_num;			//!< 拥有道具数量
		};

		int check_num;				//!< 检查队员数量
		Member_item_check member_info[MAX_TEAM_MEMBER_NUM];
	};

	class CSReqJoinTeamInfo	//!< 9202		请求邀请队伍信息
	{
	public:
		CSReqJoinTeamInfo();
		MessageHeader header;
		
		short is_from_cross;
		short invite_type;
		int team_index;
		int inviter_uid;
	};

	class SCTeamRedNameNotifyArea			//!<9203 队伍是否红名，通知附近的人
	{
	public:
		SCTeamRedNameNotifyArea();
		MessageHeader header;

		int is_red;
		int team_id;
	};

	class CSTeamLeaderEjectWindowsReq
	{
	public:
		CSTeamLeaderEjectWindowsReq();
		MessageHeader header;

		int windows_type;//弹窗类型--客户端自己决定
	};

	class SCTeamLeaderEjectWindowsInfo
	{
	public:
		SCTeamLeaderEjectWindowsInfo();
		MessageHeader header;

		int windows_type;
	};

	class SCTeamRoleRecruitInfo	//!< 9206 队伍弹窗
	{
	public:
		SCTeamRoleRecruitInfo();
		MessageHeader header;

		int team_id;			// 队伍ID
		int team_type;			// 队伍目标
		GameName leader_name;	// 队长名字
		short level_low;		// 最低等级
		short level_high;		// 最高等级
		short cur_member_num;	// 当前成员数
		short max_member_num;	// 最大成员数	
	};

	class CSTeamRoleRecruitAnswer	//!< 9207 回应招募
	{
	public:
		CSTeamRoleRecruitAnswer();
		MessageHeader header;

		enum ANSWER_TYPE
		{
			ACCEPT = 0,				// 接受
			REFUSE_MANUALLY = 1,	// 手动拒绝
			AUTO_REFUSE = 2,		// 自动拒绝
		};

		int answer_type;
	};
}

#pragma pack(pop)

#endif

