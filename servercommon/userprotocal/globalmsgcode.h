#ifndef __GLOBALMSGCODE_H__
#define __GLOBALMSGCODE_H__

namespace Protocol
{
	enum
	{
		GLOBAL_SERVER_SC_BEGIN = 9000,
		GLOBAL_SERVER_CS_BEGIN = 9050,

		TEAM_SC_BEGIN = 9100,
		TEAM_CS_BEGIN = 9150,

		GLOBAL_ROLE_INFO_SC_BEGIN = 9300,
		GLOBAL_ROLE_INFO_CS_BEGIN = 9350,

		GLOBAL_MAIL_SC_BEGIN = 9500,
		GLOBAL_MAIL_CS_BEGIN = 9550,

		GLOBAL_PUBLICSALE_SC_BEGIN = 9600,
		GLOBAL_PUBLICSALE_CS_BEGIN = 9650,

		GLOBAL_GUILD_SC_BEGIN = 9800,
		GLOBAL_GUILD_CS_BEGIN = 9850,

		GLOBAL_GUILD_UNION_SC_BEGIN = 9900,
		GLOBAL_GUILD_UNION_CS_BEGIN = 9950,

		GLOBAL_RANK_SC_BEGIN = 10000,
		GLOBAL_RANK_CS_BEGIN = 10050,

		GLOBAL_OTHER_SC_BEGIN = 10100,
		GLOBAL_OTHER_CS_BEGIN = 10150,

		GLOBAL_CAMP_SC_BEGIN = 10200,
		GLOBAL_CAMP_CS_BEGIN = 10250,

		GLOBAL_ACTIVITY_SC_BEGIN = 10300,
		GLOBAL_ACTIVITY_CS_BEGIN = 10350,
	};

	enum _MsgTypeGlobal
	{
		//////////////////////////////////////////////// 全局服务器相关 /////////////////////////////////////////////////////////
		MT_ENTER_GLOBAL_SERVER_ACK_SC = 9000,									// 请求登陆全局服务器回复
		MT_TIME_ACK_SC = 9001,													// 服务器时间返回
		MT_WALLOW_NOTICE_SC = 9002,												// 用户防沉迷返回
		MT_DISCONNECT_NOTICE_SC = 9003,											// 通知客户端断开服务器
		MT_TIME_DATE_SC = 9004,													// 服务器日期下发
		MT_CONNECT_CROSS_INFO_SC = 9005,										// 跨服连接信息下发

		MT_ENTER_GLOBAL_SERVER_REQ_CS = 9050,									// 请求登陆全局服务器
		MT_TIME_REQ_CS = 9051,													// 请求服务器时间

		//////////////////////////////////////////////// 组队相关 /////////////////////////////////////////////////////////
		MT_TEAM_INFO_SC = 9100,													// 队伍信息
		MT_OUT_OF_TEAM_SC = 9101,												// 队伍已经解散
		MT_INVITE_USER_TRANSMIT_SC = 9102,										// 通知被邀请
		MT_REQ_JOIN_TEAM_SC = 9103,												// 通知队长有人申请加入队伍
		MT_TEAM_LIST_ACK_SC = 9104,												// 请求队伍列表回复
		MT_JOIN_TEAM_SC = 9105,													// 通知玩家加入了队伍
		MT_TEAM_LEADER_INFO = 9106,												// 队长信息，给队友用来追随队长
		MT_TEAM_LEADER_CALLTOGETHER = 9107,										// 队长发起召集
		MT_TEAM_ENTER_ROUTE_SC = 9108,											// 进入副本请求转发
		MT_TEAM_ENTER_INFO_SC = 9109,											// 队员确定信息
		MT_TEAM_POS_INFO_SC = 9110,												// 组队站位信息
		MT_TEAM_LEADER_CHANGE_NOTICE_AREA_SC = 9111,							// 队长改变后通知附近玩家
		MT_CHIVALROUS_TEAM_LIST_SC = 9112,										// 侠义值队伍列表

