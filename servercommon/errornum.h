#ifndef __ERRORNUM_H__
#define __ERRORNUM_H__

#include "servercommon/servercommon.h"

namespace errornum
{
	enum ERROR_NUM_DEF
	{
		////////////////////////////////////// 背包类错误码 //////////////////////////////////////
		EN_KNAPSACK_USE_NOT_DIRECT = 100,				//该物品不能直接使用
		EN_KNAPSACK_IS_LOCK,							//背包已经锁住
		EN_KNAPSACK_USE_ITEM_ERROR,						//使用物品错误
		EN_KNAPSACK_USE_ITEM_NUM_NOT_ENOUGH,			//使用物品数量错误
		EN_KNAPSACK_LOAD_ERROR,							//读取物品错误
		EN_KNAPSACK_CANNOT_DISCARD,						//物品不能丢弃
		EN_KNAPSACK_MOVE_ERROR,							//物品移动错误
		EN_KNAPSACK_SPLIT_CANNOT,						//物品不能拆分
		EN_KNAPSACK_SPLIT_NUM_ERROR,	                //物品拆分数量错误
		EN_KNAPSACK_PUT_ERROR,							//物品放入错误
		EN_KNAPSACK_NO_SPACE,							//背包空间不足
		EN_TEMPORARY_KNAPSACK_NO_SPACE,					//临时背包空间不足
		EN_KNAPSACK_EQUIP_TAKEOFF_ERROR,				//装备脱下错误
		EN_KNAPSACK_USE_XUANYUN_STATE,					//眩晕状态不允许使用物品
		EN_KNAPSACK_STORAGE_EXTEND_MAX,					//背包仓库扩展已达最大值
		EN_ITEM_IS_INVALID,								//物品已失效
		EN_ITEM_PROF_LIMIT,								//职业不符合
		EN_ITEM_LEVEL_LIMIT,							//当前等级不足
		EN_ITEM_CANNOT_MULTI_USE,						//该物品不能批量使用
		EN_CHESTSHOP_NOT_ENOUGH_TEMP_SPACE,				//宝箱背包空间不足
		EN_ITEM_COLDDOWN,								//物品冷却中
		EN_KNAPSACK_USE_SCENE_LIMIT,					//当前场景不允许使用该物品
		EN_ITEM_TAOFA_TASK_NOT_ACCEPTABLE,				//无法接受该讨伐任务
		EN_ITEM_TASK_LIST_FULL,							//无法使用讨伐令 任务列表满
		EN_ITEM_USE_COUNT_LIMIT,						//使用次数达到上限
		EN_ITEM_TAOFA_TASK_HAS_TASK,					//该级别的讨伐令任务尚未完成 不可重复接取～
		EN_ITEM_EVIL_NEED_NOT_USE,						//您无需使用免罪令牌
		EN_ITEM_SEX_LIMIT,								//性别不符合
		EN_KNAPSACK_CANNOT_DISCARD_INLAY,				//有镶嵌宝石不能丢弃
		EN_KNAPSACK_USE_DEAD,							//已死亡无法使用物品
		EN_SUBLOCK_IS_LOCKED,							//二级锁未解开
		EN_SUBLOCK_ERROR_TIMES_LIMIT,					//已到达二级锁每天允许尝试密码次数
		EN_ITEM_CALL_MONSTER_IN_FB,						//副本内不可召唤怪物
		EN_SUBLOCK_PASSWORD_ILLEGAL,					//二级锁密码非法
		EN_ITEM_HAVE_BIGGER_BUFF,						//身上已有更强大的状态
		EN_EXTEND_KNAPSACK_TIME_LIMIT,					//在线时间不足以开启当前格子
		EN_ITEM_UNUSABLE_ON_FLY,						//飞行状态下不可使用该物品
		EN_ITEM_TIME_NO_IN_STORAGE,						//时效性物品不可放入仓库
		EN_STOP_FLY_AREA_LIMIT,							//不可走区域不能停止飞行
		EN_PET_COUNT_LIMIT,								//无法拥有更多的宠物
		EN_PET_LEVEL_LIMIT,								//已达到最大等级
		EN_TARGET_NOT_ONLINE,							//对方不在线
		EN_NOT_MEET_JOIN_CONDITIONS,					//您与对方要求的等级不符
		EN_HAS_BEEN_SEND_JOIN_REQ,						//已发送过组队申请
		EN_NOT_ELIGIBLE,								//不符合领取条件
		EN_STUFF_NOT_ENOUGH,							//材料物品不足
		EN_KNAPSACK_EXTEND_TIMES_LIMIT,					//扩展次数已达上限
		EN_KNAPSACK_ORDER_TOO_FREQUENT,					//整理背包过于频繁，请稍后再试
		EN_ITEM_IS_BIND,								//物品已绑定
		EN_ITEM_CAN_NOT_TRADE,							//物品不可交易
		EN_ITEM_AMOUNT_NOT_ENOUGH,						//物品数量不足
		EN_ITEM_NOT_ELIGIBLE,							//所需物品不足
		EN_PET_COUNT_LIMIT2,							//宠物背包空间不足，请腾出空间后再试
		EN_KNAPSACK_CANNOT_DISCOMPOSE,					//物品不能分解
		EN_SURFACE_ALREADY_HAD,							//已拥有该时装，多余的时装可进行分解
		EN_SURFACE_DECOMPOSE_ERROR,						//时装分解失败
		EN_KNAPSACK_NO_ITEM,							//您的背包没有所需道具
		EN_KNAPSACK_CONVERT_ITEM,						//抱歉,物品兑换失败,请查看邮件
		EN_KNAPSACK_CONVERT_ALREADY_OWNED,				//您已有重复时装,新的会自动转化为对应的时装升级道具
		EN_PET_BAG_EXTEND_LIMIT,						//宠物背包格数已达到上限
		EN_PET_NOT_TAKE_EQUIPMENT,						//宠物没有穿戴装备
		EN_EQUIPMENT_PUT_LEVEL_LIMIT,					//等级不足，无法穿戴
		EN_PET_NO_EQUIPMENT,							//宠物未穿戴装备
		EN_OPERATE_NEED_IN_CROSS,						//需要在跨服操作
		EN_CROSS_ROLE_CANNOT_INVITE,					//您目前处于原服，无法邀请处于跨服状态下的玩家
		EN_KNAPSACK_PARTNER_SKIN_CONVERT,				//您已有重复伙伴皮肤，新的会自动转化为伙伴皮肤碎片
		EN_KNAPSACK_PET_SKIN_CONVERT,					//您已有重复宠物皮肤，新的会自动转化为宠物皮肤碎片
		EN_KNAPSACK_ITEM_ENOUGH_COMMIT_ERROR,			//物品不足，无法提交
		EN_PET_NO_SOUL_WEAPON_ERR,						//宠物未穿戴魂器
		EN_PET_SOUL_WEAPON_PARAM_ERR,					//魂器脱下错误
		EN_PET_SOUL_WEAPON_LEVEL_LIMIT_ERR,				//魂器等级不足
		EN_PET_SOUL_BELONG_ERR,							//该宠物无法穿戴这件专属魂器
		EN_KNAPSACK_ITEM_NOT_ELIGIBLE,					//物品不符合
		EN_MARRY_NOT_DISAGREE,					//对方不同意
		EN_MARRY_REPLY_TIME_OUT,					//邀请已过期
		EN_HAS_QING_YUAN,					//一方已有情缘
		EN_HAS_QING_YUAN_INTIMACY_NOT_ENGOUH ,		//亲密度不足
		EN_QING_YUAN_REPLY_TIME_OUT,		//操作已过期
		EN_QING_YUAN_CANT_GIVE,					//对方爱情礼盒未到期，无法继续赠送
		EN_QING_YUAN_GIFT_ALREADY_FETCH,					//奖励已经领取了
		EN_QING_YUAN_DISAGREE_DISMISS,					//对方已拒绝了您的离婚申请
		EN_QING_YUAN_AGREE_DISMISS,					//对方同意了离婚申请
		EN_QING_YUAN_CANT_SINGLE_LEAVE,					//对方离线时长不足，不能单方面离婚
		EN_QING_YUAN_NULL,								//清先缔结情缘, 

		EN_HUAN_SHOU_BAG_NOT_SPACE,						//幻兽背包空间不足

		////////////////////////////////////// 人物类错误代码 //////////////////////////////////////
		EN_ROLE_LEVEL_TOO_LOW = 200,					//人物等级过低
		EN_ROLE_LEVEL_OVERTOP,							//人物等级过高
		EN_ROLE_OCCUP_NOT_FIT,							//人物职业不符合要求
		EN_ROLE_ATTR_POINT_LIMIT,						//不能超过当前总资质点的一半
		
		////////////////////////////////////// 任务类错误代码 //////////////////////////////////////
		EN_TASK_PREDECESSOR_NEED = 250,					//必须先完成前置任务
		EN_TASK_MISSION_COMPLETE = 251,					//任务已经完成，请选择提交任务
		EN_TASK_TIMER_LIMIT_OVER = 252,					//任务时限已过，请选择放弃任务
		EN_TASK_NOLONGER_EXISTED = 253,					//任务已不存在，请选择放弃任务
		EN_TASK_IS_NOT_COMPLETED = 254,					//任务尚未完成，请加油完成任务
		EN_TASK_ALREADY_FINISHED = 255,					//这个任务早就完成过了
		EN_TASK_ALREADY_ACCEPTED = 256,					//您已经接受了这个任务
		EN_TASK_CONDI_MUST_ALONE = 257,					//这个任务必须单独完成
		EN_TASK_CONDI_NEED_MORES = 258,					//这个任务需要更多队员
		EN_TASK_LIST_ALREADY_MAX = 259,					//当前任务太多，不能再接受任务
		EN_TASK_CAN_NOT_BE_ABORT = 260,					//此任务不能在当前身份下放弃
		EN_TASK_TIME_ALREADY_MAX = 261,					//任务次数已达上限
		EN_TASK_NOT_ACCEPTEND = 262,					//任务尚未领取
		EN_ESCORT_TASK_ALREADY_ACCEPTED = 263,			//请先完成其他镖师的任务，再来接新的任务
		EN_TASK_LUO_SHU_QI_WEN_NOT_FINISH = 264,		//这样似乎不能破阵呢，再检查一下吧！
		EN_TASK_PARTNER_SOAR_TASK_FINISH = 265,			//今天已完成过伙伴飞升，请明天再来
		EN_TASK_PARTNER_SOAR_TASK_IN_PROGRESS = 266,	//当前正在进行伙伴飞升任务
		EN_TASK_NOT_GIVE_UP_TASK = 267,					//抱歉,该任务不能放弃
		EN_TASK_PARTNER_SOAR_NOT_ACCPET_BY_ROLE_NOT_SOAR = 268,	//角色还未进行飞升,暂时无法接取伙伴飞升任务
		EN_TASK_ACCEPT_OPEN_SERVER_LIMIT = 269,			//任务尚未可接取，请耐心等待

		///////////////////////////////////////// 伙伴模块 /////////////////////////////////////////
		EN_PARTNER_STATUS_IS_LOCKED = 270,				//这个伙伴还没解锁哦
		EN_PARTNER_STATUS_IS_UNLOCK,					//这个伙伴已经解锁了
		EN_PARTNER_SYNCFIGHT_LIMITS,					//伙伴出战数已达上限
		EN_PARTNER_GRADE_ALREAY_MAX,					//这个伙伴已经满阶了
		EN_PARTNER_ATTR_BREAKTH_UNFINISHED,				//这个伙伴的属性突破尚未完成
		EN_PARTNER_UPGRADE_TASK_UNFINISHED,				//这个伙伴的升阶任务尚未完成
		EN_PARTNER_SKILLS_IS_LOCKED,					//这个技能还没有解锁
		EN_PARTNER_LEVEL_TOO_LOW,						//伙伴等级不足
		EN_PARTNER_EQUIP_UNLOCK_TOO_LOW,				//命格解锁等级不足无法装备
		EN_PARTNER_BIOGRAPHY_UPGRADE_LV_NOT_WORTH,		//抱歉,该伙伴等阶不足,无法查看此传记

		///////////////////////////////////////// 宠物模块 /////////////////////////////////////////
		EN_PET_LEVEL_DEFICIENCY = 280,					//宠物等级不足
		EN_PET_INDEX_ERROR,								//宠物序号错误
		EN_PET_SKILL_INDEX_ERROR,						//宠物技能槽不足
		EN_PET_SKILL_NEW_LOWER_THAN_OLD,				//已学会更高级的技能
		EN_PET_IS_FIGHT,								//该宠物处于出战状态，操作失败
		EN_PET_IS_STANDBY,								//该宠物处于待机状态，操作失败
		EN_STANDBY_AND_FIGHT_PET_NOT_EXCEED_5,			//待机和出战宠物不能超过5只
		EN_PET_IS_LOCK,									//该宠物已上锁，操作失败
		EN_PET_IS_BIND,									//该宠物已绑定，操作失败
		EN_PET_CAN_NOT_COMPREHEND_GIFT,					//该宠物已经没有技能可以领悟
		EN_PET_COMPOSE_FAILD_NEED_PET_ITEM_NOT_ENOUGH,	//宠物专属碎片不足

		///////////////////////////////////////// NPC模块 /////////////////////////////////////////
		EN_NPC_ROLE_IS_FAR_FROM_NPC,					//距离目标太远
		EN_NPC_ROLE_IS_BUSY_CURRENT,					//忙碌中...

		///////////////////////////////////////// 工坊模块 /////////////////////////////////////////
		EN_WORKSHOP_RECIPE_ALREADY_ACTIVE = 300,		//这个配方已经激活过了哦
		EN_WORKSHOP_RECIPE_ACTIVE_SUCCEED = 301,		//工坊配方激活成功
		EN_WORKSHOP_GEM_MOUNT_ALREADY_MAX = 302,		//宝石镶嵌已达上限
		EN_WORKSHOP_GEM_PARAM_OCCUR_ERROR = 303,		//宝石参数出现错误
		EN_WORKSHOP_RECIPE_IS_NOT_ACTIVED = 304,		//目标配方尚未激活
		EN_WORKSHOP_ITEM_COMPOUND_FAILURE = 305,		//目标物品合成失败
		EN_WORKSHOP_EQUIPMENT_CAN_T_CRAFT = 306,		//此装备不支持强化打造
		EN_WORKSHOP_WITHOUT_LEGAL_RESULTS = 307,		//操作失败！没有匹配的成果！
		EN_WORKSHOP_REFORGE_RESULTS_EXIST = 308,		//存在上次重锻结果
		EN_WORKSHOP_GEM_ALREADY_MAX_LEVEL = 309,		//这个宝石已经是最高级的宝石了！

		///////////////////////////////////////// 商店模块 /////////////////////////////////////////
		EN_SHOP_ITEM_NOT_SUPPORT_FAST_BUY = 310,		//此道具不支持快速购买
		EN_SHOP_ITEM_NOT_CROSS_CONVERT_BUY = 311,		//跨服无法兑换，请返回原服		

		///////////////////////////////////////// 颜值系统 /////////////////////////////////////////
		EN_FACE_SCORE_RES_ALREADY_BUY = 320,			//已经拥有了这个颜值资源，不能重复购买
		EN_FACE_SCORE_CURRENCY_INSUFF = 321,			//颜值币不足
		EN_FACE_SCORE_RES_IS_INACTIVE = 322,			//这个颜值资源还没有激活，操作失败
		EN_FACE_SCORE_NOT_REPEAT_WEAR = 323,			//不可重复穿戴
		EN_FACE_SCORE_RES_NOT_ACTIVE  = 324,			//您已激活该颜值道具，操作失败

