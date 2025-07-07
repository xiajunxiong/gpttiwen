RoleChangeHeadView = RoleChangeHeadView or DeclareView("RoleChangeHeadView", "role_setting/role_change_head")

function RoleChangeHeadView:RoleChangeHeadView()
    self.data = RoleSettingData.Instance
end

function RoleChangeHeadView:LoadCallback()
    self:OnClickType(1)
end

function RoleChangeHeadView:OnClickType(type)
    self.List:SetData(self.data:GetChangeHeadData(type))
    self.List:CancelSelect()
    self.select_data = nil
end

function RoleChangeHeadView:OnClickItem(cell,data)
    self.select_data = data
    if data.type == 1 then
        self.data:SetHeadTipsFlag(data.type,data.advancement_level)
    else
        self.data:SetHeadTipsFlag(data.type,data.surface_id)
    end
end

function RoleChangeHeadView:OnClickSave()
    if self.select_data == nil then
        return
    end
    local data = self.select_data or {}
    if data.advancement_level == 1 and data.surface_id == 0 then
        RoleSettingCtrl.Instance:SendChangeHeadReq(0)
    else
        RoleSettingCtrl.Instance:SendChangeHeadReq(data.avatar_id)
    end
    ViewMgr:CloseView(RoleChangeHeadView)
end

function RoleChangeHeadView:OnClickClose()
    ViewMgr:CloseView(RoleChangeHeadView)
end

----------------------------RoleChangeHeadItem----------------------------
RoleChangeHeadItem = RoleChangeHeadItem or DeclareMono("RoleChangeHeadItem", UIWidgetBaseItem)

function RoleChangeHeadItem:SetData(data)
    self.Tips:SetActive(false)
    UH.SetAvatar(self.Head,data.avartar_type,data.avatar_id)
    local role_id = RoleData.Instance:GetRoleId()
    if data.type == 1 then
        self.Interactor.Interactable = RoleData.Instance:ProfStage() >= data.advancement_level
        self.Tips:SetActive(RoleSettingData.Instance:GetHeadTipsFlag(data.type,data.advancement_level)==true)
    else
        local info = IllusionData.Instance:GetFashionInfoById(data.surface_id)
        self.Interactor.Interactable = info and info.level > 0 or false
        self.Tips:SetActive(RoleSettingData.Instance:GetHeadTipsFlag(data.type,data.surface_id)==true)
    end
    self.Block:SetActive(not self.Interactor.Interactable)
    UIWidgetBaseItem.SetData(self, data)
end

function RoleChangeHeadItem:OnClickItem()
    if self.data.type == 1 then
        local prof_id = RoleData.Instance:GetRoleProfType() + self.data.advancement_level
        local prof_name = RolePromoteData.GetProfName(prof_id) or ""
        PublicPopupCtrl.Instance:Error(Format(Language.RoleSetting.ClickHeadTips[self.data.type],prof_name))
    else
        local info = IllusionData.Instance:GetFashionInfoById(self.data.surface_id)
        PublicPopupCtrl.Instance:Error(Format(Language.RoleSetting.ClickHeadTips[self.data.type],info.name))
    end
end

function RoleChangeHeadItem:Click()
	if self.ToggleClick then
        self.ToggleClick:Trigger()
	end
end

function RoleChangeHeadItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
        self.Tips:SetActive(false)
    end
end