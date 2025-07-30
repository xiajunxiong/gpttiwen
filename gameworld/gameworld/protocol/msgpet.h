#ifndef __MSGPET_H__
#define __MSGPET_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"
#include "gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "item/itembase.h"
#include "other/attributelist.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	class CSPetOp						//!< 1700
	{
	public:
		CSPetOp();
		MessageHeader		header;

		enum OP_TYPE
		{
			AbandonmentPet = 1,				//!< 遗弃宠物		遗弃index为p1的宠物
			ConversionExp = 2,				//!< 转换经验		将index为p1的宠物等级降至p2级
			Rebirth = 3,					//!< 返生			将index为p1的宠物返生
			Lock = 4,						//!< 上锁			给index为p1的宠物上锁或解锁
			Breakthroughs = 8,				//!< 强化			强化index为p1的宠物 消耗index为p2 p3 p4的宠物  p5 是否炼妖
			TransferBreakthroughs = 9,		//!< 强化转移		将index为p2的宠物的强化等级转换到index为p1的宠物上
			SetPetState = 10,				//!< 设置宠物状态		将index为p1的宠物设置成p2[1:出战 2:跟随 3:待机1 4:待机2 5:待机3]等状态
			PetAttrsReq = 11,				//!< 请求宠物属性		请求index为p1的宠物的属性
			AddExp = 12,					//!< 使用经验药		给宠物index为p1的宠物使用item_id为p2的道具p3个
			RefreshPoints = 13,				//!< 洗点			给宠物index为p1的宠物洗点
			Refined = 15,					//!< 洗练			p1宠物index，p2 是否技能也一起洗练
			ComprehendPassiveSkill = 16,	//!< 领悟			p1宠物index
			Compose = 17,					//!< 合成			p1 配置里seq
			RefinedSave = 18,				//!< 洗练保存		p1宠物index
			Convert = 19,					//!< 神兽兑换　		p1 配置里seq　 
			RefinedGiveUp = 20,				//!< 洗练放弃  p1宠物index
			MoveStorage = 21,				//!< 移动到仓库  p1 背包格子index p2 仓库格子index
			MovePetGrid = 22,				//!< 移动到背包  p1 背包格子index p2 仓库格子index
			ExtendBag = 23,					//!< 扩展背包
			PetEquipmentPutOn = 24,			//!< 宠物装备穿戴 p1 宠物index p2 背包格子
			PetEquipmentPutOff = 25,		//!< 宠物装备脱下 p1 宠物index
			PetEquipmentChange = 26,		//!< 宠物装备更替 p1 获得装备的宠物index  p2 失去装备的宠物index
			UsePetSkin = 27,		//!< 使用宠物皮肤 p1 宠物index  p2 皮肤id
			UpLevelBreak = 28,		//!< 宠物升级突破 p1 宠物index  
			UpLevelBreakAtrr = 29,		//!<  请求宠物突破属性 p1 宠物index  
			GuideReplacePassiveSkill = 30,		//!<  指引打书		p1 宠物index  
			GuideComprehendPassiveSkill = 31,		//!<  指引领悟 p1 宠物index  
			GuideRefined = 32,					//!< 指引洗练			p1 宠物index  p2 是否洗练技能 
			ReplacePassiveSkillSave = 33,					//!< 保存打书技能 p1 宠物index  p2 0保存，1是放弃
			PutOnPetSoulEquipMent = 34,				//!< 穿戴魂器 p1宠物下标，p2背包index
			PutOffPetSoulEquipMent = 35,			//!< 脱下魂器 p1宠物下标
			StepsPetSoulEquipMent = 36,				//!< 突破 p1 是否穿戴（0/1） p2 下标（背包下标/宠物index）
			RefinePetSoulEquipMent = 37,			//!< 精炼 p1 是否穿戴（0/1） p2 下标（背包下标/宠物index）
			LockPetSoulEquipMent = 38,				//!< 锁定魂器 p1 是否穿戴（0/1），p2 下标（背包下标/宠物index）,p3 (0解锁/1锁定)
			PutOnPetGodPrintEquipMent = 39,			//!< 穿戴神印 p1 宠物index，p2 神印背包index[1 - 100]
			PutOffPetGodPrintEquipMent = 40,		//!< 脱下神印 p1 宠物index，p2 对应穿戴位置posi[1 - 100]
			UpLevelPetGodPrintEquipMent = 41,		//!< 升级神印 p1 背包下标index[1 - 100] p2赋予经验值
			DisComplsePetGodPrintEquipMent = 42,	//!< 分解神印 p1 背包对应index[1 - 100]
			PetGodPrintReqWish = 43,				//!< 祈神 p1 使用铜币/元宝（0/1） p2 消耗总数 p3 是否自动分解材料
			PetGodPrintReqWishLogList = 44,			//!< 祈神日志列表
			PetGodPrintReqWishLogDetail = 45,		//!< 祈神日志详细 p1 日志下标index
			PetGodPrintReqExchange = 46,			//!< 祈神兑换 p1 seq
			PetGodPrintReqLock = 47,				//!< 神印锁定 p1 背包下表[1-100] p2 是否锁定[1/0]
			PetGodPrintReqUpLevelItem = 48,			//!< 神印消耗背包材料升级 p1 神印背包下表[1-100] p2 背包类型，p3 背包下表， p4 消耗数量
			PetGodPrintReqWearScheme = 49,			//!< 穿戴方案神印 p1 宠物index p2 方案idx
			PetGodPrintReqDiscomplse = 50,			//!< 分解背包内神印材料获取经验 p1 背包类型，p2 背包下表， p3 消耗数量
			PET_OP_TYPE_KEY_LEVEL_UP = 51,			//!< 宠物一键升级操作 P1:pet_index
			PetSoulEquipExchange = 52,				//!< 宠物魂器兑换 P1:兑换seq
			PetSoulEquipChange = 53,				//!< 宠物魂器转化 p1 是否穿戴（0 = 穿戴/1 = 否） p2 下标 p3 转化道具id
			PetMutant = 54,							//!< 宠物使用物品变异 P1:pet_index
			SkillBookConvert = 55,					//!< 超级技能书兑换 p1 道具ID1  p2 道具ID2
			PetBenediction  = 56,					//!< 宠物祝福 P1:main_pet_index(主宠) P2:vice_pet_index(副宠)
			PetCancelBenediction = 57,				//!< 取消宠物祝福 P1:vice_pet_index(副宠)
			StartPetFeiShengTask= 58,				//!<  p1: 开始宠物飞升任务pet_index
			UpLevelPassiveSkill = 59,				//!<  p1: pet_index  p2 被动技能index p3 是否炼妖技能
			PetFeiSheng = 60,				//!<  p1: pet_index  p2 选择的技能index
			PetFeiShengSkillMod = 61,				//!<  p1: pet_index  p2 选择的技能index
			PetFeiShengRewardFetch = 62,				//!<  宠物异闻录 p1: mutant_id p2 第几次飞升奖励
			PetFeiShengSkillUplv = 63,				//!<  飞升技能升级 p1: pet_index
			PetGodPrintReqExchange2 = 64,			//!< 神印置换 p1:消耗品1_idx p2:消耗品2_idx
			PetBadgePutOn = 65,				//!<  穿戴 p1: pet_index  p2,p3 背包格子index
			PetBadgeTakeOff = 66,				//!<  脱下 p1: pet_index 
			PetBadgeExChange = 67,				//!< 替换 p1: pet_index  p2 pet_index  
			PetBadgeCompose = 68,				//!< 合成 p1: 配置seq
			PetLianYao = 69,					//!< 炼妖 p1: 主宠 pet_index   p2 pet_index  
			PetLianYaoComprehendPassiveSkill = 70,			//!< 炼妖领悟技能 p1:pet_index 
			PetUnlockTalent = 71,				//!< 解锁宠物天赋 p1:pet_index p2:talent_type[1,20] p3:inborn_grid[1,3]
			PetTalentSkillLevelUp = 72,			//!< 宠物天赋技能升级 p1:pet_index p2:talent_type[1,20]
			PetTalentSkillReplace = 73,			//!< 宠物天赋技能替换 p1:pet_index p2:talent_type[1,20] p3:new_inborn_grid[1,3]
			PetTalentReset = 74,				//!< 宠物天赋重置 p1:pet_index
			PetStrengthenBuffSet = 75,				//!< 宠物强化buff p1:格子类型 p2 格子index p3 pet_index
			PetStrengthenBuffUnSet = 76,				//!< 取消宠物强化buff p1:格子类型 p2 格子index 
			PetGodPrintDevour = 77,				//!< 神印吞噬 p1: 吞噬神印index p2:被吞噬神印index p3:吞噬词条index
			PetGodPrintDrop = 78,				//!< 丢弃神印 p1: 背包index（该神印必须非绑，非穿戴中）
			PetGaiMingUpLevel = 79,				//!< 改命 p1:pet_index
			PetGaiMingAttrUpLevel = 80,				//!< 改命加属性 p1: pet_index p2 净值改命属性数组 index
			PetGaiMingAttrReset = 81,				//!< 改命重置属性 p1: pet_index 
		};

		int op_type;
		int p1;
		int p2;
		int p3;
		int p4;
		int p5;
	};

	class CSPetRename						//!< 改名 1701
	{
	public:
		CSPetRename();
		MessageHeader		header;

		int index;
		GameName name;
	};

	class CSPetAssignAttributePoints		//!< 分配属性点 1702
	{
	public:
		CSPetAssignAttributePoints();
		MessageHeader		header;

		int index;
		int con;
		int str;
		int def;
		int agi;
		int magic;
	};

	enum PET_ALL_INFO_REASON
	{
		PET_ALL_INFO_REASON_DEFAULT,		//默认
		PET_ALL_INFO_REASON_TRAIN_SKILL,	//修炼技能
		PET_ALL_INFO_REASON_GUILD_INBORN,	//家族灵脉
		PET_ALL_INFO_REASON_XIU_WEI,		//修为
		PET_ALL_INFO_REASON_STAR_CHART,		//星图
	};
	const static int MAX_PET_SOUL_EQUIPMENT_COST = 10;

	struct MsgPetInfo
	{
		MsgPetInfo() : rank(-1) {}

		PetPro pet_pro;
		int rank;			// 排名，从0开始，-1为未上榜
	};

	class SCSendPetAllInfo				//!< 1703 宠物所有的信息 只在登录时发一次 
	{
	public:
		SCSendPetAllInfo();
		MessageHeader		header;

		int reason;
		int pet_count;		//!< 根据这个参数读后面的list个数
		int pet_storage_count;
		MsgPetInfo pet_list[ItemNamespace::MAX_PET_GRID_NUM + ItemNamespace::MAX_PET_STORAGE_GRID_NUM];
	};

	class SCSendPetSingleInfo				//!< 1704 单个宠物的信息
	{
	public:
		enum PET_NOTIE_TYPE
		{
			PET_NOTIE_TYPE_NO_TYPE = 0,//!< 无标记下发
			PET_NOTIE_TYPE_EXP,//!< 经验增加下发
			PET_NOTIE_TYPE_REBIRTH,//!< 宠物返生
			PET_NOTIE_TYPE_RECOVER,//!< 状态复原（满血满蓝）
			PET_NOTIE_TYPE_HPMP,//!< 回血回蓝
			PET_NOTIE_TYPE_ASSIGN_POINTS,//!< 属性点
			PET_NOTIE_TYPE_SKILL,//!< 技能变更
			PET_NOTIE_TYPE_GIFT,//!< 天赋变更
			PET_NOTIE_TYPE_RENAME,//!< 重命名
			PET_NOTIE_TYPE_LOCK,//!< 锁定
			PET_NOTIE_TYPE_STATUS,//!< 宠物状态变更
			PET_NOTIE_TYPE_BREAK_THROUGHS,//!< 宠物突破
			PET_NOTIE_TYPE_FABAO,//!< 法宝影响宠物属性
			PET_NOTIE_TYPE_REFINED,//!< 洗练下发
			PET_NOTIE_TYPE_COMPREHEND_PASSIVE ,//!< 领悟技能
			PET_NOTIE_TYPE_REPLACE_PASSIVE,//!< 打书
			PET_NOTIE_TYPE_REFINED_SAVE,//!< 洗练保存下发
			PET_NOTIE_TYPE_BATTLE_ADD_EXP,//!< 宠物战斗结束增加经验
			PET_NOTIE_TYPE_PET_EQUIPMENT_CHANGE,//!< 宠物装备穿脱变更
			PET_NOTIE_TYPE_PET_USE_SKIN,		//!< 使用宠物皮肤
			PET_NOTIE_TYPE_PET_UP_LEVEL_BREAK,		//!< 使用宠物升级突破
			PET_NOTIE_TYPE_AUTO_ASSIGN_POINTS,	//!< 自动分配属性点
			PET_NOTIE_TYPE_DOMINANCE_CHANGE,	//!< 灵骑统御变更
			PET_NOTIE_TYPE_REPLACE_PASSIVE_SAVE,	//!< 保存打书
			PET_NOTIE_TYPE_RESON,				//共鸣
			PET_NOTIE_TYPE_SET_ADD_POINT_INFO,	// 设置自动加点信息
			PET_NOTIE_TYPE_PET_SOUL_EQUIPMENT,	//!< 宠物魂器穿脱变更
			PET_NOTIE_TYPE_PET_SOUL_LOCK,		//!< 魂器锁定
			PET_NOTIE_TYPE_PET_SOUL_DATA_CHANGE,//!< 魂器数值变化
			PET_NOTIE_TYPE_PET_GOD_PRINT,		//!< 宠物神印变化
			PET_NOTIE_TYPE_KEY_LEVEL_UP,		//!< 宠物一键升级
			PET_NOTIE_TYPE_ATTR_PELLET,			//!< 属性丹
			PET_NOTIE_TYPE_PET_SOUL_EXCHANGE,	//!< 魂器转换
			PET_NOTIE_TYPE_BENEDICTION,			//!< 祝福变更
			PET_NOTIE_TYPE_FINISH_FEI_SHENG_TASK,			//!< 完成飞升任务完成
			PET_NOTIE_TYPE_FEI_SHENG,			//!< 完成飞升
			PET_NOTIE_TYPE_FEI_SHENG_SKILL_MOD,			//!< 宠物飞升技能修改
			PET_NOTIE_TYPE_UPGRADE_SKILL,			//!< 宠物升级技能
			PET_NOTIE_TYPE_UPGRADE_FEI_SHENG_SKILL,			//!< 宠物升级飞升技能
			PET_NOTIE_TYPE_BADGE_CHANGE,			//!< 宠物徽章改变
			PET_NOTIE_TYPE_LIAN_YAO,			//!< 宠物炼妖
			PET_NOTIE_TYPE_LIAN_YAO_COMPREHEND_PASSIVE_SKILL, //!< 宠物炼妖领悟技能
			PET_NOTIE_TYPE_UNLOCK_TALENT,			//!< 42.解锁宠物天赋
			PET_NOTIE_TYPE_TALENT_SKILL_LEVEL_UP,	//!< 43.宠物天赋技能升级
			PET_NOTIE_TYPE_TALENT_SKILL_REPLACE,	//!< 44.宠物天赋技能替换
			PET_NOTIE_TYPE_TALENT_RESET,			//!< 45.宠物天赋重置
			PET_NOTIE_TYPE_HOLY_BLESS,				//!< 46.圣器圣佑
			PET_NOTIE_TYPE_STRENGTH_BUFF,				//!< 47.宠物共鸣
			PET_NOTIE_TYPE_GAI_MING,				//!< 48.宠物改命
			PET_NOTIE_TYPE_GAI_MING_ATTR,				//!< 49.宠物改命属性
			PET_NOTIE_TYPE_GAI_MING_ATTR_RESET,			//!< 50.宠物改命属性重置
		};
		SCSendPetSingleInfo();
		MessageHeader		header;

		int notie_type;
		int add_exp;				// 战斗结束宠物增加的经验
		MsgPetInfo pet_info;
	};

	class SCSendPetDeleteInfo				//!< 1706 删除宠物
	{
	public:
		SCSendPetDeleteInfo();
		MessageHeader		header;

		int pet_index;
	};

	class SCSendPetOtherInfo			//!< 1705 宠物其他信息
	{
	public:
		SCSendPetOtherInfo();
		MessageHeader		header;

		short		fight_index;		//!< 出战宠物index
		short		follow_index;		//!< 跟随宠物index
		short		standby_index1;		//!< 待机宠物index
		short		standby_index2;		//!< 待机宠物index
		short		standby_index3;		//!< 待机宠物index
		short		extend_grid_num;	//!< 已扩展的格数
		short		extend_times;		//!< 已扩展的次数
		short		standby_index4;		//!< 待机宠物index
		short		standby_index5;		//!< 待机宠物index
	};

	class SCSendPetAttrListInfo			//!< 1707 宠物属性
	{
	public:
		SCSendPetAttrListInfo();
		MessageHeader		header;

		int index;
		int capability;
		AttributeList attr_list;
	};

	class SCSendPetBreakthroughsRet			//!< 1708 强化返回
	{
	public:
		SCSendPetBreakthroughsRet();
		MessageHeader		header;

		int index;
		int is_succ;		/// < 0失败 1 成功
		int old_level;
		int is_liao_yao_pet;
	};

	class SCSendPetGetRet			//!< 1709 获得宠物返回
	{
	public:
		SCSendPetGetRet();
		MessageHeader		header;

		int index;
		int reason;
	};

	class SCPetFollowStatus
	{
	public:
		SCPetFollowStatus();		// 1690 - 跟随宠物信息
		MessageHeader header;

		int role_obj_id;			// 玩家ObjID
		int follow_pet_id;			// 宠物ID
		GameName follow_pet_name;	// 宠物名字
		int skin_id;
		int fei_sheng_times;		
	};

	class SCSealPetGetItemInfo
	{
	public:
		SCSealPetGetItemInfo();		// 1691 - 捕抓宠物得到什么物品
		MessageHeader header;

		int pet_id;
		int item_id;
		short num;
		short is_get_pet;	//!< 0: 获得碎片   1:获得宠物
	};

	class SCPetIdRecordList  //!<1692 宠物图鉴
	{
	public:
		SCPetIdRecordList();		
		MessageHeader header;

		int count;
		unsigned short pet_id_list[PET_ID_MAX_COUNT];
	};

	class CSAbandonPetByList			//!< 1693 批量遗弃宠物
	{
	public:
		CSAbandonPetByList();
		MessageHeader		header;

		unsigned char del_pet_index_list[ItemNamespace::MAX_PET_GRID_NUM];
	};

	enum MOVE_PET_ITEM_NOTICE_TYPE
	{
		MOVE_PET_ITEM_NOTICE_TYPE_FAIL,			// 是失败
		MOVE_PET_ITEM_NOTICE_TYPE_STORAGE,		//移动到仓库
		MOVE_PET_ITEM_NOTICE_TYPE_GRID,			//仓库移动到背包
	};

	class SCMovePetGridNotice				//!< 1694 移动物品通知
	{
	public:
		SCMovePetGridNotice();
		MessageHeader		header;

		int type;				//!< 0是失败，1是移动到仓库，2是仓库移动到背包
		int pet_index;
		int pet_storage_index;
	};

	class SCPetSkinInfo				//!< 1695 宠物皮肤
	{
	public:
		SCPetSkinInfo();
		MessageHeader		header;

		int count;
		int pet_skin_list[PET_SKIN_MAX_NUM];
	};

	class SCPetCalAttrRet				//!< 1696 客户端用来显示突破预览数值，防止误差
	{
	public:
		SCPetCalAttrRet();
		MessageHeader		header;

		int index;
		AttributeList attr_list;
	};

	class CSPetSoulUplevelReq			//!< 1697 魂器升级
	{
	public:
		CSPetSoulUplevelReq();
		MessageHeader		header;
		const static int MAX_USE_ITEM_NUM = 30;//最大消耗道具数
		int is_put_on;
		short index;
		short pet_soul_index;
		int cost_num;
		short bag_colum[MAX_USE_ITEM_NUM];
		short bag_idx[MAX_USE_ITEM_NUM];
		short bag_num[MAX_USE_ITEM_NUM];
	};
	UNSTD_STATIC_CHECK(sizeof(CSPetSoulUplevelReq) % 4 == 0);

	struct GodPrintItemSendInfo
	{
		int bag_idx;//背包下表
		GodPrintItem item_info;//背包信息
	};

	class SCPetGodPrintBagInfo			//!< 1661 神印背包
	{
	public:
		SCPetGodPrintBagInfo();
		MessageHeader		header;

		int round_seq;
		long long exp;//存有的经验
		int today_free_wish_times;//使用免费祈神次数
		GodPrintWearScheme wear_scheme_list[MAX_PET_GOD_PRONT_SCHEME_NUM];	//预存方案
		int bag_num;
		GodPrintItemSendInfo bag_list[NEW_MAX_PET_GOD_PRONT_BAG_NUM];			//神印背包
	};

	class SCPetGodPrintBagSingleInfo			//!< 1660 神印背包单个修改
	{
	public:
		SCPetGodPrintBagSingleInfo();
		MessageHeader		header;
		int put_reason;
		int bag_idx;
		long long exp;
		GodPrintItem bag_list;				//神印背包
	};

	class SCPetGodPrintEXPInfo			//!< 1662 神印经验池
	{
	public:
		SCPetGodPrintEXPInfo();
		MessageHeader		header;

		long long exp;
	};

	class SCPetGodPrintWearSchemeSingleInfo			//!< 1663 神印穿戴方案单个下发
	{
	public:
		SCPetGodPrintWearSchemeSingleInfo();
		MessageHeader		header;

		int bag_idx;
		GodPrintWearScheme bag_list;				//神印背包
	};

	class CSPetGodPrintSetearSchemeReq			//!< 1664 设置神印穿戴方案
	{
	public:
		CSPetGodPrintSetearSchemeReq();
		MessageHeader		header;

		int bag_idx;
		GodPrintWearScheme bag_list;				//神印背包
	};

	class SCPetGodPrintWishLogListInfo			//!< 1665 神印日志列表下发
	{
	public:
		SCPetGodPrintWishLogListInfo();
		MessageHeader		header;
		struct GodPrintLogSinple
		{
			GodPrintLogSinple() :log_tamp(0), item_id(0) {}
			unsigned int log_tamp;//日志时间戳
			int item_id;//神印id
		};
		int log_num;	//日志数量
		GodPrintLogSinple log_data[MAX_GOD_PRINT_WISH_LOG_NUM];
	};

	class SCPetGodPrintWishLogDetailInfo			//!< 1666 神印日志详细下发
	{
	public:
		SCPetGodPrintWishLogDetailInfo();
		MessageHeader		header;

		NewWishLogData log_detail;
	};
	const static int MAX_GOD_PRINT_WISH_REWARD_NUM = 80;//最大祈神奖励数量
	class SCPetGodPrintWishRewardInfo			//!< 1667 祈神奖励下发
	{
	public:
		SCPetGodPrintWishRewardInfo();
		MessageHeader		header;

		struct WishInfo
		{
			WishInfo() {}
			int bag_idx;//背包下标
			int wish_idx;//祈神seq
			int item_group;//通用材料group
			int item_seq;//通用材料seq
		};
		int today_free_times;//使用免费祈神次数
		int use_gold;//[0/1]是否使用元宝
		int real_cost_coin;//实际花销
		int reward_num;
		int end_seq;//最终停留下标
		WishInfo reward_list[MAX_GOD_PRINT_WISH_REWARD_NUM];//神印背包下标
	};

	class CSPetGodPrintUplevelReq			//!< 1668 神印消耗神印升级
	{
	public:
		CSPetGodPrintUplevelReq();
		MessageHeader		header;
		
		int index;
		int bag_num;
		short bag_idx[MAX_USE_ITEM_NUM];
	};

	class SCPetBenedictionAllInfo		//!< 1698 宠物祝福列表信息下发
	{
	public:
		SCPetBenedictionAllInfo();
		MessageHeader header;

		short benediction_pet_index_list[ItemNamespace::MAX_PET_GRID_NUM];		// 宠物祝福列表
	};

	class SCPetBenedictionSingleInfo	//!< 1699 单个宠物祝福信息下发
	{
	public:
		SCPetBenedictionSingleInfo();
		MessageHeader header;

		short pet_index;				// 哪个宠物
		short main_pet_index;			// 受到祝福的宠物
	};

	class SCPetFeiShengTaskInfo	//!<4300 宠物飞升任务
	{
	public:
		SCPetFeiShengTaskInfo();
		MessageHeader header;

		long long pet_uuid;
		int today_is_finish;
		int task_id;

	};

	class SCPetFeiShengRewardInfo				//!< 4420 
	{
	public:
		SCPetFeiShengRewardInfo();
		MessageHeader		header;

		int count;
		PetYiWenLuRewardParam infos[100];
	};

	class CSPetComposeBatch		//!< 4430   批量合成
	{
	public:
		CSPetComposeBatch();
		MessageHeader		header;

		struct Item
		{
			short seq;
			short use_shards_num;
		};
		
		Item batch_req[100];   //不用不满100
	};

	class SCPetComposeBatchRet		//!< 4431   批量合成 返回
	{
	public:
		SCPetComposeBatchRet();
		MessageHeader		header;

		int count;
		short pet_index_list[100];
	};

	class CSPetComposeAndDecomposeBatch		//!< 4432   批量合成后并分解，就是兑换道具
	{
	public:
		CSPetComposeAndDecomposeBatch();
		MessageHeader		header;

		struct Item
		{
			short seq;
			short num;
		};

		Item batch_req[100];   //不用不满100
	};

	class CSPetBadgeDecompose   //4433 宠物徽章分解
	{
	public:
		CSPetBadgeDecompose();
		MessageHeader header;

		KnapsackItemIndex decompose_list[100];
	};

	class SCPetStrengthenBuffInfo   //4434 宠物强化
	{
	public:
		SCPetStrengthenBuffInfo();
		MessageHeader header;

		PetStrengthenBuffParamItem strengthen_buff_item_list[PET_STRENGTHEN_BUFF_TYPE_NUM];
	};

	class SCPetLianYaoCommonInfo   //5230 宠物炼妖信息
	{
	public:
		SCPetLianYaoCommonInfo();
		MessageHeader header;

		unsigned int is_unlock_lian_yao_furance_flag; 
	};

	enum PET_ADD_EXP_NOTICE_TYPE
	{
		PET_ADD_EXP_NOTICE_TYPE_GE_ROLE_LVEL=0, //大于等于角色等级
	};

	class SCPetAddExpNotice   //5231 宠物添加经验通知
	{
	public:
		SCPetAddExpNotice();
		MessageHeader header;

		int notice_type;
		int pet_index;
	};
}

#pragma pack(pop)
#endif