		/////////////////////////////////////////// 家族 ///////////////////////////////////////////
		EN_GUILD_INVILD_NAME = 400,						//家族名已存在
		EN_GUILD_APPLICANT_HAS_GUILD = 401,				//申请人已拥有家族
		EN_GUILD_IMPEACHMENT_CANNOT = 402,				//当前无法弹劾族长
		EN_GUILD_INSUFFICIENT_AUTHORITY= 403,			//权限不足
		EN_GUILD_POST_REACHED_MAXINUM = 404,			//职位人数已达最大
		EN_GUILD_GONGXIAN_NOT_ENOUGH = 405,				//贡献不足
		EN_GUILD_BOSS_LIMIT_TODAY = 406,				//今天已挑战，请明天再来
		EN_GUILD_NOT_IN_GUILD = 407,					//您还没有加入任何家族，请先加入家族后再试~
		EN_GUILD_APPLY_SUCC = 408,						//申请成功，请少侠静候佳音
		EN_GUILD_APPLY_REPEAT = 409,					//少侠已经申请过该家族了哦
		EN_GUILD_EXP_NOT_ENOUTH = 410,					//家族资金不足
		EN_GUILD_APPLY_LIMIT_NUM = 411,					//你申请的家族数量太多，请耐心等待
		EN_GUILD_IS_SAME_AS_YOURS = 412,				//对方与您处于同一家族中，操作失败
		EN_GUILD_OPPOSITE_IS_NOT_IN_GUILD = 413,		//对方没有加入任何家族，操作失败
		EN_GUILD_OPPOSITE_HAVE_GUILD_YET = 414,			//对方已有家族，操作失败
		EN_GUILD_YOU_HAVE_GUILD = 415,					//您已经拥有家族，操作失败
		EN_GUILD_BOSS_FINISH = 416,						//家族试炼已全部击杀
		EN_GUILD_PET_TRAINING_ALREADY = 417,			//这个宠物已经在训练中了
		EN_GUILD_CAN_NOT_KICKOUT_SELF = 418,			//不能把自己踢出家族哦~
		EN_GUILD_CAN_NOT_APPOINT_SELF = 419,			//不能给自己任命职务哦~
		EN_GUILD_LEVEL_ALREADY_MAX = 420,				//家族等級已达上限
		EN_GUILD_BOSS_CANT_SELECT_CHAPTER = 421,		//只能选择已通关的章节
		EN_GUILD_CAN_NOT_IMPEACH_YOURSELF = 422,		//不能弹劾自己哦~
		EN_GUILD_MAX_MEMBER_CAN_NOT_JOIN = 423,			//家族人数已满，无法加入
		EN_GUILD_CREATE_GOLD_NOT_ENOUGH = 424,			//您的元宝不足,请补足后再试
		EN_GUILD_CANT_INVITE_OTHER_FREQUENTLY,			//抱歉，10分钟内不允许频繁邀请同一角色
		EN_GUILD_GUILD_FIGHT_OPEN_NOT_QUIT_GUILD,		//抱歉,家族大乱斗期间暂时无法退出家族
		EN_GUILD_BANNER_SENSITIVE_WORDS,				//抱歉，您输入的族徽内容含有敏感字符，请重试
		EN_GUILD_BANNER_EMPTY,							//少侠，请输入家族徽章内容
		EN_GUILD_HAS_SET_BANNER_TODAY,					//今天已设置过族徽，请明天再试
		EN_GUILD_BANNER_IS_USED,						//抱歉，该族徽已经被占用
		EN_GUILD_FIGHT_NOT_SAME_GUILD_WITH_LEADER,		//与队长不同家族，无法加入到场景内
		EN_GUILD_GATHER_DREAM_NOT_CAN_PRAY,				//抱歉,这种品质的宠物不能祈愿
		EN_GUILD_GATHER_DREAM_NOT_REPEATABLE,			//抱歉,您今天已发布过集愿,不可重复发布
		EN_GUILD_GATHER_DREAM_NOT_ROLE,					//抱歉,该玩家并未发布集愿信息
		EN_GUILD_GATHER_DREAM_NOT_GIFT,					//抱歉,该玩家已集齐集愿
		EN_GUILD_GATHER_DREAM_NOT_CAN_GIFT,				//抱歉,您今天已赠送过该玩家
		EN_GUILD_GATHER_DREAM_NOT_HAVE_GIFT_NUM,		//抱歉,您今天的集愿次数已满
		EN_GUILD_GATHER_DREAM_NOT_FETCH,				//抱歉,当前暂无可领碎片
		EN_GUILD_GATHER_DREAM_GIFT_MY,					//抱歉,你不能赠送给自己
		EN_GUILD_NAME_EMPTY,							//少侠，请输入家族名称
		EN_GUILD_GATHER_DREAM_NOT_ENOUGH,				//少侠,您当前持有的碎片数量不足哦~
		EN_GUILD_FREQUENTLY_QUILD,						//您今日频繁退出家族，请稍后再试
		EN_GUILD_MAX_BUILD_LEVEL,						//已达到建筑最高等级,无法再升级
		EN_GUILD_LIMIT_BUILD_LEVEL,						//家族大厅等级不足,请先升级大厅
		EN_GUILD_BUILDING_IS_BEING_BUILD,				//当前有建筑正在升级，请稍后再试。
		EN_GUILD_NOT_BUILDING_PERMISSIONS,				//此等家族大事，还是让族长、副族长来操作吧
		EN_GUILD_LACK_STUDY_LEVEL,						//书房等级不足,请先提升家族书房等级
		EN_GUILD_MEMBER_EXCEED,							//成员超过上限,无法拆除
		EN_GUILD_OTHER_BUILD_BUILDING,					//有建筑正在建造中,当前不可拆除
		EN_GUILD_NOT_GUILD_CAN_NOT_USE,					//当前未加入家族,无法使用
		EN_GUILD_CAN_NOT_REPEAT_OPEN_SPEED_UP,			//不可重复开启加速建造
		EN_GUILD_SPEED_UP_LIMIT_COUNT,					//今日开启加速次数已用完
		EN_GUILD_JOIN_SPEED_UP_BUILDING_LACK_COIN,		//铜币不足，无法加速建设
		EN_GUILD_CANNOT_REPEAT_SPEED_UP,				//您已经贡献了自己的力量，给他人留一些机会吧！
		EN_GUILD_SPEED_UP_FINISH,						//加速建设完成！
		EN_GUILD_SPEED_UP_SUCCESS,						//家族建设加速成功
		EN_GUILD_LAST_BUILDING_TIMES,					//今日建设次数已用完，请明日再来
		EN_GUILD_LEVEL_NOT_ENOUGH,						//家族等级不足


		//////////////////////////////////////// 场景 ////////////////////////////////////////////
		EN_SCENE_TRANSPORT_TOO_FAR = 500,				//距离传送点太远
		EN_SCENE_TRANSPORT_LIMIT_BY_ROLE_STATUS,		//角色处于不能传送的状态
		EN_FB_NOT_EXIST,								//副本不存在
		EN_FB_ENTER_FB_ERROR,							//进入副本错误
		EN_SCENE_TRANSPORT_FB_LIMIT,					//不允许从传送门移动到副本
		EN_CURR_LEVEL_NOT_ENTER_SCENE,					//当前等级无法进入
		EN_SCENE_UNLOCK_YET,							//该场景太过危险，请等级高些再来
		EN_MEMBER_CAN_NOT_ENTER_SCENE,					//队伍中有不能进入目标场景的队员
		EN_SPECIAL_CAN_NOT_MOVE_ON,						//受到特殊效果影响，不能移动
		EN_SCENE_FORBID_ENTER_TEAM,						//此场景或对方所处场景不允许有玩家创建或加入队伍
		EN_TARGET_SCENE_NO_SAME,						//对方与您处于不同的场景，操作失败
		EN_YOU_ARE_IN_FB_NOW,							//您在副本中，操作失败
		EN_THIS_SCENE_FORBID_LEAVE_GUILD,				//此场景不允许玩家离开家族
		EN_TARGET_SCENE_FORBID_LEAVE_GUILD,				//对方所处场景不允许其离开家族
		EN_MEMBER_LEVEL_NOT_ENTER_SCENE,				//队伍中有队员等级过低，无法进入该场景
		EN_ALREADY_IN_GUILD,							//您已有家族，操作失败
		EN_SCENE_EXCEED_MAX_LEVEL_LIMIT,				//抱歉,您的等级过高,无法进入该场景
		EN_SCENE_NOT_CAN_FAST_PASS_FB,					//当前场景不允许使用快速扫荡,请退出该场景后再试

		//////////////////////////////////////// 组队 ////////////////////////////////////////////
		EN_ROLE_NOT_IN_TEAM = 600,						//您不在队伍中，操作失败
		EN_ROLE_IN_TEAM,								//您在队伍中，操作失败
		EN_ROLE_NOT_LEADER,								//您不是队长，操作失败
		EN_TEAM_IS_FULL,								//队伍人数已满
		EN_TARGET_HAVE_TEAM,							//对方已有队伍
		EN_HAS_DISMISS_MEMBER_IN_TEAM,					//队中有暂离的玩家，操作失败
		EN_FOLLOW_MEMBER_NOT_ENOUGH,					//怪物非常强大，找几个朋友再来挑战吧
		EN_NOW_STATUS_UN_JOIN_TEAM,						//当前状态无法加入队伍
		EN_MEMBER_LEVEL_NOT_ENOUGH,						//队伍中有玩家等级过低
		EN_MEMBER_LEVEL_TOO_HIGH,						//队伍中有玩家等级过高
		EN_IN_TEAM_CAN_NOT,								//组队中不能执行该操作
		EN_MEMBER_GUILD_NOT_UNIFORM,					//队伍中有玩家加入了其它的家族，操作失败！
		EN_MEMBER_GUILD_NOT_EXIST,						//队伍中有玩家没有加入任何家族，操作失败！
		EN_TARGET_IS_NOT_LEADER,						//对方不是队长，操作失败
		EN_MATCH_CROSS_TEAM_NOT_EXIST,					//匹配队伍不存在或已解散，请返回原服再试
		EN_CROSS_TEAM_NOT_AVALIABLE,					//跨服组队匹配暂不可用，已为您开启本服组队匹配
		EN_TEAM_LEADER_CANT_CHANGE_POSITION,			//队长站位无法改变
		EN_TEAM_EMPTY_POSITION_CANT_SWITCH,				//无法与空位置互换站位
		EN_MEMBER_FUN_OPEN_LIMIT,						//队伍中有玩家尚未满足进入条件
		EN_ROLE_DONT_WANT_TO_JOIN,						//对方拒绝了您的组队邀请
		EN_TEAM_DISMISS,								//队伍已解散
		EN_TEAM_CAN_NOT_APPLY_JOIN,						//该队伍不接受入队申请
		EN_TEAM_NOT_EXIST,								//队伍不存在
		EN_TEAM_LEVEL_LIMIT,							//不符合队伍等级限制
		EN_CAN_NOT_INVITE_YOURSELF,						//不能邀请自己组队哦~
		EN_IN_BATTLE_CANNOT_TEMPORARILY_PART,			//战斗状态中无法暂离
		EN_LEVEL_LIMIT_CANNOT_GO_TO_LEADER_SCENE,		//等级不足无法进入队长所在地图
		EN_TEAM_MEMBER_LACK_OF_TICKET,					//队伍中有人缺少门票
		EN_TEAM_MEMBER_MUST_TEMP_APART,					//无法传送到队长位置，已为您暂离队伍
		EN_TEAM_LEADER_CANNOT_LEAVE_BATTLE,				//队长无法退出观战
		EN_TEAM_VOTE_ALREADY_VOTE,						//您已投票，无需重复投票
		EN_TEAM_VOTE_HAS_NO_VOTE,						//抱歉，您没有投票权
		EN_TEAM_VOTE_NO,								//未获得多数队员同意，申请失败
		EN_TEAM_VOTE_ROLE_IS_LEADER,					//您已是队长，无需发起投票
		EN_TEAM_VOTE_IS_ON,								//已有竞选在进行，请稍后！
		EN_TEAM_VOTE_IS_OFF,							//投票已结束
		EN_MEMBER_NOT_ENOUGH_LEVEL,						//队伍中有玩家等级未满足进入条件
		EN_ARRESTING_THIEVES_IS_SOLO_NOT_TEAM,			//缉捕盗贼只能单人参与，组队状态无法进入战斗
		EN_TEAM_MEMBER_STEP_OUT,						//当前有队员从队伍中暂离
		EN_YOU_HAVE_BEEN_KICKED,						//您已被移出队伍
		EN_SORRY_OTHER_SIDE_TEAM_DISMISS,				//抱歉,对方队伍已解散
		EN_SORRY_OTHER_SIDE_TEAM_FULL,					//抱歉,对方队伍已满员
		EN_TEAM_CAN_NOT_CHANGE_LEADER,					//抱歉,现在无法更换队长
		EN_TEAM_MEMBER_NOT_ENGOUH,						//抱歉,队伍人数不足
		EN_TEAM_MEMBER_NOT_TRANSPORT,					//队伍中有队员正在操作，传送失败
		EN_TEAM_MEMBER_NOT_BE_ALLOW_ENTER_BATTLE,		//有队员不符合进入条件
		EN_TEAM_MEMBER_LIMIT_PLAY_TIMES,				//有队员进入次数不足
		EN_CAN_NOT_TEMP_APART_IN_FB,					//在副本中不可暂离
		EN_TEAM_MEMBER_LIMIT_LEVEL_NOT_MEET,			//队伍中有玩家不满足队伍目标等级要求，操作失败
		EN_SORRY_OTHER_SIDE_TEAM_NOT_AVALIABLE,			//抱歉，对方队伍暂不可进入
		EN_TEAM_LEVEL_MORE_TEN,							//成员等级相差超过10级，无法进行挑战
		EN_CAN_NOT_NVITATE,								//当前状态无法进行组队邀请
		EN_TARGET_TEAM_NOW_NOT_AVALIABLE,				//目标队伍已进入副本场景，暂时无法加入，请稍后再试
		EN_TEAM_TOO_MANY_PEOPLE,						//队伍中人太多了，会打草惊蛇的！
		EN_TEAM_LEVEL_LOW_LIMIT,						//队伍最低等级必须小于等于您的等级
		EN_MEMBER_NOT_ONLINE_LEADER_NO_CHANGE,			//该玩家处于离线状态，无法成为队长
		EN_CROSS_MEMBER_LEAVE_TEAM,						//跨服玩家离开队伍
		EN_ENTER_SCENE_NOT_TEAM,						//抱歉，该场景无法组队进入
		EN_TEAM_IN_HUAN_JIE_MATCHING_CANN_ADD,			// 队伍正在幻界匹配中，无法加入
		EN_ROLE_IN_HUAN_JIE_MATCHING,					// 正在幻界匹配中，无法操作
		EN_TEAM_MEMBER_LEVL_NOT_ENOUGH,					// 队伍中有玩家等级不符
		EN_TEAM_NOT_EXIST_2,								//队伍已解散
		EN_TEAM_INVITE_CROSS_USER_SERVER_NOT_ALLOW,		// 该玩家不属于本服，请到跨服进行邀请
		EN_TEAM_INVITE_CROSS_USER_IN_BATTLE,			// 该玩家正在战斗中，请稍后再试
		EN_TEAM_CAN_NOT_JOIN_SHAN_HAI_DENG_HUI,			// 队伍正在进行灯会答题中,暂时不可加入
		EN_NETHER_TODAY_TIMES_LIMIT,					// 今天挑战次数已经使用完毕

		/////////////////////////////////////// 邮件系统 相关 ///////////////////////////////////////
		EN_HAS_NO_SPACING = 700,						//背包空间不足
		EN_ITEM_IS_NOT_EXIST,							//附件物品不存在
		EN_ITEM_NOT_FETCH_ATTACHMENT,					//少侠，有附件还没有领取哦
		EN_ONEKEY_FETCH_CD_LIMIT,						//一键领取太频繁，请稍候再试
		EN_KNASACK_NO_SPACE_SEND_MAIL,					//抱歉,您的背包空间不足,奖励将通过邮件发送

		///////////////////////////////////////// 战斗相关 /////////////////////////////////////////
		EN_MONSTER_IS_LOCK = 800,						//当前的怪物正在战斗中...
		EN_ROLE_NOT_SUIT_FOR_BATTLE = 801,				//角色当前无法战斗，请稍后再试
		EN_MEMBER_NOT_SUIT_FOR_BATTLE = 802,			//有队员当前无法战斗，请稍后再试
		EN_CUR_SCENE_NO_MINE_MONSTER = 803,				//当前场景没有暗怪
		EN_SERIALIZE_ROLE_BATTLE_DATA_FAILED = 804,		//序列化玩家战斗数据失败
		EN_UNSERIALIZE_ROLE_BATTLE_DATA_FAILED = 805,	//反序列化玩家战斗数据失败
		EN_ENEMY_ROLE_CAN_NOT_BATLLE = 806,				//对方当前无法战斗，请稍后再试
		EN_QIE_CUO_REQ_ALREADY_SEND = 807,				//请求已发送，请耐心等待回复
		EN_TARGET_NOT_IN_BATTLE = 808,					//目标不在战斗中，观战失败
		EN_TARGET_IS_IN_BATLLE = 809,					//对方正在战斗中，操作失败
		EN_SAME_TARGET_TWO_COMMAND_LIMIT = 810,			//同一目标最多只能被指定两种指令
		EN_OBSERVER_WATCH_USER_NOT_FOUND = 811,			//找不到要观战的人
		EN_TARGET_IS_OB_ING = 812,						//目标正在观战
		EN_ROLE_IS_OB_ING = 813,						//您正在观战，操作失败
		EN_CAN_NOT_LEAVE_OB_ON_THIS_MODE = 814,         //暂无法离开观战，请等待战斗结束
		EN_ROLE_NOT_OB_ING = 815,                       //您当前不在观战状态，操作失败
		EN_BATTLE_NOT_FOUND = 816,						//找不到本场战斗相关信息，操作失败
		EN_HONOR_FIGHT_FORBID_LEAVE_OB = 817,			//尚未参加战斗，请战斗后再试
		EN_IN_PREPARING_CROSS_STATUS_OB_LIMIT = 818,	//当前处于跨服准备状态，无法观战
		EN_MP_NOT_ENOUGH_PERFORM_SKILL_FAIL = 819,		//蓝量不足无法施放技能
		EN_HP_NOT_ENOUGH_PERFORM_SKILL_FAIL = 820,		//血量不足无法施放技能
		EN_COND_NOT_MEET_PERFORM_SKILL_FAIL = 821,		//条件不足无法施放技能
		EN_NOT_ALLOW_USE_MEDICINE = 822,				//无法使用药品
		EN_NOT_ALLOW_MODIFY_ANIMATION_SPEED = 823,		//抱歉，当前战斗无法开启2倍速
		EN_SKILL_REMAIN_CAST_TIMES_NOT_ENOUGH = 824,	//技能剩余释放次数不足
		EN_ONLY_LEADER_CAN_MODIFY_ANIMATION_SPEED = 825,//2倍速战斗只有队长才能开启哦

