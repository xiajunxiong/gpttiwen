VitalityFlopData = VitalityFlopData or BaseClass()
function VitalityFlopData:__init()
    if VitalityFlopData.Instance ~= nil then
        Debuger.LogError("[VitalityFlopData] attempt to create singleton twice!")
        return
    end
    VitalityFlopData.Instance = self
    self.all_config = Config.yuanqifanpai_auto

    self.info = SmartData.New({
        times = {},
        state = 0,
        count = 0,
        exchange = false,
    })
    self.select_idx = 0
end
function VitalityFlopData:SetCardReward(item, notice)
    self.card_reward = item
    if true == notice then
        self.info.state = 1
    end
end
function VitalityFlopData:CardReward()
    return self.card_reward
end
function VitalityFlopData:__delete()
    
end
function VitalityFlopData:Info()
    return self.info
end
function VitalityFlopData:DayTime()
    return TimeCtrl.Instance:GetActOpenServerDay(ACTIVITY_TYPE.VITALITY_FLOP)
end
function VitalityFlopData:GetActReward()
	local day_num = self:DayTime()
	local cfg = self.all_config.reward
    local reward_list = {}
	for i,v in pairs(cfg) do
		if day_num >= v.section_start and (day_num <= v.section_end or v.section_end == 0) then
			reward_list[v.seq] = v
	    end
	end
    return reward_list
end
function VitalityFlopData:RamdonList()
    local reward_list = self:GetActReward()
    local list = DataHelper.GetRamdonList(1,15)
    local re_list = {}
    if table.nums(reward_list) >= 15 then
        for i, v in ipairs(list) do
            re_list[i] = reward_list[v]
            re_list[i].fuc_seq = i
        end
    end
    return re_list
end
function VitalityFlopData:PreviewReward()
    local config = self:GetActReward()
    local preview_list = {}
    for i, v in ipairs(config) do
        if v.reward == 1 then
            table.insert(preview_list, v)
        end
    end
    return preview_list
end
function VitalityFlopData:CountReward()
    local day_num = self:DayTime()
    local config = self.all_config.reward_frequency
    local reward_list = {}
	for i,v in pairs(config) do
		if day_num >= v.section_start and (day_num <= v.section_end or v.section_end == 0) then
			reward_list[v.seq] = v
	    end
	end
    return reward_list
end
function VitalityFlopData:ExchangeList()
    return self.all_config.gather    
end

function VitalityFlopData:GetActTime()
    local act_type = ACTIVITY_TYPE.VITALITY_FLOP
    local start = ActivityData.GetStartStampTime(act_type)
    local end_time = ActivityData.GetSwitchStampTime(act_type)
    end_time = end_time - 1
    if start == 0 and end_time == 0 then
        LogError("活动未开启或者已结束")
        return Language.Butterfly.Close
    else
        end_time = end_time <= -1 and 0 or end_time
        local s = os.date("*t",start)
        local e = os.date("*t",end_time)
        return string.format(Language.Butterfly.Time, s.month, s.day, e.month, e.day)
    end
end
function VitalityFlopData:ExItem()
    return Item.Create(self.all_config.other[1].reward2_item)
end
function VitalityFlopData:IsGet(seq)
    return self.info.times[seq] == 1
end
function VitalityFlopData:CanGet(count)
    return self.info.count >= count
end
function VitalityFlopData:GetRemindCount()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.VITALITY_FLOP) then
        return 0
    end
    if self:CanClickCard() then
        return 1
    end
    if self:CanGetCountReward() then
        return 1
    end
    if self:CanExchange() then
        return 1
    end
    return 0
end
function VitalityFlopData:CanClickCard()
    if self:Info().exchange then
        return false
    end
    local item = self:ExItem()
    return Item.GetNum(item.item_id) > 0
end
function VitalityFlopData:CanGetCountReward()
    local list = self:CountReward()
    for i, v in ipairs(list) do
        if not self:IsGet(v.seq) and self:CanGet(v.task) then
            return true
        end
    end
    return false
end
function VitalityFlopData:CanExchange()
    if self:Info().exchange then
        return false
    end
    local list = self:ExchangeList()
    for k, v in ipairs(list) do
        if self:CalcCount(v) then
            return true
        end
    end
    return false
end
function VitalityFlopData:CalcCount(info)
    local temp
    for i = 1, 4 do
        temp = info["item"..i]
        if next(temp) then
            if Item.GetNum(temp.item_id) < temp.num then
                return false
            end
        end
    end
    return true
end