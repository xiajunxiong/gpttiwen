#ifndef __MSG_PARTNER_H__
#define __MSG_PARTNER_H__

#include "gamedef.h"
#include "servercommon/serverdef.h"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/partnerdef.h"

#pragma pack(push, 4)

namespace Protocol
{
	class CSPartnerHandleRequest
	{
	public:
		CSPartnerHandleRequest();			//!< 1710 - 伙伴系统操作
		MessageHeader header;

		enum PARTNER_OP
		{
			PARTNER_INFO_REQUIRE = 0,		//!< 请求所有伙伴信息
			PARTNER_DISP_PARTNER = 1,		//!< 伙伴出战状态改变
			PARTNER_ATTR_BREAKTH = 2,		//!< 属性突破 / 能力进阶
			PARTNER_WASH_FETTERS = 3,		//!< 情缘洗练
			PARTNER_SAVE_FETTERS = 4,		//!< 保存情缘洗练结果
			PARTNER_ACTI_PARTNER = 5,		//!< 道具激活伙伴
			PARTNER_SIGNLE_INFO  = 6,		//!< 请求单个伙伴信息
			PARTNER_UP_LEVEL = 7,			//!< 请求伙伴升级  param1:次数
			PARTNER_RESET_LEVEL = 8,		//!< 请求伙伴重置等级
			PARTNER_OPEN_BIOGRAPHY = 9,		//!< 打开传记	   param1:biography_id
			PARTNER_FETCH_BGP_REWARD = 10,	//!< 领取传记奖励   param1:biography_id
			PARTNER_PAGE_COMPOSE = 11,		//!< 请求伙伴残页合成 param1:item_id
			PARTNER_UPLEVEL_GIFT_TREE = 12,	//!< 请求升级天赋点 param: 天赋下标
			PARTNER_RESET_GIFT_TREE = 13,	//!< 请求重置天赋点 param: 天赋下标
			PARTNER_PACT_PUT_ON = 14,		//!< 元神契约 装备 p1:装备位置  p2:装备save_index
			PARTNER_PACT_GET_OFF = 15,		//!< 元神契约 卸下 p1:装备位置
			PARTNER_PACT_UP_STAR = 16,		//!< 元神契约 升星 p1:装备位置
			PARTNER_PACT_SWAP = 17,			//!< 元神契约 替换 p1:装备位置 p2:原装备伙伴id p3:原装备位置
			PARTNER_SOAR = 18,				//!< 请求飞升 
			PARTNER_SOAR_ARMS_LEVEL_UP = 19,//!< 请求飞升武器升级

			PARTNER_HANDLE_TYPE_MAX,
		};

		short op_type;		//!< 操作类型
		UInt16 partner;		//!< 操作的伙伴的 partner_id 详情见配置表
		int param1;
		int param2;
		int param3;
	};

	class SCPartnerAllDataInform
	{
	public:
		SCPartnerAllDataInform();			//!< 1711 - 所有伙伴的信息
		MessageHeader header;

		int partner_count;						//!< 伙伴数量
		PartnerPro partner[PARTNER_NUM_MAX];	//!< 所有伙伴信息 [0, 29]
	};

	class SCPartnerCombatsStatus
	{
	public:
		SCPartnerCombatsStatus();		//!< 1712 - 改变伙伴出战状态协议回复
		MessageHeader header;

		UInt16 partner;		//!< 对应的伙伴 ID
		char is_fight;		//!< 出战状态： 0 - 待机； 1 - 出战；
		char is_show;		//!< 是否解锁： 0 - 锁定； 1 - 解锁；
	};

