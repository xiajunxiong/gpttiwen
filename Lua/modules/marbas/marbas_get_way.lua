MarbasGetwayView = MarbasGetwayView or DeclareView("MarbasGetwayView", "marbas/marbas_getway")
VIEW_DECLARE_MASK(MarbasGetwayView,ViewMask.BlockClose)
function MarbasGetwayView:MarbasGetwayView()
    
end 

function MarbasGetwayView:LoadCallback(param_t)
    UH.SetText(self.Title, Language.Marbas.Getway)
    local list_data = MarbasData.Instance:GetEquipGetWay()
    self.BtnList:SetData(list_data or {})

    UH.LocalPos(self.Pos, param_t.pos)
end

function MarbasGetwayView:OnCloseClick()
    ViewMgr:CloseView(MarbasGetwayView)
end

MarbasGetwayBtnItem = MarbasGetwayBtnItem or DeclareMono("MarbasGetwayBtnItem", UIWidgetBaseItem)
function MarbasGetwayBtnItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.desc)
end

function MarbasGetwayBtnItem:OnBtnClick()
    local mod_key = self.data.open_panel
    if not mod_key then
        return
    end
    -- if mod_key == Mod.RandActivity.FirstCharge then
    --     if FirstChargeData.Instance:GetFirstChargeFlag() ~= 0 then
    --         PublicPopupCtrl.Instance:Center(Language.Pet.PetEquip.Tip2)
    --         return
    --     end
    -- end
    ViewMgr:OpenViewByKey(mod_key)
end