GuildBuildingData = GuildBuildingData or BaseClass()

-- 内政建设操作请求类型
GuildBuildingReqType = {
    Up = 1,      --建造升级
    Down = 2,    --拆除降级
    SpeedUp = 3, --建筑加速
}

-- 内政建设建筑类型
GuildBuildingType = {
    Hall = 1,     --大厅
    Book = 2,     --书房
    Wing = 3,     --厢房
    Gold = 4,     --金库
    Shop = 5,     --商铺
}

-- 家族建筑的通知类型
GuildBuildingNotifyType = {
    UseItem = 1,            --使用物品
    StartSpeed = 2,         --开启加速
    AddContribution = 3,    --增加贡献
    SpeedUp = 4,            --成员加速
    SpeedUpFinish = 5,      --加速建设完成
}

function GuildBuildingData:__init()
    if GuildBuildingData.Instance ~= nil then
        Debuger.LogError("[GuildBuildingData] attempt to create singleton twice!")
        return
    end
    GuildBuildingData.Instance = self
    self.config = Config.family_auto
    self.other = self.config.other[1]
    self.info_data = SmartData.New()
    -- 这里面有存放哪个建筑有红点
    self.remind_data = SmartData.New()
    self.select_building_data = SmartData.New()
    -- 是否正在播放特效
    self.is_playing_effect = false
    self:ResetData()
    Cfg.Register("GuildBuildingConfig", Cfg.RealTimeSearch2Key(self.config.build_up, "build_type", "build_level"))
end

function GuildBuildingData:__delete()
    GuildBuildingData.Instance = nil
end

function GuildBuildingData:Decode()
    return self:NewBuilding(
        MsgAdapter.ReadShort(),
        MsgAdapter.ReadShort()
    )
end

function GuildBuildingData:NewBuilding(build_type, build_level)
    local info = {}
    info.build_type = build_type or GuildBuildingType.Hall
    info.build_level = build_level or 0
    info.finish_time = 0
    return info
end

function GuildBuildingData:ResetData()
    self:ResetRemind()
    local info = {}
    for i = 1, GUILD_BUILDING_MAX do
        info[i] = self:NewBuilding(i)
    end
    self.info_data:Set({
        info = info,
        speed_info = {today_up_count = 0, is_show_speed = 0, is_helped_count = 0},
        now_building_type = 0,
        finish_time = 0,
        building_count = 0,
        open_info = {open_post = -1, open_name = ""},
    })
    self.select_building_data:Set({index = 1, info = {}})
end

-- 返回今日家族剩余可加速次数
function GuildBuildingData:TodayCanSpeedTimes()
    local num = self.other.speed_times - self.info_data.speed_info.today_up_count
    return num >= 0 and num or 0
end

-- 返回今日家族剩余可建造次数
function GuildBuildingData:TodayCanBuildTimes()
    local num = self.other.build_times - self.info_data.building_count
    return num >= 0 and num or 0
end

-- 返回今日个人剩余可加速次数
function GuildBuildingData:TodayCanSpeedTimesPerson()
    local num = self.other.person_times - self.info_data.speed_info.is_helped_count
    return num >= 0 and num or 0
end

-- 判断是否正在开启加速建造中
function GuildBuildingData:IsSpeedUping()
    return self.info_data.speed_info.is_show_speed > 0
end

-- 当前建造的建筑类型和时间
function GuildBuildingData:NowBuildingData(now_building_type, finish_time, building_count)
    if now_building_type then
        for k, v in pairs(self.info_data.info) do
            v.finish_time = 0
        end
        self.info_data.now_building_type = 0
        self.info_data.finish_time = 0
        self.info_data.building_count = building_count
        if now_building_type > 0 then
            self.info_data.now_building_type = now_building_type
            self.info_data.finish_time = finish_time
            self.info_data.info[now_building_type].finish_time = finish_time
        end
        self.info_data:SetDirty("info")
    else
        return self.info_data.now_building_type, self.info_data.finish_time
    end
end

function GuildBuildingData:BuildingIsOpen()
    return FunOpen.Instance:GetFunIsOpened(Mod.Guild.GuildBuilding)
end

function GuildBuildingData:ResetAllNowFinishBuilding()
    local t = {}
    for k, v in pairs(GuildBuildingType) do
        t[v] = 0
    end
    self.remind_data.info = t
end

function GuildBuildingData:NowFinishBuilding(build_type, build_level)
    if build_level then
        self.remind_data.info[build_type] = build_level
    else
        return self.remind_data.info[build_type]
    end
end

-- 今日红点
function GuildBuildingData:TodayRemind()
    if not self:BuildingIsOpen() then
        return 0
    end
    -- 有权限并且有加速次数时需要今日红点
    if self:TodayCanSpeedTimes() > 0 and self:BuildPermission() then
        return (not DialogTipsData.Instance:TodayIsTrue(DTNewTodayNotRemind.GuildBuilding)) and 1 or 0
    end
    return 0
end

