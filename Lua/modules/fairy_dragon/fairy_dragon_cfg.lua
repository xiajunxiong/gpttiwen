MAX_DRAGON_SOUL_BAG_NUM = 300                       -- 龙魂背包容量
MAX_GOD_DRAGON_SYSTEM_SOUL_GRID_NUM = 5;			--//神龙系统龙魂穿戴格子数量
MAX_GOD_DRAGON_SYSTEM_SKIN_NUM = 30				    --//神龙系统皮肤最大数量
MAX_GOD_DRAGON_USE_GRID_NUM = 30                    --龙魂升级材料最大种类

FairyDragonCfg = {}

FairyDragonCfg.REQ_TYPE =
{
    ALL_INFO = 0,					--//!< 请求神龙系统所有信息 *
    BAG_ALL_INFO = 1,				--//!< 请求神龙系统背包所有信息 *
    UNLOCK_DRAGON = 2,				--//!< 请求解锁神龙 *
    USE_DRAGON_EXP_ITEM = 3,		--//!< 请求使用神龙经验道具 param1:item_id param2:is_use_all(0:使用单次) *
    BREAK = 4,						--//!< 请求神龙突破(进阶) *
    ACTIVE_NODE = 5,				--//!< 请求激活神龙星级节点 param1:node[1,10] *
    STAR_BREAK = 6,					--//!< 请求升星(突破) *
    SKIN_ACTIVE = 7,				--//!< 请求激活皮肤		param1:seq *
    SKIN_STAR_LEVEL_UP = 8,			--//!< 请求皮肤升星		param1:seq *
    USE_SKIN = 9,				    --//!< 请求幻化皮肤		param1:skin_seq *
    CANCEL_SKIN = 10,				--//!< 请求取消幻化 *
    WEAR_SOUL = 11,					--//!< 请求穿戴龙魂		param1:bag_seq *
    SINGLE_PRAY = 12,               -- 请求单次许愿 *
    TEN_PRAY = 13,                  -- 请求十连许愿 *
    --SOUL_LEVEL_UP= 14,              -- 请求龙魂升级 p1, bag_seq     >> 废弃
    UNFIX_SOUL= 14,                 -- 请求卸下龙魂 p1: bag_seq
}

FairyDragonCfg.NOTICE_TYPE = {
    UNLOCK = 0,				--//神龙解锁 param1:level *
    EXP_CHANGE = 1,			--//神龙经验变动 param1:level param2:is_break_flag param3:cur_exp_count *
    BREAK = 2,				--//神龙突破(进阶) param1:level param2:is_break_flag *
    ACTIVE_STAR_NODE = 3,	--//神龙激活星级节点 param1:node_index *
    STAR_BREAK = 4,			--//神龙升星(突破) param1:star_level *
    SKIN_ACTIVE = 5,		--//神龙皮肤激活	param1:seq	param2:star_level *
    SKIN_STAR_LEVEL_UP = 6,	--//神龙皮肤升星	param1:seq	param2:star_level *
    WEAR_DRAGON_SOUL = 7,	--//穿戴龙魂		param1:soul_type param2:bag_seq *
    SKIN_CHANGE = 8,		--//皮肤幻化变化	param1:use_skin_seq *
    SOUL_GRID_UNLOCK = 9,   --//龙魂格子解锁	param1:soul_type *
    SOUL_RESOLVE = 10,      --//龙魂分解		param3:get_dragon_soul_chips_count *
    SYSTEM_OPEN = 11,       --//神龙系统开启    p1:is_open
    UNFIX_SOUL = 12,        --//卸下龙魂        p1:soul_type
}

--龙魂类型
FairyDragonCfg.SOUL_TYPE = {
    All = -1,   --全部
    Gong = 0,   --攻（攻击触发）
    Kong = 1,   --控（受击触发）
    FuZhu = 2,  --辅（死亡触发）
    Fu = 3,     --符（回合开始触发）
    Qu = 4,     --驱（回合结束触发
    Max = 4,
}
