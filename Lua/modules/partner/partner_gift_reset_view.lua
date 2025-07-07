PartnerGiftResetView = PartnerGiftResetView or DeclareView("PartnerGiftResetView", PartnerConfig.ResPath .. "partner_gift_reset")
VIEW_DECLARE_MASK(PartnerGiftResetView, ViewMask.BgBlockNoAnim)

function PartnerGiftResetView:PartnerGiftResetView()
    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner
end

function PartnerGiftResetView:LoadCallback(param_t)
    self.Content.text = self.language.GiftResetTips
    self.reset_info = self.data:GetResetTotalResult()

    self.item_1_obj:SetActive(self.reset_info.item_list[1] ~= nil)
    if self.reset_info.item_list[1] ~= nil then 
        local cost_item_1 = Item.Create(self.reset_info.item_list[1])
        self.item_1:SetData(cost_item_1)
        UH.SetText(self.item_1_num,self.reset_info.item_list[1].num)
    end 

    self.item_2_obj:SetActive(self.reset_info.item_list[2] ~= nil)
    if self.reset_info.item_list[2] ~= nil then 
        local cost_item_2 = Item.Create(self.reset_info.item_list[2])
        self.item_2:SetData(cost_item_2)
        UH.SetText(self.item_2_num,self.reset_info.item_list[2].num)
    end 

    UH.SetText(self.CostVal,self.reset_info.coin)
end

function PartnerGiftResetView:OnClickConfirm()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    
    if RoleData.Instance:GetCurrency(CurrencyType.CoinBind) < self.reset_info.coin then
        PublicPopupCtrl.Instance:Center(string.format(Language.Common.NotEnough,Item.GetName(CommonItemId.Coin)))
        ViewMgr:CloseView(PartnerGiftResetView)
        return
    end
    PartnerCtrl.Instance:SendPartnerGiftReset(self.data.cur_sel_partner,self.data.gift_flush.cur_select)
	ViewMgr:CloseView(PartnerGiftResetView)
end

function PartnerGiftResetView:OnClickClose()
    ViewMgr:CloseView(PartnerGiftResetView)
end