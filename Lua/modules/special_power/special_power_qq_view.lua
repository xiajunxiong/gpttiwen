SpecialPowerQQView = SpecialPowerQQView or DeclareView("SpecialPowerQQView", "special_power_qq/special_power_qq_view", Mod.ServerActivity.SpecialPowerQQ)
VIEW_DECLARE_MASK(SpecialPowerQQView,ViewMask.BgBlock)
function SpecialPowerQQView:SpecialPowerQQView()
end
function SpecialPowerQQView:LoadCallback()
    SpecialPowerData.Instance:SetIndex(1)
end
function SpecialPowerQQView:OnClickClose()
    ViewMgr:CloseView(SpecialPowerQQView)
end
function SpecialPowerQQView:CloseCallback()
    SpecialPowerData.Instance:SetIndex(1)
end
-----------------------------------
SpecialPowerQQPanel = SpecialPowerQQPanel or DeclareMono("SpecialPowerQQPanel", UIWFlushPanel)
function SpecialPowerQQPanel:SpecialPowerQQPanel()
    self.data = SpecialPowerData.Instance
    self.data_cares = {
        {data = self.data.sm_data, func = self.FlushView, keys = {"view_index"}}
    }
end

function SpecialPowerQQPanel:OnClickToggle(index)
    self.data:SetIndex(index)
end

function SpecialPowerQQPanel:FlushView()
    if self.data.sm_data.view_index == 3 then
        self.LevelList:SetData(self.data:GetQQSpecialLevelReward())
    else
        self.ShowList:SetData(self.data:GetQQSpecialReward())
        self.BtnInteractor.Interactable = self.data:CanGetQQReward()
    end
end

function SpecialPowerQQPanel:OnClickGet()
    if self.data:CanGetQQReward() then
        SpecialPowerCtrl.Instance:SendQQSpecialReq(self.data.sm_data.view_index)
    else
        PublicPopupCtrl.Instance:Center(Language.SpecialPower["HasGet"..self.data.sm_data.view_index])
    end
end
-----------------------------------------------
SpecialPowerLevelItem = SpecialPowerLevelItem or DeclareMono("SpecialPowerLevelItem", UIWidgetBaseItem)
function SpecialPowerLevelItem:SpecialPowerLevelItem()
    self.data_instance = SpecialPowerData.Instance
end

function SpecialPowerLevelItem:SetData(data)
    self.data = data
    UH.SetText(self.ConditionTxt, Format(Language.SpecialPower.ConditionLevel, data.level))
    self.ItemList:SetData(data.item_list)
    self.BtnObj:SetActive(data.get_state == 0)
    self.HasGetObj:SetActive(data.get_state ~= 0)
    if data.get_state == 0 then
        self.BtnInteractor.Interactable = RoleLevel() >= data.level
    end
end

function SpecialPowerLevelItem:OnClickGet()
    if RoleLevel() < self.data.level then
        PublicPopupCtrl.Instance:Center(ErrorText[153])
    else
        SpecialPowerCtrl.Instance:SendQQSpecialReq(3, self.data.seq)
    end
end