	class SCPartnerAttrChangeRep
	{
	public:
		enum NOTICE_TYPE
		{
			NOTICE_TYPE_NO = 0,				//!< 无标记下发
			NOTICE_TYPE_HPMP,				//!< HP,MP变化
			NOTICE_TYPE_BREAKE_THOUGH,		//!< 伙伴突破
			NOTICE_TYPE_UP_GARD,			//!< 伙伴进阶
			NOTICE_TYPE_EQUIP,				//!< 伙伴装备
			NOTICE_TYPE_ROLE_UP_LEVEL,		//!< 人物升级导致属性重算
			NOTICE_TYPE_TRAIN_UP,			//!< 人物修炼导致属性重算
			NOTIC_TYPE_QUALITY_UP,			//!< 伙伴升品属性重算
			NOTIC_TYPE_SKIN,				//!< 伙伴皮肤属性重算 (伙伴激活时)
			NOTIC_TYPE_SKIN_NO_AC,			//!< 伙伴皮肤属性重算 (伙伴未激活)
			NOTIC_TYPE_GUILD_INBORN,		//!< 家族灵脉
			NOTIC_TYPE_ATTR_PELLET,			//!< 属性丹
			NOTIC_TYPE_PACT,				//!< 契约属性(其他伙伴属性变更引起)
			NOTIC_TYPE_PACT_CHANGE,			//!< 用于契约变更
			NOTIC_TYPE_SOAR,				//!< 伙伴飞升
			NOTIC_TYPE_SOAR_ARMS_LEVEL_UP,	//!< 飞升武器强化
			NOTICE_TYPE_XIU_WEI,			//!< 人物修为导致属性重算
			NOTICE_TYPE_INTENSIFY,			//!< 伙伴强化
			NOTICE_TYPE_FU_YU,				//!< 符玉
			NOTICE_TYPE_STAR_CHART,			//!< 星图星位变化
		};
		SCPartnerAttrChangeRep();		//!< 1713 - 伙伴属性变化通知
		MessageHeader header;

		UInt16 partner;								//!< 对应的伙伴 ID
		char list_count;							//!< 改变的属性数量

		char notic_type;

		int capability;								//!< 伙伴评分
		BattleAttrPair attr_list[BATTLE_ATTR_MAX];	//!< 属性数值 - [0, 2147483647]
	};

	class SCPartnerFettersInfoma
	{
	public:
		SCPartnerFettersInfoma();		//!< 1714 - 羁绊界面信息回复
		MessageHeader header;

		UInt16 partner;						//!< 对应的伙伴 ID

		short resver_sh;

		unsigned char curr_fetter[PARTNER_FETTER_COUNT_MAX];		//!< 当前框羁绊
		unsigned char wash_fetter[PARTNER_FETTER_COUNT_MAX];		//!< 洗练框羁绊
	};

	class SCPartnerBaseGradeInfo
	{
	public:
		SCPartnerBaseGradeInfo();		//!< 1715 - 伙伴基础阶级信息
		MessageHeader header;

		UInt16 partner;				//!< 伙伴 ID
		char level;					//!< 进阶等级
		char stage;					//!< 突破阶段 - [0, 7]

		char upgrade_ret;			//!< 进阶标记：0 - 非进阶，不弹窗； 1 - 是进阶，要弹窗

		char task_finish;			//!< 是否完成进阶所需任务
		short resetve_sh;
	};

	class CSToSaveSkillSelection
	{
	public:
		CSToSaveSkillSelection();		//!< 1716 - 保存所选的伙伴技能
		MessageHeader header;

		UInt16 partner;		//!< 伙伴 ID

		short index;		//!< 技能槽 - 0、1、2
		short skill;		//!< 技能ID    -1表示锁定技能槽

		short reserve_sh;	//!< 保留字
	};

	class SCSavePartnersSkillRep
	{
	public:
		SCSavePartnersSkillRep();		//!< 1717 - 伙伴当前技能通知
		MessageHeader header;

		UInt16 partner;
		short fight_skill_list[PARTNER_COMMON_SKILL_MAX];
	};
	UNSTD_STATIC_CHECK(3 == PARTNER_COMMON_SKILL_MAX);

	class SCSynFightPartnerOrder
	{
	public:
		SCSynFightPartnerOrder();		//!< 1718 - 出战伙伴顺序信息
		MessageHeader header;

		int fight_numb;
		int fight_list[PARTNER_SYNFIGHT_NUM_MAX];
	};

