LoversShopData = LoversShopData or BaseClass()
function LoversShopData:__init()
    if LoversShopData.Instance ~= nil then
        Debuger.LogError("[LoversShopData] attempt to create singleton twice!")
        return
    end
    LoversShopData.Instance = self

    self.act_info = SmartData.New({store_list = {}})
end

function LoversShopData:__delete()
    LoversShopData.Instance = nil
end

function LoversShopData:Config()
    return Config.lovers_market_auto
end
function LoversShopData:OtherConfig()
    return self:Config().other[1]
end

--货币物品id
function LoversShopData:GetCoinItemId()
    return self:OtherConfig().play_coin
end

function LoversShopData:GetShowList()
    local show_list = {}
    for i, v in pairs(self:Config().store_config) do
        if v.is_show then
            table.insert(show_list, v)
        end
    end
    table.sort(show_list, DataHelper.WeightSort("seq", BindTool.Bind1(self.GetSortFunction, self)))
    return show_list
end

function LoversShopData:GetSortFunction(data)
    if data.limit_convert_count > 0 and data.limit_convert_count - self:GetLimitTimes(data.seq) == 0 then
        return 10000
    end
    return 0
end

function LoversShopData:SetShopInfo(protocol)
    self.act_info.store_list = protocol.store_list
end

function LoversShopData:GetLimitTimes(seq)
    return self.act_info.store_list[seq] or 0
end

-----红点
function LoversShopData:GetRemindNum()
    return 0
end
