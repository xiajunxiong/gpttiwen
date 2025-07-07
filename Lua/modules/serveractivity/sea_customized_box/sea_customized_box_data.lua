SeaCustomizedBoxData = SeaCustomizedBoxData or BaseClass()
function SeaCustomizedBoxData:__init()
    if SeaCustomizedBoxData.Instance ~= nil then
        Debuger.LogError("[SeaCustomizedBoxData] attempt to create singleton twice!")
        return
    end
    SeaCustomizedBoxData.Instance = self

    self.reward_type = SmartData.New({seq = -1})
    self.show_select = SmartData.New({flag = false})
    self.reward_select_change = SmartData.New({flag = false})
    self.buy_count_change = SmartData.New({flag = false})
    self.reward_data_list = {}
    self.gift_timer = nil
    self.reward_seq = 0
end

function SeaCustomizedBoxData:__delete()
    if self.gift_timer ~= nil then
        TimeHelper:CancelTimer(self.gift_timer)
        self.gift_timer = nil
    end
end

function SeaCustomizedBoxData:Config()
    return Config.shanhai_ustomized_box_auto
end
function SeaCustomizedBoxData:OtherConfig()
    return self:Config().other[1]
end

function SeaCustomizedBoxData:SelectShowFlagData()
    return self.show_select_data
end

function SeaCustomizedBoxData:OnProtocolData(protocol)
    self.buy_day_times = protocol.buy_day_times
    self.buy_sum_times = protocol.buy_sum_times
    self.buy_count_change.flag = not self.buy_count_change.flag
    self.buy_count_change.has_free_reward = protocol.has_free_reward
end

function SeaCustomizedBoxData:GetTreasureList()
    local box_configure = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SEA_CUSTOMIZED_BOX, self:Config().box_configure, "seq")
    return box_configure
end

function SeaCustomizedBoxData:SortTreasureList()
    local show_list = self:GetTreasureList()
    local function func_sort(data)
        if self:GetTreasureHaveCount(data.seq) <= 0 then
            return 100
        end
    end
    table.sort(show_list, DataHelper.WeightSort("seq", func_sort))
	--新增逻辑，front首位推荐
	local front, front_index = 0, 0
	for i, v in ipairs(show_list) do
		if v.front ~= 0 and self:GetTreasureHaveCount(v.seq) > 0 and (front == 0 or front > v.front) then
			front = v.front
			front_index = i
		end
	end
	if front ~= 0 then
		table.insert(show_list, 1, table.remove(show_list, front_index))
	end
    return show_list
end

function SeaCustomizedBoxData:GetTreasureCanBuyCount(seq)
    for k, v in pairs(self:GetTreasureList()) do
        if v.seq == seq then
            return v.limit_type == 1 and v.buy_times or v.today_times
        end
    end
    return 0
end

function SeaCustomizedBoxData:GetTreasureBuyCount(seq)
    local buy_count = 0
    for k, v in pairs(self:GetTreasureList()) do
        if v.seq == seq then
            if v.limit_type == 1 then
                buy_count = self.buy_sum_times[seq]
            else
                buy_count = self.buy_day_times[seq]
            end
            break
        end
    end
    return buy_count
end

function SeaCustomizedBoxData:GetTreasureHaveCount(seq)
    local have_count = 0
    local buy_count = self:GetTreasureBuyCount(seq)
    local all_count = self:GetTreasureCanBuyCount(seq)
    have_count = all_count - buy_count
    return have_count
end

function SeaCustomizedBoxData:GetSelectDataList(group_id)
    local show_list = {}
    for i, v in ipairs(self:Config().reward_pool) do
        if v.item_id == group_id then --迷一样的命名
            table.insert(show_list, v)
        end
    end
    table.sort(show_list, DataHelper.SortFunc("type"))
    return show_list
end

function SeaCustomizedBoxData:GetRemindNum()
    return self.buy_count_change.has_free_reward == 0 and 1 or 0
end

function SeaCustomizedBoxData:IsFreeGiftFetched()
    return self.buy_count_change.has_free_reward == 1
end

function SeaCustomizedBoxData:GetFreeReward()
    return self:OtherConfig().free_reward_item
end