function GuildBuildingData:SetTodayRemind()
    if self:TodayCanSpeedTimes() > 0 and self:BuildPermission() then
        DialogTipsData.Instance:SetTodayRemind(DTNewTodayNotRemind.GuildBuilding)
        self.remind_data:Notify()
    end
end

function GuildBuildingData:ResetRemind()
    self:IsPlayingEffect(false)
    self:ResetAllNowFinishBuilding()
    self.remind_data:Notify()
end

function GuildBuildingData:SelectBuildingData(index, info)
    self.select_building_data.index = index or 1
    self.select_building_data.info = info or {}
    self.select_building_data:Notify()
end

function GuildBuildingData:IsPlayingEffect(value)
    if value ~= nil then
        self.is_playing_effect = value
    else
        return self.is_playing_effect
    end
end

function GuildBuildingData:AllRemind()
    if not self:BuildingIsOpen() then
        return 0
    end
    if not TableIsEmpty(self.remind_data.info) then
        for k, v in pairs(self.remind_data.info) do
            if v > 0 then
                return 1
            end
        end
    end
    if GuildData.Instance:GetGuildID() > 0 then
        return self:TodayRemind()
    end
    return 0
end

function GuildBuildingData:BuildingConfig(build_type, build_level)
    build_level = build_level or self.info_data.info[build_type].build_level
    return Cfg.GuildBuildingConfig(build_type, build_level, false)
end

-- 返回建筑名称，是否加等级，0级不加
function GuildBuildingData:BuildingName(build_type, add_level)
    for k, v in pairs(self.config.build_name) do
        if v.build_type == build_type then
            if add_level and self.info_data.info[build_type].build_level > 0 then
                return Format(Language.GuildBuilding.BuildNameLevel, v.build_name, self.info_data.info[build_type].build_level)
            end
            return v.build_name
        end
    end
    LogError("不存在build_type："..build_type)
    return ""
end

-- 家族建筑每日所需要的维护资金
function GuildBuildingData:BuildingServeMoney()
    local money = 0
    for k, v in pairs(self.info_data.info) do
        money = money + v.build_level * self.other.serve_build_capital
    end
    return money
end

-- 家族建筑所增加的人口上限数
function GuildBuildingData:AddPersonMax()
    local person = 0
    local config = self:BuildingConfig(GuildBuildingType.Wing)
    if not TableIsEmpty(config) then
        person = person + config.paramet_1
    end
    return person
end

-- 当前家族资金上限
function GuildBuildingData:FullMoney()
    local config = self:BuildingConfig(GuildBuildingType.Gold)
    if not TableIsEmpty(config) then
        return config.paramet_1
    end
    return 0
end

-- 当前家族商店道具价格系数
function GuildBuildingData:ShopConst()
    local config = self:BuildingConfig(GuildBuildingType.Shop)
    if not TableIsEmpty(config) then
        return config.paramet_1
    end
    return 1
end

-- 获取当前家族限制的生活等级
function GuildBuildingData:LiveLimitLevel()
    local config = self:BuildingConfig(GuildBuildingType.Book)
    if not TableIsEmpty(config) then
        return config.paramet_1
    end
    return 1
end

-- 是否有建筑操作权限
function GuildBuildingData:BuildPermission()
    local config = GuildData.Instance:GetJobInfo()
    if not TableIsEmpty(config) then
        return config.build_limit > 0
    end
    return false
end

-- 返回家族建材道具id
function GuildBuildingData:BuildItemId()
    return self.other.capital_item
end

-- 返回家族消耗铜币
function GuildBuildingData:SpeedUpCs()
    return self.other.expend_coin
end

-- 返回家族建材道具详情
function GuildBuildingData:BuildItemCfg()
    return Item.Config(self:BuildItemId())
end

function GuildBuildingData:SetOneBuildingInfo(protocol)
    self.info_data.info[protocol.info.build_type] = protocol.info
    self.info_data:Notify()
    -- 手动请求一遍家族信息
    if protocol.info.finish_time > 0 then
        GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_BASE_INFO})
    end
end

function GuildBuildingData:SetAllBuildingInfo(protocol)
    self.info_data.info = protocol.info
    self:NowBuildingData(protocol.now_building_type, protocol.finish_time, protocol.building_count)
end

--单建筑建造完会下发一遍
function GuildBuildingData:SetNowBuildFinish(protocol)
    if self:BuildPermission() then
        self:NowFinishBuilding(protocol.info.build_type, protocol.info.build_level)
        self:IsPlayingEffect(true)
    else
        self:ResetAllNowFinishBuilding()
        self:IsPlayingEffect(false)
    end
    self.remind_data:Notify()
end

function GuildBuildingData:SetSpeedInfo(protocol)
    self.info_data.speed_info = protocol.info
    if self.info_data.speed_info.is_show_speed == 0 then
        self.info_data.open_info = {open_post = -1, open_name = ""}
    end
end

function GuildBuildingData:SetOpenSpeedInfo(protocol)
    self.info_data.open_info = protocol.info
end