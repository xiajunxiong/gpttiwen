--等级助力
LevelComplementView = LevelComplementView or DeclareView("LevelComplementView", "level_complement/level_complement", Mod.LevelComplement.Main)
VIEW_DECLARE_LEVEL(LevelComplementView, ViewLevel.L1)

function LevelComplementView:LevelComplementView()
    self.Currency:DefaultSet()
    
    local save_key = LevelComplementCtrl.Instance:SaveKey()
    if PrefsInt(save_key) == 0 then
        PrefsInt(save_key, 1)
    end
end

function LevelComplementView:OnClickClose()
    ViewMgr:CloseView(LevelComplementView)
end

LevelComplementPanel = LevelComplementPanel or DeclareMono("LevelComplementPanel", UIWFlushPanel)
function LevelComplementPanel:LevelComplementPanel()
    self.data = LevelComplementData.Instance
    self.lang = Language.LevelComplement
    self.data_cares = {
        {data = self.data.data_info, func = self.FlushView}
    }
end

function LevelComplementPanel:FlushView()
    local cur_cfg = self.data:GetCurCfg()
    if cur_cfg == nil then
        return
    end
    local data_info = self.data:GetInfo()
    local desc3 = FlyUpData.Instance:IsLevelLimit() and Language.LevelComplement.LevelLimit or string.format(self.lang.DescList[3], self.data:GetNextDayAddExp())
    UH.SetText(self.DescList[1], string.format(self.lang.DescList[1], self.data:GetMultiple() * 100))
    UH.SetText(self.DescList[2], string.format(self.lang.DescList[2], self.data:GetExpLimit()))
    UH.SetText(self.DescList[3], desc3)

    local remain_exp = self.data:GetRemainExp()
    local exp_limit = self.data:GetExpLimit()
    UH.SetText(self.CurExp, string.format(self.lang.CurExp, remain_exp))
    UH.SetText(self.MaxExp, string.format(self.lang.MaxExp, exp_limit))
    self.ExpSlider:SetProgress(exp_limit > 0 and remain_exp / exp_limit or 0)

    local can_buy_exp = self.data:GetBuyExp()
    UH.SetText(self.BuyDesc, string.format(self.lang.BuyTips, cur_cfg.price, self:GetHuoBiName(cur_cfg), can_buy_exp))
    UH.SpriteName(self.PriceIcon, HuoBi[cur_cfg.price_type])
    UH.SetText(self.PriceValue, cur_cfg.price)

    local max_times = self.data:GetMaxBuyTimes()
    local buy_times = self.data:GetBuyTimes()
    local color_str = buy_times >= max_times and COLORSTR.Red5 or COLORSTR.Green11 
    UH.SetText(self.BuyTimes, Format(self.lang.BuyTimes, color_str, (max_times - buy_times), max_times))
    self.BtnBuyInter.Interactable = remain_exp ~= 0 and buy_times < max_times
end

function LevelComplementPanel:GetHuoBiName(cur_cfg)
    return Language.Common.HuoBi[cur_cfg.price_type]
end

function LevelComplementPanel:FlushTime(stamp_time)
    self.ActTimer:SetShowCallBack(BindTool.Bind(self.OnTimeUpdate, self))
    self.ActTimer:SetCallBack(BindTool.Bind(self.OnTimeComplete, self))
    self.ActTimer:StampTime(stamp_time)
end
function LevelComplementPanel:OnTimeUpdate(t)
    local t_str = ""
    if t.day > 0 then
        t_str = string.format(Language.UiTimeMeter.Special3, ColorStr(t.day, COLORSTR.Green11), ColorStr(t.hour, COLORSTR.Green11))
    else
        t_str = string.format(Language.UiTimeMeter.Special4, ColorStr(t.hour, COLORSTR.Green11), ColorStr(t.min, COLORSTR.Green11))
    end
    return self.lang.RemainTime .. t_str
end
function LevelComplementPanel:OnTimeComplete()
    ViewMgr:CloseView(LevelComplementView)
end

function LevelComplementPanel:OnClickBuy()
    local cur_cfg = self.data:GetCurCfg()
    if cur_cfg == nil then
        return
    end
    if self.data:GetRemainExp() == 0 then
        PublicPopupCtrl.Instance:Center(self.lang.Error1)
        return
    elseif not MallCtrl.IsNotCurrency(cur_cfg.price_type, cur_cfg.price) then
        return
    end

    local can_buy_exp = math.min(self.data:GetRemainExp(), self.data:GetBuyExp())
    local param_t = {
        content = string.format(self.lang.Alert1, cur_cfg.price, self:GetHuoBiName(cur_cfg), can_buy_exp),
        confirm = {
            func = function()
                LevelComplementCtrl.Instance:SendBuy()
                PublicPopupCtrl.Instance:CloseDialogTips()
            end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(param_t)
end
