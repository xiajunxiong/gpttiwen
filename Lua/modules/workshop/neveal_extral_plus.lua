NevealExtralPlusView = NevealExtralPlusView or DeclareView("NevealExtralPlusView", "workshop/neveal_extral_plus",Mod.NevealExtraPlus.Main)
function NevealExtralPlusView:NevealExtralPlusView()end 
function NevealExtralPlusView:LoadCallback(param_t)
    if param_t.item ~= nil then 

        -- MedalData.Instance:SetSelectedTargetSpChange(param_t.item,0)
    end   
end

function NevealExtralPlusView:OnClickClose()
    ViewMgr:CloseView(NevealExtralPlusView)
end

NevealExtralPlusPanel = NevealExtralPlusPanel or DeclareMono("NevealExtralPlusPanel", UIWFlushPanel)
function NevealExtralPlusPanel:NevealExtralPlusPanel()
    self.data = WorkshopData.Instance
    self.data_cares = {
        {data = self.data.neveal_extra_plus_info,func = self.ShowNotice,keys = {"is_show"}},
        {data = self.data.neveal_extra_plus_info,func = self.FlushShow,keys = {"main","stuff_1","stuff_2"}},
        {data = BagData.Instance.item_grid, func = self.FlushItemChange, init_call = false},
    }

    self.select_data = nil

end 

function NevealExtralPlusPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:ClearNevealExtraPlus()
end

function NevealExtralPlusPanel:FlushShow()
    local is_max = self.data:GetNevealExtralIsMax()

    self.MainCall:SetData(self.data.neveal_extra_plus_info.main)
    self.MainCall.gameObject:SetActive(not is_max)
    self.StuffCall1.gameObject:SetActive(self.data.neveal_extra_plus_info.stuff_1 ~= nil and not is_max)
    self.StuffCall2.gameObject:SetActive(self.data.neveal_extra_plus_info.stuff_2 ~= nil and not is_max)

    if not is_max then 
        self.StuffCall1:SetData(self.data.neveal_extra_plus_info.stuff_1)
        self.StuffCall2:SetData(self.data.neveal_extra_plus_info.stuff_2)
    end 

    local target = not is_max and self.data:GetNevealExtraMainUpgrade() or self.data.neveal_extra_plus_info.main
    self.TargetCall:SetData(target)
    UH.SetText(self.MainName,target:QuaName(true))

    UH.SetText(self.BtnText,is_max and Language.NevealExtralPlus.Max or Language.NevealExtralPlus.Upgrade)
    self.BtnInter.Interactable = not is_max
end

function NevealExtralPlusPanel:FlushItemChange()
    
    self.data:NevealExtralHandleItemChange()
    self.Block:SetActive(false)
    self:FlushShow()
end

function NevealExtralPlusPanel:EffectComplete()
    WorkshopCtrl.Instance:SendOperateReqWithParamt({
        op_type = WORLSHOP_OP.WORKSHOP_OP_UP_LEVEL_DIANHUA,
        param1 = self.data.neveal_extra_plus_info.main:GridIndex(),
        param2 = self.data.neveal_extra_plus_info.stuff_1:GridIndex(),
        param3 = self.data.neveal_extra_plus_info.stuff_2:GridIndex(),
    }) 
end

function NevealExtralPlusPanel:ShowNotice()
    if not self.data.neveal_extra_plus_info.is_show then 
        return 
    end 
    MainOtherCtrl.Instance:OpenGiftView({[1]=self.data.neveal_extra_plus_info.main},GIFT_OPEN_TYPE.CONGRATULATIONS,true)

    self.data.neveal_extra_plus_info.is_show = false
end

function NevealExtralPlusPanel:SelectFlush(data)
    self.select_data = data
end

function NevealExtralPlusPanel:OnClickSelectSure()
    if self.data.neveal_extra_plus_info.select_type == 0 then 
        self.data:SetMainNevealExtraPlus(self.select_data)
    elseif self.data.neveal_extra_plus_info.select_type == 1 then 
        self.data:SetMainNevealExtraStuff1Plus(self.select_data)
    elseif self.data.neveal_extra_plus_info.select_type == 2 then 
        self.data:SetMainNevealExtraStuff2Plus(self.select_data)
    end 

    self:OnClickSelectClose()
    self.select_data = nil
end

function NevealExtralPlusPanel:OnClickSelect(index)
    if self.data:GetNevealExtralIsMax() then 
        PublicPopupCtrl.Instance:Center(Language.NevealExtralPlus.ErrorMax)
        return 
    end

    self.data.neveal_extra_plus_info.select_type = index

    self.SelectPart:SetActive(true)

    local list = self.data:GetNevealExtraSelectList()

    self.SelectList:SetData(list)
    self.WithoutItem:SetActive(#list == 0)

    self.SelectTitle:SetActive(index > 0)
    self.SelectMainTitle:SetActive(index == 0)
end

function NevealExtralPlusPanel:OnClickSelectClose()
    self.SelectToggle:SetAllTogglesOff()
    self.SelectPart:SetActive(false)
end

function NevealExtralPlusPanel:OnClickUpgrade()
    if self.data:GetNevealExtralIsMax() then 
        PublicPopupCtrl.Instance:Center(Language.NevealExtralPlus.ErrorMax)
        return 
    end

    if self.data.neveal_extra_plus_info.main == nil then 
        PublicPopupCtrl.Instance:Center(Language.NevealExtralPlus.ErrorTipsMain)
        return 
    end 

    if self.data.neveal_extra_plus_info.stuff_1 == nil or self.data.neveal_extra_plus_info.stuff_2 == nil then 
        PublicPopupCtrl.Instance:Center(Language.NevealExtralPlus.ErrorTipsStuff)
        return 
    end 

    -- self.Block:SetActive(true)
    self.UiEffectTool:Play(4165115)
end

function NevealExtralPlusPanel:OnClickOneKey()
    if self.data:GetNevealExtralIsMax() then 
        PublicPopupCtrl.Instance:Center(Language.NevealExtralPlus.ErrorMax)
        return 
    end

    self.data:OneKeyNevealExtraSelect()
end

NevealExtralPlusSelectItem = NevealExtralPlusSelectItem or DeclareMono("NevealExtralPlusSelectItem", UIWidgetBaseItem)
function NevealExtralPlusSelectItem:NevealExtralPlusSelectItem()end
function NevealExtralPlusSelectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    UH.SetText(self.Name,data:QuaName())
end 