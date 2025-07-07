
MainViewBattleTop = MainViewBattleTop or DeclareMono("MainViewBattleTop", UIWFlushPanel)

function MainViewBattleTop:MainViewBattleTop()
    self.battle_data = BattleData.Instance
    self.base_data = self.battle_data:BaseData()

    self.data_cares = {
        {data = self.base_data, func = self.FlushBattleMode, keys = {"battle_mode"}},
    }
end

function MainViewBattleTop:FlushBattleMode()
    local battle_mode = self.base_data.battle_mode
    self.WorldBossSub:SetActive(battle_mode == BATTLE_MODE.BATTLE_MODE_SERVER_BOSS or battle_mode == BATTLE_MODE.BATTLE_MODE_SERVER_BOSS2 or battle_mode == BATTLE_MODE.BATTLE_MODE_SERVER_BOSS3)
end

MainBattleLimitTimeView = MainBattleLimitTimeView or DeclareMono("MainBattleLimitTimeView", UIWFlushPanel)

function MainBattleLimitTimeView:MainBattleLimitTimeView()
    self.base_data = BattleData.Instance:BaseData()
end

function MainBattleLimitTimeView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.Time:StampTime(self.base_data.limit_timestamp,TimeType.Type_Special_0)
    self:FlushHelp()
end

function MainBattleLimitTimeView:OnDisable()
    UIWFlushPanel.OnDisable(self)
    self.Time:CloseCountDownTime()
end

function MainBattleLimitTimeView:FlushHelp()
    UH.SetText(self.Help,Config.language_cfg_auto.textdesc[18].desc)
end

function MainBattleLimitTimeView:OnClickLimitTime()
    self.Parent:SetActive(not self.Parent.activeSelf)
end

MainBattleCurRoundView = MainBattleCurRoundView or DeclareMono("MainBattleCurRoundView", UIWFlushPanel)

function MainBattleCurRoundView:MainBattleCurRoundView()
    self.base_data = BattleData.Instance:BaseData()
    self.data_cares = {
        {data = self.base_data, func = self.FlushRoundView, init_call = true,keys = {"cur_round"}},
    }
end

function MainBattleCurRoundView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    UH.SetText(self.Help,self:GetDesc(Language.BattleEventTip[self.base_data.battle_mode]))
    self.Parent:SetActive(false)
    self:FlushRoundView()
end

function MainBattleCurRoundView:FlushRoundView()
    local config = Language.BattleEventTip[self.base_data.battle_mode]
    UH.SetText(self.Round,config and Format("%s/%s",self.base_data.cur_round,self:GetRoundNum(config)) or "")
end

function MainBattleCurRoundView:OnClickLimitTime()
    self.Parent:SetActive(not self.Parent.activeSelf)
end

function MainBattleCurRoundView:OnClickClose()
    self.Parent:SetActive(false)
end

function MainBattleCurRoundView:GetDesc(data)
    if data.round_func then
        return Format(data.desc,self:GetRoundNum(data))
    end
    return data.desc
end

function MainBattleCurRoundView:GetRoundNum(data)
    if data.round_func then
        return data.round_func() or 0
    end
    return data.round
end

MainBattleScneDescribeView = MainBattleScneDescribeView or DeclareMono("MainBattleScneDescribeView", UIWFlushPanel)

function MainBattleScneDescribeView:MainBattleScneDescribeView()
    self.base_data = BattleData.Instance:BaseData()
end

function MainBattleScneDescribeView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.Time:StampTime(self.base_data.limit_timestamp,TimeType.Type_Special_0)
    self:FlushHelp()
end

function MainBattleScneDescribeView:OnDisable()
    UIWFlushPanel.OnDisable(self)
    self.Time:CloseCountDownTime()
end

function MainBattleScneDescribeView:FlushHelp()
    local battle_mode = self.base_data.battle_mode
    local config = Config.language_cfg_auto.textdesc
    local is_guild_fight = battle_mode == BATTLE_MODE.BATTLE_MODE_GUILD_FIGHT
    UH.SetText(self.Help,config[is_guild_fight and 18 or 17].desc)
end

function MainBattleScneDescribeView:OnClickLimitTime()
    self.Parent:SetActive(not self.Parent.activeSelf)
end