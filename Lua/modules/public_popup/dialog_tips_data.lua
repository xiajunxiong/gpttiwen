-- 这个今日不在显示其实是本次登录不再显示，包括切换账号后也不会显示tnr
DTTodayNotRemind = {
    WorkshopForge = "WorkshopForge",
    WorkshopLife = "WorkshopLife",
    PetSkillLearn = "PetSkillLearn",
    PetEquipBreak = "PetEquipBreak",
    DivinationAddLot = "DivinationAddLot",
    DivinationReset = "DivinationReset",
    MedalSpFuling = "MedalSpFuling",
    TreasureHunt = "TreasureHunt",
    TeamItemCheck = "TeamItemCheck",
    RoleCreateVideo = "RoleCreateVideo",
    TreasureHuntPray = "TreasureHuntPray",
    BicycleEnergy = "BicycleEnergy",
    PetSkillResult = "PetSkillResult",
    FightJYTips = "FightJYTips",
    FightXYTips = "FightXYTips",
    RoleSkillTrainAutoLearn = "RoleSkillTrainAutoLearn",
    RoleProfSkillAutoLearn = "RoleProfSkillAutoLearn",
    DownloadNetChek = "DownloadNetChek",
    RewardFundExchangeChek = "RewardFundExchangeChek",
    TradeRoadMarketNext = "TradeRoadMarketNext",
    LoversMatchTips = "LoversMatchTips",
    QuickBattle = "QuickBattle",
    HarmonyFight = "HarmonyFight",
    LeaderChangeSpeed = "LeaderChangeSpeed",
    CloudArena = "CloudArena",
    UniversalCardBuy = "UniversalCardBuy",
    YouMingSealCard = "YouMingSealCard",
}

-- 这个新的今日不再显示是角色绑定的，每天12点才会重新显示new_tnr
DTNewTodayNotRemind = {
    MysteryStoreFlush = "MysteryStoreFlush",
    MysteryStoreBuy = "MysteryStoreBuy",
    NotAutoExchangeSpirit = "NotAutoExchangeSpirit",
    GodTokenBuyTip = "GodTokenBuyTip",
    SameFlowerSelTip = "SameFlowerSelTip",
    YZWish = "YZWish",
    WorkshopNeveal = "WorkshopNeveal",
    ActCloseTips = "ActCloseTips",
    PetIntensifyDialog = "PetIntensifyDialog",
    GuildHof = "GuildHof",
    GuildBuilding = "GuildBuilding",
    Braving = "Braving",
    ExclusiveGift = "ExclusiveGift_",
    WeekGift = "WeekGift_",
    MayDay = "MayDay",
    MarbasBuild = "MarbasBuild",
    MarbasBatheCasting1 = "MarbasBatheCasting1",
    MarbasBatheCasting2 = "MarbasBatheCasting2",
    MarbasBatheCastingTen1 = "MarbasBatheCastingTen1",
    MarbasBatheCastingTen2 = "MarbasBatheCastingTen2",
    MarbasBatheCastingTen3 = "MarbasBatheCastingTen3",
    MarbasBatheCastingClear = "MarbasBatheCastingClear",
    MarbasBatheCastingClearAll = "MarbasBatheCastingClearAll",
    MarbasBatheCastingClearAll2 = "MarbasBatheCastingClearAll2",
    ShengQiStudySkill = "ShengQiStudySkill",
    PetXiLianTip = "PetXiLianTipKey",
    ShengQiStudySkillCancel = "ShengQiStudySkillCancel",
    HuanShouStudyResult = "HuanShouStudyResult",
    LingBaoStrAll = "LingBaoStrAll",
}

DialogTipsData = DialogTipsData or BaseClass()

function DialogTipsData:__init()
    self.cache_info_list = {}
    self.today_not_reminds = {}
    DialogTipsData.Instance = self
end

function DialogTipsData:__delete()
    DialogTipsData.Instance = nil
end

function DialogTipsData:AddInfo(info)
    table.insert(self.cache_info_list, info)
end

function DialogTipsData:NextInfo()
    if next(self.cache_info_list) then
        self.cur_info = table.remove(self.cache_info_list, #self.cache_info_list)
        return self.cur_info
    end
end

function DialogTipsData:InfoEmpty()
    if next(self.cache_info_list) then
        return false
    else
        return true
    end
end

function DialogTipsData:SetCurInfo(info)
    self.cur_info = info
end

function DialogTipsData:GetCurInfo()
    return self.cur_info
end

-- 将新今天的日期设置进去
function DialogTipsData:SetTodayRemind(key)
    local t0 = TimeManager.FormatUnixTime2Date(os.time())
    local str = t0.year .. "_" .. t0.month .. "_" .. t0.day
    UnityPlayerPrefs.SetString(PrefKeys.TodayRemind(key), str)
end

-- 获得新今日红点信息
function DialogTipsData:GetTodayRemind(key)
    if not key then
        local t0 = TimeManager.FormatUnixTime2Date(os.time())
        local str = t0.year .. "_" .. t0.month .. "_" .. t0.day
        return str
    end
    return UnityPlayerPrefs.GetString(PrefKeys.TodayRemind(key)) or ""
end

-- 新今日红点是否一致
function DialogTipsData:TodayIsTrue(key)
    return self:GetTodayRemind(key) == self:GetTodayRemind()
end

--获取修改今日不在提示状态
function DialogTipsData:GetTodayValue(remind_key)
    return self.today_not_reminds[remind_key]
end
--boolean
function DialogTipsData:SetTodayValue(remind_key, value)
    self.today_not_reminds[remind_key] = value
end