		////////////////////////////////////////// 交易行 //////////////////////////////////////////
		EN_TRADE_MARKET_SHELVE_FULL = 850,				//货架已满，请先整理
		EN_TRADE_MARKET_ADD_ON_SHELVE_FAILED,			//上架失败，请稍后再试
		EN_TRADE_MARKET_REMOVE_ITEM_NOT_FOUND,			//未找到需要下架的货品
		EN_TRADE_MARKET_CASH_OUT_FIRST,					//请先提现
		EN_TRADE_MARKET_CASH_OUT_ITEM_NOT_FOUND,		//未找到可提现的货品
		EN_TRADE_MARKET_ITEM_CAN_NOT_CASH_OUT,			//该货品无法提现
		EN_TRADE_MARKET_ITEM_CASH_OUT_FAILED,			//提现失败，请稍后再试
		EN_TRADE_MARKET_SELLER_NOT_FOUND,				//未找到该卖家，请稍后再试
		EN_TRADE_MARKET_ITEM_NOT_FOUND,					//这件商品已经下架了哦！请您选择其他商品
		EN_TRADE_MARKET_ITEM_ANNOUNCING,				//货品处于公示期，敬请期待
		EN_TRADE_MARKET_ITEM_NOT_ENOUGH,				//货品数量不足，请确认数量
		EN_TRADE_MARKET_ITEM_NOT_FOUND_FOR_AUTO_BUY,	//未找到所需的货品或商品已售罄
		EN_TRADE_MARKET_UNFOCUS_FAILED,					//取消关注失败，请稍后再试
		EN_TRADE_MARKET_FOCUS_ITEM_NOT_FOUND,			//没有找到要关注的物品，请刷新重试
		EN_TRADE_MARKET_ITEM_CAN_NOT_FOCUS_NOW,			//货品已下架
		EN_TRADE_MARKET_ITEM_FOCUS_FAILED,				//关注失败，请稍后再试
		EN_TRADE_MARKET_ITEM_SEARCH_TOO_FREQUENTLY,		//操作过于频繁，请稍后再试
		EN_TRADE_MARKET_FOCUS_ITEM_OFFSHELVE,			//该货品已售罄或已被下架
		EN_TRADE_MARKET_FOCUS_ITEM_ON_SALE,				//该货品已上架，请尽快前往抢购
		EN_TRADE_MARKET_NOT_ANOUNCE_OR_SALE,			//该货品尚未公示或发售
		EN_TRADE_MARKET_PRICE_TOO_LOW_TO_SALE,			//不可低于物品交易底价
		EN_TRADE_MARKET_CANNOT_FLIP_PAGE,				//摊位有限，其他商品正在排队
		EN_TRADE_MARKET_ITEM_TYPE_CANNOT_FOCUS,			//必须宠物、装备、灵饰才可以关注
		EN_TRADE_MARKET_REDO_ADD_ITEM_ON_SHELVE_FAILED,	//重新上架失败
		EN_TRADE_MARKET_ITEM_NOT_OFF_SHELVE,			//货品不在下架状态
		EN_TRADE_MARKET_ITEM_SOLD_OUT,					//货品已售罄
		EN_TRADE_MARKET_CROSS_CANNOT_OPERATE,			//当前处于跨服状态，无法进行当前操作
		EN_TRADE_MARKET_EQUIP_MUST_BE_TREASURE,			//只有珍品才能在交易行中交易
		EN_TRADE_MARKET_AUTOBUY_EXCEED_EXPECT_PRICE,	//商品已售罄或价格超过预期，放弃购买
		EN_TRADE_MARKET_ITEM_NUM_NOT_DEVIDED_EVENLY,	//购买或出售的数量必须整除商品的单位数量
		EN_TRADE_MARKET_ITEM_NUM_PER_GROUP_CHANGED,		//商品配置出现变化，请先取回物品再重新操作上架
		EN_TRADE_MARKET_PET_BIND_CAN_NOT_TRADE,			//抱歉，已绑定的宠物无法上架交易
		EN_TRADE_MARKET_PRICE_TOO_HIGH_TO_SALE,			//不可高于物品交易最高价
		EN_TRADE_MARKET_NEIWAN_CANNOT_SALE,				//无法进行出售，如有疑问请联系客服人员！
		EN_TRADE_MARKET_NEIWAN_CANNOT_BUY,				//无法完成购买，如有疑问请联系客服人员！
		EN_TRADE_MARKET_PRICE_NOT_IN_RANGE,				//售价不在允许范围内，请下架重试
		EN_CROSS_TRADE_MARKET_NOT_AVALIABLE_NOW,		//交易行暂不可用，请稍后再试
		EN_TRADE_MARKET_PET_PRICE_NOT_IN_RANGE,			//宠物售价不在合法范围内
		EN_ITEM_CANNOT_TRADE,							//该物品不可交易
		EN_TRADE_MARKET_ITEM_CAN_NOT_BUY,				//该物品不可购买

		///////////////////////////////////////// 活动相关 /////////////////////////////////////////
		EN_WORLD_BOSS_ALREADY_DEAD = 900,					//世界首领已被消灭
		EN_ACTIVITY_ISNT_IN_STANDY = 901,					//活动不处于准备状态，操作失败
		EN_COLOSSEUM_CAN_NOT_FIGHT = 902,					//您已进入古神宝库，不能再挑战
		EN_GAME_SEASON_ALREADY_END = 903,					//赛季已结算，战斗无效
		EN_WORLD_BOSS_2_ALREADY_DEAD = 904,					//巨龙领主已被消灭
		EN_COLOSSEUM_NOT_IN_RANK_LIST = 905,				//您不在宝库战斗列表中
		EN_RAND_ACTIVITY_NOT_ENOUGH = 906,					//您的次数不足
		EN_RAND_ACTIVITY_BOUGHT = 907,						//抱歉,您已购买过月度投资
		EN_RAND_ACTIVITY_NOT_BOUGHT = 908,					//抱歉,您还未购买月度投资,无法签到
		EN_RAND_ACTIVITY_EXPIRED = 909,						//抱歉,您的月度投资已过期
		EN_RAND_ACTIVITY_MAKE_NOT_ENOUGH = 910,				//抱歉,您的补签次数不足
		EN_RAND_ACTIVITY_PIGMENT_NOT_ENOUGH = 911,			//抱歉,您的花灵颜料不足
		EN_RAND_ACTIVITY_NOT_OPEN_ITEM_NOT_USE = 912,		//抱歉,活动尚未开启,该道具暂时无法使用
		EN_RAND_ACTIVITY_PICTURE_REACHED_THE_LIMIT = 913,	//抱歉,该图颜料使用已达上限
		EN_RAND_ACTIVITY_ANIMAL_RACE_ITEM_NOT_ENOUGH = 914,	//抱歉,您的道具不足,押注失败
		EN_RAND_ACTIVITY_ANIMAL_RACE_BET_LIMIT = 915,		//抱歉,您的押注数量已达上限,押注失败
		EN_RAND_ACTIVITY_ANIMAL_RACE_BET_TIME = 916,		//抱歉,当前时间不再接受押注
		EN_RAND_ACTIVITY_MAX_RECEIVE_NUM = 917,				//您已没有领取次数
		EN_RAND_ACTIVITY_LAYER_NOT_OPEN = 918,				//抱歉,当前关卡暂未开放
		EN_ACTIVITY_SIGN_UP_FAILD = 919,					//抱歉,您今天已报名参加该活动
		EN_ACTIVITY_WAITING_FOR_COUNTDOWN = 920,			//请稍等，倒计时结束后将进入战斗阶段
		EN_RAND_ACTIVITY_HAVE_DISCOUNT = 921,				//抱歉,您已抽取过折扣
		EN_RAND_ACTIVITY_BUY_NOT_ENOUGH = 922,				//抱歉,您已无购买次数
		EN_ACTIVITY_CROSS_SERVER_NOT_OPEN = 923,			//抱歉,跨服服务器暂未开启,请留意后续公告
		EN_ACTIVITY_OPEN_NOT_ENTER_NOTICE = 924,			//抱歉,该活动已正式开启,请在活动准备时前往
		EN_ACTIVITY_IS_STANDY_NOT_ENTER = 925,				//活动正在准备阶段，即将正式开启，请稍候
		EN_RAND_ACTIVITY_BUY_NOT_ENOUGH_TODAY = 926,		//抱歉,您今日已无购买次数
		EN_RAND_ACTIVITY_BUY_PASS_ENOUGH = 927,				//抱歉,超过限购次数
		EN_RAND_ACTIVITY_GOD_TOKEN_IS_ACTIVE = 928,			//高级战令已经激活
		EN_RAND_ACTIVITY_GOD_TOKEN_LEVEL_NOT_ENOUGH = 929,	//战令等级不足，请完成每日任务进行升级
		EN_ACTIVITY_HUANJIEZHANCHANG_HUASHEN_SYN_SUCC = 930,		//化身数据同步成功
		EN_RAND_ACTIVITY_COLLECT_WORD_NOT_FETCH = 931,		//抱歉,今日您已领取过了
		EN_RAND_ACTIVITY_GOD_TOKEN_PLEASE_ACTIVE = 932,		//您未激活高级战令
		EN_ACTIVITY_ENTER_TIME_LATER_NOT_ENTER = 933,		//抱歉，入场阶段已结束，请您下次参与
		EN_ACTIVITY_ARENA_IS_COLSE = 934,					//该赛场已关闭			
		EN_RAND_ACTIVITY_NOT_FINISH_BATTLE_TASK = 935,		//少侠似乎并未消灭完妖兽呢~
		EN_RAND_ACTIVITY_MAKE_UP_REWARD_FINISHED = 936,		//抱歉，您已集齐所有图鉴，不可重复集齐
		EN_RAND_ACTIVITY_MAKE_UP_CUR_TIME_NOT_USE = 937,	//抱歉，当前时间暂时无法使用该功能
		EN_RAND_ACTIVITY_SUMMER_LI_LIAN_LEVEL_LOCK = 938,			//请先通关上一个首领
		EN_RAND_ACTIVITY_SUMMER_LI_LIAN_LEVEL_TODAY_LOCK = 939,		//该首领今天尚未解锁
		EN_RAND_ACTIVITY_COLORFUL_LANTERN_OFFLINE_LOGIN = 940,		//在您离线期间内游戏时间已到
		EN_RAND_ACTIVITY_LUCKY_SIGN_REWARD_FETCHED = 941,			//抱歉,您已进行过每日占卜签到
		EN_RAND_ACTIVITY_COOL_BREEZE_NOT_REFRESH = 942,				//少侠，本任务仅可刷新一次哦~
		EN_RAND_ACTIVITY_LOTUS_MI_BAO_LIGHT_UP = 943,				//已经点亮过啦~
		EN_RAND_ACTIVITY_NOT_IN_ACTIVE_TIME = 944,					//不在活动时间内
		EN_RAND_ACTIVITY_LACK_ITEM = 945,							//抱歉，您的物品不足
		EN_RAND_ACTIVITY_UNFINISHED_TASK = 946,						//任务还未全部做完哦
		EN_RAND_ACTIVITY_GAME_IN_PROGRESS = 947,					//当前游戏进行中
		EN_RAND_ACTIVITY_CAN_NOT_REPEAT_LOCK = 948,					//无法重复锁定
		EN_RAND_ACTIVITY_LOCK_UPPER_LIMIT = 949,					//超过奖励可锁定数量，无法再锁定
		EN_RAND_ACTIVITY_PLEASE_LOCK_ALL_REWARD = 950,				//奖励锁定数量不足，无法祈愿
		EN_RAND_ACTIVITY_PET_CHALLENGE_SUCCESS = 951,				//该萌宠已经挑战成功，试试挑战其他萌宠吧
		EN_RAND_ACTIVITY_NO_RECEIVE_FISHERMAN_TASK = 952,			//请先前往渔夫处领取任务
		EN_RAND_ACTIVITY_PLEASE_SPECIFIED_TIME_RECEIVE = 953,		//请在指定时间领取~
		EN_RAND_ACTIVITY_FIND_SPRING_TASK_FINISH = 954,				//探春完成！世界很大，常去看看哦~
		EN_RAND_ACTIVITY_BO_ZHONG_YOU_LI_PLANT_LIMIT = 955,			//当前已种满,不可继续种植
		EN_RAND_ACTIVITY_BO_ZHONG_YOU_LI_NO_PLANTS = 956,			//当前没有种植植物或植物已全部成熟
		EN_RAND_ACTIVITY_BO_ZHONG_YOU_LI_PLANTS_IMMATURE = 957,		//该植物还未成熟,不可收获
		EN_RAND_ACTIVITY_CHENG_FENG_PO_LANG_ANSWER_LIMIT = 958,		//当前答题次数已用完,下次再来吧~
		
		EN_RAND_ACTIVITY_REWARD_ALL_RECEIVE_ALREADY,				//该任务奖励已全部领取~
		EN_RAND_ACTIVITY_PAI_MAI_HANG_END_OF_AUCTION,				//抱歉,该商品竞拍已经结束
		EN_RAND_ACTIVITY_PAI_MAI_HANG_NOT_BID_TIME,					//抱歉,当前不是竞拍时间
		EN_RAND_ACTIVITY_PAI_MAI_HANG_NOT_BID_OF_HIGH,				//抱歉,您当前出价已为最高价
		EN_RAND_ACTIVITY_PAI_MAI_HANG_BID_PRICE_CHANGE,				//价格已变更，请重新出价
		EN_RAND_ACTIVITY_PAI_MAI_HANG_BID_NOT_ENOUGH,				//抱歉,您的出价不足
		EN_RAND_ACTIVITY_PAI_MAI_HANG_DEPOSIT_NOT_ENOUGH,			//寄存元宝不足，已停止自动竞价
		EN_RAND_ACTIVITY_PAI_MAI_DEPOSIT_NEED_AUTO,					//首次寄存需要选自动竞拍
		EN_RAND_ACTIVITY_PAI_MAI_HANG_BID_GOLD_NOT_ENOUGH,			//抱歉,寄存元宝余额不足
		EN_RAND_ACTIVITY_PAI_MAI_HANG_RETRIEVE_FAILD,				//抱歉,所选取出元宝数额超过当前剩余寄存
		EN_RAND_ACTIVITY_JINGJI_YUEKA_LEVEL_NOT_ENOUGH,				//活动等级不足,请先提升等级
		EN_RAND_ACTIVITY_JINGJI_YUEKA_NOT_ACTIVE,					//您还未激活江湖引，请先激活
		EN_RAND_ACTIVITY_JINGJI_YUEKA_LEVEL_LIMIT,					//已达最大购买等级
		EN_RAND_ACTIVITY_JINGJI_YUEKA_TASK_FINISH_NOT_REFRESH,		//任务已完成，不可刷新
		EN_RAND_ACTIVITY_JINGJI_YUEKA_ACTIVED,						//您已激活江湖引,不可重复购买激活

		///////////////////////////////////// 活跃系统 //////////////////////////////////////////////
		EN_ACTIVE_SYSTEM_NOT_ENOUGH = 990,				//抱歉,您的活跃度不足,无法领取奖励
		EN_ACTIVE_SYSTEM_REWARD_TOO_MUCH = 991,			//抱歉,活动特殊道具超出范围

		/////////////////////////////////////////// 其他 ///////////////////////////////////////////
		EN_WST_COUNT_LIMIT = 1000,						//今日答题次数不足
		EN_WST_NO_BUY_EXP_COUNT,						//没有购买次数
		EN_SKILL_FULL,									//技能槽已满
		EN_SKILL_NOT_LEARN,								//技能尚未学习
		EN_SKILL_CANNOT_LEARN,							//技能不可用
		EN_SKILL_LEVEL_LIMIT,							//技能等级已到上限
		EN_SKILL_HAS_LEARNED,							//技能已学习
		EN_FIND_COUNT_NOT_ENOUGH,						//找回次数不足
		EN_FIND_REWARD_ERROR,							//没有找到奖励
		EN_NO_LEGITIMATE_TARGET,						//场上没有其他队伍可以施加状态
		EN_SELF_HAS_AMULET,						//你已被其他状态符影响
		EN_USE_AMULET_SUCC,						// 你已成功使用该状态符
		EN_REQ_ALREADY_TIMEOUT,							//请求已超时
		EN_NEED_SKILL_LEVEL_NOT_ENOUGH,					//所需要技能等级不足
		EN_REWARD_HAS_NOT_GET,							//还有奖励没有领取，操作失败
		EN_BOSS_ALREADY_DEFEAT,							//首领已被击败
		EN_WORLD_BOSS_NOT_ENOUGH_TEAM_MEMBER,			//异族魔将过于强大，请至少3人组队应战
		EN_WORLD_BOSS_2_NOT_ENOUGH_TEAM_MEMBER,			//上古巨龙过于强大，请至少3人组队应战
		EN_WORLD_BOSS_3_NOT_ENOUGH_TEAM_MEMBER,			//上古天神过于强大，请至少3人组队应战
		EN_PLATFORM_BATTLE_MAX_JOIN_NUM,				//已到最大的守擂数量
		EN_PLATFORM_BATTLE_CANT_CHALLENGE_RANK,			//不能挑战比自己排名低
		EN_MEDAL_YOU_HAVE_CERTIFICATE,					//您的勋章证明还未兑换，操作失败
		EN_MEDAL_MEMBER_HAVE_CERTIFICATE,				//有队员的勋章证明还未兑换，操作失败
		EN_MONSTER_WAVE_CANT_PLAY,					//今天的战斗已结束，请明天再来
		EN_CHAT_ITEM_INFO_NOT_EXIST,						//物品信息拉取失败
		EN_TEAM_MEMBER_NOT_ENOUGH_ENTER_TIMES,				//队伍中有成员的挑战次数不足
		EN_COURAGE_CHALLENGE_STAR_ENOUGH,					//抱歉，您的星数不足，领取失败
		EN_SHAN_HAI_BOSS_NOT_ENOUGH_TEAM_MEMBER,			//Boss过于强大，请至少5人组队应战
		EN_SHAN_HAI_BOSS_NOT_CHALLENGE_COUNT,				//不能重复挑战，请等下次刷新
		EN_MEDAL_SPECIAL_EXCHANGE_NOT_SPECIAL,				//抱歉,所选道具不是天赐令牌
		EN_LACK_AOE_AND_TREAT_PARTNER,						//当前缺少群攻、治疗伙伴，请注意阵容搭配
		EN_LACK_AOE_PARTNER,								//当前缺少群攻伙伴，请注意阵容搭配
		EN_LACK_TREAT_PARTNER,								//当前缺少治疗伙伴，请注意阵容搭配
		EN_NOT_LANTERN_IRDDIE_ANSWER_REWARD_COUNT,			//今日答题奖励次数已用完
		EN_NOT_LANTERN_IRDDIE_USED_SPECIAL_CARD,			//每题只可使用一次特殊卡
		EN_NOT_LANTERN_IRDDIE_ERROR_ANSWER,					//该答案已被排除~