	class CSChangeSyncFightOrder
	{
	public:
		CSChangeSyncFightOrder();		//!< 1719 - 调整伙伴出战顺序
		MessageHeader header;

		UInt16 insert_partner;			//!< 将要插入的伙伴ID
		UInt16 target_partner;			//!< 将要变位的伙伴ID
	};

	class CSPartnerSwitchFightPos
	{
	public:
		CSPartnerSwitchFightPos();		//!< 2044 - 互换伙伴出战位置
		MessageHeader header;

		UInt16 partnerA_id;				//!< 伙伴A id
		UInt16 partnerB_id;				//!< 伙伴B id
	};

	class CSPartnerEqOperatorReq
	{
	public:
		CSPartnerEqOperatorReq();		//!< 1770 - 伙伴装备操作请求
		MessageHeader header;

		enum PARTNER_EQUIP_OPERATOR_TYPE
		{
			PE_OP_PULL_ON_EQUIP = 0,		//!< 穿装备
			PE_OP_GET_OFF_EQUIP = 1,		//!< 脱装备
			PE_OP_UNLOCK_EQUIP = 2,			//!< 解锁
			PE_OP_RESOLVE_EQUIP = 3,		//!< 分解
		};

		UInt16 op_type;			//!< 操作类型
		UInt16 partner_id;		//!< 伙伴ID
		int param1;				//!< 参数1		格子ID
	};

	class SCPartnerEqSingleSlots
	{
	public:
		SCPartnerEqSingleSlots();		//!< 1771 - 单个伙伴装备槽信息
		MessageHeader header;

		UInt16 partner_id;			//!< 伙伴ID
		UInt16 equip_slot;			//!< 装备槽位
		UInt16 equip_id;			//!< 装备ID
		UInt16 unlock_level;		//!< 解锁等级
		BattleAttrPair attr_pair[MAX_PARTNER_NATAL_CARD_VOC_NUM];	//!< 属性
	};

	struct SCPartnerQualityUp			//!< 1772 - 伙伴升品下发
	{
		SCPartnerQualityUp();
		MessageHeader header;

		enum NoticeType
		{
			NOTICETYPE_INIT		= 0,					// 初始化下发
			NOTICETYPE_UP		= 1,					// 升品下发
			NOTICETYPE_ACTIVE	= 2,					// 激活伙伴
		};

		int partner_id;								// 伙伴id
		int up_type;								// 升品类型
		int quality;								// 品质
		int att_list[PARTNER_ATTR_MAX];				// 属性值
		char notic_type;							// 下发类型
		char quality_segment;						// 段数	
		short rew_2;								// 保留字

	};

	struct CSPartnerQualityUpReq		//!< 1773 - 伙伴升品请求
	{
		CSPartnerQualityUpReq();
		MessageHeader header;

		enum QualityUpType			//升品类型
		{
			QUALITY_TYPE_BLUE_TO_PURPLE		= 1,			//!< 蓝  --> 紫
			QUALITY_TYPE_PURPLE_TO_ORANGE	= 2,			//!< 紫  --> 橙
			QUALITY_TYPE_ORANGE_TO_RED		= 3,			//!< 橙  --> 红
			QUALITY_TYPE_RED				= 4,			//!< 红  --> 金
			QUALITY_TYPE_GOLD				= 5,			//!< 金色
		};

		int use_type;	  // 升品主动道具类型
		int up_type;      // 升品类型
		int partner_id;	  // 伙伴id

	};

	//struct SCPartnerUseElementCrytal		//!< 1774 - 伙伴元素水晶信息下发
	//{
	//	SCPartnerUseElementCrytal();
	//	MessageHeader header;

	//	int partner_id;								// 伙伴id
	//	int crytal_id;								// 装备的水晶id
	//	int use_time;								// 剩余使用次数
	//	
	//};

	struct SCPartnerUseElementCrytalFight		//!< 1775 - 伙伴元素快速装备下发
	{
		SCPartnerUseElementCrytalFight();
		MessageHeader header;

