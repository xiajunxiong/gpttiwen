
PartnerLevelResetView = PartnerLevelResetView or DeclareView("PartnerLevelResetView", PartnerConfig.ResPath .. "partner_level_reset", nil, ViewLayer.Top + 2)
VIEW_DECLARE_MASK(PartnerLevelResetView, ViewMask.BgBlockNoAnim)

function PartnerLevelResetView:PartnerLevelResetView()
    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.LevelReset
end

function PartnerLevelResetView:LoadCallback()
	if -1 == self.data.cur_sel_partner then return end
	local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    self.Content.text = self.language.content_show
    self.ResetVal.text = string.format(self.language.back_show, data.info.reset_up_level)
    self.CostVal.text = self.data:GetPartnerResetCostVal()
end

function PartnerLevelResetView:OnClickConfirm()
    if RoleData.Instance:GetCurrency(CurrencyType.CoinBind) < self.data:GetPartnerResetCostVal() then
        ViewMgr:CloseView(PartnerLevelResetView)
        PublicPopupCtrl.Instance:AlertTip(Language.Mall.NotCurrency[2],function()
            MallCtrl.Instance:Jump(24326)
        end)
        return
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QiangHuaChengGong)
	PartnerCtrl.Instance:SendPartnerResetLevel(self.data.cur_sel_partner)
	ViewMgr:CloseView(PartnerLevelResetView)
end

function PartnerLevelResetView:OnClickClose()
    ViewMgr:CloseView(PartnerLevelResetView)
end
