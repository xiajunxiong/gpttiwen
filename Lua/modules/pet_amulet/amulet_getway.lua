AmuletGetwayView = AmuletGetwayView or DeclareView("AmuletGetwayView", "pet_amulet/pet_amulet_getway")
VIEW_DECLARE_MASK(AmuletGetwayView,ViewMask.BlockClose)
function AmuletGetwayView:AmuletGetwayView()
    
end 

function AmuletGetwayView:LoadCallback()
    UH.SetText(self.Title, Language.Pet.PetEquip.Getway)
    local list_data = Config.pet_equipment_auto.get_way
    self.BtnList:SetData(list_data or {})
end

function AmuletGetwayView:OnCloseClick()
    ViewMgr:CloseView(AmuletGetwayView)
end

AmuletGetwayBtnItem = AmuletGetwayBtnItem or DeclareMono("AmuletGetwayBtnItem", UIWidgetBaseItem)
function AmuletGetwayBtnItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.mod_name)
end

function AmuletGetwayBtnItem:OnBtnClick()
    local mod_key = self.data.mod_key
    if not mod_key then
        return
    end
    if mod_key == Mod.RandActivity.FirstCharge then
        if FirstChargeData.Instance:GetFirstChargeFlag() ~= 0 then
            PublicPopupCtrl.Instance:Center(Language.Pet.PetEquip.Tip2)
            return
        end
    end
    ViewMgr:OpenViewByKey(mod_key)
end