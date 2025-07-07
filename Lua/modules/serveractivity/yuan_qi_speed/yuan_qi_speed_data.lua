YuanQiSpeedData = YuanQiSpeedData or BaseClass()
function YuanQiSpeedData:__init()
    if YuanQiSpeedData.Instance ~= nil then
        Debuger.LogError("[YuanQiSpeedData] attempt to create singleton twice!")
        return
    end
    YuanQiSpeedData.Instance = self

    self.act_info = SmartData.New({today_play_times = 0, pass_flag = {}, reward_flag = {}})
    self.game_info = SmartData.New({game_type = -1, })
    self.select_info = SmartData.New({seq = -1, flush = false})
end

function YuanQiSpeedData:__delete()
    YuanQiSpeedData.Instance = nil
end

function YuanQiSpeedData:Config()
    return Config.yuanqijingsu_auto
end
function YuanQiSpeedData:OtherConfig()
    return self:Config().other[1]
end

function YuanQiSpeedData:GetRabbitCfg(seq)
    return self:Config().rabbit[seq]
end

function YuanQiSpeedData:GetShowList()
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_YUAN_QI_SPEED, self:Config().reward, "seq")
end
function YuanQiSpeedData:GetShowCfg(seq)
    return DataHelper.GetSearch(self:GetShowList(), "seq", seq)
end

function YuanQiSpeedData:SetActInfo(protocol)
    self.act_info.today_play_times = protocol.today_play_times
    self.act_info.pass_flag = protocol.pass_flag
    self.act_info.reward_flag = protocol.reward_flag

    local red_num = self:GetRemindNum();
    MayDayData.Instance:MainBtnState(ACTIVITY_TYPE.RAND_YUAN_QI_SPEED, red_num > 0 and MayDayConfig.MainBtnState.Go or MayDayConfig.MainBtnState.NextDay)
end

function YuanQiSpeedData:SetGameInfo(protocol)
    self.game_info.game_type = protocol.game_type
    self.game_info.play_end_tamp = protocol.play_end_tamp
    self.game_info.play_seq = protocol.play_seq
end

function YuanQiSpeedData:SetSelect(seq)
    self.select_info.seq = seq
    self.select_info:Notify()
end
function YuanQiSpeedData:GetSelect()
    return self.select_info.seq
end

--参与次数  最大次数
function YuanQiSpeedData:GetJoinTimes()
    return self.act_info.today_play_times
end
function YuanQiSpeedData:GetMaxTimes()
    return self:OtherConfig().frequency
end

--是否通关过    首通奖励是否领取
function YuanQiSpeedData:IsPassFlag(seq)
    return self.act_info.pass_flag[seq + 1] == 1
end
function YuanQiSpeedData:IsPassFetched(seq)
    return self.act_info.reward_flag[seq + 1] == 1
end

function YuanQiSpeedData:GetPKTime()
    return self:OtherConfig().time
end

-----红点 (有次数，可领首通)
function YuanQiSpeedData:GetRemindNum()
    if PeriodActivityData.Instance:GetRemindData(Mod.MayDay.YuanQiSpeed) then
        return 0
    end
    if self:GetJoinTimes() < self:GetMaxTimes() then
        return 1
    end
    for i = 0, 2 do
        if self:IsPassFlag(i) and not self:IsPassFetched(i) then
            return 1
        end
    end
    return 0
end
