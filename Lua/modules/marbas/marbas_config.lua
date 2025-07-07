MarbasConfig = {
    MaxBag = 500, -- 服务器写死最大长度
    QuaMaster = 10,-- 魔装品质
    QuaEnter = 9, -- 仙装品质
    
    SuitNum = {8},

    NeoType = {
        Build = 1,
        Demons = 2,
    },
    -- 
    ItemInfo = {
        [6] = "marbas_red",
        [9] = "marbas_aya",
        [10] = "marbas_black",
    },
    -- 背包·分解界面帅选
    fix_select= {
        { name = Language.Marbas.FixSelect[0], qua = -1,color = 0},
        { name = Language.Marbas.FixSelect[1], qua = 9,color = 9},
        { name = Language.Marbas.FixSelect[2], qua = 10, color = 10},
    },
    base_suit_show = {
        { flag = true ,},
        { flag = false ,},
    },
    -- 基础界面仙魔模型展示
    base_model = {
        [11] = "803",
        [21] = "809",
        [31] = "807",
        [41] = "805",
        [51] = "813",
        [61] = "811",
        [12] = "804",
        [22] = "810",
        [32] = "808",
        [42] = "806",
        [52] = "814",
        [62] = "812",
    },
    equip_type_sp = {
        [0] = "TouKui",
        [1] = "KaiJia",
        [2] = "FaQi",
        [3] = "DunPai",
        [4] = "XueZi",
        [5] = "JieZhi",
        [6] = "ShouZhuo",
        [7] = "DiaoZhui",
    },
    PageNum = 4,
    LevelShow = {
        [1] = 90,
        [2] = 100,
        [3] = 110,
        [4] = 120,
        [5] = 130,
        [6] = 140,
        [7] = 150,
        [8] = 160,
        [9] = 170,
    },
    ModLevelShow = {
        [1] = 815,
        [2] = 816,
        [3] = 817,
        [4] = 818,
        [5] = 819,
        [6] = 820,
        [7] = 821,
        [8] = 822,
        [9] = 823,
    },
    ComposeCost = {
        [1] = 26447,
        [2] = 26448,
    },
    DefluatGem = 61500,
}
