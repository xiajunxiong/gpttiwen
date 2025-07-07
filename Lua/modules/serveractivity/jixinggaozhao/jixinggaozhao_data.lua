ActJiXingGaoZhaoData = ActJiXingGaoZhaoData or BaseClass()

function ActJiXingGaoZhaoData:__init()
	if ActJiXingGaoZhaoData.Instance ~= nil then
		Debuger.LogError("[ActJiXingGaoZhaoData] attempt to create singleton twice!")
		return
	end
    ActJiXingGaoZhaoData.Instance = self
    self.jixing_smart = SmartData.New({draw_result = -1, storeFlush = false, flush = false, flush_pool_item = false, skip_rolling = false, flush_record = false})

    self.price_type_id = {
        [0] = HuoBi[CurrencyType.Gold],
        [3] = HuoBi[CurrencyType.RenMinBi],
        [16] = HuoBi[CurrencyType.ImmortalCoin],
    }
end


function ActJiXingGaoZhaoData:__delete()
	ActJiXingGaoZhaoData.Instance = nil
end

function ActJiXingGaoZhaoData:SetJiXingInfo(protocol)
    self.draw_times = protocol.draw_times
    self.draw_reward_fetch_mark = protocol.draw_reward_fetch_mark
    self.shop_list = protocol.shop_list

    self.total_info = TableCopy(protocol)
    if ViewMgr:IsOpen(ActJiXingGaoZhaoStoreView) then
        self.jixing_smart:SetDirty("storeFlush")
    end
    if ViewMgr:IsOpen(ActJiXingGaoZhaoView) then
        self.jixing_smart:SetDirty("flush")
    end
end

function ActJiXingGaoZhaoData:SetPoolInfo(protocol)
    self.pool_item_id = protocol.item_id
    self.pool_item_num = protocol.num
    if ViewMgr:IsOpen(ActJiXingGaoZhaoView) then
        self.jixing_smart:SetDirty("flush_pool_item")
    end
end

function ActJiXingGaoZhaoData:SetDrawResult(protocol)
    local section_step = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_JIXINGGAOZHAO, Config.jixinggaozhao_auto.basis_configuration)
    for k,v in pairs(section_step) do
        if v.seq == protocol.seq then
            if v.reward_type == 2 then
                self.draw_result_item = Item.Init(self.pool_item_id, protocol.num)
            elseif v.reward_type == 0 then
                self.draw_result_item = Item.Create(v.reward_item)
            else
                self.draw_result_item = Item.Init(HuoBi[CurrencyType.Gold], v.gold_num)
                ChatCtrl.Instance:ChannelChatCustom {item_get = {item = Item.Init(HuoBi[CurrencyType.Gold]), num = v.gold_num}}
            end
            
            if self.jixing_smart.draw_result == k then
                self.jixing_smart:SetDirty("draw_result")
            else
                self.jixing_smart.draw_result = k
            end
            break
        end
    end
    ActJiXingGaoZhaoCtrl.Instance:SendJiXingGaoZhaoReq(ActJiXingGaoZhaoCtrl.REQ_TYPE.REQ_INFO)
end

--from 1507
function ActJiXingGaoZhaoData:SetGetReward(protocol)
    if protocol.param1 > 0 then
        self.reward_list = {{item_id = HuoBi[CurrencyType.Gold], num = protocol.param1}}
    else
        self.reward_list = protocol.reward_list
    end
end

function ActJiXingGaoZhaoData:SetRecordList(protocol)
    self.record_list = protocol.record_list
end

function ActJiXingGaoZhaoData:AddRecordInfo(protocol)
    self.record_list = self.record_list or {}
    self.record_list[#self.record_list+1] = protocol.record_info
    if #self.record_list > 10 then
        table.remove(self.record_list,1)
    end
    self.jixing_smart:SetDirty("flush_record")
end

function ActJiXingGaoZhaoData:GetRecordList()
    return self.record_list or {}
end

function ActJiXingGaoZhaoData:GetDrawProgress()
    local section_step = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_JIXINGGAOZHAO, Config.jixinggaozhao_auto.phase_reward)
    local progress = 0
    if self.draw_times <= 0 then
        return 0
    end
    local per_section_progress = tonumber(string.format("%0.3f", 1 / #section_step))
    for i, v in ipairs(section_step) do
        if self.draw_times >= v.lucky_draw_num then
            progress = tonumber(string.format("%0.3f", i / #section_step))

        elseif i > 1 and self.draw_times < v.lucky_draw_num then
            progress = progress + (self.draw_times - section_step[i - 1].lucky_draw_num) / (section_step[i].lucky_draw_num - section_step[i - 1].lucky_draw_num) * per_section_progress

            break
        elseif i == 1 and self.draw_times < v.lucky_draw_num then
            progress = (self.draw_times / v.lucky_draw_num) * per_section_progress
            break
        end
    end
    return progress > 1 and 1 or progress
end

function ActJiXingGaoZhaoData:GetStarCount()
    return Item.GetNum(Config.jixinggaozhao_auto.other[1].item_id)
end

function ActJiXingGaoZhaoData:GetRollingList()
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_JIXINGGAOZHAO, Config.jixinggaozhao_auto.basis_configuration)
end

function ActJiXingGaoZhaoData:GetBoxInfoList()
    local box_list = {}
    for i, v in ipairs(ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_JIXINGGAOZHAO, Config.jixinggaozhao_auto.phase_reward)) do
        local t = {}--TableCopy(v)
        t.seq = v.seq
        t.type = v.type
        t.section_start = v.section_start
        t.section_end = v.section_end
        t.reward_item = v.reward_item
        t.lucky_draw_num = v.lucky_draw_num
        t.flag = self.draw_reward_fetch_mark[32 - v.seq]
        t.can_get = self.draw_times >= v.lucky_draw_num
        t.item_list = {}
        for _, value in pairs(t.reward_item) do
            table.insert(t.item_list, Item.Create(value))
        end
        table.insert(box_list, t)
    end
    return box_list
end

function ActJiXingGaoZhaoData:GetStoreInfoList()
    local store_info_list = {}
    for _, v in pairs(ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_JIXINGGAOZHAO, Config.jixinggaozhao_auto.buy_configuration)) do
        local t = {}--TableCopy(v)
        t.section_start = v.section_start
        t.section_end = v.section_end
        t.limit_type = v.limit_type
        t.type = v.type
        t.seq = v.seq
        t.price_type = v.price_type
        t.buy_money = v.buy_money
        t.gift_name = v.gift_name
        t.reward_item = v.reward_item
        t.buy_times = v.buy_times
        t.buy_count = self.shop_list[t.seq + 1] or 0
        t.buy_max = t.buy_count >= t.buy_times
        t.item_list = {}
        for _, value in pairs(v.reward_item) do
            table.insert(t.item_list, Item.Create(value))
        end
        table.insert(store_info_list,t)
    end
    table.sort(store_info_list, function (a,b)
        if a.buy_max == b.buy_max then
            return a.seq < b.seq
        else
            return a.buy_max == false
        end
    end)
    return store_info_list
end