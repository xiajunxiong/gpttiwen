IllustrateConfig = {
    Req = {
        Active = 1, --激活，p1：seq
        Convert = 2, --兑换，p1：seq
        Included = 3, --收录，p1：seq
        Treasure = 4, --珍品升级卡片，p1：seq
    },
    COLOR = {
        [2] = COLORS.Green1,
        [3] = COLORS.Blue1,
        [4] = COLORS.Purple3,
        [5] = COLORS.Red2,
    },
    None = "collect_none",
    BaseType = {"maxhp","maxmp","gongji","fangyu","minjie","jingshen","huifu"},
    DefenceType = {"def_zhongdu","def_shihua","def_hunshui","def_jiuzui","def_hunluan"},
    ModifyType = {"mogong","mokang","bisha","jianren","fanji","mingzhong","shanduo","bisha_dmg"},
    ChangeList = {[1] = 5,[2] = 4,[3] = 3,[4] = 2,[5] = 1,[6] = 6},
    CollectCardColor = {
        [1] = {COLORS.Green3,COLORS.Green4,COLORS.Yellow5},
        [2] = {COLORS.Green3,COLORS.Green4,COLORS.Yellow5},
        [3] = {COLORS.Blue2,COLORS.Blue11,COLORS.Yellow5},
        [4] = {COLORS.Purple1,COLORS.Purple2,COLORS.Yellow5},
        [5] = {COLORS.Yellow18,COLORS.Yellow16,COLORS.Yellow5},
        [6] = {COLORS.Red8,COLORS.Red18,COLORS.Yellow5},
    },
    CollectCardName = "collection_back",

    --最大页数
    MAX_PAGE_NUM = 6,

    --底框前缀
    ImageName = {
        [1] = "pet_collect_",
        [2] = "partner_collect_",
        [3] = "magic_weapon_",
        [4] = "heart_skill_",
        [5] = "heart_skill_",
        [6] = "heart_skill_",
    },
    --品质名字对应标题
    PageName = {
        [1] = Language.Pet.ColorLevel,
        [2] = Language.Partner.ColorLevel,
        [3] = Language.Fabao.ColorLevel,
        [4] = Language.HeartSkill.ColorLevel,
        [5] = Language.PetWeapon.ColorLevel,
        [6] = Language.PetWeapon.ColorLevel,
    },
    --品质名字对应 颜色渐变：颜色1，颜色2 + 描边 
    PageColor = {
        [1] = {
            [1] = {COLORS.Yellow11,COLORS.Yellow17,COLORS.Red17},
            [2] = {COLORS.Purple6,COLORS.Blue4,COLORS.Blue1},
            [3] = {COLORS.Yellow16,COLORS.Yellow21,COLORS.Yellow5},
            [4] = {COLORS.Green7,COLORS.Green8,COLORS.Green1},
            [5] = {COLORS.Blue2,COLORS.Blue10,COLORS.Blue5},
        },
        [2] = {
            [3] = {COLORS.Blue2,COLORS.Blue11,COLORS.Blue5},
            [4] = {COLORS.Purple1,COLORS.Purple2,COLORS.Purple3},
            [5] = {COLORS.Yellow6,COLORS.Yellow25,COLORS.Yellow5},
        },
        [3] = {
            [3] = {COLORS.Blue2,COLORS.Blue11,COLORS.Blue5},
            [4] = {COLORS.Purple1,COLORS.Purple2,COLORS.Purple3},
            [5] = {COLORS.Yellow6,COLORS.Yellow25,COLORS.Yellow5},
        },
        [4] = {
            [3] = {COLORS.Blue2,COLORS.Blue11,COLORS.Blue5},
            [4] = {COLORS.Purple1,COLORS.Purple2,COLORS.Purple3},
            [5] = {COLORS.Yellow18,COLORS.Yellow16,COLORS.Yellow5},
            [6] = {COLORS.Red8,COLORS.Red18,COLORS.Yellow5},
        },
        [5] = {
            [3] = {COLORS.Blue2,COLORS.Blue11,COLORS.Blue5},
            [4] = {COLORS.Purple1,COLORS.Purple2,COLORS.Purple3},
            [5] = {COLORS.Yellow18,COLORS.Yellow16,COLORS.Yellow5},
            [6] = {COLORS.Red8,COLORS.Red18,COLORS.Yellow5},
        }, 
        [6] = {
            [3] = {COLORS.Blue2,COLORS.Blue11,COLORS.Blue5},
            [4] = {COLORS.Purple1,COLORS.Purple2,COLORS.Purple3},
            [5] = {COLORS.Yellow18,COLORS.Yellow16,COLORS.Yellow5},
            [6] = {COLORS.Red8,COLORS.Red18,COLORS.Yellow5},
        }, 
    },
    --获取对应的Item SetData的数据data + item的名字
    INCLUDED_ITEM_FUNC = {
        [1] = function(id)
            local item = Pet.New({pet_id = id})
            return item,item:Name()
        end,
        [2] = function(id)
            local config = Config.partner_cfg_auto.partner_info[id] or {}
            return {id = id},config.name or ""
        end,
        [3] = function(id)
            local config = FabaoData.Instance:FabaoInfo(id) or {}
            return {id = id},config.name or ""
        end,
        [4] = function(id)
            local config = HeartSkillData.Instance:GetSkillConfig(id) or {}
            return {id = id},config.heart_skill_name or ""
        end,
        [5] = function(id)
            local item = PetWeapon.New({item_id = id})
            return item,item:Name()
        end,
        [6] = function(id)
            local item = ShengQi.New({item_id = id})
            return item,item:Name()
        end
    },
    Rare = {
        Title = "_LocTitle",
        Bg = "KaPianDi",
        Frame = "KaPianKuang",
        Name = "MingZiDi",
    },
}