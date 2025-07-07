
AnecdoteHintData = AnecdoteHintData or BaseClass()
AnecdoteHintData.ActIconName = "wanlingdashijian"
function AnecdoteHintData:__init()
    if AnecdoteHintData.Instance ~= nil then
        Debuger.LogError("[AnecdoteHintData] attempt to create singleton twice!")
        return
    end
    AnecdoteHintData.Instance = self

    self.view_data = SmartData.New({
        cur_sel = nil,      --当前选中的数据
    })
    self.net_data = SmartData.New({
        reward_version = 0,
        is_open = false,
        end_timestamp = 0,
        task_finish_num = 0,
        reward_fetch_status = 0,
    })
    self.hint_close_timer = nil
end

function AnecdoteHintData:HintAnecdoteList()
    return Config.anecdotes_tasklist_auto.event_activity
end

function AnecdoteHintData:SetCurSelData(cur_sel)
    self.view_data.cur_sel = cur_sel
end

function AnecdoteHintData:CurSelData()
    return self.view_data.cur_sel
end

function AnecdoteHintData:ViewData()
    return self.view_data
end

function AnecdoteHintData:OnSCAnecdoteBigEventInfo(protc)
    self.net_data.reward_version = protc.reward_version
    self.net_data.is_open = protc.is_big_event_open_now ~= 0
    self.net_data.end_timestamp = protc.end_timestamp
    self.net_data.task_finish_num = protc.anecdote_finish_num
    self.net_data.reward_fetch_status = protc.reward_fetch_status
    self:CancelCloseTimer()
    if self:IsOpening() then
        self.hint_close_timer = TimeHelper:AddDelayTimer(
            function() 
                self.net_data.is_open = false
            end,
            AnecdoteHintData.Instance:EndTimeStamp() - TimeHelper:GetServerTime())
    end

end
function AnecdoteHintData:CancelCloseTimer()
    if self.hint_close_timer ~= nil then
        TimeHelper:CancelTimer(self.hint_close_timer)
        self.hint_close_timer = nil
    end
end

function AnecdoteHintData:Clear()
    self:CancelCloseTimer()
end

function AnecdoteHintData:EndTimeStamp()
    return self.net_data.end_timestamp
end

function AnecdoteHintData:IsOpening()
    return self.net_data.is_open and TimeHelper:GetServerTime() < self.net_data.end_timestamp
end

function AnecdoteHintData:TaskFinishNum()
    return self.net_data.task_finish_num
end

function AnecdoteHintData:RewardHasGetted(reward_seq)
    return bit:_hasflag(self.net_data.reward_fetch_status,reward_seq+1) --因为服务端是从第二位开始算的(从0开始计数)
end

function AnecdoteHintData:NetData()
    return self.net_data
end

function AnecdoteHintData:RewardData()
    local re_da = {}
    for _,v in pairs(Config.anecdotes_tasklist_auto.event_reward) do
        if v.reward_reset == self.net_data.reward_version then
            local cfg_da = v
           -- cfg_da.seq = cfg_da.seq - 7 * self.net_data.reward_version
            re_da[cfg_da.seq] = cfg_da
        end
    end
    return re_da
end

function AnecdoteHintData:OnRemind()
    if not AnecdoteHintData.IsOpened({act_type = ActType.Hint }) then
        return 0
    end
    for _,v in ipairs(Config.anecdotes_tasklist_auto.event_reward) do
        if v.reward_reset == self.net_data.reward_version then
            if self.net_data.task_finish_num >= v.complete_num and self:RewardHasGetted(v.seq) == false then
                return 1
            end
        end
    end
    return 0
end

function AnecdoteHintData.IsHasRemind(data)
    return data.act_type == ActType.Hint and AnecdoteHintData.Instance:OnRemind() == 1
end

function AnecdoteHintData.IsOpened(data)
    if data.act_type == ActType.Hint then
        return AnecdoteHintData.Instance:IsOpening()
    end
    return true
end