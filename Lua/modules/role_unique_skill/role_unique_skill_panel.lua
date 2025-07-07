RoleUniquaSkillPanel = RoleUniquaSkillPanel or DeclareMono("RoleUniquaSkillPanel", UIWFlushPanel)
function RoleUniquaSkillPanel:RoleUniquaSkillPanel()
    self.data_cares = {
        {data = RoleData.Instance:GetBaseData(), func = self.FlushToggle, init_call = false, keys = {"level"}},
        {data = FlyUpData.Instance.info_data, func = self.FlushToggle, init_call = false},
        {data = RoleXiuWeiData.Instance.remind_data, func = self.FlushToggle, init_call = false},
        {data = RoleAngrySoulData.Instance.nu_hun_info, func = self.FlushToggle, init_call = false},
    }    
end

function RoleUniquaSkillPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushToggle()
    self:SelTab(1)
end

function RoleUniquaSkillPanel:FlushToggle()
    self.ToggleMonos[1]:SetData({})
    self.ToggleMonos[2]:SetData(RoleXiuWeiData.Instance:GetOpenData())
    self.ToggleMonos[3]:SetData(RoleAngrySoulData.Instance:GetOpenData())
end

function RoleUniquaSkillPanel:SelTab(index)
    self.ToggleMonos[index]:SetSelected(true)
end

function RoleUniquaSkillPanel:OnSelClick(data)
    
end

RoleUniquaSkillToggle = RoleUniquaSkillToggle or DeclareMono("RoleUniquaSkillToggle", UIWidgetBaseItem)
-- data : {is_lock, lock_tip, is_shield, red_num}
-- is_lock:是否置灰
-- is_shield:是否屏蔽隐藏
-- lock_tip:提示
-- red_num:红点提醒
function RoleUniquaSkillToggle:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:SetObjActive(data.is_shield ~= true)
    self.Lock:SetActive(data.is_lock == true)
    self.Selector.enabled = not data.is_lock
    self.Interactor.Interactable = not data.is_lock
    self.RedPoint:SetNum(data.red_num or 0)
end

function RoleUniquaSkillToggle:OnLockClick()
    local data = self.data or {}
    if data.lock_tip then
        PublicPopupCtrl.Instance:Center(data.lock_tip)
    end
end
