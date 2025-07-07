TaxDiscountView = TaxDiscountView or DeclareView("TaxDiscountView","serveractivity/tax_discount_view",Mod.RandActivity.TaxDiscount)
VIEW_DECLARE_LEVEL(TaxDiscountView, ViewLevel.L1)
function TaxDiscountView:TaxDiscountView()
    self.Board:SetData(self:GetType(), ActivityRandData.GetName(ACTIVITY_TYPE.RAND_TAX_DISCOUNT))
end

function TaxDiscountView:LoadCallback()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_TAX_DISCOUNT,TimeType.Type_Special_4)
    UH.SetText(self.Num,ServerActivityData.Instance:GetTaxDiscountRate() .. "%")
end

function TaxDiscountView:OnClickClose()
    ViewMgr:CloseView(TaxDiscountView)
end

function TaxDiscountView:OnClickEnter()
    ViewMgr:OpenViewByKey(Mod.Mall.Trade)
end

function TaxDiscountView:OnClickHelp()
    local rate = ServerActivityData.Instance:GetTaxDiscountRate()
    local desc = Config.language_cfg_auto.textdesc[43].desc
    PublicPopupCtrl.Instance:HelpTip(Format(desc,rate))
end

----------------------------TaxDiscountPanel----------------------------
TaxDiscountPanel = TaxDiscountPanel or DeclareMono("TaxDiscountPanel", UIWFlushPanel)

function TaxDiscountPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    if ActivityData.IsOpen(ACTIVITY_TYPE.RAND_TAX_DISCOUNT) then
        local rate = ServerActivityData.Instance:GetTaxDiscountRate()
        UH.SetText(self.DiscountTip,Format(Language.Trade.RateTip,rate))
    end
end