		MT_CREATE_TEAM_CS = 9150,												// 创建队伍
		MT_INVITE_USER_CS = 9151,												// 邀请加入队伍
		MT_INVITE_JOIN_TEAM_RET_CS = 9152,										// 答复入队邀请
		MT_REQ_JOIN_TEAM_RET_CS = 9153,										// 队长回复允许申请者加入队伍
		MT_REQ_JOIN_TEAM_CS = 9154,											// 玩家申请加入某队伍
		MT_DISMISS_TEAM_CS = 9155,												// 队长解散队伍
		MT_KICK_OUT_OF_TEAM_CS = 9156,											// 队长将队员踢出队伍
		MT_CHANGE_TEAM_LEADER_CS = 9157,										// 换队长
		MT_EXIT_TEAM_CS = 9158,												// 玩家退出队伍
		MT_TEAM_LIST_REQ_CS = 9159,											// 请求所在场景的队伍列表
		MT_CHANGE_MUST_CHECK_CS = 9160,										// 改变队伍加入是否是要队长通过
		MT_CHANGE_ASSIGN_MODE_CS = 9161,										// 改变队伍分配模式
		MT_CHANGE_MEMBER_CAN_INVITE_CS = 9162,									// 改变队伍普通队员是否可邀请
		MT_CHANGE_TEAM_LIMIT_CS = 9163,										// 修改队伍加入限制
		MT_AUTO_HAVE_TEAM_CS = 9164,											// 自动组队
		MT_UNFULL_TEAM_LIST_BY_TYPE_REQ_CS = 9165,								// 根据队伍类型获取未满的队伍列表
		MT_SET_TEAM_FLAG_CS = 9166,											// 设置满人自动开始标记
		MT_TEAM_OPERATE_CS = 9167,												// 队伍操作
		MT_TEAM_MATCH_INFO_SC = 9169,											// 是否在匹配中
		MT_TEAM_MATCH_REQ_CS = 9170,											// 匹配
		MT_TEAM_TEMPORARILY_PART_CS = 9171,										// 暂离
		MT_TEAM_INVITATION_REQ_CS = 9172,										// 组队邀请
		MT_TEAM_INVITATION_NOTIC_SC = 9173,										// 通知收到了组队邀请
		MT_TEAM_INVITATION_RET_CS = 9174,										// 组队邀请回复
		MT_TEAM_RECRUIT_CS = 9175,												// 招募
		MT_TEAM_ENTER_FB_RET_CS = 9176,											// 回复进入副本请求
		MT_TEAM_SWITCH_TO_CROSS = 9177,											// 切换到跨服状态
		MT_TEAM_CHANNEL_INFO_SC = 9178,											// 组队频道信息
		MT_TEAM_SWITCH_POSITION_CS = 9179,										// 队伍切换站位请求
		MT_TEAM_MEMBER_APPEARANCE_CHANGE_SC = 9180,								// 成员外观改变通知														
		MT_TEAM_REMOVE_APPLY_SC = 9181,											// 去除审批通知
		MT_TEAM_APPLY_LIST_SC = 9182,											// 申请列表下发					
		MT_TEAM_VOTE_START_SC = 9183,											// 提示开始投票
		MT_TEAM_LAUNCH_LEADER_VOTE_CS = 9184,									// 申请成为队长
		MT_TEAM_ROLE_VOTE_CS = 9185,											// 投票
		MT_TEAM_SYNC_MEMBER_HPMP_SC = 9186,										// 同步成员的HP和MP
		MT_TEAM_TEAM_LEADER_NPC_REQ_CS = 9187,									// 队长可视NPC操作
		MT_TEAM_TEAM_LEADER_NPC_INFO_SC = 9188,									// 队长可视NPC下发
		MT_TEAM_SYNC_MEMBER_PET_HPMP_SC = 9189,									// 同步成员宠物的HP和MP
		MT_TEAM_MEMBER_ADD_HP_MP_NOTICE_SC = 9190,								// 队员帮助加血加蓝通知
		MT_TEAM_SIMPLE_INFO_CHANGE_NOTICE_SC = 9191,
		MT_TEAM_CHANGE_MEMBER_BASE_INFO_NOTICE_SC = 9192,
		MT_TEAM_LEADER_SUMMON_MEMBER_CS = 9193,
		MT_TEAM_LEADER_SUMMON_MEMBER_NOTICE_SC = 9194,
		MT_TEAM_MEMBER_REPLY_SUMMON_CS = 9195,
		MT_TEAM_MEMBER_INCOMPATIBLE_NOTICE_SC = 9196,
		MT_TEAM_LEADER_BATTLE_PARTNER_LIST_CS = 9197,
		MT_TEAM_LEADER_BATTLE_PARTNER_LIST_SC = 9198,
		MT_TEAM_LEADER_CHECK_MEMBER_ITEM_CS = 9199,
		MT_TEAM_LEADER_CHECK_MEMBER_ITEM_RET_SC = 9200,
		MT_CHIVALROUS_TEAM_LIST_REQ_CS = 9201,									// 侠义值队伍列表
		MT_REQ_JOIN_TEAM_INFO_CS = 9202,
		MT_TEAM_IS_RED_NAME_INFO_CS = 9203,										// 是否红名队伍信息
		MT_TEAM_LEADER_EJECT_WINDOWS_CS = 9204,
		MT_TEAM_LEADER_EJECT_WINDOWS_SC = 9205,
		MT_TEAM_ROLE_RECRUIT_INFO_SC = 9206,
		MT_TEAM_ROLE_RECRUIT_ANSWER_CS = 9207,