		int count;									// 要装备水晶的伙伴数量
		PartnerUseElementCrytalFightInfo once_info[PARTNER_NUM_MAX];		
	};

	struct CSPartnerElementCrytalOper		//!< 1776 - 伙伴元素水晶请求
	{
		CSPartnerElementCrytalOper();
		MessageHeader header;

		enum ElementCrytalUseType			
		{
			ELEMENT_CRYTALE_USE_TO_ONCE						= 1,			//!< 为某个伙伴装备元素水晶
			ELEMENT_CRAYTAL_REMOVE_TO_ONCE					= 2,			//!< 卸下某个伙伴的元素水晶
			ELEMENT_CRAYTAL_USE_TO_FIGHT_PARTNER			= 3,			//!< 快速装备元素水晶到出战伙伴
			ELEMENT_CRAYTAL_REMOVE_FROM_EXIST_PARTNER		= 4,			//!< 卸下所有激活伙伴的元素水晶
		};
		int oper_type;		// 操作类型
		int param_1;		// 1 - 要装备元素水晶的伙伴id			2 - 要卸下元素水晶的伙伴id		3 - 快速装备到出战伙伴的元素水晶id_1id在背包的index
		int param_2;		// 1 - 水晶元素物品id在背包的index										3 - 快速装备到出战伙伴的元素水晶id_2id在背包的index
		int param_3;		//																		3 - 快速装备到出战伙伴的元素水晶id_3id在背包的index
		int param_4;		//																		3 - 快速装备到出战伙伴的元素水晶id_4id在背包的index
		
	};

	struct SCParnerSkinActive				//!< 1777 - 伙伴皮肤激活总信息下发
	{
		SCParnerSkinActive();
		MessageHeader header;

		struct PartnerSkinPro
		{
			PartnerSkinPro() { this->Reset(); }
			
			void Reset()
			{
				partner_id = 0;
				skin_active_status = 0;		//二进制, 皮肤配置seq  1:已激活
			}

			short partner_id;
			short skin_active_status;
		};

		int count;									// 拥有皮肤数量
		PartnerSkinPro skin_list[PARTNER_NUM_MAX];	// 皮肤列表  PARTNER_NUM_MAX -- 40
	};

	struct SCParnerSkinSingleOper				//!< 1778 - 伙伴皮肤单个信息下发
	{
		SCParnerSkinSingleOper();
		MessageHeader header;

		enum SendType		// 下发类型
		{
			SKIN_AVTIVE = 0,	// 激活皮肤
			SKIN_PUT_ON = 1,	// 皮肤穿戴
			SKIN_PUT_OFF = 2,	// 皮肤卸下
		};

		int send_type;								// 下发类型
		int partner_id;								// 伙伴id
		int skin_id;								// 皮肤id
	};

	struct CSPartnerSkinReq					//!< 1779 伙伴皮肤请求
	{
		CSPartnerSkinReq();
		MessageHeader header;

		enum ReqType
		{
			SKIN_AVTIVE			= 0,	// 激活皮肤
			SKIN_PUT_ON			= 1,	// 皮肤穿戴
			SKIN_PUT_OFF		= 2,	// 皮肤卸下
		};

		int oper_type;	// 操作类型
		int param_1;	// 伙伴id
		int param_2;	// 皮肤id
	};

	class CSPartnerSkillActivate	//!< 2049 伙伴技能启用/禁用
	{
	public:
		CSPartnerSkillActivate();	
		MessageHeader header;

		int partner_id;		//!< 伙伴ID
		short is_activate;	//!< 0：禁用 1：启用
		short skill_idx;	//!< 技能序号 [0,3)
	};

	class SCMysteryStoreReq			//!< 3350 神秘商店请求
	{
	public:
		SCMysteryStoreReq();
		MessageHeader header;

		enum ReqType
		{
			REQ_REFRESH = 0,// 刷新商品
			REQ_BUY = 1,	// 购买
			REQ_INFO = 2,	// 全部信息
		};

		short req_type;
		short param;
	};

