GuildHofData = GuildHofData or BaseClass()

-- 名人堂操作请求类型
GuildHofReqType = {
    RankInfo = 0,   --请求排行榜下发，param1：排行榜类型
    Bless = 1,      --祈福
    Reward = 2,     --领取祈福奖励
    Info = 3,       --请求下发
}

GuildHofRankType = {
    Battle = 0,     --战神榜
    Rich = 1,       --富豪榜
    Kind = 2,       --善人榜
}

function GuildHofData:__init()
    if GuildHofData.Instance ~= nil then
        Debuger.LogError("[GuildHofData] attempt to create singleton twice!")
        return
    end
    GuildHofData.Instance = self
    self.config = Config.family_auto
    self.other = self.config.other[1]
    self.info_data = SmartData.New()
    self.remind_data = SmartData.New()
    self:ResetData()
    Cfg.Register("GuildHofAttr", Cfg.RealTimeSearch(self.config.bless_reward, "world_level"))
end

function GuildHofData:__delete()
    GuildHofData.Instance = nil
end

function GuildHofData:ResetData(not_clear_info)
    local get_rank_list = function(rank_type)
        local t = {}
        t.rank_type = rank_type
        t.appearance = {}
        t.rank_count = 0
        t.rank_list = {}
        for i = 1, self.other.rank_amount do
            t.rank_list[i] = self:GetOneRankData(i)
        end
        return t
    end
    local rank_list = {}
    for k, v in pairs(GuildHofRankType) do
        rank_list[v] = get_rank_list(v)
    end
    if not_clear_info ~= true then
        self.info_data:Set({
            info = {
                is_open = 0,
                bless_value = 0,
                today_bless_times = 0,
                is_get_bless_attr = 0,
            },
            rank_list = rank_list,
        })
    end
    self.remind_data:Notify()
end

function GuildHofData:GetOneRankData(i)
    return {index = i}
end

-- 祈福最大次数
function GuildHofData:MaxBlessCount()
    if not self.max_bless_count then
        self.max_bless_count = 0
        for k, v in pairs(self.config.bless_base) do
            self.max_bless_count = self.max_bless_count + 1
        end
    end
    return self.max_bless_count
end

function GuildHofData:IsBlessMax()
    return self.info_data.info.today_bless_times + 1 > self:MaxBlessCount()
end

function GuildHofData:IsOpen()
    return self.info_data.info.is_open > 0
end

function GuildHofData:AllRemind()
    if self:IsOpen() then
        -- 检测每日红点
        if self:TodayRemind() > 0 then
            return 1
        else
            -- 检测是否可领取属性
            return self:IsCanGetAttr() and 1 or 0
        end
    end
    return 0
end

function GuildHofData:TodayRemind()
    if self:IsOpen() then
        return (not DialogTipsData.Instance:TodayIsTrue(DTNewTodayNotRemind.GuildHof)) and 1 or 0
    end
    return 0
end

function GuildHofData:SetTodayRemind()
    if self:IsOpen() then
        DialogTipsData.Instance:SetTodayRemind(DTNewTodayNotRemind.GuildHof)
        self.remind_data:Notify()
    end
end

function GuildHofData:IsBlessFull()
    return self.info_data.info.bless_value >= self.other.bless_all_add
end

-- 获取祈福进度，0~1（是否0~100）
function GuildHofData:GetPercent(is_hundred)
    local percent = self.info_data.info.bless_value / self.other.bless_all_add * 100
    percent = math.floor(percent)
    if percent > 100 then
        percent = 100
    end
    return is_hundred and percent or percent / 100
end

function GuildHofData:IsCanGetAttr()
    return self:IsBlessFull() and not self:IsGetAttr()
end

function GuildHofData:IsGetAttr()
    return self.info_data.info.is_get_bless_attr == 1
end

function GuildHofData:SetRankListInfo(protocol)
    local need_count = self.other.rank_amount - protocol.info.rank_count
    -- 补上空的rank_list
    if need_count > 0 then
        for i = 1, need_count do
            table.insert(protocol.info.rank_list, self:GetOneRankData(protocol.info.rank_count + i))
        end
    end
    self.info_data.rank_list[protocol.info.rank_type] = protocol.info
    self.info_data:SetDirty("rank_list")
end

function GuildHofData:SetBlessingInfo(protocol)
    self.info_data.info = protocol.info
    self.remind_data:Notify()
end

function GuildHofData:GetRankList(rank_type)
    return self.info_data.rank_list[rank_type]
end

function GuildHofData:GetSingleRank()
    return self.other.rank_type
end

function GuildHofData:GetOtherNumber()
    return self.other.number
end

function GuildHofData:GetGuildInfoShow()
    local t = {}
    local rank_info = self:GetRankList(self:GetSingleRank())
    for i = 1, self.other.number do
        table.insert(t, rank_info.rank_list[i])
    end
    return Language.GuildHof.RankType[self:GetSingleRank()], t
end

function GuildHofData:GetBlessAttr()
    local world_level = RoleData.Instance:GetWorldLevel()
    local attr_config = Cfg.GuildHofAttr(world_level)
    local t = {}
    if not TableIsEmpty(attr_config) then
        local index = 1
        while attr_config["attr_add_"..index] and attr_config["attr_add_"..index] > 0 do
            table.insert(t, CommonStruct.AttrStruct(attr_config["attr_type_"..index], attr_config["attr_add_"..index]))
            index = index + 1
        end
    end
    return t
end

-- 根据当前祈福次数返回本次祈福时的信息
function GuildHofData:GetBlessConfig()
    for k, v in pairs(self.config.bless_base) do
        if (self.info_data.info.today_bless_times + 1) == v.bless_list then
            return v
        end
    end
end