		//////////////////////////////////////////////// 其他相关 /////////////////////////////////////////////////////////

		//////////////////////////////////////////////// 邮件相关 /////////////////////////////////////////////////////////
		MT_GLOBAL_MAIL_SEND_RESULT_SC = 9500,									// 提取附件返回
		MT_GLOBAL_MAIL_DELETE_SC = 9501,										// 删除邮件返回
		MT_GLOBAL_MAIL_LOCK_SC = 9502,											// 锁邮件返回
		MT_GLOBAL_MAIL_UNLOCK_SC = 9503,										// 解锁邮件返回
		MT_GLOBAL_MAIL_LIST_SC = 9504,											// 邮件列表返回
		MT_GLOBAL_MAIL_DETAIL_SC = 9505,										// 邮件详细信息
		MT_GLOBAL_MAIL_ATTACHMENT_SC = 9506,									// 提取邮件附件返回
		MT_GLOBAL_MAIL_NEW_MAIL_NOTICE_SC = 9507,								// 新邮件通知
		MT_GLOBAL_MAIL_HAS_UNREAD_MAIL_SC = 9508,								// 未读取邮件通知

		MT_MAIL_SEND_CS = 9550,									// 发送邮件
		MT_MAIL_DELETE_CS = 9551,								// 删除邮件
		MT_MAIL_GETLIST_CS = 9552,								// 获取邮件列表
		MT_MAIL_READ_CS = 9553,									// 读取邮件
		MT_MAIL_ATTACHMENT_CS = 9554,							// 获取附件
		MT_MAIL_CLEAN_CS = 9555,								// 清空邮件
		MT_MAIL_ONE_KEY_FETCH_ATTACHMENT = 9556,				//一键提取邮件

		//////////////////////////////////////////////// 交易市场 /////////////////////////////////////////////////////////
		MT_PUBLICSALE_SEARCH_CS = 9650,											// 搜索

		MT_PUBLICSALE_ADD_ACK_SC = 9600,										// 拍卖物品上架返回
		MT_PUBLICSALE_DEL_ACK_SC = 9601,										// 拍卖物品下架返回
		MT_PUBLICSALE_BUY_ACK_SC = 9602,										// 购买拍卖物品返回
		MT_PUBLICSALE_GET_ITEM_LIST_SC = 9603,									// 获取自己拍卖物品列表
		MT_PUBLICSALE_SEARCH_SC = 9604,											// 搜索返回 
		MT_PUBLICSALE_BUY_FAILED_REFRESH_SC = 9605,								// 购买失败后刷新最新物品信息

		MT_PUBLICSALE_SEND_ALL_RECORD_SC = 9606,								// 发送全部寄售交易记录
		MT_PUBLICSALE_SEND_RECORD_CHANGE_SC = 9607,								// 发送寄售交易记录变化

		MT_PUBLICSALE_SEND_NUM_OF_SEARCH_TYPE_SC = 9608,							// 发送根据 search type 获取寄售物品数量

