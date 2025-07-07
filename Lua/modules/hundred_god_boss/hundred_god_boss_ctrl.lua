FILE_REQ("modules/hundred_god_boss/hundred_god_boss_data")
VIEW_REQ("modules/hundred_god_boss/hundred_god_boss")

HundredGodBossCtrl = HundredGodBossCtrl or BaseClass(BaseCtrl)
function HundredGodBossCtrl:__init()
    if HundredGodBossCtrl.Instance ~= nil then
		Debuger.LogError("[HundredGodBossCtrl] attempt to create singleton twice!")
		return
	end
	HundredGodBossCtrl.Instance = self
    
    self.data = HundredGodBossData.New()

    self:RegisterProtocol(CSHundredGodBossReq)
    self:RegisterProtocol(SCHundredGodBossInfo, "SCHundredGodBossInfo")
    local remind_func = BindTool.Bind(self.GetRemindNum, self)
    Remind.Instance:Register(Mod.HundredGodBoss.Main, self.data:Info(), remind_func)
    -- FunOpen.Instance:SelfCondition(Mod.HundredGodBoss.Main, function ()
    --     return not self.data:TimeOut()
    -- end)
    ActivityRandData.Instance:Register(Mod.HundredGodBoss.Main,function ()
        return not self.data:TimeOut()
    end,self.data:Info())

    -- TaskData.Instance:ListenTaskCompleteByIds(BindTool.Bind(self.OnTaskComplete, self), {self.data:PerTask()})
end
function HundredGodBossCtrl:OnUnloadGameLogic()
    self.data:Clear()
end
function HundredGodBossCtrl:OnTaskComplete()
    -- LogDG("任务完成111", TaskData.Instance:GetTaskIsCompleted(self.data:PerTask()))
    -- HundredGodBossCtrl.Start()
end
function HundredGodBossCtrl:__delete()
    
end
function HundredGodBossCtrl:GetRemindNum()
    if self.data:TimeOut() then
        return 0
    end
    return self.data:GetRemindNum()
end
function HundredGodBossCtrl:SCHundredGodBossInfo(protocol)
    -- LogDG(protocol)
    -- is_finish 已领取和到时间都会变成1
    self.data:Info().is_finish = (protocol.is_finish_hundred_god_boss_flag == 1)
    self.data:Info().count = protocol.hundred_god_boss_fin_bounty_count
    self.data:Info().reward_flag = protocol.fetch_hundred_god_boss_flag
    self.data:Info().time = protocol.hundred_god_boss_end_timestamp
    ActivityRandData.Instance:RegisterActCountDown(Mod.HundredGodBoss.Main, BindTool.Bind(self.Time, self))
end
function HundredGodBossCtrl:Time(timeMeter)
    local param_t = {
        stamp_time = self.data:Info().time,
        show_type = TimeType.Type_Time_8,
        none_zero = false,
    }
    timeMeter:SetShowCallBack(function ()
        if self.data:AllFinish() then
            return Language.HundredGodBoss.CanGet
        else
            return self.data:UnFinishTaskShow()
        end
    end)
    timeMeter:SetData(param_t)
end
function HundredGodBossCtrl.ReqInfo()
    local protocol = GetProtocol(CSHundredGodBossReq)
    protocol.req_type = 0
    protocol.param = 0
    SendProtocol(protocol)
end
function HundredGodBossCtrl.TargetReward(target_id)
    local protocol = GetProtocol(CSHundredGodBossReq)
    protocol.req_type = 1
    protocol.param = target_id
    SendProtocol(protocol)
end
function HundredGodBossCtrl.FinalReward()
    local protocol = GetProtocol(CSHundredGodBossReq)
    protocol.req_type = 2
    protocol.param = 0
    SendProtocol(protocol)
end
function HundredGodBossCtrl.Start()
    local protocol = GetProtocol(CSHundredGodBossReq)
    protocol.req_type = 3
    protocol.param = 0
    SendProtocol(protocol)
end