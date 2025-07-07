VIEW_REQ("modules/reward_online_gift/reward_online_gift_view")

RewardOnlineGiftCtrl = RewardOnlineGiftCtrl or BaseClass(BaseCtrl)
function RewardOnlineGiftCtrl:__init()
	if RewardOnlineGiftCtrl.Instance ~= nil then
		Debuger.LogError("[RewardOnlineGiftCtrl] attempt to create singleton twice!")
		return
	end	
	RewardOnlineGiftCtrl.Instance = self

    self.data = RewardOnlineGiftData.New()
    self:RegisterAllProtocols()
end 

function RewardOnlineGiftCtrl:OnInit()
    
end

function RewardOnlineGiftCtrl:SendReqForReward(paramt)
    RewardCtrl.Instance:SendFetchRewardReq({
        oper_type = 6,
        seq = paramt.seq,
    })
end

function RewardOnlineGiftCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCDayRewardOnlineGiftInfo, "OnDayRewardOnlineGiftInfo")
end

function RewardOnlineGiftCtrl:OnDayRewardOnlineGiftInfo(protocol)
    self.data:SetNetOnlineInfo(protocol)
end

RewardOnlineGiftData = RewardOnlineGiftData or BaseClass()
function RewardOnlineGiftData:__init()
    if RewardOnlineGiftData.Instance ~= nil then
        Debuger.LogError("[RewardOnlineGiftData] attempt to create singleton twice!")
        return
    end
    RewardOnlineGiftData.Instance = self
    self.config = Config.day_reward_auto.online_gift

    self.base_info = SmartData.New()
    self.check_info = SmartData.New({flush = 0})

end 

function RewardOnlineGiftData:SetNetOnlineInfo(protocol)
    -- LogError("杰哥哥，这是在线礼包的下发哦?1990",protocol)
    self.base_info.begin_timestamp = protocol.online_gift_join_begin_timestamp -- 开始累积时间
    self.base_info.online_gift_flag = protocol.online_gift_flag --领取标记
    self.base_info.online_gift_times = protocol.online_gift_times -- 已经累积时间

    self:CheckRunTimer()
end

function RewardOnlineGiftData:CheckRunTimer()
    TimeHelper:CancelTimer(self.time_func)

    self.check_info.flush = self:GetCompleteNum()
    if not self:ShouldShowGift() then 
        return 
    end 

    self.time_func =
        TimeHelper:AddRunTimer(
        function()
            if self:WithComplete() then 
                self.check_info.flush = self:GetCompleteNum()
            end 
        end,
        5)
end

function RewardOnlineGiftData:GetCellList()
    local list = {}
    for k,v in pairs(self.config) do 
        local vo = {}
        vo.index = v.index
        vo.cfg = v
        table.insert(list,vo)
    end 

    table.sort(list, DataSorter.KeyLowerSorter("index"))
 
    return list
end

function RewardOnlineGiftData:CheckStatus(data)
    local play_time = self:GetNeedWaitTime(data.time)
    local last_play_time = data.index == 1 and 0 or self:GetNeedWaitTime(self.config[data.index - 1].time)
    local is_running = last_play_time <= 0 and play_time > 0

    local item_catch = self.base_info.online_gift_flag[data.seq] == 1

    local now_complete = play_time <= 0 
    
    return {
        play_time = play_time,                -- 需计时时长
        item_catch = item_catch,              -- 已领取
        is_running = is_running,              -- 正在进行时
        now_complete = now_complete,          -- 已完成计时
    }
end

function RewardOnlineGiftData:GetNeedWaitTime(need_time)
    return (self.base_info.begin_timestamp + need_time - TimeManager.GetServerTime() - self.base_info.online_gift_times)
end

-- 
function RewardOnlineGiftData:GetIsLastRunDoneTimer(data)
    if data.index == #self.config then
        local paly_time = self:GetNeedWaitTime(data.time)
        return true
    end 

    local rundone = 0
    for i = 1,#self.config  do 
        local play_time = self:GetNeedWaitTime(self.config[i].time)
        -- 还在走的倒计时的上一个就是最新走完的
        if play_time > 0 then 
            rundone = self.config[i].seq - 1
            break
        end 
    end 

    return rundone == data.seq
end

-- 是否已完成所有的在线礼包
function RewardOnlineGiftData:ShouldShowGift()
    local role_level = RoleData.Instance:GetRoleLevel()
    local is_complete = true
    for i = 1,#self.config  do  
        if self.base_info.online_gift_flag[self.config[i].seq] == 0 then
            is_complete = false
            break
        end 
    end 
    return role_level >= Config.day_reward_auto.reward_find[1].online_gift_open and not is_complete and self:CanJoin()
end

-- 是否创角第一天
function RewardOnlineGiftData:ShouldForeShow()
    return self:CanJoin()
end

-- 能否启动
function RewardOnlineGiftData:CanJoin()
    return self.base_info.begin_timestamp > 0
end

function RewardOnlineGiftData:WithComplete()
    local with_complete =false
    for i = 1,#self.config  do  
        local timer = self:GetNeedWaitTime(self.config[i].time)
        if self.base_info.online_gift_flag[self.config[i].seq] == 0 and timer <= 0 then
            with_complete = true
            break
        end 
    end 

    return with_complete
end

function RewardOnlineGiftData:GetRunningGiftTimer()
    if self:WithComplete() then 
        return 0
    end 

    local cur_running = 0
    for i = 1,#self.config  do  
        local timer = self:GetNeedWaitTime(self.config[i].time)
        if self.base_info.online_gift_flag[self.config[i].seq] == 0 and timer > 0 then
            cur_running = timer
            break
        end 
    end 

    return cur_running
end

function RewardOnlineGiftData:CompleteText()
    local with_complete = self:WithComplete()
    return with_complete and Language.RewardOnlineGift.WithFor or Language.RewardOnlineGift.WaitingFor
end

function RewardOnlineGiftData:GetCompleteNum()
    local num = 0
    for i = 1,#self.config  do  
        local timer = self:GetNeedWaitTime(self.config[i].time)
        if self.base_info.online_gift_flag[self.config[i].seq] == 0 and timer <= 0 then
            num = num + 1
        end 
    end 

    return num
end