		//////////////////////////////////////////////// 家族相关 /////////////////////////////////////////////////////////
		MT_CREATE_GUILD_CS = 9800,												// 创建家族
		MT_QUIT_GUILD_REQ_CS = 9801,											// 退出家族
		MT_QUIT_GUILD_SC = 9802,												// 通知玩家已经没有家族
		MT_GUILD_BASE_INFO_SC = 9803,											// 家族基础信息
		MT_GUILD_MEMBER_INFO_SC = 9804,											// 家族成员信息
		MT_GUILD_APPLY_JOIN_REQ_CS = 9805,										// 申请加入
		MT_GUILD_APPLY_JOIN_ACK_CS = 9806,										// 审批申请
		MT_GUILD_EXPEL_OUT_REQ_CS = 9807,										// 踢出家族
		MT_GUILD_SET_NOTICE_REQ_CS = 9808,										// 更改公告
		MT_GET_GUILD_LIST_CS = 9809,											// 请求家族列表
		MT_SEND_GUILD_LIST_SC = 9810,											// 下发家族列表
		MT_SEND_GUILD_NOTICE_SC = 9811,											// 下发家族公告
		MT_GUILD_PET_TRAINING_UP_CS = 9812,										// 强化宠物训练
		MT_GUILD_PET_TRAINING_CS = 9813,										// 宠物训练
		MT_GET_GUILD_TRAINING_INFO_CS = 9814,									// 请求训练信息
		MT_GUILD_SEND_TRAINING_INFO_SC = 9815,									// 发送训练信息
		MT_SEND_GUILD_APPLY_LIST_SC = 9816,										// 下发申请列表
		MT_GUILD_CREATE_RET_SC = 9819,											// 创建家族返回
		MT_GUILD_ABDICATION_REQ_CS = 9820,										// 转让族长
		MT_GUILD_IMPEACHMENT_REQ_CS = 9821,										// 弹劾
		MT_GUILD_APPOINTMENT_REQ_CS = 9822,										// 任命
		MT_GUILD_SHOP_BUY_CS = 9823,											// 商店购买
		MT_GUILD_SHOP_INFO_SC = 9824,											// 商店信息
		MT_GUILD_PET_TRAINING_EXP_SC = 9825,									// 宠物训练结算经验显示
		MT_GUILD_RENAME_CS = 9826,												// 家族改名

		MT_GUILD_INVITATION_REQ_CS = 9829,										// 家族邀请
		MT_GUILD_INVITATION_NOTIC_SC = 9830,									// 通知收到了家族邀请
		MT_GUILD_BOSS_INFO_SC = 9831,											// 家族试炼Boss信息返回
		MT_GUILD_BOSS_REQ_CS = 9832,											// 家族试炼Boss信息请求
		MT_GUILD_LEVEL_UP_CS = 9833,											// 家族升级请求
		MT_PLAYER_APPLY_LIST_SC = 9834,											// 家族申请信息
		MT_GUILD_INVITATION_REPLY_CS = 9835,									// 家族邀请回复
		MT_GUILD_BOSS_RANK_INFO_SC = 9836,										// 家族试炼排行榜
		MT_GUILD_NEED_APPROVE_INFO_SC = 9837,									// 工会是否需要批准加入信息
		MT_GUILD_SET_NEED_APPROVE_INFO_CS = 9838,								// 设置工会是否需要批准加入信息
		MT_GUILD_EVENT_INFO_CS = 9839,											// 家族事件下发
		MT_GUILD_RESET_BANNER_CS = 9840,										// 家族修改族徽
		MT_GUILD_BOSS_CHAPTER_GUILD_RANK_SC = 9841,								// 各个家族之间家族试炼进度排行
		MT_GUILD_GATHER_DREAM_INFO_LIST_SC = 9842,								// 家族集愿信息列表下发
		MT_GUILD_GATHER_DREAM_SIGNLE_INFO_SC = 9843,							// 家族集愿单挑信息下发
		MT_GUILD_QUIT_INFO_SC = 9844,											// 退出家族信息
		MT_GUILD_RECOMMEND_INFO_SC = 9845,										// 推荐家族
		MT_GUILD_HALLOFFAME_REQ_CS = 9846,										// 名人堂操作
		MT_GUILD_HALLOFFAME_INFO_SC = 9847,										// 家族名人堂排行下发
		MT_GUILD_QIFU_INFO_SC = 9848,											// 祈福信息下发