	class SCMysteryStoreInfo		//!< 3351 神秘商店信息
	{
	public:
		SCMysteryStoreInfo();
		MessageHeader header;

		unsigned int next_free_refresh_time;
		unsigned int next_refresh_time;
		short store_refresh_num;						//刷新次数
		short count;
		NewStoreNatalCard data[MYSTERY_STORE_MAX_SEQ];
	};

	class  SCPartnerSingleInfo					//!< 2022 下发伙伴单条信息
	{
	public:
		SCPartnerSingleInfo();
		MessageHeader header;

		PartnerPro partner_info;
	};

	struct PartnerGiftTreeNode
	{
		PartnerGiftTreeNode() :partner_id(0) {}
		int partner_id;
		PartnerGiftTree gift_tree;
	};

	class  SCPartnerGiftTreeListInfo					//!< 2023 下发伙伴天赋树信息
	{
	public:
		SCPartnerGiftTreeListInfo();
		MessageHeader header;

		int partner_num;
		PartnerGiftTreeNode partner_gift_tree_list[PARTNER_NUM_MAX];
	};

	class  SCPartnerSingleGiftTreeInfo					//!< 2024 下发单个伙伴天赋树信息
	{
	public:
		SCPartnerSingleGiftTreeInfo();
		MessageHeader header;

		int partner_id;
		PartnerGiftTree partner_gift_tree;
	};

	//4010 伙伴--元神契约仓库通知
	class SCPartnerSpiritPactList
	{
	public:
		SCPartnerSpiritPactList();
		MessageHeader header;
		
		enum SendType
		{
			SEND_TYPE_ALL = 0,			//全部
			SEND_TYPE_CHANGE = 1,		//改变

		};

		struct PactInfo
		{
			PactInfo()
			{
				save_index = 0;
				item_id = 0;
				num = 0;
			}
			short save_index;	//储存位置
			ItemID item_id;		//物品id
			int num;			//物品数量
		};

		int send_type;

		int count;
		PactInfo pact_list[PARTNER_SPIRIT_PACT_MAX_GRID];
	};

	//4011 伙伴--元神契约 契约用 属性通知
	class SCPartnerPactBaseAttr
	{
	public:
		SCPartnerPactBaseAttr();
		MessageHeader header;

		int partner_id;
		//该伙伴用于给其契约目标增加的属性
		Attribute attach_base_attr_list[PARTNER_ATTR_MAX];
	};

	//4012 伙伴--元神契约 契约改变
	class SCPartnerPactInfo
	{
	public:
		SCPartnerPactInfo();
		MessageHeader header;

		enum Type
		{
			DEF = 0,		//默认
			UP_START,		//升星	p:位置 0-2
		};

		int partner_id;
		int type;
		ItemID put_on_pact_id_list[PARTNER_SPIRIT_PACT_MAX];
		short param;

	};

	class SCPartnerSoarNoitce		//!< 2028 下发伙伴飞升相关通知
	{
	public:
		SCPartnerSoarNoitce();
		MessageHeader header;

		enum NOTICE_TYPE
		{
			NOTICE_TYPE_SOAR = 0,						// 伙伴飞升
			NOTICE_TYPE_SOAR_ARMS_LEVEL_UP = 1,			// 伙伴飞升武器强化

			NOTICE_TYPE_MAX
		};

		int notice_type;
		int partner_id;
		short soar_count;
		short soar_arms_level;
	};

	class CSPartnerIntensifyReq			// 4465
	{
	public:
		CSPartnerIntensifyReq();
		MessageHeader header;

		struct UseItem
		{
			ItemID use_item_id;
			short use_num;
		};

		int partner_id;

		UseItem expend_item_1;
		UseItem expend_item_3;

		int expend_item_2_count;
		UseItem expend_item_2_list[20];
	};

	class SCPartnerIntensifyInfo		// 4466
	{
	public:
		SCPartnerIntensifyInfo();
		MessageHeader header;

		short partner_id;
		short intensify_level;
	};
}

#pragma pack(pop)
#endif