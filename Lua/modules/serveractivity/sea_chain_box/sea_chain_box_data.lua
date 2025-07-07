SeaChainBoxData = SeaChainBoxData or BaseClass()
function SeaChainBoxData:__init()
    if SeaChainBoxData.Instance ~= nil then
        Debuger.LogError("[SeaChainBoxData] attempt to create singleton twice!")
        return
    end
    SeaChainBoxData.Instance = self

    self.chain_box_data = SmartData.New({can_buy_seq = bit:d2b(0), day_buy_times = {}, buy_times = {}})
end

function SeaChainBoxData:__delete()
    if SeaChainBoxData.Instance == self then
        SeaChainBoxData.Instance = nil
    end
end

function SeaChainBoxData:Config()
    return Config.shanhai_chain_box_auto
end
function SeaChainBoxData:OtherConfig()
    return self:Config().other[1]
end

function SeaChainBoxData:SetChainBoxInfo(protocol)
    self.chain_box_data.can_buy_seq = protocol.can_buy_seq
    self.chain_box_data.day_buy_times = protocol.day_buy_times
    self.chain_box_data.buy_times = protocol.buy_times
    self.chain_box_data.has_free_reward = protocol.has_free_reward
end
--档位是否开启
function SeaChainBoxData:GetChainBoxBuyIsOpen(seq)
    if seq == 0 then
        return true
    end
    return self.chain_box_data.can_buy_seq[32 - seq + 1] == 1
end

function SeaChainBoxData:GetChainBoxDayBuyTimes(seq)
    return self.chain_box_data.day_buy_times[seq] or 0
end
function SeaChainBoxData:GetChainBoxBuyTimes(seq)
    return self.chain_box_data.buy_times[seq] or 0
end

--判断是否还有购买次数
function SeaChainBoxData:IsChainBoxBuyHaveTimes(cfg)
    if cfg ~= nil then
        if cfg.limit_type == 1 then
            local buy_times = self:GetChainBoxBuyTimes(cfg.seq)
            return buy_times < cfg.buy_times
        elseif cfg.limit_type == 2 then
            local buy_times = self:GetChainBoxDayBuyTimes(cfg.seq)
            return buy_times < cfg.today_times
        end
    end
    return false
end

function SeaChainBoxData:GetChainBoxItemList()
    local act_cfg = self:Config().gift_configure
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SEA_CHAIN_BOX, act_cfg, "seq")
end

function SeaChainBoxData:GetChainBoxShowList()
    local show_list = self:GetChainBoxItemList()
    local function func_sort(data)
        if not self:IsChainBoxBuyHaveTimes(data) then
            return 100
        end
    end
    table.sort(show_list, DataHelper.WeightSort("seq", func_sort))
    return show_list
end

function SeaChainBoxData:GetRemindNum()
    return self.chain_box_data.has_free_reward == 0 and 1 or 0
end

function SeaChainBoxData:IsFreeGiftFetched()
    return self.chain_box_data.has_free_reward == 1
end

function SeaChainBoxData:GetFreeReward()
    return self:OtherConfig().free_reward_item
end