		//////////////////////////////////////// 结婚 ///////////////////////////////////////////
		EN_MARRY_TARGET_MARRIED,						//对方已经结婚
		EN_MARRY_MUST_DIFFERENT_SEX_IN_TEAM,			//必须由一男一女组队才可申请
		EN_BABY_REFUSE_YOUR_REQ,						//对方拒绝了你
		EN_LOVER_BABY_NUM_MAX,							//对方的子女数量已满
		EN_LOVER_BABY_CREATING,							//对方正在生产中
		EN_LOVER_LEVEL_NOT_ENOUG,						//爱人等级不足
		EN_MARRIAGE_SEEKING_SUCCESS,					//宣言成功
		EN_MARRIAGE_SEEKING_MARRIED,					//您已经结婚，不能进行宣言
		EN_BABY_NUM_MAX,								//子女数量已满
		EN_LOVER_CAN_NOT_START_TASK,					// 对方次数不足
		EN_CAN_NOT_START_TASK,							// 次数不足
		EN_NOT_ONLY_LOVER_IN_TEAM,						// 只能和爱人组队开始
		EN_CAN_NOT_AUTO_FB,								// 已经进行过扫荡或波数不符合要求
			
		//////////////////////////////////////  好友、黑名单 相关    ////////////////////////////////////
		EN_FRIEND_NUM_MAX,								//好友人数已达到上限
		EN_FRIEND_EXIST,								//对方已是您的好友
		EN_CAN_NOT_MAKE_FRIEND_WITH_SELF,				//不能加自己为好友哦~
		EN_ROLE_NOT_ONLINE,								//对方不在线
		EN_FRIEND_OTHER_REJECT_REQ,						//对方拒绝了您的好友请求
		EN_FRIEND_OTHER_FULL,							//对方好友已满
		EN_FRIEND_CAN_ADD_IN_HIDEEN,							//跨服里不能加好友
		EN_FRIEND_BLESS_NOT_FRIEND,						//对方不是您的好友
		EN_FRIEND_BLESS_THANK_BLESS_TIMES_ZERO,			//回礼次数为零，不能继续回礼了
		EN_FRIEND_REMOVE_LOVER_LIMIT,					//不能和夫妻解除好友关系
		EN_ADD_BLACK_LOVER_LIMIT,						//不能将爱人加入黑名单
		EN_IN_YOUR_BLACKLISTS,							//对方在您的黑名单中
		EN_BLACKLISTS_NUM_MAX,							//黑名单已达上限
		EN_YOU_ARE_IN_BLACKLISTS,						//您在对方的黑名单中
		EN_ADD_BLACK_SHITU_LIMIT,						//不能将师徒加入黑名单
		EN_FRIEND_REMOVE_SHITU,							//不能和师徒解除好友关系
		EN_GIVE_FLOWER_TO_NO_FRIEND,					//不是好友不能送花
		EN_SINGLE_CHAT_ROLE_TO_BLACK,					//你已经将对方拉黑, 无法私聊
		EN_SINGLE_CHAT_BLACK_TO_ROLE,					//你已经被对方拉黑, 无法私聊
		EN_BLESS_ROLE_TO_BLACK,							//你已经将对方拉黑, 无法祝福
		EN_BLESS_BLACK_TO_ROLE,							//你已经被对方拉黑, 无法祝福
		EN_NOT_ENOUGH_FRIEND_INTIMACY,					//亲密度不足
		EN_TARGET_ROLE_IS_PROPOSE,						//对方正在求婚中
		EN_FRIEND_NOT_REFRESH,							//少侠手速太快了，过几分钟再来试试看哦
		EN_FRIEND_NOT_HAS_OTHER_FRIEND,					//暂无其他可推荐的好友，少侠可以等等再来看哦~

		EN_CAREER_ADVANCE_PRE_PROF_LIMIT,				//前置职业不正确
		EN_CARRER_ADVANCE_UNKNOWN_PROF,					//找不到该职业
		EN_CAREER_ADVANCE_PRE_TASK_LIMIT,				//前置任务未完成
		EN_CAREER_ADVANCE_TITLE_LIMIT,					//前置称号未拥有
		EN_CAREER_ADVANCE_CAPABILITY_NOT_WORTH,			//目标评分未完成

		///////////////////////////////////////// 师徒 /////////////////////////////////////////
		EN_SHITU_TARGET_NOT_FRIEND,						//抱歉，不能和非好友缔结师徒关系
		EN_SHITU_TARGET_APPRENTICE_FULL,				//对方已经有足够多的徒弟了
		EN_SHITU_APPRENTICE_FULL,						//您已经有足够多的徒弟了
		EN_SHITU_ALREADY_HAS_MASTER_FULL,					//您的师傅已满
		EN_SHITU_TARGET_ALREADY_SHITU,					//对方已经和您缔结了师徒关系
		EN_SHITU_TARGET_ALREADY_HAS_MASTER,				//对方师父已经满了
		EN_SHITU_TARGET_REJECT_REQ,						//抱歉，对方拒绝了你的请求
		EN_SHITU_ROLE_LEVEL_NOT_SATISFY,				//对方未达到师傅等级

		EN_CANNOT_USE_ITEM_IN_TEAM,						//组队中不能使用
		EN_ESCORT_NOT_CAN_ENTER_FB,						//运镖中不能进入副本
		EN_ESCORT_NOT_CAN_JOIN_TEAM,					//运镖中不能加入队伍
		EN_ESCORT_OVERTIME,								//少侠未能按时将镖车运到，镖局现已收回镖车

		///////////////////////////////////////// 法宝 /////////////////////////////////////////
		EN_FABAO_NO_THIS_FABAO,							//没有这个法宝
		EN_FABAO_HAS_ACTIVE_FABAO,						//法宝已激活
		EN_FABAO_NO_ACTIVE_FABAO,						//法宝未激活
		EN_FABAO_LEVEL_MAX_ERR,							//这个法宝已经升到最高级
		EN_FABAO_WEEK_UPLEVEL_ERR,						//本周法宝升级次数达到上限
		EN_FABAO_LEVEL_ERR,								//请先进阶法宝,才能继续升级
		EN_FABAO_JIE_MAX_ERR,							//法宝已经达到最高阶
		EN_FABAO_LEVEL_NEED_ERR,						//法宝未达到进阶等级
		EN_FABAO_PASSIVE_NEED_ERR,						//非被动法宝不能装备到法宝装备栏
		EN_FABAO_NO_EQUIP_ERR,							//该法宝装备栏未装备法宝
		EN_FABAO_DECOMPOSE_DRESSED_ERR,					//装备中的法宝无法分解，请脱下法宝在尝试
		EN_FABAO_DECOMPOSE_NO_DATA_ERR,					//该法宝无法分解
		EN_FABAO_RAND_ATTR_ERR,							//请先进阶法宝,才能继续洗炼

		EN_FB_STUFF_NOT_ENOUGH,							//少侠缺少副本材料，请收集材料之后再来挑战
		EN_FB_ROLE_LEVEL_NOT_ENOUTH,					//少侠等级不足，可通过支线任务，活动，成就，挂机等方式提升等级

		EN_FABAO_PRE_FABAO_NO_ACTIVE_ERR,				//前置法宝未激活

		EN_CHAT_FREQUENTLY,			//发言太频繁
		EN_CHAT_CAN_NOT_CHAT_WITH_YOURSELF,				//不能和自己聊天哦~

		EN_TEAM_FIGHT_BOX_ALREADY_FETCH,				//宝箱已被领取，不可重复领取
		EN_BRAVE_GROUND_OVER_USE_NPC_RECOVER_TIMES,			//恢复效果已经使用完
		EN_BRAVE_GROUND_USE_NPC_RECOVER,			//恢复成功
		EN_TODAY_CHALLENGE_TIMES_NOT_ENOUGH,			//今日挑战次数不足，少侠请明日再来挑战
		EN_GATHER_TIME_NOT_ENOUGH,						//采集时间不足采集失败

		EN_PET_IN_PLATFORM_BATTLE,			//宠物正在守擂中，操作失败
		EN_PET_USE_EXP_PILL,						//少侠，宠物等级无法超过角色等级哦！
		EN_ATTR_HAS_FULL_ERR,						//状态已满，不需要再吃药了
		EN_TEAM_AVG_LEVEL_NOT_ENOUGH,					//队伍平均等级过低
		EN_TEAM_AVG_LEVEL_TOO_HIGH,						//队伍平均等级过高
		EN_TASK_QIYU_TIMES_MAX,							//今天已遇到太多奇遇，奇遇太多就不出奇了，请明天再来试试

		EN_SCHOOL_TASK_DAILY_NUM_LIMIT,					//今天接取任务数量已到上限
		EN_GUILD_INVITE_NO_AUTHORITY,					//需要长老及以上职位才可邀请他人
		EN_EQUIPMENT_VOC_CHANGE_LEVEL_LIMIT,			//高级装备词条不能熔炼到低级装备上
		EN_EQUIPMENT_VOC_NOT_SAME_VOC_ERR,				//一件装备无法同时拥有2个相同的属性

		EN_CONVERT_SHOP_BUY_TIMES_LIMIT,				//将超过该物品的最大可兑换次数，无法兑换
		EN_CONVERT_SHOP_RESOURCE_NOT_ENOUGH,			//所需资源不足，无法兑换
		EN_CONVERT_SHOP_BUY_ITEM_NOT_SAME,				//兑换的物品与配置不同，请重登游戏更新内容再试
		EN_MARKET_BUY_MY_SELLER,						//该商品是自己上架物品
		EN_MARKET_SALE_EQUIP_HAS_STONE,					//镶有宝石的装备无法上架交易行
		EN_ACTIVITY_SKY_DROP_GATHER_ERR,				//组队状态无法开启宝箱，请解散队伍单人开启
		EN_ACTIVITY_CHIEF_PEAK_BATTLE_BET_NOT_IN_CROSS,	// 只能在跨服里下注

		EN_BOSS_CHAPTER_BOSS_NOT_FOUND,					// 没有找到该怪物组
		EN_BOSS_CHAPTER_BOSS_NOT_KILLED,				// 尚未打败该怪物组
		EN_BOSS_CHAPTER_REWARD_CLAIMED,					// 奖励已领取
		EN_BOSS_CHAPTER_CHAPTER_BOSS_NOT_ALL_KILLED,	// 尚未打败该章节所有怪物组
		EN_BOSS_CHAPTER_BOSS_KILLED,					// 该怪物组已被打败
		EN_BOSS_CHAPTER_TASK_NOT_ACCEPTED,				// 任务未接，不可挑战该怪物组
		EN_TASK_CHAIN_PET_ERR,							// 上交宠物失败
		EN_TEAM_NOT_READY_ERR,							// 队伍中有玩家尚未准备好

		EN_MEDAL_FB_OVER_MEMBER,							// 法阵不稳定，无法承载多人同行
		EN_MEDAL_FB_LESS_MEMBER,							// 该区域极为凶险，建议3人组队再来
		EN_SPECIAL_ADVANCE_FB_PASS_ERR,					// 物品收集不足，无法启动传送阵
		
		EN_CAN_NOT_REPLACE_PET_SKILL,					// 有更强大的元素法术存在
		EN_REPEAT_PET_SKILL,					// 已经有相同技能了
		EN_RESTIGE_ERR_ITEM_LEVEL,						// 当前捐赠中包含不符合捐赠装备等级的装备，操作失败
		EN_RESTIGE_ERR_ITEM_QUALITY,					// 当前捐赠中包含不符合捐赠装备品质的装备，操作失败
		EN_RESTIGE_ERR_NOT_ENOUGH,						// 当前捐赠次数已达上限,操作失败

		EN_ITEM_REQUIRE_COLOSSEUM,						// 该商品需要更高的竞技场排名
		EN_PRESTIGE_NOT_ENOUGH,							// 该商品需要更高的声望
		EN_GATHER_POWER_FULLY,							// 活力已满

		EN_KNAPSACK_ITEM_COLUMN_NO_SPACE,               // 背包物品栏空间不足，请先清理背包
		EN_KNAPSACK_SUPPLY_COLUMN_NO_SPACE,             // 背包补给栏空间不足，请先清理背包
		EN_KNAPSACK_TASK_MATERIAL_COLUMN_NO_SPACE,      // 背包材料栏空间不足，请先清理背包
		EN_KNAPSACK_FRAGMENT_COLUMN_NO_SPACE,           // 背包碎片栏空间不足，请先清理背包
		EN_KNAPSACK_ILLUSTRATION_COLUMN_NO_SPACE,       // 背包图鉴栏空间不足，请先清理背包
		EN_KNAPSACK_STORAGE_COLUMN_NO_SPACE,            // 仓库空间不足，请先清理
		EN_KNAPSACK_TEMPERARY_COLUMN_NO_SPACE,          // 临时背包空间不足，请先清理

		EN_BATTLE_MODE_CANNOT_GIVEUP,                   // 该战斗模式无法放弃
		EN_BATTLE_ONLY_LEADER_CAN_GIVEUP,               // 只有队长才能放弃
		EN_BATTLE_AFTER_ROUND3_CAN_GIVEUP,              // 第3回合后可以放弃
		EN_BATTLE_PVP_CANNOT_GIVEUP,					// PVP下无法放弃
		EN_BATTLE_ONLY_TEAMUP_CAN_GIVEUP,				// 只有组队情况下才能放弃
		EN_EQUIPMENT_VOC_LOCK_ERR,						// 锁定的属性无法熔炼
		EN_ROLE_CHAT_GROUP_MAX,							// 加入群组数量已到达上限
		EN_ROLE_CHAT_GROUP_CREATE_FAIL,					// 创建聊天群组失败
		EN_CRATER_LEAVE_CHAT_GROUP_FAIL,				// 群组创建者无法离开聊天群组，请使用解散群组
		EN_ROLE_CHAT_GROUP_DISMISS_RIGHT,				// 权限不足，创建者方可解散群组
		EN_BE_INVITE_MEMBER_WAS_IN_GROUP,				// 被邀请对象已经加入本群
		EN_ROLE_CHAT_GROUP_KICK_RIGHT,					// 权限不足，创建者方可踢出成员
		EN_ROLE_CHAT_GROUP_INVALID,						// 加入的群组不存在

