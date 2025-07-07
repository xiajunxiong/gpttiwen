MonthCardAlert = MonthCardAlert or DeclareView("MonthCardAlert", RewardConfig.ResPath .. "month_card_alert")
VIEW_DECLARE_MASK(MonthCardAlert, ViewMask.BlockClose)

function MonthCardAlert:LoadCallback(param_t)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    self.data = param_t
end

function MonthCardAlert:OnFlush()
    local card_type = self.data.card_type
    local card_cfg = RewardData.Instance:GetMonthCardCfg(card_type)
    local tips_str = string.format(Language.MonthCard.TwiceBuyTip, Language.MonthCard.CardType[card_cfg.card_type], card_cfg.buy_money / 10)
    UH.SetText(self.Desc, tips_str)
end

function MonthCardAlert:OnClickOk()
    self.data.func_ok()
    ViewMgr:CloseView(MonthCardAlert)
end

function MonthCardAlert:OnClickClose()
    ViewMgr:CloseView(MonthCardAlert)
end
