MainViewBattleWarning = MainViewBattleWarning or DeclareMono("MainViewBattleWarning", UIWFlushPanel)

function MainViewBattleWarning:MainViewBattleWarning()
    self.battle_data = BattleData.Instance
    self.language = Language.Main.Battle.Warning
    self.data_cares = {
        {data = self.battle_data:BaseData(), func = self.FlushBattle, keys = {"battle_mode"}}
    }
end

function MainViewBattleWarning:FlushBattle()
    local battle_mode = self.battle_data:BattleMode()
    local warning_text = self.language[battle_mode]
    if not warning_text then
        local no_error, text = pcall(BindTool.Bind1(self.WarningText, self), battle_mode)
        if no_error then
            warning_text = text
        elseif BattleSimulator.Instance:IsPlaying() then
            warning_text = "模拟中，暂不支持此功能"
        end
    end
    if warning_text then
        self.Content.text = warning_text
        TimeHelper:CancelTimer(self.timer_play)
        self.EffectTool:Play(10010269)
    end
end

--这个方法返回变化的警告 如果是固定的加在Language.Main.Battle.Warning
function MainViewBattleWarning:WarningText(battle_mode)
    if BATTLE_MODE.BATTLE_MODE_SERVER_BOSS == battle_mode then
        return WorldBossCtrl.Instance:Data():GetBattleWarningText()
    elseif BATTLE_MODE.BATTLE_MODE_SERVER_BOSS2 == battle_mode then
        return WorldBossCtrl.Instance:Data():GetBattleWarningText2()
    elseif BATTLE_MODE.BATTLE_MODE_SERVER_BOSS3 == battle_mode then
        return WorldBossCtrl.Instance:Data():GetBattleWarningText3()
    elseif BATTLE_MODE.BATTLE_MODE_CHIEF_ELECTION == battle_mode then
        return ChiefElectionData.Instance:GetBattleWarningText()
    elseif BATTLE_MODE.BATTLE_MODE_CHIEF_PEAK_BATTLE == battle_mode then
        return ChiefPeakBattleData.Instance:GetBattleWarningText()
    elseif BATTLE_MODE.BATTLE_MODE_WAVE == battle_mode then
        return FightDenData.Instance:GetBattleWarningText()
    end
end

function MainViewBattleWarning:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_play)
    TimeHelper:CancelTimer(self.timer_tt)
    TimeHelper:CancelTimer(self.timer_dt1)
    TimeHelper:CancelTimer(self.timer_dt2)
end

function MainViewBattleWarning:OnEffectEvent(handle, time, event)
    if EffectEvent.event_start == event then
        self.Content.color.a = 0
        local total_s = 0.1
        local total_e = 0.25
        local interval = 0.1
        TimeHelper:CancelTimer(self.timer_dt1)
        self.timer_dt1 =
            TimeHelper:AddDelayTimer(
            function()
                TimeHelper:CancelTimer(self.timer_tt)
                self.timer_tt =
                    TimeHelper:AddCountDownTT(
                    function()
                        self.ContentAlpha.alpha = self.ContentAlpha.alpha + 0.15 * (interval / total_s)
                    end,
                    function()
                        self.ContentAlpha.alpha = 1
                    end,
                    total_s,
                    interval
                )
            end,
            0.2
        )
        TimeHelper:CancelTimer(self.timer_dt2)
        self.timer_dt2 =
            TimeHelper:AddDelayTimer(
            function()
                TimeHelper:CancelTimer(self.timer_tt)
                self.timer_tt =
                    TimeHelper:AddCountDownTT(
                    function()
                        self.ContentAlpha.alpha = self.ContentAlpha.alpha - 1 * (interval / total_e)
                    end,
                    function()
                        self.ContentAlpha.alpha = 0
                        self.Content.text = ""
                    end,
                    total_e,
                    interval
                )
            end,
            1
        )
    else
        TimeHelper:CancelTimer(self.timer_tt)
        TimeHelper:CancelTimer(self.timer_dt1)
        TimeHelper:CancelTimer(self.timer_dt2)
        self.Content.text = ""
    end
end