		EN_BATTLE_VIDEO_NOT_FOUND,						// 暂无可播放的录像
		EN_RAND_ACTIVITY_EXCHANGE_UNLOCK,				// 条件未满足无法兑换
		EN_RAND_ACTIVITY_EXCHANGE_ITEM_ERR,				// 兑换材料不足
		EN_RAND_ACTIVITY_EXCHANGE_TIMES_ERR,			// 兑换次数不足
		EN_RA_TIAN_MING_DIVINATION_MAX_ADD_LOT,			// 该竹签加注已满
		EN_EQUIPMENT_HAS_STONE_ERR,						// 部分装备请卸下宝石后再进行捐赠
		EN_EXHANGE_ITEM_TIME_HAS_PASSED,				// 物品兑换时间已过
		EN_TEAM_AUTO_JOIN_FAIL,							// 有队员连接跨服失败
		EN_COLLECTION_INCLUDED_FAIL,					// 尚未收录无法激活
		EN_COLLECTION_INCLUDED_AGAIN,					// 已经收录完成无法再次收录
		EN_TARGET_FIGHT_PROTECT,						// 对方处于30秒战斗保护中，无法挑战
		EN_MY_FIGHT_PROTECT,							// 你正处于30秒战斗保护中，无法挑战
		EN_ADV_PROF_ERR_PRESTIGE_LIMIT,					// 声望等级不足
		EN_RA_SPORTS_HISTORY_ERR,						// 历史排名未达到，无法领取
		EN_MAZE_RACER_FIERCE_BATTLE,					// 受激战卡影响，即将进入战斗
		EN_ROLE_CHAT_GROUP_FULL,						// 抱歉，对方群组人数已满
		EN_ROLE_CHAT_GROUP_INVITE_ERR,					// 非跨服群组不允许在跨服邀请别人
		EN_PET_CAN_NOT_SALE_EQUIP,						// 卸下宠物装备后可交易
		EN_FUNC_PREVIEW_ALREADY_CLAIMED,				// 预告奖励已领取
		EN_PET_HSA_EQUIPMENT_ERR,						// 宠物穿戴护符中操作无效
		EN_TREASURE_CHEST_IS_OPEN,						// 宝箱已被开启
		EN_TASK_CHAIN_COMMIT_PET_ERROR,					// 抱歉，该宠物不是1级宠物，提交失败
		EN_EXPELMONSTER_NO_REWARD_TIMES,				// 今日奖励已达上限
		EN_CHAT_GROUP_KICK_OUT_ERR,						// 踢出他人不在同一个群组
		EN_RAND_ACTIVITY_LOGIN_COUNT_ERR,				// 登陆天数不足
		EN_CAN_NOT_REPLACE_PET_SKILL_LOCKED,			// 要替换的技能已经锁定
		EN_FORMATION_NOT_AVAILABLE,						// 该阵容尚未解锁
		EN_CUR_FORMATION_CANNOT_DELETE,					// 当前正在使用该阵容，无法删除
		EN_HEADSHOT_ADVANCE_TIMES_LIMIT,				// 尚未满足进阶次数条件		
		EN_HEADSHOT_SHIZHUANG_LIMIT,					// 尚未解锁对应时装
		EN_TASK_CHAIN_WAIT_LIMIT,						// 物品尚未准备好，请耐心等待
		EN_TEAMMANAGER_INV_ERR_DIF_GUILD,				// 抱歉，你和对方的家族不一致
		EN_CHIEF_ELECTION_SEA_CANT_ENTER,				// 进场时间已过
		EN_MONEY_LUCK_COIN_NOT_ENOUGH,					// 气运值不足，无法进行兑换
		EN_NO_EMPTY_SLOT_FOR_NEW_FORMATION,				// 暂无空位保存新阵容
		EN_PET_IS_TRAINNING_OPER_FAIL,					// 宠物正在训练中，操作失败
		EN_GUILD_FIGHT_MEMBER_GOTO_LEADER_SCENE_FAIL,	// 活动已开启，无法传送到队长所在场景	
		EN_JOIN_TEAM_ACTIVITY_LIMIT,					// 活动已开启，无法加入该队伍
		EN_SHAN_HAI_LIAN_HUA_ERROR,						// 需要消耗各类灵魂碎片1片
		EN_ACTIVITY_OPEN_OPER_FAIL,						// 活动已开启，操作失败
		EN_TEAMMANAGER_INV_ERR_NO_GUILD,				// 您目前暂未加入家族，操作失败
		EN_TEAMMANAGER_INV_ERR_TAR_NO_GUILD,			// 抱歉，对方目前暂未加入家族
		EN_ROLE_CHAT_GROUP_IN,							// 您已经是该群聊的成员了
		EN_ESCORT_CANNOT_TEAM_MATCH,					// 正在运镖，无法进行队伍匹配
		EN_ESCORT_CANNOT_FIGHT,							// 运镖中不可进入战斗
		EN_CAN_NOT_REPLACE_PET_SKILL_LOCKED_2,			// 只能学习一种元素被动技能
		EN_SHAN_HAI_KEY_NOT_ENOUGH,						// 抱歉，您缺少挑战钥匙
		EN_CHAT_INV_TIME_LIMIT,							// 抱歉，1分钟内只能邀请一次
		EN_TEAM_CREATE_SCENE_ERR,						// 本场景只能创建以家族大乱斗为目标的队伍
		EN_FB_MANAGER_ENTER_TEAM_TYPE_ERR,				// 队伍类型与副本类型不符合
		EN_CHAT_GROUP_INV_FULL_ERR,						// 抱歉，群组人数已满
		EN_CAN_NOT_MAKE_SHI_TI,							// 只能和本服的玩家组成师徒
		EN_FUZZY_SEARCH_NOT_FOUND,						// 输入的昵称或ID不存在
		EN_CHAT_ITEM_INFO_LOST,							// 道具信息已过期
		EN_ACTIVITY_TEAM_FIGHT_ENEMY_ALL_OFFLINE,		// 因敌方战斗人员全部掉线，本方自动获胜
		EN_GUILD_HONOR_BATTLE_CANT_ENTER,			// 不在入场时间不能进入
		EN_ROLE_CRYSTAL_FB_ENERGY_NOT_ENOUGH,			// 体力不足
		EN_CHAT_GROUP_CROSS_CANNOT_OPERATE,				// 当前处于跨服状态，无法进行当前操作
		EN_CHAT_GROUP_POWER_LIMIT,						// 权限不足
		EN_ANECDOTE_COMMIT_ITEM_MISMATCH,				// 这不是任务所需物品哦
		EN_ANECDOTE_ALREADY_SUCC,						// 该任务已成功完成，无法再次尝试
		EN_ANECDOTE_TASK_NOT_ACCEPTED,					// 该任务尚未接取，操作失败
		EN_ANECDOTE_TASK_NOT_FINISH,					// 尚未完成任务，无法领取奖励
		EN_ANECDOTE_TASK_ALREADY_CLAIM_REWARD,			// 奇闻奖励已领取，操作失败

		EN_DRAW_MANAGER_HAS_BEEN_SELECT,				// 该奖励已经被选择
		EN_DRAW_MANAGER_SELECTING,						// 有队员尚未选择奖励，请稍后再试
		EN_NEW_FORMATION_LIMIT_WITH_LEADER,				// 队长无法保存新阵型 
		EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT,		// 购买次数不足
		EN_RA_COW_EXCHANGE_JIFEN_LIMIT,					// 未达到兑换积分段
		EN_RA_COW_LUCKY_EXCHANGE_JIFEN_LIMIT,			// 积分不足
		EN_RA_LITTLE_YEAR_GIFT_FAIL_NPC,				// 找错人了
		EN_RA_COW_HAS_REWARD_ERR,						// 你已经拿过奖励了
		EN_RA_COW_LUCKY_EXCHANGE_IND_LIMIT,				// 亲密度不足

		EN_NOW_ESCORTING_NO_MORE_ACCEPT_ESCORT_TASK,	// 当前正在运镖，无法接取该任务

		EN_GIVE_SECRET_KEY_HAS_GET_REWARD,				// 奖励已经领取过了
		EN_GIVE_SECRET_KEY_PARAM_NOT_ADV,				// 条件未达成，无法领取
		EN_GIVE_SECRET_KEY_NO_SPACE,					// 背包已满，请清理背包
		EN_USE_PET_EXP_ITEM_ERR_WITHOUT_BREAK_LEVEL,	//抱歉，需要进行宠物突破后方可使用
		EN_GUILD_PET_TRAINING_ERR_WITHOUT_BREAK_LEVEL,	//抱歉，需要进行宠物突破后方可训练
		EN_CUMULATIVE_RECHARGE_HAS_GET_REWARD,			// 奖励已经领取过了
		EN_CUMULATIVE_RECHARGE_ACC_PRICE_ERR,			// 累计充值金额不足
		EN_LEVELCOMPLEMENT_EXP_NOT_ENOUGH_ERR,			// 抱歉，当前存储经验不足
		EN_PARTNER_UP_LEVEL_OVER_ROLE_LEVEL,			// 抱歉，伙伴等级不允许超过角色当前等级
		EN_PET_LOOT_REWARD_HAS_FATCH_ERR,				// 奖励已经领取过了
		EN_PET_LOOT_TASK_NOT_ALLOW,						// 任务未达成无法领取
		EN_DREAM_NOTES_CHALLENGE_NOT_CHALLENGE,			// 请先通关前置关卡后再来尝试
		EN_DREAM_NOTES_CHALLENGE_NOT_TIMES,				// 今天快速扫荡次数已用完 
		EN_DREAM_NOTES_CHALLENGE_NOT_FAST_PASS,			// 通关本关后解锁本关快速扫荡
		EN_USE_CRYSTAL_FB_ENERGY_ITEM_UPPER_LIMIT,		// 体力已达上限
		EN_CRYSTAL_FB_ENERGY_UPPER_LIMIT,				// 秘境体力值已满
		EN_ULTIMATE_MAX_ERR,							// 已经达到最大值
		EN_WORKSHOP_CAN_USE_THIS_EQUIP,					// 这件装备无法进行这个操作
		EN_GOD_STRONGTHEN_NULL,							// 这件装备无法进阶
		EN_DREAM_NOTES_CHALLENGE_BUY_TIMES_LIMIT,		// 抱歉，您今日额外购买次数已用完
		EN_DREAM_NOTES_CHALLENGE_BUY_ERROR,				// 抱歉，您的货币不足，购买失败
		EN_WORKSHOP_HAS_BE_CREATED_ERR,					// 该装备已经制造过了无法再次制造
		EN_GOD_UP_GREADE_NULL,							// 这件装备无法强化
		EN_NO_INS_NUM,									// 无词条可以激活
		EN_ROLE_CHAT_GROUP_ALREADY_IN,					// 抱歉，您已在该群组中
		EN_ORIGIN_SERVER_TRADE_MARKET_BUY_NOT_AVAILABLE,// 原服购买交易所商品功能暂时关闭，请到跨服中操作
		EN_ORIGIN_SERVER_TRADE_MARKET_UPSHELVE_NOT_AVAILABLE,// 原服上架交易所商品功能暂时关闭，请到跨服中操作
		EN_SCHOOL_EXERCISE_UNLOCK_PREVIOUS_LIMIT,		// 通关上一难度后可挑战
		EN_ALREADY_CATCH_PET_BEFORE,					// 此前已抓取过宠物，操作失败
		EN_NO_CHECK_ITEM_NO_KNAPSACK,					// 背包已满，鉴定失败
		EN_MIJINGQIWEN_BUY_NO_INDEX,					// 购买失败，无此道具
		EN_MIJINGQIWEN_BUY_TIME_LIMIT,					// 购买次数限制
		EN_MIJINGQIWEN_ANSWER_QUESTION_ERR,				// 答题错误
		EN_NEED_UPDATE_CFG_VER,							// 客户端不是最新，请重启后再试
		EN_CROSS_NOT_ALLOW_COLOSSEUM_FIGHT,				// 抱歉，进行竞技场挑战需要返回原服
		EN_CROSS_NOT_ALLOW_COLOSSEUM_EVERY_REWARD,		// 抱歉，竞技场每日奖励需要返回原服领取
		EN_NEED_BOTH_IN_CROSS_TO_ADD_FRIEND,			// 需双方同时进入跨服方可添加跨服玩家为好友
		EN_RETURN_ORIGIN_SERVER_IN_BATTLE_LIMIT,		// 当前正在战斗中，无法返回原服，请在战斗结束后再试
		EN_COLOSSEUM_ACHIEVEMENT_HAS_GET,				// 成就奖励已经领取过了
		EN_COLOSSEUM_ACHIEVEMENT_NO_COMPLETE,			// 成就未完成
		EN_REALM_BREAK_PERFECT_SPIRITUAL_PRACTIVCE,		// 修行已圆满
		EN_REALM_BREAK_BREAK_EXP_FULL_TUNA,				// 请先行突破,然后再来吐纳 
		EN_REALM_BREAK_BREAK_EXP_FULL_USE_ITEM,			// 请先行突破,然后再来使用灵力丹
		EN_REALM_BREAK_BREAK_EXP_NOT_ENOUGH,			// 抱歉,您的灵力值不足,无法进行突破
		EN_REALM_BREAK_SCENE_TYPE_FAILD_NOT_BREAK,		// 当前场景无法突破,请退出该场景之后再次尝试
		EN_REALM_BREAK_FETCH_EROOR,						// 请努力修炼之后,再来领取
		EN_REALM_BREAK_TUNA_NUM_NOT_ENOUGH,				// 每日0点刷新之后获得吐纳次数
		EN_PET_MONOPOLY_GRID_NO_REWARD,					// 暂无奖励物品可领取
		EN_PET_MONOPOLY_GRID_NO_PET_TO_CATCH,			// 暂无宠物可抓
		EN_MATERIAL_PASS_TIME_NOT_ENOUGH,				// 今日剩余挑战次数已用完
		EN_MATERIAL_LEVEL_NOT_PASS_BUY_ERROR,			// 尚未通关本关卡,无法购买额外挑战次数
		EN_MATERIAL_LEVEL_BUY_NOT_ENOUGH,				// 该关卡今日额外购买次数已获取
		EN_ROLE_ADD_EXP_EXCEED_LEVEL_LIMIT,				// 经验已超出等级上限
		EN_PARTNER_PAGE_COMPOSE_ITEM_NOT_ENOUGH,		// 数量不足，请少侠集齐后再来合成吧
		EN_GUILD_INBORN_PREPOSEL_SKILL_NOT_LEARN,		// 前置技能未学习，无法学习该技能
		EN_QIONG_LOU_YU_YAN_NOT_YAN_HUI,				// 当前不是宴会时间
		EN_QIONG_LOU_YU_YAN_NO_EAT_TIMES,				// 您已经吃饱啦，再吃就要吃撑啦~
		EN_QIONG_LOU_YU_YAN_NO_PREPARATION_TIMES,		// 现在不是筹备宴会时间，提交失败
		EN_QIONG_LOU_YU_YAN_NO_JOIN,					// 您未参与筹备无法参加宴会
		EN_QIONG_LOU_YU_YAN_CANNT_REWARD,				// 您未参与筹备，无法领取哦
		EN_SHOP_BUY_ITEM_FAILED_BY_SOAR_COUNT_NOT_ENOUGH,// 该物品需要角色飞升后才可购买


		////////////////////////////////////// 灵骑系统 /////////////////////////////////////////////
		EN_SMART_MOUNTS_SYSTEM_ACTIVE_ITEM_NOT_ENOUGH,	// 解锁道具不足
		EN_SMART_MOUNTS_SYSTEM_NOT_UNLOCKED,			// 抱歉,该灵骑尚未解锁,操作失败
		EN_SMART_MOUNTS_SYSTEM_SKILL_POINT_NOT_ENOUGH,	// 技能点不足
		EN_SMART_MOUNTS_SYSTEM_ENERGY_FULL,				// 抱歉,该灵骑的精力值已满
		EN_SMART_MOUNTS_SYSTEM_PET_DOMINATION,			// 该宠物已被统御
		EN_SMART_MOUNTS_SYSTEM_NOT_FINISH_BREACH,		// 抱歉,该灵骑还未进行突破,无法继续升级
		EN_SMART_MOUNTS_SYSTEM_LEVEL_FULL,				// 抱歉,该灵骑等级已达上限,无法继续升级
		EN_SMART_MOUNTS_SYSTEM_REVEAL_LEVEL_FULL,		// 抱歉,该灵骑点化等级已达上限,无法继续升级
		EN_SMART_MOUNTS_SYSTEM_ALREADY_BRAECH,			// 该灵骑已突破,请勿重复操作
		EN_SMART_MOUNTS_SYSTEM_BREACH_FAILD,			// 突破失败
		EN_SMART_MOUNTS_SYSTEM_UNLOCKED,				// 抱歉,该灵骑已解锁,请勿重复操作
		EN_SMART_MOUNTS_SYSTEM_LOCK,					// 抱歉,该技能还未解锁,操作失败
		EN_SMART_MOUNTS_SYSTEM_SKILL_LEVEL_FULL,		// 抱歉,该技能等级已达上限,无法继续升级
		EN_SMART_MOUNTS_SYSTEM_NOT_REFINE,				// 没有可洗练的技能格子
		EN_SMART_MOUNTS_SYSTEM_NO_REFINING,				// 抱歉,您还未进行洗练,保存失败
		EN_SMART_MOUNTS_SYSTEM_SKILL_LEVEL_MIN,			// 当前等级无法重置
		EN_SMART_MOUNTS_SYSTEM_UNLOCK_COND_NOT_MEET,	// 解锁条件未达标
		EN_SMART_MOUNTS_SYSTEM_NATURE_LIMIT,			// 灵性已达最大值
		EN_SMART_MOUNTS_SYSTEM_JEWELRY_NOT_ACTIVE,		// 抱歉,该灵骑没有穿戴饰品
		EN_SMART_MOUNTS_SYSTEM_EQUIP_ACTIVED,			// 抱歉,该装备栏已被激活
		EN_SMART_MOUNTS_SYSTEM_EQUIP_NOT_ACTIVE,		// 抱歉,该装备栏尚未激活
		EN_SMART_MOUNTS_SYSTEM_EQUIP_MAX_LEVEL,			// 抱歉,该装备强化等级已满
		EN_SMART_MOUNTS_SYSTEM_JEWELRY_MAX_LEVEL,		// 抱歉,该饰品等级已满
		EN_SMART_MOUNTS_SYSTEM_HOLY_ACTIVED,			// 抱歉,该灵骑的圣物已被激活
		EN_SMART_MOUNTS_SYSTEM_HOLY_NOT_ACTIVE,			// 抱歉,该灵骑的圣物尚未激活
		EN_SMART_MOUNTS_SYSTEM_HOLY_MAX_LEVEL,			// 抱歉,该灵骑的圣物强化等级已满
		EN_SMART_MOUNTS_SYSTEM_SKILL_UP_FAILED,			// 当前灵技卷轴等级不足

		EN_LUNHUIWANGCHUAN_REWARD_HAS_GET,				// 奖励已经领取过了
		EN_LUNHUIWANGCHUAN_HAS_PASS_GET,				// 已经通关过了
		EN_LUNHUIWANGCHUAN_NOT_PASS,					// 未通关，无法领取奖励

		EN_PET_MONOPOLY_NOT_ON_ENDING_POINT,			// 当前不在终点，操作失败
		EN_PET_MONOPOLY_CUR_GRID_HAS_REWARD,			// 当前格子还有奖励哦
		EN_PET_MONOPOLY_CUR_GRID_NO_TURNTABLE,			// 当前格子没有转盘哦
		EN_PET_MONOPOLY_PET_NOT_EXPECTABLE,				// 该宠物不在可祈福列表中
		EN_PET_MONOPOLY_ON_ENDING_POINT,				// 当前已在终点，操作失败
		EN_PET_MONOPOLY_LACK_SPECIAL_ITEM,				// 道具数量不足
		EN_PET_MONOPOLY_NO_REWARD_GRID,					// 本层没有奖励可替换了
			
