AttrReasonConfig = {
    MAX_REASON_TYPE_LIST_NUM = 23, --下发属性来源列表最大值
    --属性来源列表类型
    ATTRLIST_REASON_TYPE = {
        TYPE_ROLE = 0, --角色
        TYPE_PET = 1, --宠物
        TYPE_PARTNER = 2, --伙伴
        TYPE_HUASHEN_ROLE = 3, --化身角色
        TYPE_HUASHEN_PET = 4, --化身宠物
        TYPE_MAX = 5
    },
    --人物属性来源
    ROLE_REASON_LIST_TYPE = {
        TYPE_BASE = 0, --基础
        TYPE_POINT = 1, --潜能
        TYPE_EQUIP = 2, --装备
        TYPE_MEDAL = 3, --令牌(勋章)
        TYPE_TRAIN = 4, --修炼
        TYPE_ADV = 5, --进阶
        TYPE_COLLE = 6, --藏品(图鉴+收录)
        TYPE_OTHER = 7, --其它
        TYPR_BICYCLE = 8,--灵骑
        TYPR_FABAO = 9,--法宝
        TYPE_SOUL = 10,--魂器
        TYPE_GOD_PRINT = 11,--神印
        TYPE_PELLET = 12,--属性丹
        TYPE_ACCESSORIES = 13,--配饰
        TYPE_XIU_WEI = 14,--修为
        TYPE_SHENG_QI = 17,--圣器
        TYPE_STAR_MAP = 18,--星图
        TYPE_HUAN_SHOU = 19,--幻兽
        TYPE_LING_BAO = 20,--灵宝系统
        TYPE_SHEN_LONG = 21,--神龙系统
        TYPE_DEFENSE = 22,  --防御
        TYPE_MAX = 23
    },
    --宠物属性来源
    PET_REASON_LIST_TYPE = {
        TYPE_BASE = 0, --基础
        TYPE_GROWTH = 1, --资质
        TYPE_POINT = 2, --潜能
        TYPE_STREN = 3, --强化
        TYPE_RESON = 4, --共鸣
        TYPE_SKILL = 5, --技能(包括护符技能)
        TYPE_TRAIN = 6, --修炼
        TYPE_OTHER = 7, --其它
        TYPR_BICYCLE = 8,--灵骑
        TYPE_FABAO = 9, --法宝
        TYPE_PET_WEAPON = 10,--魂器
        TYPE_PET_STAMP = 11, --神印
        TYPE_PET_ATTRI_ITEM = 12,--属性丹
        TYPE_PET_PRAY = 13,  --祝福
        TYPE_XIU_WEI = 14,--修为
        TYPE_PET_INBORN = 15,--天赋
        TYPE_SHENG_QI = 16,     -- 圣器
        TYPE_STAR_MAP = 17,--星图
        TYPE_GAI_MING = 18,--改命
        TYPE_LING_BAO = 19,--灵宝系统
        TYPE_DEFENSE = 20,  --防御
        TYPE_YOUMING = 21,  --幽冥
        TYPE_MAX = 22,
    },
    --伙伴属性来源
    PARTNER_REASON_LIST_TYPE = {
        TYPE_LEVEL = 0, --等级(等级属性)
        TYPE_EQUIP = 1, --装备(命格)
        TYPE_ADV = 2, --晋升(进阶)
        TYPE_WAKE = 3, --觉醒(升品)
        TYPE_TALENT = 4, --天赋
        TYPE_TRAIN = 5, --修炼
        TYPE_OTHER = 6, --其它
        TYPR_BICYCLE = 7,--灵骑
        TYPE_PACT = 8, --契约
        TYPE_XIU_WEI = 9,--修为
        TYPE_STRENGTHEN = 10,--强化
        TYPE_STAR_MAP = 11,--星图
        TYPE_LING_BAO = 12,--灵宝系统
        TYPE_DEFENSE = 13,  --防御
        TYPE_MAX = 14
    },
    --属性来源列表类型对应Language名字
    REASON_NAME = {[0] = "RoleType", [1] = "PetType", [2] = "PartnerType", [3] = "RoleType", [4] = "PetType",}
}
