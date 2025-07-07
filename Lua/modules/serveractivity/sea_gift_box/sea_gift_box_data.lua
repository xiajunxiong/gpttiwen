SeaGiftBoxData = SeaGiftBoxData or BaseClass()
function SeaGiftBoxData:__init()
    if SeaGiftBoxData.Instance ~= nil then
        Debuger.LogError("[SeaGiftBoxData] attempt to create singleton twice!")
        return
    end
    SeaGiftBoxData.Instance = self

    self.gift_box_data = SmartData.New({day_buy_times = {}, buy_times = {}})
end

function SeaGiftBoxData:__delete()
    if SeaGiftBoxData.Instance == self then
        SeaGiftBoxData.Instance = nil
    end
end

function SeaGiftBoxData:Config()
    return Config.shanhai_box_auto
end
function SeaGiftBoxData:OtherConfig()
    return self:Config().other[1]
end

----------限时宝盒
function SeaGiftBoxData:SetGiftBoxInfo(protocol)
    self.gift_box_data.day_buy_times = protocol.day_buy_times
    self.gift_box_data.buy_times = protocol.buy_times
    self.gift_box_data.has_free_reward = protocol.has_free_reward
end

function SeaGiftBoxData:GetGiftBoxDayBuyTimes(seq)
    return self.gift_box_data.day_buy_times[seq] or 0
end

function SeaGiftBoxData:GetGiftBoxBuyTimes(seq)
    return self.gift_box_data.buy_times[seq] or 0
end

function SeaGiftBoxData:IsGiftBoxBuyHaveTimes(cfg)
    if cfg ~= nil then
        if cfg.limit_type == 1 then
            local buy_times = self:GetGiftBoxBuyTimes(cfg.seq)
            return buy_times < cfg.buy_times
        elseif cfg.limit_type == 2 then
            local buy_times = self:GetGiftBoxDayBuyTimes(cfg.seq)
            return buy_times < cfg.today_times
        end
    end
    return false
end

function SeaGiftBoxData:GetGiftBoxItemList()
    local act_cfg = self:Config().gift_configure
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SEA_GIFT_BOX, act_cfg, "seq")
end

function SeaGiftBoxData:GetGiftBoxShowList()
    local show_list = self:GetGiftBoxItemList()
    local function func_sort(data)
        if not self:IsGiftBoxBuyHaveTimes(data) then
            return 100
        end
    end
    table.sort(show_list, DataHelper.WeightSort("seq", func_sort))
    --新增逻辑，front首位推荐
    local front, front_index = 0, 0
    for i, v in ipairs(show_list) do
        if v.front ~= 0 and self:IsGiftBoxBuyHaveTimes(v) and (front == 0 or front > v.front) then
            front = v.front
            front_index = i
        end
    end
    if front ~= 0 then
        table.insert(show_list, 1, table.remove(show_list, front_index))
    end
    return show_list
end

function SeaGiftBoxData:GetRemindNum()
    return self.gift_box_data.has_free_reward == 0 and 1 or 0
end

function SeaGiftBoxData:IsFreeGiftFetched()
    return self.gift_box_data.has_free_reward == 1
end

function SeaGiftBoxData:GetFreeReward()
    return self:OtherConfig().free_reward_item
end