		EN_MATERIAL_BOX_REAWRD_FETCHED,					// 该奖励已领取,请继续努力哦!
		EN_MATERIAL_INTEGRAL_NOT_ENOUGH,				// 抱歉,您的积分不足,领取失败
		EN_MATERIAL_CONDITION_NOT_ALL_FINISH,			// 完成以上扫荡条件后解锁本关快速扫荡
		
		EN_CRYSTAL_BUY_ENERGY_TIMES_NOT_ENOUGH,			// 剩余购买次数不足	
		EN_CRYSTAL_BUY_ENERGY_REACH_LIMIT,				// 体力已达到上限了，请少侠先去使用后再来购买吧

		EN_CLOUD_ARENA_ONLY_IN_CROSS,					// 抱歉，穹顶之争只能在跨服中参与
		EN_PLAY_ACTIVITY_NOW_ERR,						// 您正在活动中，请稍后再试。
		EN_RELICLOOTING_CANNT_MOVE,						// 玩家准备中，暂时无法移动
		EN_PET_MONOPOLY_TURNTABLE_GEN_FAILED,			// 转盘奖励生成失败，请稍后再试
		EN_CLOUD_ARENA_MAX_REMAIN_FIGHT_TIMES,			// 挑战次数已满，不需要购买
		EN_CLOUD_ARENA_FIGHT_TIMES_NOT_ENOUGH,			// 挑战次数不足，无法发起挑战
		EN_CLOUD_ARENA_MANUAL_REFRESH_CD_LIMIT,			// 手动刷新对手功能冷却中，请稍后再试
		EN_CLOUD_ARENA_ROLE_LEVEL_NOT_ENOUGH,			// 该功能将在45级开启，少侠继续努力吧~
		EN_CLOUD_ARENA_NO_ADD_EXP_TIMES,				// 加经验次数已用完
		EN_CLOUD_ARENA_RANKING_NOT_REACH,				// 尚未到达该段位
		EN_CLOUD_ARENA_RANKING_REWARD_HAS_FETCHED,		// 该段位奖励已领取
		EN_CLOUD_ARENA_SETTLEMENT_LIMIT,				// 当前处于赛季结算期，不能进行挑战哦~
		EN_CLOUD_ARENA_ALREADY_FETCH_ACHIVEMENT,		// 已经领取过当前成就奖励
		EN_CLOUD_ARENA_ACHIEVEMENT_NOT_FETCHABLE,		// 尚未完成该成就，无法领取奖励
		EN_CLOUD_ARENA_ALREADY_FETCH_DAY_REWARD,		// 今天已经领取每日奖励
		EN_CLOUD_ARENA_DAY_REWARD_NOT_FETCHABLE,		// 暂不可领取每日奖励
		EN_CLOUD_ARENA_BUY_FIGHT_TIMES_DAY_LIMIT,		// 今天可购买挑战次数机会已用完

		EN_RELICLOOTING_BE_CATCH,						// 你被抓住了，无法动弹，请呼叫队友援助
		EN_RELICLOOTING_BE_TRAP,						// 你被夹住了，无法动弹
		EN_RELICLOOTING_MATH_MEMBER_TOO_MUTCH,			// 匹配人员太多了
		EN_RELICLOOTING_PARAM_NOT_ADV,					// 条件未达成，无法领取
		EN_RELICLOOTING_REWARD_HAS_GET,					// 奖励已经领取过了

		EN_ITEM_REQUIRE_CLOUD_ARENA_DAN,				// 该商品需要更高的穹顶之争段位
		EN_ITEM_REQUIRE_CLOUD_ARENA_RANK,				// 该商品需要更高的穹顶之争排名

		EN_CRYSTAL_FB_NORMAL_TIMES_LIMIT,				// 该关卡今日剩余次数不足

		EN_HUAN_JIE_ZHAN_CHANG_CANNT_FIGHT,				// 有角色连接异常，系统重新进行匹配
		EN_HUAN_JIE_ZHAN_TIAN_TI_MATCH_TOO_MANY_ROLE,	// 队伍人数过多，无法进行匹配
		EN_PLATFORM_BATTLE_IN_OTHER_PLATFORM,			// 要出战宠物或伙伴已经在其他擂台
		EN_GUILD_HONOR_BATTLE_CANT_ENTER_IS_BYE,		// 本轮战斗轮空
		EN_TEAM_INVITE_LEVEL_LIMIT,						// 玩家等级不足，无法加入队伍
		EN_MAP_EXPLORATION_HAS_CHECK,					// 这里已经探索过了
		EN_TODAY_HAS_BEEN_POKER_GET,					// 本日通关次数已达上限，无法继续获取翻牌奖励
		EN_ROLE_HEART_SKILL_NOT_REPEAT_WEAR,			// 抱歉,该技能栏已使用了该技能
		EN_ROLE_HEART_SKILL_LEVEL_NOT_ENOUGH_UNLOCK,	// 抱歉,由于您的等级不足,该技能栏解锁失败
		EN_ROLE_HEART_SKILL_GRID_NOT_UNLOCK,			// 抱歉,该技能栏尚未解锁,装备失败
		EN_ROLE_HEART_SKILL_UNLOCK,						// 抱歉,该技能栏已解锁
		EN_PET_HELPER_NUM_OVER_LIMIT,					// 抱歉，宠物助战数量超过上限
		EN_PET_SOUL_REFINE_ITEM_ERR,					// 精炼失败，道具不同
		EN_ROLE_TOP_LEVEL_NOT_ENOUGH_FETCH_FAILD,		// 实力不足，暂时无法领取
		EN_KNAPSACK_GOD_PRINT_COLUMN_NO_SPACE,          // 神印背包空间不足，请先清理
		EN_BATTLE_DIFFICULT_MUST_MANUAL,				// 本次战斗难度较大，已自动帮您设置为手动操作
		EN_CHIVALROUS_ACHIEVE_ALL_FETCHED,				// 侠义值成就已全部领取完毕
		EN_HISTORY_CHIVALROUS_NOT_ENOUGH,				// 累计侠义值不足，领取失败
		EN_CHIVALROUS_REWARD_ALREADY_FETCHED,			// 该奖励已领取
		EN_SINGLE_CHAT_IS_LIMIT,						// 当前等级过低，私聊人数已达上限
		EN_WORKSHOP_ACTVIE_VOC_MAX_NUM,					// 激活词条已经达到最多条数
		EN_GOD_EQUIP_NO_LEVEL,							// 神装等级不足
		EN_CLOUD_ARENA_SHOP_ERROR,						// 穹顶商店商品有误，请稍后再试
		EN_PET_STRENGTHEN_PROTECT_CANNT_USE,						// 当前强化不能使用强化保护道具
		EN_PET_STRENGTHEN_PROTECT_ITEM_NOT_ENOUGH,				// 宠物强化保护道具不足
		EN_HAS_ORDER,				// 由于上一个订单未完成，请等待2分钟后再试
		EN_GUILD_ACTIVE_DAY_ACTIVE_LIMIT,				// 家族日活跃度不足,无法领取该礼包
		EN_INSTRUCTION_FULL,							// 自定义指令数量已满
		EN_MAZE_RACER_ALREADY_GET_REWARD,				// 本次通关无法继续获得奖励
		EN_MAZE_RACER_NEW_PASS_TIME,				// 恭喜！本次通关打破了自己本次活动的最佳记录
		EN_NOT_PASS_CRYSTAL_FB,							//未通关无法领取
		EN_PASS_CRYSTAL_FB_REWARD_HAS_GET,				//通关奖励已经领取了
		EN_SHOP_LOCK_NOT_CAN_BUY_ITEM,					//未解锁，不可购买

		EN_GOD_PRINT_EXP_NOT_ENOUGH,					// 经验不足
		EN_PET_WEAR_GOD_PRINT,							// 宠物穿戴神印中
		EN_PET_NO_WEAR_GOD_PRINT,						// 宠物没有穿戴神印
		EN_PET_GOD_PRINT_HAS_LOCK,						// 宠物神印锁定中
		EN_FB_NOT_PASS,									// 上一关卡未通关
		EN_NETHER_TASK_IS_NOT_COMPLETED,				// 任务尚未完成
		EN_NETHER_TASK_REWARD_HAS_GET,					// 奖励已经领取过了
		EN_NETHER_TASK_NOT_OPEN,						// 关卡尚未开启
		EN_CRYSTAL_PASS_LEVEL_NOT_HAS_CONSUME,			// 由于没有消耗封印石，本次通关无法获取翻牌奖励


		EN_HONG_BAO_GET_LIMIT_FOR_HIDDEN,				// 抱歉，领取红包奖励需要返回原服领取
		EN_HONG_BAO_SEED_LIMIT_FOR_HIDDEN,				// 抱歉，发送红包需要返回原服发送
		EN_HONG_BAO_INVALID_TIMESTAMP,					// 您来晚了，红包已过期
		EN_HONG_BAO_OVER,								// 您来晚了，红包已被领完
		EN_HONG_BAO_QUIT_GUILD_GET_LIMIT,				// 今日奔波劳碌，稍作休息后再来领取红包吧。
		EN_HONG_BAO_QUIT_GUILD_SEED_LIMIT,				// 今日奔波劳碌，稍作休息后再来发红包吧。
		EN_HONG_BAO_TYPE_NOT_TIMES,						// 该类型红包不足 , 无法发送红包
		EN_HONG_BAO_GET_LIMIT,							// 您已经领取过该红包了 , 不可重复领取

		EN_PET_GOD_FB_NOT_PASS,							// 当前关卡未解锁，请先通关上一层
		EN_KNAPSACK_PET_SOUL_EQUIP_COLUMN_NO_SPACE,		// 魂印背包空间不足
		EN_NETHER_WORLD_PURGATORY_MILING_NO_TIMES,		// 当前密令数量不足，无法开启
		EN_PET_WEAR_SOUL_EQUIP,							// 宠物穿戴魂器中
		EN_EQUIP_HAS_STONE_ERR,							// 该装备有镶嵌宝石无法分解
		EN_EQUIP_HAS_ZHULING_ERR,						// 该装备有铸灵无法分解
		EN_JEWERLY_HAS_STONE_ERR,						// 该灵饰有镶嵌宝石无法分解
		EN_JEWERLY_HAS_ZHULING_ERR,						// 该灵饰有铸灵无法分解
		EN_ACTIVITY_BIG_DIPPER_NO_DATA,					// 未知星君或帝君信息
		EN_ACTIVITY_BIG_DIPPER_NO_MONSTER,				// 此星君不可以挑战
		EN_ACTIVITY_BIG_DIPPER_NO_BOSS,					// 此帝君不可以挑战
		EN_ACTIVITY_BIG_DIPPER_NO_PASS_LEVEL,			// 未达到奖励阶段
		EN_ACTIVITY_BIG_DIPPER_HAS_REWARD,				// 奖励已经领取过了
		EN_ACTIVITY_BIG_DIPPER_NO_PASS,					// 尚未通关
		EN_TODAY_VILLAGE_ALREADY_SELECT,				// 今天已选择了村庄，无法再次选择
		EN_VILLAGE_NOT_FOUND,							// 未找到该村庄
		EN_VILLAGE_NO_TASK_FOUND,						// 该村庄未发布任务
		EN_PARTNER_GIFT_TREE_LEVEL_LIMIT,				// 前置天赋未满足
		EN_PRESTIGE_TASK_ACCEPT_FAILED,					// 接取声望任务失败
		EN_BRAVE_GROUND_WORLD_LEVEL_NOT_OPEN,			// 当前世界等级勇闯地宫未开启
		EN_FANG_FRAUD_ERROR,							// 请少侠重新选择正确的漫画
		EN_ROLE_ATTR_PELLET_USE_LIMIT,					// 属性丹已达到本条属性的使用上限，使用失败

		EN_MAZE_SUMMER_NOT_UNLOCK,						// 该地图未解锁无法前往
		EN_MAZE_SUMMER_DOING_MAP,						// 您在挑战中无法移动
		EN_MAZE_SUMMER_REWARD_HAS_GET,					// 奖励已经领取过了

		EN_PARTNER_PACT_CANT_SELF,						// 无法契约伙伴自身元神
		EN_PARTNER_PACT_CANT_SAME,						// 已契约该伙伴元神，请勿重复契约
		EN_PARTNER_PACT_MAX,							// 该伙伴元神已被契约2次，请解除契约后再试
		EN_PARTNER_BE_PACT_UP,							// 该元神契约伙伴属性对应提升

		EN_TITLE_HAS_KILL_VALUE,						// 您当前存在战意值，暂时无法佩戴称号
		EN_TITLE_KILL_VALUE_TITLE_CHANGE,				// 您当前存在战意值，已强制替换称号
		EN_DUEL_ITEM_NOT_ENOUGH,						// 决斗令不足
		EN_DUEL_NOT_HAS_KILL_VALUE,						// 当前没有战意值，无法使用忏悔录
		EN_DUEL_SCENE_CAN_NOT_DUEL,						// 当前场景无法决斗，请去往野外场景
		EN_DUEL_MINE_CAN_NOT_DUEL,						// 当前等级不足，无法决斗
		EN_DUEL_TARGET_CAN_NOT_DUEL,					// 对方等级不足，无法决斗
		EN_DUEL_TEAM_IS_RED_NAME,						// 当前队伍存在红名玩家，将会遭到所有玩家追杀!
		EN_DUEL_TEAM_NOT_ALLOW_RED_JOIN,				// 该队伍拒绝红名玩家入队
		EN_DUEL_TEAM_HAS_RED,							// 该队伍已经存在红名玩家，修改失败
		EN_RA_HAPPY_LOOK_NOT_FETCH_SUCC,				// 请领取您的宝盒再进行下一轮挑战哦~
		EN_RA_HAPPY_LOOK_NOT_FETCH_FAIL,				// 请领取您的礼袋再进行下一轮挑战哦~
		EN_GUILD_HONOR_BATTLE_CANT_ENTER_2,				// 不是参赛人员不能进入
		EN_PET_NOT_OP_BY_BENEDICTION,					// 当前宠物已作为祝福副宠，操作失败
		EN_PET_CANCEL_HUA_SHEN_PET_STATUS,				// 因为其他宠物提供祝福效果，无法展示宠物化身
		EN_YIN_YANG_EYE_PLEASE_OPEN,					// 阴阳眼尚未开启
		EN_KNAPSACK_ACCESSORIES_COLUMN_NO_SPACE,		// 配饰背包空间不足
		EN_MEDAL_EXCHANGE_NOT_SAME_SP,					// 无法转换同名令牌
		EN_BREAK_SOUL_FIGHT_OLD_NOT_PASS,				// 前置关卡未通关
		EN_COMPLETE_ANECDOTE_NUM_LIMIT,					// 奇闻任务完成数量不足
		EN_BIG_EVENT_NOT_OPEN,							// 不在大事件开放阶段
		EN_BIG_EVENT_REWARD_ALREADY_FETCHED,			// 该奖励已领取
		EN_COURSE_LEVEL_NOT_ENOUGH_FETCH_FAILED,		// 历程等级不足,无法领取
		EN_RA_SHAN_HAI_MEET_NOT_BUY,					// 山海相逢未激活
		EN_RA_SHAN_HAI_MEET_CAN_NOT_POKER,				// 未激活山海相逢活动，不可进行翻牌
		EN_RA_SHAN_HAI_MEET_NOT_POKER_ALL,				// 请先翻齐所有牌
		EN_DAY_REWARD_DAY_FOOD_FETCH_NOT_TIME,			// 还未到时间,无法领取
		EN_DAY_REWARD_DAY_FOOD_NOT_MAKE_FETCH,			// 抱歉,该奖励不可补领
		EN_DAY_REWARD_DAY_FOOD_FETCH_TIME_OUT,			// 补领已结束
		EN_TARGET_HAS_JIE_YIE_ZU,						// 对方已经有结义
		EN_TARGET_JIE_YI_OFFLINE_SHORT,					// 对方离线不够长，不能踢出
		EN_TARGET_JIE_YI_ONLINE,						// 对方在线中，不能踢出
		EN_JIE_YI_QIN_MI_DU_BU_ZU,						// 队长和队员的亲密度不足
		EN_NOW_ESCORTING_NO_START_CHANGE,				// 正在运镖中，不可挑战，请稍后再来哦~
		EN_GATHER_FAILED_BY_SLOW,						// 哎呀，被人抢先一步摘走了
		EN_GATHER_POWER_NOT_ENOUGH,						// 抱歉,您的活力值不足
		EN_GATHER_FAIL_BY_DUEL,							// 采摘失败
		EN_PARTNER_SOAR_FAIL_BY_LEVEL_NOT_ENOUGH,		// 抱歉,该伙伴等级不足,飞升失败
		EN_PARTNER_SOAR_ARMS_LEVEL_UP_FAIL_BY_NOT_SOAR,	// 抱歉,该伙伴还未飞升,飞升武器暂未开放
		EN_PARTNER_SOAR_FAIL_BY_SOAR_TASK_NOT_FINISH,	// 抱歉,该伙伴飞升任务暂未完成,飞升失败
		EN_PARTNER_SOAR_ARMS_LEVEL_UP_FAIL_LEVEL_LIMIT,	// 抱歉,该伙伴飞升武器等级已达上限,强化失败
		EN_PARTNER_SOAR_FAIL_BY_SCORE_NOT_ENOUGH,		// 抱歉,该伙伴评分未达标,飞升失败
		EN_PARTNER_SOAR_FAIL_BY_QUALITY_LIMIT,			// 抱歉,该伙伴觉醒品级不足,飞升失败
		EN_PARTNER_UP_GRADE_FAIL_BY_MAX_LIMIT,			// 抱歉,该伙伴阶数已至当前上限,进阶失败
		EN_PARTNER_SOAR_FAIL_BY_UPGRADE_LEVEL_LIMIT,	// 抱歉,该伙伴进阶阶数不足,飞升失败
		EN_ROLE_FLY_UP_STAG_NOT_ENOUGH,					// 抱歉,您的角色当前飞升阶段还不足以解锁该关卡
		EN_HAS_DIANHUASHI_ERR,							// 抱歉,该道具被点化了无法上架
		EN_FLY_UP_LEVEL_ERR,							// 需要完成高阶飞升才可穿戴
		EN_PARTNER_SOAR_ACTIVATION_FAILED,				// 抱歉,该伙伴觉醒品级不足,激活失败

