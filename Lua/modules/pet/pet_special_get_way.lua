PetSpecialGetWayView = PetSpecialGetWayView or DeclareView("PetSpecialGetWayView", "pet/pet_special_get_way")

VIEW_DECLARE_MASK(PetSpecialGetWayView,ViewMask.BgBlockClose)

function PetSpecialGetWayView:LoadCallback(data)
    
    UH.SetText(self.Name, Language.Pet.QualityName[data.quality] .. (data.quality < 5 and Language.Pet.Pet or ""))
    UH.SetText(self.Type, Language.Pet.QualityTitle .. Language.Pet.QualityName[data.quality])
    UH.SetIcon(self.Icon, data.icon_id)
    UH.SpriteName(self.QuaIcon, PetData.PinZhi[data.quality])
    self:ShowPetGetWay(data.catch_way_str)
end

function PetSpecialGetWayView:OnClickClose()
    ViewMgr:CloseView(PetSpecialGetWayView)
end

function PetSpecialGetWayView:ShowPetGetWay(catch_way_str)

    local catch_way_t = {}
    local split = string.split(tostring(catch_way_str), "|")
    for k, v in pairs(split) do
        table.insert(catch_way_t, BagData.Instance:GetWayCfg(v))
    end

    self.PetFunctions:SetData(catch_way_t)
end
---------------------------------------------
PetSpecialGetWayItem = PetSpecialGetWayItem or DeclareMono("PetSpecialGetWayItem", UIWidgetBaseItem)
function PetSpecialGetWayItem:SetData(data)
    self.way_cfg = data
    UH.SetText(self.Name, data.desc)
    UIWidgetBaseItem.SetData(self, data)
end

function PetSpecialGetWayItem:OnClick()
    if self.way_cfg then
        GetWayData.OpenPanel(self.way_cfg,{})
	end
    ViewMgr:CloseView(PetSpecialGetWayView)
    ViewMgr:CloseView(PetStrSelectCopet)
end