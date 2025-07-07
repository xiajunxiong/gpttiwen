ActiveConfig = {
    TabList = {
        [1] = Mod.Activity.Daily,
        [2] = Mod.Activity.TimeLimit,
        [3] = Mod.Activity.WeekReward,
        [4] = Mod.Activity.ActivityBack,
        -- [5] = Mod.Activity.ActiveRoller,
    },
    FlagType = {
        CantReward = 0, --未完成无法领奖
        CanReward = 1,
        Rewarded = 2
    },
    RewardKey = {
        [1] = {key = "integral_active",item_id = CommonItemId.ActivePoint},
        [2] = {key = "strength",item_id = CommonItemId.StrengthPoint},
        [3] = {key = "exp",item_id = CommonItemId.Exp},
        [4] = {key = "gold",item_id = CommonItemId.Gold},
        [5] = {key = "huanjie_money",item_id = CommonItemId.BattlefieldPoint},
    },
    HonorType = {
        [ActType.GuildEmbattle] = true,
    },
    GuildConfig = {
        [ActType.GuildMelee] = true,
        [ActType.GuildEmbattle] = true,
        [ActType.GuildAnswer] = true
    },
    GloryType = {
        [ActType.HeroVanBattle] = true,
        [ActType.UTArena2] = true
    },
    HeroType = {
        [ActType.HeroVanBattle] = true,
    }
}