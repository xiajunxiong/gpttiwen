FightDenFinish = FightDenFinish or DeclareView("FightDenFinish", "fight_den/fight_den_finish")
VIEW_DECLARE_MASK(FightDenFinish, ViewMask.BlockClose)

function FightDenFinish:FightDenFinish()
    self.data = FightDenData.Instance
    self.lang = Language.FightDen
end
function FightDenFinish:LoadCallback()
end
function FightDenFinish:OnFlush()
    local info = self.data:GetReward()
    UH.SetText(self.Level, string.format(self.lang.Result, self.data:PassWave(true) or self.data.wave.num, self.data:GetMaxWave()))
    self.ItemList:SetData(info)
    self.handle_delay = TimeHelper:AddDelayTimer(function ()
        self:AutoDragon()
    end, TeamConfig.AutoTime)
end
function FightDenFinish:AutoDragon()
    local dragon = TeamCtrl.Instance:Dragon()
    if dragon then
        local pass_wave = FightDenData.Instance:PassWave(true)
        local max_wave = FightDenData.Instance:GetMaxWave()
        if pass_wave >= 1 and pass_wave < max_wave then
            if DEBUG_MODE then
                LogError("太菜了没打满算了算了")
            end
        else
            dragon:Start()
        end
    else
        if DEBUG_MODE then
            LogError("没有一条龙服务3333")
        end
    end
    --[[ if dragon and dragon:Step() > 2 then
        self:OnClickClose()
        local count = FightData.Instance:GetTodayFantasyFetch()
        if count < 4 then
            TaskData.Instance:TraceTask(TaskConfig.TaskType.dragon, CUSTOM_OBJID.FantasyFB)
            BehaviorCtrl.ExeByCfgVo(BehaviorCtrl.GetCfg(5063))
        end
    end ]]
end
function FightDenFinish:CloseCallback()
    self.data:ClearFinishReward()
    -- TimeHelper:CancelTimer(self.handle_delay)
end
function FightDenFinish:OnClickClose()
    ViewMgr:CloseView(FightDenFinish)
    self.data:SetWave(1)
end

FightDenFinishFail = FightDenFinishFail or DeclareView("FightDenFinishFail", "fight_den/fight_den_finish_fail")
VIEW_DECLARE_MASK(FightDenFinishFail, ViewMask.BgBlockClose)
