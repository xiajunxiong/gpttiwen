PartnerUpGiftView = PartnerUpGiftView or DeclareView("PartnerUpGiftView", PartnerConfig.ResPath .. "partner_up_gift")

function PartnerUpGiftView:PartnerUpGiftView()
	self.data = PartnerCtrl.Instance:Data()
	self.language = Language.Partner
end

function PartnerUpGiftView:LoadCallback()
	self.Board:SetData(self:GetType(), self.language.UpGift, Vector2.New(654, 520))
end

function PartnerUpGiftView:OnFlush()
    local total = self.data:GetPartnerTotalLevel()
    self.nothastip:SetActive(total == 0)
    if total > 0 then 
        local attr_list = self.data:GetPartnerTotalGiftAttr()
        for i = 1,self.Attrs:Length() do
            if attr_list[i] ~= nil then 
                local attr_name = Language.Common.AttrList[attr_list[i].attr_type]
                local attr_add = DataHelper.IsPercent(attr_list[i].attr_type) and Percent(attr_list[i].attr_add or 0) or (attr_list[i].attr_add or 0)
                UH.SetText(self.Attrs[i] ,attr_name ..": " ..RichTextHelper.ColorStr("+" .. attr_add, COLORSTR.Green3))
            end 
        end 
    end 

    local effs = self.data:GetPartnerGiftSpEffectCfg()
    for i = 1,3 do 
        local act_str = total >= effs[i].inborn_level_ask and "" or string.format(Language.Partner.GiftSkillEffLockDetail,effs[i].inborn_level_ask)
        UH.SetIcon(self["sp_"..i][1], effs[i].sp_icon, ICON_TYPE.BUFF)
        UH.SetText(self["sp_"..i][2], effs[i].sp_name..act_str)
        UH.SetText(self["sp_"..i][3], effs[i].sp_des)
    end 
end