#ifndef __NOREX_ITEM_H__
#define __NOREX_ITEM_H__

#include "item/itembase.h"
static const int GIFT_ITEM_MAX_NUM = 100;		//随机礼包获得物品的最大数量

class NorexItem : public ItemBase
{
public:
	enum NOREX_ITEM_TYPE
	{
		I_NOREX_ITEM_INVALID = 0,

		I_NOREX_ITEM_JINGYAN_DAN,						// 经验丹
		I_NOREX_ITEM_GIFT,								// 礼包(用于随机范围大的礼包)
		I_NOREX_ITEM_IMMORTAL_COIN_CARD,				// 魔石
		I_NOREX_ITEM_PET_EGGS,							// 宠物蛋
		I_NOREX_ITEM_PET_SKILL,							// 宠物技能书
		I_NOREX_ITEM_PARTNER_LICENCE,					// 伙伴凭证
		I_NOREX_ITEM_WORKSHOP_RECIPE,					// 工坊配方
		I_NOREX_ITEM_SURFACE,							// 幻化
		I_NOREX_ITEM_HP_RECOVER,						// 回血药	param1 非战斗状态回血量   param2 战斗中回复固定值  param3 战斗中净化异常效果百分比
		I_NOREX_ITEM_BIND_COIN_CARD,					// 铜币卡
		I_NOREX_ITEM_TREASURE_MAP,						// 没有净值的藏宝图,使用后消耗该道具并给予玩家有净值的藏宝图
		I_NOREX_ITEM_RECOVER_MP,						// 回蓝药	param1 非战斗状态回蓝量 param2 战斗状态下回蓝量
		I_NOREX_ITEM_GATHER_POWER,						// 采集物活力药	param1 加活力的值
		I_NOREX_ITEM_VILLAGE_PRESTIGE,					// 增加村庄声望	param1 村庄id param2 增加的值
		I_NOREX_ITEM_ADVTASK_TRIGGER,					// 奇遇任务触发道具 - 参数无实意，具体配置在 R-任务-奇遇任务 下的『奇遇任务激活方式』分页中.
		I_NOREX_ITEM_TITLE_CATD,						// 称号卡
		I_NOREX_ITEM_REALIVE,							// 复活药   param1 目标玩家的最大生命上限百分比   param2 战斗中回血系数百分比(使用的是目标回复值*P2)
		I_NOREX_ITEM_OPTIONAL_ITEM,						// 自选礼包(大)
		I_NOREX_ITEM_FABAO,								// 法宝激活道具 param1激活法宝的index
		I_NOREX_ITEM_INST_KILL_MONSTER,					// 降妖鞭   param1 怪物ID   param2 成功率百分比[0, 100]
		I_NOREX_ITEM_FLOWERS,							// 七彩图鉴(鲜花)
		I_NOREX_ITEM_GOLD_CHONGZHI_CARD,				// 元宝卡 - 等价于充值
		I_NOREX_ITEM_EXP,								// 升级丹 param1等级限制	param2 增加经验
		I_NOREX_LUCKYLOTTERY_TICKET,					// 幸运抽奖券  -- 对应幸运转盘活动
		I_NOREX_ITEM_FACESROE_RES,						// 颜值资源
		I_NOREX_ITEM_PARTNER_QUALITYUP,					// 伙伴升品
		I_NOREX_ITEM_MEDAL_SP,							// 附灵石 (附加勋章特效的物品)
		I_NOREX_ITEM_PET_SKIN,							// 宠物皮肤  p1 激活的宠物皮肤id
		I_NOREX_ITEM_PARTNER_SKIN,						// 伙伴皮肤  p1 伙伴id	p2激活的皮肤id
		I_NOREX_ITEM_EXP_PERSENT,						// 百分比经验丹  p1 百分比
		I_NOREX_ITEM_AUTO_QIYUN,						// 自动气运卡		p1  气运值
		I_NOREX_ITEM_QIYUN,								// 气运卡		p1  气运值
		I_NOREX_ITEM_JIFEN,								// 活动积分卡   p1	积分值
		I_NOREX_ITEM_INTIMACY,							// 活动亲密卡   p1	亲密值
		I_NOREX_ITEM_RECOVERY_POWER,					// 秘境体力药剂
		I_NOREX_ITEM_EQUIP,								// 未鉴定装备	p1 装备等级	p2道具id
		I_NOREX_ITEM_BUY_GIFT_ITEM,						// 直购礼包激活卡
		I_NOREX_ITEM_DREAM_NOTES_CHALLENGE,				// 梦渊卷轴  param1:增加次数
		I_NOREX_ITEM_MONEY_AUTO_USE,					// 货币道具自动使用 p1:money_type p2:money_num
		I_NOREX_ITEM_CRESTAL_XIYOU_TIMES,				// 水晶副本稀有模式次数增加
		I_NOREX_ITEM_CONVERT_ITEM,						// 转换道具 p1:item_id  p2:item_num
		I_NOREX_ITEM_PET_GOD_PRINT,						// 宠物神印
		I_NOREX_ITEM_DUEL_REDUCE,						// 减少杀戮值
		I_NOREX_ITEM_FORTUNE_MAZE,						// 幸运迷宫次数
		I_NOREX_ITEM_ROLE_ATTR_PELEET,					// 角色属性丹礼包
		I_NOREX_ITEM_NEW_LIFE_SKILL_PROPS,				// 生活道具(NEW_LIFE_SKILL_PROPS_TYPE) p1:seq
		I_NOREX_ITEM_CHANGE_CARD,						// 变身卡 p1:seq
		I_NOREX_ITEM_GUILD_BUILD_MATERIAL,				// 家族建材, p1:家族资金
		I_NOREX_ITEM_SILVER_COIN_CARD,					// 银两(灵玉)卡  p_1:奖励数量
		I_NOREX_ITEM_MONEY_BOX,							// 货币红包  p_1:红包类型
		I_NOREX_ITEM_LIMITED_TIME_TITLE_CARD,			// 有时限的称号卡 p1:颜值ID(客户端用) p2:称号ID(服务端用)
		I_NOREX_ITEM_DAILY_CUMULATIVE_RECHARGE,			// 每日累充卡
		I_NOREX_ITEM_JIN_YI_TIAN_XIANG_ADD_VALUE,		// 锦衣天香添加抽奖积分
		I_NOREX_ITEM_HP_PERCENT_RECOVER,				// 百分比回血药 param1 非战斗状态回血百分比 param2 战斗中回血百分比  (最大血量取目标)
		I_NOREX_ITEM_MP_PERCENT_RECOVER,				// 百分比回蓝药 param1 非战斗状态回蓝百分比 param2 战斗状态下回蓝百分比 (最大蓝量取目标)
		I_NOREX_ITEM_SMART_MOUNTS_SKILL,				// 灵骑技能书 P1:技能ID P2:技能孔位 P3:技能等级
		I_NOREX_ITEM_HUANSHOU_EGG,						// 指定幻兽蛋 P1:huanshou_id

		I_NOREX_ITEM_MAX,
	};

	NorexItem();
	~NorexItem();

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;
	int GetParam1() const { return m_param1; }
	int GetParam2() const { return m_param2; }
	int GetParam3() const { return m_param3; }
	short GetUseType() const { return m_use_type; }

private:
	short m_use_type;
	int m_param1;
	int m_param2;
	int m_param3;
};

#endif //__NOREX_ITEM_H__
