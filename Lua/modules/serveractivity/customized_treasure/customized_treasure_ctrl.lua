VIEW_REQ("modules/serveractivity/customized_treasure/customized_treasure_view")

CustomizedTreasureCtrl = CustomizedTreasureCtrl or BaseClass(BaseCtrl)
 
function CustomizedTreasureCtrl:__init()
    self.data = CustomizedTreasureData.New()
    self:RegisterProtocol(SCRADingZhiBaoXiangInfo,"OnCustomizedTreasureInfo")
end

function CustomizedTreasureCtrl:__delete()
    self:UnRegisterProtocol(SCRADingZhiBaoXiangInfo)
end

function CustomizedTreasureCtrl:OnCustomizedTreasureInfo(protocol)
	self.data:OnProtocolData(protocol)
	ServerActivityData.Instance:SetBonusNum(ACTIVITY_TYPE.RAND_CUSTOMIZED_TREASURE,protocol.flag_num,#self.data:GetTreasureList())
end

CustomizedTreasureData = CustomizedTreasureData or BaseClass()

function CustomizedTreasureData:__init()
	if CustomizedTreasureData.Instance ~= nil then
		Debuger.LogError("[CustomizedTreasureData] attempt to create singleton twice!")
		return
	end	
    CustomizedTreasureData.Instance = self
    self.reward_type = SmartData.New({seq = -1})
	self.show_select = SmartData.New({flag = false})
	self.reward_select_change = SmartData.New({flag = false})
	self.buy_count_change = SmartData.New({flag = false})
	self.reward_data_list = {}
    self.gift_timer = nil
	self.reward_seq = 0
end

function CustomizedTreasureData:__delete()
    if self.gift_timer ~= nil then
        TimeHelper:CancelTimer(self.gift_timer)
        self.gift_timer = nil
    end
end


function CustomizedTreasureData:SelectShowFlagData()
    return self.show_select_data
end

function CustomizedTreasureData:OnProtocolData(protocol)
	self.buy_day_times = protocol.buy_day_times
	self.buy_sum_times = protocol.buy_sum_times
	self.buy_count_change.flag = not self.buy_count_change.flag
	self.buy_count_change.has_free_reward = protocol.has_free_reward
end


function CustomizedTreasureData:ClearRewardDataList2()
	self.reward_data_list = {}
	local box_configure = self:GetTreasureList()
	local reward_num  = 1
	for k,v in pairs(box_configure) do 
		if v.seq == self.reward_type.seq then
			reward_num = v.reward_num
			break
		end
	end
	for i=1,reward_num do
		local item_data = {}
		item_data.item_id = 0
		item_data.num = 0
		item_data.index = i
		item_data.type = 0
		table.insert(self.reward_data_list,item_data)
	end
	return self.reward_data_list
end

function CustomizedTreasureData:GetRewardDataList()
	return self.reward_data_list
end

function CustomizedTreasureData:SetRewardDataList(seq,item_data)
	for i=1,#self.reward_data_list do
		if seq == i then
			self.reward_data_list[i] = item_data
		end
	end
	self.reward_select_change.flag = not self.reward_select_change.flag
end

function CustomizedTreasureData:GetTreasureList()
	local box_configure = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_CUSTOMIZED_TREASURE,Config.ustomized_box_auto.box_configure,"seq")
    return box_configure
end

function CustomizedTreasureData:SortTreasureList()
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

function CustomizedTreasureData:GetTreasureCanBuyCount(seq)
	local box_configure = self:GetTreasureList()
	local all_count = 0
	for k,v in pairs(box_configure) do 
		if v.seq == seq then
			if v.limit_type == 1 then
				all_count = v.buy_times
			else
				all_count = v.today_times
			end
			break
		end
	end
	return all_count
end

function CustomizedTreasureData:GetTreasureBuyCount(seq)
	local box_configure = self:GetTreasureList()
	local buy_count = 0
	for k,v in pairs(box_configure) do 
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

function CustomizedTreasureData:GetTreasureHaveCount(seq)
	local have_count = 0
	local buy_count = self:GetTreasureBuyCount(seq)
	local all_count = self:GetTreasureCanBuyCount(seq)
	have_count  = all_count - buy_count
	return have_count
end

function CustomizedTreasureData:GetSelectDataList(group_id)
	local show_list = {}
	for i, v in ipairs(Config.ustomized_box_auto.reward_pool) do
		if v.item_id == group_id then --迷一样的命名
			table.insert(show_list, v)
		end
	end
	table.sort(show_list, DataHelper.SortFunc("type"))
	return show_list
end

function CustomizedTreasureData:Config()
    return Config.ustomized_box_auto
end
function CustomizedTreasureData:OtherConfig()
    return self:Config().other[1]
end

function CustomizedTreasureData:GetRemindNum()
    return self.buy_count_change.has_free_reward == 0 and 1 or 0
end

function CustomizedTreasureData:IsFreeGiftFetched()
    return self.buy_count_change.has_free_reward == 1
end

function CustomizedTreasureData:GetFreeReward()
    return self:OtherConfig().free_reward_item
end

function CustomizedTreasureData:GetShowStyle()
	for i, v in ipairs(Config.ustomized_box_auto.activity_interface) do
		if v.is_show == 1 then
			return v.interface
		end
	end
	return 0
end
