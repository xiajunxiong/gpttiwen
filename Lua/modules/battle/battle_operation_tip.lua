BattleOperationTip = BattleOperationTip or DeclareView("BattleOperationTip","battle/battle_operation_tip", Mod.BattleOpTip.Main)
VIEW_DECLARE_MASK(BattleOperationTip, ViewMask.Block)
function BattleOperationTip:BattleOperationTip()
    
end
function BattleOperationTip:LoadCallback()
    UH.SetText(self.Desc1, Language.RoleSetting.BattleDesc1)
    UH.SetText(self.Desc2, Language.RoleSetting.BattleDesc2)
    self.handle_delay = TimeHelper:AddDelayTimer(function ()
        self:OnClickOld()
    end, 10)
end
function BattleOperationTip:OnClickOld()
    RoleSettingData.Instance:SetData(ROLE_SETTING_TYPE.SYSTEM_SET_INFO_BATTLE_CONTROL_MODE, 0)
    ViewMgr:CloseView(BattleOperationTip)
end
function BattleOperationTip:OnClickNew()
    RoleSettingData.Instance:SetData(ROLE_SETTING_TYPE.SYSTEM_SET_INFO_BATTLE_CONTROL_MODE, 1)
    ViewMgr:CloseView(BattleOperationTip)
end 
function BattleOperationTip:CloseCallback()
    TimeHelper:CancelTimer(self.handle_delay)
end