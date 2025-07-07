AppearanceShowView = AppearanceShowView or DeclareView("AppearanceShowView", "appearance/AppearanceShow")
VIEW_DECLARE_MASK(AppearanceShowView, ViewMask.BgBlockClose)

function AppearanceShowView:LoadCallback(param_t)
    self.click_func = param_t.click_func
    self.param_t = param_t
    UH.SetText(self.ItemName, param_t.item_name)
    UH.SetText(self.Desc, param_t.desc_str or Language.Appearance.ShowResult)
    self.AttrList:SetData(param_t.attr_list)
    if param_t.show_type == AppearanceShowViewType.Illusion then
        self:ShowRoleModel(param_t.show_param)
    elseif param_t.show_type == AppearanceShowViewType.Pet then
        self:ShowOtherModel(param_t.show_param)
    elseif param_t.show_type == AppearanceShowViewType.Partner then
        self:ShowOtherModel(param_t.show_param)
    elseif param_t.show_type == AppearanceShowViewType.Face then
        self:FlushIcon(param_t.show_param)
    end
    -- self.AutoCloseTimer = TimeHelper:AddDelayTimer(BindTool.Bind(self.OnClickClose, self), 2)
end

function AppearanceShowView:CloseCallback()
    -- TimeHelper:CancelTimer(self.AutoCloseTimer)
end

function AppearanceShowView:OnClickClose()
    ViewMgr:CloseView(AppearanceShowView)
end

function AppearanceShowView:OnClickGo()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    if self.click_func ~= nil then
        self.click_func()
    end
    if self.param_t.show_type == AppearanceShowViewType.Face then
        AppearanceData.Instance:ClearRedPoint()
    end
    ViewMgr:CloseView(AppearanceShowView)
end

function AppearanceShowView:ShowRoleModel(show_param)
    self.ModelShowObj1:SetActive(true)
    self.ModelShowObj2:SetActive(false)
    if nil == self.ui_obj_role then
		self.ui_obj_role = UIChDrawer.New()
        self.ModelShow1:SetShowData({ui_obj = self.ui_obj_role, view = self})
    end
    local appearance = show_param.appearance
	local is_ride = show_param.is_ride
    self.ui_obj_role.is_ride = is_ride
    -- appearance.avatar_type = 3
	if is_ride and appearance.ride_id > 0 then
        self.ui_obj_role:SetMainSolo(DrawerHelper.GetRidePath(appearance.ride_id))
    else
		self.ui_obj_role:SetData(appearance, {avatar_type = appearance.avatar_type})
    end
end

function AppearanceShowView:ShowOtherModel(show_param)
    self.ModelShowObj2:SetActive(true)
    self.ModelShowObj1:SetActive(false)
    local res_path = show_param.res_path
    if not self.ui_obj then
        self.ui_obj = UIObjDrawer.New()
        self.ui_obj:SetData(res_path)
        self.ModelShow2:SetShowData({ui_obj = self.ui_obj, view = self})
    else
        self.ui_obj:SetData(res_path)
    end
end

function AppearanceShowView:FlushIcon(show_param)
    self.IconList[1]:SetActive(true)
    local res_id, res_type, title_id = show_param.res_id, show_param.res_type, show_param.title_id
    local index = res_type + 1
    local pre = AppearanceConfig.IconPre[res_type]
    local icon_type = AppearanceConfig.IconType[res_type]

    if res_type == AppearanceConfig.Type.Bubble then
        self.Icon[index]:SetData(res_id, Vector2.New(174,65))
    else
        if res_id ~= 0 then
            UH.SetIcon(self.Icon[index], pre .. res_id, icon_type)
        else
            UH.SetText(self.TitleName, TitleData.GetQuaName(title_id))
            UH.SetIcon(self.Icon[index], nil)
        end
        self.Title:SetActive(res_id == 0)
    end
    for i = 1, self.IconList:Length() do
        self.IconList[i]:SetActive(i == index)
    end
end

AppearanceShowItem = AppearanceShowItem or DeclareMono("AppearanceShowItem", UIWidgetBaseItem)
function AppearanceShowItem:SetData(data)
    UH.SetText(self.Name, data.name)
    UH.SetText(self.Value, data.value)
end