		// 福牛报喜,寅虎纳福
		EN_COW_REPORT_GOOD_NEWS_CAN_NOT_BUY,			// 抱歉,已错过可购买时间
		EN_COW_REPORT_GOOD_NEWS_BUY_TIMES_LIMIT,		// 抱歉,您已经购买过了,不可重复购买
		EN_COW_REPORT_GOOD_NEWS_IS_NOT_SHOW_TIME,		// 抱歉,现在不是获奖名单展示期
		EN_RA_ZHIGOU_NOT_BUY,							// 权限未激活，无法领取奖励
		
		EN_WAN_LING_FB_BATTLE_MAX,						// 今日铜币奖励领取数已达到上限
		EN_WAN_LING_FB_PASS_MAX,						// 今日通关奖励领取数已达到上限
		EN_CLOUD_ARENA_MANUAL_REFRESH_TIMES_LIMIT,		// 今日手动刷新次数已用完

		EN_ACTIVITY_SILK_ROADS_TRADING_LOG_ALL_SHOW,	// 本次丝绸之路，消息已全部公示完毕
		EN_ACTIVITY_SILK_ROADS_SUPPLY_NOT_ENOUGH,		// 抱歉,您的补给不足,无法前往下一个城镇
		EN_ACTIVITY_SILK_ROADS_SALE_COMMODITY_FAILED,	// 抱歉,您的货物不足,售卖失败
		EN_ACTIVITY_SILK_ROADS_BUY_TRADING_LOG_LIMIT,	// 抱歉,该城镇已无消息可以打探
		EN_ACTIVITY_SILK_ROADS_MONEY_NOT_ENOUGH,		// 抱歉,您的货币不足
		EN_ACTIVITY_SILK_ROADS_WAREHOUSE_EXP_LIMIT,		// 抱歉,仓库扩充已达上限
		EN_ACTIVITY_SILK_ROADS_BUY_SUPPLY_LIMIT,		// 抱歉,补给购买次数已达上限
		EN_ACTIVITY_SILK_ROADS_FINISH_NUM_LIMIT,		// 抱歉,您的完成次数已达上限
		EN_ACTIVITY_SILK_ROADS_NOT_ADVANCE_END,			// 抱歉,您还有足够的补给点,不能提前结束
		EN_ACTIVITY_SILK_ROADS_ACTIVITY_END,			// 丝绸之路活动已结束，奖励已发送至邮箱，请查收
	
		EN_PET_FEI_SHENG_TIMES_NOT_ENOUGH,				// 宠物飞升等级不足
		EN_RA_CHAO_ZHI_GIFT,							// 您还未激活超值献礼，请先激活
		EN_WORLD_ARENA_ALREADY_SIGNED_UP,				// 已完成报名，后续赛程请留意邮件及相关界面通知

		EN_STAR_UP_ERR_NOT_SAME_STAR_LEVEL,				// 星级不一致
		EN_STAR_UP_ERR_NOT_SAME_LEVEL,					// 等级不一致
		EN_STAR_UP_ERR_NOT_SAME_QUALITY,				// 品质不一致
		EN_STAR_UP_ERR_NOT_SAME_PROF,					// 职业不同
		EN_STAR_UP_ERR_NOT_SAME_ITEM_TYPE,				// 不是同类装备

		EN_FLY_UP_ROLE_CAPBILITY_ERR,					// 玩家评分不足
		EN_PET_TWO_MUTANT_NEED_FEI_SHENG,				// 二次变异需要宠物飞升

		EN_NEW_LIFE_SKILL_LEVEL_LIMIT_BY_GUILD_LEVEL,	//当前技能等级已达家族最大技能等级限制
		EN_NEW_LIFE_PROF_LEVEL_LIMIT_BY_GUILD_LEVEL,	//当前职业等级已达家族最大技能等级限制
		
		EN_FRIEND_BY_CROSS_NOT_CONNECTED ,				// 好友功能暂时无法使用 , 请稍后再试

		EN_EQUIP_JEWELRY_UP_STAR_LEVEL_ERR,				//装备有铸灵或宝石无法升星
		EN_NOT_LEADER_CANNT_MATH_REQ_ERR,				//请等待队长开始匹配
		EN_PET_NEED_FEI_SHENG,							//需要宠物飞升
		EN_NEED_ROLE_FEI_SHENG,							//角色飞升不满足
		EN_TEAM_NEED_ROLE_FEI_SHENG,					//队伍中有玩家飞升不满足
		EN_NEED_PET_STR_LEVEL,							//宠物强化等级不满足
		EN_NEED_PET_CAPBILITY_LEVEL,					//宠物战力等级不满足
		EN_NOT_IN_PET_LIST,								//宠物不在列表中，无法完成飞升任务
		EN_PET_TASK_NOT_MATCH,							//宠物飞升阶段和任务不匹配，无法完成飞升任务
		EN_NEED_ROLE_FLY_UP,							//完成角色飞升才能挑战
		EN_FUNOPEN_SERVER_OPEN_ERR,						//玩法暂未开启，少侠请耐心等待
		EN_SHI_TU_BREAK_NOT_ENOUGH_DAY,					//师徒关系不足30天

		EN_RA_YINHU_WENSHI_NOT_BUY,						// 您还未激活寅虎问世哦~
		EN_RA_ACTIVITY_DAY_NOT,							// 所需活动天数不足
		EN_RA_CINIU_YINGXINGHU_NOT_AWAKEN,				// 未达到唤醒阶段
		EN_RA_CINIU_YINGXINGHU_AWAKEN,					// 已唤醒
		EN_RA_HU_XING_DA_YUN_NOT_SERVER,				// 您和您的好友不在同一服务器
		EN_RA_HU_XING_DA_YUN_NOT_INVITED,				// 好友正在同行中
		EN_RA_HU_XING_DA_YUN_INVITED_TIMES,				// 邀请的好友次数不足
		EN_RA_HU_XING_DA_YUN_TASK_END,					// 好友已经到终点
		EN_RA_YIN_HU_BAI_SUI_NOT_BUY,					// 您还未激活寅虎拜岁哦~

		EN_WEDDING_REGIST_FULL,							//婚宴已预约,操作无效
		EN_WEDDING_REGIST_SUCC,							//成功预约婚宴
		EN_WEDDING_REGIST_OFF,							//婚宴预约已取消
		EN_WEDDING_REGIST_TOO_NEAR,						//婚宴预约时间过近
		EN_WEDDING_REGIST_OVER,							//该时段已被预约
		EN_WEDDING_NULL,								//婚宴已失效
		EN_WEDDING_INVITE_SUCC,							//成功邀请对方参加婚宴
		EN_WEDDING_BE_INVITED,							//已经邀请过该玩家了
		EN_WEDDING_INVITED_LIST_FULL,					//邀请列表已满
		EN_WEDDING_APPLY_SEND,							//婚宴参与申请已发送
		EN_WEDDING_APPLY_WAIT,							//已申请过参与该婚宴, 等待主办者处理
		EN_WEDDING_WAIT_LINK,							//婚宴处于预热阶段, 请稍后
		EN_WEDDING_MEAL_MAX,							//用餐次数已达上限
		EN_WEDDING_MEAL_FAST,							//用餐过于频繁, 清稍后
		EN_WEDDING_SUB_ACT_FINISH,						//当前任务全部完成，请等待下一个环节
		EN_WEDDING_XUN_YOU_BUY_SUCC,					//已购买花车服务
		EN_WEDDING_XUN_YOU_HAS_BUY,						//无法重复购买
		EN_NEW_LIFE_SKILL_SURFACE_COUNT_NOT_ENOUGH,		//抱歉,您的幻化点不足,幻化失败
		EN_NEW_LIFE_SKILL_PERFORM_SKILL_ERROR,			//抱歉,您的幻化点不足,无法释放该技能
		EN_NEW_LIFE_SKILL_NOT_HAS_CHANGE_CARD,			//抱歉,您没有使用幻化卡
		EN_FALLEN_GOD_MEMBER_NOT_PASS_PRE_SN,			//有队员未具备挑战资格，无法唤醒该魂灵
		EN_MINI_GAME_REWARD_ALREADY_FETCHED,			//奖励已领取
		EN_MINI_GAME_PHASE_REWARD_CLEAR_STAGE_NUM_LIMIT,//尚未通关指定关卡
		EN_RELICLOOTING_HAS_ITEM_SKILL,					//当前存在正在使用的道具，暂时无法拾取

		EN_WORLD_ARENA_NOT_IN_TIME_TO_ENTER_SCENE,		//当前不在进场时间内
		EN_WORLD_ARENA_NOT_IN_BATTLE_STATUS,			//当前不是战斗阶段
		EN_WORLD_ARENA_UNABLE_ENTER_SCENE,				//服务器错误，无法进场，请联系客服
		EN_WORLD_ARENA_ENTER_SCENE_FAILED,				//进场失败，请稍后再试
		EN_WORLD_ARENA_WIN_NUM_NOT_ENOUGH,				//胜场不足，无法领取该奖励
		EN_WORLD_ARENA_WIN_BOX_ALREADY_FETCHED,			//该奖励已领取，无法重复领取
		EN_WORLD_ARENA_IN_SIGNUP_STATUS_LIMIT,			//尚在报名阶段，操作失败
		EN_WORLD_ARENA_HAS_FETCH_SIGNUP_REWARD,			//报名奖励已领取，无法重复领取
		EN_WORLD_ARENA_DAILY_REWARD_FETCH_FAIL,			//每日奖励不可领取或已领取

		EN_RELICLOOTING_TEAM_TYPE_ERR,					//抱歉，您当前的队伍目标与玩法目标不一致
		EN_RELICLOOTING_TEAM_ADD_ERR,					//队伍匹配中无法加入
		EN_WORLD_QUESTION_ANS_ERR,						//回答错误，请继续努力吧！！

		EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_NAME,				//抱歉,战队名称存在敏感字符
		EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_MEMBER,			//抱歉,需要组成5人队伍才可进行战队报名
		EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_TIME,				//抱歉,当前暂时无法报名
		EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_MEMBER_SIGN_UP,	//抱歉,队伍中有成员已加入战队
		EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_MONEY_NOT_ENOUGH,	//抱歉,队长未拥有足够的报名费
		EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_MEMBER_CHANGE,	//抱歉,成员信息发生改变,战队报名失败
		EN_WORLD_TEAM_ARENA_SIGN_UP_FAILED_BY_NAME_REPEAT,		//抱歉，与其他战队名称重复
		EN_WORLD_TEAM_ARENA_ENTER_SCENE_FAILED_BY_NOT_HAS_TEAM,	//抱歉，您需要加入队伍后方可进入赛场
		EN_WORLD_TEAM_ARENA_FAILED_TEAM_NOT_SAME_TEAM,			//抱歉，队伍内必须均为同一战队成员
		EN_WORLD_TEAM_ARENA_NOT_HAS_TEAM,						//抱歉，仅已报名的战队可进行参赛
		EN_GUILD_QIFU_NOT_OPEN,									//名人堂祈福未开启
		EN_GUILD_QIFU_VALUE_NOT_ENOUGH,							//祈福进度未满足
		EN_TASK_CANNT_SKIP,										//此任务无法跳过
		EN_ENTRY_COIN_USE_TIMES_NOT_ENOUGH,						//今日铜币熔炼次数不足
		EN_PARTNER_FEI_SHENG_TIMES_NOT_ENOUGH,					//伙伴飞升等级不足
		EN_PET_BADGE_TAKE_OFF_FAIL,								//宠物徽章脱下失败
		EN_YAO_XING_SHI_LIAN_PASS_ERR,					// 成就未完成
		EN_RELICLOOTING_IN_MATCH_ERR,					// 正在匹配中，无法加入
		EN_DONATE_ERR_HAS_ZHU_LING_ERR,							//部分装备请卸下铸灵再捐赠


		EN_SHI_TU_MILE_FETCH_LIMIT,						//抱歉，今日师徒里程奖励领取次数已达到上限


		EN_PET_TALENT_NOT_REPEAT_ACTIVED,						//该天赋组已激活
		EN_PET_TALENT_NOT_YET_ACTIVE,							//该天赋组尚未激活
		EN_PET_TALENT_NOT_RESET_NOT_TRADE,						//宠物天赋未重置，无法进行交易

		EN_SHITU_CHUAN_GONG_TEAM_FULL,					//师徒传功, 需要师徒二人参与
		EN_SHITU_CHUAN_GONG_TEAM_LEADER,				//师徒传功, 需要弟子带队进行
		EN_LEVEL_COMPLEMENT_BUY_EXP_COUNT_NOT_ENOUGH,	//助力经验今日购买次数不足
		EN_LEVEL_COMPLEMENT_NOT_HAS_EXP_BUY_FAILED,		//当前助力经验已经全部获得了，明日可以购买哦

		EN_QINGYUAN_DUIDUIPNEG_PEIDUI_NOT,				//验证失败，请再接再厉

		EN_YAO_XING_SHI_LIAN_CAP_NOT_ENOUGH,			// 战力不足
		EN_TEAM_MEMBEI_NOT_CANENTER_ITEM,				// 当前队伍无配对的同心结
		EN_QING_YUAN_SHENG_HUI_QUESTION_FINISH,			// 此环节答题已经结束
		EN_QINGYUAN_DUIDUIPENG_LEAVE_SCENE,				// 队伍中有玩家离开，本场游戏失败
		EN_SHITU_CHUAN_GONG_BREAK_OFF,					// 对方退出传功，传功中断
		EN_RA_QIYUAN_KONGMINGDENG_NOT_ITEM,				// 火种不足
		EN_RA_XUNMENG_HUDIE_NOT_NPC,					// 蝴蝶已经被别人抓走了
		EN_RA_ACTIVITY_NOT_TIMES,						// 活动次数不足
		EN_ZAO_HUA_KNAPSACK_NO_SPACE,					// 造化背包空间不足
		EN_ZAO_HUA_NO_WEAR_EQUIP,						// 未穿戴装备
		EN_ZAO_HUA_WEAR_ERR_SUIT,						// 造化装备未集齐套装
		EN_ZAO_HUA_APP_HAS_ACTIVE,						// 外观已经激活过了
		EN_ZAO_HUA_APP_NOT_ACTIVE,						// 该外观未激活
		EN_ZAO_HUA_SUIT_LEVEL_LIMIT,					// 套装等级不足

		EN_UTA_TEAM_NOT_FOUND,							//未找到该队伍信息
		EN_UTA_MY_TEAM_DONT_HAVE_QUALIFICATION,			//抱歉，您的战队未获得资格淘汰赛名额
		EN_UTA_NO_SCHEDULE_YET,							//抱歉，暂未有资格淘汰赛赛程

		EN_UTA_GUESS_BET_REPETITION_ERROR ,				//该队伍您已经下注过了
		EN_UTA_WIN_BOX_FETCH_LIMIT,						//该宝箱暂不满足要求，或已领取
		EN_UTA_NO_MAIN_SCHEDULE_YET,					//抱歉，暂未有晋级赛赛程
		EN_UTA_NO_ROUND_MAIN_SCHEDULE_YET,				//抱歉，此阶段赛程尚未公布
		EN_UTA_NO_FINAL_LIST_YET,						//抱歉，最终榜单尚未公布
		EN_UTA_ONCE_LOTTERY_LIMIT,						//已达单次下注上限

		EN_UTA_NOT_IN_ENTER_SCENE_TIME,					//当前不在可进场时间内
		EN_UTA_SCENE_NOT_FOUND,							//抱歉，找不到您的战斗场景
		EN_UTA_ENTER_SCENE_FAILED,						//进场失败，请稍后再试
		EN_UTA_TEAM_ENTER_SCENE_FAILED,					//您的队伍进场失败，请检查人员配置，确保队员都处于可进场状态

		EN_UTA_SCHEDULE_ALREADY_HAS_RESULT,				//本场战斗已有结果，不需重复战斗
		EN_UTA_OB_DATA_EMPTY,							//尚未有本场战斗的数据回传，暂无法观战
		EN_UTA_OB_FIGHT_ALREADY_END,					//本场战斗已结束，请稍后尝试观看录像
		EN_UTA_OB_IN_TEAM_LIMIT,						//当前在队伍中，请退队或暂离后重试
		EN_BIGCROSS_GM_OPER_SUCC,						//大跨服GM命令操作成功
		EN_BIGCROSS_GM_OPER_FAIL,						//大跨服GM命令操作失败失败失败

		EN_UTA_GUESS_BET_ERROR,							//当前阶段不可以下注
		EN_UTA_LAST_SEASON_OVERVIEW_NOT_AVALIABLE,		//暂无上届赛程总览