		MT_GUILD_CONSTRUCT_BUILDING_INFO_SC = 9849,								// 家族内政建设单条信息下发
		MT_GUILD_CONSTRUCT_BUILDING_SEQ_CS = 9850,								// 家族内政建设请求
		MT_GUILD_CONSTRUCT_BUILDING_FINISH_INFO_SC = 9851,						// 家族建筑建成,方便客户端判断红点
		MT_GUILD_CONSTRUCT_USE_BUILD_MATERIAL_INFO_SC = 9852,					// 家族建造通用漂字弹窗
		MT_GUILD_CONSTRUCT_BUILDING_ALL_INFO_SC = 9853,							// 家族内政建设全部信息下发
		MT_GUILD_CONSTRUCT_SPEED_UP_INFO_SC = 9854,								// 家族建设加速信息下发
		MT_GUILD_CONSTRUCT_BUILDING_FINISH_TIMESTAMP = 9855,					// 家族建造完成时间戳
		MT_GUILD_CONSTRUCT_OPEN_SUP_INFO_SC = 9856,								// 家族加速开启信息
		MT_GUILD_MEMBER_INFO_CHANGE_SC = 9857,								// 

		//////////////////////////////////////////////// 排行榜相关 /////////////////////////////////////////////////////////
		MT_RANK_GET_PERSONRANK_LIST_ACK_SC = 10000,
		MT_RANK_GET_GUILDRANK_LIST_ACK_SC = 10001,
		MT_RANK_GET_TEAMRANK_LIST_ACK_SC = 10002,
		MT_RANK_GET_PERSONRANK_TOP_USER_SC = 10003,
		MT_RANK_GET_WORLD_LEVEL_ACK_SC = 10004,
		MT_RANK_GET_MINGRENRANK_INFO_SC = 10005,			//名人榜信息返回
		MT_RANK_GET_FENGYUNRANK_INFO_SC = 10006,			//风云榜信息返回
		MT_RANK_GET_MINGRENRANK_OPEN_INFO_SC = 10007,		//风云榜信息返回
		MT_RANK_GET_PETRANK_LIST_ACK_SC = 10008,			// 宠物榜
		MT_RANK_GET_PET_RANK_DETAILED_INFO_SC = 10009,		// 宠物详细信息下发
		
		
		MT_RANK_GET_PERSONRANK_LIST_CS = 10050,
		MT_RANK_GET_GUILDRANK_LIST_CS = 10051,
		MT_RANK_GET_TEAMRANK_LIST_CS = 10052,
		MT_RANK_GET_PERSONRANK_TOP_USER_CS = 10053,
		MT_RANK_GET_MINGRENRANK_LIST_CS = 10054,			//请求名人榜信息
		MT_RANK_GET_FENGYUNRANK_LIST_CS = 10055,			//请求风云榜信息
		MT_RANK_GET_PETRANK_LIST_CS = 10056,				//请求宠物榜
		MT_RANK_GET_REFRESH_TIMESTAMP_CS = 10057,			//请求排行榜刷新时间
		MT_RANK_GET_REFRESH_TIMESTAMP_SC = 10058,			//下发排行榜刷新时间	
		MT_RANK_GET_PETRANK_DETAILED_INFO_CS = 10059,		//请求宠物排行榜某一个宠物信息

		MT_RANK_GET_COURAGE_CHALLENGE_REQ_CS = 10060,		//锢魔之塔排行信息请求
		MT_RANK_GET_COURAGE_CHALLENGE_SC = 10061,			//锢魔之塔排行信息下发

		MT_RANK_GET_CHIVALROUS_REQ_CS = 10062,				//侠义值排行榜请求
		MT_RANK_GET_CHIVALROUS_RESP_SC = 10063,				//侠义值排行榜返回

		//////////////////////////////////////////////// 其它 /////////////////////////////////////////////////////////
		RESERVE_MT_SHOP_LIMIT_ITEM_LIST_SC = 10100,								// 保留
		MT_WORLDBOSS_INFO_SC = 10101,											// 下发世界boss信息
		MT_WORLDBOSS_BORN_SC = 10102,											// 世界boss出生
		MT_JINGYING_BOSS_BORN_NOTICE_SC = 10103,								// 精英boss出生通知

		RESERVE_MT_GET_SHOP_LIMIT_ITEM_LIST_CS = 10150,							// 保留
		MT_GET_WORLDBOSS_INFO_CS = 10151,										// 请求世界boss信息

		//////////////////////////////////////////////// 阵营 /////////////////////////////////////////////////////////
		MT_CAMP_INFO_SC = GLOBAL_CAMP_SC_BEGIN,									// 阵营信息返回

		MT_CAMP_INFO_CS = GLOBAL_CAMP_CS_BEGIN,									// 拉取阵营信息
	};
}

#endif // __GLOBALMSGCODE_H__


