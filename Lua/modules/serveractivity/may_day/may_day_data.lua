
MayDayData = MayDayData or BaseClass()

function MayDayData:__init()
    if MayDayData.Instance ~= nil then
        Debuger.LogError("[MayDayData] attempt to create singleton twice!")
        return
    end
    MayDayData.Instance = self
    
    self:MayDayConfig()
    self:MayDayData()
end

function MayDayData:MayDayConfig()
    
end

function MayDayData:MayDayData()
    self.main_info = SmartData.New{
        is_fetch_day_reward = 0
    }
    self.main_sm = SmartData.New{
        is_flush = false
    }
    self.bun_come_sm = SmartData.New{
        state = MayDayConfig.BunComeState.Ready,
        num = 0,
        total = self:BunComeConfigOtherFrequency(),
        game = {init = true},
    }
    self.bun_come_info = SmartData.New{
        count = 0,
    }

    self.vitality_note_info = SmartData.New{init = true}
    self.main_btn_states = {}
end

function MayDayData:GetMayDayEnd()
	for k,v in pairs(Config.activity_main_auto.yuanqiqingdian) do
		if ActivityData.IsOpen(v.act_type) then
			return true
		end
	end
	if ViewMgr:IsOpen(MayDayView) then
		ViewMgr:CloseView(MayDayView)
	end
	return false
end

function MayDayData:GetMayDayShowList()
	local list, item = {}
    local open_times = {}
    for k, v in pairs(Config.activity_main_auto.yuanqiqingdian) do
        if ACTIVITY_TYPE.VITALITY_FLOP ~= v.act_type then
            local open_time, end_time = ActivityRandData.GetBaseDayOpenStampTime(v.act_type)
            if not ActivityData.IsOpen(v.act_type) and (open_time or 0) >= TimeHelper.GetServerTime() then
                open_times[v.plan_type] = open_times[v.plan_type] or open_time
                open_times[v.plan_type] = math.min(open_times[v.plan_type], open_time)
            end
        end
	end
	for k, v in pairs(Config.activity_main_auto.yuanqiqingdian) do
		v.mod = TabMod(v.mod_key)
        v.main_btn_state = self.main_btn_states[v.act_type] or MayDayConfig.MainBtnState.Go
        if MayDayConfig.MainBtnState.NextDay == v.main_btn_state then
            local activity_info = ActivityData.Instance:GetActivityStatusInfo(v.act_type)
            if activity_info and (activity_info.next_status_switch_time - TimeHelper.GetServerTime() < 86400) then
                v.main_btn_state = MayDayConfig.MainBtnState.ActEnd
            end
        end
        if ACTIVITY_TYPE.VITALITY_FLOP == v.act_type then
            item = v
        else
            local open_time, end_time = ActivityRandData.GetBaseDayOpenStampTime(v.act_type)
            if ActivityData.IsOpen(v.act_type) or (open_time or 0) == open_times[v.plan_type] then
                table.insert(list, v)
            end
        end
	end
	table.sort(list, function(a, b)
		return self:GetNowActState(a.act_type) * 1000 + a.main_btn_state * 100 + a.seq < self:GetNowActState(b.act_type) * 1000 + b.main_btn_state * 100 + b.seq
	end)
	for i, v in ipairs(list) do
		v.index = i
	end
	return list, item
end

function MayDayData:GetNowActState(act_type)
	local open_time, end_time = ActivityRandData.GetBaseDayOpenStampTime(act_type)
	return ActivityData.IsOpen(act_type) and 1 or ((open_time or 0) >= TimeHelper.GetServerTime() and 2 or 3)
end

-- function MayDayData:GetMainBtnState(act_type)
-- 	return self.main_btn_states[act_type] or MayDayConfig.MainBtnState.Go
-- end

-- 获取活动预览奖励
function MayDayData:GetRewardPreview(act_type)
    local reward_cfg = Config.yuanqiqingdian_auto.rand_reward_item
    local list = {}
    for i,v in ipairs(reward_cfg) do
        if v.act_type == act_type then
            table.insert(list, v)
        end
    end
    if TableIsEmpty(list) then
        return {}
    end
    local reward = ServerActivityData.Instance:GetActOpenDayCfg(act_type, list)
    if TableIsEmpty(reward) then
        return {}
    else
        return reward[1].reward_item
    end
end

function MayDayData:MainDayRewardItem()
	local reward_item = Config.yuanqifanpai_auto.other[1].reward_item
    return Item.Init(reward_item.item_id, reward_item.num, reward_item.is_bind)