		EN_ROLE_HUNDRED_GOD_BOSS_FINISH_NOT_FETCH,		//该功能已完成
		EN_ROLE_HUNDRED_GOD_BOSS_END,					//该功能已结束
		EN_ROLE_HUNDRED_GOD_BOSS_TARGET_NOT_FINISH,		//该目标未达成

		EN_AUTHORITY_TYPE_TEST_DAY_BUY_LIMIT,			//今日交易已达上限

		EN_RA_GOD_BEAST_ADVENT_UNLOCK_REWARD,			// 好感度未达到,不可以兑换
		EN_RA_GOD_BEAST_ADVENT_EXCHANGE_LIMIT,			// 达到可兑换上限,不可兑换
		EN_RA_LOGIN_DAY_NOT,							// 登录天数不足
		EN_RA_YUANQI_JISHIBU_NOT_BUY_JINJIE,			// 进阶奖励未激活
		EN_TRADEMARKET_UPSHELVE_TIME_LIMIT,				// 该物品剩余有效时间不足，不可出售

		EN_HOLY_EQUIP_BAG_NO_SPACE,						//圣器背包空间不足
		EN_FU_SHI_BAG_NO_SPACE,							//符石背包空间不足
		EN_HOLY_EQUIP_STANDBY_MAX,						//出战或待机圣器位置已满
		EN_HOLY_EQUIP_SKILL_HOLE_LOCK,					//技能孔位未解锁
		EN_HOLY_EQUIP_FORE_SKILL_HOLE_UNLOCK,			//请先解锁前面的技能孔位
		EN_HOLY_EQUIP_NO_SKILLS_TO_SAVE,				//没有可保存的技能
		EN_HOLY_EQUIP_HAVE_SAME_TYPE_FUSHI,				//不可镶嵌同类型符石
		EN_HOLY_EQUIP_FUSHI_LEVEL_NOT_SAME,				//请放入等级相同的符石
		EN_HOLY_EQUIP_FUSHI_LEVEL_ERROR,				//符石等级不符合条件
		EN_HOLY_EQUIP_CAN_NOT_ADD_FINGHT_STANDBY,		//出战或待机状态圣器不可作为材料
		EN_HONGMENG_TIANJIE_NOT_REWARD_TIMES,			//当前无奖励次数，本次挑战将无法获得奖励
		EN_GOTO_COMMON_SCENE_NOT,						//请离开副本后再尝试
		EN_HOLY_EQUIP_LEVEL_NOT_ENOUGH,					//圣器等级不足
		EN_HOLY_EQUIP_PLEASE_UNLOCK_LOWER_LEVEL_HOLE,	//请先解锁更低等级的符石孔
		EN_HOLY_EQUIP_CAN_NOT_BLESS_PET,				//只有出战或待机圣器才可以圣佑宠物
		EN_HOLY_EQUIP_BLESS_PET_ONE_ON_ONE,				//一个圣器只能圣佑一只宠物
		EN_HOLY_EQUIP_NOT_REPEATABLE_STUDY,				//本圣器已拥有此技能，无法再次学习
		EN_HOLY_EQUIP_HAS_NO_SAVE_SKILL,				//本圣器有尚未保存的技能
		EN_HOLY_EQUIP_QUALITY_TOO_CHEAP,				//圣器品质不足

		EN_JING_YING_SHI_LIAN_TEAM_HAVE_ROBOT,			//当前队伍中存在助战人员，无法进入该秘境

		EN_RA_CHONGZHI_NOT_ENOUGH,						// 今日充值未达到要求
		EN_RA_SANBAI_CHOU_NOT_DRAW,						// 当前类型抽奖次数不足

		EN_RA_LAN_TANG_CHU_MU_CAN_NOT_ADD_HERBS,		//当前不可加入药材

		EN_NEW_SHOP_ITEM_NOT_SHOW,						//当前商品处于不可售状态
		EN_KE_JU_EXAM_CAN_NOT_ANSWER,					//只有指定玩家可答题
		EN_KE_JU_EXAM_CAN_NOT_SEEK_HELP,				//您不是指定答题玩家,不可求助
		EN_KE_JU_EXAM_CAN_NOT_SUGGEST,					//您是指定答题玩家,不可建议
		EN_TEAM_CAN_NOT_JOIN_KE_JU_EXAM,				// 队伍正在进行科举答题中,暂时不可加入
		EN_TARGET_OFFLINE_TIME_NOT_ENOUTH,				// 对方离线时长不满足
		EN_GET_ROLE_INFO_FAIL,				// 获取对方信息失败，请稍后再试
		EN_JIE_YI_TARGET_OFFLINE_TIME_NOT_ENOUTH,			//  对方离线时长不满足

		EN_RA_CHARISMATIC_TANABATA_FOWLS_ERR,			//少侠，送花应该当面送啊
		EN_RA_CHARISMATIC_TANABATA_LOVE_VALUE_LIMIT,	//爱慕值不足
		EN_RA_CHARISMATIC_TANABATA_MEILI_VALUE_LIMIT,	//魅力值不足

		EN_FU_YU_ALREADY_MAX_LEVEL,						//符玉已达到满阶,不可继续强化
		EN_FU_YU_NOT_PROTECT_ITEM,						//未拥有所需道具, 无法保护
		EN_EQUIP_JEWELRY_UP_EQUIP_LEVEL_LIMIT,			//该装备等级不足，无法强化
		EN_BATTLE_CANNOT_SURRENDER,						//当前战斗无法投降
		EN_BATTLE_TOO_SOON_TO_SURRENDER,				//少侠，现在投降还太早
		EN_BATTLE_ONLY_LEADER_CAN_SURRENDER,            //只有队长才能投降
		EN_RA_HUN_QI_FAN_LI_LOTTERY_COUNT_NOT_ENOUGH,	//抽奖次数不足,不可领取

		EN_KUA_FU_YING_XIONG_LU_FETCH_FAIL,				//抱歉,该榜单未开启,不可领取
		EN_MONTH_CARD_CAN_USE_FUNCTION,					//周卡或月卡特权可使用该功能
		EN_DIANHUASHI_ERR_NOT_SAME_STONE,				//请使用同等级点化石

		EN_STAR_CHART_SKILL_HOLE_UNLOCK,				// 该技能孔未锁定
		EN_STAR_CHART_SKILL_LOCK,						// 该技能未解锁
		EN_STAR_CHART_SKILL_TYPE_MISMATCH,				// 技能类型不匹配
		EN_STAR_CHART_SKILL_REPEAT,						// 该技能已被装备,不可重复装备
		EN_STAR_CHART_SKILL_HOLE_UNLOCKED,				// 恭喜您解锁新的技能孔位
		EN_STAR_CHART_POINT_CONDITION_NOT_MET,			// 条件不满足,不可解锁
		EN_STAR_CHART_NOT_ALL_LIGHT_UP,					// 请先点亮前面的星图~

		EN_FASHION_COLLECTION_MAX_SPECIALIZE_LEVEL,		// 专精已达最大等级, 不可继续升级
		EN_FASHION_COLLECTION_STAR_NOT_ENOUGH,			// 所需星数不足,无法升级
		EN_RA_YUE_XI_DENG_MI_ANSWER_QUESTION_ERR,		//抱歉,回答错误~

		EN_FU_YU_LEVEL_NOT_ENOUGH,						//符玉阶数不足
		EN_FU_YU_SP_UNSAME,								//符玉特效不一致,不可升华
		EN_FU_YU_CAN_NOT_SWITCH,						//当前符玉不可转换
		EN_FU_YU_SAME_SP_CAN_NOT_SWITCH,				//转换特效与原特效相同
		
		EN_HUAN_SHOU_NOT_ACTIVE,						//抱歉,该幻兽暂未激活,操作失败
		EN_HUAN_SHOU_STR_LEVEL_LIMIT,					//抱歉,该幻兽已达进化上限,操作失败
		EN_HUAN_SHOU_SKILL_NOT_ENOUGH,					//抱歉,该幻兽已解锁技能数量不足,不可加锁
		EN_HUAN_SHOU_PP_LEVEL_LIMIT,					//抱歉,该点位等级已达上限,操作失败
		EN_HUAN_SHOU_PP_NOT_LEARN_BY_ADJOIN_POINT_LEVEL,//前置点位暂未到达指定等级,激活失败
		EN_HUAN_SHOU_LIAN_DAN_LIMIT,					//炼丹数量已达上限，请稍后再来

		EN_NORMAL_ERROR_NUM_MAX,		
		////////////////////////////////////////// 固定值 //////////////////////////////////////////
		EN_GET_ACCOUNT_GOLD_TOO_FAST = 100000,			//从账号提取元宝间隔时间不足
		EN_COIN_NOT_ENOUGH = 100001,					//铜币不足，可通过锢魔之塔，挑战任务等玩法获得铜币
		EN_GOLD_NOT_ENOUGH = 100002,					//您元宝不足，请前往充值！
		EN_BIND_GOLD_NOT_ENOUGH = 100003,				//绑定元宝不足
		EN_MONEY_IS_LOCK = 100004,						//金钱已经锁定
		EN_ROLE_ZHENQI_NOT_ENOUGH = 100005,				//仙魂不足
		EN_XIANNV_EXP_DAN_LIMIT = 100006,				//仙女经验丹不足
		EN_CONVERTSHOP_BATTLE_FIELD_HONOR_LESS = 100007,//战场荣誉不足
		EN_ROLE_LEVEL_NOT_ENOUTH = 100008,				//角色等级不足，请先升级
		EN_ROLE_CAPABILITY_NOT_ENOUTH = 100009,			//角色战力不足15万，无法报名参加以一敌百
		EN_REWARD_ALREADY_FETCHED = 100010,				//该奖励已领取
		EN_ACTIVITY_NOT_OPEN = 100011,					//活动尚未开启
		EN_INPUT_PARAM_ERROR = 100012,					//参数错误
		EN_ACTIVITY_IN_STANBY_STATUS = 100013,			//活动正在准备中，请各位仙友准备参与！
		EN_CROSS_NOT_CONNECTED = 100014,				//跨服尚未连通，请稍后再试
		EN_MARKET_COIN_BIND_NOT_ENOUGH = 100015,		//神仙币不足
		EN_UNKNOWN_ERROR = 100016,						//未知错误,请稍后再试
		EN_STORAGE_SPACE_NOT_ENOUGH = 100017,			//仓库空间不足
		EN_ROLE_IN_BATTLE = 100018,						//角色正在战斗中，操作失败
		EN_BATTLE_SERVER_NOT_CONNECT = 100019,			//战斗服务器出错，请稍后再试
		EN_ROLE_NOT_IN_BATTLE = 100020,					//角色不在战斗中，操作失败
		EN_CONDITIONS_NOT_REACHED = 100021,				//条件未达成
		EN_TODAY_USE_COUNT_LIMIT = 100022,				//今日使用次数不足
		EN_SHOP_BUY_LIMIT = 100023,						//已达购买上限
		EN_MONSTER_GROUP_NOT_EXIST = 100024,			//怪物组不存在
		EN_SCENE_TARGET_POS_NOT_VALIDATE = 100027,		//目标位置不可走
		EN_ROLE_NAME_REPEAT = 100028,					//昵称已被使用
		EN_IMMORTAL_COIN_NOT_ENOUGH = 100029,			//仙币不足
		EN_SOMEONE_IN_BATTLE = 100030,					//有角色正在战斗中
		EN_TODAY_JOIN_COUNT_LIMIT = 100031,				//今日次数不足
		EN_CHIVALROUS_NOT_ENOUGH = 100032,				//侠义值不足
		EN_ROLE_CAN_NOT_CROSS_NOW = 100033,				//当前处于无法跨服的状态，请稍后再试
		EN_CROSS_FAILED = 100034,						//请求跨服失败
		EN_OPERATE_FORBID_IN_CROSS = 100035,			//该操作在跨服无效，请回原服尝试
		EN_OPERATE_TOO_FREQUENT = 100036,				//操作过于频繁，请稍后再试
		EN_FUN_OPEN_ROLE_LEVEL_LIMIT = 100037,			//角色等级不足，无法操作该功能
		EN_FUN_OPEN_ROLE_TASK_LIMIT = 100038,			//未完成所需任务，无法操作该功能
		EN_HAVE_NOT_REACH_ACTIVE_CONDITION = 10039,		//暂未达成激活条件
		EN_YOUR_CLICK_IS_TOO_OFTEN = 10040,				//少侠的手速太快了，请过会儿再点吧~
		EN_ACTIVITY_IS_END = 10041,						//活动已结束
		EN_OTHER_MONEY_NOT_ENOUGH = 10042,				//所需资源不足，操作失败
		EN_TEAM_MEMBER_NOT_GUILD_MEMBER = 10043,		//队伍中有成员不在同一家族中，操作失败
		EN_TEAM_MEMBER_LEVEL_LIMIT = 10044,				//队伍中有成员不符合等级限制，操作失败
		EN_AVATAR_COLOR_NOT_MATCH = 10045,				//颜色与角色模型或职业不匹配
		EN_START_CROSS_ERROR_PLAT_TYPE = 10046,			//跨服渠道类型错误,无法进入跨服
		EN_START_CROSS_ERROR_PARAM_LEN = 10047,			//跨服版本错误，无法进入跨服
		EN_LOGIN_ACCOUNT_KEY_ERROR = 10048,				//登录Token错误，请您重新登录
		EN_RETURN_ORIGIN_SERVER_FAIL = 10049,			//暂无法返回原服，请稍后再试
		EN_REQ_PARAM_NOT_LONG_ENOUGH = 10050,			//请求参数长度不足
		EN_DEBUG_RAND_NAME_EXIST = 10051,				//随机名字重复啦！（这是测试代码，不会显示在正式服）
		EN_YOUR_ELEMENT_CRYTAL_COUNT_DONE = 10052,		//少侠的灵珠已无剩余使用次数
		EN_MIYI_HAS_BEEN_POKER_GET = 10053,				//本日奖励次数已达上限，无法继续获取翻牌奖励
		EN_CREDIT_NUM_NOT_ENOUGH = 10054,				//交易信用值不足
		EN_DB_SESSION_LOST = 10055,						//服务暂不可用，请稍后再试
		EN_SILVER_COIN_NOT_ENOUGH = 10056,				//灵玉不足
		EN_TARGET_HONG_BAO_INEXISTENCE = 10057,			//目标红包不存在，操作失败
		EN_CFG_VER_CHANGE = 10058,						//配置变更，请重登游戏更新内容再试
		EN_LEVEL_MAX_LIMIT = 10059,						//等级已达到上限
		EN_ASSIST_REWARD = 10060,						//您助人为乐的举动获得了天神的赏赐！
		EN_ACTIVITY_NOT_FLY_SHOE = 10061,				//活动期间,无法离开场景
		EN_CURR_LEVEL_LIMIT = 10062,					//当前等级已达到上限
		EN_HIDDEN_REQ = 10063,							//请前往跨服操作
		EN_OPPONENT_IN_BATTLE = 100064,					//对手正在战斗中，战斗开启失败
		EN_CHAT_SERVER_LOST = 10063,					//聊天服务器维护中,清稍后再试
		EN_BIGCROSS_SERVER_LOST = 10064,				//功能维护中,请稍后再试
		EN_LOTTERY_NUM_NOT_ENOUGH = 10065,				//竞猜币不足
		EN_SERVER_TOO_HOT_TO_CREATE_NEW_ROLE = 10066,	//服务器太火爆啦，请选择最新的服务器哦

		EN_ROLE_IN_BATTLE_1 = 100101,					//角色正在战斗中，操作失败(这是测试代码1)
		EN_ROLE_IN_BATTLE_2 = 100102,					//角色正在战斗中，操作失败(这是测试代码2)
		EN_ROLE_IN_BATTLE_3 = 100103,					//角色正在战斗中，操作失败(这是测试代码3)
		EN_ROLE_IN_BATTLE_4 = 100104,					//角色正在战斗中，操作失败(这是测试代码4)
		EN_ROLE_IN_BATTLE_5 = 100105,					//角色正在战斗中，操作失败(这是测试代码5)
		EN_ROLE_IN_BATTLE_6 = 100106,					//角色正在战斗中，操作失败(这是测试代码6)
		EN_ROLE_IN_BATTLE_7 = 100107,					//角色正在战斗中，操作失败(这是测试代码7)
		EN_ROLE_IN_BATTLE_8 = 100108,					//角色正在战斗中，操作失败(这是测试代码8)
		EN_ROLE_IN_BATTLE_9 = 100109,					//角色正在战斗中，操作失败(这是测试代码9 该提示仅内网服会出现)
		EN_ROLE_IN_BATTLE_10 = 100110,					//角色正在战斗中，操作失败(这是测试代码10)
		EN_ROLE_IN_BATTLE_11 = 100111,					//角色正在战斗中，操作失败(这是测试代码11)	
		
		EN_ROLE_BATTLE_INFO_NOT_FOUND = 100200,			//主角战斗信息未找到（这是测试代码）
		EN_BATTLE_LOCAL_INFO_NOT_FOUND = 100201,		//本地战斗信息未找到（这是测试代码）
		EN_ROLE_BATTLE_ID_AQUIRE_FAILED = 100202,		//获取主角战斗实例ID失败（这是测试代码）
		EN_BATTLE_ID_NOT_EQUAL = 100203,				//战斗实例ID不相同（这是测试代码）
		EN_BATTLE_INSTANCE_AQUIRE_FAILED = 100204,		//获取战斗实例失败（这是测试代码）
		EN_BATTLE_INSTANCE_ROLE_INFO_NOT_FOUND = 100205,//战斗实例中找不到主角的信息（这是测试代码）
		EN_MAX,
	};
	UNSTD_STATIC_CHECK(EN_MAX < 200000);
}

#endif