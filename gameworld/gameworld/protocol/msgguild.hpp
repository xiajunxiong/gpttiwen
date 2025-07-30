#ifndef __MSGGUILD_HPP__
#define __MSGGUILD_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/serverdef.h"
#include "servercommon/guilddef.hpp"
#include "servercommon/commondata.hpp"
#include "servercommon/activitydef.hpp"
#include "servercommon/roleguildinborndef.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	class SCCreateRet	//!< 创建家族 9819
	{
	public:
		SCCreateRet();
		MessageHeader header;
		enum TYPE
		{
			CREATE = 1,
		};

		int param;
	};

	class CSCreateGuild		//!< 创建家族 9800
	{
	public:
		CSCreateGuild();
		MessageHeader header;

		GuildName name;
		GuildNotice notice;
		GuildBanner banner;
	};

	class CSQuitGuild		//!< 退出家族 9801
	{
	public:
		CSQuitGuild();
		MessageHeader header;

		GuildID guild_id;
	};

	class CSApplyJoinGuildReq		//!< 申请加入家族 9805
	{
	public:
		CSApplyJoinGuildReq();
		MessageHeader header;

		GuildID guild_id;
		int one_click;			//!< 是否一键申请
	};

	class CSApplyJoinGuildAck		//!< 审批申请 9806
	{
	public:
		CSApplyJoinGuildAck();
		MessageHeader header;

		GuildID guild_id;
		int uid;
		int result;				//!< 0为拒绝
	};

	class SCQuitGuild		//!< 9802 通知没有家族了
	{
	public:
		SCQuitGuild();
		MessageHeader header;
		enum REASON_TYPE
		{
			QUIT = 1, //!< 主动退出
			EXPEL_OUT, //!< 被踢了
		};

		int reason;
	};

	class SCGuildBaseInfo : public IMessageSerializer	//!<9803 家族基础信息
	{
	public:
		SCGuildBaseInfo();
		virtual ~SCGuildBaseInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteStrN(patriarch_name, sizeof(GameName), buffer, buffer_size, out_length);
			result = result && WriteUInt(patriarch_last_login_time, buffer, buffer_size, out_length);
			result = result && WriteUInt(patriarch_tan_he_start_time, buffer, buffer_size, out_length);
			result = result && WriteShort(can_tan_he_patriarch, buffer, buffer_size, out_length);
			result = result && WriteShort(is_join_tan_he_patriarch, buffer, buffer_size, out_length);
			result = result && WriteStrN(guild_name, sizeof(GuildName), buffer, buffer_size, out_length);
			result = result && WriteStrN(guild_notice, sizeof(GuildNotice), buffer, buffer_size, out_length);

			result = result && WriteInt(guild_id, buffer, buffer_size, out_length);
			result = result && WriteInt(level, buffer, buffer_size, out_length);
			result = result && WriteLL(exp, buffer, buffer_size, out_length);

			result = result && WriteUInt(create_time, buffer, buffer_size, out_length);
			result = result && WriteUInt(join_time, buffer, buffer_size, out_length);
			result = result && WriteInt(is_need_approval, buffer, buffer_size, out_length);
			result = result && WriteInt(guild_buff_rank, buffer, buffer_size, out_length);

			result = result && WRITE_STRN(guild_banner, sizeof(guild_banner));

			return result;
		}

		GameName patriarch_name;		//!<族长名
		unsigned int patriarch_last_login_time; //!<族长登录时间
		unsigned int patriarch_tan_he_start_time; //!<弹劾时间,不为0就是有人开始了弹劾，
		short can_tan_he_patriarch;		//是否能弹劾
		short is_join_tan_he_patriarch;		//是否参加了族长弹劾
		GuildName guild_name;			//!<家族名
		GuildNotice guild_notice;		//!<家族公告
		int guild_id;					//!<家族id
		int level;						//!<等级
		long long exp;					//!<经验

		unsigned int create_time;		//!<家族创建时间
		unsigned int join_time;			//!<加入家族时间
		int is_need_approval;					//是否需要批处申请加入工会的人
		int guild_buff_rank;			//工会buff

		GuildBanner guild_banner;
	};

	struct MemberInfo
	{
		GameName name;			//!< 成员名
		int uid;				//!< 
		char online;			//!< 1：在线
		char post;				//!< 职务
		short level;			//!< 等级
		int top_level;			//!< 巅峰
		int m_profession;		//!< 职业
		int gongxian;			//!< 贡献
		int total_gongxian;		//!< 总贡献
		long long join_time;	//!< 加入家族的时间
		long long last_online_timestamp;		//!< 最后在线时间戳
		Appearance appearance;	//!< 角色形象
		int seven_active;		//!< 最近七日总活跃度
		unsigned int seven_active_times;	//!< 上一次家族活跃度变更时间戳
		short role_guild_task_finish_times;		//完成次数
		char role_guild_task_is_mail_notice;		//邮件通知
		char role_guild_task_is_solicit;		//是否征集
		int role_guild_task_solicit_task_id;		//征集任务id
		short role_guild_task_solicit_num;		//征集进度
		short role_guild_task_solicit_fetch_num;	//征集进度是否领取
	};

	class SCGuildMemberInfo		//!<9804 家族成员信息
	{
	public:
		SCGuildMemberInfo();
		MessageHeader header;
		
		int member_count;		//!< 成员信息的数组大小
		MemberInfo member_list[GUILD_MAX_MEMBER_COUNT];
	};

	class CSGuildExpelOutReq		//!<9807 踢出家族
	{
	public:
		CSGuildExpelOutReq();
		MessageHeader header;

		GuildID guild_id;
		int expel_uid;
	};

	class CSSetGuildNoticeReq		//!< 9808 修改仙盟公告
	{
	public:
		CSSetGuildNoticeReq();
		MessageHeader header;

		GuildID guild_id;
		GuildNotice notice;
	};

	class CSGetGuildList			//!< 请求家族信息 9809
	{
	public:
		CSGetGuildList();
		MessageHeader header;

		enum REQ_TYPE
		{
			GET_GUILD_LIST =1,						//!< 获取家族列表
			GET_GUILD_NOTICE,						//!< 获取家族通知 param 家族id
			GET_GUILD_BASE_INFO,					//!< 获取家族基本信息 param 家族id
			GET_GUILD_MEMBER_LIST,					//!< 获取家族成员列表
			GET_GUILD_APPLY_LIST,					//!< 获取申请列表
			GET_GUILD_BUILD_INFO,					//!< 获取建造信息 param 家族id
			GET_GUILD_TASK_INFO,					//!< 获取家族任务信息
			GET_GUILD_TASK_RANK_INFO,				//!< 获取家族任务排行
			GET_GUILD_GATHER_DREAM_LIST,			//!< 获取家族集愿信息列表
			GET_GUILD_PUBLISH_GATHER_DREAM,			//!< 发布家族集愿信息  param item_id param1 gather_dream_type
			GET_GUILD_GIFT_GATHER_DREAM,			//!< 赠予集愿碎片   param role_uid
			GET_GUILD_FETCH_GATHER_DREAM,			//!< 领取集愿碎片
			GET_GUILD_GIFT_GATHER_DREAM_LIST,		//!< 获取玩家的赠予列表
			GET_GUILD_GET_GATHER_DREAM_LIST,		//!< 获得玩家的受赠列表
			GET_GUILD_ROLE_GIFT_GATHER_DREAM_LIST,	//!< 获得玩家今天赠送信息
			GET_GUILD_ROLE_CAN_PULISH_LIST,			//!< 获得玩家可发布的集愿信息列表
			GET_GUILD_ROLE_SET_PUBLISH_SEND,		//!< 设置玩家发布集愿是否发送消息至家族频道  param:0 发送 1:不发送
			TAN_HE_ZU_ZHANG,						//!< 弹劾族长
			REFUSE_GUILD_RECOMMEND,					//!< 拒绝工会推荐
			REQ_GUILD_RECOMMEND,					//!< 请求工会推荐
		};

		int req_type;
		int param;
		int param1;
	};

	static const int MAX_GUILD_COUNT = 100;
	struct GuildNodeInfo
	{
		int guild_id;				//!< 家族id
		int patriarch_id;			//!< 族长id
		int level;					//!< 等级
		int member_count;			//!< 成员数量
		GuildName guild_name;		//!< 家族名
		GameName patriarch_name;	//!< 族长名
		GuildBanner guild_banner;	//!< 族徽
		int seven_day_active;		//!< 家族近7日总活跃
		unsigned int seven_day_active_last_change_times;		//!< 家族近7日总活跃度最后变更时间
		int extra_member_limit;		//!< 厢房额外人数
	};
	class SCSendGuildList			//!< 下发家族列表 9810
	{
	public:
		SCSendGuildList();
		MessageHeader header;

		int count;
		GuildNodeInfo guild_list[MAX_GUILD_COUNT];
	};
	
	class SCSendGuildNotic			//!< 下发家族通知 9811
	{
	public:
		SCSendGuildNotic();
		MessageHeader header;

		int guild_id;
		GuildNotice notice;
	};

	class CSGuildPetTrainingUp			//!< 强化宠物训练 9812
	{
	public:
		CSGuildPetTrainingUp();
		MessageHeader header;

		int training_type;			//!< 技能类型
	};

	class CSGuildPetTraining			//!< 宠物训练 9813
	{
	public:
		CSGuildPetTraining();
		MessageHeader header;

		int pet_index;				//!< 宠物的index
		int places_index;			//!< 训练格子的index[0,2]
	};

	class CSGetGuildTrainingInfo			//!< 请求训练信息 9814
	{
	public:
		CSGetGuildTrainingInfo();
		MessageHeader header;

	};

	class SCSendGuildTrainingInfo			//!< 发送训练信息 9815
	{
	public:
		SCSendGuildTrainingInfo();
		MessageHeader header;

		int guild_training_level[PET_TRAINING_TYPE_MAX];				//!< 强化训练等级 PET_TRAINING_TYPE_MAX = 4
		GuildTrainingPetParam training_pet_index[GUILD_PET_TRAINING_PET_MAX_COUNT];		//!< 在训练的宠物的index  GUILD_PET_TRAINING_PET_MAX_COUNT = 3
	};

	
	static const int APPLY_COUNT_MAX = 100;
	class SCSendGuildApplyList			//!< 下发申请列表 9816
	{
	public:
		SCSendGuildApplyList();
		MessageHeader header;

		int count;
		GuildApplyUserInfo apply_list[APPLY_COUNT_MAX];		//!<申请列表
	};

	class CSGuildAbdicationOp			//!< 转让族长 9820
	{
	public:
		CSGuildAbdicationOp();
		MessageHeader header;

		GuildID guild_id;
		int uid;
	};

	class CSGuildImpeachmentOp			//!< 弹劾 9821
	{
	public:
		CSGuildImpeachmentOp();
		MessageHeader header;

		GuildID guild_id;
	};

	class CSGuildAppointmentOp			//!< 任命 9822
	{
	public:
		CSGuildAppointmentOp();
		MessageHeader header;

		GuildID guild_id;
		int uid;
		int post;
	};

	class CSGuildShopBuy			//!< 商店购买 9823
	{
	public:
		CSGuildShopBuy();
		MessageHeader header;

		GuildID guild_id;
		int shop_seq;
		int count;
	};


	class SCGuildShopInfo			//!< 商店信息 9824
	{
	public:
		SCGuildShopInfo();
		MessageHeader header;
		struct ShopNode
		{
			short seq;
			short buy_count;
		};

		int count;
		ShopNode shop_buy_list[GUILD_SHOP_COUNT_MAX];		//!< 今天购买了多少次
	};

	class SCGuildPetTrainingExp			//!< 宠物训练结算经验显示 9825
	{
	public:
		SCGuildPetTrainingExp();
		MessageHeader header;

		int pet_index;
		int time;
		int exp;
	};


	class CSGuildRenameReq		//!< 9826 家族改名
	{
	public:
		CSGuildRenameReq();
		MessageHeader header;

		GuildName new_name;
	};

	class CSGuildInvitationReq			//!< 家族邀请9829
	{
	public:
		CSGuildInvitationReq();
		MessageHeader header;

		int uid;				//!< 邀请谁加入
	};

	class SCGuildInvitationNotice			//!< 收到家族邀请 9830
	{
	public:
		SCGuildInvitationNotice();
		MessageHeader header;

		GameName inviter_name;
		GuildName guild_name;
	};

	class SCGuildBossInfo				//!< 家族boss信息返回 9831
	{
	public:
		SCGuildBossInfo();
		MessageHeader header;

		int guild_boss_chapter_id;		//!< 今天打的章节

		int boss_participate_reward_fetch_flag;	//!< 参与奖励领取标记 1:已领取  0:未领取
	};


	enum GUILD_BOSS_REQ_TYPE
	{
		GUILD_BOSS_REQ_TYPE_INFO,						//信息
		GUILD_BOSS_REQ_TYPE_RANK,						//伤害排行榜
		GUILD_BOSS_REQ_TYPE_SET_CHAPTER,				//设置明天打的章节 ,p1 章节
		GUILD_BOSS_REQ_TYPE_FETCH_CHAPTER_REWARD,		//领取章节奖励
		GUILD_BOSS_REQ_TYPE_GUILD_RANK,					//各个家族之间的家族试炼进度排行
	};

	class CSGuildBossReq				//!< 家族boss信息请求 9832
	{
	public:
		CSGuildBossReq();
		MessageHeader header;

		int req_type;
		int p1;
	};

	class CSGuildLevelUpReq				//!< 家族升级请求 9833
	{
	public:
		CSGuildLevelUpReq();
		MessageHeader header;

		GuildID guild_id;
	};

	class SCPlayerApplyList
	{
	public:
		SCPlayerApplyList();			//!< 我的申请列表 9834
		MessageHeader header;

		int apply_count;
		int guild_id_list[PLAYER_APPLY_COUNT_MAX];
	};

	class CSInvitationReply
	{
	public:
		CSInvitationReply();			//!< 9835 家族邀请回复
		MessageHeader header;

		int result;		// 回复结果：0 - 拒绝；1 - 同意；
	};

	class SCGuildNeedApproveInfo   //!< 9837 工会是否需要批准加入信息
	{
	public:
		SCGuildNeedApproveInfo();
		MessageHeader header;

		int is_need_approve;
	};

	class CSSetGuildNeedApproveReq
	{
	public:
		CSSetGuildNeedApproveReq();			//!< 9838 设置工会是否需要批准加入信息
		MessageHeader header;

		int is_need_approve;
	};

	//////////////////////////////////// 家族任务 ////////////////////////////////////
	class SCGuildWeekTaskAllInfor
	{
	public:
		SCGuildWeekTaskAllInfor();		//!< 1930 家族任务 - 所有信息
		MessageHeader header;

		int cur_finish_times;	//!< 本家族本周当前完成任务的次数
		int cur_task_id;		//!< 当前的家族任务ID
		int cur_task_prog;		//!< 当前的任务进度
	};

	class SCGuildWeekTaskRankList
	{
	public:
		SCGuildWeekTaskRankList();		//!< 1931 家族任务 - 排行信息
		MessageHeader header;

		struct GWTRankItem
		{
			GWTRankItem()
			{
				memset(name, 0, sizeof(GameName));
				avatar_type = 0;
				headshot_id = 0;
				profession = 0;
				contribution = 0;
			}

			GameName name;
			short avatar_type;
			short headshot_id;
			int profession;
			int contribution;
		};

		int list_count;
		GWTRankItem rank_list[GUILD_WEEK_TASK_RANK_LIST_NUM_MAX];
	};

	//////////////////////////////////// 家族答题 ////////////////////////////////////
	class SCActivityGuildAnswerCurQuestion
	{
	public:
		SCActivityGuildAnswerCurQuestion();		//!< 1223 - 当前题目信息
		MessageHeader header;

		int cur_seq;		//!< 当前是第几题, -1代表准备答题时间
		int question_id;	//!< 问题ID
		unsigned int next_time;
	};

	class CSActivityGuildAnswerAnswerQuest
	{
	public:
		CSActivityGuildAnswerAnswerQuest();		//!< 1224 - 玩家选择的答案
		MessageHeader header;

		int answer;			//!< 我的答案 - [1, 4] - 0 表示玩家离开了答题区
	};

	class SCActivityGuildAnswerResultInfor
	{
	public:
		SCActivityGuildAnswerResultInfor();		//!< 1226 - 本题答题结果
		MessageHeader header;

		int answer_result;		//!< 本题是否答对：0 - 答错； 1 - 答对；
	};

	class SCActivityGuildAnswerAwardSettle
	{
	public:
		SCActivityGuildAnswerAwardSettle();		//!< 1227 - 答题奖励总表
		MessageHeader header;

		int coin;			//!< 铜币
		int exp;			//!< 经验
		int contribution;	//!< 贡献

		int item_num;		//!< 道具表长度
		PtcItemCon item_list[ACTIVITY_AWARD_NUM_MAX];
	};

	//
	class SCGuildBossRankInfo		//!< 		9836 家族试炼 伤害排行榜
	{
	public:
		SCGuildBossRankInfo();
		MessageHeader header;

		struct GuildBossRankItem
		{
			int role_id;
			int role_level;
			int top_level;
			int avatar_type;
			int headshot_id;
			int prof_base;			//!< 职业[0,9)
			GameName role_name;
			long long total_damage;
		};

		int count;
		GuildBossRankItem rank_item[GUILD_MAX_MEMBER_COUNT];
	};

	class SCGuildEventInfo : public IMessageSerializer		//!< 		9839 家族事件下发
	{
	public:
		SCGuildEventInfo();
		virtual ~SCGuildEventInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(count, buffer, buffer_size, out_length);
			
			for (int i = 0; i < count && i < GUILD_EVENTHANDLER_COUNT; i++)
			{
				result = result && WriteInt(eventhandler_list[i].eventhandler_type, buffer, buffer_size, out_length);
				result = result && WriteUInt(eventhandler_list[i].eventhandler_time, buffer, buffer_size, out_length);
				result = result && WriteStrN(eventhandler_list[i].eventhandler_name, sizeof(GameName), buffer, buffer_size, out_length);

				int eventhandler_type = eventhandler_list[i].eventhandler_type / GUILD_EVENTHANDLER_PARAM;
				switch (eventhandler_type)
				{
					case GUILD_EVENTHANDLER_TYPE_XUNBAO:
					case GUILD_EVENTHANDLER_TYPE_ADD_PET:
					case GUILD_EVENTHANDLER_TYPE_WORK_SHOP_COMPOUND:
					case GUILD_EVENTHANDLER_TYPE_WORK_SHOP_CRAFT_JEWELRY:
					{
						result = result && WriteUShort(eventhandler_list[i].item_info.item_id, buffer, buffer_size, out_length);
						/*result = result && WriteShort(eventhandler_list[i].item_info.column, buffer, buffer_size, out_length);
						result = result && WriteShort(eventhandler_list[i].item_info.index, buffer, buffer_size, out_length);
						result = result && WriteShort(eventhandler_list[i].item_info.num, buffer, buffer_size, out_length);
						result = result && WriteChar(eventhandler_list[i].item_info.is_bind, buffer, buffer_size, out_length);
						result = result && WriteChar(eventhandler_list[i].item_info.has_param, buffer, buffer_size, out_length);
						result = result && WriteUInt(eventhandler_list[i].item_info.invalid_time, buffer, buffer_size, out_length);*/
						if (1 == eventhandler_list[i].item_info.has_param)
						{
							result = result && WriteShort(eventhandler_list[i].item_info.param_length, buffer, buffer_size, out_length);
							result = result && WriteStrN(eventhandler_list[i].item_info.param_data, eventhandler_list[i].item_info.param_length, buffer, buffer_size, out_length);
						}
						else
						{
							result = result && WriteShort(0, buffer, buffer_size, out_length);
						}
					}
					break;
					default:
					{
						//与客户端对好的,客户端以此为判断
						result = result && WriteUShort(0, buffer, buffer_size, out_length);
						result = result && WriteShort(0, buffer, buffer_size, out_length);
					}
					break;
				}
			}
			return result;
		}

		int count;
		GuildEventhandlerParam eventhandler_list[GUILD_EVENTHANDLER_COUNT];
	};

	class CSGuildSetBanner		//!< 9840 家族修改族徽
	{
	public:
		CSGuildSetBanner();
		MessageHeader header;

		GuildBanner banner;		//!< 长度为8的字符串 
	};

	class SCGuildBossChapterGuildRankInfo : public IMessageSerializer		//!< 9841 各个家族之间家族试炼进度排行
	{
	public:
		SCGuildBossChapterGuildRankInfo();
		virtual ~SCGuildBossChapterGuildRankInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteInt(count, buffer, buffer_size, out_length);

			for (int i = 0; i < count && i < ARRAY_LENGTH(rank_list); i++)
			{
				result = result && WriteShort(rank_list[i].boss_index, buffer, buffer_size, out_length);
				result = result && WriteShort(rank_list[i].kill_num, buffer, buffer_size, out_length);
				result = result && WriteStrN(rank_list[i].guild_name, sizeof(GameName), buffer, buffer_size, out_length);
				result = result && WriteStrN(rank_list[i].guild_banner, sizeof(GuildBanner), buffer, buffer_size, out_length);
			}
			return result;
		}

		int count;
		SynGuildBossChapterRankInfo rank_list[50];
	};

	class SCGuildGatherDreamInfoList	// 9842 家族集愿信息列表下发
	{
	public:
		SCGuildGatherDreamInfoList();
		MessageHeader header;

		int count;
		GuildGatherDreamData gather_dream_list[GUILD_MAX_MEMBER_COUNT];
	};

	class SCGuildGatherDreamSignleInfo			// 9843 家族集愿单条信息下发
	{
	public:
		SCGuildGatherDreamSignleInfo();
		MessageHeader header;

		GuildGatherDreamData gather_dream_info;
	};

	class CSGuildActivePersonRank			// 3910  获取 帮派活跃度 个人排行信息 
	{
	public:
		CSGuildActivePersonRank();
		MessageHeader header;

	};

	class SCGuildActiveInfo						// 3911  帮派活跃度信息[帮派活跃值]
	{
	public:
		SCGuildActiveInfo();
		MessageHeader header;

		int day_active_val;						// 帮派日活跃总值
		int week_active_val;					// 帮派周活跃总值
	};

	class SCGuildActiveInfoMember				// 3912  帮派活跃度信息[个人领取奖励的信息]
	{
	public:
		SCGuildActiveInfoMember();
		MessageHeader header;

		int flag;								// 领取奖励标记[按位]
		int seq;								// -1时不使用 , 非-1 则表示刚才领取成功的奖励序号
	};

	class SCGuildActivePersonRank				// 3913  帮派活跃度 个人排行信息 
	{
	public:
		SCGuildActivePersonRank();
		MessageHeader header;

		struct PersonRankItem
		{
			GameName game_name;					// 玩家姓名
			int prof;							// 职业
			char reserve_ch;					// 备用
			char post;							// 帮派职位
			short level;						// 玩家等级
			int week_active_val;				// 周活跃度
		};

		int index;		// -1表示不在榜上
		int count;
		PersonRankItem  rank_info[GUILD_MAX_MEMBER_COUNT];		// 已排序
	};

	class CSGuildActiveGetGift					// 3914  获取 帮派活跃度 礼包
	{
	public:
		CSGuildActiveGetGift();
		MessageHeader header;

		int seq;								// 礼包的序号
	};

	struct GuildInbornSkillInfo
	{
		GuildInbornSkillInfo() : guild_inborn_type(0), guild_type_seq(0), skill_level(0) {}
		GuildInbornSkillInfo(char _type, char _seq, short _level) : guild_inborn_type(_type), guild_type_seq(_seq), skill_level(_level) {}

		char guild_inborn_type;			//灵脉技能类型
		char guild_type_seq;			//灵脉技能序号
		short skill_level;
	};

	class CSGuildInbornReq					//!< 3360 家族灵脉操作请求		
	{
	public:
		CSGuildInbornReq();
		MessageHeader header;

		enum GUILD_INBORN_REQ_TYPE
		{
			GUILD_INBORN_REQ_TYPE_ALL_INFO = 0,			//!< 获取家族灵脉所有信息
			GUILD_INBORN_REQ_TYPE_LEARN_SKILL = 1,		//!< 家族灵脉学习技能 param:inborn_type param1:type_seq
			GUILD_INBORN_REQ_TYPE_RESET_SKILL = 2,		//!< 家族灵脉技能重置 param:inborn_type param1:type_seq
		};

		int req_type;
		int param1;
		int param2;
	};

	class SCGuildInbornAllInfo				//!< 3361 家族灵脉所有信息下发
	{
	public:
		SCGuildInbornAllInfo();
		MessageHeader header;

		short reserve_sh;
		short learn_count;
		GuildInbornSkillInfo learn_list[MAX_GUILD_INBORN_SKILL_CUR_NUM];	//已学习列表
	};

	class SCGuildInbornSingleInfo		//!< 3362 家族灵脉单个信息下发
	{
	public:
		SCGuildInbornSingleInfo();
		MessageHeader header;

		GuildInbornSkillInfo learn_info;			//已学习的技能信息
	};

	class SCGuildInbornSingleTypeInfo	//!< 3363 家族灵脉类型信息下发
	{
	public:
		SCGuildInbornSingleTypeInfo();
		MessageHeader header;

		short reserve_sh;
		short guild_inborn_type;													//灵脉技能类型
		short learn_level_list[MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_CUR_NUM];	//学习等级列表  
	};

	class SCGuildQuitInfo						// 9844 退出家族信息
	{
	public:
		SCGuildQuitInfo();
		MessageHeader header;

		int quit_count;
		unsigned int next_join_time;
	};

	class SCGuildRecommend						// 9845 工会推荐
	{
	public:
		SCGuildRecommend();
		MessageHeader header;

		int guild_id;
		GuildName guild_name;
		GameName role_name;
	};

	class CSGuildHallOfFameReq						// 9846 名人堂操作
	{
	public:
		CSGuildHallOfFameReq();
		MessageHeader header;
		enum REQ_TYPE
		{
			REQ_TYPE_INFO = 0,//请求排行下发 param enum HALLOFFAME_TYPE
			REQ_TYPE_QIFU = 1,//祈福
			REQ_TYPE_REWARD = 2,//领取祈福奖励
			REQ_TYPE_QIFU_INFO = 3,//祈福信息下发
		};
		int type;
		int param;
	};

	class SCGuildHallOfFameInfo						// 9847 帮派名人堂排行下发
	{
	public:
		SCGuildHallOfFameInfo();
		MessageHeader header;
		struct HallOfFameInfo
		{
			int value;
			int role_id;
			GameName name;
			int prof;
		};
		int rank_type;//排行类型
		Appearance appearance_info;//第一外观
		int rank_count;//排行人数
		HallOfFameInfo rank_list[GUILD_MAX_MEMBER_COUNT];
	};

	class SCGuildQiFuInfo						// 9848 祈福信息下发
	{
	public:
		SCGuildQiFuInfo();
		MessageHeader header;
		int is_open;
		int qifu_value;//祈福进度
		int today_qifu_times;//今日祈福次数
		int is_get_qifu_attr;//是否领取祈福属性
	};

	struct ConstructInfo		//建筑单个信息
	{
		ConstructInfo():build_type(0),build_level(0)
		{}

		short build_type;
		short build_level;
	};

	class SCGuildConstructInfo					//!< 9849 家族内政建设单条信息下发
	{
	public:
		SCGuildConstructInfo();
		MessageHeader header;

		ConstructInfo build_info;
	};

	class CSGuildConstructReq					//!< 9850 家族内政建设请求
	{
	public:
		CSGuildConstructReq();
		MessageHeader header;

		enum OP_TYPE
		{
			OP_TYPE_BUILDING = 1,		// 建造
			OP_TYPE_DESTROY = 2,		// 拆除
			OP_TYPE_MEMBER_SPEED_UP = 3,// 家族成员为建筑加速
		};

		short build_type;
		short op_type;
	};

	class SCGuildBuildingFinishInfo			//!< 9851 家族建筑建成,方便客户端判断红点
	{
	public:
		SCGuildBuildingFinishInfo();
		MessageHeader header;

		ConstructInfo build_finish_info;
	};

	class SCGuildUseBuildMaterialInfo		//!< 9852 家族建造通用漂字弹窗
	{
	public:
		SCGuildUseBuildMaterialInfo();
		MessageHeader header;

		void Reset()
		{
			op_type = 0;
			uid = 0;
			item_id = 0;
			num = 0;
			build_type = 0;
			memset(name, 0, sizeof(name));
		}

		enum OP_TYPE
		{
			OP_TYPE_USE_ITEM = 1,		//使用物品
			OP_TYPE_OPEN_SUP = 2,		//开启加速
			OP_TYPE_ADD_GONGXIAN = 3,	//增加贡献
			OP_TYPE_MEMBER_SUP = 4,		//成员加速
			OP_TYPE_SUP_FINISH = 5,		//加速建设完成
		};

		int op_type;
		int uid;				// type=2时,表示职位
		GameName name;
		int item_id;
		int num;
		int build_type;
	};

	class SCGuildConstructAllInfo					//!< 9853 家族内政建设全部信息下发
	{
	public:
		SCGuildConstructAllInfo();
		MessageHeader header;

		ConstructInfo build_list[GUILD_CONSTRUCT_BUILDING-1]; // 5个建筑
		int building_build;									// 正在建造的建筑信息,0-无建造
		unsigned int building_finish_timestamp;				// 建造完成时间戳
		int building_count;									// 当日已建造次数
	};

	class SCGuildSpeedUpInfo			//!<9854 家族建设加速信息下发
	{
	public:
		SCGuildSpeedUpInfo();
		MessageHeader header;

		short quicken_building_count;		// 今日已开启加速次数
		short can_speed_up;					// 当前是否可加速
		int is_cur_building_help;			// 玩家今日参与加速次数
	};

	class SCGuildConstructTimeStampInfo			//!<9855 家族建造完成时间戳	
	{
	public:
		SCGuildConstructTimeStampInfo();
		MessageHeader header;

		int building_build;					// 正在建造的建筑,0-未建造
		unsigned int building_finish_timestamp;	// 建造完成时间戳,0-未建造
		int building_count;							// 当日已建造次数
	};

	class SCGuildConstructOpenSUPInfo		//!<9856 家族加速开启人信息
	{
	public:
		SCGuildConstructOpenSUPInfo();
		MessageHeader header;

		int open_post;
		GameName name;
	};

	class SCGuildMemberInfoChange		//!<9857 家族成员信息
	{
	public:
		SCGuildMemberInfoChange();
		MessageHeader header;

		MemberInfo member_info;
	};
}

#pragma pack(pop)
#endif