end

function MayDayData:SetIsFetchFayReward(is_fetch_day_reward)
	self.main_info.is_fetch_day_reward = is_fetch_day_reward
end

function MayDayData:ActTimeShow(act_type)
    local start = ActivityData.GetStartStampTime(act_type)
    local end_time = ActivityData.GetSwitchStampTime(act_type)
    end_time = end_time - 1
    if start == 0 and end_time == 0 then
        return Language.Activity.NotOpenTip
    else
        local s = os.date("*t",start)
        local e = os.date("*t",end_time)
        return string.format(Language.MayDay.TimeShow, s.month, s.day, e.month, e.day)
    end
end

-- 元气庆典主界面按钮状态 main_btn_state = MayDayConfig.MainBtnState
function MayDayData:MainBtnState(act_type, main_btn_state)
    self.main_btn_states[act_type] = main_btn_state
    self:MainViewFlush()
end

function MayDayData:MainViewFlush()
    self.main_sm.is_flush = not self.main_sm.is_flush
end


-- 包子来了
function MayDayData:SetBaoZiLaiLeInfo(protocol)
    local info = protocol.info
    self.bun_come_info:Set(info)

    self:MainBtnState(ACTIVITY_TYPE.RAND_BUN_COME, info.count == self:BunComeConfigOtherFrequency() and MayDayConfig.MainBtnState.NextDay or MayDayConfig.MainBtnState.Go)
end

function MayDayData:BunComeConfigOtherFrequency()
    return Config.baozilaile_auto.other[1].frequency
end

function MayDayData:BunComeConfigOtherDesc()
    return Config.baozilaile_auto.other[1].desc
end

function MayDayData:ResetBunComeInfo()
    self.bun_come_sm.state = MayDayConfig.BunComeState.Ready
    self.bun_come_sm.num = 0
end

function MayDayData:BunComeGameRandom()
    local list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_BUN_COME, Config.baozilaile_auto.reward)
    local succ = #list > 0
    if succ then
        local num = math.random(1, #list)
        return succ, list[num]
    end
    return succ
end

function MayDayData:BunComeRewardShow()
    local list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_BUN_COME, Config.baozilaile_auto.reward_preview)
    if list[1] and nil == list[1].reward_show then
        local reward_show = {}
        local str_rewards = string.split(list[1].preview, ",")
        for i = 1, #str_rewards do
            local str_reward = string.split(str_rewards[i], ":")
            table.insert(reward_show, Item.Init(tonumber(str_reward[1]), tonumber(str_reward[2]), tonumber(str_reward[3])))
        end
        list[1].reward_show = reward_show
    end
    return list[1] and list[1].reward_show or {}
end

function MayDayData:BunComeRemind()
    return self.bun_come_info.count < self:BunComeConfigOtherFrequency() and 1 or 0
end


-- 元气记事簿
function MayDayData:SetRAYuanQiJiShiBuInfo(protocol)
    self.vitality_note_info:Set(protocol.info)
end

function MayDayData:VitalityNoteConfigOtherPrice()
    return Config.yuanqijishibu_auto.other[1].price
end

function MayDayData:VitalityNoteOpenDay()
    return TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_VITALITY_NOTE)
end

function MayDayData:VitalityNoteTaskList()
    local task_list = {}
    local open_day = self:VitalityNoteOpenDay()
    for _, task in ipairs(Config.yuanqijishibu_auto.task) do
        if task.section == open_day then
            table.insert(task_list, task)
        end
    end
    return task_list
end

function MayDayData:VitalityNoteRewardList()
    local list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_VITALITY_NOTE, Config.yuanqijishibu_auto.reward)
    for index, item in ipairs(list) do
        item.index = index
    end
	return list
end

function MayDayData:VitalityNoteRemind()
	local info = self.vitality_note_info
	if info.init then
		return 0
	end
	local list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_VITALITY_NOTE, Config.yuanqijishibu_auto.reward)
    local is_buy = 1 == info.is_jinjie_jiangli
	for _, reward in pairs(list) do
        local can_get = info.combat >= reward.combat
        if can_get then
            local is_get1 = 1 == bit:_rshift(info.quanming_reward_fetch_flag, reward.seq) % 2
            if not is_get1 then
                return 1
            end
            if is_buy then
                local is_get2 = 1 == bit:_rshift(info.jinjie_reward_fetch_flag, reward.seq) % 2
                if not is_get2 then
                    return 1
                end
            end
        end
	end
	return 0
end