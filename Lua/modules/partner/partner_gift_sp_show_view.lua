PartnerGiftSpShowView = PartnerGiftSpShowView or DeclareView("PartnerGiftSpShowView", PartnerConfig.ResPath .. "partner_gift_sp_show")
VIEW_DECLARE_MASK(PartnerGiftSpShowView, ViewMask.BgBlockCloseNoAnim)

function PartnerGiftSpShowView:PartnerGiftSpShowView()
    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.Up
end

function PartnerGiftSpShowView:LoadCallback()
    if -1 == self.data.cur_sel_partner then return end
    local total = self.data:GetPartnerTotalLevel()
    local effs = self.data:GetPartnerGiftSpEffectCfg()
    local high_lock = nil
    for k,v in pairs(effs) do 
        if high_lock == nil then 
            high_lock = v
        elseif high_lock.inborn_level_ask < v.inborn_level_ask and v.inborn_level_ask <= total then 
            high_lock = v
        end 
    end 

    UH.SetIcon(self.Icon, high_lock.sp_icon, ICON_TYPE.BUFF)
    UH.SetText(self.Name, high_lock.sp_name)
    UH.SetText(self.desc, high_lock.sp_des_clear)
end 